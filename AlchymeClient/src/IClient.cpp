#include "IClient.h"
#include <optick.h>

IClient::IClient() {}

IClient::~IClient() {
	LOG(DEBUG) << "~IClient()";
	Stop();
}

Rpc* IClient::GetRpc() {
	return m_rpc.get();
}

void IClient::RunTask(std::function<void()> event) {
	const auto now = std::chrono::steady_clock::now();
	m_taskQueue.push_back({ now, std::move(event) });
}

void IClient::RunTaskLater(std::function<void()> event, std::chrono::steady_clock::time_point at) {
	m_taskQueue.push_back({ at, std::move(event) });
}

void IClient::Run() {
	m_running = true;
	while (m_running) {
		OPTICK_FRAME("MainThread");

		// DELTAS
		const auto now = std::chrono::steady_clock::now();
		static auto last_tick = now;

		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		last_tick = std::chrono::steady_clock::now();

		// TASKS
		while (!m_taskQueue.empty()) {
			const auto now = std::chrono::steady_clock::now();
			if (m_taskQueue.front().at < now)
				m_taskQueue.pop_front().function();
		}

		// RPC
		if (m_rpc)
			if (m_rpc->m_socket->WasDisconnected()) {
				DisconnectCallback(m_rpc.get());
				m_rpc.reset();
			}
			else {
				m_rpc->Update();
			}

		// UPDATE
		Update((float)dt / 1000000.f);
	}
}

void IClient::Stop() {
	m_running = false;
	Disconnect();
}

void IClient::Connect(std::string host, std::string port) {
	if (m_rpc)
		return;

	m_rpc = std::make_unique<Rpc>(
		std::make_shared<AsioSocket>(m_ctx));

	asio::ip::tcp::resolver resolver(m_ctx);
	auto endpoints = resolver.resolve(asio::ip::tcp::v4(), host, port);

	LOG(INFO) << "Connecting to " << host << ":" << port;

	asio::async_connect(
		m_rpc->m_socket->GetSocket(), endpoints.begin(), endpoints.end(),
		[this](const asio::error_code& ec, asio::ip::tcp::resolver::results_type::iterator it) {

		ConnResult res = ConnResult::OK;

		if (!ec) {
			LOG(INFO) << "Successfully connected";

			m_rpc->m_socket->Accept();
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
			else if (ec.value() == 10061) { // might be win32 only
				res = ConnResult::NOT_FOUND;
			}
			else {
				res = ConnResult::OTHER;
			}

			LOG(ERROR) << "Failed to connect to server (" << (int)res << ")";

			m_rpc.reset();
		}

		RunTask([this, res] {
			ConnectCallback(m_rpc.get(), res);
		});

	});

	m_ctxThread = std::thread([this]() {
		el::Helpers::setThreadName("networker");
		m_ctx.run();
	});
}

void IClient::Disconnect() {
	//if (!m_rpc)
	//	return;

	m_rpc.reset();

	m_ctx.stop();
	if (m_ctxThread.joinable())
		m_ctxThread.join();

	m_ctx.restart();
}
