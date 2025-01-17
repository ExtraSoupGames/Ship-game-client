#include <SDL.h>
#include <SDL_net.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cstring>
#include <chrono>
#include <sstream>
#include <vector>
#include <string>

#include "MyGame.h"
#include "Discovery.h"
#include "GameState.h"
#include "MainMenu.h"

int main(int argc, char* argv[]) {
#pragma region initialization
    // Initialize SDL and SDL_net
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return 1;
    }

    if (SDLNet_Init() < 0) {
        cerr << "SDLNet could not initialize! SDLNet_Error: " << SDLNet_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    if (TTF_Init() < 0) {
        cerr << "TTF Could not initialize! TTF_Error: " << TTF_GetError() << endl;
        SDL_Quit();
        SDLNet_Quit();
        return 1;
    }
    //Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        SDL_Quit();
        SDLNet_Quit();
        Mix_Quit();
        return 1;
    }
#pragma endregion initialization
    GameStateMachine* game = new GameStateMachine();
    GameState* gameState = (GameState*)new MainMenu(game);
    game->SwitchState(gameState);
    game->receivingPackets = true;
    game->Run();
    SDLNet_Quit();
    SDL_Quit();
    Mix_Quit();

    return 0;
}
