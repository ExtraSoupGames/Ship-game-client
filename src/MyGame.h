#ifndef __MY_GAME_H__
#define __MY_GAME_H__

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <SDL.h>
#include <SDL_net.h>

#include "Interpolator.h"
#include "Collision.h"
#include "ServerManager.h"
#include "TextureManager.h"
#include "Animation.h"
#include "PlayerController.h"
#include "Data.h"
#include "GameState.h"
using namespace std;
struct Hitbox {
    //defined by a point and a width and height
    int x;
    int y;
    int w;
    int h;
    bool Collides(Hitbox& other);
};
class MyGame : public GameState{

    private:
        vector<Enemy*>* enemies;
        vector<OtherPlayer*>* players;
        double clientServerTimeDiff;
        double serverStartTime;
        int clientID = 0;
        CollisionManager* collisions;
        ServerManager* server;
        SDL_Renderer* renderer;
        TextureManager* textureManager;
        PlayerController* playerController;
        double broadcastTimer;
        double broadcastSpacing = 20;
        double serverBroadcastTimer = 0;
        double serverBroadcastDelay = 500;//adjust this to change how quickly important messages are resent - could affect performance if too low

        Animatable* number;
        template <typename T> Enemy* ProcessEnemy(DataPoint* data, int ID, double timestamp);
        void HandlePlayerData(string data);
        void HandleEnemyData(string data);
        void HandleBoundaryData(string data);
    public:
        MyGame(int pClientID, ServerManager* serverManager, SDL_Renderer* renderer);
        std::vector<std::string> messages;


        void OnReceive(char* message, int messageLength) override;
        void Input(SDL_Event& event) override;
        void Update(double deltaTime) override;
        void Render(SDL_Renderer* renderer) override;
        void OnEnter() override;
        void OnExit() override;
        double thisPlayerX = 0;
        double thisPlayerY = 0;
        vector<Enemy*> GetCollidingEnemies(Hitbox area);
        void SendEnemyDamageMessage(Enemy* enemyDamaged, int damage);
};

#endif