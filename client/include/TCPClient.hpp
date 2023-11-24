#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include <string>

class TCPClient {
public:
    TCPClient(const std::string& serverIP, int serverPort);
    ~TCPClient();

    bool connectToServer();
    void startReceiving();
    void closeConnection();

private:
    int clientSocket;
    std::string serverIP;
    int serverPort;

    static constexpr int bufferSize = 1024;

    void receiveLoop();
};

#endif // TCP_CLIENT_HPP