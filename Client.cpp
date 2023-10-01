//
// Created by callu on 30/09/2023.
//

#include "Client.h"

void messageReceived(std::string message) {}

ByteFrost::Client::Client(std::string username)
    : _username(std::move(username)),
      _discoveryServer(_username,
                       [this](auto&& username_, auto&& ipAddress_) {
                         discoveredPeer(std::forward<decltype(username_)>(username_),
                                        std::forward<decltype(ipAddress_)>(ipAddress_));
                       }),
      _messageServer(&messageReceived) {
#ifdef _WIN32
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    std::cerr << "WSAStartup failed: " << iResult << std::endl;
    exit(1);
  }
#endif

  _discoveryServer.start();
  _messageServer.start();
}

bool ByteFrost::Client::sendMessage(const std::string& username, const std::string& message) {
  return _messageServer.sendMessage(username, message);
}

void ByteFrost::Client::discoveredPeer(const std::string& username, const std::string& ipAddress) {
  // Ignore discovering self
  if (username == _username) return;

  auto lastSeen = std::chrono::steady_clock::now();

  auto found_it = _availablePeers.find(username);
  if (found_it == _availablePeers.end()) {
    Peer peer{username, ipAddress, lastSeen};
    _availablePeers.emplace(username, peer);
    _messageServer.addPeer(username, ipAddress);
  } else {
    Peer& peer = found_it->second;
    peer.lastSeen = lastSeen;
    if (peer.ipAddress != ipAddress) {
      peer.ipAddress = ipAddress;
    }
  }
}
