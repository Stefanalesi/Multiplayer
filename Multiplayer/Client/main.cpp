#include "WebSocketClient.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    WebSocketClient client;
    
    // Connect to the server
    if (!client.connect("ws://localhost:9001")) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }
    
    // Send a test message
    if (!client.send("Hello, server!")) {
        std::cerr << "Failed to send message" << std::endl;
        return 1;
    }
    
    // Receive response
    std::string response = client.receive();
    if (!response.empty()) {
        std::cout << "Received: " << response << std::endl;
    }
    
    // Keep the client running to receive messages
    std::cout << "Press Enter to disconnect..." << std::endl;
    std::cin.get();
    
    client.close();
    return 0;
}
