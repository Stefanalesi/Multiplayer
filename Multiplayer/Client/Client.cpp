#include "Client.h"
#include <iostream>

Client::Client(const std::string& serverUrl)
    : serverUrl(serverUrl), httpClient(serverUrl), webSocketClient(serverUrl) {}

void Client::connect() {
    std::cout << "Connecting to server..." << std::endl;
    webSocketClient.connect();
}

void Client::sendHttpRequest(const std::string& endpoint) {
    httpClient.get(endpoint);
}

void Client::sendWebSocketMessage(const std::string& message) {
    webSocketClient.send(message);
}
