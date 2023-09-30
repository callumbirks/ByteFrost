//
// Created by callu on 30/09/2023.
//

#ifndef BYTEFROST_CLIENT_H
#define BYTEFROST_CLIENT_H

#include "MessageServer.h"

namespace ByteFrost {
class Client {
 public:
  Client(std::string username, uint16_t port);

 private:
  std::string _username;
  internal::MessageServer _messageServer;
};
}  // namespace ByteFrost

#endif  // BYTEFROST_CLIENT_H
