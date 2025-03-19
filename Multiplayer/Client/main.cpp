#include "Client.h"
int main() {
    Client client;
    client.connectToServer("ws://localhost:9001");
    return 0;
}
