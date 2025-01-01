#pragma once
#include <SDL.h>
#include "Collision.h"
#include "Data.h"
#include "Animation.h"
#include "PlayerAttack.h"
#include "GameState.h"
struct Hitbox;
class MyGame;
class TextureManager;
struct InputMapping {
    bool upPressed;
    bool downPressed;
    bool rightPressed;
    bool leftPressed;
    Vector2 mousePos;
    Vector2 GetDirectionFacing(Vector2 playerPos);
    Vector2 GetDirectionMoving();
    bool IsStill();
    int GetDirectionState(Vector2 playerState);
    int GetAnimationID();
};
class PlayerController : public Animatable {
private:
    //player values
    int playerHealth;
    int playerSpeed;
    //player actions
    void Attack(MyGame* game);
    void Dash();
    void Stun();

    //update player movement
    void UpdateBasicMovement(double deltaTime);
    void UpdateDashMovement();
    void UpdateStun();

    //hitboxes, collision, ect
    Hitbox* attackBox;
    Vector2 attackBoxOffset;
    Vector2 GetMiddle();
    double width;
    double height;
    Hitbox GetPlayerBox();
    vector<PlayerAttack*>* attacks;

    //movement
    double xPos;
    double yPos;
    double xVelo;
    double yVelo;
    void ApplyForce(double xForce, double yForce);

    //states
    int direction; //0-7, representing cardinal directions starting from 0 = north (directly up)
    int movementState; // 0 = still, 1 = walking, 2 = dashing
    int attackState; // 0 = not attacking, 1 = attacking (aka on cooldown from attack)

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

    //stun values
    double lastStunTimestamp;
    double stunDuration; //player gets stunned if they dash into a wall

    //references
    CollisionManager* collisionManager;
    InputMapping* inputs;
    GameStateMachine* machine;
public:
    //getters
    PlayerState GetState();
    int GetXForServer();
    int GetYForServer();
    int GetWidth();
    int GetHeight();
    bool IsAlive();

    PlayerController(GameStateMachine* machine, CollisionManager* pCollisionManager);
    void HandleInput(SDL_Event& event, MyGame* game);
    void UpdateMove(double deltaTime, int screenScaling);
    void UpdateEnemyAttacks(MyGame* game);
    void Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY);
    void ReloadTexturesWithPalette(string palette);
};