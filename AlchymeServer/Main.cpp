#include "Server.hpp"


int main() {

    // Test
    //Rpc rpc(nullptr);
    //rpc.Invoke("Print", std::string("hi!"));

    Server server;

    server.Start();
}