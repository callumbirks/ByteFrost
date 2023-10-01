//
// Created by Callum Birks on 30/09/2023.
//

#include "library.h"

int main() {
  ByteFrost::Client client{"Steve's PC"};

  auto& availablePeers = client.availablePeers();

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    availablePeers = client.availablePeers();
    std::cout << "Available peers:\n";
    for (const auto& peer : availablePeers) {
      std::cout << peer.first << " - " << peer.second.ipAddress << "\n";
    }
    if (!availablePeers.empty()) {
      std::string username{};
      do {
        std::cout << std::endl;
        std::cout << "Enter username to send message: ";
        std::cin >> username;
      } while (username.empty() || availablePeers.count(username) == 0);

      std::string message{};
      std::cout << "Enter message: ";
      std::cin >> message;

      client.sendMessage(username, message);
    }
  }
}
