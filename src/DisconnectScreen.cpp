#include "DisconnectScreen.h"
#include "MainMenu.h"
DisconnectScreen::DisconnectScreen(GameStateMachine* machine) : GameState(machine) {
	UIElements.push_back(new TextDisplay(machine->settings->textureManager, 25, 10, machine->settings->screenScaling(), "Disconnected from server!"));
	UIElements.push_back(new Button("Exit", 25, 30, [this] {this->ExitButtonPressed(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
}
DisconnectScreen::~DisconnectScreen() {

}
void DisconnectScreen::Update(double deltaTime)
{
	GameState::UIUpdate();
}

void DisconnectScreen::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
	SDL_RenderClear(renderer);
	GameState::RenderUI(renderer);
	SDL_RenderPresent(renderer);
}

void DisconnectScreen::OnEnter()
{
}

void DisconnectScreen::OnExit()
{
}

void DisconnectScreen::Input(SDL_Event& event)
{
	UIInput(event);
}

void DisconnectScreen::OnReceive(char* inData, int dataLength)
{
}

void DisconnectScreen::ExitButtonPressed()
{
	machine->SwitchState(new MainMenu(machine));
}
