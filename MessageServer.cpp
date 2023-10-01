//
// Created by Callum Birks on 30/09/2023.
//

#include "MessageServer.h"

namespace ByteFrost::internal {
MessageServer::MessageServer(uint16_t port_number, const MessageServer::MessageCallback& callback)

            : _server_addr {
#ifndef _WIN32
                            0,
#endif
                            AF_INET,
                            htons(port_number),
                            { INADDR_ANY },
                            { 0 } }

            , _client_addr { 0 }
            , _buffer {}
            , _sock1 {}
            , _sock2 {}
            , _status { Status::STOPPED }
            , _messageCallback(std::make_shared<MessageCallback>(callback)) {
}

MessageServer::~MessageServer() { stop(); }

bool MessageServer::start() {
  _sock1 = socket(AF_INET, SOCK_STREAM, 0);
  if (SOCK_ERR(_sock1)) {
    std::cerr << "Error opening socket" << std::endl;
    return false;
  }
  if (bind(_sock1, (sockaddr *)&_server_addr, sizeof(_server_addr)) < 0) {
    std::cerr << "Error binding socket" << std::endl;
    return false;
  }
  listen(_sock1, 1);
  socklen_t client_len = sizeof(_client_addr);
  _sock2 = accept(_sock1, (sockaddr *)&_client_addr, &client_len);
  if (SOCK_ERR(_sock2)) {
    std::cerr << "Error accepting socket connection" << std::endl;
    return false;
  }

  _listening = true;
  _listenerThread = std::thread(&MessageServer::listenForMessages, this);

  _status = Status::STARTED;

  return true;
}

void MessageServer::stop() {
  _status = Status::STOPPED;

  _listening = false;
  if (_listenerThread.joinable()) {
    _listenerThread.join();
  }

  SOCK_CLOSE(_sock2);
  SOCK_CLOSE(_sock1);
}

bool MessageServer::sendMessage(const std::string &message) const {
  ssize_t n = SOCK_WRITE(_sock2, message.data(), message.size());

  if (n < 0) {
    std::cerr << "Error sending message" << std::endl;
  } else {
    std::cerr << "Message sent successfully" << std::endl;
  }

  return n >= 0;
}

void MessageServer::listenForMessages() {
  while (_listening) {
    ssize_t n = SOCK_READ(_sock2, _buffer, 255);

    if (n > 0) {
      (*_messageCallback)(std::string(_buffer, n));
    }
  }
}

}  // namespace ByteFrost::internal