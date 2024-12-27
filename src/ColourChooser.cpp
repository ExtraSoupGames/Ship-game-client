#include "ColourChooser.h"

ColourChooser::ColourChooser(ServerManager* pServer, int pClientID)
{
	server = pServer;
	clientID = pClientID;
	selectedColour = "";
	availableColours = new vector<bool>{true, true, true, true, true, true };
}
ColourChooser::~ColourChooser() {
	delete availableColours;
}

void ColourChooser::ColourChosen(int colourID)
{
	//send the server a request, first 4 digits is a player select request code, next 3 is colour code, then the client ID at the end
	switch (colourID) {
	case 1:
		server->SendImportantMessage("0101000" + server->IntCompress(clientID));
		break;
	case 2:
		server->SendImportantMessage("0101001" + server->IntCompress(clientID));
		break;
	case 3:
		server->SendImportantMessage("0101010" + server->IntCompress(clientID));
		break;
	case 4:
		server->SendImportantMessage("0101011" + server->IntCompress(clientID));
		break;
	case 5:
		server->SendImportantMessage("0101100" + server->IntCompress(clientID));
		break;
	case 6:
		server->SendImportantMessage("0101101" + server->IntCompress(clientID));
		break;
	}
}

void ColourChooser::IncomingData(string message)
{
	for (int i = 0; i < 6; i++) {
		if (message[i] == '1') {
			availableColours->at(i) = true;
		}
		else {
			availableColours->at(i) = false;
		}
	}
}

void ColourChooser::SelectedColour(string colourCode)
{
	cout << "My Colour is now: " << colourCode << endl;
	if (selectedColour != "") {
		server->SendImportantMessage("0110" + selectedColour); // code for old colour to be freed up on the server
	}
	selectedColour = colourCode;
}
