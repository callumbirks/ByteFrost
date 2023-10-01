//
// Created by callu on 01/10/2023.
//

#ifndef BYTEFROST_PEER_H
#define BYTEFROST_PEER_H

#include <chrono>
#include <string>
struct Peer {
  std::string username;
  std::string ipAddress;
  std::chrono::time_point<std::chrono::steady_clock> lastSeen;
};

#endif  // BYTEFROST_PEER_H
