#pragma once
#include <string>
#include <vector>
#include "SDL_mixer.h"
#include "SDL.h"
using namespace std;
class Sound {
private:
	Mix_Chunk* audio;
public:
	Sound(string name, string filename = "");
	~Sound();
	void Play();
	string soundName;
};
class SoundManager
{
private:
	vector<Sound*>* sounds;
public:
	SoundManager();
	~SoundManager();
	void PlaySound(string soundName);
	void LoadSounds();
};

