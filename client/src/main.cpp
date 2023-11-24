#include "TCPClient.hpp"
#include <iostream>

int main() {
    const std::string serverIP = "127.0.0.1"; // Replace with your server's IP address
    const int serverPort = 12346;             // Replace with your server's port

    TCPClient client(serverIP, serverPort);

    client.startReceiving();

    return 0;
}