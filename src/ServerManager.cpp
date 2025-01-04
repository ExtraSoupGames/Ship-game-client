#include "ServerManager.h"
#include "PlayerController.h"
PlayerState::PlayerState(int pDirection, int pMovementState, int pAttackState, int pAnimationState) {
    direction = pDirection;
    movementState = pMovementState;
    attackState = pAttackState;
    animationState = pAnimationState;
}

void ServerManager::Send(bitset<32> headerData, vector<bitset<8>> packetData, IPaddress address, int length)
{
    // Prepare the packet to send, based on the size of the data
    UDPpacket* packet = SDLNet_AllocPacket(length);
    //handle errors from the packet preperation
    if (!packet) {
        std::cerr << "SDLNet_AllocPacket: " << SDLNet_GetError() << std::endl;
        SDLNet_UDP_Close(socket);
        SDLNet_Quit();
        SDL_Quit();
    }
    //copy the header to the start of the packet data
    memcpy(packet->data, &headerData, 4);
    //iterate through the data, copying each byte in one at a time
    for (int i = 0; i < packetData.size(); i++) {
        memcpy(packet->data + 4 + i, &packetData.at(i), 1);
    }
    //set the length of the packet
    packet->len = length + 1;
    //set the address of the packet to the IP address we found earlier
    packet->address = address;
    // Send the packet and handle any errors
    if (SDLNet_UDP_Send(socket, -1, packet) == 0) {
        std::cerr << "SDLNet_UDP_Send: " << SDLNet_GetError() << std::endl;
    }
}

ServerManager::ServerManager(UDPsocket serverSocket, int pClientID) {
	socket = serverSocket;
    host = "255.255.255.255";
    port = 55555;
    clientID = pClientID;
    importantMessages = new vector<ImportantMessage*>();
}
ServerManager::~ServerManager() {
    delete importantMessages;
}
string ServerManager::ToString()
{
    return "Server Manager Properties, Host:" + host + " port: " + to_string(port);
}
void ServerManager::SetHost(string pHost, int pPort) {
    pHost.erase(std::remove(pHost.begin(), pHost.end(), '\0'), pHost.end());
    host = pHost;
    port = pPort;
}
void ServerManager::ResetSocket() {
    if (socket) {
        SDLNet_UDP_Close(socket);
    }
    socket = SDLNet_UDP_Open(0);
    if (!socket) {
        std::cout << "SDLNet_UDP_Open: " << SDLNet_GetError() << std::endl;
        SDLNet_Quit();
        SDL_Quit();
        throw new exception("Error opening socket");
    }
}
void ServerManager::SendMessage(string messageBinary) {
    //declare the IPadress variable
    IPaddress ip;
    //assign it by resolving the host declared in the server manager
    if (SDLNet_ResolveHost(&ip, host.c_str(), port) == -1) {
        std::cerr << "SDLNet_ResolveHost error: " << SDLNet_GetError() << std::endl;
        return;
    }
    //pad the message to a byte
    while ((messageBinary.size() % 8) != 0) {
        messageBinary.append("0");
    }
    //compress the bit data into a string for sending
    vector<bitset<8>> newData;
    string compressedData;
    //iterate through each 8 bits of the message
    for (int i = 0; i < messageBinary.size(); i+=8) {
        //extract the relevant data
        string byteData = messageBinary.substr(i, 8);
        // convert this into a bitset of 8 bits and add to data vector
        newData.push_back(bitset<8>(stoi(byteData, nullptr, 2)));
    }
    //calculate the size of the compressed data, with the header
    __int32 bytesWithHeader = newData.size() + 4;
    bitset<32> bytesHeader(bytesWithHeader);
    Send(bytesHeader, newData, ip, bytesWithHeader);
}
void ServerManager::SendImportantMessage(string binaryContents)
{
    ImportantMessage* messageToSend = new ImportantMessage(binaryContents, nextMessageID, clientID);
    nextMessageID++;
    importantMessages->push_back(messageToSend);
}
void ServerManager::SendAllImportantMessages()
{
    for (ImportantMessage* im : *importantMessages) {
        SendMessage(im->GetMessage());
    }
}
void ServerManager::ReceiveImportantMessageConfirmation(string binaryIn) {
    int messageID = IntDecompress(binaryIn.substr(0, 32));
    int intendedClient = IntDecompress(binaryIn.substr(32, 32));
    if (intendedClient != clientID) {
        //this message was intended for a different client, ignore
        return;
    }
    for (ImportantMessage* im : *importantMessages) {
        if (messageID != im->messageID) {
            //dont remove any messages that arent the one that we just received confirmation for
            continue;
        }
        //find the correct message and remove it as we dont need to send it anymore
        importantMessages->erase(remove(importantMessages->begin(), importantMessages->end(), im));
        return;
    }
}
void ServerManager::SendImportantMessageConfirmation(string messageIn, int clientID) {
    string returnHeader = "1110"; // this is the header code for all confirmation messages from client to server
    string messageID = messageIn.substr(0, 32);
    string clntID = IntCompress(clientID);
    SendMessage(returnHeader.append(messageID.append(clntID)));
}
string ServerManager::CharToBinary(char* inData, int dataLength) {
    stringstream ss;
    for (int i = 0; i < dataLength;i++) {
        int nextInt = (int)*(inData + i);
        bitset<8> inByte(nextInt);
        ss << inByte;
    }
    return ss.str().substr(0, dataLength * 8);
}
string ServerManager::PositionCompress(int positionX, int positionY) {
    //all positions are divided by 4, then remultiplied, so they are essentially being rounded to save on packet size
    positionX = positionX / 4;
    positionY = positionY / 4;
    //we then convert the 2 integers into a bitset of 8 bits, then convert that into a string of 0s and 1s, ready to be transmitted
    bitset<8> xbits = (__int8)positionX;
    bitset<8> ybits = (__int8)positionY;
    stringstream ss;
    ss << xbits.to_string();
    ss << ybits.to_string();
    return ss.str();
}
int* ServerManager::PositionDecompress(string binaryIn) {
    if (binaryIn.size() != 16) {
        cout << "Error decompressing position, expected data size: 16 bits, received data size: " << binaryIn.size() << " bits." << endl;
        return new int(-1);
    }
    bitset<8> xbits = stoi(binaryIn.substr(0, 8), nullptr, 2);
    bitset<8> ybits = stoi(binaryIn.substr(8, 8), nullptr, 2);
    int* positionValues = new int[2] {(int)xbits.to_ulong() * 4, (int)ybits.to_ulong() * 4};
    return positionValues;
}
string ServerManager::IntCompress(int inInt){
    bitset<32> bits = inInt;
    stringstream ss;
    ss << bits.to_string();
    return ss.str();
}
int ServerManager::IntDecompress(string binaryIn) {
    bitset<32> bits(binaryIn);
    int out = (int)bits.to_ulong();
    return out;
}
double ServerManager::DoubleDecompress(string binaryIn) {
    bitset<512> bits(binaryIn);
    long out = (long)bits.to_ulong();
    return out;
}
__int64 ServerManager::TimestampDecompress(string binaryIn) {
    bitset<64> bits(binaryIn);
    __int64 out = (__int64)strtoull(bits.to_string<char,char_traits<char>,allocator<char> >().c_str(), NULL, 2);
    return out;
}
PlayerState ServerManager::PlayerStateDecompress(string binaryIn)
{
    if (binaryIn.size() != 9) {
        cout << "PlayerState binary in must be exactly 9 bits" << endl;
    }
    int direction = IntDecompress(binaryIn.substr(0, 3));
    int movementState = IntDecompress(binaryIn.substr(3, 2));
    int attackState = IntDecompress(binaryIn.substr(5, 2));
    int animationState = IntDecompress(binaryIn.substr(7,2));
    return PlayerState(direction,  movementState, attackState, animationState);
}
string ServerManager::PlayerStateCompress(PlayerState state)
{
    bitset<3> dirBits(state.direction);
    bitset<2> movBits(state.movementState);
    bitset<2> atkBits(state.attackState);
    bitset<2> animBits(state.animationState);
    string out = dirBits.to_string().append(movBits.to_string().append(atkBits.to_string().append(animBits.to_string())));
    return out;
}
string ServerManager::DecompressHost(string binaryIn)
{
    string out = "";
    for (int i = 0; i < binaryIn.size() - 7; i += 8) {
        char newChar = (char)stoi(binaryIn.substr(i, 8), nullptr, 2);
        out = out + newChar;
    }
    return out;
}
string ServerManager::DecompressString(string binaryIn) {
    string out = "";
    for (int i = 0; i < binaryIn.size() - 7; i += 8) {
        char newChar = (char)stoi(binaryIn.substr(i, 8), nullptr, 2);
        out = out + newChar;
    }
    return out;
}