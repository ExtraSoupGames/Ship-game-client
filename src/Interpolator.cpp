#include "Interpolator.h"
#include "MyGame.h"
#include "Data.h"
#pragma region Interpolator
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
    x = 0;
    y = 0;
}
#pragma endregion Interpolator
#pragma region Enemy
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
#pragma endregion Enemy
#pragma region Bobleech
Bobleech::Bobleech(int ID, TextureManager* t) : Enemy(ID), Animatable(*new vector<string>{"LeechAttack"}, t) {
    Animatable::PlayAnimation(0);
}
void Bobleech::Render(SDL_Renderer* renderer) {
    Animatable::Render(renderer, x, y, 20, 20);
}
#pragma endregion Bobleech
#pragma region Flopper
Flopper::Flopper(int ID, TextureManager* t) : Enemy(ID), Animatable(*new vector<string>{ "%FlopperIdle", "%FlopperFly", "FlopperSpawn" }, t) {
    state = SPAWNING;
}
void Flopper::OnInterpolate(DataPoint* data) {
    FlopperData* flopperData = (FlopperData*)data;
    state = flopperData->state;
    PlayAnimation(state);
}
void Flopper::Render(SDL_Renderer* renderer) {
    //TODO draw the flopper based on the state
    Animatable::UpdateAnimation();
    Animatable::Render(renderer, x, y, 30, 30);
}
#pragma endregion Flopper
#pragma region Clingabing
void Clingabing::OnInterpolate(DataPoint* data)
{
}

Clingabing::Clingabing(int ID, TextureManager* t) : Enemy(ID), Animatable(*new vector<string>{}, t) // TODO add animation names
{
    state = IDLE;
}

void Clingabing::Render(SDL_Renderer* renderer)
{
    //TODO add clingabing frames and animate
    SDL_Rect* clingabingRect = new SDL_Rect{x, y, 15, 15};
    SDL_RenderDrawRect(renderer, clingabingRect);
}
#pragma endregion Clingabing
#pragma region OtherPlayer
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
OtherPlayer::OtherPlayer(int ID, TextureManager* t) : Interpolator(ID), Animatable(*new vector<string>{"%walk", "%run", "%dash"}, t) {
    state = *new PlayerState(0 ,0, 0); // default state is direction 0, standing still = 0, and not attacking = 0
}
#pragma endregion OtherPlayer