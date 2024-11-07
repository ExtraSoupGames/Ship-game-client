#pragma once
#include "SDL.h"
#include "ServerManager.h"
#include <vector>
#include "Animation.h"
using namespace std;
struct DataStream;
struct DataPoint;
struct Hitbox;
class Interpolator {
private:
	vector<DataStream*> incomingDataBuffer;
protected:
	int ID;
	int x;
	int y;
	virtual void OnInterpolate(DataPoint* data) {};
public:
	Interpolator(int pID);
	virtual void AddToBuffer(DataStream*  newData);
	virtual void Interpolate(double timestamp);
	bool HasID(int IDToCompare);
};
class Enemy : public Interpolator
{
private:
protected:
	int width;
	int height;
	void OnInterpolate(DataPoint* data) override;
public:
	Hitbox GetHitbox();
	Enemy(int ID);
	int GetID();
	void Render(SDL_Renderer* renderer);
};
class OtherPlayer : public Interpolator, public Animatable
{
protected:
	PlayerState state;
	void OnInterpolate(DataPoint* data) override;
public:
	OtherPlayer(int ID, TextureManager* t);
	void Render(SDL_Renderer* renderer);
};

