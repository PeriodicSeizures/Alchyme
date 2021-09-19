#include "Socket.h"
#include <string>
#include <iostream>

//auto static constexpr UINT_MAX = std::numeric_limits<int>().max();

#define PING (INT_MAX)
#define PONG (PING-1)

AsioSocket::AsioSocket(asio::io_context& ctx,
	asio::ip::tcp::socket socket)
	: m_socket(std::move(socket)),
	m_ping_timer(ctx),
	m_pong_timer(ctx),
	m_connected(false)
{
	ReadHeader();
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
	return std::move(m_recvQueue.front());
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

		m_pong_timer.cancel();
	}
}

void AsioSocket::GetConnectionQuality(float& localQuality, float& remoteQuality, int& ping, float& outByteSec, float& inByteSec) {
	// do nothing
	ping = this->m_ping;
}

void AsioSocket::Accept() {

	m_connected = true;

	// begin reader
	ReadHeader();

	/*
	* Send ping
	*/
	Send({ std::vector<char>(), PING });

	m_pong_timer.expires_after(std::chrono::seconds(10));
	m_pong_timer.async_wait(std::bind(&AsioSocket::CheckPong, this));
}

void AsioSocket::ReadHeader() {
	if (!m_connected)
		return;

	auto self(shared_from_this());
	asio::async_read(m_socket,
		asio::buffer(&temp_packet.offset, sizeof(Packet::offset)),
		[this, self](const std::error_code& e, size_t) {
			if (!e) {
				//LOG_DEBUG("read_header()\n");

				// Offset is size, with some special macro behaviour
				switch (temp_packet.offset) {
					
				case PING: { // if PING received, reply with PONG

					std::cout << "Ponging!\n";

					Send({ std::vector<char>(), PONG });
					ReadHeader();
					break;
				} case PONG: { // if PONG received, record timings

					std::cout << "Recording timings!\n";

					auto now = std::chrono::steady_clock::now();
					m_ping = static_cast<uint16_t>(
						std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_ping).count());

					m_ping_timer.expires_after(std::chrono::seconds(5));
					m_ping_timer.async_wait(std::bind(&AsioSocket::CheckPing, this));

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
	if (!m_connected)
		return;

	// The packet.offset will serve as the size
	temp_packet.m_buf.reserve(temp_packet.offset);

	auto self(shared_from_this());
	asio::async_read(m_socket,
		asio::buffer(temp_packet.m_buf),
		[this, self](const std::error_code& e, size_t) {
			if (!e) {
				m_recvQueue.push_back(temp_packet);

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

void AsioSocket::WriteHeader() {
	if (!m_connected)
		return;

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
				//LOG_DEBUG("write header error: %s\n", e.message().c_str());
				Close();
			}
		}
	);
}

void AsioSocket::WriteBody() {
	if (!m_connected)
		return;

	auto self(shared_from_this());
	asio::async_write(m_socket,
		asio::buffer(m_sendQueue.pop_front().m_buf),
		[this, self](const std::error_code& e, size_t) {
			if (!e) {
				//LOG_DEBUG("write_body()\n");

				//out_packets.pop_front();

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

void AsioSocket::CheckPong() {
	if (!m_connected)
		return;

	// Check whether the deadline has passed. We compare the deadline against
	// the current time since a new asynchronous operation may have moved the
	// deadline before this actor had a chance to run.
	if (m_pong_timer.expiry() <= asio::steady_timer::clock_type::now())
	{
		//LOG_DEBUG("pong timeout\n");

		// The deadline has passed. The socket is closed so that any outstanding
		// asynchronous operations are cancelled.
		m_socket.close();

		// There is no longer an active deadline. The expiry is set to the
		// maximum time point so that the actor takes no action until a new
		// deadline is set.
		m_pong_timer.expires_at(asio::steady_timer::time_point::max());
	}

	// Put the actor back to sleep.
	m_pong_timer.async_wait(std::bind(&AsioSocket::CheckPong, this));

}

void AsioSocket::CheckPing() {
	if (!m_connected)
		return;

	if (m_ping_timer.expiry() <= asio::steady_timer::clock_type::now()) {

		Send({ std::vector<char>(), PING });

		m_pong_timer.expires_after(std::chrono::seconds(10));
	}
}
