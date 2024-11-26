#pragma once
#include "SDL.h"
#include "UIRendering.h"
#include <iostream>
#include <functional>
using namespace std;
class Button {
	string displayText;
	int x;
	int y;
	int width;
	int height;
	function<void()> clickFunction;
public:
	Button(string buttonText, int buttonX, int buttonY, int buttonWidth, int buttonHeight, function<void()> clickEvent);
	void Render(SDL_Renderer* renderer);
	void Click();
	bool IsInBounds(int clickX, int clickY);
};