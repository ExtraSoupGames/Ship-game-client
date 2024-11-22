#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <cstring>
#include <chrono>
#include <sstream>
#include <vector>
#include <string>

#include "MyGame.h"
#include "Discovery.h"
#include "GameState.h"

int main(int argc, char* argv[]) {
#pragma region initialization
    // Initialize SDL and SDL_net
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (SDLNet_Init() < 0) {
        std::cerr << "SDLNet could not initialize! SDLNet_Error: " << SDLNet_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow(
        "Multiplayer Ship Game Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
#pragma endregion initialization

    GameStateMachine* game = new GameStateMachine();
    GameState* gameState = (GameState*)new DiscoveryScreen(game, renderer);
    game->SwitchState(gameState);
    game->receivingPackets = true;


    game->Run(renderer);
    // Clean up
    SDLNet_Quit();
    SDL_Quit();

    return 0;
}
