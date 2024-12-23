#include "Animation.h"
Animation::Animation(string name, TextureManager* t, double frameDuration, bool loop)
{
	frames = t->LoadAnimation(name);
	totalDuration = frameDuration * frames.size();
	looping = loop;
}
Animation::Animation(string name, TextureManager* t, double frameDuration, bool loop, string paletteName)
{
	frames = t->LoadAnimation(name, paletteName);
	totalDuration = frameDuration * frames.size();
	looping = loop;
}
float Animation::frameDuration() {
	return totalDuration / frames.size();
}
Animatable::Animatable(vector<string> animationNames, TextureManager* t, int defaultAnim) {
	animations = new vector<Animation*>();
	animating = true;
	currentAnimation = 0;
	defaultAnimation = defaultAnim;
	currentFrame = 0;
	lastAnimated = SDL_GetTicks();
	texture = t->GetErrorTexture();
	for (string animation : animationNames) {
		bool animationIsLooping = animation.at(0) == '%';
		animations->push_back(new Animation(animation, t, 100, animationIsLooping));
	}
}
Animatable::Animatable(vector<string> animationNames, TextureManager* t, string paletteName, int defaultAnim)
{
	animations = new vector<Animation*>();
	animating = true;
	currentAnimation = 0;
	defaultAnimation = defaultAnim;
	currentFrame = 0;
	lastAnimated = SDL_GetTicks();
	texture = t->GetErrorTexture();
	for (string animation : animationNames) {
		bool animationIsLooping = animation.at(0) == '%';
		animations->push_back(new Animation(animation, t, 100, animationIsLooping, paletteName));
	}
}
Animatable::~Animatable() {
	delete[] animations;
}
void Animatable::PlayAnimation(int ID) {
	if (ID >= animations->size() || ID < 0) {
		cout << "Animation ID provided was invalid, ID: " << ID << endl;
		return;
	}
	if (ID != currentAnimation) {
		//if changing to a new animation, reset current frame in case new animation is shorter
		currentFrame = 0;
		currentAnimation = ID;
		lastAnimated = SDL_GetTicks();
		SetTexture(animations->at(currentAnimation)->frames[currentFrame].texture);
	}
	animating = true;
	currentAnimation = ID;
}
void Animatable::StartAnimating()
{
	if (!animating) {
		lastAnimated = SDL_GetTicks();
	}
	animating = true;
}
void Animatable::StopAnimating() {
	animating = false;
}
void Animatable::UpdateAnimation() {
	if (animating) {
		float frameDuration = animations->at(currentAnimation)->frameDuration();
		if (SDL_GetTicks() - lastAnimated > frameDuration) {
			SetTexture(animations->at(currentAnimation)->frames[currentFrame].texture);
			lastAnimated += frameDuration;
			if (!(currentFrame < animations->at(currentAnimation)->frames.size() - 1)) {
				currentFrame = 0;
				if (!animations->at(currentAnimation)->looping) {
					currentAnimation = defaultAnimation;
				}
			}
			else {
				currentFrame++;
			}
		}
	}
}
void Animatable::SetTexture(SDL_Texture* t)
{
	texture = t;
}
void Animatable::Render(SDL_Renderer* renderer, int xPos, int yPos, int width, int height, GlobalSettingsProfile* settings) {
	int screenScale = settings->screenScaling();
	const SDL_Rect *dst = new SDL_Rect{ xPos * screenScale, yPos * screenScale, width * screenScale, height * screenScale };
	SDL_RenderCopy(renderer, texture, NULL, dst);
}