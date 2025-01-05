#pragma once
#include "SDL.h"
#include "ServerManager.h"
#include <vector>
#include "Animation.h"
using namespace std;
struct DataStream;
struct DataPoint;
struct Hitbox;
struct EnemyAttackData {
	int enemyMiddleX;
	int enemyMiddleY;
	int attackDamage;
	double knockbackModifier;
	EnemyAttackData(int enemyX, int enemyY, int damage, double kb) {
		enemyMiddleX = enemyX;
		enemyMiddleY = enemyY;
		attackDamage = damage;
		knockbackModifier = kb;
	}
};
enum EnemyType {
	LEECH,
	FLOPPER,
	CLINGABING
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
	double lastAttackTimestamp;
	void OnInterpolate(DataPoint* data) override;
public:
	Enemy(int ID);
	virtual Hitbox GetHitbox() = 0;
	int GetID();
	virtual void Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY);
	virtual bool IsAttacking() = 0; // does the player get damaged if they overlap
	virtual EnemyAttackData GetAttackData() = 0;
	void ResetAttackTimestamp(); // called by the player when the player is attacked
	static EnemyType GetEnemyTypeFromBinary(string binaryIn);
};
#pragma region Enemies
class Bobleech : public Enemy, public Animatable {
private:
	bool flipped;
protected:
	void OnInterpolate(DataPoint* data) override;
public:
	Bobleech(int ID, TextureManager* t);
	Hitbox GetHitbox() override;
	void Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY) override;
	bool IsAttacking() override;
	virtual EnemyAttackData GetAttackData() override;
};
class Flopper : public Enemy, public Animatable {
	FlopperStates state;
protected:
	void OnInterpolate(DataPoint* data) override;
public:
	Flopper(int ID, TextureManager* t);
	Hitbox GetHitbox() override;
	void Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY) override;
	bool IsAttacking() override;
	virtual EnemyAttackData GetAttackData() override;
};
class Clingabing : public Enemy, public Animatable{
	ClingabingStates state;
protected:
	void OnInterpolate(DataPoint* data) override;
public:
	Clingabing(int ID, TextureManager* t);
	Hitbox GetHitbox() override;
	void Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY) override;
	bool IsAttacking() override;
	virtual EnemyAttackData GetAttackData() override;
};
#pragma endregion enemies
class OtherPlayer : public Interpolator, public Animatable
{
protected:
	PlayerState state;
	string colourPalette;
	void OnInterpolate(DataPoint* data) override;
public:
	bool isAlive;
	OtherPlayer(int ID, TextureManager* t, string playerPalette);
	void Render(SDL_Renderer* renderer, GlobalSettingsProfile* settings, int camOffX, int camOffY);
	void CheckColour(string newPalette, TextureManager* t);
};

