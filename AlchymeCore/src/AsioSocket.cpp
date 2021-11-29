#include "Socket.h"
#include <string>
#include <iostream>

#include "Utils.h"

using namespace std::chrono_literals;

static constexpr size_t PING = MAXULONGLONG;
static constexpr size_t PONG = PING - 1;

#define PING_INTERVAL 5s
#define PONG_TIMEOUT 10s

AsioSocket::AsioSocket(asio::io_context& ctx,
	asio::ip::tcp::socket socket)
	: m_socket(std::move(socket)),
	m_pingTimer(ctx),
	m_pongTimer(ctx) {}

AsioSocket::AsioSocket(asio::io_context& ctx)
	: m_socket(ctx),
	m_pingTimer(ctx),
	m_pongTimer(ctx) {}

void AsioSocket::Accept() {
	m_connected = true;

	address = m_socket.remote_endpoint().address().to_string();
	port = m_socket.remote_endpoint().port();

	// begin reader
	ReadHeader();

	/*
	* Send ping
	*/
	Send({ PING });

	m_pongTimer.expires_after(PONG_TIMEOUT);
	m_pongTimer.async_wait(std::bind(&AsioSocket::CheckPong, this));
}

asio::ip::tcp::socket& AsioSocket::GetSocket() {
	return m_socket;
}

AsioSocket::~AsioSocket() {
	LOG_DEBUG("~AsioSocket()");
}

bool AsioSocket::IsConnected() {
	return m_connected;
}

bool AsioSocket::WasDisconnected() {
	return m_wasDisconnected;
}

void AsioSocket::Send(Packet packet) {
	const bool was_empty = m_sendQueue.empty();
	m_sendQueue.push_back(std::move(packet));
	if (was_empty) {
		LOG_DEBUG("Reinitiating Writer");
		WriteHeader();
	}
}

Packet AsioSocket::Recv() {
	return std::move(m_recvQueue.pop_front());
}

int AsioSocket::GetSendQueueSize() {
	return static_cast<int>(m_sendQueue.count());
}

bool AsioSocket::GotNewData() {
	return !m_recvQueue.empty();
}

void AsioSocket::Close() {
	if (m_connected) {
		LOG_DEBUG("AsioSocket::Close()");
		m_wasDisconnected = true;
		m_connected = false;

		m_pongTimer.cancel();
		m_pingTimer.cancel();

		//asio::error_code ec;
		m_socket.close();// ec);

		//this->disconnectResult = disconnectResult;
	}
}

int AsioSocket::GetPing() {
	return this->m_ping;
}

std::string AsioSocket::GetHostName() {
	return this->address;
}

uint_least16_t AsioSocket::GetHostPort() {
	return port;
}

//DisconnectResult AsioSocket::GetDisconnectResult() {
//	return disconnectResult;
//}

void AsioSocket::ReadHeader() {
	auto self(shared_from_this());
	asio::async_read(m_socket,
		asio::buffer(&m_inPacket.offset, sizeof(Packet::offset)),
		[this, self](const std::error_code& e, size_t) {
			if (!e) {
				switch (m_inPacket.offset) {
					
				case PING: { // if PING received, reply with PONG
					Send({ PONG });
					ReadHeader();
					break;
				} case PONG: { // if PONG received, record timings

					auto now = std::chrono::steady_clock::now();

					auto diff(now - m_pingTimer.expiry());
					// Failsafe buffer time
					// 
					// diff will be closer to zero the more 
					// accurate and equal interval it is since
					// the last ping timer
					if (diff > 2s && diff <= PING_INTERVAL) {
						LOG_DEBUG("possible pong spam: ");
						Close();
						break;
					}

					m_ping = static_cast<uint16_t>(
						std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_ping).count());

					m_pingTimer.expires_after(PING_INTERVAL);
					m_pingTimer.async_wait(std::bind(&AsioSocket::CheckPing, this));

					LOG_DEBUG("Ping: " << m_ping.load());

					ReadHeader();
					break;
				} default: ReadBody();
				}
			}
			else {
				LOG_DEBUG("read header error: " << e.message() << " (" << e.value() << ")");

				Close();
			}
		}
	);
}

void AsioSocket::ReadBody() {
	// The packet.offset will serve as the size
	m_inPacket.m_buf.resize(m_inPacket.offset);

	if (m_inPacket.m_buf.empty()) {
		ReadHeader();
	} else {
		auto self(shared_from_this());
		asio::async_read(m_socket,
			asio::buffer(m_inPacket.m_buf),
			[this, self](const std::error_code& e, size_t) {
				if (!e) {
					m_recvQueue.push_back({ 0, m_inPacket.m_buf });

					LOG_DEBUG("read_body()");
					ReadHeader();
				}
				else {
					LOG_DEBUG("read body error: " << e.message() << " (" << e.value() << ")");
					//std::cerr << "read body error: " << e.message().c_str() << "\n";
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

	LOG_DEBUG("write_header()");

	auto self(shared_from_this());
	asio::async_write(m_socket,
		asio::buffer(&(m_sendQueue.front().offset), sizeof(Packet::offset)),

		[this, self](const std::error_code& e, size_t) {
			if (!e) {
				LOG_DEBUG("Wrote header");
				auto front = m_sendQueue.front();
				if (!front.m_buf.empty()) {
					WriteBody(std::move(front));
				}
				else {
					m_sendQueue.pop_front();
					if (!m_sendQueue.empty()) {
						WriteHeader();
					}
				}
			}
			else {
				
				LOG_DEBUG("write header error: " << e.message() << " (" << e.value() << ")");
				//std::cerr << "write header error: " << e.message().c_str() << "\n";
				Close();
			}
		}
	);
}

void AsioSocket::WriteBody(Packet front) {

	LOG_DEBUG("write_body()");

	auto self(shared_from_this());
	asio::async_write(m_socket,
		asio::buffer(front.m_buf.data(), front.offset),
		[this, self](const std::error_code& e, size_t) {
		if (!e) {
			LOG_DEBUG("Wrote body\n");

			m_sendQueue.pop_front();
			if (!m_sendQueue.empty()) {
				WriteHeader();
			}
		}
		else {
			LOG_DEBUG("write body error: " << e.message() << " (" << e.value() << ")");
			//std::cerr << "write body error: " << e.message().c_str() << "\n";
			Close();
		}
	}
	);
}

void AsioSocket::CheckPong() {
	if (!m_connected)
		return;

	// Check whether the deadline has passed. We compare the deadline against
	// the current time since a new asynchronous operation may have moved the
	// deadline before this actor had a chance to run.
	if (m_pongTimer.expiry() <= asio::steady_timer::clock_type::now())
	{
		LOG_DEBUG("Pong not received in time");

		// The deadline has passed. The socket is closed so that any outstanding
		// asynchronous operations are cancelled.
		//m_socket.cancel();
		m_socket.close();

		// There is no longer an active deadline. The expiry is set to the
		// maximum time point so that the actor takes no action until a new
		// deadline is set.
		m_pongTimer.expires_at(asio::steady_timer::time_point::max());
		return;
	}

	// Put the actor back to sleep.
	m_pongTimer.async_wait(std::bind(&AsioSocket::CheckPong, this));

}

void AsioSocket::CheckPing() {
	if (!m_connected)
		return;

	if (m_pingTimer.expiry() <= asio::steady_timer::clock_type::now()) {
		Send({ PING });

		m_pongTimer.expires_after(PONG_TIMEOUT);
	}
}
