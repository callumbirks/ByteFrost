//
// Created by callu on 30/09/2023.
//

#include "DiscoveryServer.h"

#include <Ws2tcpip.h>

#include <iostream>

namespace ByteFrost::internal {
DiscoveryServer::DiscoveryServer(std::string username)
    : _inBuf{},
      _broadcastMessage{std::move(username)},
      _inSock{},
      _outSock{},
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
  _outSock = socket(AF_INET, SOCK_DGRAM, 0);
  inet_pton(AF_INET, kMulticastAddress, &_outAddr.sin_addr);

  _inSock = socket(AF_INET, SOCK_DGRAM, 0);
  ip_mreq mreq{};
  inet_pton(AF_INET, kMulticastAddress, &mreq.imr_multiaddr);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  setsockopt(_inSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq,
             sizeof(mreq));
  _inAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(_inSock, (sockaddr*)&_inAddr, sizeof(_inAddr)) < 0) {
    // Log some error
  }
}

void DiscoveryServer::start() {
  _workThread = std::thread(&DiscoveryServer::discoveryJob, this);
}

void DiscoveryServer::discoveryJob() {
  std::cerr << "Broadcasting " << _broadcastMessage << std::endl;

  sendto(_outSock, _broadcastMessage.data(), _broadcastMessage.size(), 0,
         (sockaddr*)&_outAddr, sizeof(_outAddr));
  int n = SOCK_READ(_inSock, _inBuf, sizeof(_inBuf));

  if (n > 0) {
    std::cerr << "I discovered someone!: '" << std::string(_inBuf, n) << "'"
              << std::endl;
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));
}

}  // namespace ByteFrost::internal