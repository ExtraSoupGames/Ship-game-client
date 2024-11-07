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
    Hitbox* attackBox;
    int direction;
    int movementState;
    int attackState;
public:
    double xVelo;
    double yVelo;
    double xPos;
    double yPos;
    double lastAttackTimestamp;
    double attackCooldown;
    PlayerState GetState();
    PlayerController(TextureManager* t);
    void HandleInput(SDL_Event& event, MyGame* game);
    void UpdateMove(CollisionManager* c, double deltaTime);
    void Render(SDL_Renderer* renderer);
};