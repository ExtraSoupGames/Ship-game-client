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
void Enemy::Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY) {
    SDL_Rect* enemyRect = new SDL_Rect{x - camOffX, y - camOffY, 20, 20};
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    SDL_RenderDrawRect(renderer, enemyRect);
}
void Enemy::ResetAttackTimestamp()
{
    lastAttackTimestamp = SDL_GetTicks();
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
    lastAttackTimestamp = 0;
}
void Enemy::OnInterpolate(DataPoint* data)
{
}
#pragma endregion Enemy
#pragma region Bobleech
Bobleech::Bobleech(int ID, TextureManager* t) : Enemy(ID), Animatable(*new vector<string>{"%Bobleech", "%Bobleech"}, t) {
    Animatable::PlayAnimation(0);
    width = 16;
    height = 16;
}
Hitbox Bobleech::GetHitbox()
{
    return Hitbox{x + 2, y + 7, 12, 8};
}
void Bobleech::Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY) {
    Animatable::Render(renderer, x - camOffX, y - camOffY, 16, 16, settings);
    Animatable::UpdateAnimation();
}
bool Bobleech::IsAttacking()
{
    double timeSinceAttack = SDL_GetTicks() - lastAttackTimestamp;
    return (timeSinceAttack > 1000); // bobleech is always ready to attack
}
EnemyAttackData Bobleech::GetAttackData()
{
    return EnemyAttackData(x, y, 3, 0.2f);
}
#pragma endregion Bobleech
#pragma region Flopper
Flopper::Flopper(int ID, TextureManager* t) : Enemy(ID), Animatable(*new vector<string>{ "%FlopperIdle", "FlopperLaunch", "FlopperSpawn", "%FlopperFly"}, t, 3) {//set flopper fly as default so after launching it goes to that TODO add queueing system
    state = SPAWNING;
    width = 32;
    height = 32;
}
Hitbox Flopper::GetHitbox()
{
    return Hitbox{x + 3, y + 10, 26, 19};
}
void Flopper::OnInterpolate(DataPoint* data) {
    FlopperData* flopperData = (FlopperData*)data;
    if (state != flopperData->state) {
        state = flopperData->state;
        PlayAnimation(state);
    }
}
void Flopper::Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY) {
    Animatable::UpdateAnimation();
    Animatable::Render(renderer, x - camOffX, y - camOffY, 32, 32, settings);
}
bool Flopper::IsAttacking()
{
    double timeSinceAttack = SDL_GetTicks() - lastAttackTimestamp;
    return (timeSinceAttack > 2000) && state == IDLE; // flopper doesnt damage player when flying or spawning
}
EnemyAttackData Flopper::GetAttackData()
{
    return EnemyAttackData(x, y, 10, 0.5f);
}
#pragma endregion Flopper
#pragma region Clingabing
void Clingabing::OnInterpolate(DataPoint* data)
{
}

Clingabing::Clingabing(int ID, TextureManager* t) : Enemy(ID), Animatable(*new vector<string>{}, t) // TODO add animation names
{
    state = IDLE;
    width = 32;
    height = 32;
}

Hitbox Clingabing::GetHitbox()
{
    return Hitbox();
}

void Clingabing::Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY)
{
    //TODO add clingabing frames and animate
    SDL_Rect* clingabingRect = new SDL_Rect{(x - camOffX) * settings->screenScaling(), (y - camOffY) * settings->screenScaling(), 15, 15};
    SDL_RenderDrawRect(renderer, clingabingRect);
}
bool Clingabing::IsAttacking()
{
    double timeSinceAttack = SDL_GetTicks() - lastAttackTimestamp;
    return timeSinceAttack > 1000; //clingabing is always ready to attack
}
EnemyAttackData Clingabing::GetAttackData()
{
    return EnemyAttackData(x, y, 5, 0);
}
#pragma endregion Clingabing
#pragma region OtherPlayer
void OtherPlayer::Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY) {
    if (isAlive) {
        SDL_Rect* playerRect = new SDL_Rect{ (x - camOffX) * settings->screenScaling(), (y - camOffY) * settings->screenScaling(), 20, 20 };
        if (state.attackState == 1) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        SDL_RenderDrawRect(renderer, playerRect);
        Animatable::UpdateAnimation();
        Animatable::Render(renderer, x - camOffX, y - camOffY, 16, 32, settings);
        switch (state.movementState) {
        case 0:
            //stood still
            Animatable::PlayAnimation(0);
            break;
        case 2:
            //dashing
            Animatable::PlayAnimation(4);
            break;
        case 1:
            //walking
            Animatable::PlayAnimation(state.animationState);
            break;
        }
    }
}
void OtherPlayer::CheckColour(string newPalette, TextureManager* t)
{
    if (newPalette != colourPalette) {
        colourPalette = newPalette;
        Animatable::ReloadAllFrames(*new vector<string>{ "%CatStraight", "%CatLeft", "%CatRight", "%CatUp", "%CatDash" }, t, "Cats\\Cat" + colourPalette);
    }
}
void OtherPlayer::OnInterpolate(DataPoint* data){
    PlayerData* playerData = (PlayerData*)(data);
    state = playerData->state;
}
OtherPlayer::OtherPlayer(int ID, TextureManager* t, string playerPalette) : Interpolator(ID), Animatable(*new vector<string>{ "%CatStraight", "%CatLeft", "%CatRight", "%CatUp", "%CatDash"}, t, "Cats\\Cat" + playerPalette) {
    state = *new PlayerState(0 ,0, 0, 0); // default state is direction 0, standing still = 0, and not attacking = 0, and animation state of 0
    isAlive = true;
    colourPalette = playerPalette;
}
#pragma endregion OtherPlayer