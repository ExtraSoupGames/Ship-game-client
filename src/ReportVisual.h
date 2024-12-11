#pragma once
#include "SDL.h"
#include "UIRendering.h"
#include "UIElement.h"
#include <iostream>
#include "TextureManager.h"
#include "Data.h"
using namespace std;
class ReportVisual : public UIElement {
	string displayText;
	TTF_Font* font;
	int screenScaling;
	int x;
	int y;
	SDL_Texture* texture;
public:
	ReportVisual(GameReport* report, int pX, int pY, TextureManager* textureManager, int screenScaling, int fontSize);
	~ReportVisual();
	void Render(SDL_Renderer* renderer) override;
	void Input(SDL_Event& e) override;
	void Update() override;
};