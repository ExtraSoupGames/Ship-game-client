#pragma once
#include "SDL.h"
#include "UIRendering.h"
#include "UIelement.h"
#include "TextureManager.h"
using namespace std;
class GameTimeDisplay : public UIElement{
private:
	SDL_Texture* texture;
	TTF_Font* font;
	int currentTimeSurvived;
	int screenScaling;
	int x;
	int y;
public:
	GameTimeDisplay(TextureManager* t, int pX, int pY, int pScreenScaling);
	void Input(SDL_Event& e) override;
	void Update() override;
	void Render(SDL_Renderer* renderer) override;
	void SetTimeSurvived(int timeSurvived);
};