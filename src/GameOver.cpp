#include "GameOver.h"
#include "StartRoom.h"
GameOver::GameOver(GameStateMachine* machine) : GameState(machine) {
	UIElements.push_back(new Button("Exit", 25, 10, [this] {this->ExitButtonPressed(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	//TODO add method to show game report
}
GameOver::~GameOver() {

}
void GameOver::Update(double deltaTime)
{
}

void GameOver::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
	SDL_RenderClear(renderer);
	GameState::RenderUI(renderer);
	SDL_RenderPresent(renderer);
}

void GameOver::OnEnter()
{
}

void GameOver::OnExit()
{
}

void GameOver::Input(SDL_Event& event)
{
	UIInput(event);
}

void GameOver::OnReceive(char* inData, int dataLength)
{
}

void GameOver::ExitButtonPressed()
{
	//TODO add wipe server info function
	//machine->WipeServerInfo(); // TODO allow players to play again without having to reconnect
	machine->SwitchState(new StartRoom(machine));
}
