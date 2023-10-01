//
// Created by Callum Birks on 30/09/2023.
//

#include "library.h"

int main() {
  ByteFrost::Client client{"Steve's PC"};

  std::this_thread::sleep_for(std::chrono::seconds(5));

  auto availablePeers = client.availablePeers();

  for (const auto& peer : availablePeers) {
    client.sendMessage(peer.first, "Hi, this is Jeff's Mac");
  }

  return 0;
}
