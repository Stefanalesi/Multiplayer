#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H
#include <string>
#include <uwebsockets/App.h>
class WebSocketClient {
public:
    void connect(const std::string& url);
};
#endif