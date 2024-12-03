#include "MyGame.h"
#include "MainMenu.h"
bool Hitbox::Collides(Hitbox& other) {
    if (other.x + other.w < x || x + w < other.x) {
        return false;
    }
    if (other.y + other.h < y || y + w < other.y) {
        return false;
    }
    return true;
}

MyGame::MyGame(GameStateMachine* pMachine) : GameState(pMachine){
    serverStartTime = 0;
    clientServerTimeDiff = 0;
    broadcastTimer = 0;
    textureManager = new TextureManager(machine->settings->renderer);
    collisions = new CollisionManager();
    playerController = new PlayerController(textureManager, collisions);
    textureManager->InitializeAllTextures();

    enemies = new vector<Enemy*>();
    players = new vector<OtherPlayer*>();

    pMachine = machine;

    cameraOffsetX = 0;
    cameraOffsetY = 0;
}
MyGame::~MyGame() {
    delete textureManager;
    delete collisions;
    delete playerController;
    delete enemies;
    delete players;
}
void MyGame::AdjustCamera() {
    int playerScreenX = playerController->GetXForServer() - cameraOffsetX;
    int playerScreenY = playerController->GetYForServer() - cameraOffsetY;
    float screenRatio = 0.5; // the amount of screen (0 - 1) that the player occupies
    float minRatio = 0.5 - screenRatio / 2;
    float maxRatio = 0.5 + screenRatio / 2;
    int maximumX = machine->settings->screenWidth * maxRatio;
    int minimumX = machine->settings->screenWidth * minRatio;
    int maximumY = machine->settings->screenHeight * maxRatio;
    int minimumY = machine->settings->screenHeight * minRatio;
    if (playerScreenX < minimumX) {
        cameraOffsetX -= 1;
    }
    else if (playerScreenX > maximumX) {
        cameraOffsetX += 1;
    }
    if (playerScreenY < minimumY) {
        cameraOffsetY -= 1;
    }
    else if (playerScreenY > maximumY) {
        cameraOffsetY += 1;
    }
}

#pragma region incomingData
#pragma region incomingDataProcessing
template <typename T> Enemy* MyGame::ProcessEnemy(DataPoint* data, int ID, double timestamp)
{
    auto it = std::find_if(enemies->begin(), enemies->end(), [&ID](Enemy* e) {return e->HasID(ID); });
    if (it == enemies->end()) {
        //if this is a new enemy
        Enemy* newEnemy = new T(ID, textureManager);
        newEnemy->AddToBuffer(new DataStream{ data });
        enemies->push_back(newEnemy);
        return newEnemy;
    }
    else {
        Enemy* thisEnemy = (Enemy*)(*it);
        thisEnemy->AddToBuffer(new DataStream{ data, timestamp });
        return thisEnemy;
    }
}
void MyGame::HandleEnemyData(string message) {
    int enemyPacketSize = 53;
    if (!((message.size() - 64) % enemyPacketSize) == 0 && message.size() > 1) { // size should be a multiple of 32 for the ID + 16 for the position per enemy + 64 for timestamp
        int padding = (message.size() - 64) % enemyPacketSize;
        message = message.substr(0, message.size() - padding); // remove the padding
    }

    double timestamp = machine->settings->server->TimestampDecompress(message.substr(message.size() - 64, 64)); // the timestamp is the final piece of data

    string enemyData = message.substr(0, message.size() - 64);
    vector<int> aliveEnemyIDs = *new vector<int>();
    if (enemyData.size() == 0) { // packet contains only header and timestamp - all enemies are deas
        enemies = new vector<Enemy*>;
        return;
    }
    for (int i = 0; i < (enemyData.size() - (enemyPacketSize - 1)); i += enemyPacketSize) { //iterate through each enemies data (each enemy has 48 bits to store position and ID
        //get the information about the enemy
        EnemyType enemyType = Enemy::GetEnemyTypeFromBinary(enemyData.substr(i, 3));
        int ID = machine->settings->server->IntDecompress(enemyData.substr(i+3, 32));
        int* position = machine->settings->server->PositionDecompress(enemyData.substr(i + 35, 16));
        int X = *position;
        int Y = *(position + 1);
        string extraData = enemyData.substr(i + 51, 2);
        aliveEnemyIDs.push_back(ID);
        //process the data for the enemy, depending on the type of enemy
        switch (enemyType) {
        case LEECH:
            ProcessEnemy<Bobleech>(new BobleechData(X, Y), ID, timestamp);
            break;
        case FLOPPER:
            ProcessEnemy<Flopper>(new FlopperData(X, Y, extraData), ID, timestamp);
            break;
        case CLINGABING:
            ProcessEnemy<Clingabing>(new ClingabingData(X, Y, extraData), ID, timestamp);
        }
    }
    //remove any enemies that didnt have any data sent for them as they are dead
    int enemyIDToRemove = -1;
    for (Enemy* e : *enemies) {
        if (find(aliveEnemyIDs.begin(), aliveEnemyIDs.end(), e->GetID()) == aliveEnemyIDs.end()) {
            enemyIDToRemove = e->GetID();
        }
    }
    if (enemyIDToRemove != -1) {
        enemies->erase(remove_if(enemies->begin(), enemies->end(), [&enemyIDToRemove](Enemy* e) {return e->HasID(enemyIDToRemove); }));
    }
}
void MyGame::HandlePlayerData(string message) {
    if (!((message.size() - 64) % 55) == 0 && message.size() > 1) { // size shoule be a multiple of 32 for the ID + 16 for the position + 7 for the state per player + 64 for timestamp
        int padding = (message.size() - 64) % 55;
        message = message.substr(0, message.size() - padding); // remove the padding
    }
    double timestamp = machine->settings->server->TimestampDecompress(message.substr(message.size() - 64, 64)); // the timestamp is the final piece of data

    //the first packet that comes from the server will be the base start time offset for all other packets
    if (serverStartTime < 100) {
        serverStartTime = timestamp;
        clientServerTimeDiff = serverStartTime - SDL_GetTicks();
    }
    string playerData = message.substr(0, (message.size() - 64));
    for (int i = 0; i < (playerData.size() - 54); i += 55) { //iterate through each player data (each player has 3 args: ID, X, Y)
        int ID = machine->settings->server->IntDecompress(playerData.substr(i, 32));
        if (ID == machine->settings->clientID) {
            continue; // this player's movement is handled by its own playercontroller
        }
        int* position = machine->settings->server->PositionDecompress(playerData.substr(i + 32, 16));
        int X = *position;
        int Y = *(position + 1);
        PlayerState state = machine->settings->server->PlayerStateDecompress(playerData.substr(i+48, 7));
        if (ID != machine->settings->clientID) {
            //ignore the incoming data about this client as the client has authority on it's own player's movement

            auto it = std::find_if(players->begin(), players->end(), [&ID](OtherPlayer* e) {return e->HasID(ID); });
            if (it == players->end()) {
                //if this is a new other player
                OtherPlayer* newPlayer = new OtherPlayer(ID, textureManager);
                newPlayer->PlayAnimation(0);
                newPlayer->AddToBuffer(new DataStream({ new PlayerData(X, Y, state), timestamp }));
                players->push_back(newPlayer);
            }
            else {
                OtherPlayer* player = *it;
                player->AddToBuffer(new DataStream({ new PlayerData(X, Y, state), timestamp}));
            }
        }
    }
}
void MyGame::HandleBoundaryData(string message) {
    //cout << "Boundary data received" << endl;
    if (!collisions->IsEmpty()) {
        //only needed once to initialize game boundaries
        return;
    }
    if ((message.size() - 64) % 192 != 0 || message.size() < 1) {
        // size should be a multiple of 6x32 = 192 for 6 32 bit integers, and 64 for the timestamp
        cout << "unexpected boundary data received" << endl;
        cout << "boundary data size: " << message.size();
        message = message.substr(0, message.size() - (message.size()) % 7); // trim the padding
    }
    int messageDataLength = message.size() - 64;
    for (int i = 0; i < messageDataLength - 5 * 32; i += 6 * 32) {
        double p1x = machine->settings->server->IntDecompress(message.substr(i, 32));
        double p1y = machine->settings->server->IntDecompress(message.substr(i + 32, 32));
        double p2x = machine->settings->server->IntDecompress(message.substr(i + 64, 32));
        double p2y = machine->settings->server->IntDecompress(message.substr(i + 96, 32));
        double ofx = machine->settings->server->IntDecompress(message.substr(i + 128, 32));
        double ofy = machine->settings->server->IntDecompress(message.substr(i + 160, 32));
        collisions->AddBoundary(*new CollisionBoundary(p1x, p1y, p2x, p2y, ofx, ofy));
    }
}
#pragma endregion incomingDataProcessing
void MyGame::OnReceive(char* data, int messagelength) {
    string message = machine->settings->server->CharToBinary(data, messagelength);
    string messageType = message.substr(0, 4); // first 4 bits denote type of data in packet
    message = message.substr(4);
    if (messageType == "1000") { // enemy positions code
        HandleEnemyData(message);
    }
    if (messageType == "0101") { // player data code
        HandlePlayerData(message);
    }
    if (messageType == "0011") { // boundary data code
        HandleBoundaryData(message);
        cout << "Game Initiated!" << endl;
    }
    if (messageType == "1010") { // important message confirmation from server code
        machine->settings->server->ReceiveImportantMessageConfirmation(message);
    }
    if (messageType == "1100") { // impotant message received from server, send confirmation, just for testing
        machine->settings->server->SendImportantMessageConfirmation(message, machine->settings->clientID);
    }
}
#pragma endregion incomingData

void MyGame::Input(SDL_Event& event) {
    playerController->HandleInput(event, this);
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_p) {
            machine->SwitchState(new MainMenu(machine));
        }
    }
}
void MyGame::Update(double deltaTime) {
#pragma region boundaryRequests
    if (collisions->IsEmpty()) {
        stringstream binaryText;
        binaryText << "0010"; // this is a boundary data request code
        machine->settings->server->SendMessage(binaryText.str());
        return;
    }
#pragma endregion boundaryRequests
#pragma region playerDataOut
    broadcastTimer += deltaTime;
    if (broadcastTimer > broadcastSpacing) {
        broadcastTimer -= broadcastSpacing;
        // Prepare player data
        stringstream binaryText;
        binaryText << "0100" << 
            machine->settings->server->IntCompress(machine->settings->clientID) <<
            machine->settings->server->PositionCompress(playerController->GetXForServer(), playerController->GetYForServer()) <<
            machine->settings->server->PlayerStateCompress(playerController->GetState());
        // Send player data
        machine->settings->server->SendMessage(binaryText.str());
    }
#pragma endregion playerDataOut
    AdjustCamera();
#pragma region playerProcessing
    playerController->UpdateMove(deltaTime, machine->settings->screenScaling());
    playerController->UpdateEnemyAttacks(this);
    double delay = 100;  //snapshot buffer should be 3-4x base rate of packets - this way we can lose 2 packets and not experience jittering
    double timern = clientServerTimeDiff + SDL_GetTicks() - delay;
    //cout << "ClientServerDiff: " << clientServerTimeDiff << endl;
    //cout << "Time right now: " << timern << endl;
    for (Enemy* e : *enemies) {
        e->Interpolate(timern);
    }
    for (OtherPlayer* p : *players) {
        p->Interpolate(timern);
    }
#pragma endregion playerProcessing
#pragma region serverUpdates
    serverBroadcastTimer += deltaTime;
    if (serverBroadcastTimer > serverBroadcastDelay) {
        serverBroadcastTimer -= serverBroadcastDelay;
        machine->settings->server->SendAllImportantMessages();
    }
#pragma endregion serverUpdates
}
void MyGame::Render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderClear(renderer);

    for (Enemy* e : *enemies) {
        e->Render(renderer, machine->settings, cameraOffsetX, cameraOffsetY);
    }
    for (OtherPlayer* p : *players) {
        p->Render(renderer, machine->settings, cameraOffsetX, cameraOffsetY);
    }
    playerController->Render(renderer, machine->settings, cameraOffsetX, cameraOffsetY);

    collisions->DrawDebug(renderer, cameraOffsetX, cameraOffsetY);
    GameState::RenderUI(renderer);
    SDL_RenderPresent(renderer);
}
void MyGame::OnEnter()
{
}
void MyGame::OnExit()
{
}

vector<Enemy*> MyGame::GetCollidingEnemies(Hitbox area)
{
    vector<Enemy*> collidingEnemies = *new vector<Enemy*>();
    for (Enemy* e : *enemies) {
        if (e->GetHitbox().Collides(area)) {
            collidingEnemies.push_back(e);
        }
    }
    return collidingEnemies;
}

void MyGame::SendEnemyDamageMessage(Enemy* enemyDamaged, int damage) {
    stringstream binaryText;
    int knockback = 0;
    binaryText << "0110" << 
        machine->settings->server->IntCompress(enemyDamaged->GetID()) << 
        machine->settings->server->IntCompress(damage) << 
        machine->settings->server->IntCompress(knockback);
    // Send hit data
    machine->settings->server->SendMessage(binaryText.str());
}