#include "PlayerController.h"
#include "MyGame.h"
#pragma region InputMapping
Vector2 InputMapping::GetDirectionFacing(Vector2 playerPos) {
    Vector2 facing = mousePos - playerPos;
    double playerFacingAngle = SDL_atan2(facing.y, facing.x);
    playerFacingAngle = (playerFacingAngle * 4) / M_PI;
    playerFacingAngle = round(playerFacingAngle);
    playerFacingAngle = (playerFacingAngle * M_PI) / 4;
    Vector2 directionVector = Vector2(cos(playerFacingAngle), sin(playerFacingAngle));
    return directionVector;
}
Vector2 InputMapping::GetDirectionMoving() {
    return Vector2(rightPressed - leftPressed, downPressed - upPressed);
}
bool InputMapping::IsStill() {
    return !(upPressed || downPressed || rightPressed || leftPressed);
}
int InputMapping::GetDirectionState(Vector2 playerPos) {
    Vector2 directionVec = GetDirectionFacing(playerPos).Normalise();
    double angle = atan2(directionVec.x, directionVec.y);
    //atan2 returns in range [-pi - pi] so convert to [0 - 2pi]
    if (angle < 0) {
        angle += M_PI * 2;
    }
    int returnState = static_cast<int>((angle + (M_PI / 8.0f)) / (M_PI / 4.0f)) % 8;
    return returnState;
}
int InputMapping::GetAnimationID() {
    Vector2 directionVec = GetDirectionMoving().Normalise();
    double angle = atan2(directionVec.x, directionVec.y);
    //atan2 returns in range [-pi - pi] so convert to [0 - 2pi]
    if (angle < 0) {
        angle += M_PI * 2;
    }
    int direction = static_cast<int>((angle + (M_PI / 8.0f)) / (M_PI / 4.0f)) % 8;
    switch (direction) {
    case 0:
        return 0; // // if player is moving straight down return the cat down animation ID
    case 1:
    case 2:
    case 3:
        return 2; // if player is moving left at all, return the cat facing left animation ID
    case 4:
        return 3; // if player is moving straight up, return the cat up animation ID
    case 5:
    case 6:
    case 7:
        return 1; // if player is moving right at all, return the cat facing right animation ID
    default:
        return 0;
    }
}
#pragma endregion InputMapping
#pragma region PlayerController

PlayerController::PlayerController(GameStateMachine* pMachine, CollisionManager* pCollisionManager)
    : Animatable(*new vector<string>{ "%CatStraight", "%CatLeft", "%CatRight", "%CatUp", "%CatDash"}, pMachine->settings->textureManager, "Cats\\Cat" + pMachine->settings->playerPalette) {
    //player's values
    attackCooldown = 500;
    playerHealth = 100;
    maxHealth = playerHealth;
    playerSpeed = 10;
    //dash values
    dashCooldown = 1000; // time after start of dash when another dash can be initiated, must be greather than dashduration
    dashDuration = 250; // time spent physically dashing
    dashLength = 100; // distance covered in a dash
    //stun values
    stunDuration = 500;
    //starting position / hitbox size
    xPos = 20;
    yPos = 50;
    width = 10;
    height = 24;
    //used later
    lastAttackTimestamp = 0;
    lastDashTimestamp = 0;
    lastStunTimestamp = 0;
    attackBox = new Hitbox();
    attackBoxOffset = Vector2(0, 0);
    direction = 0;
    movementState = 0;
    attackState = 0;
    collisionManager = pCollisionManager;
    machine = pMachine;
    dashStartPoint = *new Vector2{ 0,0 };
    dashEndPoint = *new Vector2{ 0,0 };
    inputs = new InputMapping();
    xVelo = 0;
    yVelo = 0;
    attacks = new vector<PlayerAttack*>();
}

#pragma region Getters
PlayerState PlayerController::GetState() {
    return *new PlayerState(direction, movementState, attackState, inputs->GetAnimationID());
}
int PlayerController::GetXForServer()
{
    return (int)xPos;
}
int PlayerController::GetYForServer()
{
    return (int)yPos;
}
int PlayerController::GetWidth() {
    return width;
}
int PlayerController::GetHeight() {
    return height;
}
Vector2 PlayerController::GetMiddle()
{
    double xMid = xPos + (width / 2);
    double yMid = yPos + (height / 2);
    return Vector2(xMid, yMid);
}
Hitbox PlayerController::GetPlayerBox()
{
    return Hitbox{(int)xPos + 3, (int)yPos + 8, (int)height, (int)width};
}
bool PlayerController::IsAlive() {
    return playerHealth > 0;
}
int PlayerController::GetHealthPercent()
{
    return ((double)playerHealth / maxHealth) * 100;
}
void PlayerController::ApplyForce(double xForce, double yForce)
{
    xVelo += xForce;
    yVelo += yForce;
}
#pragma endregion Getters

void PlayerController::HandleInput(SDL_Event& event, MyGame* game){
    switch (event.key.keysym.sym) {
#pragma region directional keys
    case SDLK_w:
        if (event.type == SDL_KEYUP) {
            inputs->upPressed = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            inputs->upPressed = true;
        }
        break;
    case SDLK_s:
        if (event.type == SDL_KEYUP) {
            inputs->downPressed = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            inputs->downPressed = true;
        }
        break;
    case SDLK_d:
        if (event.type == SDL_KEYUP) {
            inputs->rightPressed = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            inputs->rightPressed = true;
        }
        break;
    case SDLK_a:
        if (event.type == SDL_KEYUP) {
            inputs->leftPressed = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            inputs->leftPressed = true;
        }
        break;
#pragma endregion directionalKeys
    case SDLK_LSHIFT:
        if (event.type == SDL_KEYDOWN && IsAlive()) {
            Dash();
        }
        break;
    case SDLK_SPACE:
        if (event.type == SDL_KEYDOWN && IsAlive()) {
            Attack(game);
        }
        break;
    }
}

#pragma region UpdateMovement
void PlayerController::UpdateMove(double deltaTime, int screenScaling){
    for (PlayerAttack* pA : *attacks) {
        pA->Update(deltaTime);
    }
    attacks->erase(remove_if(attacks->begin(),
        attacks->end(),
        [](PlayerAttack* attack) {
            if (attack->Delete()) {
                delete attack;
                return true;
            }
            return false;
    }), attacks->end());
#pragma region stateUpdate
    if (inputs->IsStill()) {
        if (movementState == 1) {
            movementState = 0;
        }
    }
    else {
        if (movementState == 0) {
            movementState = 1;
        }
    }
    int nextDirState = inputs->GetDirectionState(Vector2(xPos, yPos));
    if (nextDirState >= 0) {
        direction = nextDirState;
        //cout << "direction: " << direction << endl;
    }
    //apply friction to velocity
    double frictionModifier = 0.13f;
    xVelo = xVelo - (xVelo * (frictionModifier * (deltaTime / 10)));
    yVelo = yVelo - (yVelo * (frictionModifier * (deltaTime / 10)));
#pragma endregion stateUpdate
    switch (movementState) {
        //0 or 1 is still or moving so just do movement code for either incase the still state is inaccurate
    case 0:
    case 1:
        UpdateBasicMovement(deltaTime * screenScaling); // multiplying by screen scaling is a hacky way to adjust player speed for screen size
        break;
    case 2:
        UpdateDashMovement();
        break;
    case 3:
        UpdateStun();
        break;
    }

    attackState = (SDL_GetTicks() - lastAttackTimestamp < attackCooldown);
}
void PlayerController::UpdateEnemyAttacks(MyGame* game)
{
    vector<Enemy*> enemies = game->GetCollidingEnemies(GetPlayerBox());
    for (Enemy* e : enemies) {
        if (e->IsAttacking()) {
            e->ResetAttackTimestamp();
            EnemyAttackData incomingAttack = e->GetAttackData();
            playerHealth -= incomingAttack.attackDamage;
            //receive knockback
            Vector2 playerPos = Vector2(xPos, yPos);
            Vector2 enemyPos = Vector2(incomingAttack.enemyMiddleX, incomingAttack.enemyMiddleY);
            Vector2 knockbackDirection = (playerPos - enemyPos).Normalise();
            Vector2 finalKnockback = knockbackDirection * (incomingAttack.knockbackModifier) * 0.5f;
            ApplyForce(finalKnockback.x, finalKnockback.y);
        }
    }
}
void PlayerController::UpdateBasicMovement(double deltaTime)
{
    double speed = 0.0001f * playerSpeed;
    Vector2 currentPos = *new Vector2{ xPos, yPos };
    Vector2 endPos;
    Vector2 finalPos;
    Vector2 currentDir = inputs->GetDirectionMoving();
    xVelo += currentDir.x * speed;
    yVelo += currentDir.y * speed;
    endPos = *new Vector2{ (xPos + (xVelo * deltaTime)), (yPos + (yVelo * deltaTime)) };
    finalPos = collisionManager->ApplyCollision(currentPos, endPos);
    xPos = finalPos.x;
    yPos = finalPos.y;
}
void PlayerController::UpdateDashMovement()
{
    Vector2 currentPos = *new Vector2{ xPos, yPos };
    //calculate this movement step between dash start and end
    double timeSinceDash = SDL_GetTicks() - lastDashTimestamp;
    double dashProgress = timeSinceDash / dashDuration;
    Vector2 stepEndPoint = dashStartPoint.Add(((dashEndPoint-(dashStartPoint))*(dashProgress)));
    //apply the collision
    Vector2 actualEnd = collisionManager->ApplyCollision(currentPos, stepEndPoint);
    if (actualEnd.FindDistance(stepEndPoint) > 0.5f) {
        //player has collided
        Stun();
    }
    //otherwise apply it
    xPos = actualEnd.x;
    yPos = actualEnd.y;
    if (SDL_GetTicks() - lastDashTimestamp > dashDuration) {
        movementState = 1;
    }
}
void PlayerController::UpdateStun()
{
    if (SDL_GetTicks() > lastStunTimestamp + stunDuration) {
        movementState = 0;
    }
}
#pragma endregion UpdateMovement
#pragma region PlayerActions
void PlayerController::Attack(MyGame* game) {
    if (game == nullptr) {
        return;
    }
    if (SDL_GetTicks() - lastAttackTimestamp < attackCooldown) {
        //attack on cooldown
        return;
    }
    lastAttackTimestamp = SDL_GetTicks();
    //update attack hitbox offset
    Vector2 currentDirection = inputs->GetDirectionFacing(Vector2(xPos,yPos));
    Vector2 middle = GetMiddle();
    int attackSize = 24;
    attackBoxOffset.x = (currentDirection.x * attackSize) - attackSize;
    attackBoxOffset.y = (currentDirection.y * attackSize) - attackSize;
    //update attack box size (might be useful later when attack size could vary)
    attackBox->w = attackSize * 2;
    attackBox->h = attackSize * 2;
    //update attack box position
    attackBox->x = attackBoxOffset.x + middle.x;
    attackBox->y = attackBoxOffset.y + middle.y;
    int attackAnimationX = middle.x + attackBoxOffset.x;
    int attackAnimationY = middle.y + attackBoxOffset.y;
    //last parameter of playerattack constructor is bool for flipped, so we just check if the x the player is facing is negative
    attacks->push_back(new PlayerAttack(machine->settings, attackAnimationX, attackAnimationY, (currentDirection.x < 0)));
    //find enemies hit
    vector<Enemy*> enemiesHit = game->GetCollidingEnemies(*attackBox);
    //send message to the server detailing enemies caught in attack
    if (enemiesHit.size() > 0) {
        for (Enemy* hitEnemy : enemiesHit) {
            game->SendEnemyDamageMessage(hitEnemy, 40);
            cout << "Hit enemy!" << endl;
        }
    }
}
void PlayerController::Dash()
{
    if (SDL_GetTicks() - lastDashTimestamp < dashCooldown) {
        //TODO some sort of way to indicate to player that dash failed because it was still on cooldown
        //maybe a sound plays or something
        return;
    }
    movementState = 2;
    lastDashTimestamp = SDL_GetTicks();
    dashStartPoint = *new Vector2{ xPos, yPos };
    Vector2 dashDirection = inputs->GetDirectionFacing(Vector2(xPos,yPos));
    Vector2 dashMovement = dashDirection*(dashLength);
    dashEndPoint = dashStartPoint.Add(dashMovement);
}
void PlayerController::Stun()
{
    movementState = 3;
    lastStunTimestamp = SDL_GetTicks();
}
#pragma endregion PlayerActions
void PlayerController::Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY) {
    for (PlayerAttack* pA : *attacks) {
        pA->Render(renderer, camOffX, camOffY);
    }
    if (IsAlive()) // TODO add a ghost sprite for rendering player when they are dead
    {
        //convert the mouse coordinates back into world coordinates here because the offset is passed in
        //TODO regorganise this as having it here is strange
        int mouseX;
        int mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        inputs->mousePos = Vector2(mouseX / machine->settings->screenScaling() + camOffX, mouseY / machine->settings->screenScaling() + camOffY);
        switch (movementState) {
        case 0:
            //standing still
            Animatable::PlayAnimation(0);
            break;
        case 1:
            //walking left, right, up or down
            Animatable::PlayAnimation(inputs->GetAnimationID());
            break;
        case 2:
            //dashing
            Animatable::PlayAnimation(4);
            break;
        }
        Animatable::UpdateAnimation();
        Animatable::Render(renderer, xPos - camOffX, yPos - camOffY, 16, 32, settings);
    }
}
void PlayerController::ReloadTexturesWithPalette(string palette)
{
    Animatable::ReloadAllFrames(*new vector<string>{ "%CatStraight", "%CatLeft", "%CatRight", "%CatUp", "%CatDash"}, machine->settings->textureManager, "Cats\\Cat" + palette);
}
#pragma endregion PlayerController


/*
Debug attack box rendering code, add to render function when debugging attack

        SDL_Rect* DebugAttackBoxRect = new SDL_Rect{ (attackBox->x - camOffX) * machine->settings->screenScaling(),
            (attackBox->y - camOffY) * machine->settings->screenScaling(),
            (attackBox->w) * machine->settings->screenScaling(),
            (attackBox->h) * machine->settings->screenScaling() };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, DebugAttackBoxRect);
*/