#pragma once

#include "LobbyProtocol.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <functional>

class LobbyManager {
public:
    LobbyManager();
    
    // Lobby operations
    std::string createLobby(const std::string& hostId, const std::string& username);
    bool joinLobby(const std::string& lobbyId, const std::string& playerId, const std::string& username);
    bool leaveLobby(const std::string& lobbyId, const std::string& playerId);
    bool setReady(const std::string& lobbyId, const std::string& playerId, bool ready);
    bool startGame(const std::string& lobbyId, const std::string& playerId);
    
    // Message handling
    std::string handleMessage(const std::string& message);
    
    // Getters
    const std::vector<std::string>& getLobbyList() const;
    std::string getLobbyState(const std::string& lobbyId) const;
    
private:
    std::unordered_map<std::string, std::unique_ptr<LobbyProtocol::Lobby>> lobbies;
    std::vector<std::string> lobbyList;
    
    // Helper functions
    std::string generateLobbyId() const;
    void updateLobbyList();
    std::string serializeLobbyState(const LobbyProtocol::Lobby& lobby) const;
};