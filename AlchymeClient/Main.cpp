#include <iostream>
#include "Client.hpp"

int main()
{
    Client client;

    std::cout << "Enter the address: <host> <port>: ";

    std::string host, port;
    std::cin >> host >> port;

    // "10.224.118.20", "8001"
    client.Connect(host, port);

    client.Start();
}