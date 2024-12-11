#include "UIElement.h"

ClickableUIElement::ClickableUIElement(int pX, int pY, int pScreenScaling, UIElementSize size , TextureManager* textureManager, function<void()> clickFunc, function<void()> clickOffFunc)
{
	clickFunction = clickFunc;
	x = pX * pScreenScaling;
	y = pY * pScreenScaling;
	clickAwayFunction = clickOffFunc;
	switch (size) {
	case Tiny:
		width = 16 * pScreenScaling;
		height = 17 * pScreenScaling;
		unHoverTexture = textureManager->GetTexture("UI\\ButtonTiny");
		hoverTexture = textureManager->GetTexture("UI\\ButtonTinyHover");
		clickTexture = textureManager->GetTexture("UI\\ButtonTinyClick");
		break;
	case Small:
		width = 80 * pScreenScaling;
		height = 17 * pScreenScaling;
		unHoverTexture = textureManager->GetTexture("UI\\ButtonSmall");
		hoverTexture = textureManager->GetTexture("UI\\ButtonSmallHover");
		clickTexture = textureManager->GetTexture("UI\\ButtonSmallCLick");
		break;
	case Normal:
	default:
		width = 160 * pScreenScaling;
		height = 17 * pScreenScaling;
		unHoverTexture = textureManager->GetTexture("UI\\Button");
		hoverTexture = textureManager->GetTexture("UI\\ButtonHover");
		clickTexture = textureManager->GetTexture("UI\\ButtonClick");
		break;
	}
	currentTexture = unHoverTexture;
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

void ClickableUIElement::OnHover()
{
	currentTexture = hoverTexture;
}

void ClickableUIElement::OnUnHover()
{
	currentTexture = unHoverTexture;
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
	if (e.type == SDL_MOUSEBUTTONUP) {
		if (IsInBounds(e.button.x, e.button.y)) {
			OnClick();
		}
		else {
			OnClickOff();
		}
	}
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (IsInBounds(e.button.x, e.button.y)) {
			//TODO Clicked animation
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
void ClickableUIElement::Render(SDL_Renderer* renderer) {
	SDL_Rect* destRect;
	destRect = new SDL_Rect{ x, y, width, height };
	SDL_RenderCopy(renderer, currentTexture, NULL, destRect);
}
