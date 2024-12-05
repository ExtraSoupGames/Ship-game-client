#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "SDL.h"
struct Frame;
using namespace std;
class TextureManager
{
private:
	map<string, SDL_Texture*>* textures;
	Frame* LoadFrame(string name, string suffix);
	void LoadTextures();
	bool LoadTexture(string name);
	SDL_Renderer* renderer;
public:
	TextureManager(SDL_Renderer* pRenderer);
	~TextureManager();
	void InitializeAllTextures();
	SDL_Texture* GetTexture(string name);
	SDL_Texture* GetErrorTexture();
	vector<Frame> LoadAnimation(string name);
};

