#include "GameState.h"
void GameState::OnClick(int x, int y)
{
    for (Button* b : buttons) {
        if (b->IsInBounds(x, y)) {
            b->Click();
        }
    }
}
void GameState::RenderButtons(SDL_Renderer* renderer)
{
    for (Button* b : buttons) {
        b->Render(renderer);
    }
}
GameStateMachine::GameStateMachine(SDL_Renderer* renderer)
{
    currentState = nullptr;
    nextState = nullptr;
    running = true;
    settings = new GlobalSettingsProfile(renderer);
    receivingPackets = false;
}

void GameStateMachine::SwitchState(GameState* newState)
{
	nextState = newState;
    if (currentState == nullptr) {
        currentState = nextState;
    }
}
void GameStateMachine::Run(SDL_Renderer* renderer) {
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
            if (receivingPackets) {
                int packets = SDLNet_UDP_Recv(settings->socket, packet);
                if (packets) {
                    char* inData = ((char*)packet->data);
                    currentState->OnReceive(inData, packet->len);
                }
            }
#pragma endregion receivePackets

#pragma region input
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                currentState->Input(event);
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    currentState->OnClick(event.button.x, event.button.y);
                }
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
#pragma endregion input

            currentState->Render(renderer);
            currentState = nextState;
        }
    }
}


