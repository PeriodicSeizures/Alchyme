#include "IServer.h"
#include "Utils.h"

IServer::IServer(unsigned short port, unsigned short authPort)
	: m_acceptor(m_ctx, tcp::endpoint(tcp::v4(), port)),
	m_authAcceptor(m_ctx, tcp::endpoint(tcp::v4(), authPort)){
	std::cout << "Server port set to " << port << "\n";
}

IServer::~IServer() {
	LOG_DEBUG("IServer::~IServer()\n");
	Disconnect();
}

void IServer::StartListening() {

	LOG_DEBUG("Starting server\n");

	//std::cout << "Starting server on host " <<
	//	m_acceptor.local_endpoint().address().to_string() << "\n";

	DoAccept();

	m_alive = true;
	m_ctxThread = std::thread([this]() { m_ctx.run(); });

	auto last_tick = std::chrono::steady_clock::now();
	while (m_alive) {
		auto now = std::chrono::steady_clock::now();
		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		last_tick = now; // std::chrono::steady_clock::now();

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
		Update(dt / 1000000.f);

		m_timeSinceStart += dt / 1000000.f;

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void IServer::Disconnect() {
	m_alive = false;
	m_ctx.stop();

	if (m_ctxThread.joinable())
		m_ctxThread.join();

	m_ctx.restart();
}

bool IServer::IsAlive() {
	return m_alive;
}

void IServer::Disconnect(Rpc* rpc) { // , bool doCloseAfterSends
	//if (doCloseAfterSends)
	//	rpc->m_socket->CloseAfterNextSends();
	//else
	//if (rpc && rpc->m_socket)
		rpc->m_socket->Close();
}

double IServer::getTimeSinceStart() {
	return m_timeSinceStart;
}

void IServer::DoAccept() {
	m_acceptor.async_accept(
		[this](const asio::error_code& ec, tcp::socket socket) {
			if (!ec) {
				auto rpc = std::make_unique<Rpc>(
					std::make_shared<AsioSocket>(m_ctx, std::move(socket)));

				//std::cout << rpc->m_socket->GetHostName() << " has connected\n";

				rpc->m_socket->Accept();

				ConnectCallback(rpc.get());

				// register and invoke RPCS at this point
				//rpc.Invoke("print", 69420);
				//rpc.Invoke("noargs");

				m_rpcs.push_back(std::move(rpc));
			}
			else {
				std::cerr << "error: " << ec.message() << "\n";
			}

			DoAccept();
		});

}

void IServer::DoAuthAccept() {
	m_authAcceptor.async_accept(
		[this](const asio::error_code& ec, tcp::socket socket) {
		if (!ec) {

			//m_auths

			auto rpc = std::make_unique<Rpc>(
				std::make_shared<AsioSocket>(m_ctx, std::move(socket)));

			//std::cout << rpc->m_socket->GetHostName() << " has connected\n";

			rpc->m_socket->Accept();

			ConnectCallback(rpc.get());

			// register and invoke RPCS at this point
			//rpc.Invoke("print", 69420);
			//rpc.Invoke("noargs");

			m_rpcs.push_back(std::move(rpc));
		}
		else {
			std::cerr << "error: " << ec.message() << "\n";
		}

		DoAccept();
	});

}