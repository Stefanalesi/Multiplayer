#pragma once

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();

    bool connect(const std::string& url);
    bool send(const std::string& message);
    std::string receive();
    void close();

private:
    SOCKET clientSocket;
    bool isConnected;
    std::string serverUrl;
    int port;

    bool parseUrl(const std::string& url);
    std::string generateWebSocketKey();
    std::string base64Encode(const std::vector<unsigned char>& data);
    bool performHandshake(SOCKET socket);
};
