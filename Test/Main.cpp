
// C++ program to demonstrate working of
// Variadic function Template
#include <iostream>
#include "Packet.h"

// To handle base case of below recursive
// Variadic function Template
void Invoke(Packet &p) {
    std::cout << "I am empty function and "
        "I am called at last.\n";

    // this will be called last, and so will be done
    // so, send packet
}

// Append var1 to 
template <typename T, typename... Types>
void Invoke(Packet &p, T var1, Types... var2) {
    p.Write(var1);

    Invoke(p, var2...);
}

template <typename... Types>
void Invoke(const char* name, Types... types) {
    Packet p;
    Invoke(p, types...);    

    p.offset = 0;
    std::cout << p.Read<int>() << "\n";
    std::cout << p.Read<float>() << "\n";

    std::cout << "Sending packet for remote invocation!\n";
}

// Driver code
int main() {
    //Packet p;
    Invoke("myFunction");// , (int)4, (float)3.14);

    //p.Write((int)69);
    //p.Write((float)4.68);

    return 0;
}