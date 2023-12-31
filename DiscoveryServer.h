//
// Created by callu on 30/09/2023.
//

#ifndef BYTEFROST_DISCOVERYSERVER_H
#define BYTEFROST_DISCOVERYSERVER_H

#ifdef _WIN32
#include <Ws2tcpip.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <atomic>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

#include "sock_utility.h"

namespace ByteFrost::internal {
class DiscoveryServer {
 public:
  typedef std::function<void(const std::string&, const std::string&)> DiscoveryCallback;

  DiscoveryServer(std::string username, const DiscoveryCallback& discoveryCallback);
  ~DiscoveryServer();

  void start();

  void stop();

 private:
  std::thread _workThread;
  sockaddr_in _outAddr;
  sockaddr_in _inAddr;
  SOCK_T _outSock;
  SOCK_T _inSock;
  std::string _broadcastMessage;
  char _inBuf[256];
  std::atomic<bool> _running;
  std::shared_ptr<DiscoveryCallback> _callback;

  void discoveryJob();

  static constexpr const char* const kMulticastAddress = "239.255.73.5";
  static constexpr uint16_t kMulticastPort = 26372;
};
}  // namespace ByteFrost::internal

#endif  // BYTEFROST_DISCOVERYSERVER_H
