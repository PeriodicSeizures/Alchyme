#include "AlchymeClient.h"
#include "Script.hpp"

void AlchymeClient::RPC_ClientHandshake(Rpc* rpc, int magic) {
	LOG(INFO) << "ClientHandshake()!";

	if (magic != MAGIC) {
		Disconnect();
	}
	else {
		rpc->Register("Print", new Method(this, &AlchymeClient::RPC_Print));
		rpc->Register("PeerInfo", new Method(this, &AlchymeClient::RPC_PeerInfo));
		rpc->Register("Error", new Method(this, &AlchymeClient::RPC_Error));
		
		if (m_mode == ConnectMode::STATUS)
			rpc->Register("ModeStatus", new Method(this, &AlchymeClient::RPC_ModeStatus));
		else if (m_mode == ConnectMode::LOGIN)
			rpc->Register("ModeLogin", new Method(this, &AlchymeClient::RPC_ModeLogin));

		//										modes
		//										status, login, 
		rpc->Invoke("ServerHandshake", MAGIC, m_mode, VERSION);
		//serverAwaitingLogin = true;

		//ScriptManager::Event::OnHandshake();
	}
}

void AlchymeClient::RPC_ModeStatus(Rpc* rpc, std::string serverName, std::string serverVersion, long long serverBirthDate, long long serverUpTime, long long serverStartTime, uint16_t serverConnections, std::string serverHead, std::string serverDesc) {
	LOG(INFO) << "Server Status: " << serverName << ", " << serverVersion << ", " << serverBirthDate << ", " << serverUpTime << ", " << serverStartTime << ", " << serverConnections << ", " << serverHead << ", " << serverDesc;
}

void AlchymeClient::RPC_ModeLogin(Rpc* rpc, std::string serverVersion) {
	LOG(INFO) << "Initiating Server Login...";
	serverAwaitingLogin = true;

	ScriptManager::Event::OnLogin();
}

void AlchymeClient::RPC_PeerInfo(Rpc* rpc,
	size_t peerUid,
	size_t worldSeed,
	size_t worldTime) {

	LOG(DEBUG) << "my uid: " << peerUid <<
		", worldSeed: " << worldSeed <<
		", worldTime: " << worldTime;
}

void AlchymeClient::RPC_Print(Rpc* rpc, std::string s) {
	LOG(INFO) << "Remote print: " << s << "\n";
}

void AlchymeClient::RPC_Error(Rpc* rpc, std::string s) {
	LOG(ERROR) << "Remote error: " << s << "\n";
}