#include "Button.h"
#include "GameState.h"
TTF_Font* LoadFontAtPixelHeight(int fontSize, int screenScaling) {
	TTF_Font* tempFont = TTF_OpenFont("arial.ttf", fontSize);
	int pixelHeight;
	int pixelWidth;
	TTF_SizeText(tempFont, "A", &pixelHeight, &pixelWidth);
	double requiredScaleFactor = pixelHeight / (static_cast<double>(fontSize) * screenScaling);
	TTF_CloseFont(tempFont);
	return TTF_OpenFont("arial.ttf", fontSize * requiredScaleFactor);
}
Button::Button(string buttonText, int buttonX, int buttonY, function<void()> clickEvent, TextureManager* textureManager, int pScreenScaling, int fontSize = 10)
	: ClickableUIElement(buttonX, buttonY, ((displayText.size() > 20) ? 160 * pScreenScaling : 80 * pScreenScaling), 16 * pScreenScaling, clickEvent) {
	displayText = buttonText;
	screenScaling = pScreenScaling;
	font = TTF_OpenFont("arial.ttf", fontSize * screenScaling / 3);
	useBigButton = false;
	if (displayText.size() > 20) {
		useBigButton = true;
	}
	if (useBigButton) {
		width = 160 * screenScaling;
		height = 16 * screenScaling;
		texture = textureManager->GetTexture("UI\\Button");
	}
	else {
		width = 80 * screenScaling;
		height = 16 * screenScaling;
		texture = textureManager->GetTexture("UI\\ButtonSmall");
	}
}
Button::~Button()
{
	TTF_CloseFont(font);
}
void Button::Render(SDL_Renderer* renderer) {
	SDL_Rect* destRect;
	destRect = new SDL_Rect{ x, y, width, height };
	SDL_RenderCopy(renderer, texture, NULL, destRect);
	UIRendering::RenderText(renderer, displayText, x + (3 * screenScaling), y + (3 * screenScaling), font);
}
void Button::Input(SDL_Event& e)
{
	HandleClickInput(e);
}

