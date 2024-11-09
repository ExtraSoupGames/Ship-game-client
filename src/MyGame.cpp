#include "MyGame.h"


void DebugMessages(double t, vector<DataStream*> d, double n, double o) {
    std::cout << "timern: " << t << std::endl;
    for (DataStream* da : d) {
        std::cout << da->timestamp << " - ";
    }
    std::cout << std::endl;
    std::cout << "oldTimestamp" << o << " newTimestamp: " << n << std::endl;
    std::cout << std::endl << std::endl;
}
bool Hitbox::Collides(Hitbox& other) {
    if (other.x + other.w < x || x + w < other.x) {
        return false;
    }
    if (other.y + other.h < y || y + w < other.y) {
        return false;
    }
    return true;
}

MyGame::MyGame(int pClientID, UDPsocket* serverSocket, SDL_Renderer* gameRenderer) {
    clientConnectTime = 0;
    serverStartTime = 0;
    broadcastTimer = 0;
    clientID = pClientID;
    server = new ServerManager(serverSocket);
    renderer = gameRenderer;
    textureManager = new TextureManager(renderer);
    collisions = new CollisionManager();
    playerController = new PlayerController(textureManager);
    textureManager->InitializeAllTextures();

    enemies = new vector<Enemy*>();
    players = new vector<OtherPlayer*>();


    //test animations
    string animationName = "anim";
    vector<string> animations = *new vector<string>{ animationName };
    number = new Animatable(animations, textureManager);
    number->PlayAnimation(0);
}
#pragma region incomingData
void MyGame::HandleEnemyData(string message) {
    int enemyPacketSize = 53;
    if (!((message.size() - 3 - 64) % enemyPacketSize) == 0 && message.size() > 1) { // size should be 3 for header + a multiple of 32 for the ID + 16 for the position per enemy + 64 for timestamp
        int padding = (message.size() - 3 - 64) % enemyPacketSize;
        message = message.substr(0, message.size() - padding); // remove the padding
    }

    double timestamp = server->TimestampDecompress(message.substr(message.size() - 64, 64)); // the timestamp is the final piece of data

    string enemyData = message.substr(3, message.size() - 67);
    vector<int> aliveEnemyIDs = *new vector<int>();
    if (enemyData.size() == 0) { // packet contains only header and timestamp - all enemies are deas
        enemies = new vector<Enemy*>;
        return;
    }
    for (int i = 0; i < (enemyData.size() - (enemyPacketSize - 1)); i += enemyPacketSize) { //iterate through each enemies data (each enemy has 48 bits to store position and ID
        EnemyType enemyType = Enemy::GetEnemyTypeFromBinary(enemyData.substr(i, 3));
        int ID = server->IntDecompress(enemyData.substr(i+3, 32));
        int* position = server->PositionDecompress(enemyData.substr(i + 35, 16));
        int X = *position;
        int Y = *(position + 1);
        string ExtraData = enemyData.substr(i + 51, 2);
        aliveEnemyIDs.push_back(ID);
        // find the enemy with the given ID, if there is one
        switch (enemyType) {
        case LEECH:
            //no extra data for leech
            Leech::ProcessEnemy(new DataPoint{X, Y}, ID, timestamp, enemies, textureManager);
            break;
        case FLOPPER:
            //process extra 2 bits of information
            Flopper* f = (Flopper*)Flopper::ProcessEnemy(new DataPoint{ X, Y }, ID, timestamp, enemies, textureManager);
            f->SetState(ExtraData);
            break;
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
    if (!((message.size() - 3 - 64) % 55) == 0 && message.size() > 1) { // size should be 3 for header + a multiple of 32 for the ID + 16 for the position + 7 for the state per player + 64 for timestamp
        int padding = (message.size() - 3 - 64) % 55;
        message = message.substr(0, message.size() - padding); // remove the padding
    }
    double timestamp = server->TimestampDecompress(message.substr(message.size() - 64, 64)); // the timestamp is the final piece of data

    //the first packet that comes from the server will be the base start time offset for all other packets
    if (serverStartTime < 100) {
        serverStartTime = timestamp;
        clientConnectTime = SDL_GetTicks();
    }
    string playerData = message.substr(3, (message.size() - 3 - 64));
    for (int i = 0; i < (playerData.size() - 54); i += 55) { //iterate through each player data (each player has 3 args: ID, X, Y)
        int ID = server->IntDecompress(playerData.substr(i, 32));
        if (ID == clientID) {
            continue; // this player's movement is handled by its own playercontroller
        }
        int* position = server->PositionDecompress(playerData.substr(i + 32, 16));
        int X = *position;
        int Y = *(position + 1);
        PlayerState state = server->PlayerStateDecompress(playerData.substr(i+48, 7));
        if (ID != clientID) {
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
    if ((message.size() - 3 - 64) % 192 != 0 || message.size() < 1) {
        // size should be 3 for header, then a multiple of 6x32 = 192 for 6 32 bit integers, and 64 for the timestamp
        cout << "unexpected boundary data received" << endl;
        cout << "boundary data size: " << message.size();
        return;
    }
    int messageDataLength = message.size() - 3 - 64;
    for (int i = 3; i < messageDataLength - 5 * 32; i += 6 * 32) {
        double p1x = server->IntDecompress(message.substr(i, 32));
        double p1y = server->IntDecompress(message.substr(i + 32, 32));
        double p2x = server->IntDecompress(message.substr(i + 64, 32));
        double p2y = server->IntDecompress(message.substr(i + 96, 32));
        double ofx = server->IntDecompress(message.substr(i + 128, 32));
        double ofy = server->IntDecompress(message.substr(i + 160, 32));
        collisions->AddBoundary(*new CollisionBoundary(p1x, p1y, p2x, p2y, ofx, ofy));
    }
}
void MyGame::on_receive(char* data, int messagelength) {
    string message = server->CharToBinary(data, messagelength);
    string messageType = message.substr(0, 3); // first 3 bits denote type of data in packet
    if (messageType == "100") { // enemy positions code
        HandleEnemyData(message);
    }
    if (messageType == "011") { // player data code
        HandlePlayerData(message);
    }
    if (messageType == "001") { // boundary data code
        HandleBoundaryData(message);
        cout << "Game Initiated!" << endl;
    }
}
#pragma endregion incomingData

void MyGame::input(SDL_Event& event) {
    playerController->HandleInput(event, this);
}

void MyGame::update(double deltaTime) {
#pragma region boundaryRequests
    if (collisions->IsEmpty()) {
        stringstream binaryText;
        binaryText << "000"; // this is a network join request code
        server->SendMessage(binaryText.str()); // this means the header (first 3 bits) will be 000 which corresponds to the boundary request code
        return;
    }
#pragma endregion boundaryRequests
#pragma region playerDataOut
    broadcastTimer += deltaTime;
    if (broadcastTimer > broadcastSpacing) {
        broadcastTimer -= broadcastSpacing;
        // Prepare player data
        stringstream binaryText;
        binaryText << "010" << 
            server->IntCompress(clientID) <<
            server->PositionCompress((int)playerController->xPos, (int)playerController->yPos) << 
            server->PlayerStateCompress(playerController->GetState());
        // Send player data
        server->SendMessage(binaryText.str());
    }
#pragma endregion playerDataOut
#pragma region playerProcessing
    playerController->UpdateMove(collisions, deltaTime);
    double delay = 100;  //snapshot buffer should be 3-4x base rate of packets - this way we can lose 2 packets and not experience jittering
    double timern = serverStartTime + SDL_GetTicks() - delay - clientConnectTime;
    for (Enemy* e : *enemies) {
        e->Interpolate(timern);
    }
    for (OtherPlayer* p : *players) {
        p->Interpolate(timern);
    }
#pragma endregion playerProcessing
#pragma region animationProcessing
    number->UpdateAnimation();
#pragma endregion animationProcessing
}

void MyGame::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderClear(renderer);

    for (Enemy* e : *enemies) {
        e->Render(renderer);
    }
    for (OtherPlayer* p : *players) {
        p->Render(renderer);
    }
    playerController->Render(renderer);

    //number->Render(renderer);

    collisions->DrawDebug(renderer);
    SDL_RenderPresent(renderer);
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
    binaryText << "101" << server->IntCompress(enemyDamaged->GetID()) << server->IntCompress(damage) << server->IntCompress(knockback);
    // Send hit data
    server->SendMessage(binaryText.str());
}