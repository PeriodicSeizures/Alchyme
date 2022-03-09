#include "Socket.h"
#include <string>
#include <iostream>

#include "Utils.h"

using namespace std::chrono_literals;

static constexpr decltype(Packet::offset) PING = MAXULONGLONG;
static constexpr decltype(Packet::offset) PONG = PING - 1;

#define PING_INTERVAL 10000s //5s
#define PONG_TIMEOUT 10000s //10s

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
	m_status = IOStatus::OPEN;

	//m_connected = true;
	//m_wasDisconnected = false;

	address = m_socket.remote_endpoint().address().to_string();
	port = m_socket.remote_endpoint().port();

	// begin reader
	ReadHeader();

	/*
	* Send ping
	*/
	SendPing();
	m_pongTimer.async_wait(std::bind(&AsioSocket::CheckPong, this));
}

bool AsioSocket::Closed() {
	return m_status == IOStatus::SELF_CLOSED
		|| m_status == IOStatus::HOST_CLOSED;
}

asio::ip::tcp::socket& AsioSocket::GetSocket() {
	return m_socket;
}

AsioSocket::~AsioSocket() {
	Close();
	LOG(DEBUG) << "~AsioSocket()";
}

//bool AsioSocket::IsConnected() {
//	return m_connected;
//}
//
//bool AsioSocket::WasDisconnected() {
//	return m_wasDisconnected;
//}

IOStatus AsioSocket::Status() {
	return m_status;
}

void AsioSocket::Send(Packet packet) {
	const bool was_empty = m_sendQueue.empty();
	m_sendQueue.push_back(std::move(packet));
	if (was_empty) {
		LOG(DEBUG) << "Reinitiating Writer";
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

void AsioSocket::HostClose() {
	Close();
	m_status = IOStatus::HOST_CLOSED;
	//m_wasUserDisconnected = false;
}

void AsioSocket::Close() {
	if (m_status == IOStatus::OPEN) {
		m_status = IOStatus::SELF_CLOSED;
		LOG(DEBUG) << "AsioSocket::Close()";

		m_pongTimer.cancel();
		m_pingTimer.cancel();

		m_socket.close();
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
						LOG(DEBUG) << "possible pong spam: ";
						Close();
						break;
					}

					m_ping = static_cast<uint16_t>(
						std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_ping).count());

					m_pingTimer.expires_after(PING_INTERVAL);
					m_pingTimer.async_wait(std::bind(&AsioSocket::CheckPing, this));

					LOG(DEBUG) << "Ping: " << m_ping.load();

					ReadHeader();
					break;
				} default: ReadBody();
				}
			}
			else {
				LOG(DEBUG) << "read header error: " << e.message() << " (" << e.value() << ")";

				HostClose();
			}
		}
	);
}

void AsioSocket::ReadBody() {
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

					LOG(DEBUG) << "read_body()";
					ReadHeader();
				}
				else {
					LOG(DEBUG) << "read body error: " << e.message() << " (" << e.value() << ")";
					//std::cerr << "read body error: " << e.message().c_str() << "\n";
					HostClose();
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

	LOG(DEBUG) << "write_header()";

	auto self(shared_from_this());
	asio::async_write(m_socket,
		asio::buffer(&(m_sendQueue.front().offset), sizeof(Packet::offset)),

		[this, self](const std::error_code& e, size_t) {
			if (!e) {
				LOG(DEBUG) << "Wrote header";
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
				LOG(DEBUG) << "write header error: " << e.message() << " (" << e.value() << ")";
				//std::cerr << "write header error: " << e.message().c_str() << "\n";
				HostClose();
			}
		}
	);
}

void AsioSocket::WriteBody(Packet front) {

	LOG(DEBUG) << "write_body()";

	auto self(shared_from_this());
	asio::async_write(m_socket,
		asio::buffer(front.m_buf.data(), front.offset),
		[this, self](const std::error_code& e, size_t) {
		if (!e) {
			LOG(DEBUG) << "Wrote body\n";

			m_sendQueue.pop_front();
			if (!m_sendQueue.empty()) {
				WriteHeader();
			}
		}
		else {
			LOG(DEBUG) << "write body error: " << e.message() << " (" << e.value() << ")";
			//std::cerr << "write body error: " << e.message().c_str() << "\n";
			HostClose();
		}
	}
	);
}

void AsioSocket::CheckPong() {
	if (m_status != IOStatus::OPEN)
		return;

	// Check whether the deadline has passed. We compare the deadline against
	// the current time since a new asynchronous operation may have moved the
	// deadline before this actor had a chance to run.
	if (m_pongTimer.expiry() <= asio::steady_timer::clock_type::now())
	{
		LOG(DEBUG) << "Pong not received in time";

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
	if (m_status != IOStatus::OPEN)
		return;

	if (m_pingTimer.expiry() <= asio::steady_timer::clock_type::now()) {
		SendPing();
	}
}

void AsioSocket::SendPing() {
	Send({ PING });

	m_pongTimer.expires_after(PONG_TIMEOUT);
}