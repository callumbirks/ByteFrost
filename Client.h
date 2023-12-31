//
// Created by callu on 30/09/2023.
//

#ifndef BYTEFROST_CLIENT_H
#define BYTEFROST_CLIENT_H

#include <unordered_map>

#include "DiscoveryServer.h"
#include "MessageServer.h"
#include "Peer.h"

namespace ByteFrost {
class Client {
  friend internal::DiscoveryServer;

 public:
  Client(std::string username);

  bool sendMessage(const std::string& username, const std::string& message);

  std::unordered_map<std::string, Peer>& availablePeers() { return _availablePeers; }

 protected:
  void discoveredPeer(const std::string& username, const std::string& ipAddress);

 private:
  std::string _username;
  internal::MessageServer _messageServer;
  internal::DiscoveryServer _discoveryServer;
  std::unordered_map<std::string, Peer> _availablePeers;
};
}  // namespace ByteFrost

#endif  // BYTEFROST_CLIENT_H
