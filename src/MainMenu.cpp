#include "MainMenu.h"
#include "Discovery.h"
#include "SettingsMenu.h"
#include "ServerCreate.h"
#include "Textbox.h"
MainMenu::MainMenu(GameStateMachine* pMachine) : GameState(pMachine)
{
	UIElements.push_back(new Button("Search for a server", 100, 100, [this] {this->TransferToDiscoveryScreen(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	UIElements.push_back(new Button("Settings", 100, 200, [this] {this->TransferToSettingsScreen(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	UIElements.push_back(new Button("Create a server", 100, 300, [this] {this->TransferToCreateScreen(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
}
MainMenu::~MainMenu() {

}
void MainMenu::Update(double deltaTime)
{
}
void MainMenu::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
	SDL_RenderClear(renderer);
	GameState::RenderUI(renderer);
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
	UIInput(event);
}
void MainMenu::OnReceive(char* inData, int dataLength)
{
}

void MainMenu::TransferToDiscoveryScreen()
{
	machine->SwitchState(new DiscoveryScreen(machine));
}
void MainMenu::TransferToSettingsScreen()
{
	machine->SwitchState(new SettingsMenu(machine));
}
void MainMenu::TransferToCreateScreen()
{
	machine->SwitchState(new ServerCreate(machine));
}