#include "MainMenu.h"
#include "Discovery.h"
MainMenu::MainMenu(GameStateMachine* pMachine) : GameState(pMachine)
{
	buttons.push_back(new Button("Search for a server", 100, 100, 500, 30, [this] {this->TransferToDiscoveryScreen(); }));
}

void MainMenu::TransferToDiscoveryScreen()
{
	machine->SwitchState(new DiscoveryScreen(machine));
}

void MainMenu::Update(double deltaTime)
{
}
void MainMenu::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 150, 255);
	SDL_RenderClear(renderer);
	GameState::RenderButtons(renderer);
	SDL_RenderPresent(renderer);
}
void MainMenu::OnEnter()
{
}
void MainMenu::OnExit()
{
}
void MainMenu::Input(SDL_Event& event)
{
}
void MainMenu::OnReceive(char* inData, int dataLength)
{
}