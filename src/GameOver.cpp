#include "GameOver.h"
#include "MainMenu.h"
GameOver::GameOver(GameStateMachine* machine) : GameOver(machine, new GameReport()) {
}
GameOver::GameOver(GameStateMachine* pMachine, GameReport* report) : GameState(pMachine) {
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
#pragma region PlayerDataOut
    broadcastTimer += deltaTime;
    if (broadcastTimer > broadcastSpacing) {
        broadcastTimer -= broadcastSpacing;
        // Prepare player data
        stringstream binaryText;
        binaryText << "0100" <<
            machine->settings->server->IntCompress(machine->settings->clientID) <<
            machine->settings->server->PositionCompress(player->GetXForServer(), player->GetYForServer()) <<
            machine->settings->server->PlayerStateCompress(player->GetState()) <<
            machine->settings->server->BoolToChar(player->IsAlive());
        // Send player data
        machine->settings->server->SendMessage(binaryText.str());
    }
#pragma endregion PlayerDataOut
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
void GameOver::HandlePlayerData(string message) {
    if (!((message.size() - 64) % 55) == 0 && message.size() > 1) { // size shoule be a multiple of 32 for the ID + 16 for the position + 7 for the state per player + 64 for timestamp
        int padding = (message.size() - 64) % 55;
        message = message.substr(0, message.size() - padding); // remove the padding
    }
    double timestamp = machine->settings->server->TimestampDecompress(message.substr(message.size() - 64, 64)); // the timestamp is the final piece of data

    //the first packet that comes from the server will be the base start time offset for all other packets
    if (serverStartTime < 100) {
        serverStartTime = timestamp;
        clientServerTimeDiff = serverStartTime - SDL_GetTicks();
    }
    string playerData = message.substr(0, (message.size() - 64));
    for (int i = 0; i < (playerData.size() - 54); i += 55) { //iterate through each player data (each player has 3 args: ID, X, Y)
        int ID = machine->settings->server->IntDecompress(playerData.substr(i, 32));
        if (ID == machine->settings->clientID) {
            continue; // this player's movement is handled by its own playercontroller
        }
        int* position = machine->settings->server->PositionDecompress(playerData.substr(i + 32, 16));
        int X = *position;
        int Y = *(position + 1);
        PlayerState state = machine->settings->server->PlayerStateDecompress(playerData.substr(i + 48, 7));
        if (ID != machine->settings->clientID) {
            //ignore the incoming data about this client as the client has authority on it's own player's movement

            auto it = std::find_if(players->begin(), players->end(), [&ID](OtherPlayer* e) {return e->HasID(ID); });
            if (it == players->end()) {
                //if this is a new other player
                OtherPlayer* newPlayer = new OtherPlayer(ID, machine->settings->textureManager);
                newPlayer->PlayAnimation(0);
                newPlayer->AddToBuffer(new DataStream({ new PlayerData(X, Y, state), timestamp }));
                players->push_back(newPlayer);
            }
            else {
                OtherPlayer* player = *it;
                player->AddToBuffer(new DataStream({ new PlayerData(X, Y, state), timestamp }));
            }
        }
    }
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
        HandlePlayerData(message);
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
