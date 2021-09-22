#pragma once
#include "IServer.h"
#include "NetPeer.h"

class Server : public IServer {

	std::vector<std::unique_ptr<NetPeer>> m_peers;
	std::string password = "imapassword";

	void RPC_ServerHandshake(Rpc* rpc) {
		std::cout << "ServerHandshake()!\n";

		rpc->Invoke("ClientHandshake", !password.empty());
	}

	void RPC_Print(Rpc* rpc, std::string s) {
		std::cout << "Remote print: " << s << "\n";
	}

	void Update(float dt) override {}

	void ConnectCallback(Rpc* rpc) override {
		m_peers.push_back(std::make_unique<NetPeer>(rpc));
		rpc->Register("ServerHandshake", new Method(this, &Server::RPC_ServerHandshake));
		rpc->Register("Print", new Method(this, &Server::RPC_Print));
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
