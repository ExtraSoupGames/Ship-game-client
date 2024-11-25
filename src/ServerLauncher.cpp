#include "ServerLauncher.h"
#include "Windows.h"
ServerLauncher::ServerLauncher() {
#if defined(_WINDOWS)
    cout << "Server Launcher created for windows" << endl;
#else
    cout << "Server Launcher not created, wrong platform" << endl;
    throw runtime_error("Creating Servers currently only works on windows, sorry :/");
#endif
}
void ServerLauncher::RunServer() {
	std::string command = "\"C:\\Program Files\\Java\\jdk-21\\bin\\java.exe\" -jar ..\\Assets\\ServerBuild\\Pong.jar";
    // Setup the process information
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;

    // Create the server process but don't wait for it
    if (CreateProcess(
        NULL,                    // No module name (use command line)
        (LPSTR)command.c_str(),   // Command line
        NULL,                     // Process handle not inheritable
        NULL,                     // Thread handle not inheritable
        FALSE,                    // Set handle inheritance to FALSE
        DETACHED_PROCESS,         // Create the process detached
        NULL,                     // Use parent's environment block
        NULL,                     // Use parent's starting directory
        &si,                      // Pointer to STARTUPINFO structure
        &pi)                      // Pointer to PROCESS_INFORMATION structure
        ) {
        std::cout << "Server started successfully!" << std::endl;
    }
    else {
        std::cout << "Failed to start the server." << std::endl;
    }
}