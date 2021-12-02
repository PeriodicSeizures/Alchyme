#include "IServer.h"
#include "Utils.h"

IServer::IServer(unsigned short port)
	: m_acceptor(m_ctx, tcp::endpoint(tcp::v4(), port)) {
	LOG(DEBUG) << "Server port set to " << port;
}

IServer::~IServer() {
	LOG(DEBUG) << "IServer::~IServer()";
	Disconnect();
}

void IServer::StartListening() {

	LOG(DEBUG) << "Starting server";

	//std::cout << "Starting server on host " <<
	//	m_acceptor.local_endpoint().address().to_string() << "\n";

	DoAccept();

	m_alive = true;
	m_ctxThread = std::thread([this]() { 
		el::Helpers::setThreadName("networker");
		m_ctx.run(); });

	auto last_tick = std::chrono::steady_clock::now();
	while (m_alive) {
		auto now = std::chrono::steady_clock::now();
		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		last_tick = now; // std::chrono::steady_clock::now();

		while (!m_taskQueue.empty()) {
			const auto now = std::chrono::steady_clock::now();
			if (m_taskQueue.front().at < now)
				m_taskQueue.pop_front().function();
		}

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

void IServer::RunTask(std::function<void()> event) {
	const auto now = std::chrono::steady_clock::now();
	m_taskQueue.push_back({ now, std::move(event) });
}

void IServer::RunTaskLater(std::function<void()> event, std::chrono::steady_clock::time_point at) {
	m_taskQueue.push_back({ at, std::move(event) });
}

void IServer::Disconnect(Rpc* rpc) { // , bool doCloseAfterSends
	//if (doCloseAfterSends)
	//	rpc->m_socket->CloseAfterNextSends();
	//else
	//if (rpc && rpc->m_socket)
		rpc->m_socket->Close();
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

				m_rpcs.push_back(std::move(rpc));
			}
			else {
				LOG(DEBUG) << "error: " << ec.message();
			}

			DoAccept();
		});

}