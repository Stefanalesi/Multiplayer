#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <string>
#include <uwebsockets/App.h>

class WebSocketClient {
public:
    explicit WebSocketClient(const std::string& serverUrl);

    void connect();
    void send(const std::string& message);
    void receive();

private:
    std::string serverUrl;
    uWS::WebSocket<>* ws = nullptr;  // Pointer to the WebSocket connection
};

#endif // WEBSOCKETCLIENT_H
