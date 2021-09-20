#include "Server.h"

Server::Server(unsigned short port)
	: m_acceptor(m_ctx, tcp::endpoint(tcp::v4(), port)) {}

Server::~Server() {
	Stop();
}

void Server::Start() {
	DoAccept();

	m_alive = true;
	m_ctxThread = std::thread([this]() {m_ctx.run(); });

	auto last_tick = std::chrono::steady_clock::now();
	while (m_alive) {
		const auto now = std::chrono::steady_clock::now();

		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		Update((double)dt / 1000000.);
		last_tick = std::chrono::steady_clock::now();

		// reduces cpu usage
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Server::Stop() {
	m_alive = false;
	m_ctx.stop();

	if (m_ctxThread.joinable())
		m_ctxThread.join();

	m_ctx.restart();
}

void Server::Update(double dt) {

	//std::cout << "time running: " << m_elapsedTime << "\n";

	m_elapsedTime += dt;
	// Update everything down the chain
	// iterate peers, and update

	for (auto&& it = m_peers.begin(); it != m_peers.end();) {
		if (!it->m_socket->IsConnected()) {
			// remove it
			it = m_peers.erase(it);
		}
		else {
			it->m_rpc->Update();
			++it;
		}
	}

}

bool Server::IsAlive() {
	return m_alive;
}

void Server::DoAccept() {
	m_acceptor.async_accept(
		[this](const asio::error_code& ec, tcp::socket socket) {
			if (!ec) {

				std::cout << "Client has connected\n";

				NetPeer peer;
				peer.m_socket = std::make_shared<AsioSocket>(m_ctx, std::move(socket));
				peer.m_rpc = std::make_unique<Rpc>(peer.m_socket);

				peer.m_socket->Start();

				// register and invoke RPCS at this point
				peer.m_rpc->Invoke("print", 1);

				m_peers.push_back(std::move(peer));				
			}

			DoAccept();
		});

}

bool Server::OnJoin(AsioSocket::ptr) {
	return true;
}

void Server::OnQuit(AsioSocket::ptr) {
	// to do something when
}