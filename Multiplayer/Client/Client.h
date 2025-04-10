#ifndef CLIENT_H
#define CLIENT_H

#include "HttpClient.h"
#include "WebSocketClient.h"
#include <string>

class Client {
public:
    Client(const std::string& serverUrl);
    void connect();
    void sendHttpRequest(const std::string& endpoint);
    void sendWebSocketMessage(const std::string& message);

private:
    std::string serverUrl;
    HttpClient httpClient;
    WebSocketClient webSocketClient;
};

#endif // CLIENT_H
