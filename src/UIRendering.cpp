#include "UIRendering.h"
void UIRendering::RenderText(SDL_Renderer* renderer, string textToRender, int x, int y, TTF_Font* font, SDL_Color textColour) {
	SDL_Color color = textColour;
	SDL_Surface* surface = TTF_RenderText_Solid(font,textToRender.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { x, y, texW, texH };
	SDL_RenderCopy(renderer, texture, NULL, &dstrect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}
TTF_Font* UIRendering::LoadFontAtPixelHeight(int fontPixels, int screenScaling) {
	TTF_Font* tempFont = TTF_OpenFont("PixelatedElegance.ttf", fontPixels);
	int pixelHeight;
	int pixelWidth;
	TTF_SizeText(tempFont, "A", &pixelHeight, &pixelWidth);
	double requiredScaleFactor = (static_cast<double>(fontPixels) / pixelHeight) / 3.875; //i dont know why it needs to be divided by 3.875
	TTF_CloseFont(tempFont);
	return TTF_OpenFont("PixelatedElegance.ttf", fontPixels * screenScaling * requiredScaleFactor);
}