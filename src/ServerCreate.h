#pragma once
#include "SDL.h"
#include "GameState.h"
#include "Textbox.h"
using namespace std;
class ServerCreate : public GameState {
private:
	Textbox* servername;
public:
	ServerCreate(GameStateMachine* machine);
	~ServerCreate();
	void Update(double deltaTime);
	void Render(SDL_Renderer* renderer);
	void OnEnter();
	void OnExit();
	void Input(SDL_Event& event);
	void OnReceive(char* inData, int dataLength);
	void CreateButtonPressed();
	void ExitButtonPressed();
};