#include "AlchymeServer.hpp"

//#define SETTING(key) (settings.emplace(key, def).first->second)
#define SETTING(key, def) (settings.emplace(key, def).first->second)

static bool loadSettings(std::unordered_map<std::string, std::string>& settings) {
	std::ifstream file;

	file.open("C:\\Users\\Rico\\Documents\\VisualStudio2019\\Projects\\Alchyme\\AlchymeServer\\data\\settings.txt");
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			size_t index = line.find(':');

			std::string key = line.substr(0, index);
			std::string value = line.substr(index + 2);
			settings.insert({ key, value });
		}

		file.close();
		return true;
	}
	else {
		return false;
	}
}

void AlchymeServer::Start() {
	// Load all stuff here
	loadSettings(settings);

	StartAccepting(std::stoi(SETTING("port", "8001")));
	m_useWhitelist = SETTING("use-whitelist", "true") == "true";

	LoadUsers();

	IServer::Run();
}

void AlchymeServer::Stop() {
	SaveUsers();

	AlchymeGame::Stop();
}

void AlchymeServer::RPC_ServerHandshake(Rpc* rpc) {
	LOG(INFO) << "ServerHandshake()!";

	rpc->Invoke("ClientHandshake");
}

void AlchymeServer::RPC_PeerInfo(Rpc* rpc,
	std::string version,
	std::string name,
	std::string key) {
	// assert version, name, hash
	if (version != m_version) {

		LOG(DEBUG) << "Client has wrong version";

		rpc->Invoke("Error", std::string("wrong version; need: ") + this->m_version);

		// schedule a disconnect
		DisconnectLater(rpc, 2ms * rpc->m_socket->GetPing() + 40ms);
		return;
	}

	if (m_whitelist.find(key) == m_whitelist.end()) {
		rpc->Invoke("Error", std::string("not whitelisted"));

		// schedule a disconnect
		DisconnectLater(rpc, 2ms * rpc->m_socket->GetPing() + 40ms);
		return;
	}

	auto peer = this->GetPeer(rpc);

	peer->m_uid = StrHash(name.c_str());
	peer->m_key = key;
	peer->name = name;

	rpc->Invoke("PeerInfo",
		peer->m_uid, StrHash("my world"), size_t(0));
}

void AlchymeServer::RPC_Print(Rpc* rpc, std::string s) {
	LOG(INFO) << "Remote print: " << s;
}

void AlchymeServer::PreUpdate(float dt) {
	for (auto&& it = m_rpcs.begin(); it != m_rpcs.end();) {
		if ((*it)->m_socket->WasDisconnected()) {
			DisconnectCallback(it->get());
			it = m_rpcs.erase(it);
		}
		else {
			(*it)->Update();
			++it;
		}
	}
}

void AlchymeServer::Update(float dt) {
	using namespace std::chrono_literals;
	auto now = std::chrono::steady_clock::now();

	auto delta = now - m_lastBanCheck;
	if (delta > 5s) {
		CheckUsers();
		m_lastBanCheck = std::chrono::steady_clock::now();
	}
}

void AlchymeServer::ConnectCallback(Rpc* rpc) {
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

void Server::CheckUsers() {
	for (auto&& peer : m_peers) {

		if (peer->m_rpc) {
			using namespace std::chrono_literals;

			// If peer is not whitelisted or
			// if peer is ip banned, disconnect
			if (m_whitelist.find(peer->m_key) == m_whitelist.end() || 
				m_bannedIps.find(peer->m_rpc->m_socket->GetHostName()) == m_bannedIps.end())
				DisconnectLater(peer->m_rpc, 2ms * peer->m_rpc->m_socket->GetPing() + 40ms);

		}
	}
}

void Server::SaveUsers() {
	std::ofstream myfile;
	myfile.open("banned_ips.txt", std::ios::out);
	if (myfile.is_open()) {
		for (auto&& s : m_bannedIps) {
			myfile << s << std::endl;
		}
		myfile.close();
	}

	myfile.open("whitelist.txt", std::ios::out);
	if (myfile.is_open()) {
		for (auto&& s : m_whitelist) {
			myfile << s << std::endl;
		}
		myfile.close();
	}
}

void Server::LoadUsers() {
	std::ifstream myfile;
	myfile.open("banned_ips.txt", std::ios::in);
	if (myfile.is_open()) {
		std::string line;
		while (std::getline(myfile, line)) {
			m_bannedIps.insert(line);
		}
		myfile.close();
	}

	myfile.open("whitelist.txt", std::ios::in);
	if (myfile.is_open()) {
		std::string line;
		while (std::getline(myfile, line)) {
			m_whitelist.insert(line);
		}
		myfile.close();
	}
}

/*
*
* end of ban section
*
*/

void Server::DisconnectLater(Rpc* rpc, std::chrono::milliseconds ms) {

	const auto now = std::chrono::steady_clock::now();

	RunTaskLater([this, rpc, ms]() {
		std::this_thread::sleep_for(ms);
		Disconnect(rpc);
	}, now + ms);

	// Would cause executing thread to sleep
	//asio::post([this, rpc, ms]() {
	//	std::this_thread::sleep_for(ms);
	//	Disconnect(rpc);
	//});
}
