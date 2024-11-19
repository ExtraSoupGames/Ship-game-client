#include "PlayerController.h"
#include "MyGame.h"
PlayerController::PlayerController(TextureManager* t, CollisionManager* pCollisionManager) : Animatable(*new vector<string>{ "%walk", "%run", "%dash" }, t) {
    //player's values
    attackCooldown = 500;
    //dash values
    dashCooldown = 1000; // time after start of dash when another dash can be initiated, must be greather than dashduration
    dashDuration = 200; // time spent physically dashing
    dashLength = 200; // distance covered in a dash
    //stun values
    stunDuration = 500;
    //starting position
    xPos = 50;
    yPos = 50;
    width = 20;
    height = 20;
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
    dashStartPoint = *new Vector2{ 0,0 };
    dashEndPoint = *new Vector2{ 0,0 };
    inputs = new InputMapping();
}
#pragma region Getters
PlayerState PlayerController::GetState() {
    return *new PlayerState(direction, movementState, attackState);
}
int PlayerController::GetXForServer()
{
    return (int)xPos;
}
int PlayerController::GetYForServer()
{
    return (int)yPos;
}
Vector2 PlayerController::GetMiddle()
{
    double xMid = xPos + (width / 2);
    double yMid = yPos + (height / 2);
    return Vector2(xMid, yMid);
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
        if (event.type == SDL_KEYDOWN) {
            Dash();
        }
        break;
    case SDLK_SPACE:
        if (event.type == SDL_KEYDOWN) {
            Attack(game);
        }
        break;
    }
    //update movement state to reflect players movement, unless they are in a special state (dashing ect)
}
#pragma region UpdateMovement
void PlayerController::UpdateMove(double deltaTime){
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
    int nextDirState = inputs->GetDirectionState();
    if (nextDirState >= 0) {
        direction = nextDirState;
        //cout << "direction: " << direction << endl;
    }
#pragma endregion stateUpdate
    switch (movementState) {
        //0 or 1 is still or moving so just do movement code for either incase the still state is inaccurate
    case 0:
    case 1:
        UpdateBasicMovement(deltaTime);
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
void PlayerController::UpdateBasicMovement(double deltaTime)
{
    double speed = 0.3f;
    Vector2 currentPos = *new Vector2{ xPos, yPos };
    Vector2 endPos;
    Vector2 finalPos;
    Vector2 currentDir = inputs->GetCurrentDirection();
    double xVelo = currentDir.x * speed;
    double yVelo = currentDir.y * speed;
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
    if (SDL_GetTicks() - lastAttackTimestamp < attackCooldown) {
        //attack on cooldown
        return;
    }
    lastAttackTimestamp = SDL_GetTicks();
    //update attack hitbox offset
    Vector2 currentDirection = inputs->GetCurrentDirection();
    Vector2 middle = GetMiddle();
    int attackSize = 40;
    attackBoxOffset.x = (currentDirection.x * attackSize) - attackSize;
    attackBoxOffset.y = (currentDirection.y * attackSize) - attackSize;
    //update attack box size (might be useful later when attack size could vary)
    attackBox->w = attackSize * 2;
    attackBox->h = attackSize * 2;
    //update attack box position
    attackBox->x = attackBoxOffset.x + middle.x;
    attackBox->y = attackBoxOffset.y + middle.y;
    //find enemies hit
    vector<Enemy*> enemiesHit = game->GetCollidingEnemies(*attackBox);
    //send message to the server detailing enemies caught in attack
    if (enemiesHit.size() > 0) {
        for (Enemy* hitEnemy : enemiesHit) {
            game->SendEnemyDamageMessage(hitEnemy, 40);
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
    Vector2 dashDirection = inputs->GetCurrentDirection();
    Vector2 dashMovement = dashDirection*(dashLength);
    dashEndPoint = dashStartPoint.Add(dashMovement);
}
void PlayerController::Stun()
{
    movementState = 3;
    lastStunTimestamp = SDL_GetTicks();
}
#pragma endregion PlayerActions
void PlayerController::Render(SDL_Renderer* renderer) {
    SDL_Rect* playerRect = new SDL_Rect{ (int)xPos, (int)yPos, 20, 20 };
    if (attackState == 1) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }
    SDL_RenderDrawRect(renderer, playerRect);
    SDL_Rect* DebugAttackBoxRect = new SDL_Rect{ attackBox->x, attackBox->y, attackBox->w, attackBox->h};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, DebugAttackBoxRect);
    if (currentAnimation != movementState) {
        Animatable::PlayAnimation(movementState);
    }
    Animatable::UpdateAnimation();
    Animatable::Render(renderer, xPos, yPos, 30, 30);
}