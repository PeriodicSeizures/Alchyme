#include "IServer.h"

IServer::IServer(unsigned short port)
	: m_acceptor(m_ctx, tcp::endpoint(tcp::v4(), port)) {}

IServer::~IServer() {
	Stop();
}

void IServer::Start() {

	//std::cout << "Starting server on host " <<
	//	m_acceptor.local_endpoint().address().to_string() << "\n";

	DoAccept();

	m_alive = true;
	m_ctxThread = std::thread([this]() {m_ctx.run(); });

	auto last_tick = std::chrono::steady_clock::now();
	while (m_alive) {
		const auto now = std::chrono::steady_clock::now();
		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		last_tick = std::chrono::steady_clock::now();


		for (auto&& it = m_rpcs.begin(); it != m_rpcs.end();) {
			if (!(*it)->m_socket->IsConnected()) {
				DisconnectCallback(it->get());
				it = m_rpcs.erase(it);
			}
			else {
				(*it)->Update();
				++it;
			}
		}
		Update((double)dt / 1000000.);
		

		// reduces cpu usage
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void IServer::Stop() {
	m_alive = false;
	m_ctx.stop();

	if (m_ctxThread.joinable())
		m_ctxThread.join();

	m_ctx.restart();
}

bool IServer::IsAlive() {
	return m_alive;
}

void IServer::Disconnect(Rpc* rpc) {
	rpc->m_socket->Close();
}

void IServer::DoAccept() {
	m_acceptor.async_accept(
		[this](const asio::error_code& ec, tcp::socket socket) {
			if (!ec) {
				auto rpc = std::make_unique<Rpc>(
					std::make_shared<AsioSocket>(m_ctx, std::move(socket)));

				std::cout << rpc->m_socket->GetHostName() << " has connected\n";

				rpc->m_socket->Start();

				ConnectCallback(rpc.get());

				// register and invoke RPCS at this point
				//rpc.Invoke("print", 69420);
				//rpc.Invoke("noargs");

				m_rpcs.push_back(std::move(rpc));
			}
			else {
				std::cout << "error: " << ec.message() << "\n";
			}

			DoAccept();
		});

}