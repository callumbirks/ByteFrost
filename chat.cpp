//
// Created by Callum Birks on 30/09/2023.
//

#include "library.h"

void receiveMessage(std::string message) {
  std::cerr << "Received message '" << message << "'" << std::endl;
}

using namespace ByteFrost::internal;

int main() {
  WSADATA wsaData;
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    std::cerr << "WSAStartup failed: " << iResult << std::endl;
    return 1;
  }

  DiscoveryServer ds{"Steve's PC"};
  ds.start();
  ds.wait();

  WSACleanup();

  return 0;
}
