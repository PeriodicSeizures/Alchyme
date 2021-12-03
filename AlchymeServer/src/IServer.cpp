#include "IServer.h"
#include "Utils.h"

IServer::IServer() {}

IServer::~IServer() {
	LOG(DEBUG) << "~IServer()";
	Stop();
}

void IServer::Run() {
	m_running = true;
	while (m_running) {
		auto now = std::chrono::steady_clock::now();
		static auto last_tick = now;
		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		last_tick = now;

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
		// UPDATE
		Update(dt / 1000000.f);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void IServer::StartAccepting(uint_least16_t port) {
	m_acceptor = std::make_unique<tcp::acceptor>(m_ctx, tcp::endpoint(tcp::v4(), port));

	LOG(INFO) << "Starting server on *:" << port;

	DoAccept();

	m_ctxThread = std::thread([this]() {
		el::Helpers::setThreadName("networker");
		m_ctx.run();
	});
}

void IServer::Stop() {
	m_running = false;
	m_ctx.stop();

	if (m_ctxThread.joinable())
		m_ctxThread.join();

	m_ctx.restart();
}

void IServer::RunTask(std::function<void()> event) {
	const auto now = std::chrono::steady_clock::now();
	m_taskQueue.push_back({ now, std::move(event) });
}

void IServer::RunTaskLater(std::function<void()> event, std::chrono::steady_clock::time_point at) {
	m_taskQueue.push_back({ at, std::move(event) });
}

void IServer::Disconnect(Rpc* rpc) {
	rpc->m_socket->Close();
}

void IServer::DoAccept() {
	m_acceptor->async_accept(
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
				LOG(ERROR) << ec.message();
			}

			DoAccept();
		});

}