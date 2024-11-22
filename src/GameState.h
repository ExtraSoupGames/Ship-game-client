#pragma once
#include "SDL.h"
#include "GlobalSettings.h"
#include <SDL_net.h>
#include <iostream>
using namespace std;
class GameStateMachine;
class GameState {
protected:
	GameStateMachine* machine;
public:
	GameState(GameStateMachine* pMachine) {
		machine = pMachine;
	}
	virtual void Update(double deltaTime) = 0;
	virtual void Render(SDL_Renderer* renderer) = 0;
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;
	virtual void Input(SDL_Event& event) = 0;
	virtual void OnReceive(char* inData, int dataLength) = 0;
};
class GameStateMachine {
private:
	GameState* currentState;
	GameState* nextState;
protected:
public:
	GameStateMachine();
	bool receivingPackets;
	GlobalSettingsProfile* settings;
	bool running;
	void SwitchState(GameState* newState);
	void Run(SDL_Renderer* renderer);
};