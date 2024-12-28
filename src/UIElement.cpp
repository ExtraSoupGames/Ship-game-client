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
		disabledTexture = textureManager->GetTexture("UI\\DisabledButtonTiny");
		break;
	case Small:
		width = 80 * pScreenScaling;
		height = 17 * pScreenScaling;
		unHoverTexture = textureManager->GetTexture("UI\\ButtonSmall");
		hoverTexture = textureManager->GetTexture("UI\\ButtonSmallHover");
		clickTexture = textureManager->GetTexture("UI\\ButtonSmallClick");
		disabledTexture = textureManager->GetTexture("UI\\DisabledButtonSmall");
		break;
	case Normal:
	default:
		width = 160 * pScreenScaling;
		height = 17 * pScreenScaling;
		unHoverTexture = textureManager->GetTexture("UI\\Button");
		hoverTexture = textureManager->GetTexture("UI\\ButtonHover");
		clickTexture = textureManager->GetTexture("UI\\ButtonClick");
		disabledTexture = textureManager->GetTexture("UI\\DisabledButton");
		break;
	}
	currentTexture = unHoverTexture;
	enabled = true;
}

void ClickableUIElement::OnClick()
{
	if (enabled) {
		clickFunction();
	}
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
	if (!enabled) {
		return;
	}
	if (e.type == SDL_MOUSEBUTTONUP) {
		if (IsInBounds(e.button.x, e.button.y)) {
			OnClick();
			currentTexture = hoverTexture;
		}
		else {
			OnClickOff();
		}
	}
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (IsInBounds(e.button.x, e.button.y)) {
			currentTexture = clickTexture;
		}
	}
}
void ClickableUIElement::Update() {
	if (!enabled) {
		return;
	}
	int* mouseX = new int();
	int* mouseY = new int();
	SDL_GetMouseState(mouseX, mouseY);
	if (currentTexture != clickTexture) {
		if (IsInBounds(*mouseX, *mouseY)) {
			currentTexture = hoverTexture;
		}
		else {
			currentTexture = unHoverTexture;
		}
	}
	delete mouseX;
	delete mouseY;
}
void ClickableUIElement::Render(SDL_Renderer* renderer) {
	SDL_Rect* destRect;
	destRect = new SDL_Rect{ x, y, width, height };
	SDL_RenderCopy(renderer, currentTexture, NULL, destRect);
}

void ClickableUIElement::Disable()
{
	currentTexture = disabledTexture;
	enabled = false;
}

void ClickableUIElement::Enable()
{
	enabled = true;
	currentTexture = unHoverTexture;
}
