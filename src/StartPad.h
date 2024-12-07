#pragma once
#include <iostream>
#include "SDL.h"
#include "TextureManager.h"
#include "ServerManager.h"
#include "MyGame.h"
using namespace std;
class StartingLever {
	int x;
	int y;
	SDL_Texture* currentTexture;
	SDL_Texture* unpoweredTexture;
	SDL_Texture* poweredTexture;
	bool powered;
public:
	StartingLever(TextureManager* t);
	~StartingLever();
	void Render(SDL_Renderer* renderer, int screenScaling);
	void UpdateTexture(string bianryData);
};
class StartPad {
	int x;
	int y;
	SDL_Texture* currentTexture;
	SDL_Texture* unpoweredTexture; // no players on
	SDL_Texture* partialPoweredTexture; // some players on
	SDL_Texture* poweredTexture; // all players on
	int poweredState;
	int countdown;
	StartingLever* startLever;
public:
	StartPad(TextureManager* t);
	~StartPad();
	void Render(SDL_Renderer* renderer, int screenScaling);
	void UpdateTexture(string binaryData);
	Hitbox* GetLeverBox(); // get the hitbox for the area a player can be in to activate the start lever
};