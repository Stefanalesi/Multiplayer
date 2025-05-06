#include "WebSocketClient.h"
#include <iostream>
#include <sstream>
#include <random>
#include <vector>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

WebSocketClient::WebSocketClient() : clientSocket(INVALID_SOCKET), isConnected(false), port(0) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
    }
}

WebSocketClient::~WebSocketClient() {
    close();
    WSACleanup();
}

bool WebSocketClient::parseUrl(const std::string& url) {
    if (url.substr(0, 5) != "ws://") {
        std::cerr << "Invalid WebSocket URL. Must start with ws://" << std::endl;
        return false;
    }

    size_t hostStart = 5;
    size_t hostEnd = url.find(':', hostStart);
    if (hostEnd == std::string::npos) {
        hostEnd = url.find('/', hostStart);
        if (hostEnd == std::string::npos) {
            hostEnd = url.length();
        }
        port = 80;
    } else {
        size_t portEnd = url.find('/', hostEnd);
        if (portEnd == std::string::npos) {
            portEnd = url.length();
        }
        port = std::stoi(url.substr(hostEnd + 1, portEnd - hostEnd - 1));
    }

    serverUrl = url.substr(hostStart, hostEnd - hostStart);
    return true;
}

std::string WebSocketClient::generateWebSocketKey() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::vector<unsigned char> key(16);
    for (int i = 0; i < 16; ++i) {
        key[i] = static_cast<unsigned char>(dis(gen));
    }
    return base64Encode(key);
}

std::string WebSocketClient::base64Encode(const std::vector<unsigned char>& data) {
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    for (const auto& byte : data) {
        char_array_3[i++] = byte;
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < i + 1; j++)
            ret += base64_chars[char_array_4[j]];

        while (i++ < 3)
            ret += '=';
    }

    return ret;
}

bool WebSocketClient::performHandshake(SOCKET socket) {
    std::string key = generateWebSocketKey();
    std::stringstream request;
    request << "GET / HTTP/1.1\r\n"
            << "Host: " << serverUrl << ":" << port << "\r\n"
            << "Upgrade: websocket\r\n"
            << "Connection: Upgrade\r\n"
            << "Sec-WebSocket-Key: " << key << "\r\n"
            << "Sec-WebSocket-Version: 13\r\n"
            << "\r\n";

    std::string requestStr = request.str();
    if (::send(socket, requestStr.c_str(), static_cast<int>(requestStr.length()), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send handshake request" << std::endl;
        return false;
    }

    char buffer[1024];
    int bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        std::cerr << "Failed to receive handshake response" << std::endl;
        return false;
    }

    std::string response(buffer, bytesReceived);
    if (response.find("HTTP/1.1 101") == std::string::npos) {
        std::cerr << "Invalid handshake response" << std::endl;
        return false;
    }

    return true;
}

bool WebSocketClient::connect(const std::string& url) {
    if (!parseUrl(url)) {
        return false;
    }

    struct addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* result = nullptr;
    if (getaddrinfo(serverUrl.c_str(), std::to_string(port).c_str(), &hints, &result) != 0) {
        std::cerr << "Failed to resolve server address" << std::endl;
        return false;
    }

    clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        freeaddrinfo(result);
        return false;
    }

    if (::connect(clientSocket, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server" << std::endl;
        closesocket(clientSocket);
        freeaddrinfo(result);
        return false;
    }

    freeaddrinfo(result);

    if (!performHandshake(clientSocket)) {
        closesocket(clientSocket);
        return false;
    }

    isConnected = true;
    return true;
}

bool WebSocketClient::send(const std::string& message) {
    if (!isConnected) {
        std::cerr << "Not connected to server" << std::endl;
        return false;
    }

    std::vector<unsigned char> frame;
    frame.push_back(0x81); // FIN + Text frame

    if (message.length() <= 125) {
        frame.push_back(static_cast<unsigned char>(message.length()));
    } else if (message.length() <= 65535) {
        frame.push_back(126);
        frame.push_back((message.length() >> 8) & 0xFF);
        frame.push_back(message.length() & 0xFF);
    } else {
        frame.push_back(127);
        for (int i = 7; i >= 0; --i) {
            frame.push_back((message.length() >> (i * 8)) & 0xFF);
        }
    }

    frame.insert(frame.end(), message.begin(), message.end());

    if (::send(clientSocket, reinterpret_cast<const char*>(frame.data()), static_cast<int>(frame.size()), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send message" << std::endl;
        return false;
    }

    return true;
}

std::string WebSocketClient::receive() {
    if (!isConnected) {
        std::cerr << "Not connected to server" << std::endl;
        return "";
    }

    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        std::cerr << "Failed to receive message" << std::endl;
        return "";
    }

    unsigned char* data = reinterpret_cast<unsigned char*>(buffer);
    bool fin = (data[0] & 0x80) != 0;
    int opcode = data[0] & 0x0F;
    bool masked = (data[1] & 0x80) != 0;
    size_t payloadLength = data[1] & 0x7F;

    size_t headerSize = 2;
    if (payloadLength == 126) {
        payloadLength = (data[2] << 8) | data[3];
        headerSize += 2;
    } else if (payloadLength == 127) {
        payloadLength = 0;
        for (int i = 0; i < 8; ++i) {
            payloadLength = (payloadLength << 8) | data[2 + i];
        }
        headerSize += 8;
    }

    if (masked) {
        unsigned char mask[4];
        for (int i = 0; i < 4; ++i) {
            mask[i] = data[headerSize + i];
        }
        headerSize += 4;

        for (size_t i = 0; i < payloadLength; ++i) {
            data[headerSize + i] ^= mask[i % 4];
        }
    }

    return std::string(reinterpret_cast<char*>(data + headerSize), payloadLength);
}

void WebSocketClient::close() {
    if (isConnected) {
        if (clientSocket != INVALID_SOCKET) {
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
        }
        isConnected = false;
    }
}
