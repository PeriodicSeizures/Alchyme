#pragma once
#include "IServer.h"
#include "NetPeer.h"

namespace {
	void RPC_ServerHandshake(Rpc* rpc) {

	}
}

struct Dummy {
	void dum(Rpc* rpc) {
		std::cout << "Dummy::dum()\n";
	}
};

class Server : public IServer {

	std::vector<std::unique_ptr<NetPeer>> m_peers;
	std::string password;

	void ClassMethod(Rpc* rpc) {
		std::cout << "ClassMethod()!\n";
	}

	void Update(float dt) override {}

	void ConnectCallback(Rpc* rpc) override {
		Dummy dummy;
		Method<Dummy> m(&dummy, &Dummy::dum);

		m_peers.push_back(std::make_unique<NetPeer>(rpc));
		//Method<Server> m(nullptr, &Server::ClassMethod);
		//rpc->Register("ServerHandshake", new Method(this, &Server::ClassMethod));

		//rpc->Invoke("Print", std::string("hi!"));
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
