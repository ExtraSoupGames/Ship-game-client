#pragma once
#include "SDL.h"
#include "SDL_net.h"
#include "ServerManager.h"
#include "GameState.h"
#include "ServerLauncher.h"
#include "Button.h"
#include "TextDisplay.h"
#include <vector>
using namespace std;
struct ServerHost {
	string host;
	int port;
	string name;
	ServerHost(string pHost, int pPort, string pName) {
		host = pHost;
		port = pPort;
		name = pName;
	}
};
class DiscoveryScreen : public GameState{
	int selectedServer;
	vector<ServerHost*> servers;
	bool ServerExists(string host, int port);
	bool selecting;
	Button* connectButton;
	TextDisplay* selectedServerText;
	double discoverDelay; // dont spam packets as this is unneccesary load on network
	double discoverTimer;
	TTF_Font* font;
	void ServerClickedEvent();
	void TransferToMainMenu();
	void SelectServer(int serverID);
public:
	DiscoveryScreen(GameStateMachine* machine);
	~DiscoveryScreen();
	void Render(SDL_Renderer* renderer) override;
	void OnReceive(char* inData, int dataLength) override;
	void Input(SDL_Event& event) override;
	void Update(double deltaTime) override;
	void OnEnter() override;
	void OnExit() override;
};