#pragma once
#include "SDL.h"
#include "SDL_net.h"
#include <iostream>
using namespace std;
class ServerManager;
class GlobalSettingsProfile {
public:
	UDPsocket socket;
	ServerManager* server;
	int clientID;
	int screenHeight;
	int screenWidth;
	SDL_Renderer* renderer;
	GlobalSettingsProfile(SDL_Renderer* pRenderer);
};