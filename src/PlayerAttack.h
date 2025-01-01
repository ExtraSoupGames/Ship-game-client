#pragma once
#include "SDL.h"
#include "Animation.h"
using namespace std;
class PlayerAttack : public Animatable {
private:
	double timeAlive; // should die at 599
	int x;
	int y;
	GlobalSettingsProfile* settings;
public:
	PlayerAttack(GlobalSettingsProfile* g, int pX, int pY);
	~PlayerAttack();
	void Render(SDL_Renderer* renderer, int camOffX, int camOffY);
	void Update(double deltaTime);
	bool Delete();
};