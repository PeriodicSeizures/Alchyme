#include "Rpc.h"

Rpc::Rpc(std::shared_ptr<AsioSocket> socket) 
	: m_socket(socket) {}

void Rpc::Register(const char* name, IMethod *method) {
	size_t hash = StrHash(name);
	std::cout << "hash: " << hash << "\n";
	m_methods.insert({ hash, method });
}

void Rpc::Update() {
	/// iterate packets and execute
	//std::cout << "Rpc::Update()\n";
	int max = 3;
	while (m_socket->GotNewData() && max-- >= 0) {
		Packet packet = m_socket->Recv();

		// First size_t is the function name hash
		size_t hash = packet.Read<size_t>();
		// find method in stored
		auto&& find = m_methods.find(hash);
		if (find != m_methods.end()) {
			find->second->Invoke(this, std::move(packet));
		}
		else {
			std::cout << "Remote tried invoking unknown function\n";
		}

	}
}

// Can register methods before socket is initialized
// but invocation