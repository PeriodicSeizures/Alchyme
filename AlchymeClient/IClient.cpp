#include "IClient.h"

IClient::IClient() {}

IClient::~IClient() {
	Disconnect();
}

void IClient::Connect(std::string host, std::string port) {
	if (m_alive)
		return;

	m_rpc = std::make_unique<Rpc>(
		std::make_shared<AsioSocket>(m_ctx));

	asio::ip::tcp::resolver resolver(m_ctx);
	auto endpoints = resolver.resolve(asio::ip::tcp::v4(), host, port);

	//std::cout << "Connecting to host " << host << ":" << port << "\n";

	asio::async_connect(
		m_rpc->m_socket->GetSocket(), endpoints.begin(), endpoints.end(),
		[this](const asio::error_code& ec, asio::ip::tcp::resolver::iterator it) {
		if (!ec) {
			//std::cout << "Successfully connected\n";

			m_rpc->m_socket->Accept();
			ConnectCallback(m_rpc.get(), ConnResult::OK);
		}
		else {
			ConnResult res;
			// 10060
			if (ec == asio::error::timed_out) {
				res = ConnResult::TIMEOUT;
			}
			else if (ec == asio::error::operation_aborted) {
				res = ConnResult::ABORT;
			}
			else if (ec == asio::error::not_found) {
				res = ConnResult::NOT_FOUND;
			}
			else {
				res = ConnResult::OTHER;
				std::cout << __LINE__ << " " << __FILE__ << " other asio error\n";
				std::cout << "error: " << ec.value() << " " << ec.message() << "\n";
			}
			ConnectCallback(nullptr, res);
		}
	}
	);

	m_alive = true;
	m_ctxThread = std::thread([this]() {m_ctx.run(); });
}

void IClient::Disconnect() {
	if (!m_alive)
		return;

	m_alive = false;
	m_ctx.stop();

	m_rpc.reset();

	if (m_ctxThread.joinable())
		m_ctxThread.join();

	m_ctx.restart();
}

void IClient::Update() {
	if (m_rpc)
		if (m_rpc->m_socket->WasDisconnected()) {
			DisconnectCallback(m_rpc.get());
			m_rpc.reset();
		}
		else {
			m_rpc->Update();
		}
}
