#pragma once
#include <SDL.h>
#include "Collision.h"
#include "Data.h"
#include "Animation.h"
struct Hitbox;
class MyGame;
class TextureManager;
class PlayerController : public Animatable {
private:
    void Attack(MyGame* game);
    void Dash();
    Vector2 GetDirectionVector();
    Hitbox* attackBox;
    //states
    int direction;
    int movementState; // 0 = still, 1 = walking, 2 = dashing
    int attackState;

    //attack values
    double lastAttackTimestamp;
    double attackCooldown;

    //dash values
    double lastDashTimestamp;
    double dashCooldown;
    double dashDuration;
    double dashLength;
    Vector2 dashStartPoint;
    Vector2 dashEndPoint;

    //references
    CollisionManager* collisionManager;
public:
    double xVelo;
    double yVelo;
    double xPos;
    double yPos;
    PlayerState GetState();
    PlayerController(TextureManager* t, CollisionManager* pCollisionManager);
    void HandleInput(SDL_Event& event, MyGame* game);
    void UpdateMove(double deltaTime);
    void Render(SDL_Renderer* renderer);
};