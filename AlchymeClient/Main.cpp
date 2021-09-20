#include <iostream>
#include "Client.h"

int main() {
    Client client;

    client.Connect("10.224.118.20", "8001");

    client.Start();


    //Packet p = Invoke("myFunction", (int)8, true);

    //std::cout << p.Read<int>() << " " << p.Read<bool>() << "\n";

    //Method m(testFunc);
    //
    //Packet p{ std::vector<char>(50) };
    //p.Write<int>(4345);
    //p.Write<char>(3);
    //p.Write<long>(279478489);
    //p.offset = 0;
    //
    //m.Invoke(nullptr, p);
}