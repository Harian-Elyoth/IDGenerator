#include "TCPServer.hpp"

int main()
{
    const int port = 12346;
    TCPServer server(port);

    // Start the server and accept incoming connections
    server.start();

    // Stop the server (cleanup)
    server.stop();

    return 0;
}
