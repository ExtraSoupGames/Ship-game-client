#include "PlayerAttack.h"
PlayerAttack::PlayerAttack(GlobalSettingsProfile* g, int pX, int pY) : Animatable(vector<string>{"Cat\\Attack"}, g->textureManager) {
	settings = g;
	timeAlive = 0;
	x = pX;
	y = pY;
}

PlayerAttack::~PlayerAttack()
{
}

void PlayerAttack::Render(SDL_Renderer* renderer, int camOffX, int camOffY)
{
	Animatable::Render(renderer, x - camOffX, y - camOffY, 32, 32, settings);
}
void PlayerAttack::Update(double deltaTime) {
	timeAlive += deltaTime;
	Animatable::UpdateAnimation();
}
bool PlayerAttack::Delete() {
	return timeAlive > 599;
}