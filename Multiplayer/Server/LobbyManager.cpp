#include "LobbyManager.h"

int LobbyManager::createLobby() {
    int lobbyId = nextLobbyId++;
    lobbies[lobbyId] = {};
    return lobbyId;
}

bool LobbyManager::joinLobby(int lobbyId, const std::string& player) {
    auto it = lobbies.find(lobbyId);
    if (it == lobbies.end()) return false;
    it->second.push_back(player);
    return true;
}

std::vector<std::string> LobbyManager::getLobbyPlayers(int lobbyId) const {
    auto it = lobbies.find(lobbyId);
    if (it != lobbies.end()) return it->second;
    return {};
}
