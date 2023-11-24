#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <string>
#include <mutex>
#include <vector>
#include <csignal>


class TCPServer {
public:
    TCPServer(int port);
    ~TCPServer();

    void start();
    void stop();

private:

    static void handleSignal(int signal);
    static TCPServer* serverInstance;
    static volatile sig_atomic_t terminationSignalReceived;

    int port;
    int serverSocket;

    static uint id;
    static std::mutex idMutex;

    std::vector<int> connectedClients;

    void createSocket();
    void bindSocket();
    void listenForClients();
    void acceptClients();
    void handleClient(int clientSocket);
    void shutdownClients();
};

#endif // TCP_SERVER_HPP