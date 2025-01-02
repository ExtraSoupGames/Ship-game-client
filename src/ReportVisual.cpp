#include "ReportVisual.h"
ReportVisual::ReportVisual(GameReport* report, int pX, int pY, TextureManager* textureManager, int pScreenScaling, int fontSize = 10) {
	displayText = "Time Survived: " + to_string(report->timeSurvived / 10.00).substr(0, 6);
	screenScaling = pScreenScaling;
	font = UIRendering::LoadFontAtPixelHeight(fontSize, screenScaling);
	texture = textureManager->GetTexture("UI\\Button");
	x = pX;
	y = pY;
}
ReportVisual::~ReportVisual()
{
	TTF_CloseFont(font);
}
void ReportVisual::Render(SDL_Renderer* renderer) {
	SDL_RenderCopy(renderer, texture, NULL, new SDL_Rect{x * screenScaling, y * screenScaling, 160 * screenScaling, 17 * screenScaling});
	UIRendering::RenderText(renderer, displayText, (x + 3) * screenScaling, (y + 3) * screenScaling, font);
}
void ReportVisual::Input(SDL_Event& e)
{
}
void ReportVisual::Update() {

}