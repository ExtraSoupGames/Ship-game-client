#pragma once
#include <vector>
#include <iostream>
#include <thread>
#include <SDL_thread.h>
#include "TextureManager.h"
#include "GlobalSettings.h"
#include "SDL.h"
using namespace std;
struct Frame {
	SDL_Texture* texture;
	Frame(SDL_Texture* t) {
		texture = t;
	}
};
struct Animation {
	Animation(string name, TextureManager* t, double frameDuration, bool loop);
	Animation(string name, TextureManager* t, double frameDuration, bool loop, string paletteName);
	vector<Frame> frames;
	float totalDuration;
	float frameDuration();
	bool looping;
};
class Animatable {
private:
	SDL_Texture* texture;
	int defaultAnimation;
	int currentFrame;
	bool animating;
	double lastAnimated;
	vector<Animation*>* animations;
	void SetTexture(SDL_Texture* t);
protected:
	int currentAnimation;
public:
	Animatable(vector<string> animationNames, TextureManager* t, int defaultAnim = 0);
	Animatable(vector<string> animationNames, TextureManager* t, string paletteName, int defaultAnim = 0);
	~Animatable();
	void PlayAnimation(int ID);
	void StartAnimating();
	void StopAnimating();
	void UpdateAnimation();
	void Render(SDL_Renderer* renderer, int xPos, int yPos, int width, int height, GlobalSettingsProfile* settings);
};