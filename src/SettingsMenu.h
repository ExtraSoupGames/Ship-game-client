#pragma once
#include "SDL.h"
#include "GameState.h"
using namespace std;
class SettingsMenu : public GameState {
	void Update(double deltaTime);
	void Render(SDL_Renderer* renderer);
	void OnEnter();
	void OnExit();
	void Input(SDL_Event& event);
	void OnReceive(char* inData, int dataLength);
};