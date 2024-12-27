#pragma once
#include "SDL.h"
#include "SDL_net.h"
#include <iostream>
using namespace std;
class ServerManager;
class TextureManager;
class GlobalSettingsProfile {
public:
	UDPsocket socket;
	ServerManager* server;
	TextureManager* textureManager;
	int clientID;
	int screenHeight;
	int screenWidth;
	string playerPalette;
	int screenScaling() {
		SDL_assert(screenWidth / screenHeight == 16 / 9); //ensure aspect ratio
		return screenWidth / 320; // output should be 320 x 180 overall so this is the scale to render things at
	}
	SDL_Renderer* renderer;
	GlobalSettingsProfile(SDL_Renderer* pRenderer, int screenWidth, int screenHeight);
	~GlobalSettingsProfile();
};