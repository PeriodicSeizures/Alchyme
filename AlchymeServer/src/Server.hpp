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
	bool m_useWhitelist;

	// Contains login keys which are banned
	std::unordered_set<std::string> m_whitelist;
	std::unordered_set<std::string> m_bannedIps;
		
	std::unique_ptr<World> world;


	// use std string for runtime passing
	const char* m_version = "1.0.0";

	std::thread m_consoleIn;

public:
	Server();

	~Server();

	void Run() override;
	void Stop() override;

	void addIpBan(const std::string& host);
	void addToWhitelist(const std::string& key);

	void removeIpBan(const std::string& host);
	void removeFromWhitelist(const std::string& key);

	bool isIpBanned(const std::string &host);
	bool isWhitelisted(const std::string &key);

private:
	void RPC_ServerHandshake(Rpc* rpc);
	void RPC_PeerInfo(Rpc* rpc, std::string version, std::string name, std::string key);
	void RPC_Print(Rpc* rpc, std::string s);

	void Update(float dt) override;
	void ConnectCallback(Rpc* rpc) override;
	void DisconnectCallback(Rpc* rpc) override;

	NetPeer* GetPeer(size_t uid);
	NetPeer* GetPeer(std::string name);
	NetPeer* GetPeer(Rpc* rpc);

	void DisconnectLater(Rpc* rpc, std::chrono::milliseconds ms);

	void SaveUsers();
	void LoadUsers();
};
