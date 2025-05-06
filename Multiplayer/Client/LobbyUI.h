#pragma once

#include <SFML/Graphics.hpp>
#include "LobbyManager.h"
#include <memory>
#include <functional>

class LobbyUI {
public:
    LobbyUI(std::shared_ptr<LobbyManager> lobbyManager);
    ~LobbyUI();

    void update();
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);

private:
    std::shared_ptr<LobbyManager> lobbyManager;
    
    // UI Elements
    sf::Font font;
    sf::Text titleText;
    sf::Text playerListText;
    sf::Text readyButtonText;
    sf::Text startGameButtonText;
    
    // Buttons
    sf::RectangleShape readyButton;
    sf::RectangleShape startGameButton;
    
    // Colors
    sf::Color backgroundColor;
    sf::Color buttonColor;
    sf::Color buttonHoverColor;
    sf::Color textColor;
    
    // Helper functions
    void initializeUI();
    void updatePlayerList();
    void drawButton(sf::RenderWindow& window, const sf::RectangleShape& button, const sf::Text& text);
    bool isMouseOverButton(const sf::RectangleShape& button, const sf::Vector2f& mousePos);
}; 