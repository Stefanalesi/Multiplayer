#pragma once

#include "WebSocketClient.h"
#include "Lobby.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

class LobbyManager {
public:
    LobbyManager(WebSocketClient& client);
    
    // Lobby operations
    bool createLobby(const std::string& playerId, const std::string& username);
    bool joinLobby(const std::string& lobbyId, const std::string& playerId, const std::string& username);
    bool leaveLobby(const std::string& playerId);
    bool setReady(const std::string& playerId, bool ready);
    bool startGame(const std::string& playerId);
    
    // Getters
    const Lobby* getCurrentLobby() const { return currentLobby.get(); }
    const std::vector<std::string>& getAvailableLobbies() const { return availableLobbies; }
    
    // Callbacks
    void setOnLobbyUpdateCallback(std::function<void()> callback) { onLobbyUpdate = callback; }
    void setOnGameStartCallback(std::function<void()> callback) { onGameStart = callback; }
    
    // Message handling
    void handleServerMessage(const std::string& message);
    
private:
    WebSocketClient& client;
    std::unique_ptr<Lobby> currentLobby;
    std::vector<std::string> availableLobbies;
    std::string playerId;
    
    // Callbacks
    std::function<void()> onLobbyUpdate;
    std::function<void()> onGameStart;
    
    // Message handling helpers
    void handleLobbyList(const std::string& message);
    void handleLobbyUpdate(const std::string& message);
    void handleGameStart(const std::string& message);
}; 