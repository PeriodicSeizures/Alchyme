#include "Server.hpp"

std::unique_ptr<Server> server;

#ifdef _WIN32
#include <Windows.h>
BOOL WINAPI ConsoleHandler(DWORD CEvent) {
    if (CEvent == CTRL_CLOSE_EVENT) {
        server.reset();
    }
    return TRUE;
}
#endif

bool loadSettings(std::unordered_map<std::string, std::string>& settings) {
    std::ifstream file;

    file.open("settings.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t index = line.find(':');

            std::string key = line.substr(0, index);
            std::string value = line.substr(index + 2);
            settings.insert({ key, value });
        }

        file.close();
        return true;
    }
    else {
        return false;
        // else, generate things from scratch
    }
}

int main() {

#ifdef _WIN32
    if (SetConsoleCtrlHandler(
        (PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE) {
        std::cout << "failed to register exit handler";
    }
#endif

    std::unordered_map<std::string, std::string> settings;
    if (!loadSettings(settings)) {
        std::cerr << "settings file not found\n";
        return 0;
    }

    //Server server;
    server = std::make_unique<Server>(std::move(settings));

    //try {
        server->Start();
    //}
    //catch (std::exception& e) {
    //    std::cout << "fatal error: " << e.what() << "\n";
    //}
    
    return 0;
}