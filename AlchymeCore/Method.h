#pragma once
#include <tuple>
#include <functional>
#include "Packet.h"

class Rpc;

// Thanks @Fux
template <class Tuple, class F, size_t... Is>
constexpr auto invoke_tuple_impl(F f, Rpc* rpc, Tuple t,
    std::index_sequence<Is...>) {
    return std::invoke(f, rpc, std::get<Is>(t)...);
}

template <class Tuple, class F>
constexpr auto invoke_tuple(F f, Rpc *rpc, Tuple t) {
    return invoke_tuple_impl(f, rpc, t, std::make_index_sequence < std::tuple_size<Tuple>{} > {});
}

class IMethod
{
public:
	virtual void Invoke(Rpc *rpc, Packet packet) = 0;
};

template<class First, class...Rest>
class Method final : IMethod {
    using Lambda = void(*)(Rpc*, First, Rest...);
    Lambda lambda;

    template<class F>
    auto Invoke_impl(Packet p, F) {
        std::tuple<F> a{ p.Read<F>() };
        return a;
    }

    // Split a param,
    // Add that param from Packet into tuple
    template<class F, class...R>
    auto Invoke_impl(Packet p, F, R...) {
        std::tuple<F> a{ p.Read<F>() };
        std::tuple<R...> b = Invoke_impl<R...>(p, R{}...);
        return std::tuple_cat(a, b);
    }

public:
    Method(Lambda lam) : lambda(lam) {}

    void Invoke(Rpc *rpc, Packet p) override {
        // Invoke_impl returns a tuple of types by recursion
        auto tupl = Invoke_impl<First, Rest...>(p, First{}, Rest{}...);
        invoke_tuple(lambda, rpc, tupl);
    }
};
