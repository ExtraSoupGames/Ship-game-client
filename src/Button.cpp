#include "Button.h"
#include "GameState.h"
Button::Button(string buttonText, int buttonX, int buttonY, int buttonWidth, int buttonHeight, function<void()> clickEvent)
	: ClickableUIElement(buttonX, buttonY, buttonWidth, buttonHeight, clickEvent){
	displayText = buttonText;
}
void Button::Render(SDL_Renderer* renderer) {
	SDL_RenderDrawRect(renderer, new SDL_Rect{ x, y, width, height });
	UIRendering::RenderText(renderer, displayText, x, y);
}
void Button::Input(SDL_Event& e)
{
	HandleClickInput(e);
}

