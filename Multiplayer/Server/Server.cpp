#include "Server.h"
void Server::start() {
    WebSocketServer wsServer;
    HttpServer httpServer;
    wsServer.run();
    httpServer.run();
}