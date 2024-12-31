#pragma once
#include "SDL.h"
#include "GameState.h"
#include "TextDisplay.h"
using namespace std;
class DisconnectScreen : public GameState {
public:
	DisconnectScreen(GameStateMachine* machine);
	~DisconnectScreen();
	void Update(double deltaTime);
	void Render(SDL_Renderer* renderer);
	void OnEnter();
	void OnExit();
	void Input(SDL_Event& event);
	void OnReceive(char* inData, int dataLength);
	void ExitButtonPressed();
};