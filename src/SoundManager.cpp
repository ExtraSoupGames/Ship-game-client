#include"SoundManager.h"

Sound::Sound(string name, string filename)
{
	if (filename == "") {
		filename = "Sounds\\" + name + ".wav";
	}
	soundName = name;
	filename = "..\\Assets\\" + filename;
	audio = Mix_LoadWAV(filename.c_str());
	if (audio == NULL) {
		throw new exception("failed to load audio clip, add .wav to assets");
	}
}

Sound::~Sound()
{
	Mix_FreeChunk(audio);
	delete audio;
}

void Sound::Play()
{
	Mix_PlayChannel(-1, audio, 0);
}

SoundManager::SoundManager()
{
	sounds = new vector<Sound*>();
}

SoundManager::~SoundManager()
{
	delete sounds;
}

void SoundManager::PlaySound(string soundName)
{
	for (Sound* s : *sounds) {
		if (s->soundName == soundName) {
			s->Play();
		}
	}
}

void SoundManager::LoadSounds()
{
	vector<string> soundsToAdd = vector<string>{
	"testSound"
	};
	for (string s : soundsToAdd) {
		sounds->push_back(new Sound(s));
	}
}
