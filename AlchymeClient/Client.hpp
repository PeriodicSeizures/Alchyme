#pragma once
#include "IClient.h"
#include "World.h"
//#include "EngineVk.h"

class Client : public IClient {

	std::thread m_thrPassword;

	const std::string m_version = "1.0.0";
	NetPeer m_peer;

	//EngineVk evk;

	void PasswordCallback(Rpc* rpc) {
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

	void RPC_ClientHandshake(Rpc* rpc) {
		LOG_INFO("ClientHandshake()!");
	
		m_thrPassword = std::thread(&Client::PasswordCallback, this, rpc);
	}

	void RPC_PeerInfo(Rpc* rpc,
		size_t peerUid,
		size_t worldSeed,
		size_t worldTime) {

		LOG_INFO("my uid: " << peerUid << 
						", worldSeed: " << worldSeed << 
						", worldTime: " << worldTime);


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

	void ConnectCallback(Rpc* rpc, ConnResult res) override {
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

	void DisconnectCallback(Rpc* rpc) override {
		//std::cout << ""
	}

public:
	Client() {
		World w("myworld");
		w.GenerateHeader("MyWorld", m_version);
		w.Save();

		m_peer.name = "crazicrafter1";
		//evk.run();
	}
};
