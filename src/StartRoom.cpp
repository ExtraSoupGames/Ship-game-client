#include "StartRoom.h"
#include "MyGame.h"
StartRoom::StartRoom(GameStateMachine* pMachine) : PlayerGameState(pMachine)
{
    player = new PlayerController(pMachine, new CollisionManager());
    startPad = new StartPad(machine->settings->textureManager);
    colourChooser = new ColourChooser(machine->settings->server, machine->settings->clientID);
    UIElements.push_back(new Button("ChooseColour1", 50, 50, [this] {colourChooser->ColourChosen(1); }, machine->settings->textureManager, machine->settings->screenScaling(), 20));
    UIElements.push_back(new Button("ChooseColour2", 50, 100, [this] {colourChooser->ColourChosen(2); }, machine->settings->textureManager, machine->settings->screenScaling(), 20));
    UIElements.push_back(new Button("ChooseColour3", 50, 150, [this] {colourChooser->ColourChosen(3); }, machine->settings->textureManager, machine->settings->screenScaling(), 20));
    UIElements.push_back(new Button("ChooseColour4", 200, 50, [this] {colourChooser->ColourChosen(4); }, machine->settings->textureManager, machine->settings->screenScaling(), 20));
    UIElements.push_back(new Button("ChooseColour5", 200, 100, [this] {colourChooser->ColourChosen(5); }, machine->settings->textureManager, machine->settings->screenScaling(), 20));
    UIElements.push_back(new Button("ChooseColour6", 200, 150, [this] {colourChooser->ColourChosen(6); }, machine->settings->textureManager, machine->settings->screenScaling(), 20));

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
    cout << message.at(32) << endl;;
    if (message.at(32) == '1') {
        colourChooser->SelectedColour(message.substr(33,3));
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
}

void StartRoom::Input(SDL_Event& event)
{
    GameState::UIInput(event);
    player->HandleInput(event, nullptr);
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            if (startPad->GetLeverBox()->Collides(Hitbox{ player->GetXForServer(), player->GetYForServer(), 16, 16 })) {
                machine->settings->server->SendMessage("0100"); // lever pulled code
                cout << "Lever pulled" << endl;
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
}

void StartRoom::OnEnter()
{
}

void StartRoom::OnExit()
{
}
