#pragma once
#include "SDL.h"
#include "UIRendering.h"
#include "UIelement.h"
#include "TextureManager.h"
#include "PlayerController.h"
using namespace std;
class HealthBar : public UIElement{
private:
	SDL_Texture* texture;
	PlayerController* player;
	int screenScaling;
	int x;
	int y;
public:
	HealthBar(TextureManager* t, int pX, int pY, int pScreenScaling, PlayerController* playerController);
	void Input(SDL_Event& e) override;
	void Update() override;
	void Render(SDL_Renderer* renderer) override;
};