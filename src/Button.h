#pragma once
#include "SDL.h"
#include "UIRendering.h"
#include "UIElement.h"
#include <iostream>
#include <functional>
using namespace std;
class Button : public ClickableUIElement {
	string displayText;
public:
	Button(string buttonText, int buttonX, int buttonY, int buttonWidth, int buttonHeight, function<void()> clickEvent);
	void Render(SDL_Renderer* renderer) override;
	void Input(SDL_Event& e) override;
};