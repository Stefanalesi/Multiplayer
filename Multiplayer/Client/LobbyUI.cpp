#include "LobbyUI.h"
#include <iostream>

LobbyUI::LobbyUI(std::shared_ptr<LobbyManager> lobbyManager)
    : lobbyManager(lobbyManager) {
    initializeUI();
}

LobbyUI::~LobbyUI() {}

void LobbyUI::initializeUI() {
    // Load font
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }

    // Set colors
    backgroundColor = sf::Color(40, 40, 40);
    buttonColor = sf::Color(70, 70, 70);
    buttonHoverColor = sf::Color(90, 90, 90);
    textColor = sf::Color::White;

    // Initialize title
    titleText.setFont(font);
    titleText.setString("Game Lobby");
    titleText.setCharacterSize(48);
    titleText.setFillColor(textColor);
    titleText.setPosition(400, 50);

    // Initialize player list
    playerListText.setFont(font);
    playerListText.setCharacterSize(24);
    playerListText.setFillColor(textColor);
    playerListText.setPosition(50, 150);

    // Initialize buttons
    readyButton.setSize(sf::Vector2f(200, 50));
    readyButton.setPosition(300, 400);
    readyButton.setFillColor(buttonColor);

    startGameButton.setSize(sf::Vector2f(200, 50));
    startGameButton.setPosition(300, 500);
    startGameButton.setFillColor(buttonColor);

    readyButtonText.setFont(font);
    readyButtonText.setString("Ready");
    readyButtonText.setCharacterSize(24);
    readyButtonText.setFillColor(textColor);
    readyButtonText.setPosition(350, 410);

    startGameButtonText.setFont(font);
    startGameButtonText.setString("Start Game");
    startGameButtonText.setCharacterSize(24);
    startGameButtonText.setFillColor(textColor);
    startGameButtonText.setPosition(320, 510);
}

void LobbyUI::update() {
    updatePlayerList();
}

void LobbyUI::updatePlayerList() {
    const Lobby* lobby = lobbyManager->getCurrentLobby();
    if (!lobby) return;

    std::string playerList = "Players in Lobby:\n\n";
    for (const auto& player : lobby->players) {
        playerList += player.username + " - " + (player.isReady ? "Ready" : "Not Ready") + "\n";
    }

    playerListText.setString(playerList);
}

void LobbyUI::render(sf::RenderWindow& window) {
    window.clear(backgroundColor);
    
    window.draw(titleText);
    window.draw(playerListText);
    
    drawButton(window, readyButton, readyButtonText);
    drawButton(window, startGameButton, startGameButtonText);
    
    window.display();
}

void LobbyUI::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        
        if (isMouseOverButton(readyButton, mousePos)) {
            // Toggle ready status
            const Lobby* lobby = lobbyManager->getCurrentLobby();
            if (lobby) {
                lobbyManager->setReady(lobby->players[0].id, !lobby->players[0].isReady);
            }
        }
        else if (isMouseOverButton(startGameButton, mousePos)) {
            // Start game
            const Lobby* lobby = lobbyManager->getCurrentLobby();
            if (lobby && lobby->hostId == lobby->players[0].id) {
                lobbyManager->startGame(lobby->players[0].id);
            }
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        
        readyButton.setFillColor(isMouseOverButton(readyButton, mousePos) ? buttonHoverColor : buttonColor);
        startGameButton.setFillColor(isMouseOverButton(startGameButton, mousePos) ? buttonHoverColor : buttonColor);
    }
}

void LobbyUI::drawButton(sf::RenderWindow& window, const sf::RectangleShape& button, const sf::Text& text) {
    window.draw(button);
    window.draw(text);
}

bool LobbyUI::isMouseOverButton(const sf::RectangleShape& button, const sf::Vector2f& mousePos) {
    return button.getGlobalBounds().contains(mousePos);
} 