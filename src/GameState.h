#pragma once
#include "SDL.h"
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
protected:
public:
	GameStateMachine();
	bool running;
	void SwitchState(GameState* newState);
	void Run(UDPsocket* socket, UDPpacket* packet, SDL_Renderer* renderer);
};