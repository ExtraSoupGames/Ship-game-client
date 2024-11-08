#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <cstring>
#include <chrono>
#include <sstream>
#include <vector>
#include <string>
#include <random>;

#include "MyGame.h"

void SendMessage(UDPsocket* socket, int message) {
    IPaddress ip;
    SDLNet_ResolveHost(&ip, "127.0.0.1", 55555); // Connect to the server

    // Prepare the packet to send
    UDPpacket* packet = SDLNet_AllocPacket(512);
    if (!packet) {
        std::cerr << "SDLNet_AllocPacket: " << SDLNet_GetError() << std::endl;
        SDLNet_UDP_Close(*socket);
        SDLNet_Quit();
        SDL_Quit();
    }

    // Add the data to the packet
    memcpy(packet->data, &message, sizeof(int));
    packet->len = sizeof(int) + 1; // Include null terminator
    packet->address = ip;

    // Send the packet
    if (SDLNet_UDP_Send(*socket, -1, packet) == 0) {
        std::cerr << "SDLNet_UDP_Send: " << SDLNet_GetError() << std::endl;
    }
}

int getRandomID() {
    //gets called once at the start to generate an ID for this client so that the server knows which data is from which client
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> randomID(1, 999999);//random distribution between 1 and big number so 2 clients most likely wont get same ID
    return randomID(rng);
}
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
    // Create a UDP socket
    UDPsocket socket = SDLNet_UDP_Open(0);
    if (!socket) {
        std::cerr << "SDLNet_UDP_Open: " << SDLNet_GetError() << std::endl;
        SDLNet_Quit();
        SDL_Quit();
        return 1;
    }

    signed int clientID;
    clientID = getRandomID(); //generate a (probably) unique user ID
    //this means the client is registered to recieve packets from the server
    SDL_Event event;

    SDL_Window* window = SDL_CreateWindow(
        "Multiplayer Ship Game Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    MyGame* game = new MyGame(clientID, &socket, renderer);


    //prepare packet for receiving data
    UDPpacket* packet = SDLNet_AllocPacket(512);
    if (!packet) {
        std::cerr << "SDLNet_AllocPacket: " << SDLNet_GetError() << std::endl;
        SDLNet_UDP_Close(socket);
        SDLNet_Quit();
        SDL_Quit();
    }
    bool is_running = true;
#pragma endregion initialization
#pragma region mainLoop

    Uint64 currentTime = SDL_GetPerformanceCounter();
    Uint64 lastFrameTime = 0;
    double deltaTime = 0;
    while (is_running) {
        lastFrameTime = currentTime;
        currentTime = SDL_GetPerformanceCounter();
        deltaTime = (double)((currentTime - lastFrameTime) * 1000 / (double)SDL_GetPerformanceFrequency());

        game->update(deltaTime);

#pragma region receivePackets
        int packets = SDLNet_UDP_Recv(socket, packet);
        if (packets) {
            char* inData = ((char*)packet->data);
            game->on_receive(inData, packet->len);
        }
#pragma endregion receivePackets

        while (SDL_PollEvent(&event)) {
            if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && event.key.repeat == 0) {
                game->input(event);

                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    is_running = false;
                    break;
                case SDLK_p:
                    cout << "P PRESSED" << endl;
                default:
                    break;
                }
            }

            if (event.type == SDL_QUIT) {
                is_running = false;
            }
        }
        game->render(renderer);
    }
#pragma endregion mainLoop


    // Clean up
    SDLNet_UDP_Close(socket);
    SDLNet_Quit();
    SDL_Quit();

    return 0;
}
