#include "GameTimeDisplay.h"


GameTimeDisplay::GameTimeDisplay(TextureManager* t, int pX, int pY, int pScreenScaling)
{
	texture = t->GetTexture("UI\\ButtonTiny");
	currentTimeSurvived = 0;
	screenScaling = pScreenScaling;
	font = UIRendering::LoadFontAtPixelHeight(15, screenScaling);
	x = pX;
	y = pY;

}

void GameTimeDisplay::Input(SDL_Event& e)
{
}

void GameTimeDisplay::Update()
{
}

void GameTimeDisplay::Render(SDL_Renderer* renderer)
{
	SDL_Rect* destRect;
	destRect = new SDL_Rect{ x, y, 16 * screenScaling, 17 * screenScaling };
	SDL_RenderCopy(renderer, texture, NULL, destRect);
	UIRendering::RenderText(renderer, to_string(currentTimeSurvived), x + (4 * screenScaling), y + (4 * screenScaling), font);

}

void GameTimeDisplay::SetTimeSurvived(int timeSurvived)
{
	currentTimeSurvived = timeSurvived;
}
