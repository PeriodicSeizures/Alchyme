#pragma once
#include "IClient.h"

namespace {
	//void RPC_Pos(Rpc* rpc, )
}

static std::string getPasswordCallback() {
	std::string answer;
	std::cin >> answer;
	return answer;
}

class Client : public IClient {

	std::thread m_thrPassword;

	const std::string m_version = "1.0.0";
	NetPeer m_peer;

	void PasswordCallback(Rpc* rpc) {
		std::cout << "Password: ";
		std::string password;
		std::cin >> password;

		// then send
		rpc->Invoke("PeerInfo", m_version, m_peer.name, StrHash(password.c_str()));
	}

	/*
	* 
	*	RPC DECLARATIONS
	* 
	*/

	void RPC_ClientHandshake(Rpc* rpc, bool needPassword) {
		std::cout << "ClientHandshake()!\n";
		std::cout << "Password needed for server: " << needPassword << "\n";
	
		if (needPassword) {			
			m_thrPassword = std::thread(&Client::PasswordCallback, this, rpc);
		}
		else {
			rpc->Invoke("PeerInfo", m_version, m_peer.name, "");
		}
	}

	void RPC_PeerInfo(Rpc* rpc,
		size_t peerUid,
		std::string worldName, 
		size_t worldSeed,
		size_t worldTime) {

		std::cout << "my uid: " << peerUid << ", worldname: " << worldName << "\n";
	}

	void RPC_Print(Rpc* rpc, std::string s) {
		std::cout << "Remote print: " << s << "\n";
	}

	void RPC_Error(Rpc* rpc, std::string s) {
		std::cout << "Remote error: " << s << "\n";
	}



	/*
	* 
	* server implemtation
	* 
	*/

	void Update(float dt) override {}

	void ConnectCallback(Rpc* rpc) override {
		rpc->Register("ClientHandshake", new Method(this, &Client::RPC_ClientHandshake));
		rpc->Register("Print", new Method(this, &Client::RPC_Print));
		rpc->Register("PeerInfo", new Method(this, &Client::RPC_PeerInfo));
		rpc->Register("Error", new Method(this, &Client::RPC_Error));

		rpc->Invoke("ServerHandshake");
	}

	void DisconnectCallback(Rpc* rpc) override {
		//std::cout << ""
	}

public:
	Client() {
		m_peer.name = "crazicrafter1";
	}
};
