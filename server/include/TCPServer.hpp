#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <csignal>
#include <mutex>
#include <string>
#include <vector>

class TCPServer {
public:
  TCPServer(int port);
  ~TCPServer();

  void start();
  void stop();

private:
  static volatile sig_atomic_t terminationSignalReceived;
  static void handleSignal(int signal);
  static TCPServer *serverInstance;

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