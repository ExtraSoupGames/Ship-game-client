#include "PlayerPad.h"

PlayerPad::PlayerPad(TextureManager* t)
{
	x = 50;
	y = 100;
	poweredState = 0;
	unpoweredTexture = t->GetTexture("UI\\ButtonTinyHover");
	partialPoweredTexture = t->GetTexture("UI\\ButtonTiny");
	poweredTexture = t->GetTexture("UI\\ButtonTinyClick");
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


StartingLever::StartingLever(TextureManager* t)
{
	x = 70;
	y = 30;
	unpoweredTexture = t->GetTexture("UI\\StartLeverUnpowered");
	poweredTexture = t->GetTexture("UI\\StartLeverPowered");
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
	SDL_Rect* dstRect = new SDL_Rect{ x * screenScaling, y * screenScaling, 32 * screenScaling, 32 * screenScaling };
	SDL_RenderCopy(renderer, currentTexture, NULL, dstRect);
	SDL_SetRenderDrawColor(renderer, 255, 255 ,255 ,255);
	SDL_Rect rect = { (x - 30) * screenScaling, (y - 30) * screenScaling, 76 * screenScaling, 76 * screenScaling };
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

Hitbox* StartingLever::GetHitbox()
{
	return new Hitbox{x-30, y-30, 76, 76};
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
Hitbox* StartPad::GetLeverBox()
{
	return startLever->GetHitbox();
}