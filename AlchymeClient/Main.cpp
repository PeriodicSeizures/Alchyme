#include <iostream>
#include "Client.hpp"

struct Test {
    void func(Rpc*, int, char) {
        std::cout << "calling Test::func()\n";
    }
};

int main()
{
    Test t;
    Method<Test, int, char> m(&t, &Test::func);
    Packet p;
    m.Invoke(nullptr, p);




    Client client;

    std::cout << "Enter the address: <host> <port>: ";

    std::string host, port;
    std::cin >> host >> port;

    // "10.224.118.20", "8001"
    client.Connect(host, port);

    client.Start();
}