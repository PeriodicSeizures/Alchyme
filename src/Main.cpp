#include "AlchymeClient.h"
#include "AlchymeServer.hpp"

#include <iostream>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv) {
	for (int i = 0; i < argc; i++) {
		std::cout << "arg: " << argv[i];
	}

	return 0;
}