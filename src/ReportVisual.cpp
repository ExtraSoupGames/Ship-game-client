#include "ReportVisual.h"
ReportVisual::ReportVisual(GameReport* report, int pX, int pY, TextureManager* textureManager, int pScreenScaling, int fontSize = 10) {
	displayText = to_string(report->timeSurvived);
	screenScaling = pScreenScaling;
	font = UIRendering::LoadFontAtPixelHeight(fontSize, screenScaling);
	texture = textureManager->GetTexture("UI\\ReportScroll");
	x = pX;
	y = pY;
}
ReportVisual::~ReportVisual()
{
	TTF_CloseFont(font);
}
void ReportVisual::Render(SDL_Renderer* renderer) {
	SDL_RenderCopy(renderer, texture, NULL, new SDL_Rect{x * screenScaling, y * screenScaling, 32 * screenScaling, 32 * screenScaling}); // TODO update the 2 32 values to the actual height and width of the sprite once i draw it
	UIRendering::RenderText(renderer, displayText, (x + 3) * screenScaling, (y + 3) * screenScaling, font);
}
void ReportVisual::Input(SDL_Event& e)
{
}
void ReportVisual::Update() {

}