#pragma once
#include "IServer.h"
#include "NetPeer.h"

class Server : public IServer {

	std::vector<std::unique_ptr<NetPeer>> m_peers;

	void Update(float dt) override {}

	void ConnectCallback(Rpc* rpc) override {
		rpc->Invoke("Print", std::string("hi!"));
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
			if (peer->m_rpc.get() == rpc)
				return peer.get();
		}
		return nullptr;
	}

public:
	Server() : IServer(8001) {}
};
