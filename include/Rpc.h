#ifndef RPC_H
#define RPC_H

#include <robin_hood.h>
#include "Method.h"
#include "Socket.h"

#include "Utils.h"

class Rpc {
	robin_hood::unordered_map<uint16_t, std::unique_ptr<IMethod>> m_methods;

public:
	std::shared_ptr<AsioSocket> m_socket;

	int not_garbage;

public:
	Rpc(AsioSocket::Ptr socket);
	~Rpc();

	//									must be a ptr	
	//								because inheritance
	void Register(const char* name, IMethod *method);
	//void UnregisterAll();

	// part of dummy template design
	void Append_impl(Packet *p) {}

	template <typename T, typename... Types>
	void Append_impl(Packet *p, T var1, Types... var2) {
		p->Write(var1);

		Append_impl(p, var2...);
	}

	/*
	 * Usage
	 *	Invoke("myFunction", params...);
	 * 
	 * Header: 
	 *	- 1 byte: Type
	 *	- 2 bytes: Hash id
	 *	- [0, UINT16_MAX-3]: bytes: Params
	 */
	template <typename... Types>
	void Invoke(const char* name, Types... types) {
		Packet* p = new Packet();
		p->Write((uint8_t)0);
		p->Write(static_cast<uint16_t>(StrHash(name)));

		// Goes down the line, writing the type
		Append_impl(p, types...); // 

		// Now send packet
		m_socket->Send(p);
	}

	//void Update();

	void Process(Packet *packet);
};

#endif
