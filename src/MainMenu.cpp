#include "MainMenu.h"
#include "Discovery.h"
#include "SettingsMenu.h"
#include "ServerCreate.h"
#include "Textbox.h"
MainMenu::MainMenu(GameStateMachine* pMachine) : GameState(pMachine)
{
	UIElements.push_back(new TextDisplay(machine->settings->textureManager, 25, 10, machine->settings->screenScaling(), "Tidal Tussle"));
	UIElements.push_back(new Button("Search for a server", 25, 30, [this] {this->TransferToDiscoveryScreen(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	UIElements.push_back(new Button("Settings", 25, 50, [this] {this->TransferToSettingsScreen(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	UIElements.push_back(new Button("Create a server", 25, 70, [this] {this->TransferToCreateScreen(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	UIElements.push_back(new Button("Exit Game", 25, 90, [this] {this->ExitGame(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
}
MainMenu::~MainMenu() {

}
void MainMenu::Update(double deltaTime)
{
	GameState::UIUpdate();
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
void MainMenu::ExitGame() {
	machine->Quit();
}