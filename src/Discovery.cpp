#include "Discovery.h"
#include "MainMenu.h"
void DiscoveryScreen::ServerClickedEvent()
{
    if (servers.size() <= 0) {
        //prevent out of bounds errors by not allowing server to be selected until servers have been found
        return;
    }
    selecting = true;
}
void DiscoveryScreen::TransferToMainMenu() {
    machine->SwitchState(new MainMenu(machine));
}
DiscoveryScreen::DiscoveryScreen(GameStateMachine* pMachine) : GameState(machine)
{
    selectedServer = 0;
    discoverDelay = 500;
    discoverTimer = 0;
    machine = pMachine;
    font = TTF_OpenFont("PixelatedElegance.ttf", 15);
    UIElements.push_back(new Button("Connect", 25, 10, [this]() {this->ServerClickedEvent(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
    UIElements.push_back(new Button("Back", 25, 30, [this] {this->TransferToMainMenu(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
}
DiscoveryScreen::~DiscoveryScreen() {
    TTF_CloseFont(font);
}
void DiscoveryScreen::Render(SDL_Renderer* renderer) {
    //display
    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    int serverHeight = 50;
    int serverWidth = 100;
    int padding = 10;
    int selectedPadding = 5;
    for (int i = 0; i < servers.size(); i++) {
        SDL_Rect* serverSymbol;
        if (i != selectedServer) {
            serverSymbol = new SDL_Rect{ padding, padding + i * serverWidth, serverWidth - padding * 2, serverHeight - padding * 2 };
        }
        else {
            serverSymbol = new SDL_Rect{ selectedPadding, selectedPadding + i * serverWidth, serverWidth - selectedPadding * 2, serverHeight - selectedPadding * 2};
        }
        SDL_RenderDrawRect(renderer, serverSymbol);
        UIRendering::RenderText(renderer, servers.at(i)->name, 10, 10 + i * 50, font);
    }
    GameState::RenderUI(renderer);
    SDL_RenderPresent(renderer);
}
void DiscoveryScreen::Update(double deltaTime) {
    discoverTimer++; //normally would use SDL_GetTicks(), but accuracy is not important so this innaccurate way is fine
    if (discoverTimer > discoverDelay) {
        discoverTimer -= discoverDelay;
        //send requests
        string discoverRequest = "0000";
        machine->settings->server->SendMessage(discoverRequest);
    }
    if (selecting) {
        machine->settings->server->SetHost(servers[selectedServer]->host, servers[selectedServer]->port);
        machine->settings->server->ResetSocket();
        machine->SwitchState(new MyGame(machine));
    }
}
void DiscoveryScreen::OnReceive(char* inData, int dataLength) {
    string data = ServerManager::CharToBinary(inData, dataLength);
    if (data.substr(0, 4) == "0001") {
        string host = ServerManager::DecompressHost(data.substr(4, 128));
        int port = ServerManager::IntDecompress(data.substr(132, 32));
        string name = ServerManager::DecompressString(data.substr(164, 512));
        name.erase(remove_if(name.begin(), name.end(), [](char c) { return c == '\0'; }), name.end());
        if (!ServerExists(host, port)) {
            ServerHost* newServer = new ServerHost(host, port, name);
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
    UIInput(event);
    if (event.key.keysym.sym == SDLK_TAB) {
        if (event.type == SDL_KEYUP) {
            selectedServer += 1;
            if (selectedServer > servers.size()) {
                selectedServer = 0;
            }
        }
    }
}

void DiscoveryScreen::OnEnter()
{
}

void DiscoveryScreen::OnExit()
{
}