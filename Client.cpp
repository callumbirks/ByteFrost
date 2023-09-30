//
// Created by callu on 30/09/2023.
//

#include "Client.h"

void messageReceived(std::string message) {
  std::cerr << "I received a message!: " << message << std::endl;
}

ByteFrost::Client::Client(std::string username, uint16_t port)
    : _username(std::move(username)), _messageServer(port, &messageReceived) {
  _messageServer.start();
}
