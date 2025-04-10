#include "Client.h"

int main() {
    Client client("ws://localhost:9001");
    client.connect();
    client.sendHttpRequest("/lobby");
    client.sendWebSocketMessage("Hello, server!");

    return 0;
}
