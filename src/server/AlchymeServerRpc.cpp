#include "AlchymeServer.hpp"

void AlchymeServer::RPC_ServerHandshake(Rpc* rpc, int magic, ConnectMode mode, std::string version) {
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
			
			//rpc->Invoke("ClientHandshake");
			if (mode == ConnectMode::STATUS) {
				// then send back the info of this server

				// RPC_ModeStatus(serverName, serverBirthDate, serverUpTime, serverStartTime, serverConnections, serverHead, serverDesc)
				rpc->Invoke("ModeStatus", m_serverName, VERSION, m_serverBirthDate, m_serverUpTime, m_serverStartTime, m_openConnections, m_serverHead, m_serverDesc);
			}
			else if (mode == ConnectMode::LOGIN) {
				if (!version.compare(VERSION)) {
					rpc->Invoke("Error", std::string("wrong version; need: ") + VERSION);

					// schedule a disconnect
					DisconnectLater(rpc);
				}
				else {
					rpc->Register("LoginInfo", new Method(this, &AlchymeServer::RPC_LoginInfo));
					// await info?
					rpc->Invoke("ModeLogin", VERSION);
				}

			}

		}
	}
}

void AlchymeServer::RPC_LoginInfo(Rpc* rpc,
	std::string name,
	std::string key) {

	LOG(INFO) << "Received LoginInfo";

	if (!isWhitelisted(key)) {
		rpc->Invoke("Error", std::string("not whitelisted"));

		// schedule a disconnect
		DisconnectLater(rpc);
		return;
	}

	auto peer = this->GetPeer(rpc);

	peer->m_uid = Utils::StrHash(name.c_str());
	peer->m_key = key;
	peer->name = name;

	rpc->Invoke("PeerInfo",
		peer->m_uid, Utils::StrHash("my world"), size_t(0));
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
