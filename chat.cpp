//
// Created by Callum Birks on 30/09/2023.
//

#include "library.h"

int main() {
  ByteFrost::Client client{"Steve's PC"};

  auto& availablePeers = client.availablePeers();

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    availablePeers = client.availablePeers();
    if (availablePeers.empty()) {
      std::cout << "Searching for peers..." << std::endl;
    } else {
      std::cout << "Available peers:\n";
    }
    for (const auto& peer : availablePeers) {
      std::cout << peer.first << " - " << peer.second.ipAddress << "\n";
    }
    if (!availablePeers.empty()) {
      std::string username{};
      do {
        username = "";
        std::cout << std::endl;
        std::cout << "Enter username to send message: ";
        std::getline(std::cin, username);
      } while (username.empty() || availablePeers.count(username) == 0);

      std::string message{};
      std::cout << "Enter message: ";

      std::getline(std::cin, message);

      client.sendMessage(username, message);
    }
  }
}
