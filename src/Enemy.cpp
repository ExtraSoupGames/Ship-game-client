#include "Enemy.h"
#include "MyGame.h"
#include "Data.h"
void Interpolator::AddToBuffer(DataStream* newData) {
	incomingDataBuffer.push_back(newData);
	std::sort(incomingDataBuffer.begin(), incomingDataBuffer.end(), [](DataStream*& a, DataStream*& b) {return a->timestamp < b->timestamp; });
}
void Interpolator::Interpolate(double currentTime) {
    if (incomingDataBuffer.size() >= 2) {
        if (incomingDataBuffer[1]->timestamp < currentTime) {
            incomingDataBuffer.erase(incomingDataBuffer.begin());
            //if the time is past the second element of incomingDataBuffer, then we dont need the first element for interpolation anymore so we can remove it
        }

        if (incomingDataBuffer.size() >= 2) {
            double snapshotDuration = (incomingDataBuffer[1]->timestamp - incomingDataBuffer[0]->timestamp);
            double snapshotSinceStart = (currentTime - incomingDataBuffer[0]->timestamp);
            double snapshotProgress = snapshotSinceStart / snapshotDuration;
            if (snapshotProgress > 1) { //a new packet has taken too long to arrive, no data is available for where the object should currently be so just stay still
                x = incomingDataBuffer[1]->data->X;
                y = incomingDataBuffer[1]->data->Y;
                OnInterpolate((incomingDataBuffer[1]->data));
            }
            else { //in the range of being able to interpolate
                x = incomingDataBuffer[0]->data->X + ((incomingDataBuffer[1]->data->X - incomingDataBuffer[0]->data->X) * snapshotProgress);
                y = incomingDataBuffer[0]->data->Y + ((incomingDataBuffer[1]->data->Y - incomingDataBuffer[0]->data->Y) * snapshotProgress);
                OnInterpolate((incomingDataBuffer[1]->data));
            }
        }
    }
}
bool Interpolator::HasID(int IDToCompare) {
    return ID == IDToCompare;
}
Interpolator::Interpolator(int pID) {
    ID = pID;
    int x = 0;
    int y = 0;
}
int Enemy::GetID() {
    return ID;
}
void Enemy::Render(SDL_Renderer* renderer) {
    SDL_Rect* enemyRect = new SDL_Rect{x, y, 20, 20};
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    SDL_RenderDrawRect(renderer, enemyRect);
}
EnemyType Enemy::GetEnemyTypeFromBinary(string binaryIn)
{
    if (binaryIn == "000") {
        return LEECH;
    }
    else if (binaryIn == "001") {
        return FLOPPER;
    }
    return FLOPPER;
}
Enemy* Enemy::ProcessEnemy(DataPoint* data, int ID, double timestamp, vector<Enemy*> enemies, TextureManager* t)
{
    return nullptr;
}
Enemy::Enemy(int ID) : Interpolator(ID) {
    width = 20;
    height = 20;
}
void Enemy::OnInterpolate(DataPoint* data)
{
}
Hitbox Enemy::GetHitbox() {
    return *new Hitbox{x, y, width, height};
}
Leech::Leech(int ID, TextureManager* t) : Enemy(ID), Animatable(*new vector<string>{"LeechAttack"}, t) {
    Animatable::PlayAnimation(0);
}
void Leech::Render(SDL_Renderer* renderer) {
    Animatable::Render(renderer, x, y, 20, 20);
}
Enemy* Leech::ProcessEnemy(DataPoint* data, int ID, double timestamp, vector<Enemy*>* enemies, TextureManager* t)
{
    auto it = std::find_if(enemies->begin(), enemies->end(), [&ID](Enemy* e) {return e->HasID(ID); });
    if (it == enemies->end()) {
        //if this is a new enemy
        Leech* newEnemy = new Leech(ID, t);
        newEnemy->AddToBuffer(new DataStream{ data });
        enemies->push_back(newEnemy);
        return newEnemy;
    }
    else {
        Leech* thisEnemy = (Leech*)(*it);
        thisEnemy->AddToBuffer(new DataStream{ data, timestamp });
        return thisEnemy;
    }
}
Flopper::Flopper(int ID, TextureManager* t) : Enemy(ID), Animatable(*new vector<string>{ "LFlopperIdle", "LFlopperFly", "FlopperSpawn" }, t) {
    state = SPAWNING;
}
void Flopper::SetState(string binaryIn)
{
    if (binaryIn == "00") {
        state = GROUNDED;
        Animatable::PlayAnimation(0);
    }
    else if (binaryIn == "01") {
        state = AIRBORNE;
        Animatable::PlayAnimation(1);
    }
    else if (binaryIn == "10") {
        state = SPAWNING;
        Animatable::PlayAnimation(2);
    }
    else {
        throw new exception("Unexpected flopper information received");
    }
}
void Flopper::Render(SDL_Renderer* renderer) {
    //TODO draw the flopper based on the state
    Animatable::Render(renderer, x, y, 20, 20);
}

Enemy* Flopper::ProcessEnemy(DataPoint* data, int ID, double timestamp, vector<Enemy*>* enemies, TextureManager* t)
{
    auto it = std::find_if(enemies->begin(), enemies->end(), [&ID](Enemy* e) {return e->HasID(ID); });
    if (it == enemies->end()) {
        //if this is a new enemy
        Flopper* newEnemy = new Flopper(ID, t);
        newEnemy->AddToBuffer(new DataStream{ data });
        enemies->push_back(newEnemy);
        return newEnemy;
    }
    else {
        Flopper* thisEnemy = (Flopper*)(*it);
        thisEnemy->AddToBuffer(new DataStream{ data, timestamp });
        return thisEnemy;
    }
}

void OtherPlayer::Render(SDL_Renderer* renderer) {
    SDL_Rect* playerRect = new SDL_Rect{ x, y, 20, 20 };
    if (state.attackState == 1) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    }
    SDL_RenderDrawRect(renderer, playerRect);
    Animatable::UpdateAnimation();
    Animatable::Render(renderer, x, y, 30, 30);
}
void OtherPlayer::OnInterpolate(DataPoint* data){
    PlayerData* playerData = (PlayerData*)(data);
    if (state.movementState != playerData->state.movementState) {
        Animatable::PlayAnimation(state.movementState);
    }
    state = playerData->state;
}
OtherPlayer::OtherPlayer(int ID, TextureManager* t) : Interpolator(ID), Animatable(*new vector<string>{"Lwalk", "Lrun", "Ldash"}, t) {
    state = *new PlayerState(0 ,0, 0); // default state is direction 0, standing still = 0, and not attacking = 0
}