#include "Server.hpp"
#include "World.h"

using namespace std::chrono_literals;

#define SETTING(key, def) (settings.emplace(key, def).first->second)

std::unique_ptr<Server> server;

Server* Server::GetServer() {
	return server.get();
}

void Server::RunServer() {
	server = std::make_unique<Server>();
	server->Run();
	server.reset();
}

static bool loadSettings(robin_hood::unordered_map<std::string, std::string>& settings) {
	std::ifstream file;

	file.open("C:\\Users\\Rico\\Documents\\VisualStudio2019\\Projects\\Alchyme\\AlchymeServer\\data\\settings.txt");
	//file.open("data\\settings.txt");
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

Server::Server() {}

Server::~Server() {}

void Server::Run() {
	// Load all stuff here

	if (!loadSettings(settings))
		throw std::runtime_error("could not load settings.txt");

	m_useWhitelist = SETTING("use-whitelist", "true") == "true";

	LoadUsers();

	StartAccepting(std::stoi(SETTING("port", "8001")));

	IServer::Run();
}

void Server::Stop() {
	IServer::Stop();
}

void Server::Update(float dt) {
	
}

void Server::ConnectCallback(Rpc* rpc) {
	if (isIpBanned(rpc->m_socket->GetHostName())) {
		rpc->Invoke("Error", std::string("ip banned"));
		rpc->UnregisterAll();

		// schedule a disconnect
		DisconnectLater(rpc, 2ms * rpc->m_socket->GetPing() + 40ms);
		return;
	}

	m_peers.push_back(std::make_unique<NetPeer>(rpc));

	rpc->Register("ServerHandshake", new Method(this, &Server::RPC_ServerHandshake));
	rpc->Register("Print", new Method(this, &Server::RPC_Print));
	rpc->Register("PeerInfo", new Method(this, &Server::RPC_PeerInfo));

	LOG(INFO) << rpc->m_socket->GetHostName() << " has connected";
}

void Server::DisconnectCallback(Rpc* rpc) {
	LOG(INFO) << rpc->m_socket->GetHostName() << " has disconnected";
	GetPeer(rpc)->m_rpc = nullptr; // Invalidate
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

void Server::addIpBan(const std::string& host) {
	m_bannedIps.insert(host);
}

void Server::addToWhitelist(const std::string& key) {
	m_whitelist.insert(key);
}

void Server::removeIpBan(const std::string& host) {
	m_bannedIps.erase(host);
}

void Server::removeFromWhitelist(const std::string& key) {
	m_whitelist.erase(key);
}

bool Server::isIpBanned(const std::string& host) {
	return m_bannedIps.contains(host);
}

bool Server::isWhitelisted(const std::string& key) {
	return m_whitelist.contains(key);
}

void Server::DisconnectLater(Rpc* rpc, std::chrono::milliseconds ms) {
	const auto now = std::chrono::steady_clock::now();

	RunTaskLater([this, rpc, ms]() {
		std::this_thread::sleep_for(ms);
		Disconnect(rpc);
	}, now + ms);
}

void Server::SaveUsers() {
	std::ofstream myfile;
	myfile.open("data\\banned_ips.txt", std::ios::out);
	if (myfile.is_open()) {
		for (auto&& s : m_bannedIps) {
			myfile << s << std::endl;
		}
		myfile.close();
	}

	myfile.open("data\\whitelist.txt", std::ios::out);
	if (myfile.is_open()) {
		for (auto&& s : m_whitelist) {
			myfile << s << std::endl;
		}
		myfile.close();
	}
}

void Server::LoadUsers() {
	std::ifstream myfile;
	myfile.open("data\\banned_ips.txt", std::ios::in);
	if (myfile.is_open()) {
		std::string line;
		while (std::getline(myfile, line)) {
			m_bannedIps.insert(line);
		}
		myfile.close();
	}

	myfile.open("data\\whitelist.txt", std::ios::in);
	if (myfile.is_open()) {
		std::string line;
		while (std::getline(myfile, line)) {
			m_whitelist.insert(line);
		}
		myfile.close();
	}
}
