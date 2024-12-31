#include "GameOver.h"
#include "MainMenu.h"
GameOver::GameOver(GameStateMachine* machine) : GameOver(machine, new GameReport(0)) {
}
GameOver::GameOver(GameStateMachine* pMachine, GameReport* report) : HeartbeatGameState(pMachine) {
    machine = pMachine;
    collisions = new CollisionManager();
    collisions->AddBoundary(new CollisionBoundary(0, 0, 160, 0, 0, 1));
    collisions->AddBoundary(new CollisionBoundary(160, 0, 160, 180, -1, 0));
    collisions->AddBoundary(new CollisionBoundary(0, 180, 160, 180, 0, -1));
    collisions->AddBoundary(new CollisionBoundary(0, 0, 0, 180, 1, 0));
	UIElements.push_back(new Button("Exit", 200, 10, [this] {this->ExitButtonPressed(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	UIElements.push_back(new ReportVisual(report, 200, 50, machine->settings->textureManager, machine->settings->screenScaling(), 25));
	serverStartTime = 0;
	clientServerTimeDiff = 0;
	broadcastTimer = 0;
	players = new vector<OtherPlayer*>();
	player = new PlayerController(pMachine, collisions);
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
    cout << "ClientServerDiff: " << clientServerTimeDiff << endl;
    cout << "Time right now: " << timern << endl;
    for (OtherPlayer* p : *players) {
        p->Interpolate(timern);
    }
    HeartbeatGameState::UpdateBeat(deltaTime);
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
    if (messageType == "0010") { // player data code
        HandlePlayerData(message, players);
    }
    if (messageType == "0101") { // game start code
        machine->settings->server->SendImportantMessageConfirmation(message, machine->settings->clientID);
        machine->SwitchState(new MyGame(machine));
    }
    if (messageType == "0110") { //starting pad info
        HandleNewGamePadData(message);
    }
    if (messageType == "1010") {
        HeartbeatGameState::HandleHeartbeat();
    }
    if (messageType == "1011") {
        HeartbeatGameState::PlayerKicked(message);
    }
}

void GameOver::ExitButtonPressed()
{
	machine->WipeSettings(); // TODO allow players to play again without having to reconnect
	machine->SwitchState(new MainMenu(machine));
}
