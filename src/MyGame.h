#ifndef __MY_GAME_H__
#define __MY_GAME_H__

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <SDL.h>
#include <SDL_net.h>

#include "Enemy.h"
#include "Collision.h"
#include "ServerManager.h"
#include "TextureManager.h"
#include "Animation.h"
#include "PlayerController.h"
#include "Data.h"
using namespace std;

struct Hitbox {
    //defined by a point and a width and height
    int x;
    int y;
    int w;
    int h;
    bool Collides(Hitbox& other);
};
class MyGame {

    private:
        vector<Enemy*>* enemies;
        vector<OtherPlayer*>* players;
        double serverStartTime;
        double clientConnectTime;
        int clientID = 0;
        CollisionManager* collisions;
        ServerManager* server;
        SDL_Renderer* renderer;
        TextureManager* textureManager;
        PlayerController* playerController;
        double broadcastTimer;
        double broadcastSpacing = 20;

        Animatable* number;


        void HandlePlayerData(string data);
        void HandleEnemyData(string data);
        void HandleBoundaryData(string data);
    public:
        MyGame(int pClientID, UDPsocket* serverSocket, SDL_Renderer* renderer);
        std::vector<std::string> messages;


        void on_receive(char* message, int messageLength);
        void input(SDL_Event& event);
        void update(double deltaTime);
        void render(SDL_Renderer* renderer);
        double thisPlayerX = 0;
        double thisPlayerY = 0;
        vector<Enemy*> GetCollidingEnemies(Hitbox area);
        void SendEnemyDamageMessage(Enemy* enemyDamaged, int damage);
};

#endif