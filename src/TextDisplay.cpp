#include "TextDisplay.h"


TextDisplay::TextDisplay(TextureManager* t, int pX, int pY, int pScreenScaling, string pText)
{
	texture = t->GetTexture("UI\\Button");
	text = pText;
	screenScaling = pScreenScaling;
	font = UIRendering::LoadFontAtPixelHeight(25, screenScaling);
	x = pX * pScreenScaling;
	y = pY * pScreenScaling;

}

void TextDisplay::Input(SDL_Event& e)
{
}

void TextDisplay::Update()
{
}

void TextDisplay::Render(SDL_Renderer* renderer)
{
	SDL_Rect* destRect;
	destRect = new SDL_Rect{ x, y, 160 * screenScaling, 17 * screenScaling };
	SDL_RenderCopy(renderer, texture, NULL, destRect);
	UIRendering::RenderText(renderer, text, x + (4 * screenScaling), y + (4 * screenScaling), font);

}

void TextDisplay::SetText(string setText)
{
	text = setText;
}
