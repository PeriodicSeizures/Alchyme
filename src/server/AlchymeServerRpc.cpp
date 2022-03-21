#include "AlchymeServer.hpp"

void AlchymeServer::RPC_ServerHandshake(Rpc* rpc, int magic) {
	LOG(INFO) << "ServerHandshake()!";

	if (magic != MAGIC)
		Disconnect(rpc);
	else {
		if (isIpBanned(rpc->m_socket->GetHostName())) {
			rpc->Invoke("Error", std::string("ip banned"));
			DisconnectLater(rpc);
		}
		else {
			rpc->Register("Print", new Method(this, &AlchymeServer::RPC_Print));
			rpc->Register("PeerInfo", new Method(this, &AlchymeServer::RPC_PeerInfo));
			rpc->Invoke("ClientHandshake");
		}
	}
}

void AlchymeServer::RPC_PeerInfo(Rpc* rpc,
	std::string version,
	std::string name,
	std::string key) {
	// assert version, name, hash
	if (version != m_version) {
		rpc->Invoke("Error", std::string("wrong version; need: ") + this->m_version);

		// schedule a disconnect
		DisconnectLater(rpc);
		return;
	}

	if (!isWhitelisted(key)) {
		rpc->Invoke("Error", std::string("not whitelisted"));

		// schedule a disconnect
		DisconnectLater(rpc);
		return;
	}

	auto peer = this->GetPeer(rpc);

	//peer->m_uid = StrHash(name.c_str());
	//peer->m_key = key;
	//peer->name = name;

	//rpc->Invoke("PeerInfo",
	//	peer->m_uid, StrHash("my world"), size_t(0));
}

void AlchymeServer::RPC_Print(Rpc* rpc, std::string s) {
	LOG(INFO) << "Remote print: " << s;
}

void AlchymeServer::RPC_BlacklistIp(Rpc* rpc, std::string host) {
	addIpBan(host);
}

void AlchymeServer::RPC_BlacklistIpByName(Rpc* rpc, std::string name) {
	NetPeer* peer = GetPeer(name);
	if (peer && peer->m_rpc->m_socket)
		addIpBan(peer->m_rpc->m_socket->GetHostName());
	else 
		rpc->Invoke("Error", std::string("Player not found"));
}

void AlchymeServer::RPC_UnBlacklist(Rpc* rpc, std::string host) {
	removeIpBan(host);
}

void AlchymeServer::RPC_Whitelist(Rpc* rpc, std::string key) {
	addToWhitelist(key);
}

void AlchymeServer::RPC_UnWhitelist(Rpc* rpc, std::string key) {
	removeFromWhitelist(key);
}

void AlchymeServer::RPC_ModeWhitelist(Rpc* rpc, bool useWhitelist) {
	m_useWhitelist = useWhitelist;
}
