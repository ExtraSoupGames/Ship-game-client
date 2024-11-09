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
PlayerController::PlayerController(TextureManager* t) : Animatable(*new vector<string>{ "%walk", "%run", "%dash" }, t) {
    //player's values
    attackCooldown = 500;
    //starting position
    xPos = 50;
    yPos = 50;
    //used later
    xVelo = 0;
    yVelo = 0;
    lastAttackTimestamp = 0;
    attackBox = new Hitbox();
    direction = 0;
    movementState = 0;
    attackState = 0;
}
PlayerState PlayerController::GetState() {
    return *new PlayerState(direction, movementState, attackState);
}
void PlayerController::HandleInput(SDL_Event& event, MyGame* game){
    int speed = 1;
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
            movementState = 1;
        }
        break;
    case SDLK_RSHIFT:
        if (event.type == SDL_KEYDOWN) {
            movementState = 2;
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

void PlayerController::UpdateMove(CollisionManager* c, double deltaTime){
    Vector2 currentPos = *new Vector2{ xPos, yPos };
    Vector2 endPos = *new Vector2{ (xPos + (xVelo * deltaTime)), (yPos + (yVelo * deltaTime)) };
    Vector2 finalPos = c->ApplyCollision(currentPos, endPos);
    xPos = finalPos.x;
    yPos = finalPos.y;

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