#include "GameState.h"
GameStateMachine::GameStateMachine()
{
    currentState = nullptr;
}

void GameStateMachine::SwitchState(GameState* newState)
{
	currentState = newState;
}
void GameStateMachine::Run(UDPsocket* socket, UDPpacket* packet, SDL_Renderer* renderer) {
#pragma region mainLoop

    Uint64 currentTime = SDL_GetPerformanceCounter();
    Uint64 lastFrameTime = 0;
    double deltaTime = 0;
    while (running) {
        lastFrameTime = currentTime;
        currentTime = SDL_GetPerformanceCounter();
        deltaTime = (double)((currentTime - lastFrameTime) * 1000 / (double)SDL_GetPerformanceFrequency());

        currentState->Update(deltaTime);

#pragma region receivePackets
        int packets = SDLNet_UDP_Recv(*socket, packet);
        if (packets) {
            char* inData = ((char*)packet->data);
            currentState->OnReceive(inData, packet->len);
        }
#pragma endregion receivePackets

#pragma region input
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && event.key.repeat == 0) {
                currentState->Input(event);

                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_p:
                    cout << "P PRESSED" << endl;
                default:
                    break;
                }
            }

            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
#pragma endregion input

        currentState->Render(renderer);
    }
#pragma endregion mainLoop
}
