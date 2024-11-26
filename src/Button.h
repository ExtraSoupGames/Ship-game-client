#pragma once
#include "SDL.h"
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
	Button(int buttonX, int buttonY, int buttonWidth, int buttonHeight, function<void()> clickEvent) {
		x = buttonX;
		y = buttonY;
		width = buttonWidth;
		height = buttonHeight;
		clickFunction = clickEvent;
	}
	void Render(SDL_Renderer* renderer) {
		SDL_RenderDrawRect(renderer, new SDL_Rect{x, y, width, height});
	}
	void Click() {
		clickFunction();
	}
	bool IsInBounds(int clickX, int clickY) {
		if (clickX < x || clickY < y) {
			return false;
		}
		if (clickX > x + width || clickY > y + height) {
			return false;
		}
		return true;
	}
};