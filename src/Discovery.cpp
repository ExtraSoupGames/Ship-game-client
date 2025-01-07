#include "Discovery.h"
#include "MainMenu.h"
#include "StartRoom.h"
void DiscoveryScreen::ServerClickedEvent()
{
    if (selectedServer == 0) {
        return;
    }
    selecting = true;
}
void DiscoveryScreen::TransferToMainMenu() {
    machine->SwitchState(new MainMenu(machine));
}
DiscoveryScreen::DiscoveryScreen(GameStateMachine* pMachine) : GameState(machine)
{
    selecting = false;
    selectedServer = 0;
    discoverDelay = 500;
    discoverTimer = 0;
    machine = pMachine;
    font = TTF_OpenFont("PixelatedElegance.ttf", 15);
    selectedServerText = new TextDisplay(machine->settings->textureManager, 25, 50, machine->settings->screenScaling(), "Selected: none");
    UIElements.push_back(selectedServerText);
    connectButton = new Button("No server selected", 25, 10, [this]() {this->ServerClickedEvent(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25);
    UIElements.push_back(connectButton);
    connectButton->Disable();
    UIElements.push_back(new Button("Back", 25, 30, [this] {this->TransferToMainMenu(); }, machine->settings->textureManager, machine->settings->screenScaling(), 25));
}
DiscoveryScreen::~DiscoveryScreen() {
    TTF_CloseFont(font);
}
void DiscoveryScreen::Render(SDL_Renderer* renderer) {
    //display
    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
    SDL_RenderClear(renderer);
    GameState::RenderUI(renderer);
    SDL_RenderPresent(renderer);
}
void DiscoveryScreen::Update(double deltaTime) {
    GameState::UIUpdate();
    discoverTimer+= deltaTime; //normally would use SDL_GetTicks(), but accuracy is not important so this innaccurate way is fine
    if (discoverTimer > discoverDelay) {
        discoverTimer -= discoverDelay;
        //send requests
        string discoverRequest = "0000"; // client request info code
        machine->settings->server->SendMessage(discoverRequest);
    }
    if (selecting && selectedServer != 0) {
        machine->settings->server->SetHost(servers[selectedServer - 1]->host, servers[selectedServer - 1]->port);
        machine->settings->server->ResetSocket();
        machine->SwitchState(new StartRoom(machine));
    }
}
void DiscoveryScreen::OnReceive(char* inData, int dataLength) {
    string data = ServerManager::CharToBinary(inData, dataLength);
    try {
        if (data.substr(0, 4) == "0000") { // server information code   
            string host = ServerManager::DecompressHost(data.substr(4, 128));
            int port = ServerManager::IntDecompress(data.substr(132, 32));
            string name = ServerManager::DecompressString(data.substr(164, 512));
            name.erase(remove_if(name.begin(), name.end(), [](char c) { return c == '\0'; }), name.end());
            if (!ServerExists(host, port)) {
                ServerHost* newServer = new ServerHost(host, port, name);
                servers.push_back(newServer);
                cout << "creating server at size: " << servers.size() << endl;
                UIElements.push_back(new Button(name, 25, 50 + servers.size() * 20,
                    [this] {this->SelectServer(servers.size()); },
                    machine->settings->textureManager,
                    machine->settings->screenScaling(),
                    25));
            }
        }
    }
    catch (exception){
        cout << "Bad discover info sent" << endl;
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
}
void DiscoveryScreen::SelectServer(int serverID) {
    selectedServer = serverID;
    connectButton->Enable();
    connectButton->SetText("Connect");
    selectedServerText->SetText("Selected: " + servers.at(serverID - 1)->name);
}
void DiscoveryScreen::OnEnter()
{
}

void DiscoveryScreen::OnExit()
{
}