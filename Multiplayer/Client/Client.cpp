#include "Client.h"
Client::Client() {}
void Client::connectToServer(const std::string& url) { wsClient.connect(url); }
void Client::joinLobby(int lobbyId) { httpClient.joinLobby(lobbyId); }