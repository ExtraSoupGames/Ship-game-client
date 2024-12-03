#pragma once
#include <vector>
#include <bitset>
#include <iostream>
#include <SDL.h>
#include <SDL_net.h>
#include <sstream>
#include <algorithm>
#include "Data.h"
using namespace std;

struct ImportantMessage;
class ServerManager {
private:
	UDPsocket* socket;
	string host;
	int port;
	vector<ImportantMessage*>* importantMessages;
	int nextMessageID = 0;
	int clientID;
public:
	ServerManager(UDPsocket* serverSocket, int pClientID);
	~ServerManager();
	string ToString();
	void SetHost(string pHost, int pPort);
	void SendMessage(string binaryIn);
	//send an important message that will keep sending until confirmation that it has been sent is received - do not use unless required as it is much slower
	void SendImportantMessage(string binaryContents);
	//send all important messages that havent been confirmed
	void SendAllImportantMessages();
	//called when a packet is received that has a confirmation header code
	void ReceiveImportantMessageConfirmation(string binaryIn);
	//confirm that an important message from the server has been received
	void SendImportantMessageConfirmation(string messageIn, int clientID);
#pragma region compression
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
	//undefined length
	static string DecompressString(string binaryIn);
#pragma endregion compression
};
//messages to be sent that need confirmation from the server to make sure they are sent
struct ImportantMessage {
	int messageID;
	int clientID;
	string binaryContents;
	ImportantMessage(string pBinaryContents, int pMessageID, int pClientID) {
		binaryContents = pBinaryContents;
		messageID = pMessageID;
		clientID = pClientID;
	}
	string GetMessage() {
		string header = binaryContents.substr(0, 4);
		string msgID = ServerManager::IntCompress(messageID);
		string clntID = ServerManager::IntCompress(clientID);
		string messageData = binaryContents.substr(4);
		return header + msgID + clntID + messageData;
	}
};