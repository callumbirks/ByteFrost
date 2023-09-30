//
// Created by Callum Birks on 30/09/2023.
//

#include "Server.h"

namespace ByteFrost {
    Server::Server(uint16_t port_number, const Server::MessageCallback& callback)
            : _server_addr {0,
                            AF_INET,
                            htons(port_number),
                            { INADDR_ANY },
                            { 0 } }
            , _client_addr {0 }
            , _buffer {}
            , _sockfd {}
            , _newsockfd {}
            , _messageCallback(std::make_shared<MessageCallback>(callback)) {

        ssize_t n = read(_newsockfd, _buffer, 255);

        if (n < 0) {
            perror("Error reading from socket.");
            exit(1);
        }

        printf("Message from client: %s\n", _buffer);
    }

    Server::~Server() {
        stop();
    }

    bool Server::start() {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd < 0) {
            std::cerr << "Error opening socket" << std::endl;
            return false;
        }
        if (bind(_sockfd, (sockaddr *) &_server_addr, sizeof(_server_addr)) < 0) {
            std::cerr << "Error binding socket" << std::endl;
            return false;
        }
        listen(_sockfd, 1);
        socklen_t client_len = sizeof(_client_addr);
        _newsockfd = accept(_sockfd, (sockaddr *) &_client_addr, &client_len);
        if (_newsockfd < 0) {
            std::cerr << "Error accepting socket connection" << std::endl;
            return false;
        }

        _listening = true;
        _listenerThread = std::thread(&Server::listenForMessages, this);

        return true;
    }

    void Server::stop() {
        _listening = false;
        _listenerThread.join();
        close(_newsockfd);
        close(_sockfd);
    }

    bool Server::sendMessage(const std::string &message) const {
        ssize_t n = write(_newsockfd, message.data(), message.size());

        if( n < 0 ) {
            std::cerr << "Error sending message" << std::endl;
        } else {
            std::cerr << "Message sent successfully" << std::endl;
        }

        return n >= 0;
    }

    void Server::listenForMessages() {
        while(_listening) {
            ssize_t n = read(_newsockfd, _buffer, 255);

            if (n < 0) {
                // Log error
                return;
            }

            (*_messageCallback)(std::string(_buffer));
        }
    }

} // ByteFrost