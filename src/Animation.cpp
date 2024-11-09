#include "Animation.h"
Animation::Animation(string name, TextureManager* t, float animationDuration, bool loop)
{
	frames = t->LoadAnimation(name);
	totalDuration = animationDuration;
	looping = loop;
}
float Animation::frameDuration() {
	return totalDuration / frames.size();
}
Animatable::Animatable(vector<string> animationNames, TextureManager* t) {
	animations = *new vector<Animation>();
	animating = true;
	currentAnimation = 0;
	defaultAnimation = 0;
	currentFrame = 0;
	lastAnimated = SDL_GetTicks();
	texture = t->GetErrorTexture();
	for (string animation : animationNames) {
		bool animationIsLooping = animation.at(0) == '%';
		animations.push_back(*new Animation(animation, t, 800, animationIsLooping));
	}
}
void Animatable::PlayAnimation(int ID) {
	if (ID >= animations.size() || ID < 0) {
		cout << "Animation ID provided was invalid, ID: " << ID << endl;
		return;
	}
	if (ID != currentAnimation) {
		//if changing to a new animation, reset current frame in case new animation is shorter
		currentFrame = 0;
		currentAnimation = ID;
		lastAnimated = SDL_GetTicks();
		SetTexture(animations[currentAnimation].frames[currentFrame].texture);
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
		float frameDuration = animations[currentAnimation].frameDuration();
		if (SDL_GetTicks() - lastAnimated > frameDuration) {
			SetTexture(animations[currentAnimation].frames[currentFrame].texture);
			lastAnimated += frameDuration;
			if (!(currentFrame < animations[currentAnimation].frames.size() - 1)) {
				currentFrame = 0;
				if (!animations[currentAnimation].looping) {
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
void Animatable::Render(SDL_Renderer* renderer, int xPos, int yPos, int width, int height) {
	const SDL_Rect *dst = new SDL_Rect{ xPos, yPos, width, height};
	SDL_RenderCopy(renderer, texture, NULL, dst);
}