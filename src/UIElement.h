#pragma once
#include "SDL.h"
#include <functional>
using namespace std;
class UIElement {
public:
	virtual void Render(SDL_Renderer* renderer) = 0;
	virtual void Input(SDL_Event& e) = 0;
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
public:
	ClickableUIElement(int x, int y, int width, int height, function<void()> clickFunc, function<void()> clickOffFunc = 0);
	bool IsInBounds(int clickX, int clickY);
	void HandleClickInput(SDL_Event& e);
	void OnClick();
	void OnClickOff();
	virtual void OnHover() = 0;
	virtual void OnUnHover() = 0;
};