#include "Rpc.h"

Rpc::Rpc(std::shared_ptr<AsioSocket> socket) 
	: m_socket(socket) {}

void Rpc::Register(const char* name, IMethod *method) {
	m_methods.insert({ StrHash(name), method });
}

void Rpc::Update() {
	/// iterate packets and execute
	while (m_socket->GotNewData()) {
		Packet packet = m_socket->Recv();

		// packet
		size_t hash = packet.Read<size_t>();


	}
}