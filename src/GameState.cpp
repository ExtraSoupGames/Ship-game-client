#include "GameState.h"
#include "ServerManager.h"
#include "PlayerController.h"
void GameState::UIInput(SDL_Event& e)
{
    for (UIElement* u : UIElements) {
        u->Input(e);
    }
}
void GameState::RenderUI(SDL_Renderer* renderer)
{
    for (UIElement* u : UIElements) {
        u->Render(renderer);
    }
}
void GameState::UIUpdate() {
    for (UIElement* u : UIElements) {
        u->Update();
    }
}
GameState::~GameState() {
    UIElements.clear();
}
GameStateMachine::GameStateMachine()
{
    int screenScale = 4;
    int screenHeight = 180 * screenScale;
    int screenWidth = 320 * screenScale;
    SDL_Window* window = SDL_CreateWindow(
        "Multiplayer Ship Game Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    currentState = nullptr;
    nextState = nullptr;
    running = true;
    settings = new GlobalSettingsProfile(renderer, screenWidth, screenHeight);
    receivingPackets = false;
}
GameStateMachine::~GameStateMachine() {
    delete settings;
}

void GameStateMachine::SwitchState(GameState* newState)
{
	nextState = newState;
    if (currentState == nullptr) {
        currentState = nextState;
    }
    else{
        currentState->OnExit();
    }
    nextState->OnEnter();
}
void GameStateMachine::Run() {
    //prepare packet for receiving data
    UDPpacket* packet = SDLNet_AllocPacket(512);
    if (!packet) {
        std::cerr << "SDLNet_AllocPacket: " << SDLNet_GetError() << std::endl;
        SDLNet_UDP_Close(settings->socket);
        SDLNet_Quit();
        SDL_Quit();
    }
    Uint64 currentTime = SDL_GetPerformanceCounter();
    Uint64 lastFrameTime = 0;
    double deltaTime = 0;

    while (running) {
        lastFrameTime = currentTime;
        currentTime = SDL_GetPerformanceCounter();
        deltaTime = (double)((currentTime - lastFrameTime) * 1000 / (double)SDL_GetPerformanceFrequency());

        if (currentState != nullptr) {
            currentState->Update(deltaTime);

#pragma region receivePackets
            //only receive packets when required
            if (receivingPackets) {
                //receive the packet into the packet variable, from the socket
                int packets = SDLNet_UDP_Recv(settings->socket, packet);
                //if any packets were received
                if (packets) {
                    //copy the packet data into an array of char
                    char* inData = ((char*)packet->data);
                    //allow the current state to process whatever data is received
                    currentState->OnReceive(inData, packet->len);
                }
            }
#pragma endregion receivePackets

#pragma region input
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                currentState->Input(event);
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
#pragma endregion input

            currentState->Render(settings->renderer);
            currentState = nextState;
        }
    }
}

void GameStateMachine::WipeSettings()
{
    SDL_Renderer* keepRenderer = settings->renderer;
    int keepScreenWidth = settings->screenWidth;
    int keepScreenHeight = settings->screenHeight;
    delete settings;
    settings = new GlobalSettingsProfile(keepRenderer, keepScreenWidth, keepScreenHeight);
}
void GameStateMachine::Quit() {
    running = false;
}
void PlayerGameState::HandlePlayerData(string message, vector<OtherPlayer*>* players) {
    if (!((message.size() - 64) % 56) == 0 && message.size() > 1) { // size shoule be a multiple of 32 for the ID + 16 for the position + 7 for the state per player + 64 for timestamp
        int padding = (message.size() - 64) % 56;
        message = message.substr(0, message.size() - padding); // remove the padding
    }
    double timestamp = machine->settings->server->TimestampDecompress(message.substr(message.size() - 64, 64)); // the timestamp is the final piece of data

    //the first packet that comes from the server will be the base start time offset for all other packets
    if (serverStartTime < 100) {
        serverStartTime = timestamp;
        clientServerTimeDiff = serverStartTime - SDL_GetTicks();
    }
    string playerData = message.substr(0, (message.size() - 64));
    for (int i = 0; i < ((int)(playerData.size()) - 55); i += 56) { //iterate through each player data (each player has 3 args: ID, X, Y)
        int ID = machine->settings->server->IntDecompress(playerData.substr(i, 32));
        if (ID == machine->settings->clientID) {
            continue; // this player's movement is handled by its own playercontroller
        }
        int* position = machine->settings->server->PositionDecompress(playerData.substr(i + 32, 16));
        int X = *position;
        int Y = *(position + 1);
        PlayerState state = machine->settings->server->PlayerStateDecompress(playerData.substr(i + 48, 7));
        bool isAlive = (playerData.at(i + 55) == '1');
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
                player->isAlive = isAlive;
            }
        }
    }
}

void PlayerGameState::BroadcastPlayerData(double deltaTime, PlayerController* player)
{
    broadcastTimer += deltaTime;
    if (broadcastTimer > broadcastSpacing) {
        broadcastTimer -= broadcastSpacing;
        // Prepare player data
        stringstream binaryText;
        binaryText << "0010" <<
            machine->settings->server->IntCompress(machine->settings->clientID) <<
            machine->settings->server->PositionCompress(player->GetXForServer(), player->GetYForServer()) <<
            machine->settings->server->PlayerStateCompress(player->GetState()) <<
            machine->settings->server->BoolToChar(player->IsAlive());
        // Send player data
        machine->settings->server->SendMessage(binaryText.str());
    }
}

PlayerGameState::PlayerGameState(GameStateMachine* pMachine) : GameState(pMachine)
{
    broadcastTimer = 0;
    broadcastSpacing = 20;
    clientServerTimeDiff = 0;
    serverStartTime = 0;
    players = new vector<OtherPlayer*>();

}

PlayerGameState::~PlayerGameState()
{
    delete players;
}
