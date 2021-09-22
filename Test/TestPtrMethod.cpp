#include <tuple>
#include <functional>

#include <cstring>
#include <array>
struct Packet {
    std::array<char, 50> m_buf;
    std::size_t offset = 0;

    template<typename T>
    void Read(T& out) {
        std::memcpy(&out, m_buf.data() + offset, sizeof(T));
        offset += sizeof(T);
    }
};

class Rpc;

// Thanks @Fux
template <class C, class Tuple, class F, size_t... Is>
constexpr auto invoke_tuple_impl(F f, C& c, Rpc* rpc, Tuple t,
    std::index_sequence<Is...>) {
    return std::invoke(f, c, rpc, std::get<Is>(t)...);
}

template <class C, class Tuple, class F>
constexpr auto invoke_tuple(F f, C& c, Rpc* rpc, Tuple t) {
    return invoke_tuple_impl(f, c, rpc, t, std::make_index_sequence < std::tuple_size<Tuple>{} > {});
}

template<class C, class...Args>
class Method {
    using Lambda = void(C::*)(Rpc*, Args...);

    C* object;
    Lambda lambda;

    template<class F>
    auto Invoke_impl(Packet p) {
        F f; p.Read(f);
        std::tuple<F> a{ f };
        return a;
    }

    // Split a param,
    // Add that param from Packet into tuple
    template<class F, class S, class...R>
    auto Invoke_impl(Packet p) {
        F f; p.Read(f);
        std::tuple<F> a{ f };
        std::tuple<S, R...> b = Invoke_impl<S, R...>(p);
        return std::tuple_cat(a, b);
    }

public:
    Method(C* object, Lambda lam) : object(object), lambda(lam) {}

    void Invoke(Rpc* rpc, Packet p) {
        // Invoke_impl returns a tuple of types by recursion
        if constexpr (sizeof...(Args))
        {
            auto tupl = Invoke_impl<Args...>(p);
            invoke_tuple(lambda, object, rpc, tupl);
        }
        else
        {
            object->lambda(rpc);
        }
    }
};

#include <iostream>

void testFunc(Rpc* rpc, int i, char c, long l) {
    std::cout << "calling testFunc " << i << " " << c << " " << l << "\n";
}

void testFunc2(Rpc* rpc)
{
    std::cout << "calling testFunc2()\n";
}

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
}