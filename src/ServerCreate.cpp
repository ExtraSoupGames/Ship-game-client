#include "ServerCreate.h"
#include "MainMenu.h"
ServerCreate::ServerCreate(GameStateMachine* machine) : GameState(machine) {
	buttons.push_back(new Button("Exit", 100, 100, 100, 30, [this] {this->ExitButtonPressed(); }));
	buttons.push_back(new Button("Create a server", 100, 200, 500, 30, [this] {this->CreateButtonPressed(); }));
}
void ServerCreate::Update(double deltaTime)
{
}

void ServerCreate::Render(SDL_Renderer* renderer)
{
}

void ServerCreate::OnEnter()
{
}

void ServerCreate::OnExit()
{
}

void ServerCreate::Input(SDL_Event& event)
{
}

void ServerCreate::OnReceive(char* inData, int dataLength)
{
}
void ServerCreate::CreateButtonPressed() {

}
void ServerCreate::ExitButtonPressed() {
	machine->SwitchState(new MainMenu(machine));
}