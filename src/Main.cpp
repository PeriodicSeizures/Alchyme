#include "AlchymeClient.h"
#include "AlchymeServer.hpp"

#include <iostream>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv) {

	// open file impl.txt

	initLogger();

	std::fstream f;
	f.open("./data/impl.txt");
	if (f.is_open()) {
		std::string line;
		std::getline(f, line);
		f.close();
		if (line == "client") {
			AlchymeGame::RunClient();
		}
		else if (line == "server")
			AlchymeGame::RunServer();
		else
			LOG(ERROR) << "Unknown impl in impl.txt (must be server or client)";
	} else
		LOG(ERROR) << "./data/impl.txt not found";

	return 0;

	for (int i = 0; i < argc; i++) {
		std::cout << "arg: " << argv[i] << "\n";
	}

	if (argc < 2) {
		LOG(ERROR) << "Must provide args: 'server' or 'client'";
	}
	else {
		if (!strcmp(argv[1], "server")) {
			AlchymeGame::RunServer();
		}
		else if (!strcmp(argv[1], "client")) {
			AlchymeGame::RunClient();
		}
		else
			LOG(ERROR) << "Must be either 'server' or 'client'";
	}	

	return 0;
}