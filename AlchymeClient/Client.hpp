#pragma once
#include "IClient.h"
#include "World.h"

class Client : public IClient {

	std::thread m_thrPassword;

	const std::string m_version = "1.0.0";
	NetPeer m_peer;

	//EngineVk evk;

	void PasswordCallback(Rpc* rpc);

	void RPC_ClientHandshake(Rpc* rpc);
	void RPC_PeerInfo(Rpc* rpc,
		size_t peerUid,
		size_t worldSeed,
		size_t worldTime);
	void RPC_Print(Rpc* rpc, std::string s);
	void RPC_Error(Rpc* rpc, std::string s);

	/*
	* 
	* server implemtation
	* 
	*/

	void ConnectCallback(Rpc* rpc, ConnResult res) override;

	void DisconnectCallback(Rpc* rpc) override;

public:
	Client();
};
