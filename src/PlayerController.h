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
        int returnState = 0;
        //TODO fix this mess
#pragma region GettingDirectionState
        if (upPressed) {
            if (leftPressed) {
                if (rightPressed) {
                    if (downPressed) {
                        return -1; // still - return value does not matter
                    }
                    else {
                        return 0;
                    }
                }
                else {
                    if (downPressed) {
                        return 6;
                    }
                    else {
                        return 7;
                    }
                }
            }
            else {
                if (rightPressed) {
                    if (downPressed) {
                        return 2;
                    }
                    else {
                        return 1;
                    }
                }
                else {
                    if (downPressed) {
                        return -1; //still - return value does not matter
                    }
                    else {
                        return 0;
                    }
                }
            }
        }
        else {
            if (leftPressed) {
                if (rightPressed) {
                    if (downPressed) {
                        return 4;
                    }
                    else {
                        return -1; //still - return value does not matter
                    }
                }
                else {
                    if (downPressed) {
                        return 5;
                    }
                    else {
                        return 6;
                    }
                }
            }
            else {
                if (rightPressed) {
                    if (downPressed) {
                        return 3;
                    }
                    else {
                        return 2;
                    }
                }
                else {
                    if (downPressed) {
                        return 4;
                    }
                    else {
                        return -1; //still - return value does not matter
                    }
                }
            }
        }
#pragma endregion GettingDirectionState
    }
};
class PlayerController : public Animatable {
private:
    void Attack(MyGame* game);
    void Dash();
    
    //hitboxes, collision, ect
    Hitbox* attackBox;
    Vector2 GetMiddle();
    double width;
    double height;
    double xPos;
    double yPos;

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