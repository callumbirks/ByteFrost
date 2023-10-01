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

#include <map>

#include "sock_utility.h"

namespace ByteFrost::internal {

class MessageServer {
 public:
  typedef std::function<void(std::string)> MessageCallback;

  MessageServer(uint16_t port_number, const MessageCallback &callback);

  ~MessageServer();

  bool start();

  void stop();

  void addPeer(const std::string &username, const std::string &ipAddress);

  void removePeer(const std::string &username);

  [[nodiscard]] bool sendMessage(const std::string &peerUsername, const std::string &message) const;

 private:
  sockaddr_in _server_addr, _client_addr;
  SOCK_T _sock1, _sock2;
  char _buffer[256];

  std::shared_ptr<MessageCallback> _messageCallback;
  std::thread _listenerThread;
  std::atomic<bool> _listening = false;
  std::map<std::string, pollfd> _connectedPeers;
  // We can hold up to 32 connections at once

  pollfd _peersArray[32];

  void listenForMessages();
  void updatePeersArray();
};

}  // namespace ByteFrost::internal

#endif  // BYTEFROST_MESSAGESERVER_H
