#include "MainMenu.h"

MainMenu::MainMenu(GameStateMachine* pMachine) : GameState(pMachine)
{
}

void MainMenu::Update(double deltaTime)
{
}
void MainMenu::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 150, 255);
	SDL_RenderClear(renderer);
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