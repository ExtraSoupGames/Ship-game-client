#pragma once
#include "SDL.h"
#include "Data.h"
#include "GameState.h"
#include "ReportVisual.h"
using namespace std;
class GameOver : public GameState {
public:
	GameOver(GameStateMachine* machine);
	GameOver(GameStateMachine* machine, GameReport* report);
	~GameOver();
	void Update(double deltaTime);
	void Render(SDL_Renderer* renderer);
	void OnEnter();
	void OnExit();
	void Input(SDL_Event& event);
	void OnReceive(char* inData, int dataLength);
	void ExitButtonPressed();
};