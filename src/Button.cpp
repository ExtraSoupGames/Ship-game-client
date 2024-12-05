#include "Button.h"
#include "GameState.h"
Button::Button(string buttonText, int buttonX, int buttonY, function<void()> clickEvent, TextureManager* textureManager, int pScreenScaling, int fontSize = 10)
	: ClickableUIElement(buttonX * pScreenScaling, buttonY * pScreenScaling, ((displayText.size() > 10) ? 160 * pScreenScaling : 80 * pScreenScaling), 16 * pScreenScaling, clickEvent) {
	displayText = buttonText;
	screenScaling = pScreenScaling;
	font = UIRendering::LoadFontAtPixelHeight(fontSize, screenScaling);
	useBigButton = false;
	if (displayText.size() > 10) {
		useBigButton = true;
	}
	if (useBigButton) {
		width = 160 * screenScaling;
		height = 16 * screenScaling;
		unHoverTexture = textureManager->GetTexture("UI\\Button");
		hoverTexture = textureManager->GetTexture("UI\\ButtonHover");
	}
	else {
		width = 80 * screenScaling;
		height = 16 * screenScaling;
		unHoverTexture = textureManager->GetTexture("UI\\ButtonSmall");
		hoverTexture = textureManager->GetTexture("UI\\ButtonSmallHover");
	}
	currentTexture = unHoverTexture;
}
Button::~Button()
{
	TTF_CloseFont(font);
}
void Button::Render(SDL_Renderer* renderer) {
	SDL_Rect* destRect;
	destRect = new SDL_Rect{ x, y, width, height };
	SDL_RenderCopy(renderer, currentTexture, NULL, destRect);
	UIRendering::RenderText(renderer, displayText, x + (3 * screenScaling), y + (3 * screenScaling), font);
}
void Button::Input(SDL_Event& e)
{
	HandleClickInput(e);
}
void Button::OnHover() {
	currentTexture = hoverTexture;
}
void Button::OnUnHover() {
	currentTexture = unHoverTexture;
}