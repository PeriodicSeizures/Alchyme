// RPC recursive template example
#include <tuple>
#include <functional>

template <class Tuple, class F, size_t... Is>
constexpr auto invoke_tuple_impl(F f, Tuple t,
    std::index_sequence<Is...>) {
    return std::invoke(f, std::get<Is>(t)...);
}

template <class Tuple, class F>
constexpr auto invoke_tuple(F f, Tuple t) {
    return invoke_tuple_impl(f, t, std::make_index_sequence < std::tuple_size<Tuple>{} > {});
}

#include <cstring>
#include <array>
struct Packet {
    //std::array<char, 50> m_buf;
    std::vector<char> m_buf;
    std::size_t offset = 0;



    template<typename T>
    T Read() {
        T out{};
        std::memcpy(&out, m_buf.data() + offset, sizeof(T));
        offset += sizeof(T);
        return out;
    }

    template<typename T>
    void Write(T in) {
        // should not use this, a big enough vec
        // should be passed in during construction
        //if (m_buf.size() < offset + sizeof(T)) {
        //    m_buf.reserve(offset + sizeof(T) * 3);
        //}
        std::memcpy(m_buf.data() + offset, &in, sizeof(T));
        offset += sizeof(T);
    }
};

template<class First, class...Rest>
struct Method {
    using Lambda = void(*)(First, Rest...);
    Lambda lambda;

    Method(Lambda lam) : lambda(lam) {}

    template<class F>
    auto Invoke_impl(Packet p, F)
    {
        std::tuple<F> a{ p.Read<F>() };
        return a;
    }
    template<class F, class...R>
    auto Invoke_impl(Packet p, F, R...)
    {
        std::tuple<F> a{ p.Read<F>() };
        std::tuple<R...> b = Invoke_impl<R...>(p, R{}...);
        return std::tuple_cat(a, b);
    }

    void Invoke(Packet p) {
        auto tupl = Invoke_impl<First, Rest...>(p, First{}, Rest{}...);
        invoke_tuple(lambda, tupl);
    }
};

#include <iostream>

void testFunc(int i, char c, long l) {
    std::cout << "calling testFunc " << i << " " << c << " " << l << "\n";
}

int main()
{
    Method m(testFunc);

    Packet p{ std::vector<char>(50) };
    p.Write<int>(4345);
    p.Write<char>(3);
    p.Write<long>(279478489);
    p.offset = 0;

    m.Invoke(p);
}