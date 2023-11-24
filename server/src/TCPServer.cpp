#include "TCPServer.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

#define MAX_CLIENTS 6
#define SAVE_DATA_PATH "data/id_save.data"

uint TCPServer::id = 0;
std::mutex TCPServer::idMutex;

volatile sig_atomic_t TCPServer::terminationSignalReceived = 0;
TCPServer *TCPServer::serverInstance = nullptr;

void TCPServer::handleSignal(int signal) {
  (void)signal;

  terminationSignalReceived = 1;
  if(serverInstance)
  {
    serverInstance->stop();
  }
  exit(EXIT_SUCCESS);
}

TCPServer::TCPServer(int port)
    : port(port), serverSocket(-1), connectedClients() {}

TCPServer::~TCPServer() {}

void TCPServer::start() {

  serverInstance = this;

  // Set up the signal handler for Ctrl-C
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = handleSignal;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, nullptr);

  std::ifstream file(SAVE_DATA_PATH);
  if (file.is_open()) {
    file >> TCPServer::id;
    file.close();
  } else {
    std::cerr << "Unable to open the file for reading." << std::endl;
  }

  createSocket();
  bindSocket();
  listenForClients();
  acceptClients();
}

void TCPServer::shutdownClients() {

  const std::string shutdownMessage = "\nThank you! Server is shutting down.";

  // Lock the mutex to avoid race conditions on id variable
  std::lock_guard<std::mutex> lock(idMutex);

  for (int clientSocket : connectedClients) {
    int bytesSent = send(clientSocket, shutdownMessage.c_str(), shutdownMessage.length(), 0);

    if (bytesSent == -1) {
      std::cerr << "Error sending shutdown message to a client\n";
    }
    close(clientSocket);
  }

  connectedClients.clear();
}

void TCPServer::stop() {

  shutdownClients();

  if (serverSocket != -1) {
    close(serverSocket);
  }

  // Reset the signal handler
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = SIG_DFL;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, nullptr);

  std::ofstream file(SAVE_DATA_PATH);
  if (file.is_open()) {
    file << TCPServer::id;
    file.close();
  } else {
    std::cerr << "Unable to open the file for writing." << std::endl;
  }
}

void TCPServer::acceptClients() {
  std::vector<std::thread> clientThreads;

  while (!terminationSignalReceived) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Loop until accept is successful or interrupted by a signal
    while (!terminationSignalReceived) {
      int clientSocket =
          accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);

      if (clientSocket == -1) {
        if (errno == EINTR) {
          // The accept call was interrupted by a signal, continue the loop
          continue;
        } else {
          // Handle other errors
          std::cerr << "Error accepting client connection\n";
          break;
        }
      }

      std::cout << "Accepted connection from: "
                << inet_ntoa(clientAddr.sin_addr) << ":"
                << ntohs(clientAddr.sin_port) << std::endl;

      // Create a new thread for each client
      clientThreads.emplace_back(&TCPServer::handleClient, this, clientSocket);
    }
  }

  // Join all client threads (this won't be reached in this example since the
  // server runs indefinitely)
  for (auto &thread : clientThreads) {
    thread.join();
  }
}

void TCPServer::createSocket() {
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    std::cerr << "Error creating server socket\n";
    std::exit(EXIT_FAILURE);
  }
}

void TCPServer::bindSocket() {
  struct sockaddr_in serverAddr;
  std::memset(&serverAddr, 0, sizeof(serverAddr));

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) ==
      -1) {
    std::cerr << "Error binding server socket\n";
    close(serverSocket);
    std::exit(EXIT_FAILURE);
  }
}

void TCPServer::listenForClients() {
  if (listen(serverSocket, MAX_CLIENTS) == -1) {
    std::cerr << "Error listening for clients\n";
    close(serverSocket);
    std::exit(EXIT_FAILURE);
  }

  std::cout << "Server listening on port " << port << std::endl;
}

void TCPServer::handleClient(int clientSocket) {

  connectedClients.push_back(clientSocket);

  while (!terminationSignalReceived) {
    std::string s;

    {
      std::lock_guard<std::mutex> lock(idMutex);
      s = std::to_string(TCPServer::id++);
    }

    try {
      // Attempt to send data to the client
      int bytesSent = send(clientSocket, s.c_str(), s.length(), 0);

      if (bytesSent == -1) {
        if (errno == EINTR) {
          // The accept call was interrupted by a signal, continue the loop
          continue;
        } else {
        // Handle send error
        std::cerr << "Error sending data to the client\n";
        break; // Break out of the loop on send error
        }
      } else if (bytesSent == 0) {
        // Client disconnected
        std::cout << "Client disconnected\n";
        break; // Break out of the loop on client disconnect
      }

    } catch (const std::exception &e) {
      // Handle other exceptions
      std::cerr << "Exception occurred: " << e.what() << std::endl;
      break; // Break out of the loop on exception
    }

    // Additional check to detect client disconnection
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(clientSocket, &readSet);

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int selectResult =
        select(clientSocket + 1, &readSet, nullptr, nullptr, &timeout);

    if (selectResult == -1) {
      // Handle select error
      std::cerr << "Error in select function\n";
      break; // Break out of the loop on select error
    } else if (selectResult == 0) {
      // No data to read, but the connection is still active
      // Continue processing or sleep
      sleep(1);
    } else {
      // Data available, indicating a potential client disconnection
      char buffer[1];
      int bytesRead = recv(clientSocket, buffer, sizeof(buffer), MSG_PEEK);

      if (bytesRead == 0) {
        // Client disconnected
        std::cout << "Client disconnected\n";
        
        // Close the client socket after handling disconnection
        close(clientSocket);
        break;
      }
    }
  }

  auto it = std::find(connectedClients.begin(), connectedClients.end(), clientSocket);
  if (it != connectedClients.end()) {
    connectedClients.erase(it);
  }

}