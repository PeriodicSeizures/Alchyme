#include "Client.hpp"
#include "Script.hpp"

void Client::RPC_ClientHandshake(Rpc* rpc) {
	LOG(DEBUG) << "ClientHandshake()!";

	serverAwaitingPeerInfo = true;

	ScriptManager::Event::OnHandshake();
}

void Client::RPC_PeerInfo(Rpc* rpc,
	size_t peerUid,
	size_t worldSeed,
	size_t worldTime) {

	LOG(DEBUG) << "my uid: " << peerUid <<
		", worldSeed: " << worldSeed <<
		", worldTime: " << worldTime;
}

void Client::RPC_Print(Rpc* rpc, std::string s) {
	LOG(INFO) << "Remote print: " << s << "\n";
}

void Client::RPC_Error(Rpc* rpc, std::string s) {
	LOG(ERROR) << "Remote error: " << s << "\n";
}