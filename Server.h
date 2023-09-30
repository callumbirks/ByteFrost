//
// Created by Callum Birks on 30/09/2023.
//

#ifndef BYTEFROST_SERVER_H
#define BYTEFROST_SERVER_H

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace ByteFrost {

    class Server {
    public:
        typedef std::function<void(std::string)> MessageCallback;

        Server(uint16_t port_number, const MessageCallback& callback);

        ~Server();

        bool start();

        void stop();

        bool sendMessage(const std::string& message) const;

        enum class Status {
            STOPPED,
            STARTED,
            CONNECTED,
        };

    private:
        sockaddr_in _server_addr, _client_addr;
        int32_t _sockfd, _newsockfd; // File descriptors, for storing socket system call return values
        char _buffer[256];
        Status _status;

        std::shared_ptr<MessageCallback> _messageCallback;
        std::thread _listenerThread;
        std::atomic<bool> _listening = false;

        void listenForMessages();
    };

} // ByteFrost

#endif //BYTEFROST_SERVER_H
