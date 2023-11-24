#include "TCPServer.hpp"

int main() {

  chdir("~/PROJECT/IDGenerator");

  const int port = 12346;
  TCPServer server(port);

  // Start the server and accept incoming connections
  server.start();

  // Stop the server (cleanup)
  server.stop();

  return 0;
}
