#pragma once
#include "SDL.h"
#include <iostream>
using namespace std;
class ServerLauncher {
public:
	string serverName;
	ServerLauncher(string nameForServer);
	void RunServer();
};