#pragma once
#include <unordered_set>
#include <fstream>
//#include <sqlite3.h>
#include <chrono>
#include <memory>
#include "IServer.h"
#include "NetPeer.h"

class World;

enum class Result {
	SQL_ERROR = 0,
	BANNED,
	INVALID_KEY,
	ALLOWED
};

class Server : public IServer {

	std::vector<std::unique_ptr<NetPeer>> m_peers;
	//sqlite3* DB;

	std::unordered_map<std::string, std::string> settings;

	// Contains login keys which are banned
	std::unordered_set<std::string> m_banned;
		
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

	/*
	*
	*			Ban section
	*
	*/

	void GlobalBan(const std::string& key);

	void GlobalUnBan(const std::string& key);

	bool InternalIsBanned(const std::string& key);

	//Result SQLCheckBan(const std::string& key);

	void CheckBans();
	void DisconnectLater(Rpc* rpc, std::chrono::milliseconds ms);

public:
	Server(std::unordered_map<std::string, std::string> settings);

	~Server();
};
