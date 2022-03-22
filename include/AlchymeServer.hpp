#pragma once
#include <fstream>
#include <chrono>
#include <memory>

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
	std::future<std::string> consoleFuture;

	//std::vector<std::unique_ptr<Rpc>> m_rpcs;
	std::vector<std::unique_ptr<NetPeer>> m_peers;

	robin_hood::unordered_map<std::string, std::string> settings;

	// Contains login keys which are banned
	robin_hood::unordered_set<std::string> m_whitelist;

	// Contains addresses which are banned
	robin_hood::unordered_set<std::string> m_bannedIps;
	bool m_useWhitelist;

	std::unique_ptr<tcp::acceptor> m_acceptor;

	// custom
	std::string m_serverName;
	std::chrono::seconds m_serverBirthDate; // const
	std::chrono::seconds m_serverUpTime; //  (m_serverStartTime - now) + m_serverLastUpTime;
	std::chrono::seconds m_serverStartTime; // increment on restart
	//int m_serverConnections;
	std::string m_serverHead;
	std::string m_serverDesc;

	uint16_t m_openConnections;
	std::chrono::seconds m_serverLastUpTime;
	//std::chrono::seconds m_serverStartTime; // increments every restart

	// use std string for runtime passing
	//const char* m_version = "1.0.0";

public:
	static AlchymeServer* Get();

	AlchymeServer();

	void Start() override;
	void Stop() override;

	void addIpBan(std::string_view host);
	void addToWhitelist(std::string_view key);

	void removeIpBan(std::string_view host);
	void removeFromWhitelist(std::string_view key);

	bool isIpBanned(std::string_view host);
	bool isWhitelisted(std::string_view key);

private:
	void Update(float delta) override;
	void ConnectCallback(Rpc* rpc) override;
	void DisconnectCallback(Rpc* rpc) override;

	void DoAccept();

	void RPC_ServerHandshake(Rpc* rpc, int magic, ConnectMode mode, std::string version);	
	void RPC_LoginInfo(Rpc* rpc, std::string name, std::string key);
	void RPC_Print(Rpc* rpc, std::string s);
	void RPC_BlacklistIp(Rpc* rpc, std::string host);
	void RPC_BlacklistIpByName(Rpc* rpc, std::string name);
	void RPC_UnBlacklist(Rpc* rpc, std::string host);
	void RPC_Whitelist(Rpc* rpc, std::string key);
	void RPC_UnWhitelist(Rpc* rpc, std::string key);
	void RPC_ModeWhitelist(Rpc* rpc, bool useWhitelist);

	NetPeer* GetPeer(size_t uid);
	NetPeer* GetPeer(std::string name);
	NetPeer* GetPeer(Rpc* rpc);
	//uint16_t GetOpenPeers();

	void DisconnectLater(Rpc* rpc);

	void SaveUsers();
	void LoadUsers();

	void Disconnect(Rpc* rpc);
};
