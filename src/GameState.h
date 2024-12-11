#pragma once
#include "SDL.h"
#include "GlobalSettings.h"
#include "Button.h"
#include <SDL_net.h>
#include <iostream>
#include <vector>
#include "Interpolator.h"
using namespace std;
class PlayerController;
class GameStateMachine;
class GameState {
private:
protected:
	GameStateMachine* machine;
	vector<UIElement*> UIElements;
	void RenderUI(SDL_Renderer* renderer);
	void UIInput(SDL_Event& e);
	void UIUpdate();
public:
	GameState(GameStateMachine* pMachine) {
		machine = pMachine;
	}
	~GameState();
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
	~GameStateMachine();
	bool receivingPackets;
	GlobalSettingsProfile* settings;
	bool running;
	void SwitchState(GameState* newState);
	void Run();
	void WipeSettings();
	void Quit();
};
class PlayerGameState : public GameState {
protected:

	double broadcastTimer;
	double broadcastSpacing = 20;
	double serverStartTime;
	double clientServerTimeDiff;
	void HandlePlayerData(string message, vector<OtherPlayer*>* players);
	void BroadcastPlayerData(double deltaTime, PlayerController* player);
	vector<OtherPlayer*>* players;

public:
	PlayerGameState(GameStateMachine* pMachine);
	~PlayerGameState();
};