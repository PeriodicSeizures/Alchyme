#include "Game.hpp"
#include "Server.hpp"
#include <memory>

std::unique_ptr<Server> server;

bool loadSettings(std::unordered_map<std::string, std::string>& settings) {
    std::ifstream file;

    file.open("./data/settings.txt");
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

void RunMainGame() {

	// load settings file


    server = std::make_unique<Server>();

    while ()

}

Server* GetServer() {
	return server.get();
}