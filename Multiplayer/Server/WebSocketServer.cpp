#include "WebSocketServer.h"
#include <iostream>
#include <websocketpp/common/thread.hpp>

WebSocketServer::WebSocketServer() {
    // Initialize ASIO
    ws_server.init_asio();
    
    // Set up handlers
    ws_server.set_open_handler(bind(&WebSocketServer::on_open, this, websocketpp::lib::placeholders::_1));
    ws_server.set_close_handler(bind(&WebSocketServer::on_close, this, websocketpp::lib::placeholders::_1));
    ws_server.set_message_handler(bind(&WebSocketServer::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
}

WebSocketServer::~WebSocketServer() {
    stop();
}

void WebSocketServer::run(uint16_t port) {
    try {
        // Listen on the specified port
        ws_server.listen(port);
        
        // Start the server accept loop
        ws_server.start_accept();
        
        // Start the ASIO io_service run loop
        thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&server::run, &ws_server);
        
        std::cout << "WebSocket server running on port " << port << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
    }
}

void WebSocketServer::stop() {
    if (thread) {
        ws_server.stop();
        thread->join();
    }
}

void WebSocketServer::broadcast(const std::string& message) {
    for (auto it : connections) {
        try {
            ws_server.send(it, message, websocketpp::frame::opcode::text);
        } catch (const std::exception& e) {
            std::cerr << "Error broadcasting message: " << e.what() << std::endl;
        }
    }
}

void WebSocketServer::on_open(websocketpp::connection_hdl hdl) {
    connections.insert(hdl);
    std::cout << "New client connected. Total clients: " << connections.size() << std::endl;
}

void WebSocketServer::on_close(websocketpp::connection_hdl hdl) {
    connections.erase(hdl);
    std::cout << "Client disconnected. Total clients: " << connections.size() << std::endl;
}

void WebSocketServer::on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::cout << "Received message from client: " << msg->get_payload() << std::endl;
    
    // Echo the message back to the client
    try {
        ws_server.send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << std::endl;
    }
}
