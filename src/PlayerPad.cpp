#include "PlayerPad.h"

PlayerPad::PlayerPad(TextureManager* t)
{
	x = 50;
	y = 50;
	poweredState = 0;
	unpoweredTexture = t->GetTexture("UI\\ButtonHover");
	partialPoweredTexture = t->GetTexture("UI\\Button");
	poweredTexture = t->GetTexture("UI\\ButtonClick");
	currentTexture = unpoweredTexture;
}
PlayerPad::~PlayerPad() {
	delete unpoweredTexture;
	delete partialPoweredTexture;
	delete poweredTexture;
}

void PlayerPad::Render(SDL_Renderer* renderer, int screenScaling)
{
	SDL_Rect* dstRect = new SDL_Rect{x * screenScaling, y * screenScaling, 64 * screenScaling, 64 * screenScaling};
	SDL_RenderCopy(renderer, currentTexture, NULL, dstRect);
	delete dstRect;
}

void PlayerPad::UpdateTexture(string binaryData)
{
	poweredState = ServerManager::IntDecompress(binaryData.substr(0, 2));
	switch (poweredState) {
	case 0:
		currentTexture = unpoweredTexture;
		break;
	case 1:
		currentTexture = partialPoweredTexture;
		break;
	case 2:
		currentTexture = poweredTexture;
		break;
	}
}

Hitbox* PlayerPad::GetLeverBox()
{
	return new Hitbox{x - 16, y - 16, 48, 48};
}

StartingLever::StartingLever(TextureManager* t)
{
	x = 30;
	y = 30;
	unpoweredTexture = t->GetTexture("LeverUnpowered");
	poweredTexture = t->GetTexture("LeverPowered");
	currentTexture = unpoweredTexture;
	powered = false;
}

StartingLever::~StartingLever()
{
	delete currentTexture;
	delete unpoweredTexture;
	delete poweredTexture;
}

void StartingLever::Render(SDL_Renderer* renderer, int screenScaling)
{
	SDL_Rect* dstRect = new SDL_Rect{ x * screenScaling, y * screenScaling, 16 * screenScaling, 16 * screenScaling };
	SDL_RenderCopy(renderer, currentTexture, NULL, dstRect);
	SDL_SetRenderDrawColor(renderer, 255, 255 ,255 ,255);
	SDL_Rect rect = { (x - 16) * screenScaling, (y - 16) * screenScaling, 48 * screenScaling, 48 * screenScaling };
	SDL_RenderDrawRect(renderer, &rect);
}

void StartingLever::UpdateTexture(string binaryData)
{
	powered = binaryData == "1";
	if (powered) {
		currentTexture = poweredTexture;
	}
	else {
		currentTexture = unpoweredTexture;
	}
}

StartPad::StartPad(TextureManager* t) : PlayerPad(t)
{
	startLever = new StartingLever(t);
	countdown = 6;
}
StartPad::~StartPad() {
	delete startLever;
}
void StartPad::Render(SDL_Renderer* renderer, int screenScaling) {
	startLever->Render(renderer, screenScaling);
	PlayerPad::Render(renderer, screenScaling);
}
void StartPad::UpdateTexture(string binaryData) {
	startLever->UpdateTexture(binaryData.substr(2, 1));
	countdown = ServerManager::IntDecompress(binaryData.substr(3, 3));
	if (countdown < 6) {
		//TODO add big countdown
	}
	PlayerPad::UpdateTexture(binaryData);
}