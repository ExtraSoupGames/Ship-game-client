#pragma once
#include "SDL.h"
#include "ServerManager.h"
#include <vector>
#include "Animation.h"
using namespace std;
struct DataStream;
struct DataPoint;
struct Hitbox;
enum EnemyType {
	LEECH,
	FLOPPER
};
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
	virtual void Render(SDL_Renderer* renderer);
	static EnemyType GetEnemyTypeFromBinary(string binaryIn);
	static Enemy* ProcessEnemy(DataPoint* data, int ID, double timestamp, vector<Enemy*> enemies, TextureManager* t);
};
class Bobleech : public Enemy, public Animatable {
public:
	Bobleech(int ID, TextureManager* t);
	void Render(SDL_Renderer* renderer);
	static Enemy* ProcessEnemy(DataPoint* data, int ID, double timestamp, vector<Enemy*>* enemies, TextureManager* t);
};
enum FlopperStates {
	GROUNDED,
	AIRBORNE,
	SPAWNING
};
class Flopper : public Enemy, public Animatable {
	FlopperStates state;
public:
	Flopper(int ID, TextureManager* t);
	void SetState(string binaryIn);
	void Render(SDL_Renderer* renderer);
	static Enemy* ProcessEnemy(DataPoint* data, int ID, double timestamp, vector<Enemy*>* enemies, TextureManager* t);
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

