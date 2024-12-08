#include "GameOver.h"
#include "StartRoom.h"
GameOver::GameOver(GameStateMachine* machine) : GameOver(machine, new GameReport()) {
}
GameOver::GameOver(GameStateMachine* machine, GameReport* report) : GameState(machine) {
	UIElements.push_back(new Button("Exit", 25, 10, [this] {this->ExitButtonPressed(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	//TODO add UI element to show game report
	UIElements.push_back(new ReportVisual(report, 50, 50, machine->settings->textureManager, machine->settings->screenScaling(), 25));

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
	machine->WipeSettings(); // TODO allow players to play again without having to reconnect
	machine->SwitchState(new StartRoom(machine));
}
