#include "Socket.h"
#include <string>
#include <iostream>

#include "Utils.h"

AsioSocket::AsioSocket(asio::io_context& ctx,
	asio::ip::tcp::socket socket)
	: m_socket(std::move(socket)) {}

AsioSocket::AsioSocket(asio::io_context& ctx)
	: m_socket(ctx) {}

void AsioSocket::Accept() {
	LOG(DEBUG) << "AsioSocket::Accept()";
	m_status = IOStatus::OPEN;

	address = m_socket.remote_endpoint().address().to_string();
	port = m_socket.remote_endpoint().port();
	ReadHeader();
}

AsioSocket::~AsioSocket() {
	LOG(DEBUG) << "~AsioSocket()";
	Close();
}

IOStatus AsioSocket::Status() {
	return m_status;
}

void AsioSocket::Send(Packet *packet) {
	const bool was_empty = m_sendQueue.empty();
	m_sendQueue.push_back(packet);
	if (was_empty) {
		LOG(DEBUG) << "Reinitiating Writer";
		WriteHeader();
	}
}

Packet *AsioSocket::Next() {
	return m_recvQueue.pop_front();
}

int AsioSocket::GetSendQueueSize() {
	return static_cast<int>(m_sendQueue.count());
}

bool AsioSocket::GotNewData() {
	return !m_recvQueue.empty();
}

void AsioSocket::Close(IOClosure closure) {
	if (m_status == IOStatus::OPEN) {
		LOG(DEBUG) << "AsioSocket::Close()";

		m_status = IOStatus::CLOSED;
		m_closure = closure;

		m_socket.close();
	}
}

std::string AsioSocket::GetHostName() {
	return this->address;
}

uint_least16_t AsioSocket::GetHostPort() {
	return port;
}

void AsioSocket::ReadHeader() {
	LOG(DEBUG) << "ReadHeader()";

	Packet* temp = new Packet();

	auto self(shared_from_this());
	asio::async_read(m_socket,
		asio::buffer(&temp->offset, sizeof(Packet::offset)),
		[this, self, temp](const std::error_code& e, size_t) {
			if (!e) {
				ReadBody(temp);
			}
			else {
				LOG(DEBUG) << "read header error: " << e.message() << " (" << e.value() << ")";
				CleanUpClose();
			}
		}
	);
}

void AsioSocket::ReadBody(Packet* packet) {
	LOG(DEBUG) << "ReadBody()";

	if (packet->offset == 0) {
		Close(IOClosure::ILLEGAL);
	}
	else {
		packet->m_buf.resize(packet->offset);
		auto self(shared_from_this());
		asio::async_read(m_socket,
			asio::buffer(packet->m_buf),
			[this, self, packet](const std::error_code& e, size_t) {
			if (!e) {
				packet->offset = 0;
				m_recvQueue.push_back(packet);
				ReadHeader();
			}
			else {
				LOG(DEBUG) << "read body error: " << e.message() << " (" << e.value() << ")";
				CleanUpClose();
			}
		});
	}
}

void AsioSocket::WriteHeader() {
	LOG(DEBUG) << "WriteHeader()";

	assert(m_sendQueue.front()->offset > 0 && "Offset must be greater than 0");

	Packet *packet = m_sendQueue.front();

	auto self(shared_from_this());
	asio::async_write(m_socket,
		asio::buffer(&packet->offset, sizeof(Packet::offset)),
		[this, self, packet](const std::error_code& e, size_t) {
		if (!e) {
			WriteBody(packet);
		}
		else {
			LOG(DEBUG) << "write header error: " << e.message() << " (" << e.value() << ")";
			CleanUpClose();
		}
	});
}

void AsioSocket::WriteBody(Packet *packet) {
	LOG(DEBUG) << "WriteBody()";

	auto self(shared_from_this());
	asio::async_write(m_socket,
		asio::buffer(packet->m_buf.data(), packet->offset),
		[this, self, packet](const std::error_code& e, size_t) {
		delete packet;
		if (!e) {
			m_sendQueue.pop_front();
			if (!m_sendQueue.empty()) {
				WriteHeader();
			}
		}
		else {
			LOG(DEBUG) << "write body error: " << e.message() << " (" << e.value() << ")";
			CleanUpClose();
		}
	});
}
