#pragma once
#include "SDL.h"
#include "UIRendering.h"
#include "UIElement.h"
#include <iostream>
#include <functional>
#include "TextureManager.h"
using namespace std;
class Button : public ClickableUIElement {
	string displayText;
	TTF_Font* font;
	SDL_Texture* texture;
	bool useBigButton;
	int screenScaling;
public:
	Button(string buttonText, int buttonX, int buttonY, function<void()> clickEvent, TextureManager* textureManager, int screenScaling, int fontSize);
	~Button();
	void Render(SDL_Renderer* renderer) override;
	void Input(SDL_Event& e) override;
};