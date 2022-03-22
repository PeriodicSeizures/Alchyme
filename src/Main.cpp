#include "AlchymeClient.h"
#include "AlchymeServer.hpp"

#include <iostream>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv) {

	// open file impl.txt

	Utils::initLogger();

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


/*
#include <iostream>
#include <thread>
#include <chrono>
#include <semaphore>


// global binary semaphore instances
// object counts are set to zero
// objects are in non-signaled state
std::binary_semaphore
smphSignalMainToThread{ 0 },
smphSignalThreadToMain{ 0 };

void ThreadProc()
{
	// wait for a signal from the main proc
	// by attempting to decrement the semaphore
	smphSignalMainToThread.acquire();

	// this call blocks until the semaphore's count
	// is increased from the main proc

	std::cout << "[thread] Got the signal\n"; // response message

	// wait for 3 seconds to imitate some work
	// being done by the thread
	using namespace std::literals;
	std::this_thread::sleep_for(3s);

	std::cout << "[thread] Send the signal\n"; // message

	// signal the main proc back
	smphSignalThreadToMain.release();
}

int main(int argc, char** argv)
{
	// create some worker thread
	std::thread thrWorker(ThreadProc);

	std::cout << "[main] Send the signal\n"; // message

	// signal the worker thread to start working
	// by increasing the semaphore's count
	smphSignalMainToThread.release();

	// wait until the worker thread is done doing the work
	// by attempting to decrement the semaphore's count
	smphSignalThreadToMain.acquire();

	std::cout << "[main] Got the signal\n"; // response message
	thrWorker.join();

	return 0;
}
*/