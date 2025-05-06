#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <string>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::thread> thread_ptr;

class WebSocketServer {
public:
    WebSocketServer();
    ~WebSocketServer();

    void run(uint16_t port = 9001);
    void stop();
    void broadcast(const std::string& message);

private:
    server ws_server;
    thread_ptr thread;
    std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections;

    void on_open(websocketpp::connection_hdl hdl);
    void on_close(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg);
};

#endif // WEBSOCKETSERVER_H
