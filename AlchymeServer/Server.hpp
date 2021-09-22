#pragma once
#include "IServer.h"
#include "NetPeer.h"

class Server : public IServer {

	std::vector<std::unique_ptr<NetPeer>> m_peers;
	std::string password = "esggty";

	// either use const cstring for compile time 
	
	// use std string for runtime passing
	const std::string m_version = "1.0.0";

	void RPC_ServerHandshake(Rpc* rpc) {
		std::cout << "ServerHandshake()!\n";

		rpc->Invoke("ClientHandshake", !password.empty());
	}

	void RPC_PeerInfo(Rpc* rpc,
		std::string version,
		std::string name,
		size_t passwordHash) {
		// assert version, name, hash
		if (version != m_version) {
			std::cout << "wrong version\n";

			rpc->Invoke("Error", std::string("incompatible versions"));
			
			// schedule a disconnect
			//Disconnect(rpc, true);
			return;
		}

		if (passwordHash != StrHash(password.c_str())) {
			std::cout << "wrong password\n";
			rpc->Invoke("Error", std::string("wrong password"));

			// schedule a disconnect, maybe 1 second from now. ..
			//Disconnect(rpc, true);
			return;
		}

		std::cout << "player: " << name << "\n";

		// Now unpack info to authed client
		rpc->Invoke("PeerInfo", 
			StrHash(name.c_str()), std::string("test_world"), (size_t)195672334432334723, (size_t)0);
	}

	void RPC_Print(Rpc* rpc, std::string s) {
		std::cout << "Remote print: " << s << "\n";
	}

	void Update(float dt) override {}

	void ConnectCallback(Rpc* rpc) override {
		m_peers.push_back(std::make_unique<NetPeer>(rpc));
		rpc->Register("ServerHandshake", new Method(this, &Server::RPC_ServerHandshake));
		rpc->Register("Print", new Method(this, &Server::RPC_Print));
		rpc->Register("PeerInfo", new Method(this, &Server::RPC_PeerInfo));
	}

	void DisconnectCallback(Rpc* rpc) override {}

	NetPeer* GetPeer(size_t uid) {
		for (auto&& peer : m_peers) {
			if (peer->m_uid == uid)
				return peer.get();
		}
		return nullptr;
	}

	NetPeer* GetPeer(std::string name) {
		for (auto&& peer : m_peers) {
			if (peer->name == name)
				return peer.get();
		}
		return nullptr;
	}

	NetPeer* GetPeer(Rpc* rpc) {
		for (auto&& peer : m_peers) {
			if (peer->m_rpc == rpc)
				return peer.get();
		}
		return nullptr;
	}

public:
	Server() : IServer(8001) {}
};
