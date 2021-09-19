#include <iostream>
#include "NetPeer.h"

void testFunc(Rpc* rpc, int i, char c, long l) {
    std::cout << "rpc: " << rpc << "\n";
    std::cout << "calling testFunc " << i << " " << c << " " << l << "\n";
}

int main()
{
    NetPeer peer(std::make_shared<AsioSocket>());

    Method m(testFunc);

    Packet p{ std::vector<char>(50) };
    p.Write<int>(4345);
    p.Write<char>(3);
    p.Write<long>(279478489);
    p.offset = 0;

    m.Invoke(nullptr, p);
}