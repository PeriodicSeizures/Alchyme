#include "AlchymeClient.h"
#include <easylogging++.h>
#include <robin_hood.h>
#include <memory>
#include "Utils.h"
#include "Packet.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
    initLogger();

    char c = 0;

    //Packet packet{0, std::vector<char>(10)};
    //packet.Write((char)'E');
    //packet.offset = 0;
    //packet.Read(c); // should not compile?

    AlchymeClient::Run();
    //AlchymeClient c;
    //c.Start();
    
	return 0;
}