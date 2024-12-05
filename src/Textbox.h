#pragma once
#include "SDL.h"
#include "UIRendering.h"
#include "UIElement.h"
#include "TextureManager.h"

using namespace std;
class Textbox : public ClickableUIElement {
private:
	string currentText;
	bool selected;
	TTF_Font* font;
	SDL_Texture* currentTexture;
	SDL_Texture* hoverTexture;
	SDL_Texture* unHoverTexture;
	int screenScaling;
	static char GetCharFromEvent(SDL_Event& e);
public:
	void Select();
	void Deselect();
	Textbox(string defaultText, int x, int y, int pScreenScaling, TextureManager* textureManager, int fontSize);
	~Textbox();
	void Render(SDL_Renderer* renderer) override;
	void Input(SDL_Event& e) override;
	string GetText();
	void OnHover() override;
	void OnUnHover() override;
};