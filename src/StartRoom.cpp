#include "StartRoom.h"
#include "MyGame.h"
StartRoom::StartRoom(GameStateMachine* pMachine) : HeartbeatGameState(pMachine)
{
    collisions = new CollisionManager();
    collisions->AddBoundary(new CollisionBoundary(0, 0, 160, 0, 0, 1));
    collisions->AddBoundary(new CollisionBoundary(160, 0, 160, 180, -1, 0));
    collisions->AddBoundary(new CollisionBoundary(0, 180, 160, 180, 0, -1));
    collisions->AddBoundary(new CollisionBoundary(0, 0, 0, 180, 1, 0));
    player = new PlayerController(pMachine, collisions);
    startPad = new StartPad(machine->settings->textureManager);
    colourChooser = new ColourChooser(machine->settings->server, machine->settings->clientID);
    UIElements.push_back(new Button("Pick Colour 1", 220, 5, [this] {colourChooser->ColourChosen(1); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
    UIElements.push_back(new Button("Pick Colour 2", 220, 35, [this] {colourChooser->ColourChosen(2); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
    UIElements.push_back(new Button("Pick Colour 3", 220, 65, [this] {colourChooser->ColourChosen(3); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
    UIElements.push_back(new Button("Pick Colour 4", 220, 95, [this] {colourChooser->ColourChosen(4); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
    UIElements.push_back(new Button("Pick Colour 5", 220, 125, [this] {colourChooser->ColourChosen(5); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
    UIElements.push_back(new Button("Pick Colour 6", 220, 155, [this] {colourChooser->ColourChosen(6); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));

}

StartRoom::~StartRoom()
{
    delete player;
    delete players;
    delete startPad;
    delete colourChooser;
}

void StartRoom::Render(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderClear(renderer);

    startPad->Render(renderer, machine->settings->screenScaling());

    for (OtherPlayer* p : *players) {
        p->Render(renderer, machine->settings, 0 ,0); // no camera movement on start room screen
    }
    player->Render(renderer, machine->settings, 0 ,0); // no camera movement on start room screen
    GameState::RenderUI(renderer);
    SDL_RenderPresent(renderer);
}
void StartRoom::HandleStartPadData(string message) {
    startPad->UpdateTexture(message);
}
void StartRoom::HandleColourData(string message) {
    colourChooser->IncomingData(message);
}
void StartRoom::HandleSelectionConfirmation(string message) {
    if (message.at(32) == '1') {
        string colourCode = message.substr(33, 3);
        colourChooser->SelectedColour(colourCode);
        player->ReloadTexturesWithPalette(colourCode);
        machine->settings->playerPalette = colourCode;
    }
}
void StartRoom::OnReceive(char* inData, int dataLength)
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
        HandleStartPadData(message);
    }
    if (messageType == "1000") { // confirmation of a colour selection
        machine->settings->server->SendImportantMessageConfirmation(message, machine->settings->clientID);
        if (machine->settings->server->IntDecompress(message.substr(36, 32)) == machine->settings->clientID) {
            HandleSelectionConfirmation(message);
        }
    }
    if (messageType == "1001") { // available colour info
        HandleColourData(message);
    }
    if (messageType == "1110") { // important message confirmation
        machine->settings->server->ReceiveImportantMessageConfirmation(message);
    }
    if (messageType == "1010") {
        HeartbeatGameState::HandleHeartbeat();
    }
    if (messageType == "1011") {
        HeartbeatGameState::PlayerKicked(message);
    }
}

void StartRoom::Input(SDL_Event& event)
{
    GameState::UIInput(event);
    player->HandleInput(event, nullptr);
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            if (startPad->GetLeverBox()->Collides(Hitbox{ player->GetXForServer(), player->GetYForServer(), 16, 16 })) {
                machine->settings->server->SendMessage("0100"); // lever pulled code
            }
        }
    }
}

void StartRoom::Update(double deltaTime)
{

    GameState::UIUpdate();
    BroadcastPlayerData(deltaTime, player);
    player->UpdateMove(deltaTime, machine->settings->screenScaling());

    double delay = 100;  //snapshot buffer should be 3-4x base rate of packets - this way we can lose 2 packets and not experience jittering
    double timern = clientServerTimeDiff + SDL_GetTicks() - delay;
    for (OtherPlayer* p : *players) {
        p->Interpolate(timern);
    }
    serverBroadcastTimer += deltaTime;
    if (serverBroadcastTimer > serverBroadcastDelay) {
        serverBroadcastTimer -= serverBroadcastDelay;
        machine->settings->server->SendAllImportantMessages();
    }
    HeartbeatGameState::UpdateBeat(deltaTime);
}

void StartRoom::OnEnter()
{
}

void StartRoom::OnExit()
{
}
