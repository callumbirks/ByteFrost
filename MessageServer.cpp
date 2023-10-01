//
// Created by Callum Birks on 30/09/2023.
//

#include "MessageServer.h"

namespace ByteFrost::internal {
MessageServer::MessageServer(uint16_t port_number, const MessageServer::MessageCallback& callback)

            : _server_addr {
#ifndef _WIN32
                            0,
#endif
                            AF_INET,
                            htons(port_number),
                            { INADDR_ANY },
                            { 0 } }

            , _buffer {}
            , _listeningSock {}
            , _connectedPeers {}
            , _peersArray {}
            , _messageCallback(std::make_shared<MessageCallback>(callback)) {
  WSA_STARTUP();
  _myIP = getMyIP();
}

MessageServer::~MessageServer() {
  stop();
  WSA_CLEANUP();
}

bool MessageServer::start() {
  _listeningSock = socket(AF_INET, SOCK_STREAM, 0);
  if (SOCK_ERR(_listeningSock)) {
    std::cerr << "Error opening socket" << std::endl;
    return false;
  }
  if (bind(_listeningSock, (sockaddr *)&_server_addr, sizeof(_server_addr)) < 0) {
    std::cerr << "Error binding socket" << std::endl;
    return false;
  }
  listen(_listeningSock, 1);

  _listening = true;
  _listenerThread = std::thread(&MessageServer::listenForMessages, this);

  return true;
}

void MessageServer::stop() {
  _listening = false;
  if (_listenerThread.joinable()) {
    _listenerThread.join();
  }

  for (auto it = _connectedPeers.begin(); it != _connectedPeers.end(); it++) {
    SOCK_CLOSE(it->second.fd);
  }

  SOCK_CLOSE(_listeningSock);
}

std::string MessageServer::getMyIP() {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    std::cerr << "Error creating socket" << std::endl;
    return "";
  }

  sockaddr_in remoteAddr{};
  remoteAddr.sin_family = AF_INET;
  remoteAddr.sin_port = htons(80);
  inet_pton(AF_INET, "8.8.8.8", &remoteAddr.sin_addr);

  if (connect(sock, (sockaddr *)&remoteAddr, sizeof(remoteAddr)) < 0) {
    std::cerr << "Failed to connect socket" << std::endl;
    SOCK_CLOSE(sock);
    return "";
  }

  sockaddr_in localAddr{};
  socklen_t addrLen = sizeof(localAddr);
  if (getsockname(sock, (sockaddr *)&localAddr, &addrLen) < 0) {
    std::cerr << "getsockname failed" << std::endl;
    SOCK_CLOSE(sock);
    return "";
  }

  char addressBuf[16]{};

  inet_ntop(AF_INET, &localAddr.sin_addr, addressBuf, sizeof(addressBuf));

  std::cerr << "My IP is " << addressBuf << std::endl;

  SOCK_CLOSE(sock);

  return {addressBuf};
}

bool MessageServer::sendMessage(const std::string &peerUsername, const std::string &message) const {
  auto found_it = _connectedPeers.find(peerUsername);

  if (found_it == _connectedPeers.end()) {
    return false;
  }

  const pollfd &peerfd = found_it->second;

  ssize_t n = SOCK_WRITE(peerfd.fd, message.data(), message.size());

  if (n < 0) {
    std::cerr << "Error sending message" << std::endl;
  } else {
    std::cerr << "Message sent successfully" << std::endl;
  }

  return n >= 0;
}

void MessageServer::listenForMessages() {
  while (_listening) {
    if (_connectedPeers.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }
    // Timeout of 1000 so thread is not blocked
    int numSockEvents = SOCK_POLL(_peersArray, _connectedPeers.size(), 1000);

    if (numSockEvents == 0) continue;

    if (numSockEvents == -1) {
      // Get error from WSALastErr and errno
#ifdef _WIN32
      int error = WSAGetLastError();
#else
      // Need to fix this to use `errno`
      int error = numSockEvents
#endif
      std::cerr << "Error while polling peer sockets: " << error << std::endl;
      continue;
    }

    std::vector<std::string> droppedPeers{};

    // Figure out which sockets have messages, read them and invoke callbacks.
    int eventsProcessed = 0;
    for (auto &peer : _connectedPeers) {
      pollfd &peerfd = peer.second;
      if (peerfd.revents == 0) {  // No event
        continue;
      } else if (peerfd.revents & POLLIN) {  // Socket has data to read
        int n = SOCK_READ(peerfd.fd, _buffer, 255);
        (*_messageCallback)(std::string(_buffer, n));
        eventsProcessed++;
      } else if (peerfd.revents & POLLHUP) {  // Peer closed connection
        droppedPeers.emplace_back(peer.first);
        eventsProcessed++;
      } else {  // Socket doesn't have data to read, must be error because we are only polling for `POLLIN`
        std::cerr << "An error occurred polling a connected peer socket." << std::endl;
        eventsProcessed++;
      }
    }

    for (std::string &username : droppedPeers) {
      removePeer(username);
    }
  }
}

void MessageServer::addPeer(const std::string &username, const std::string &ipAddress) {
  if (ipAddress == _myIP) return;

  SOCK_T peerSock = _myIP > ipAddress ? initiateConnection(ipAddress) : acceptConnection();

  if (SOCK_ERR(peerSock)) {
    return;
  }

  pollfd peer{peerSock, POLLIN, 0};
  _connectedPeers.emplace(username, peer);
}

void MessageServer::removePeer(const std::string &username) {
  auto it = _connectedPeers.find(username);

  if (it == _connectedPeers.end()) return;

  SOCK_CLOSE(it->second.fd);
  _connectedPeers.erase(username);
  updatePeersArray();
}

void MessageServer::updatePeersArray() {
  memset(_peersArray, 0, sizeof(_peersArray));

  int i = 0;
  for (auto it = _connectedPeers.begin(); i < 32 && it != _connectedPeers.end(); it++, i++) {
    _peersArray[i] = it->second;
  }
}

SOCK_T MessageServer::initiateConnection(const std::string &peerIP) {
  sockaddr_in peerAddr{};
  peerAddr.sin_family = AF_INET;
  peerAddr.sin_port = htons(kMessagePort);
  inet_pton(AF_INET, peerIP.data(), &peerAddr.sin_addr);

  SOCK_T peerSock = socket(AF_INET, SOCK_STREAM, 0);

  if (SOCK_ERR(peerSock)) {
    std::cerr << "Error requesting socket for peer." << std::endl;
    return peerSock;
  }

  if (connect(peerSock, (sockaddr *)&peerAddr, sizeof(peerAddr)) < 0) {
    std::cerr << "Error requesting connection to peer." << std::endl;
    return INVALID_SOCK;
  }

  return peerSock;
}

SOCK_T MessageServer::acceptConnection() {
  sockaddr_in peerAddr{};
  socklen_t addrLen = sizeof(peerAddr);
  SOCK_T peerSock = accept(_listeningSock, (sockaddr *)&peerAddr, &addrLen);

  if (SOCK_ERR(peerSock)) {
    std::cerr << "Error accepting connection from peer." << std::endl;
    return peerSock;
  }

  return peerSock;
}

}  // namespace ByteFrost::internal