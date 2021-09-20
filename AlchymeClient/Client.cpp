#include "Client.h"

Client::Client() {}

Client::~Client() {
	Stop();
}

void Client::Start() {

	if (m_alive)
		return;

	m_alive = true;
	m_ctxThread = std::thread([this]() {m_ctx.run(); });

	auto last_tick = std::chrono::steady_clock::now();
	//auto last_render = std::chrono::steady_clock::now();

	while (m_alive) {
		//std::unique_lock<std::mutex> ul(mux);
		//cv.wait(ul);

		const auto now = std::chrono::steady_clock::now();

		/*
		* Tick every 16.6ms, strictly
		*/
		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		Update((float)dt / 1000000.f);
		last_tick = std::chrono::steady_clock::now();

		/*
		* Render every 16.6ms, strictly
		*   the problem with meshing render and ticker in same thread
		*   is that renderer can delay the most, and steal cycles
		*/
		//auto renders_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_render).count();
		//if (renders_elapsed >= 16666) {
		//	// render and reset
		//	Render();
		//	last_render = std::chrono::steady_clock::now();
		//}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

}

void Client::Stop() {
	if (!m_alive)
		return;

	m_alive = false;
	m_ctx.stop();

	if (m_ctxThread.joinable())
		m_ctxThread.join();

	m_ctx.restart();
}

void Client::Connect(std::string host, std::string port) {

	// io_context must not be running while connecting
	if (m_alive)
		return;

	m_peer.m_socket = std::make_shared<AsioSocket>(m_ctx);
	m_peer.m_rpc = std::make_unique<Rpc>(m_peer.m_socket);

	//m_peer.m_socket->ConnectToHost(m_ctx, host, port);

	asio::ip::tcp::resolver resolver(m_ctx);
	auto endpoints = resolver.resolve(asio::ip::tcp::v4(), host, port);

	asio::async_connect(m_peer.m_socket->GetSocket(), endpoints,
		std::bind(&Client::ConnectCallback, this));

	//m_peer.m_socket->Start();
	
	// Rpc should 	
}

void RPC_Print(Rpc* rpc, int a) {
	std::cout << "I was remotely called from the server!\n";
}

//void Client::RPC_Print(Rpc* rpc, int a) {
//	std::cout << "I was remotely called from the server!\n";
//}

void Client::ConnectCallback() {
	m_peer.m_socket->Start();

	//Method m(RPC_Print);

	// Now register rpc stuff
	m_peer.m_rpc->Register("print", new Method(RPC_Print));
}

uint16_t Client::latency() {
	//return connection->latency();
	return 0;
}

void Client::Update(float dt) {
	m_peer.m_rpc->Update();
}