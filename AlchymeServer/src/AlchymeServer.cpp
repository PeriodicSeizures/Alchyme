#include <algorithm>
#include "AlchymeServer.hpp"
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std::chrono_literals;

#define SETTING(key, def) (settings.emplace(key, def).first->second)

std::unique_ptr<AlchymeServer> server;

AlchymeServer* AlchymeServer::Get() {
	return server.get();
}

void AlchymeServer::Run() {
	server = std::make_unique<AlchymeServer>();
	server->Start();
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



AlchymeServer::AlchymeServer() {}

//AlchymeServer::~AlchymeServer() {}

static std::string consoleInput() {
#ifdef _WIN32
	SetThreadDescription(GetCurrentThread(), L"ConsoleInputThread");
#endif
	std::string in;
	std::getline(std::cin, in);
	return in;
}

void AlchymeServer::Start() {
	// Load all stuff here

	consoleFuture = std::async(consoleInput);

	if (!loadSettings(settings))
		throw std::runtime_error("could not load settings.txt");

	m_useWhitelist = SETTING("use-whitelist", "false") == "true";

	LoadUsers();

	asio::ip::port_type port = std::stoi(SETTING("port", "8001"));
	m_acceptor = std::make_unique<tcp::acceptor>(m_ctx, tcp::endpoint(tcp::v4(), port));
	LOG(INFO) << "Starting server on *:" << port;
	DoAccept();

	AlchymeGame::StartIOThread();
	AlchymeGame::Start(); // Will begin loop
}

void AlchymeServer::Stop() {
	AlchymeGame::Stop();
}

void AlchymeServer::DoAccept() {
	m_acceptor->async_accept(
		[this](const asio::error_code& ec, tcp::socket socket) {
		if (!ec) {
			auto rpc = new Rpc(std::make_shared<AsioSocket>(m_ctx, std::move(socket)));

			LOG(INFO) << rpc->m_socket->GetHostName() << " has connected\n";

			rpc->m_socket->Accept();

			RunTask([this, rpc]() {
				ConnectCallback(rpc);
				//m_rpcs.push_back(std::unique_ptr<Rpc>(rpc));
				m_peers.push_back(std::make_unique<NetPeer>(rpc));
			});
		}
		else {
			//asio::error::connection_reset			WSAECONNRESET	10054
			//asio::error::fault					WSAFAULT		10014
			//asio::error::interrupted				WSAEINTR		10004
			//asio::error::in_progress				WSAEINPROGRESS	10036
			//asio::error::no_descriptors			WSAEMFILE		10024
			//asio::error::network_down				WSAENETDOWN		10050
			//asio::error::no_buffer_space			WSAENOBUFS		10055
			//asio::error::not_socket				WSAENOTSOCK		10038
			//asio::error::operation_not_supported	WSAEOPNOTSUPP	10045
			//asio::error::would_block				WSAEWOULDBLOCK	10035

			LOG(ERROR) << ec.message();
		}

		DoAccept();
	});
}

void AlchymeServer::PreUpdate(float dt) {
	for (auto& peer : m_peers) {
		auto& rpc = peer->m_rpc;

		// test for online and nullifying or deleting
		if (rpc) {
			if (rpc->m_socket->Closed())
				DisconnectCallback(rpc);
			else {
				// TODO whitelist could be split among each peer?
				// good for a warmup optimization,
				// but will add confusion for new player join
				// 
				if ((m_useWhitelist && m_whitelist.find(peer->m_key) == m_whitelist.end())
					|| m_bannedIps.find(rpc->m_socket->GetHostName()) != m_bannedIps.end())
					DisconnectLater(rpc, 2ms * rpc->m_socket->GetPing() + 40ms);
				else
					rpc->Update();
			}
		}
	}
}

void AlchymeServer::Update(float dt) {
	using namespace std::chrono_literals;
	auto now = std::chrono::steady_clock::now();

	auto delta = now - m_lastBanCheck;
	if (delta > 5s) {
		m_lastBanCheck = std::chrono::steady_clock::now();
	}

	if (consoleFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
		// get value and reassign
		// do something with the input
		std::string in = consoleFuture.get();
		LOG(INFO) << "Got command: " << in;
		consoleFuture = std::async(consoleInput);
	}
}



void AlchymeServer::ConnectCallback(Rpc* rpc) {
	if (isIpBanned(rpc->m_socket->GetHostName())) {
		rpc->Invoke("Error", std::string("ip banned"));
		rpc->UnregisterAll();

		// schedule a disconnect
		DisconnectLater(rpc, 2ms * rpc->m_socket->GetPing() + 40ms);
		return;
	}

	rpc->Register("ServerHandshake", new Method(this, &AlchymeServer::RPC_ServerHandshake));
	rpc->Register("Print", new Method(this, &AlchymeServer::RPC_Print));
	rpc->Register("PeerInfo", new Method(this, &AlchymeServer::RPC_PeerInfo));

	LOG(INFO) << rpc->m_socket->GetHostName() << " has connected";
}

void AlchymeServer::DisconnectCallback(Rpc* rpc) {
	LOG(INFO) << rpc->m_socket->GetHostName() << " has disconnected";
	GetPeer(rpc)->m_rpc = nullptr; // Invalidate
}



NetPeer* AlchymeServer::GetPeer(size_t uid) {
	for (auto&& peer : m_peers) {
		if (peer->m_uid == uid)
			return peer.get();
	}
	return nullptr;
}

NetPeer* AlchymeServer::GetPeer(std::string name) {
	for (auto&& peer : m_peers) {
		if (peer->name == name)
			return peer.get();
	}
	return nullptr;
}

NetPeer* AlchymeServer::GetPeer(Rpc* rpc) {
	for (auto&& peer : m_peers) {
		if (peer->m_rpc == rpc)
			return peer.get();
	}
	return nullptr;
}



void AlchymeServer::addIpBan(const std::string& host) {
	m_bannedIps.insert(host);
}

void AlchymeServer::addToWhitelist(const std::string& key) {
	m_whitelist.insert(key);
}

void AlchymeServer::removeIpBan(const std::string& host) {
	m_bannedIps.erase(host);
}

void AlchymeServer::removeFromWhitelist(const std::string& key) {
	m_whitelist.erase(key);
}

bool AlchymeServer::isIpBanned(const std::string& host) {
	return m_bannedIps.contains(host);
}

bool AlchymeServer::isWhitelisted(const std::string& key) {
	return m_whitelist.contains(key);
}



void AlchymeServer::DisconnectLater(Rpc* rpc, std::chrono::milliseconds ms) {
	const auto now = std::chrono::steady_clock::now();

	RunTaskLater([this, rpc, ms]() {
		std::this_thread::sleep_for(ms);
		Disconnect(rpc);
	}, now + ms);
}

void AlchymeServer::SaveUsers() {
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

void AlchymeServer::LoadUsers() {
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

void AlchymeServer::Disconnect(Rpc* rpc) {
	rpc->m_socket->Close();
}
