#pragma once
#include <fstream>
#include <chrono>
#include <memory>

#include "World.h"
#include "AlchymeGame.h"
#include "NetPeer.h"
#include <robin_hood.h>

enum class Result {
	SQL_ERROR = 0,
	BANNED,
	INVALID_KEY,
	ALLOWED
};

class AlchymeServer : public AlchymeGame {

	//std::thread m_consoleThread;

	std::future<std::string> consoleFuture;

	//std::vector<std::unique_ptr<Rpc>> m_rpcs;
	std::vector<std::unique_ptr<NetPeer>> m_peers;

	robin_hood::unordered_map<std::string, std::string> settings;

	// Contains login keys which are banned
	robin_hood::unordered_set<std::string> m_whitelist;
	robin_hood::unordered_set<std::string> m_bannedIps;
	bool m_useWhitelist;
	std::chrono::steady_clock::time_point m_lastBanCheck;

	std::unique_ptr<tcp::acceptor> m_acceptor;

	// use std string for runtime passing
	const char* m_version = "1.0.0";

public:
	static AlchymeServer* Get();
	static void Run();

	AlchymeServer();

	void Start() override;
	void Stop() override;

	void addIpBan(const std::string& host);
	void addToWhitelist(const std::string& key);

	void removeIpBan(const std::string& host);
	void removeFromWhitelist(const std::string& key);

	bool isIpBanned(const std::string &host);
	bool isWhitelisted(const std::string &key);

private:
	void PreUpdate(float delta) override;
	void Update(float delta) override;
	void ConnectCallback(Rpc* rpc) override;
	void DisconnectCallback(Rpc* rpc) override;

	void DoAccept();
	//void CheckUsers();

	void RPC_ServerHandshake(Rpc* rpc);
	void RPC_PeerInfo(Rpc* rpc, std::string version, std::string name, std::string key);
	void RPC_Print(Rpc* rpc, std::string s);
	void RPC_AddIpBan(Rpc* rpc, std::string host);
	void RPC_RemoveIpBan(Rpc* rpc, std::string host);
	void RPC_AddToWhitelist(Rpc* rpc, std::string key);
	void RPC_RemoveFromWhitelist(Rpc* rpc, std::string key);

	NetPeer* GetPeer(size_t uid);
	NetPeer* GetPeer(std::string name);
	NetPeer* GetPeer(Rpc* rpc);

	void DisconnectLater(Rpc* rpc, std::chrono::milliseconds ms);

	void SaveUsers();
	void LoadUsers();

	void Disconnect(Rpc* rpc);
};
