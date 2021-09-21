#pragma once
#include <unordered_map>
#include "Method.h"
#include "Socket.h"

size_t constexpr StrHash(char const* input) {
	return *input ? static_cast<size_t>(*input) + 33 * StrHash(input + 1) : 5381;
}

class Rpc {
	std::unordered_map<size_t, IMethod*> m_methods;

public:
	std::shared_ptr<AsioSocket> m_socket;

public:
	Rpc(std::shared_ptr<AsioSocket> m_socket);

	//									must be a ptr	
	//								because inheritance
	void Register(const char* name, IMethod *method);

	// part of dummy template design
	void Append_impl(Packet& p) {}

	template <typename T, typename... Types>
	void Append_impl(Packet& p, T var1, Types... var2) {

		p.Write(var1);

		Append_impl(p, var2...);
	}

	/*
	 * Use like 
	 *	Invoke("myFunction", a, b, c, ...);
	 */
	// 
	template <typename... Types>
	void Invoke(const char* name, Types... types) {
		Packet p;
		auto hash = StrHash(name);
		p.Write(hash);

		// Goes down the line, writing the type
		Append_impl(p, types...); // 

		// Now send packet
		m_socket->Send(std::move(p));
	}

	void Update();
};
