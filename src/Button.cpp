#include "Button.h"
#include "GameState.h"
Button::Button(string buttonText, int buttonX, int buttonY, function<void()> clickEvent, TextureManager* textureManager, int pScreenScaling, int fontSize = 10)
	: ClickableUIElement(buttonX , buttonY , pScreenScaling, (displayText.size() < 15), textureManager, clickEvent) {
	displayText = buttonText;
	screenScaling = pScreenScaling;
	font = UIRendering::LoadFontAtPixelHeight(fontSize, screenScaling);
	currentTexture = unHoverTexture;
}
Button::~Button()
{
	TTF_CloseFont(font);
}
void Button::Render(SDL_Renderer* renderer) {
	ClickableUIElement::Render(renderer);
	UIRendering::RenderText(renderer, displayText, x + (3 * screenScaling), y + (3 * screenScaling), font);
}
void Button::Input(SDL_Event& e)
{
	HandleClickInput(e);
}