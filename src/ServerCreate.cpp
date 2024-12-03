#include "ServerCreate.h"
#include "MainMenu.h"
#include "ServerLauncher.h"
ServerCreate::ServerCreate(GameStateMachine* machine) : GameState(machine) {
	UIElements.push_back(new Button("Exit", 100, 100, 100, 30, [this] {this->ExitButtonPressed(); }, 25));
	UIElements.push_back(new Button("Create a server", 100, 200, 500, 30, [this] {this->CreateButtonPressed(); }, 25));
	servername = new Textbox("ServerName", 100, 300, 500, 30, 25);
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