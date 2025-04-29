#pragma once

#include <string>
#include <vector>
#include <memory>

struct Player {
    std::string id;
    std::string username;
    bool isReady;
};

class Lobby {
public:
    Lobby(const std::string& lobbyId, const std::string& hostId);
    
    const std::string& getId() const { return lobbyId; }
    const std::string& getHostId() const { return hostId; }
    const std::vector<Player>& getPlayers() const { return players; }
    bool isGameStarted() const { return gameStarted; }
    
    bool addPlayer(const Player& player);
    bool removePlayer(const std::string& playerId);
    bool setPlayerReady(const std::string& playerId, bool ready);
    bool startGame();
    bool isFull() const { return players.size() >= maxPlayers; }
    
private:
    std::string lobbyId;
    std::string hostId;
    std::vector<Player> players;
    bool gameStarted;
    static const int maxPlayers = 4; // Maximum number of players per lobby
}; 