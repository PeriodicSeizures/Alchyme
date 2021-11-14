//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include <iostream>
#include "Client.hpp"

int main() {
    Client client;

    std::cout << "Enter the address: <host> <port>: ";

    std::string host, port;
    std::cin >> host >> port;

    // "10.224.118.20", "8001"
    client.Connect(host, port);

    try {
        client.StartListening();
    }
    catch (const std::exception& e) {
        std::cout << "error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}