#include "LobbyManager.h"
#include <random>
#include <sstream>
#include <algorithm>

LobbyManager::LobbyManager() {}

std::string LobbyManager::createLobby(const std::string& hostId, const std::string& username) {
    std::string lobbyId = generateLobbyId();
    
    auto lobby = std::make_unique<LobbyProtocol::Lobby>();
    lobby->id = lobbyId;
    lobby->hostId = hostId;
    lobby->gameStarted = false;
    
    LobbyProtocol::Player host;
    host.id = hostId;
    host.username = username;
    host.isReady = false;
    lobby->players.push_back(host);
    
    lobbies[lobbyId] = std::move(lobby);
    updateLobbyList();
    
    return lobbyId;
}

bool LobbyManager::joinLobby(const std::string& lobbyId, const std::string& playerId, const std::string& username) {
    auto it = lobbies.find(lobbyId);
    if (it == lobbies.end() || it->second->gameStarted) {
        return false;
    }
    
    LobbyProtocol::Player player;
    player.id = playerId;
    player.username = username;
    player.isReady = false;
    
    it->second->players.push_back(player);
    return true;
}

bool LobbyManager::leaveLobby(const std::string& lobbyId, const std::string& playerId) {
    auto it = lobbies.find(lobbyId);
    if (it == lobbies.end()) {
        return false;
    }
    
    auto& players = it->second->players;
    players.erase(
        std::remove_if(players.begin(), players.end(),
            [&playerId](const LobbyProtocol::Player& p) { return p.id == playerId; }),
        players.end()
    );
    
    if (players.empty()) {
        lobbies.erase(it);
        updateLobbyList();
    }
    
    return true;
}

bool LobbyManager::setReady(const std::string& lobbyId, const std::string& playerId, bool ready) {
    auto it = lobbies.find(lobbyId);
    if (it == lobbies.end() || it->second->gameStarted) {
        return false;
    }
    
    for (auto& player : it->second->players) {
        if (player.id == playerId) {
            player.isReady = ready;
            return true;
        }
    }
    
    return false;
}

bool LobbyManager::startGame(const std::string& lobbyId, const std::string& playerId) {
    auto it = lobbies.find(lobbyId);
    if (it == lobbies.end() || it->second->gameStarted) {
        return false;
    }
    
    // Check if player is host and all players are ready
    if (it->second->hostId != playerId) {
        return false;
    }
    
    for (const auto& player : it->second->players) {
        if (!player.isReady) {
            return false;
        }
    }
    
    it->second->gameStarted = true;
    return true;
}

std::string LobbyManager::handleMessage(const std::string& message) {
    try {
        auto msg = LobbyProtocol::deserializeMessage(message);
        LobbyProtocol::Message response;
        
        switch (msg.type) {
            case LobbyProtocol::MessageType::CREATE_LOBBY: {
                std::string lobbyId = createLobby(msg.data, msg.data);
                response.type = LobbyProtocol::MessageType::LOBBY_UPDATE;
                response.data = serializeLobbyState(*lobbies[lobbyId]);
                break;
            }
            case LobbyProtocol::MessageType::JOIN_LOBBY: {
                size_t pos = msg.data.find('|');
                if (pos != std::string::npos) {
                    std::string lobbyId = msg.data.substr(0, pos);
                    std::string username = msg.data.substr(pos + 1);
                    if (joinLobby(lobbyId, username, username)) {
                        response.type = LobbyProtocol::MessageType::LOBBY_UPDATE;
                        response.data = serializeLobbyState(*lobbies[lobbyId]);
                    }
                }
                break;
            }
            case LobbyProtocol::MessageType::LEAVE_LOBBY: {
                for (const auto& [lobbyId, lobby] : lobbies) {
                    if (leaveLobby(lobbyId, msg.data)) {
                        response.type = LobbyProtocol::MessageType::LOBBY_UPDATE;
                        response.data = serializeLobbyState(*lobby);
                        break;
                    }
                }
                break;
            }
            case LobbyProtocol::MessageType::SET_READY: {
                size_t pos = msg.data.find('|');
                if (pos != std::string::npos) {
                    std::string playerId = msg.data.substr(0, pos);
                    bool ready = msg.data.substr(pos + 1) == "1";
                    for (const auto& [lobbyId, lobby] : lobbies) {
                        if (setReady(lobbyId, playerId, ready)) {
                            response.type = LobbyProtocol::MessageType::LOBBY_UPDATE;
                            response.data = serializeLobbyState(*lobby);
                            break;
                        }
                    }
                }
                break;
            }
            case LobbyProtocol::MessageType::START_GAME: {
                for (const auto& [lobbyId, lobby] : lobbies) {
                    if (startGame(lobbyId, msg.data)) {
                        response.type = LobbyProtocol::MessageType::GAME_START;
                        response.data = lobbyId;
                        break;
                    }
                }
                break;
            }
            default:
                break;
        }
        
        return LobbyProtocol::serializeMessage(response);
    }
    catch (const std::exception& e) {
        std::cerr << "Error handling message: " << e.what() << std::endl;
        return "";
    }
}

const std::vector<std::string>& LobbyManager::getLobbyList() const {
    return lobbyList;
}

std::string LobbyManager::getLobbyState(const std::string& lobbyId) const {
    auto it = lobbies.find(lobbyId);
    if (it != lobbies.end()) {
        return serializeLobbyState(*it->second);
    }
    return "";
}

std::string LobbyManager::generateLobbyId() const {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    
    std::string id;
    for (int i = 0; i < 6; ++i) {
        id += alphanum[dis(gen)];
    }
    
    return id;
}

void LobbyManager::updateLobbyList() {
    lobbyList.clear();
    for (const auto& [id, _] : lobbies) {
        lobbyList.push_back(id);
    }
}

std::string LobbyManager::serializeLobbyState(const LobbyProtocol::Lobby& lobby) const {
    std::stringstream ss;
    ss << lobby.id << "|" << lobby.hostId;
    
    for (const auto& player : lobby.players) {
        ss << "|" << player.id << ":" << player.username << ":" << (player.isReady ? "1" : "0");
    }
    
    return ss.str();
}
