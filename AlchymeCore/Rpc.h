#pragma once
#include <unordered_map>
#include "Method.h"
#include "Socket.h"

size_t constexpr StrHash(char const* input) {
	return *input ? static_cast<size_t>(*input) + 33 * StrHash(input + 1) : 5381;
}

class Rpc {
	std::unordered_map<size_t, IMethod*> m_methods;
	std::shared_ptr<AsioSocket> m_socket;

public:
	Rpc(std::shared_ptr<AsioSocket> m_socket);

	//									must be a ptr	
	//								because inheritance
	void Register(const char* name, IMethod *method);

	

	// part of dummy template design
	void Invoke_impl(Packet& p) {}

	template <typename T, typename... Types>
	void Invoke_impl(Packet& p, T var1, Types... var2) {
		p.Write(var1);

		Invoke_impl(p, var2...);
	}

	// Invoke("myFunction", a, b, c, ...);
	// This implementation allows for no RPC arguments
	// Need to make the method instantiator also
	// take 0 arguments when needed
	template <typename... Types>
	void Invoke(const char* name, Types... types) {
		Packet p;
		p.Write(StrHash(name)); // Write name hash

		// Goes down the line, writing the type
		Invoke_impl(p, types...); // 

		// Now send packet
		m_socket->Send(std::move(p));
	}

	void Update();
};

//class RpcServer {
//	std::unordered_map<size_t, Rpc> m_rpcClients;
//
//public:
//
//
//private:
//
//
//};