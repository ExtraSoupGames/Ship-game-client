#pragma once
#include <SDL.h>
#include "Collision.h"
#include "Data.h"
#include "Animation.h"
struct Hitbox;
class MyGame;
class TextureManager;
struct InputMapping {
    bool upPressed;
    bool downPressed;
    bool rightPressed;
    bool leftPressed;
    Vector2 GetCurrentDirection() {
        return *new Vector2(rightPressed - leftPressed, downPressed - upPressed);
    }
    bool IsStill() {
        return !(upPressed || downPressed || rightPressed || leftPressed);
    }
    int GetDirectionState() {
        Vector2 directionVec = GetCurrentDirection().Normalise();
        double angle = atan2(directionVec.x, directionVec.y);
        //atan2 returns in range [-pi - pi] so convert to [0 - 2pi]
        if (angle < 0) {
            angle += M_PI * 2;
        }
        int returnState = static_cast<int>((angle + (M_PI / 8.0f)) / (M_PI / 4.0f)) % 8;
        return returnState;
    }
};
class PlayerController : public Animatable {
private:
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
    double xPos;
    double yPos;

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
public:
    //getters
    PlayerState GetState();
    int GetXForServer();
    int GetYForServer();

    PlayerController(TextureManager* t, CollisionManager* pCollisionManager);
    void HandleInput(SDL_Event& event, MyGame* game);
    void UpdateMove(double deltaTime);
    void Render(SDL_Renderer* renderer);
};