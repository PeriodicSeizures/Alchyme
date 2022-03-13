#include <algorithm>
#include "AlchymeServer.hpp"
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std::chrono_literals;

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

#define SETTING(key, def) (settings.emplace(key, def).first->second)

AlchymeServer* AlchymeServer::Get() {
	return static_cast<AlchymeServer*>(AlchymeGame::Get());
}

AlchymeServer::AlchymeServer() : AlchymeGame(true) {}

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

	RunTaskLaterRepeat([this]() {
		for (auto& peer : m_peers) {
			//auto& rpc = peer->Rpc();
			//
			//// test for online and nullifying or deleting
			//if (rpc) {
			//	if (!isWhitelisted(peer->m_key)
			//		|| isIpBanned(rpc->m_socket->GetHostName()))
			//		DisconnectLater(rpc.get());
			//}
		}
	}, 0s, 5s);

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
			auto sock = std::make_shared<AsioSocket>(m_ctx, std::move(socket));

			LOG(INFO) << sock->GetHostName() << " has connected\n";

			sock->Accept();

			RunTask([this, sock]() {
				auto peer = std::make_unique<NetPeer>(sock);
				//ConnectCallback(peer->m_rpc.get());
				m_peers.push_back(std::move(peer));
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

void AlchymeServer::Update(float dt) {
	for (auto& peer : m_peers) {
		//auto& rpc = peer->m_rpc;
		//
		//// test for online and nullifying or deleting
		//if (rpc) {
		//	if (rpc->m_socket->Status() == IOStatus::CLOSED) {
		//		DisconnectCallback(rpc.get());
		//		rpc.reset();
		//	}
		//	else {
		//		peer->Update();
		//	}
		//}
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
	rpc->Register("ServerHandshake", new Method(this, &AlchymeServer::RPC_ServerHandshake));

	LOG(INFO) << rpc->m_socket->GetHostName() << " has connected";
}

void AlchymeServer::DisconnectCallback(Rpc* rpc) {
	LOG(INFO) << rpc->m_socket->GetHostName() << " has disconnected";
	// experiment with this
}



NetPeer* AlchymeServer::GetPeer(size_t uid) {
	for (auto&& peer : m_peers) {
		//if (peer->m_uid == uid)
			return peer.get();
	}
	return nullptr;
}

NetPeer* AlchymeServer::GetPeer(std::string name) {
	for (auto&& peer : m_peers) {
		//if (peer->name == name)
			return peer.get();
	}
	return nullptr;
}

NetPeer* AlchymeServer::GetPeer(Rpc* rpc) {
	for (auto&& peer : m_peers) {
		//if (peer->m_rpc.get() == rpc)
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
	if (m_useWhitelist)
		return true;

	return m_whitelist.contains(key);
}



void AlchymeServer::DisconnectLater(Rpc* rpc) {
	RunTaskLater([this, rpc]() {
		Disconnect(rpc);
	}, 1s);
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
