#pragma once
#include "SDL.h"
#include <functional>
#include "TextureManager.h"
using namespace std;
enum UIElementSize {
	Tiny,
	Small,
	Normal
};
class UIElement {
public:
	virtual void Render(SDL_Renderer* renderer) = 0;
	virtual void Input(SDL_Event& e) = 0;
	virtual void Update() = 0;
};
class ClickableUIElement : public UIElement {
	//todo move clickable functionality here
protected:
	int x;
	int y;
	int width;
	int height;
	function<void()> clickFunction;
	function<void()> clickAwayFunction;
	SDL_Texture* currentTexture;
	SDL_Texture* unHoverTexture;
	SDL_Texture* clickTexture;
	SDL_Texture* hoverTexture;
public:
	ClickableUIElement(int x, int y, int pScreenScaling , UIElementSize size , TextureManager* textureManager , function<void()> clickFunc, function<void()> clickOffFunc = 0);
	bool IsInBounds(int clickX, int clickY);
	void OnClick();
	void OnClickOff();
	void Update();
	void HandleClickInput(SDL_Event& e);
	void Render(SDL_Renderer* renderer) override;
};