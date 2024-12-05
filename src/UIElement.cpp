#include "UIElement.h"

ClickableUIElement::ClickableUIElement(int pX, int pY, int pScreenScaling, bool isSmall , TextureManager* textureManager, function<void()> clickFunc, function<void()> clickOffFunc)
{
	clickFunction = clickFunc;
	x = pX * pScreenScaling;
	y = pY * pScreenScaling;
	clickAwayFunction = clickOffFunc;

	if (isSmall) {
		width = 80 * pScreenScaling;
		height = 16 * pScreenScaling;
		unHoverTexture = textureManager->GetTexture("UI\\ButtonSmall");
		hoverTexture = textureManager->GetTexture("UI\\ButtonSmallHover");
	}
	else {
		width = 160 * pScreenScaling;
		height = 16 * pScreenScaling;
		unHoverTexture = textureManager->GetTexture("UI\\Button");
		hoverTexture = textureManager->GetTexture("UI\\ButtonHover");
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
void ClickableUIElement::Render(SDL_Renderer* renderer) {
	SDL_Rect* destRect;
	destRect = new SDL_Rect{ x, y, width, height };
	SDL_RenderCopy(renderer, currentTexture, NULL, destRect);
}
