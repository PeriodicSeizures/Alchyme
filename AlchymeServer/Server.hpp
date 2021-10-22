#pragma once
#include <unordered_set>
#include <fstream>
#include <sqlite3.h>
#include <chrono>
#include "IServer.h"
#include "NetPeer.h"
#include "World.h"

class Server : public IServer {

	std::vector<std::unique_ptr<NetPeer>> m_peers;
	//std::string password = "password1";
	sqlite3* DB;

	std::unordered_map<std::string, std::string> settings;

	// Contains login keys which are banned
	std::unordered_set<std::string> m_banned;
		
	// either use const cstring for compile time 
	
	//std::vector<World> worlds;

	std::unique_ptr<World> world;


	// use std string for runtime passing
	const std::string m_version = "1.0.0";

	void RPC_ServerHandshake(Rpc* rpc) {
		std::cout << "ServerHandshake()!\n";

		rpc->Invoke("ClientHandshake");
	}

	void RPC_PeerInfo(Rpc* rpc,
		std::string version,
		std::string name,
		std::string key) {
		// assert version, name, hash
		if (version != m_version) {

			std::cout << "wrong version\n";

			rpc->Invoke("Error", std::string("wrong version; need: ") + this->m_version);

			// schedule a disconnect
			DisconnectLater(rpc, std::chrono::milliseconds(100));
			return;
		}

		if (InternalIsBanned(key)) {
			rpc->Invoke("Error", std::string("blacklisted"));

			// schedule a disconnect
			DisconnectLater(rpc, std::chrono::milliseconds(100));
			return;
		}

		auto peer = this->GetPeer(rpc);

		peer->m_uid = StrHash(name.c_str());
		peer->m_key = key;
		peer->name = name;
		peer->authorized = true;

		rpc->Invoke("PeerInfo",
			peer->m_uid, StrHash("my world"), size_t(0));
	}	

	void RPC_Print(Rpc* rpc, std::string s) {
		std::cout << "Remote print: " << s << "\n";
	}

	std::chrono::steady_clock::time_point m_lastBanCheck;
	void Update(float dt) override {
		using namespace std::chrono_literals;
		auto now = std::chrono::steady_clock::now();

		auto delta = now - m_lastBanCheck;
		if (delta > 5s) {
			CheckBans();
			m_lastBanCheck = std::chrono::steady_clock::now();
		}
	
		//std::cout << getTimeSinceStart() << "\n";
	}

	void ConnectCallback(Rpc* rpc) override {
		m_peers.push_back(std::make_unique<NetPeer>(rpc));
		rpc->Register("ServerHandshake", new Method(this, &Server::RPC_ServerHandshake));
		rpc->Register("Print", new Method(this, &Server::RPC_Print));
		rpc->Register("PeerInfo", new Method(this, &Server::RPC_PeerInfo));
	
		std::cout << rpc->m_socket->GetHostName() << " has connected\n";
	}

	void DisconnectCallback(Rpc* rpc) override {
		GetPeer(rpc)->m_rpc = nullptr; // Invalidate it because the object has been freed
	}

	NetPeer* GetPeer(size_t uid) {
		for (auto&& peer : m_peers) {
			if (peer->m_uid == uid)
				return peer.get();
		}
		return nullptr;
	}

	NetPeer* GetPeer(std::string name) {
		for (auto&& peer : m_peers) {
			if (peer->name == name)
				return peer.get();
		}
		return nullptr;
	}

	NetPeer* GetPeer(Rpc* rpc) {
		for (auto&& peer : m_peers) {
			if (peer->m_rpc == rpc)
				return peer.get();
		}
		return nullptr;
	}

	/*
	*
	*			Ban section
	*
	*/

	void GlobalBan(const std::string& key) {
		m_banned.insert(key);
		std::string sql = "UPDATE users SET banned = 1 WHERE key == '" + key + "';COMMIT;";
		sqlite3_exec(DB, sql.c_str(), NULL, NULL, NULL);
	}

	void GlobalUnBan(const std::string& key) {
		m_banned.erase(key);
		std::string sql = "UPDATE users SET banned = 0 WHERE key == '" + key + "';COMMIT;";
		sqlite3_exec(DB, sql.c_str(), NULL, NULL, NULL);
	}

	bool InternalIsBanned(const std::string& key) {
		return m_banned.contains(key);
	}

	enum class Result {
		SQL_ERROR = 0,
		BANNED,
		INVALID_KEY,
		ALLOWED
	};

	Result SQLCheckBan(const std::string& key) {

		std::string sql = "SELECT banned FROM users WHERE key == '" + key + "';";

		sqlite3_stmt* stmt = NULL;
		int rc = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
		if (rc != SQLITE_OK) {
			//std::cerr << "sql error\n";
			// schedule a disconnect
			return Result::SQL_ERROR;
		}
		else {
			rc = sqlite3_step(stmt);
			int row = 0;
			Result result = Result::ALLOWED;
			while (rc != SQLITE_DONE && rc != SQLITE_OK) {
				row++;
				char* banned = (char*)sqlite3_column_text(stmt, 0);

				std::cout << "Banned: " << banned << "\n";

				if (*banned == '1') {
					result = Result::BANNED;
				}
				break;
			}
			if (row == 0) {
				result = Result::INVALID_KEY;
			}

			sqlite3_finalize(stmt);
			return result;
		}
	}

	void CheckBans() {
		for (auto&& peer : m_peers) {

			if (peer->m_rpc && peer->authorized) {
				//std::string sql = "SELECT"
				switch (SQLCheckBan(peer->m_key)) {
				case Result::SQL_ERROR:
					std::cerr << "sql error while checking bans\n";
					break;
				case Result::BANNED:
					peer->m_rpc->Invoke("Error", std::string("blacklisted"));
					m_banned.insert(peer->m_key);
					// schedule a disconnect
					DisconnectLater(peer->m_rpc, std::chrono::milliseconds(100));
					break;
				case Result::INVALID_KEY:
					std::cerr << "user has an invalid key\n";
					break;
				case Result::ALLOWED:
					m_banned.erase(peer->m_key);
					break;
				}
			}
		}
	}

	//void SaveBanList() {
	//	std::ofstream myfile;
	//	myfile.open("banned_ips.txt", std::ios::out);
	//	if (myfile.is_open()) {
	//		for (auto&& s : m_bannedIps) {
	//			myfile << s << "\n";
	//		}
	//		myfile.close();
	//	}
	//
	//	myfile.open("banned_names.txt", std::ios::out);
	//	if (myfile.is_open()) {
	//		for (auto&& s : m_bannedNames) {
	//			myfile << s << "\n";
	//		}
	//		myfile.close();
	//	}
	//}

	//void LoadBanList() {
	//	std::ifstream myfile;
	//	myfile.open("banned_ips.txt", std::ios::in);
	//	if (myfile.is_open()) {
	//		std::string line;
	//		while (std::getline(myfile, line)) {
	//			m_bannedIps.insert(line);
	//		}
	//		myfile.close();
	//	}
	//
	//	myfile.open("banned_names.txt", std::ios::in);
	//	if (myfile.is_open()) {
	//		std::string line;
	//		while (std::getline(myfile, line)) {
	//			m_bannedNames.insert(line);
	//		}
	//		myfile.close();
	//	}
	//}

	/*
	*
	* end of ban section
	*
	*/

	void DisconnectLater(Rpc* rpc, std::chrono::milliseconds ms) {
		asio::post([this, rpc, ms]() {
			std::this_thread::sleep_for(ms);
			Disconnect(rpc);
		});
	}

public:
	Server(std::unordered_map<std::string, std::string> settings) : IServer(
		std::stoi(settings["port"])), settings(settings) {



		//LoadBanList();
		if (sqlite3_open(settings["db-file"].c_str(), &DB))
			std::cerr << "error: " << sqlite3_errmsg(DB) << "\n";



		//world = std::make_unique<World>()
	}

	~Server() {
		//SaveBanList();
		
		//std::ofstream myfile;
		//myfile.open("at_exit_was_called.txt", std::ios::out);
		//myfile.close();
	}
};
