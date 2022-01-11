#include "Rpc.h"
#include "Utils.h"

Rpc::Rpc(std::shared_ptr<AsioSocket> socket) 
	: m_socket(socket) {}

Rpc::~Rpc() {
	LOG(DEBUG) << "~Rpc()";
	UnregisterAll();
}

void Rpc::Register(const char* name, IMethod *method) {
	size_t hash = StrHash(name);

	#ifndef _NDEBUG
		if (m_methods.find(hash) != m_methods.end())
			throw std::runtime_error("Hash collision, this is extremely rare");
	#endif

	m_methods.insert({ hash, method });
}

void Rpc::UnregisterAll() {
	for (auto it = m_methods.begin(); it != m_methods.end(); it++) {
		delete it->second;
	}
	m_methods.clear();
}

void Rpc::Update() {
	int max = 10;
	while (m_socket->GotNewData() && max-- >= 0) {
		Packet packet = m_socket->Recv();

		// First size_t is the function name hash
		size_t hash; packet.Read(hash);
		// find method in stored
		auto&& find = m_methods.find(hash);
		if (find != m_methods.end()) {
			find->second->Invoke(this, packet);
			
			// this wont work, offset supposedly contains the method hash
			//if (packet.offset != packet.m_buf.size())
				//throw std::runtime_error("Remote failed to use correct number of args");
				
		}
		else {
			throw std::runtime_error("Remote tried invoking unknown function");
		}

	}
}

// Can register methods before socket is initialized
// but invocation