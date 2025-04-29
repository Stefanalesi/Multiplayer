#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace LobbyProtocol {
    struct Player {
        std::string id;
        std::string username;
        bool isReady;
    };

    struct Lobby {
        std::string id;
        std::string hostId;
        std::vector<Player> players;
        bool gameStarted;
    };

    // Message types
    enum class MessageType {
        CREATE_LOBBY,
        JOIN_LOBBY,
        LEAVE_LOBBY,
        SET_READY,
        START_GAME,
        LOBBY_LIST,
        LOBBY_UPDATE,
        GAME_START
    };

    // Message structure
    struct Message {
        MessageType type;
        std::string data;
    };

    // Helper functions for message serialization/deserialization
    std::string serializeMessage(const Message& msg);
    Message deserializeMessage(const std::string& data);
} 