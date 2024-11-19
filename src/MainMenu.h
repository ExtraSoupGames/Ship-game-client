#pragma once
#include "GameState.h"
using namespace std;

class MainMenu : public GameState {
private:
public:
	MainMenu(GameStateMachine* pMachine);
	void Update(double deltaTime);
	void Render(SDL_Renderer* renderer);
	void OnEnter();
	void OnExit();
	void Input(SDL_Event& event);
	void OnReceive(char* inData, int dataLength);
};