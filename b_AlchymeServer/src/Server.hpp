#pragma once
#include <unordered_set>
#include <fstream>
//#include <sqlite3.h>
#include <chrono>
#include <memory>
#include "Utils.h"
#include "IServer.h"
#include "NetPeer.h"

#include "World.h"

class Server : public IServer {
	std::vector<std::unique_ptr<NetPeer>> m_peers;

	std::unordered_map<std::string, std::string> settings;

	// Contains login keys which are banned
	std::unordered_set<std::string> m_whitelist;
	bool m_useWhitelist;
	std::unordered_set<std::string> m_bannedIps;

		
	std::unique_ptr<World> world;


	// use std string for runtime passing
	const char* m_version = "1.0.0";

	std::chrono::steady_clock::time_point m_lastBanCheck;

	void RPC_ServerHandshake(Rpc* rpc);

	void RPC_PeerInfo(Rpc* rpc,
		std::string version,
		std::string name,
		std::string key);

	void RPC_Print(Rpc* rpc, std::string s);

	
	void Update(float dt) override;
	void ConnectCallback(Rpc* rpc) override;
	void DisconnectCallback(Rpc* rpc) override;

	NetPeer* GetPeer(size_t uid);
	NetPeer* GetPeer(std::string name);
	NetPeer* GetPeer(Rpc* rpc);

	void CheckUsers();

	void SaveUsers();
	void LoadUsers();

	void DisconnectLater(Rpc* rpc, std::chrono::milliseconds ms);

public:
	Server() {}

	~Server() {}

	void Run() override;
	void Stop() override;
};
