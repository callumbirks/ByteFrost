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

            , _client_addr { 0 }
            , _buffer {}
            , _sock1 {}
            , _sock2 {}
            , _connectedPeers {}
 , _peersArray {}
            , _messageCallback(std::make_shared<MessageCallback>(callback)) {
}

MessageServer::~MessageServer() { stop(); }

bool MessageServer::start() {
  _sock1 = socket(AF_INET, SOCK_STREAM, 0);
  if (SOCK_ERR(_sock1)) {
    std::cerr << "Error opening socket" << std::endl;
    return false;
  }
  if (bind(_sock1, (sockaddr *)&_server_addr, sizeof(_server_addr)) < 0) {
    std::cerr << "Error binding socket" << std::endl;
    return false;
  }
  listen(_sock1, 1);

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

  SOCK_CLOSE(_sock2);
  SOCK_CLOSE(_sock1);
}

bool MessageServer::sendMessage(const std::string &peerUsername, const std::string &message) const {
  ssize_t n = SOCK_WRITE(_sock2, message.data(), message.size());

  if (n < 0) {
    std::cerr << "Error sending message" << std::endl;
  } else {
    std::cerr << "Message sent successfully" << std::endl;
  }

  return n >= 0;
}

void MessageServer::listenForMessages() {
  while (_listening) {
    ssize_t n = SOCK_POLL(_peersArray, _connectedPeers.size(), -1);

    if (n == -1) {
      // Get error from WSALastErr and errno
    }

    // Figure out which sockets have messages, read them and invoke callbacks.

    ssize_t n = SOCK_READ(_sock2, _buffer, 255);

    if (n > 0) {
      (*_messageCallback)(std::string(_buffer, n));
    }
  }
}

void MessageServer::addPeer(const std::string &username, const std::string &ipAddress) {
  SOCK_T peerSock = socket(AF_INET, SOCK_STREAM, 0);

  if (SOCK_ERR(peerSock)) {
    // error
    return;
  }

  // connect
  // Here we want to send out a connection request to our new peer.
  // The other peer will likely be doing the same, so need to figure
  // out how to solve conflicting requests?

  // This code isn't right, we want to send a connection rq, not accept one
  //  socklen_t client_len = sizeof(_client_addr);
  //  _sock2 = accept(_sock1, (sockaddr *)&_client_addr, &client_len);
  //  if (SOCK_ERR(_sock2)) {
  //    std::cerr << "Error accepting socket connection" << std::endl;
  //    return;
  //  }

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
  int i = 0;
  for (auto it = _connectedPeers.begin(); i < 32 && it != _connectedPeers.end(); it++, i++) {
    _peersArray[i] = it->second;
  }
}

}  // namespace ByteFrost::internal