#include "Button.h"
#include "GameState.h"
Button::Button(int buttonX, int buttonY, int buttonWidth, int buttonHeight, function<void()> clickEvent) {
	x = buttonX;
	y = buttonY;
	width = buttonWidth;
	height = buttonHeight;
	clickFunction = clickEvent;
}
void Button::Render(SDL_Renderer* renderer) {
	SDL_RenderDrawRect(renderer, new SDL_Rect{ x, y, width, height });
}
void Button::Click() {
	clickFunction();
}
bool Button::IsInBounds(int clickX, int clickY) {
	if (clickX < x || clickY < y) {
		return false;
	}
	if (clickX > x + width || clickY > y + height) {
		return false;
	}
	return true;
}