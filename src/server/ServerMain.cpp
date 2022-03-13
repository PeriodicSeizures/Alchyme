// AlchymeServer
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include "AlchymeServer.hpp"
#include "Utils.h"
//#include "FastBuffer.hpp"

INITIALIZE_EASYLOGGINGPP

int main() {
    //std::this_thread::

    initLogger();

	AlchymeServer s;

    try {
        s.Start();
    }
    catch (std::exception& e) {
        LOG(ERROR) << e.what();
    }

	return 0;
}
