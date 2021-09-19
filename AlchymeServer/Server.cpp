#include "Server.h"

Server::Server(unsigned short port)
	: m_acceptor(m_ctx, tcp::endpoint(tcp::v4(), port)) {}

Server::~Server() {
	Stop();
}

void Server::Start() {
	DoAccept();

	m_alive = true;
	m_ctxThread = std::thread([this]() {
		m_ctx.run();
	});

	auto last_tick = std::chrono::steady_clock::now();
	while (m_alive) {
		const auto now = std::chrono::steady_clock::now();

		auto ticks_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();

		//on_tick(((float)ticks_elapsed) / 1000000.f);
		Update();
		last_tick = std::chrono::steady_clock::now();
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
		//precise_sleep(0.05);
	}
}

void Server::Stop() {
	m_alive = false;
	m_ctx.stop();

	if (m_ctxThread.joinable())
		m_ctxThread.join();

	m_ctx.restart();
}

void Server::Update() {

	// Update everything down the chain
	// iterate peers, and update

}

bool Server::IsAlive() {
	return m_alive;
}

void Server::DoAccept()
{
	m_acceptor.async_accept(
		[this](const asio::error_code& ec, tcp::socket socket)
		{
			if (!ec)
			{
				asio::ip::tcp::endpoint endpoint = socket.remote_endpoint();

				auto conn = std::make_shared<AsioSocket>(m_ctx, std::move(socket));

				// always establish connection first
				// handshake will be blocking

				// Whether to accept or deny the connection
				if (on_join(conn)) {
					conn->Accept();
					m_peers.push_back(NetPeer(conn));
				}
				else {
					/*
					* something to do on deny ...
					*/
				}
			}

			DoAccept();
		});

}

