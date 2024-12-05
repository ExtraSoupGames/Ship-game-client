#include "ServerCreate.h"
#include "MainMenu.h"
#include "ServerLauncher.h"
ServerCreate::ServerCreate(GameStateMachine* machine) : GameState(machine) {
	UIElements.push_back(new Button("Exit", 25, 10, [this] {this->ExitButtonPressed(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	UIElements.push_back(new Button("Create a server", 25, 30, [this] {this->CreateButtonPressed(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	servername = new Textbox("ServerName", 25, 50, machine->settings->screenScaling(), machine->settings->textureManager, 25);
	UIElements.push_back(servername);
}
ServerCreate::~ServerCreate() {

}
void ServerCreate::Update(double deltaTime)
{
}

void ServerCreate::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
	SDL_RenderClear(renderer);
	GameState::RenderUI(renderer);
	SDL_RenderPresent(renderer);
}

void ServerCreate::OnEnter()
{
}

void ServerCreate::OnExit()
{
}

void ServerCreate::Input(SDL_Event& event)
{
	UIInput(event);
}

void ServerCreate::OnReceive(char* inData, int dataLength)
{
}
void ServerCreate::CreateButtonPressed() {
    ServerLauncher createServer = ServerLauncher(servername->GetText());
    createServer.RunServer();
}
void ServerCreate::ExitButtonPressed() {
	machine->SwitchState(new MainMenu(machine));
}