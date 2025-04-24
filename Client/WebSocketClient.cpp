#include "WebSocketClient.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

WebSocketClient::WebSocketClient(const std::string& serverUrl) : serverUrl(serverUrl), clientSocket(INVALID_SOCKET), isConnected(false) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Failed to initialize Winsock");
    }
}

WebSocketClient::~WebSocketClient() {
    close();
    WSACleanup();
}

bool WebSocketClient::parseUrl(const std::string& url) {
    size_t protocolEnd = url.find("://");
    if (protocolEnd == std::string::npos) {
        return false;
    }

    std::string protocol = url.substr(0, protocolEnd);
    if (protocol != "ws" && protocol != "wss") {
        return false;
    }

    size_t hostStart = protocolEnd + 3;
    size_t pathStart = url.find('/', hostStart);
    if (pathStart == std::string::npos) {
        host = url.substr(hostStart);
        path = "/";
    } else {
        host = url.substr(hostStart, pathStart - hostStart);
        path = url.substr(pathStart);
    }

    size_t portStart = host.find(':');
    if (portStart != std::string::npos) {
        port = std::stoi(host.substr(portStart + 1));
        host = host.substr(0, portStart);
    } else {
        port = (protocol == "wss") ? 443 : 80;
    }

    return true;
}

std::string WebSocketClient::generateWebSocketKey() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::vector<unsigned char> key(16);
    std::generate(key.begin(), key.end(), [&]() { return static_cast<unsigned char>(dis(gen)); });
    return base64Encode(std::string(key.begin(), key.end()));
}

std::string WebSocketClient::base64Encode(const std::string& input) {
    const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    for (const auto& c : input) {
        char_array_3[i++] = c;
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

bool WebSocketClient::performHandshake() {
    std::string key = generateWebSocketKey();
    std::stringstream request;
    request << "GET " << path << " HTTP/1.1\r\n"
            << "Host: " << host << ":" << port << "\r\n"
            << "Upgrade: websocket\r\n"
            << "Connection: Upgrade\r\n"
            << "Sec-WebSocket-Key: " << key << "\r\n"
            << "Sec-WebSocket-Version: 13\r\n"
            << "\r\n";

    if (::send(clientSocket, request.str().c_str(), static_cast<int>(request.str().length()), 0) == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        return false;
    }

    buffer[bytesReceived] = '\0';
    std::string response(buffer);

    return response.find("HTTP/1.1 101") != std::string::npos &&
           response.find("Upgrade: websocket") != std::string::npos &&
           response.find("Connection: Upgrade") != std::string::npos;
}

bool WebSocketClient::connect() {
    if (!parseUrl(serverUrl)) {
        return false;
    }

    addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo* result = nullptr;
    if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result) != 0) {
        return false;
    }

    clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (clientSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        return false;
    }

    if (::connect(clientSocket, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    freeaddrinfo(result);

    if (!performHandshake()) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    isConnected = true;
    return true;
}

bool WebSocketClient::sendMessage(const std::string& message) {
    if (clientSocket == INVALID_SOCKET || !isConnected) {
        return false;
    }

    std::vector<unsigned char> frame;
    frame.push_back(0x81); // FIN + text frame

    if (message.length() <= 125) {
        frame.push_back(static_cast<unsigned char>(message.length()));
    } else if (message.length() <= 65535) {
        frame.push_back(126);
        frame.push_back(static_cast<unsigned char>((message.length() >> 8) & 0xFF));
        frame.push_back(static_cast<unsigned char>(message.length() & 0xFF));
    } else {
        frame.push_back(127);
        for (int i = 7; i >= 0; --i) {
            frame.push_back(static_cast<unsigned char>((message.length() >> (i * 8)) & 0xFF));
        }
    }

    frame.insert(frame.end(), message.begin(), message.end());

    return ::send(clientSocket, reinterpret_cast<const char*>(frame.data()), static_cast<int>(frame.size()), 0) != SOCKET_ERROR;
}

std::string WebSocketClient::receive() {
    if (clientSocket == INVALID_SOCKET || !isConnected) {
        return "";
    }

    char header[2];
    if (recv(clientSocket, header, 2, 0) != 2) {
        return "";
    }

    bool fin = (header[0] & 0x80) != 0;
    int opcode = header[0] & 0x0F;
    bool masked = (header[1] & 0x80) != 0;
    uint64_t payloadLength = header[1] & 0x7F;

    if (payloadLength == 126) {
        char lengthBytes[2];
        if (recv(clientSocket, lengthBytes, 2, 0) != 2) {
            return "";
        }
        payloadLength = (static_cast<uint64_t>(lengthBytes[0]) << 8) | lengthBytes[1];
    } else if (payloadLength == 127) {
        char lengthBytes[8];
        if (recv(clientSocket, lengthBytes, 8, 0) != 8) {
            return "";
        }
        payloadLength = 0;
        for (int i = 0; i < 8; ++i) {
            payloadLength = (payloadLength << 8) | static_cast<unsigned char>(lengthBytes[i]);
        }
    }

    char maskingKey[4];
    if (masked && recv(clientSocket, maskingKey, 4, 0) != 4) {
        return "";
    }

    std::vector<char> payload(payloadLength);
    if (recv(clientSocket, payload.data(), static_cast<int>(payloadLength), 0) != payloadLength) {
        return "";
    }

    if (masked) {
        for (size_t i = 0; i < payloadLength; ++i) {
            payload[i] ^= maskingKey[i % 4];
        }
    }

    return std::string(payload.begin(), payload.end());
}

void WebSocketClient::close() {
    if (isConnected) {
        // Send close frame
        char closeFrame[] = {static_cast<char>(0x88), static_cast<char>(0x00)}; // Close frame
        ::send(clientSocket, closeFrame, 2, 0);
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        isConnected = false;
    }
} 