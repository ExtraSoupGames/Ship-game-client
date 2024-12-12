#include "StartRoom.h"
#include "MyGame.h"
StartRoom::StartRoom(GameStateMachine* pMachine) : PlayerGameState(pMachine)
{
    player = new PlayerController(pMachine, new CollisionManager());
    startPad = new StartPad(machine->settings->textureManager);
}

StartRoom::~StartRoom()
{
    delete player;
    delete players;
    delete startPad;
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
    SDL_RenderPresent(renderer);
}
void StartRoom::HandleStartPadData(string message) {
    startPad->UpdateTexture(message);
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
}

void StartRoom::Input(SDL_Event& event)
{
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
    BroadcastPlayerData(deltaTime, player);
    player->UpdateMove(deltaTime, machine->settings->screenScaling());

    double delay = 100;  //snapshot buffer should be 3-4x base rate of packets - this way we can lose 2 packets and not experience jittering
    double timern = clientServerTimeDiff + SDL_GetTicks() - delay;
    for (OtherPlayer* p : *players) {
        p->Interpolate(timern);
    }
}

void StartRoom::OnEnter()
{
}

void StartRoom::OnExit()
{
}
