#include "Client.h"
#include <iostream>

Client::Client(const std::string& serverUrl)
    : serverUrl(serverUrl), webSocketClient(serverUrl) {}

void Client::connect() {
    std::cout << "Connecting to server..." << std::endl;
    webSocketClient.connect();
}

void Client::sendHttpRequest(const std::string& endpoint) {
    std::string fullUrl = "http://" + serverUrl + endpoint;
    httpClient.get(fullUrl);
}

void Client::sendWebSocketMessage(const std::string& message) {
    webSocketClient.sendMessage(message);
}
