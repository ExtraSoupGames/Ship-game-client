#pragma once
#include <vector>
#include <iostream>
#include "SDL.h"
using namespace std;
class Vector2 {
public:
    double x;
    double y;
    Vector2() : Vector2(0, 0) {

    }
    Vector2(double pX, double pY) {
        x = pX;
        y = pY;
    }
    Vector2 Add(Vector2 other) {
        return *new Vector2(x + other.x, y + other.y);
    }
    //use pythagoras theorem to find distance to another vector
    double FindDistance(Vector2 other) {
        return this->Subtract(other).Magnitude();
    }
    Vector2 Subtract(Vector2 other) {
        return *new Vector2(x - other.x, y - other.y);
    }
    Vector2 Normalise() {
        double magnitude = this->FindDistance(*new Vector2(0, 0));
        return *new Vector2(x / magnitude, y / magnitude);
    }
    Vector2 Multiply(double factor) {
        return *new Vector2(x * factor, y * factor);
    }
    double Magnitude() {
        return sqrt(pow(x, 2) + pow(y, 2));
    }
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