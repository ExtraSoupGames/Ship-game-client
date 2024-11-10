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
	string host;
	int port;
public:
	ServerManager(UDPsocket* serverSocket);
	void SetHost(string pHost, int pPort);
	void SendMessage(string messageContents);
	//turn incoming string data into a string of 1s and 0s
	static string CharToBinary(char* inData, int dataLength);
	//compress position to 16 bits (rounds position to nearest 4 pixels
	static string PositionCompress(int positionX, int positionY);
	//16 bits total, int* of length 2 position decompress
	static int* PositionDecompress(string binaryIn);
	//compress an int into 32 bits of binary
	static string IntCompress(int inInt);
	//32 bit int decompression
	static int IntDecompress(string binaryIn);
	//512 bit double decompression
	static double DoubleDecompress(string binaryIn);
	//64 bit timestamp as integer decompression
	static __int64 TimestampDecompress(string binaryIn);
	//7 bit player state decompression
	static PlayerState PlayerStateDecompress(string binaryIn);
	//7 bit player state compression
	static string PlayerStateCompress(PlayerState state);
	//undefined length (should be 512 bits) server host decompression
	static string DecompressHost(string binaryIn);
};