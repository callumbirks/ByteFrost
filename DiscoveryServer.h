//
// Created by callu on 30/09/2023.
//

#ifndef BYTEFROST_DISCOVERYSERVER_H
#define BYTEFROST_DISCOVERYSERVER_H

#include <winsock2.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <thread>

#include "sock_utility.h"

namespace ByteFrost::internal {
class DiscoveryServer {
 public:
  DiscoveryServer(std::string username);

  void start();

 private:
  std::thread _workThread;
  sockaddr_in _outAddr;
  sockaddr_in _inAddr;
  SOCK_T _outSock;
  SOCK_T _inSock;
  std::string _broadcastMessage;
  char _inBuf[256];

  void discoveryJob();

  static constexpr const char* const kMulticastAddress = "239.255.73.5";
  static constexpr uint16_t kMulticastPort = 6372;
};
}  // namespace ByteFrost::internal

#endif  // BYTEFROST_DISCOVERYSERVER_H