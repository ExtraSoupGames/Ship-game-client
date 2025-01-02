#include "HealthBar.h"


HealthBar::HealthBar(TextureManager* t, int pX, int pY, int pScreenScaling, PlayerController* playerController)
{
	texture = t->GetTexture("UI\\HealthBar");
	screenScaling = pScreenScaling;
	x = pX * pScreenScaling;
	y = pY * pScreenScaling;
	player = playerController;
}

void HealthBar::Input(SDL_Event& e)
{
}

void HealthBar::Update()
{
}

void HealthBar::Render(SDL_Renderer* renderer)
{
	//render the health
	int healthPercent = player->GetHealthPercent();
	SDL_Rect* healthRect = new SDL_Rect{x, y + (3 * screenScaling), (int)(160 * ((double)healthPercent / 100.00)) * screenScaling,11 * screenScaling};
	SDL_SetRenderDrawColor(renderer, 186, 28, 28, 255);
	SDL_RenderFillRect(renderer, healthRect);
	//render the bar overlay
	SDL_Rect* destRect;
	destRect = new SDL_Rect{ x, y, 160 * screenScaling, 17 * screenScaling };
	SDL_RenderCopy(renderer, texture, NULL, destRect);
}