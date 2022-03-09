#include "AlchymeServer.hpp"
#include "World.h"

void AlchymeServer::RPC_ServerHandshake(Rpc* rpc) {
	LOG(DEBUG) << "ServerHandshake()!";

	rpc->Invoke("ClientHandshake");
}

void AlchymeServer::RPC_PeerInfo(Rpc* rpc,
	std::string version,
	std::string name,
	std::string key) {
	// assert version, name, hash
	if (version != m_version) {
		rpc->Invoke("Error", std::string("wrong version; need: ") + this->m_version);

		// schedule a disconnect
		DisconnectLater(rpc, 2ms * rpc->m_socket->GetPing() + 40ms);
		return;
	}

	if (!isWhitelisted(key)) {
		rpc->Invoke("Error", std::string("not whitelisted"));
		rpc->UnregisterAll();

		// schedule a disconnect
		DisconnectLater(rpc, 2ms * rpc->m_socket->GetPing() + 40ms);
		return;
	}

	auto peer = this->GetPeer(rpc);

	peer->m_uid = StrHash(name.c_str());
	peer->m_key = key;
	peer->name = name;

	rpc->Invoke("PeerInfo",
		peer->m_uid, StrHash("my world"), size_t(0));
}

void AlchymeServer::RPC_Print(Rpc* rpc, std::string s) {
	LOG(INFO) << "Remote print: " << s;
}

void AlchymeServer::RPC_AddIpBan(Rpc* rpc, std::string host) {
	addIpBan(host);
}

void AlchymeServer::RPC_RemoveIpBan(Rpc* rpc, std::string host) {
	removeIpBan(host);
}

void AlchymeServer::RPC_AddToWhitelist(Rpc* rpc, std::string key) {
	addToWhitelist(key);
}

void AlchymeServer::RPC_RemoveFromWhitelist(Rpc* rpc, std::string key) {
	removeFromWhitelist(key);
}
