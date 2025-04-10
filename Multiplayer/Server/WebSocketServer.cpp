#include "WebSocketServer.h"
#include <iostream>
#include <uwebsockets/App.h>

void WebSocketServer::run() {
    std::cout << "Running WebSocket server..." << std::endl;

    // uWebSockets setup
    uWS::App().ws<SocketData>("/*", {
        .open = [](auto* ws) {
            std::cout << "A client has connected!" << std::endl;
            // You can store user-specific data in ws->getUserData() if needed.
        },
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            std::cout << "Received message: " << message << std::endl;
            // Echo the message back to the client
            ws->send(message, opCode);
        },
        .close = [](auto* ws, int code, std::string_view message) {
            std::cout << "A client has disconnected!" << std::endl;
        }
    }).listen(9001, [](auto* token) {
        if (token) {
            std::cout << "WebSocket server is listening on port 9001" << std::endl;
        } else {
            std::cerr << "Failed to listen on port 9001" << std::endl;
        }
    }).run();
}
