#pragma once
#include <vector>
#include <iostream>
#include "GlobalSettings.h"
#include "SDL.h"
#include "ServerManager.h"
using namespace std;
class ColourChooser {
	vector<bool>* availableColours;
	ServerManager* server;
	string selectedColour;
	int clientID;
public:
	ColourChooser(ServerManager* server, int pClientID);
	~ColourChooser();
	void ColourChosen(int colourID);
	void IncomingData(string message);
	void SelectedColour(string colourCode);
};