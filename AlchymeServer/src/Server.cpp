#include "Server.hpp"
#include "World.h"

using namespace std::chrono_literals;

Server::Server(std::unordered_map<std::string, std::string> settings) : IServer(
	std::stoi(settings["port"])), settings(settings) {


	//world = std::make_unique<World>()
}

Server::~Server() {
	//SaveBanList();

	//std::ofstream myfile;
	//myfile.open("at_exit_was_called.txt", std::ios::out);
	//myfile.close();
}

void Server::RPC_ServerHandshake(Rpc* rpc) {
	std::cout << "ServerHandshake()!\n";

	rpc->Invoke("ClientHandshake");
}

void Server::RPC_PeerInfo(Rpc* rpc,
	std::string version,
	std::string name,
	std::string key) {
	// assert version, name, hash
	if (version != m_version) {

		std::cout << "wrong version\n";

		rpc->Invoke("Error", std::string("wrong version; need: ") + this->m_version);

		// schedule a disconnect
		DisconnectLater(rpc, 2ms * rpc->m_socket->GetPing() + 40ms);
		return;
	}

	if (InternalIsBanned(key)) {
		rpc->Invoke("Error", std::string("blacklisted"));
		rpc->UnregisterAll();

		// schedule a disconnect
		DisconnectLater(rpc, 2ms * rpc->m_socket->GetPing() + 40ms);
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

void Server::RPC_Print(Rpc* rpc, std::string s) {
	std::cout << "Remote print: " << s << "\n";
}

void Server::Update(float dt) {
	using namespace std::chrono_literals;
	auto now = std::chrono::steady_clock::now();

	auto delta = now - m_lastBanCheck;
	if (delta > 5s) {
		CheckBans();
		m_lastBanCheck = std::chrono::steady_clock::now();
	}

	//std::cout << getTimeSinceStart() << "\n";
}

void Server::ConnectCallback(Rpc* rpc) {
	m_peers.push_back(std::make_unique<NetPeer>(rpc));
	rpc->Register("ServerHandshake", new Method(this, &Server::RPC_ServerHandshake));
	rpc->Register("Print", new Method(this, &Server::RPC_Print));
	rpc->Register("PeerInfo", new Method(this, &Server::RPC_PeerInfo));

	LOG(INFO) << rpc->m_socket->GetHostName() << " has connected";
}

void Server::DisconnectCallback(Rpc* rpc) {
	LOG(INFO) << rpc->m_socket->GetHostName() << " has disconnected";
	GetPeer(rpc)->m_rpc = nullptr; // Invalidate it because the object has been freed
}

NetPeer* Server::GetPeer(size_t uid) {
	for (auto&& peer : m_peers) {
		if (peer->m_uid == uid)
			return peer.get();
	}
	return nullptr;
}

NetPeer* Server::GetPeer(std::string name) {
	for (auto&& peer : m_peers) {
		if (peer->name == name)
			return peer.get();
	}
	return nullptr;
}

NetPeer* Server::GetPeer(Rpc* rpc) {
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

void Server::GlobalBan(const std::string& key) {
	m_banned.insert(key);
	std::string sql = "UPDATE users SET banned = 1 WHERE key == '" + key + "';COMMIT;";
	//sqlite3_exec(DB, sql.c_str(), NULL, NULL, NULL);
}

void Server::GlobalUnBan(const std::string& key) {
	m_banned.erase(key);
	std::string sql = "UPDATE users SET banned = 0 WHERE key == '" + key + "';COMMIT;";
	//sqlite3_exec(DB, sql.c_str(), NULL, NULL, NULL);
}

bool Server::InternalIsBanned(const std::string& key) {
	return m_banned.contains(key);
}
/*
Result Server::SQLCheckBan(const std::string& key) {

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

			//std::cout << "Banned: " << banned << "\n";

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
*/

void Server::CheckBans() {
	for (auto&& peer : m_peers) {

		if (peer->m_rpc && peer->authorized) {
			using namespace std::chrono_literals;
			//std::string sql = "SELECT"
			/*
			switch (SQLCheckBan(peer->m_key)) {
			case Result::SQL_ERROR:
				std::cerr << "sql error while checking bans\n";
				break;
			case Result::BANNED:
				peer->m_rpc->Invoke("Error", std::string("blacklisted"));
				m_banned.insert(peer->m_key);
				// schedule a disconnect
				DisconnectLater(peer->m_rpc, 2ms * peer->m_rpc->m_socket->GetPing() + 40ms);
				break;
			case Result::INVALID_KEY:
				std::cerr << "user has an invalid key\n";
				DisconnectLater(peer->m_rpc, 2ms * peer->m_rpc->m_socket->GetPing() + 40ms);
				break;
			case Result::ALLOWED:
				m_banned.erase(peer->m_key);
				break;
			}
			*/
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

void Server::DisconnectLater(Rpc* rpc, std::chrono::milliseconds ms) {
	asio::post([this, rpc, ms]() {
		std::this_thread::sleep_for(ms);
		Disconnect(rpc);
	});
}
