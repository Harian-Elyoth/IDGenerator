#include "TCPClient.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

TCPClient::TCPClient(const std::string &serverIP, int serverPort)
    : clientSocket(-1), serverIP(serverIP), serverPort(serverPort) {}

TCPClient::~TCPClient() { closeConnection(); }

bool TCPClient::connectToServer() {
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    std::cerr << "Error creating client socket\n";
    return false;
  }

  struct sockaddr_in serverAddr;
  std::memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(serverPort);
  inet_pton(AF_INET, serverIP.c_str(), &(serverAddr.sin_addr));

  if (connect(clientSocket, (struct sockaddr *)&serverAddr,
              sizeof(serverAddr)) == -1) {
    std::cerr << "Error connecting to the server\n";
    closeConnection();
    return false;
  }

  return true;
}

void TCPClient::startReceiving() {
  if (connectToServer()) {
    receiveLoop();
  } else {
    std::cerr << "Failed to connect to the server.\n";
  }
}

void TCPClient::receiveLoop() {
  char buffer[bufferSize];

  while (true) {
    int bytesRead = recv(clientSocket, buffer, bufferSize - 1, 0);
    if (bytesRead <= 0) {
      // Connection closed or error
      break;
    }

    buffer[bytesRead] = '\0';
    std::cout << "Received from server: " << buffer << std::endl;
  }

  closeConnection();
}

void TCPClient::closeConnection() {
  if (clientSocket != -1) {
    close(clientSocket);
    clientSocket = -1;
  }
}