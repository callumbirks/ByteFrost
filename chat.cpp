//
// Created by Callum Birks on 30/09/2023.
//

#include "library.h"

void receiveMessage(std::string message) {
  std::cerr << "Received message '" << message << "'" << std::endl;
}

using namespace ByteFrost::internal;

int main() {
  DiscoveryServer ds{"Steve's PC"};
  return 0;
}
