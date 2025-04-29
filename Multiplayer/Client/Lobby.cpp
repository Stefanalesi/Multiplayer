#include "Lobby.h"

Lobby::Lobby(const std::string& lobbyId, const std::string& hostId)
    : lobbyId(lobbyId), hostId(hostId), gameStarted(false) {
    // Add host as first player
    Player host;
    host.id = hostId;
    host.username = "Host";
    host.isReady = false;
    players.push_back(host);
}

bool Lobby::addPlayer(const Player& player) {
    if (isFull() || gameStarted) {
        return false;
    }
    
    // Check if player already exists
    for (const auto& p : players) {
        if (p.id == player.id) {
            return false;
        }
    }
    
    players.push_back(player);
    return true;
}

bool Lobby::removePlayer(const std::string& playerId) {
    if (gameStarted) {
        return false;
    }
    
    auto it = std::find_if(players.begin(), players.end(),
        [&playerId](const Player& p) { return p.id == playerId; });
    
    if (it != players.end()) {
        players.erase(it);
        return true;
    }
    
    return false;
}

bool Lobby::setPlayerReady(const std::string& playerId, bool ready) {
    if (gameStarted) {
        return false;
    }
    
    auto it = std::find_if(players.begin(), players.end(),
        [&playerId](const Player& p) { return p.id == playerId; });
    
    if (it != players.end()) {
        it->isReady = ready;
        return true;
    }
    
    return false;
}

bool Lobby::startGame() {
    if (gameStarted || players.size() < 2) {
        return false;
    }
    
    // Check if all players are ready
    for (const auto& player : players) {
        if (!player.isReady) {
            return false;
        }
    }
    
    gameStarted = true;
    return true;
} 