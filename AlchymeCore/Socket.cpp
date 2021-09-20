#include "Socket.h"
#include <string>
#include <iostream>

//auto static constexpr UINT_MAX = std::numeric_limits<int>().max();

static constexpr size_t PING = MAXULONGLONG;
static constexpr size_t PONG = PING - 1;

//#define PING (INT_MAX)
//#define PONG (PING-1)

AsioSocket::AsioSocket(asio::io_context& ctx,
	asio::ip::tcp::socket socket)
	: m_socket(std::move(socket)),
	m_pingTimer(ctx),
	m_pongTimer(ctx)
{
	//Start();
}

AsioSocket::AsioSocket(asio::io_context& ctx)
	: m_socket(ctx),
	m_pingTimer(ctx),
	m_pongTimer(ctx) 
{
	//ConnectToHost(ctx, host, port);
}

//void AsioSocket::ConnectToHost(asio::io_context& ctx, std::string host, std::string port) {
//
//	if (m_connected)
//		Close();
//
//	asio::ip::tcp::resolver resolver(ctx);
//	auto endpoints = resolver.resolve(asio::ip::tcp::v4(), host, port);
//
//	asio::async_connect(m_socket, endpoints,
//		std::bind(&AsioSocket::Start, this));
//}

void AsioSocket::Start() {
	m_connected = true;

	// begin reader
	ReadHeader();

	/*
	* Send ping
	*/
	Send({ PING });

	m_pongTimer.expires_after(std::chrono::seconds(10));
	m_pongTimer.async_wait(std::bind(&AsioSocket::CheckPong, this));
}

asio::ip::tcp::socket& AsioSocket::GetSocket() {
	return m_socket;
}

//AsioSocket::~AsioSocket() {
//	//Close();
//}

bool AsioSocket::IsConnected() {
	return m_connected;
}

void AsioSocket::Send(Packet packet) {
	const bool was_empty = m_sendQueue.empty();
	m_sendQueue.push_back(std::move(packet));
	if (was_empty)
		WriteHeader();
}

Packet AsioSocket::Recv() {
	return std::move(m_recvQueue.pop_front());
}

int AsioSocket::GetSendQueueSize() {
	return static_cast<int>(m_sendQueue.count());
}

int AsioSocket::GetCurrentSendRate() {
	return -1;
}

bool AsioSocket::GotNewData() {
	return !m_recvQueue.empty();
}

void AsioSocket::Close() {
	if (m_connected) {
		m_connected = false;

		asio::error_code ec;
		m_socket.close(ec);

		m_pongTimer.cancel();
	}
}

void AsioSocket::GetConnectionQuality(float& localQuality, float& remoteQuality, int& ping, float& outByteSec, float& inByteSec) {
	// do nothing
	ping = this->m_ping;
}

void AsioSocket::ReadHeader() {
	auto self(shared_from_this());
	asio::async_read(m_socket,
		asio::buffer(&m_inPacket.offset, sizeof(Packet::offset)),
		[this, self](const std::error_code& e, size_t) {
			if (!e) {
				//LOG_DEBUG("read_header()\n");

				std::cout << "Read size: " << m_inPacket.offset << "\n";

				// Offset is size, with some special macro behaviour
				switch (m_inPacket.offset) {
					
				case PING: { // if PING received, reply with PONG

					std::cout << "Ponging!\n";

					Send({ PONG });
					ReadHeader();
					break;
				} case PONG: { // if PONG received, record timings

					std::cout << "Recording timings!\n";

					auto now = std::chrono::steady_clock::now();
					m_ping = static_cast<uint16_t>(
						std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_ping).count());

					m_pingTimer.expires_after(std::chrono::seconds(5));
					m_pingTimer.async_wait(std::bind(&AsioSocket::CheckPing, this));

					ReadHeader();
					break;
				}
				default: {
					ReadBody();
				}
				}
			}
			else {
				//LOG_DEBUG("read header error: %s\n", e.message().c_str());
				Close();
			}
		}
	);
}

void AsioSocket::ReadBody() {
	// The packet.offset will serve as the size
	m_inPacket.m_buf.resize(m_inPacket.offset);

	std::cout << "Buf resized: " << m_inPacket.m_buf.size() << "\n";

	if (m_inPacket.m_buf.empty()) {
		std::cout << "Empty body\n";
		ReadHeader();
	} else {
		auto self(shared_from_this());
		asio::async_read(m_socket,
			asio::buffer(m_inPacket.m_buf),
			[this, self](const std::error_code& e, size_t) {
				if (!e) {
					m_recvQueue.push_back({ 0, m_inPacket.m_buf });

					//LOG_DEBUG("read_body()\n");

					ReadHeader();
				}
				else {
					//LOG_DEBUG("read body error: %s\n", e.message().c_str());
					Close();
				}
			}
		);
	}
}

void AsioSocket::WriteHeader() {
	/*
	* When about to send out the PING packet, record the current time
	*/
	if (m_sendQueue.front().offset == PING) {
		m_last_ping = std::chrono::steady_clock::now();
	}

	auto self(shared_from_this());
	asio::async_write(m_socket,
		asio::buffer(&(m_sendQueue.front().offset), sizeof(Packet::offset)),

		[this, self](const std::error_code& e, size_t) {
			if (!e) {
				//LOG_DEBUG("write_header()\n");

				WriteBody();
			}
			else {
				if (e.value() != asio::error::operation_aborted) {
					std::cout << "Asio error " << e.message().c_str() << "\n";
				}
				//LOG_DEBUG("write header error: %s\n", e.message().c_str());
				Close();
			}
		}
	);
}

void AsioSocket::WriteBody() {

	// Dont bother sending body data if vector is empty
	
	auto front = m_sendQueue.pop_front();

	if (front.m_buf.empty()) {
		//m_sendQueue.pop_front();
		if (!m_sendQueue.empty())
			WriteHeader();
	} else {
		std::cout << "Ok3\n";
		auto self(shared_from_this());
		asio::async_write(m_socket,
			asio::buffer(front.m_buf.data(), front.offset),
			[this, self](const std::error_code& e, size_t) {
				if (!e) {
					//LOG_DEBUG("write_body()\n");

					//out_packets.pop_front();
					//m_sendQueue.pop_front();
					std::cout << "Ok4\n";

					if (!m_sendQueue.empty())
						WriteHeader();
				}
				else {
					//LOG_DEBUG("write body error: %s\n", e.message().c_str());
					Close();
				}
			}
		);
	}
}

void AsioSocket::CheckPong() {
	// Check whether the deadline has passed. We compare the deadline against
	// the current time since a new asynchronous operation may have moved the
	// deadline before this actor had a chance to run.
	if (m_pongTimer.expiry() <= asio::steady_timer::clock_type::now())
	{
		//LOG_DEBUG("pong timeout\n");

		// The deadline has passed. The socket is closed so that any outstanding
		// asynchronous operations are cancelled.
		//m_socket.cancel();
		m_socket.close();

		// There is no longer an active deadline. The expiry is set to the
		// maximum time point so that the actor takes no action until a new
		// deadline is set.
		m_pongTimer.expires_at(asio::steady_timer::time_point::max());
	}

	// Put the actor back to sleep.
	m_pongTimer.async_wait(std::bind(&AsioSocket::CheckPong, this));

}

void AsioSocket::CheckPing() {
	if (!m_connected)
		return;

	if (m_pingTimer.expiry() <= asio::steady_timer::clock_type::now()) {
		Send({ PING });

		m_pongTimer.expires_after(std::chrono::seconds(10));
	}
}
