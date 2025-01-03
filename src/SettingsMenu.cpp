#include "SettingsMenu.h"
#include "MainMenu.h"
SettingsMenu::SettingsMenu(GameStateMachine* machine) : GameState(machine) {
	UIElements.push_back(new Button("Exit", 25, 10, [this] {this->ExitButtonPressed(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
}
SettingsMenu::~SettingsMenu() {

}
void SettingsMenu::Update(double deltaTime)
{
	GameState::UIUpdate();
}

void SettingsMenu::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
	SDL_RenderClear(renderer);
	GameState::RenderUI(renderer);
	SDL_RenderPresent(renderer);
}

void SettingsMenu::OnEnter()
{
}

void SettingsMenu::OnExit()
{
}

void SettingsMenu::Input(SDL_Event& event)
{
	UIInput(event);
}

void SettingsMenu::OnReceive(char* inData, int dataLength)
{
}

void SettingsMenu::ExitButtonPressed()
{
	machine->SwitchState(new MainMenu(machine));
}
