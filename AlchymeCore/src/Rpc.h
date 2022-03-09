#ifndef RPC_H
#define RPC_H

#include <robin_hood.h>
#include "Method.h"
#include "Socket.h"

#include "Utils.h"

//#define RPC_INVOKE(func, ...) rpc->Invoke(func, ...)

class Rpc {
	robin_hood::unordered_map<size_t, IMethod*> m_methods;

public:
	std::shared_ptr<AsioSocket> m_socket;

	int not_garbage = 5;

public:
	Rpc(std::shared_ptr<AsioSocket> m_socket);
	~Rpc();

	//									must be a ptr	
	//								because inheritance
	void Register(const char* name, IMethod *method);
	void UnregisterAll();

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
	 * 
	 * Packet format:
	 *	8 bytes: hash,
	 *  
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

#endif
