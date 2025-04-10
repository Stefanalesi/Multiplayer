#include "WebSocketClient.h"
#include <iostream>
#include <uwebsockets/App.h>

WebSocketClient::WebSocketClient(const std::string& serverUrl)
    : serverUrl(serverUrl) {}

void WebSocketClient::connect() {
    std::cout << "Connecting to WebSocket server at " << serverUrl << std::endl;

    // Create a WebSocket connection
    uWS::App().ws<SocketData>("/*", {
        .open = [this](auto* ws) {
            this->ws = ws;
            std::cout << "Connected to server!" << std::endl;
        },
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            std::cout << "Received message from server: " << message << std::endl;
        },
        .close = [](auto* ws, int code, std::string_view message) {
            std::cout << "Disconnected from server!" << std::endl;
        }
    }).connect(serverUrl, [](auto* token) {
        if (token) {
            std::cout << "WebSocket connection established!" << std::endl;
        } else {
            std::cerr << "Failed to connect to WebSocket server!" << std::endl;
        }
    }).run();
}

void WebSocketClient::send(const std::string& message) {
    if (ws) {
        std::cout << "Sending message: " << message << std::endl;
        ws->send(message);
    } else {
        std::cerr << "Error: Not connected to WebSocket server." << std::endl;
    }
}

void WebSocketClient::receive() {
    // This is already handled in the `message` callback, where we process incoming messages.
    // You could implement additional logic to trigger receiving manually, if needed.
}
