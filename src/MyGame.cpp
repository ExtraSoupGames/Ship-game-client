#include "MyGame.h"
#include "MainMenu.h"
#include "GameOver.h"
bool Hitbox::Collides(Hitbox& other) {
    if (other.x + other.w < x || x + w < other.x) {
        return false;
    }
    if (other.y + other.h < y || y + w < other.y) {
        return false;
    }
    return true;
}

MyGame::MyGame(GameStateMachine* pMachine) : PlayerGameState(pMachine){
    collisions = new CollisionManager();
    playerController = new PlayerController(machine, collisions);

    enemies = new vector<Enemy*>();

    cameraOffsetX = 0;
    cameraOffsetY = 0;
}
MyGame::~MyGame() {
    delete collisions;
    delete playerController;
    delete enemies;
}
void MyGame::AdjustCamera() {
    int playerScreenX = playerController->GetXForServer() - cameraOffsetX;
    int playerScreenY = playerController->GetYForServer() - cameraOffsetY;
    int playerWidth = playerController->GetWidth();
    int playerHeight = playerController->GetHeight();
    float screenRatio = 0.6; // the amount of screen (0 - 1) that the player occupies
    float minRatio = 0.5 - screenRatio / 2;
    float maxRatio = 0.5 + screenRatio / 2;
    int screenWidthScaled = machine->settings->screenWidth / machine->settings->screenScaling();
    int screenHeightScaled = machine->settings->screenHeight / machine->settings->screenScaling();
    int maximumX = screenWidthScaled * maxRatio;
    int minimumX = screenWidthScaled * minRatio;
    int maximumY = screenHeightScaled * maxRatio;
    int minimumY = screenHeightScaled * minRatio;
    if (playerScreenX < minimumX) {
        cameraOffsetX -= 1;
    }
    else if (playerScreenX + playerWidth > maximumX) {
        cameraOffsetX += 1;
    }
    if (playerScreenY < minimumY) {
        cameraOffsetY -= 1;
    }
    else if (playerScreenY + playerHeight > maximumY) {
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
        Enemy* newEnemy = new T(ID, machine->settings->textureManager);
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
        collisions->AddBoundary(new CollisionBoundary(p1x, p1y, p2x, p2y, ofx, ofy));
    }
}
#pragma endregion incomingDataProcessing
void MyGame::OnReceive(char* data, int messagelength) {
    string message = machine->settings->server->CharToBinary(data, messagelength);
    string messageType = message.substr(0, 4); // first 4 bits denote type of data in packet
    message = message.substr(4);
    if (messageType == "0011") { // enemy positions code
        HandleEnemyData(message);
    }
    if (messageType == "0010") { // player data code
        HandlePlayerData(message, players);
    }
    if (messageType == "0001") { // boundary data code
        HandleBoundaryData(message);
        cout << "Game Initiated!" << endl;
    }
    if (messageType == "1110") { // important message confirmation from server code
        machine->settings->server->ReceiveImportantMessageConfirmation(message);
    }
    if (messageType == "0100") { // game over code
        //TODO process incoming game report data
        GameReport* report = new GameReport(); // add processing here
        machine->SwitchState(new GameOver(machine, report));
    }
    if (messageType == "0111") {
        int time = machine->settings->server->IntDecompress(message);
        cout << "current game time: " << time << endl;
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
        binaryText << "0001"; // this is a boundary data request code
        machine->settings->server->SendMessage(binaryText.str());
        cout << "Requesting boundary info" << endl;
        cout << "machine server settings: " << machine->settings->server->ToString() << endl;
        return;
    }
#pragma endregion boundaryRequests
    BroadcastPlayerData(deltaTime, playerController);
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

    collisions->DrawDebug(renderer, cameraOffsetX, cameraOffsetY, machine->settings->screenScaling());
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
    binaryText << "0011" << 
        machine->settings->server->IntCompress(enemyDamaged->GetID()) << 
        machine->settings->server->IntCompress(damage) << 
        machine->settings->server->IntCompress(knockback);
    // Send hit data
    machine->settings->server->SendMessage(binaryText.str());
}