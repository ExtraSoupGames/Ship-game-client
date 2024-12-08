#pragma once
#include "SDL.h"
#include "Data.h"
#include "GameState.h"
#include "ReportVisual.h"
#include "Interpolator.h"
#include "PlayerPad.h"
using namespace std;
class GameOver : public GameState {
private:
	PlayerPad* newGamePad;
	double serverStartTime;
	double clientServerTimeDiff;
	vector<OtherPlayer*>* players;
	PlayerController* player;
	GameStateMachine* machine;

	double broadcastTimer;
	double broadcastSpacing = 20;
	double serverBroadcastTimer = 0;
	double serverBroadcastDelay = 500;//adjust this to change how quickly important messages are resent - could affect performance if too low

	void HandleNewGamePadData(string message);
	void HandlePlayerData(string message);
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