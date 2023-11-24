#include "TCPServer.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

#define MAX_CLIENTS 6

uint TCPServer::id = 0;
std::mutex TCPServer::idMutex;
volatile sig_atomic_t TCPServer::terminationSignalReceived = 0;
TCPServer* TCPServer::serverInstance = nullptr;

void TCPServer::handleSignal(int signal) {
    if (serverInstance) {
        serverInstance->stop();
    }
}

TCPServer::TCPServer(int port) : port(port), serverSocket(-1), connectedClients(){}
TCPServer::~TCPServer() {}

void TCPServer::start() {
    
    // Set the static instance pointer to this instance
    serverInstance = this;

    // Set up the signal handler for Ctrl-C
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handleSignal;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);


    createSocket();
    bindSocket();
    listenForClients();
    acceptClients();
}

void TCPServer::shutdownClients()
{
    const std::string shutdownMessage = "Thank you! Server is shutting down.";

    // Lock the mutex to avoid race conditions on id variable
    std::lock_guard<std::mutex> lock(idMutex);

    for (int clientSocket : connectedClients) {
        send(clientSocket, shutdownMessage.c_str(), shutdownMessage.length(), 0);
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
}

void TCPServer::acceptClients() {
    std::vector<std::thread> clientThreads;

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Error accepting client connection\n";
            continue;
        }

        std::cout << "Accepted connection from: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

        // Create a new thread for each client
        clientThreads.emplace_back(&TCPServer::handleClient, this, clientSocket);
    }

    // Join all client threads (this won't be reached in this example since the server runs indefinitely)
    for (auto& thread : clientThreads) {
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

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
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

    while (true) {
        
        std::string s;
        
        {
            std::lock_guard<std::mutex> lock(idMutex);
            s = std::to_string(TCPServer::id++);
            if(send(clientSocket, s.c_str(), s.length(), 0) == -1)
            {
                break;
            }
        }

        sleep(1);
    }
}

