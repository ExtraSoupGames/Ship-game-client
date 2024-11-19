#pragma once
#include "SDL.h"
#include "SDL_net.h"
#include "ServerManager.h"
#include <vector>
#include "GameState.h"
using namespace std;
struct ServerHost {
	string host;
	int port;
	ServerHost(string pHost, int pPort) {
		host = pHost;
		port = pPort;
	}
};
class DiscoveryScreen : public GameState{
	int selectedServer;
	vector<ServerHost*> servers;
	bool ServerExists(string host, int port);
	bool selecting = false;
	double discoverDelay; // dont spam packets as this is unneccesary load on network
	double discoverTimer;
public:
	DiscoveryScreen();
	ServerHost* ShowDiscoveryScreen(ServerManager* serverManager);
	void Render(SDL_Renderer* renderer) override;
	void OnReceive(char* inData, int dataLength) override;
	void Input(SDL_Event& event) override;
	void Update(double deltaTime) override;
	void OnEnter() override;
	void OnExit() override;
};