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
#include <BaseTsd.h>
#include <Ws2tcpip.h>
#include <winsock2.h>
typedef SSIZE_T ssize_t;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "sock_utility.h"

namespace ByteFrost::internal {

class MessageServer {
 public:
  typedef std::function<void(std::string)> MessageCallback;

  MessageServer(const MessageCallback &callback);

  ~MessageServer();

  bool start();

  void stop();

  void addPeer(const std::string &username, const std::string &ipAddress);

  void removePeer(const std::string &username);

  [[nodiscard]] bool sendMessage(const std::string &peerUsername, const std::string &message) const;

 private:
  sockaddr_in _server_addr;
  SOCK_T _listeningSock;
  char _buffer[256];
  std::string _myIP;
  std::shared_ptr<MessageCallback> _messageCallback;
  std::thread _listenerThread;
  std::atomic<bool> _listening = false;
  std::map<std::string, pollfd> _connectedPeers;
  std::unordered_map<SOCK_T, std::string> _socketToPeernameMap;
  // We can hold up to 32 connections at once
  pollfd _peersArray[32];
  std::mutex _peersMutex;

  static constexpr uint16_t kMessagePort = 39635;

  static std::string getMyIP();
  void listenForMessages();
  void updatePeersArray();
  SOCK_T initiateConnection(const std::string &peerIP);
  SOCK_T acceptConnection();
};

}  // namespace ByteFrost::internal

#endif  // BYTEFROST_MESSAGESERVER_H
