#include "Net.h"

template<class F>
void Invoke_impl(Packet p, F) {
    p.Write(F);
}

template<class F, class...R>
void Invoke_impl(Packet p, F, R...) {
    // Append type of F to packet
    //std::tuple<F> a{ p.Read<F>() };
    p.Write(F);
    Invoke_impl<R...>(p, R{}...);
}

/**
* Invoke("myFunction", 8, true);
* Append types to Packet{}
*/
template<class...Rest>
Packet Invoke(const char* name, Rest...) {
	//auto tupl = 
    std::cout << name << " " << "\n";
    // recursively iterate every Parameter
    // Write to a Packet
    // call
    Packet p;
    Invoke_impl<Rest...>(p, Rest{}...);

    return p;
}

void testFunc(Rpc* rpc) {
    std::cout << "rpc: " << rpc << "\n";
    //std::cout << "calling testFunc " << i << " " << c << " " << l << "\n";
}

int main() {
    Packet p = Invoke("myFunction", (int)8, true);

    std::cout << p.Read<int>() << " " << p.Read<bool>() << "\n";

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