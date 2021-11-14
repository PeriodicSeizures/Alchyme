#include "Client.hpp"

Client::Client() {
	World w("myworld");
	w.GenerateHeader("MyWorld", m_version);
	w.Save();

	m_peer.name = "crazicrafter1";
	//evk.run();
}

void Client::PasswordCallback(Rpc* rpc) {
	std::cout << "Login key: ";
	std::string key;
	std::cin >> key;

	// then send
	rpc->Invoke("PeerInfo", m_version, m_peer.name, key);
}

	/*
	*
	*	RPC DECLARATIONS
	*
	*/

void Client::RPC_ClientHandshake(Rpc* rpc) {
	LOG_INFO("ClientHandshake()!");

	m_thrPassword = std::thread(&Client::PasswordCallback, this, rpc);
}

void Client::RPC_PeerInfo(Rpc* rpc,
	size_t peerUid,
	size_t worldSeed,
	size_t worldTime) {

	LOG_INFO("my uid: " << peerUid <<
		", worldSeed: " << worldSeed <<
		", worldTime: " << worldTime);


}

void Client::RPC_Print(Rpc* rpc, std::string s) {
	std::cout << "Remote print: " << s << "\n";
}

void Client::RPC_Error(Rpc* rpc, std::string s) {
	std::cout << "Remote error: " << s << "\n";
}

/*
*
* server implemtation
*
*/

void Client::ConnectCallback(Rpc* rpc, ConnResult res) {
	if (res == ConnResult::OK) {
		rpc->Register("ClientHandshake", new Method(this, &Client::RPC_ClientHandshake));
		rpc->Register("Print", new Method(this, &Client::RPC_Print));
		rpc->Register("PeerInfo", new Method(this, &Client::RPC_PeerInfo));
		rpc->Register("Error", new Method(this, &Client::RPC_Error));

		rpc->Invoke("ServerHandshake");
	}
	else {
		std::cout << "Failed to connect\n";
	}
}

void Client::DisconnectCallback(Rpc* rpc) {
	//std::cout << ""
}