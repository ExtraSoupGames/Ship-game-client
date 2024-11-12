#include "PlayerController.h"
#include "MyGame.h"
void PlayerController::Attack(MyGame* game){
    if (SDL_GetTicks() - lastAttackTimestamp < attackCooldown) {
        //attack on cooldown
        return;
    }
    lastAttackTimestamp = SDL_GetTicks();
    //GetCollidingEnemies
    attackBox->x = xPos;
    attackBox->y = yPos;
    attackBox->w = 50;
    attackBox->h = 50;
    vector<Enemy*> enemiesHit = game->GetCollidingEnemies(*attackBox);
    if (enemiesHit.size() > 0) {
        cout << "Hit something!!!!" << endl;
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
    Vector2 dashDirection = GetDirectionVector();
    cout << "Dash direction: " << dashDirection.x << ", " << dashDirection.y << endl;
    Vector2 dashMovement = dashDirection.Multiply(dashLength);
    dashEndPoint = dashStartPoint.Add(dashMovement);
    cout << "dashing from: " << dashStartPoint.x << ", " << dashStartPoint.y << " to " << dashEndPoint.x << ", " << dashEndPoint.y << endl;
}
Vector2 PlayerController::GetDirectionVector()
{
    return Vector2(xVelo, yVelo).Normalise();
}
PlayerController::PlayerController(TextureManager* t, CollisionManager* pCollisionManager) : Animatable(*new vector<string>{ "%walk", "%run", "%dash" }, t) {
    //player's values
    attackCooldown = 500;
    //dash values
    dashCooldown = 1000; // time after start of dash when another dash can be initiated, must be greather than dashduration
    dashDuration = 200; // time spent physically dashing
    dashLength = 200; // distance covered in a dash
    //starting position
    xPos = 50;
    yPos = 50;
    //used later
    xVelo = 0;
    yVelo = 0;
    lastAttackTimestamp = 0;
    lastDashTimestamp = 0;
    attackBox = new Hitbox();
    direction = 0;
    movementState = 0;
    attackState = 0;
    collisionManager = pCollisionManager;
    dashStartPoint = *new Vector2{ 0,0 };
    dashEndPoint = *new Vector2{ 0,0 };
}
PlayerState PlayerController::GetState() {
    return *new PlayerState(direction, movementState, attackState);
}
void PlayerController::HandleInput(SDL_Event& event, MyGame* game){
    double speed = 0.3;
    switch (event.key.keysym.sym) {
    case SDLK_w:
        if (event.type == SDL_KEYUP) {
            yVelo += speed;
        }
        else if (event.type == SDL_KEYDOWN) {
            yVelo -= speed;
        }
        break;
    case SDLK_s:
        if (event.type == SDL_KEYUP) {
            yVelo -= speed;
        }
        else if (event.type == SDL_KEYDOWN) {
            yVelo += speed;
        }
        break;
    case SDLK_d:
        if (event.type == SDL_KEYUP) {
            xVelo -= speed;
        }
        else if (event.type == SDL_KEYDOWN) {
            xVelo += speed;
        }
        break;
    case SDLK_a:
        if (event.type == SDL_KEYUP) {
            xVelo += speed;
        }
        else if (event.type == SDL_KEYDOWN) {
            xVelo -= speed;
        }
        break;
    case SDLK_LSHIFT:
        if (event.type == SDL_KEYDOWN) {
            Dash();
        }
        break;
    case SDLK_RSHIFT:
        if (event.type == SDL_KEYDOWN) {
            movementState = 1;
        }
        break;
    case SDLK_0:
        if (event.type == SDL_KEYDOWN) {
            movementState = 0;
        }
        break;
    case SDLK_SPACE:
        if (event.type == SDL_KEYDOWN) {
            Attack(game);
        }
        break;
    }
}

void PlayerController::UpdateMove(double deltaTime){
    Vector2 currentPos = *new Vector2{ xPos, yPos };
    Vector2 endPos;
    Vector2 finalPos;
    switch (movementState) {
    case 0:
        movementState = 1;
        break;
    case 1:
        endPos = *new Vector2{ (xPos + (xVelo * deltaTime)), (yPos + (yVelo * deltaTime)) };
        finalPos = collisionManager->ApplyCollision(currentPos, endPos);
        xPos = finalPos.x;
        yPos = finalPos.y;
        break;
    case 2:
        //calculate this movement step between dash start and end
        double timeSinceDash = SDL_GetTicks() - lastDashTimestamp;
        double dashProgress = timeSinceDash / dashDuration;
        Vector2 stepEndPoint = dashStartPoint.Add(((dashEndPoint.Subtract(dashStartPoint)).Multiply(dashProgress)));
        //apply the collision
        Vector2 actualEnd = collisionManager->ApplyCollision(currentPos, stepEndPoint);
        if (actualEnd.FindDistance(stepEndPoint) > 0.5f) {
            //player has collided
            //TODO stun the player
            movementState = 1;
        }
        //otherwise apply it
        xPos = actualEnd.x;
        yPos = actualEnd.y;
        if (SDL_GetTicks() - lastDashTimestamp > dashDuration) {
            movementState = 1;
        }
        break;
    }

    attackState = (SDL_GetTicks() - lastAttackTimestamp < attackCooldown);
}
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