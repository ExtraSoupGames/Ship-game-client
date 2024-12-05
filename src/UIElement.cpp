#include "UIElement.h"

ClickableUIElement::ClickableUIElement(int pX, int pY, int pWidth, int pHeight, function<void()> clickFunc, function<void()> clickOffFunc)
{
	clickFunction = clickFunc;
	x = pX;
	y = pY;
	width = pWidth;
	height = pHeight;
	clickAwayFunction = clickOffFunc;
}

void ClickableUIElement::OnClick()
{
	clickFunction();
}
void ClickableUIElement::OnClickOff() {
	if (clickAwayFunction != 0) {
		clickAwayFunction();
	}
}

bool ClickableUIElement::IsInBounds(int clickX, int clickY)
{
	if (clickX < x || clickY < y) {
		return false;
	}
	if (clickX > x + width || clickY > y + height) {
		return false;
	}
	return true;
}

void ClickableUIElement::HandleClickInput(SDL_Event& e)
{
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (IsInBounds(e.button.x, e.button.y)) {
			OnClick();
		}
		else {
			OnClickOff();
		}
	}
	else if (e.type == SDL_MOUSEMOTION) {
		if (IsInBounds(e.button.x, e.button.y)) {
			OnHover();
		}
		else {
			OnUnHover();
		}
	}
}
