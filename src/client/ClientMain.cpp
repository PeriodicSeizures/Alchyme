// AlchymeClient
//

#include "AlchymeGame.h"
#include "Utils.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
    initLogger();

    AlchymeGame::RunClient();

	return 0;
}