#include "StartPad.h"

StartPad::StartPad(TextureManager* t)
{
	x = 50;
	y = 50;
	poweredState = 0;
	unpoweredTexture = t->GetTexture("StartPadUnpowered");
	partialPoweredTexture = t->GetTexture("StartPadPartialPowered");
	poweredTexture = t->GetTexture("StartPadPowered");
	startLever = new StartingLever(t);
	currentTexture = unpoweredTexture;
	countdown = 6;
}
StartPad::~StartPad() {
	delete startLever;
	delete unpoweredTexture;
	delete partialPoweredTexture;
	delete poweredTexture;
}

void StartPad::Render(SDL_Renderer* renderer, int screenScaling)
{
	startLever->Render(renderer, screenScaling);
	SDL_Rect* dstRect = new SDL_Rect{x * screenScaling, y * screenScaling, 64 * screenScaling, 64 * screenScaling};
	SDL_RenderCopy(renderer, currentTexture, NULL, dstRect);
	delete dstRect;
}

void StartPad::UpdateTexture(string binaryData)
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
	startLever->UpdateTexture(binaryData.substr(2, 1));
	int countdown = ServerManager::IntDecompress(binaryData.substr(3,3));
	if (countdown < 6) {
		//TODO add big countdown
	}
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
