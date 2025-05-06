#include "LobbyProtocol.h"
#include <sstream>
#include <json/json.h>

namespace LobbyProtocol {
    std::string serializeMessage(const Message& msg) {
        Json::Value root;
        root["type"] = static_cast<int>(msg.type);
        root["data"] = msg.data;
        
        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, root);
    }

    Message deserializeMessage(const std::string& data) {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::stringstream ss(data);
        std::string errors;
        
        if (!Json::parseFromStream(builder, ss, &root, &errors)) {
            throw std::runtime_error("Failed to parse message: " + errors);
        }
        
        Message msg;
        msg.type = static_cast<MessageType>(root["type"].asInt());
        msg.data = root["data"].asString();
        return msg;
    }
} 