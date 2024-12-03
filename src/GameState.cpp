#include "GameState.h"
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


