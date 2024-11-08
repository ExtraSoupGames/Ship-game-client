#pragma once
#include <vector>
#include <iostream>
#include "SDL.h"
using namespace std;
struct Vector2 {
	double x = 0;
	double y = 0;
};
class CollisionBoundary {
private:
	Vector2 point1;
	Vector2 point2;
	Vector2 collideOffset; // this is the vector that the player will be moved by when they collide with it so they are not inside the line after colliding
	//note that colliders will only work in one direction
	//if a player tries to collide from other direction they will "phase" through the line
	//this shouldnt be a problem as no 2 way colliders should be required
public:
	void Draw(SDL_Renderer* renderer);
	CollisionBoundary(double x1, double y1, double x2, double y2, double ox, double oy);
	Vector2 InterruptMovement(Vector2 start, Vector2 desiredEnd);
	bool IsRelevant(Vector2 start, Vector2 desiredEnd);
};
class CollisionManager {
private:
	vector<CollisionBoundary> boundaries;
public:
	CollisionManager();
	void AddBoundary(CollisionBoundary newBoundary);
	Vector2 ApplyCollision(Vector2 start, Vector2 desiredEnd);
	void DrawDebug(SDL_Renderer* renderer);
	bool IsEmpty();
};