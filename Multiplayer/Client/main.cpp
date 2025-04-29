#include <SFML/Graphics.hpp>
#include <memory>
#include "WebSocketClient.h"
#include "LobbyManager.h"
#include "LobbyUI.h"

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Game Lobby");
    
    // Initialize WebSocket client
    WebSocketClient client;
    if (!client.connect("ws://localhost:8080")) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }
    
    // Initialize lobby manager and UI
    auto lobbyManager = std::make_shared<LobbyManager>(client);
    LobbyUI lobbyUI(lobbyManager);
    
    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            lobbyUI.handleEvent(event);
        }
        
        // Update lobby state
        lobbyUI.update();
        
        // Render
        lobbyUI.render(window);
    }
    
    return 0;
}
