#include "ServerManager.h"
#include "PlayerController.h"
PlayerState::PlayerState(int pDirection, int pMovementState, int pAttackState) {
    direction = pDirection;
    movementState = pMovementState;
    attackState = pAttackState;
}

ServerManager::ServerManager(UDPsocket* serverSocket, int pClientID) {
	socket = serverSocket;
    host = "255.255.255.255";
    port = 55555;
    clientID = pClientID;
    importantMessages = *new vector<ImportantMessage*>();
}
void ServerManager::SetHost(string pHost, int pPort) {
    host = pHost;
    port = pPort;
}
void ServerManager::SendMessage(string messageBinary) {
    IPaddress ip;
    SDLNet_ResolveHost(&ip, host.c_str(), port); // Connect to the server
    //pad the message to a byte (only first 7 bits used of each byte)
    while ((messageBinary.size() % 7) != 0) {
        messageBinary.append("0");
    }
    //compress the bit data into a string for sending
    string compressedData;
    for (int i = 0; i < messageBinary.size() / 7; i++) {
        string byteData = messageBinary.substr(i * 7, 7);
        byteData.append("1"); // each 7 digits of data is sent as a byte with a 1 at the end, this way no bytes are empty as this causes problems
        compressedData.push_back((static_cast<char>(stoi(byteData, nullptr, 2))));
    }
    int bytes = compressedData.size();
    // Prepare the packet to send
    UDPpacket* packet = SDLNet_AllocPacket(bytes);
    if (!packet) {
        std::cerr << "SDLNet_AllocPacket: " << SDLNet_GetError() << std::endl;
        SDLNet_UDP_Close(*socket);
        SDLNet_Quit();
        SDL_Quit();
    }

    //copy the compressed string data into the packet
    strcpy((char*)packet->data, compressedData.c_str());
    //cout << bytes << endl;
    packet->len = bytes + 1;
    packet->address = ip;

    // Send the packet
    if (SDLNet_UDP_Send(*socket, -1, packet) == 0) {
        std::cerr << "SDLNet_UDP_Send: " << SDLNet_GetError() << std::endl;
    }
}
void ServerManager::SendImportantMessage(string binaryContents)
{
    ImportantMessage* messageToSend = new ImportantMessage(binaryContents, nextMessageID, clientID);
    nextMessageID++;
    importantMessages.push_back(messageToSend);
}
void ServerManager::SendAllImportantMessages()
{
    for (ImportantMessage* im : importantMessages) {
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
    for (ImportantMessage* im : importantMessages) {
        if (messageID != im->messageID) {
            //dont remove any messages that arent the one that we just received confirmation for
            continue;
        }
        //find the correct message and remove it as we dont need to send it anymore
        importantMessages.erase(remove(importantMessages.begin(), importantMessages.end(), im));
        cout << "successful hit fully processed" << endl;
        return;
    }
}
void ServerManager::SendImportantMessageConfirmation(string messageIn, int clientID) {
    string returnHeader = "1011"; // this is the header code for all confirmation messages from client to server
    string messageID = messageIn.substr(0, 32);
    string clntID = IntCompress(clientID);
    SendMessage(returnHeader.append(messageID.append(clntID)));
}
string ServerManager::CharToBinary(char* inData, int dataLength) {
    string data = inData;
    stringstream ss;
    for (char c : data) {
        bitset<8> byte = c;
        ss << byte.to_string().erase(7); // removes the last bit as it is always 1
    }
    return ss.str().substr(0, dataLength * 7);
}
string ServerManager::PositionCompress(int positionX, int positionY) {
    positionX = positionX / 4;
    positionY = positionY / 4;
    //data is made smaller, then resized at decompression
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
    if (binaryIn.size() != 7) {
        cout << "PlayerState binary in must be exactly 7 bits" << endl;
    }
    int direction = IntDecompress(binaryIn.substr(0, 3));
    int movementState = IntDecompress(binaryIn.substr(3, 5));
    int attackState = IntDecompress(binaryIn.substr(5, 7));
    return PlayerState(direction,  movementState, attackState);
}
string ServerManager::PlayerStateCompress(PlayerState state)
{
    bitset<3> dirBits(state.direction);
    bitset<2> movBits(state.movementState);
    bitset<2> atkBits(state.attackState);
    string out = dirBits.to_string().append(movBits.to_string().append(atkBits.to_string()));
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
