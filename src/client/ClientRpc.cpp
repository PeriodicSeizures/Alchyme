#include "AlchymeClient.h"
#include "Script.hpp"

void AlchymeClient::RPC_ClientHandshake(Rpc* rpc) {
	LOG(INFO) << "ClientHandshake()!";

	serverAwaitingLogin = true;

	ScriptManager::Event::OnHandshake();
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