#include "Server.h"
//#include "R"

void testFunc(Rpc* rpc) {
    std::cout << "rpc: " << rpc << "\n";
    //std::cout << "calling testFunc " << i << " " << c << " " << l << "\n";
}

int main() {
    Server server(8001);

    server.Start();


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