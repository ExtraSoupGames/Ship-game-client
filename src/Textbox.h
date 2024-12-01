#pragma once
#include "SDL.h"
#include "UIRendering.h"
#include "UIElement.h"
using namespace std;
class Textbox : public ClickableUIElement {
private:
	string currentText;
	bool selected;
	static char GetCharFromEvent(SDL_Event& e);
public:
	void Select();
	void Deselect();
	Textbox(string defaultText, int x, int y, int width, int height);
	void Render(SDL_Renderer* renderer) override;
	void Input(SDL_Event& e) override;
	string GetText();
};