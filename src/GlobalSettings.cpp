#include "Globalsettings.h"
#include <random>
#include "Discovery.h"
int getRandomID() {
    //gets called once at the start to generate an ID for this client so that the server knows which data is from which client
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> randomID(1, 999999);//random distribution between 1 and big number so 2 clients most likely wont get same ID
    return randomID(rng);
}
GlobalSettingsProfile::GlobalSettingsProfile(SDL_Renderer* pRenderer, int pScreenWidth, int pScreenHeight) {
    clientID = getRandomID(); //generate a (probably) unique user ID
    //this means the client is registered to recieve packets from the server
    // 
    // Create a UDP socket
    renderer = pRenderer;
    socket = SDLNet_UDP_Open(0);
    if (!socket) {
        std::cout << "SDLNet_UDP_Open: " << SDLNet_GetError() << std::endl;
        SDLNet_Quit();
        SDL_Quit();
        throw new exception("Error opening socket");
    }
	server = new ServerManager(&socket, clientID);
	screenWidth = pScreenWidth;
    screenHeight = pScreenHeight;
}
GlobalSettingsProfile::~GlobalSettingsProfile() {
    delete server;
    SDLNet_UDP_Close(socket);
}