#include "Rpc.h"
#include "Utils.h"

Rpc::Rpc(std::shared_ptr<AsioSocket> socket) 
	: m_socket(socket) {
	this->not_garbage = 69;
}

Rpc::~Rpc() {
	LOG(DEBUG) << "~Rpc()";
}

void Rpc::Register(const char* name, IMethod *method) {
	uint16_t hash = static_cast<uint16_t>(StrHash(name));

	#ifndef _NDEBUG
		if (m_methods.find(hash) != m_methods.end())
			throw std::runtime_error("Hash collision, or most likely duplicate RPC name registered");
	#endif

	m_methods.insert({ hash, std::unique_ptr<IMethod>(method) });
}

/*
 * Usage
 *	Invoke("myFunction", params...);
 *
 * Header:
 *	- 1 byte: Type (already read)
 *	- 2 bytes: Hash id
 *	- [0, UINT16_MAX-3]: bytes: Params
 */
void Rpc::Process(Packet *packet) {
	uint16_t hash; packet->Read(hash); 
	// find method in stored
	auto&& find = m_methods.find(hash);
	if (find != m_methods.end()) {
		find->second->Invoke(this, packet);
	}
	else {
		LOG(ERROR) << "Remote tried invoking unknown function (corrupt or malicious)";
		//Invoke("Error", std::string("Unknown function"));
		m_socket->Close();
		//throw std::runtime_error("Remote tried invoking unknown function");
	}	
}
