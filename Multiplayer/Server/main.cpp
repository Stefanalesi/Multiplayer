#include "WebSocketServer.h"
#include "LobbyManager.h"
#include <iostream>
#include <memory>

int main() {
    try {
        // Create lobby manager
        LobbyManager lobbyManager;
        
        // Create WebSocket server
        WebSocketServer server(8080);
        
        // Set up message handler
        server.setOnMessageCallback([&lobbyManager](const std::string& message) {
            return lobbyManager.handleMessage(message);
        });
        
        std::cout << "Server started on port 8080" << std::endl;
        
        // Run server
        server.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
