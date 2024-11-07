#pragma once
#include <vector>
#include <bitset>
#include <iostream>
#include <SDL.h>
#include <SDL_net.h>
#include <sstream>
#include "Data.h"
using namespace std;
class ServerManager {
private:
	UDPsocket* socket;
public:
	ServerManager(UDPsocket* serverSocket);
	void SendMessage(string messageContents);
	string CharToBinary(char* inData, int dataLength);
	string PositionCompress(int positionX, int positionY);
	int* PositionDecompress(string binaryIn);
	string IntCompress(int inInt);
	int IntDecompress(string binaryIn);
	double DoubleDecompress(string binaryIn);
	__int64 TimestampDecompress(string binaryIn);
	PlayerState PlayerStateDecompress(string binaryIn);
	string PlayerStateCompress(PlayerState state);
};