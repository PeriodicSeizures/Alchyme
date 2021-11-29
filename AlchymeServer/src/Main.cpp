// AlchymeServer
//

#include <iostream>
#include <fstream>
#include "Server.hpp"

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

int main() {
    std::unordered_map<std::string, std::string> settings;
    if (!loadSettings(settings)) {
        std::cerr << "settings file not found\n";
        return 0;
    }
	Server s(std::move(settings));

	s.StartListening();

	return 0;
}
