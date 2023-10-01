//
// Created by callu on 30/09/2023.
//

#include "DiscoveryServer.h"

namespace ByteFrost::internal {
DiscoveryServer::DiscoveryServer(std::string username, const DiscoveryCallback& discoveryCallback)
    : _inBuf{},
      _broadcastMessage{std::move(username)},
      _inSock{},
      _outSock{},
      _callback(std::make_shared<DiscoveryCallback>(discoveryCallback)),
      _inAddr{
#ifndef _WIN32
          0,
#endif
          AF_INET, htons(kMulticastPort)},
      _outAddr{
#ifndef _WIN32
          0,
#endif
          AF_INET, htons(kMulticastPort)} {

#ifdef _WIN32
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    std::cerr << "WSAStartup failed: " << iResult << std::endl;
    exit(1);
  }
#endif

  _outSock = socket(AF_INET, SOCK_DGRAM, 0);
  inet_pton(AF_INET, kMulticastAddress, &_outAddr.sin_addr);

  _inSock = socket(AF_INET, SOCK_DGRAM, 0);
  ip_mreq mreq{};
  inet_pton(AF_INET, kMulticastAddress, &mreq.imr_multiaddr);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  setsockopt(_inSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
  _inAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(_inSock, (sockaddr*)&_inAddr, sizeof(_inAddr)) < 0) {
    // Log some error
  }
}

DiscoveryServer::~DiscoveryServer() { stop(); }

void DiscoveryServer::start() {
  _workThread = std::thread(&DiscoveryServer::discoveryJob, this);
  _running = true;
}

void DiscoveryServer::stop() {
  _running = false;
  if (_workThread.joinable()) {
    _workThread.join();
  }

  SOCK_CLOSE(_inSock);
  SOCK_CLOSE(_outSock);
}

void DiscoveryServer::discoveryJob() {
  while (_running) {
    std::cerr << "Broadcasting " << _broadcastMessage << std::endl;

    int n =
        sendto(_outSock, _broadcastMessage.data(), _broadcastMessage.size(), 0, (sockaddr*)&_outAddr, sizeof(_outAddr));

    if (n < 0) {
#ifdef _WIN32
      int error = WSAGetLastError();
#else
      int error = n;
#endif
      std::cerr << "Error sending multicast: " << error << std::endl;
    }

    sockaddr_in senderAddr{};
    socklen_t addrLen = sizeof(senderAddr);

    n = recvfrom(_inSock, _inBuf, sizeof(_inBuf), 0, (sockaddr*)&senderAddr, &addrLen);

    if (n < 0) {
#ifdef _WIN32
      int error = WSAGetLastError();
#else
      int error = n;
#endif
      std::cerr << "Error reading multicast socket: " << error << std::endl;
    } else if (n > 0) {
      char ipAddressBuf[16]{};
      // Parse binary ip addr to string
      inet_ntop(AF_INET, &senderAddr.sin_addr, ipAddressBuf, sizeof(ipAddressBuf));

      auto ipAddress = std::string(ipAddressBuf);
      auto username = std::string(_inBuf, n);

      (*_callback)(username, ipAddress);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

}  // namespace ByteFrost::internal