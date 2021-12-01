#include "Rpc.h"
#include "Utils.h"

Rpc::Rpc(std::shared_ptr<AsioSocket> socket) 
	: m_socket(socket) {}

Rpc::~Rpc() {
	LOG(DEBUG) << "Rpc::~Rpc()\n";
}

void Rpc::Register(const char* name, IMethod *method) {
	size_t hash = StrHash(name);
	m_methods.insert({ hash, method });
}

void Rpc::UnregisterAll() {
	for (auto it = m_methods.begin(); it != m_methods.end(); it++) {
		delete it->second;
	}
	m_methods.clear();
}

void Rpc::Update() {
	/// iterate packets and execute
	//std::cout << "Rpc::Update()\n";
	int max = 3;
	while (m_socket->GotNewData() && max-- >= 0) {
		Packet packet = m_socket->Recv();

		// First size_t is the function name hash
		size_t hash; packet.Read(hash);
		// find method in stored
		auto&& find = m_methods.find(hash);
		if (find != m_methods.end()) {
			//try {
				find->second->Invoke(this, std::move(packet));
			//}
			//catch (std::exception& e) {
				//std::cerr << "Remote did not pass the right arguments to function\n";
			//	LOG_ERROR("error: " << e.what());
			//}
		}
		else {
			throw std::runtime_error("Remote tried invoking unknown function");
		}

	}
}

// Can register methods before socket is initialized
// but invocation