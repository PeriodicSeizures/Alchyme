#include "Rpc.h"

struct Test {
    void func(Rpc*, int, char) {
        std::cout << "calling Test::func()\n";
    }
};

int main() {

    Test t;

    auto m = new Method(&t, &Test::func);
    //Method<Test, int, char> m(&t, &Test::func);


    Packet p;
    m->Invoke(nullptr, p);
    //m.Invoke(nullptr, p);
}