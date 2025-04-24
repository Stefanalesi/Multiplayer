#include "WebSocketServer.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    WebSocketServer server;
    
    // Start the server
    server.run(9001);
    
    // Keep the server running
    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();
    
    // Stop the server
    server.stop();
    
    return 0;
}
