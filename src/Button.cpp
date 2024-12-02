#include "Button.h"
#include "GameState.h"
Button::Button(string buttonText, int buttonX, int buttonY, int buttonWidth, int buttonHeight, function<void()> clickEvent, int fontSize = 10)
	: ClickableUIElement(buttonX, buttonY, buttonWidth, buttonHeight, clickEvent){
	displayText = buttonText;
	font = TTF_OpenFont("arial.ttf", fontSize);
}
Button::~Button()
{
	TTF_CloseFont(font);
}
void Button::Render(SDL_Renderer* renderer) {
	SDL_RenderDrawRect(renderer, new SDL_Rect{ x, y, width, height });
	UIRendering::RenderText(renderer, displayText, x, y, font);
}
void Button::Input(SDL_Event& e)
{
	HandleClickInput(e);
}

