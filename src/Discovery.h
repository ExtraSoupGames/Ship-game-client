#pragma once
#include "SDL.h"
#include "SDL_net.h"
#include "ServerManager.h"
#include <vector>
using namespace std;
struct ServerHost {
	string host;
	int port;
	ServerHost(string pHost, int pPort) {
		host = pHost;
		port = pPort;
	}
};
class DiscoveryScreen {
	int selectedServer;
	vector<ServerHost*> servers;
	bool ServerExists(string host, int port);
	bool selecting = false;
	double discoverDelay; // dont spam packets as this is unneccesary load on network
	double discoverTimer;
public:
	DiscoveryScreen();
	ServerHost* ShowDiscoveryScreen(SDL_Renderer* renderer, ServerManager* serverManager);
	void OnReceive(char* inData, int dataLength);
	void Input(SDL_Event& event);
};