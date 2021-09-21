#include "IClient.h"

IClient::IClient() {}

IClient::~IClient() {
	Stop();
}

void IClient::Start() {
	if (m_alive)
		return;

	m_alive = true;
	m_ctxThread = std::thread([this]() {m_ctx.run(); });

	auto last_tick = std::chrono::steady_clock::now();

	while (m_alive) {
		const auto now = std::chrono::steady_clock::now();

		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		last_tick = std::chrono::steady_clock::now();

		if (m_rpc) {
			if (m_rpc->m_socket->WasDisconnected()) {
				DisconnectCallback(m_rpc.get());
				m_rpc.reset();
			}
			else {
				m_rpc->Update();
			}
		}
		Update((float)dt / 1000000.f);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

}

void IClient::Stop() {
	if (!m_alive)
		return;

	m_alive = false;
	m_ctx.stop();

	if (m_ctxThread.joinable())
		m_ctxThread.join();

	m_ctx.restart();
}

void IClient::Connect(std::string host, std::string port) {
	if (m_alive)
		return;

	try {
		m_rpc = std::make_unique<Rpc>(std::make_shared<AsioSocket>(m_ctx));

		asio::ip::tcp::resolver resolver(m_ctx);
		auto endpoints = resolver.resolve(asio::ip::tcp::v4(), host, port);

		std::cout << "Connecting to host " << host << ":" << port << "\n";

		asio::async_connect(
			m_rpc->m_socket->GetSocket(), endpoints.begin(), endpoints.end(),
			[this](const asio::error_code& ec, asio::ip::tcp::resolver::iterator it) {
				if (!ec) {
					std::cout << "Successfully connected\n";

					m_rpc->m_socket->Start();
					ConnectCallback(m_rpc.get());
				}
				else {
					std::cout << "error: " << ec.message() << "\n";
				}				
			}
		);
	}
	catch (std::system_error& e) {
		std::cout << "error: " << e.what() << "\n";
		return;
	}
}