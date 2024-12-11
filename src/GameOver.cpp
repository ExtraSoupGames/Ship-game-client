#include "GameOver.h"
#include "MainMenu.h"
GameOver::GameOver(GameStateMachine* machine) : GameOver(machine, new GameReport()) {
}
GameOver::GameOver(GameStateMachine* pMachine, GameReport* report) : PlayerGameState(pMachine) {
    machine = pMachine;
	UIElements.push_back(new Button("Exit", 25, 10, [this] {this->ExitButtonPressed(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	//TODO add UI element to show game report
	UIElements.push_back(new ReportVisual(report, 100, 50, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	serverStartTime = 0;
	clientServerTimeDiff = 0;
	broadcastTimer = 0;
	players = new vector<OtherPlayer*>();
	player = new PlayerController(pMachine, new CollisionManager());
	newGamePad = new PlayerPad(machine->settings->textureManager);

}
GameOver::~GameOver() {
	delete newGamePad;
}
void GameOver::Update(double deltaTime)
{
    BroadcastPlayerData(deltaTime, player);
    player->UpdateMove(deltaTime, machine->settings->screenScaling());

    double delay = 100;  //snapshot buffer should be 3-4x base rate of packets - this way we can lose 2 packets and not experience jittering
    double timern = clientServerTimeDiff + SDL_GetTicks() - delay;
    for (OtherPlayer* p : *players) {
        p->Interpolate(timern);
    }
}
void GameOver::HandleNewGamePadData(string message) {
    newGamePad->UpdateTexture(message);
}
void GameOver::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
	SDL_RenderClear(renderer);

	newGamePad->Render(renderer, machine->settings->screenScaling());

	for (OtherPlayer* p : *players) {
		p->Render(renderer, machine->settings, 0, 0); // no camera movement on start room screen
	}
	player->Render(renderer, machine->settings, 0, 0); // no camera movement on start room screen
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
    player->HandleInput(event, nullptr);
}

void GameOver::OnReceive(char* inData, int dataLength)
{
    string message = machine->settings->server->CharToBinary(inData, dataLength);
    string messageType = message.substr(0, 4); // first 4 bits denote type of data in packet
    message = message.substr(4);
    if (messageType == "0101") { // player data code
        HandlePlayerData(message, players);
    }
    if (messageType == "1100") { //start room info code
        machine->settings->server->SendImportantMessageConfirmation(message, machine->settings->clientID);
    }
    if (messageType == "1101") { // game start code
        machine->settings->server->SendImportantMessageConfirmation(message, machine->settings->clientID);
        machine->SwitchState(new MyGame(machine));
    }
    if (messageType == "1110") { //starting pad info
        HandleNewGamePadData(message);
    }
}

void GameOver::ExitButtonPressed()
{
	machine->WipeSettings(); // TODO allow players to play again without having to reconnect
	machine->SwitchState(new MainMenu(machine));
}
