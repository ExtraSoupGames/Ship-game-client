#pragma once
#include "SDL.h"
#include "UIRendering.h"
#include "UIelement.h"
#include "TextureManager.h"
using namespace std;
class TextDisplay : public UIElement{
private:
	SDL_Texture* texture;
	TTF_Font* font;
	string text;
	int screenScaling;
	int x;
	int y;
public:
	TextDisplay(TextureManager* t, int pX, int pY, int pScreenScaling, string text = "");
	void Input(SDL_Event& e) override;
	void Update() override;
	void Render(SDL_Renderer* renderer) override;
	void SetText(string setText);
};