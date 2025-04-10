#include "Server.h"
#include <iostream>

void Server::start() {
    HttpServer httpServer;
    WebSocketServer wsServer;

    std::cout << "Starting server..." << std::endl;
    wsServer.run();
    httpServer.run();
}
