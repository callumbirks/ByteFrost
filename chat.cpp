//
// Created by Callum Birks on 30/09/2023.
//

#include "library.h"

void receiveMessage(std::string message) {
    std::cerr << "Received message '" << message << "'" << std::endl;
}

int main() {
    ByteFrost::Server server { 4269, &receiveMessage };
    server.start();

    return 0;
}
