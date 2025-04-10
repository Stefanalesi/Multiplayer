#ifndef LOBBYMANAGER_H
#define LOBBYMANAGER_H

#include <unordered_map>
#include <vector>
#include <string>

class LobbyManager {
public:
    int createLobby();
    bool joinLobby(int lobbyId, const std::string& player);
    std::vector<std::string> getLobbyPlayers(int lobbyId) const;

private:
    int nextLobbyId = 1;
    std::unordered_map<int, std::vector<std::string>> lobbies;
};

#endif