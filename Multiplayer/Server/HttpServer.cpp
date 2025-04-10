#include "HttpServer.h"
#include <iostream>

void HttpServer::run() {
    crow::SimpleApp app;
    CROW_ROUTE(app, "/lobby").methods("POST"_method)([]() { return "Lobby created!"; });
    app.port(8080).run();
}
