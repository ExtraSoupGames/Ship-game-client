#pragma once
#include "SDL.h"
#include "GameState.h"
#include "ServerManager.h"
#include "Interpolator.h"
#include "PlayerController.h"
#include <iostream>
using namespace std;
class StartRoom : public GameState {
private:
	double serverStartTime;
	double clientServerTimeDiff;
	vector<OtherPlayer*>* players;
	PlayerController* player;

	double broadcastTimer;
	double broadcastSpacing = 20;
	double serverBroadcastTimer = 0;
	double serverBroadcastDelay = 500;//adjust this to change how quickly important messages are resent - could affect performance if too low

	void HandlePlayerData(string message);
public:
	StartRoom(GameStateMachine* machine);
	~StartRoom();
	void Render(SDL_Renderer* renderer) override;
	void OnReceive(char* inData, int dataLength) override;
	void Input(SDL_Event& event) override;
	void Update(double deltaTime) override;
	void OnEnter() override;
	void OnExit() override;
};