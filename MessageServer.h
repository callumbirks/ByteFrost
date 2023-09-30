//
// Created by Callum Birks on 30/09/2023.
//

#ifndef BYTEFROST_MESSAGESERVER_H
#define BYTEFROST_MESSAGESERVER_H

#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#ifdef _WIN32
#include <Ws2tcpip.h>
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#include <unistd.h>

#include "sock_utility.h"

namespace ByteFrost::internal {

class MessageServer {
 public:
  typedef std::function<void(std::string)> MessageCallback;

  MessageServer(uint16_t port_number, const MessageCallback &callback);

  ~MessageServer();

  bool start();

  void stop();

  [[nodiscard]] bool sendMessage(const std::string &message) const;

  enum class Status {
    STOPPED,
    STARTED,
    CONNECTED,
  };

 private:
  sockaddr_in _server_addr, _client_addr;
  SOCK_T _sock1, _sock2;
  char _buffer[256];
  Status _status;

  std::shared_ptr<MessageCallback> _messageCallback;
  std::thread _listenerThread;
  std::atomic<bool> _listening = false;

  void listenForMessages();
};

}  // namespace ByteFrost::internal

#endif  // BYTEFROST_MESSAGESERVER_H
