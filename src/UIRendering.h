#pragma once
#include <iostream>
#include "SDL.h"
#include "SDL_test_font.h"
#include "SDL_ttf.h"
using namespace std;
class UIRendering {
public:
	static void RenderText(SDL_Renderer* renderer, string textToRender, int x, int y, TTF_Font* font, SDL_Color textColour = { 205, 148, 68 });
	static TTF_Font* LoadFontAtPixelHeight(int fontSize, int screenScaling);
};