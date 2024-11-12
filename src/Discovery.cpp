#include "Discovery.h"
DiscoveryScreen::DiscoveryScreen()
{
    selectedServer = 0;
    discoverDelay = 500;
    discoverTimer = 0;
}
ServerHost* DiscoveryScreen::ShowDiscoveryScreen(SDL_Renderer* renderer, ServerManager* serverManager) {
    discoverTimer++; //normally would use SDL_GetTicks(), but accuracy is not important so this innaccurate way is fine
    if (discoverTimer > discoverDelay) {
        discoverTimer -= discoverDelay;
        //send requests
        string discoverRequest = "0000";
        serverManager->SendMessage(discoverRequest);
    }
    //display
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < servers.size(); i++) {
        SDL_Rect* serverSymbol;
        if (i != selectedServer) {
            serverSymbol = new SDL_Rect{ 10, 10 + i * 50, 30, 30 };
        }
        else {
            serverSymbol = new SDL_Rect{ 5, 5 + i * 50, 40, 40 };
        }
        SDL_RenderDrawRect(renderer, serverSymbol);
    }
    SDL_RenderPresent(renderer);
    if (selecting) {
        return servers[selectedServer];
    }
    else {
        return nullptr;
    }
   
}
void DiscoveryScreen::OnReceive(char* inData, int dataLength) {
    string data = ServerManager::CharToBinary(inData, dataLength);
    if (data.substr(0, 4) == "0001") {
        string host = ServerManager::DecompressHost(data.substr(4, 512));
        int port = ServerManager::IntDecompress(data.substr(516, 32));
        if (!ServerExists(host, port)) {
            ServerHost* newServer = new ServerHost(host, port);
            servers.push_back(newServer);
        }
    }
}
bool DiscoveryScreen::ServerExists(string host, int port) {
    for (int i = 0; i < servers.size(); i++) {
        if (servers.at(i)->host == host && servers.at(i)->port == port) {
            return true;
        }
    }
    return false;
}
void DiscoveryScreen::Input(SDL_Event& event) {
    if (event.key.keysym.sym == SDLK_TAB) {
        if (event.type == SDL_KEYUP) {
            selectedServer += 1;
            if (selectedServer > servers.size()) {
                selectedServer = 0;
            }
        }
    }
    if (event.key.keysym.sym == SDLK_SPACE) {
        if (event.type == SDL_KEYUP) {
            if (servers.size() <= 0) {
                //prevent out of bounds errors by not allowing server to be selected until servers have been found
                return;
            }
            selecting = true;
        }
    }
}