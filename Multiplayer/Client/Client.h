#ifndef CLIENT_H
#define CLIENT_H
#include <string>

#include "WebSocketClient.h"
#include "HttpClient.h"

class Client {
public:
    Client();
    void connectToServer(const std::string& url);
    void joinLobby(int lobbyId);
private:
    WebSocketClient wsClient;
    HttpClient httpClient;
};
#endif