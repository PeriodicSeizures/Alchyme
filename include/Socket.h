#pragma once

#include <string>
#include "Packet.h"

#include <asio.hpp>
#include "AsyncDeque.hpp"
#include <memory>

using namespace asio::ip;

enum class IOStatus : uint8_t {
	INITIALIZING,
	OPEN,
	CLOSED,
};

enum class IOClosure : uint8_t {
	NONE,
	LOCAL_CLOSE,
	REMOTE_CLOSE,
	QUIT,
	KICK,
	SUDDEN,
	TIMEOUT,
	ILLEGAL,
};

/**
 * @brief Custom socket implementation which makes use of Asio tcp socket
 * 
*/
class AsioSocket : public std::enable_shared_from_this<AsioSocket> {
	tcp::socket m_socket;
	AsyncDeque<Packet*> m_sendQueue;
	AsyncDeque<Packet*> m_recvQueue;
	
	std::string address;
	uint_least16_t port;

	std::atomic<IOStatus> m_status = IOStatus::INITIALIZING;
	std::atomic<IOClosure> m_closure = IOClosure::NONE;

	//Packet m_inPacket;

	int not_garbage = 5;

public:
	typedef std::shared_ptr<AsioSocket> Ptr;

	AsioSocket(asio::io_context& ctx, tcp::socket socket);
	AsioSocket(asio::io_context& ctx);
	~AsioSocket();

	IOStatus Status();

	void Send(Packet *packet);
	Packet *Next();
	int GetSendQueueSize();
	//int GetCurrentSendRate() override;
	//bool IsHost() override;
	bool GotNewData();
	void Close(IOClosure closure = IOClosure::LOCAL_CLOSE);
	//void Accept() override;
	uint_least16_t GetHostPort();
	//bool Flush() override; // Send all data and clear packet queue
	std::string GetHostName();

	void Accept();

	tcp::socket& Socket() {
		return m_socket;
	}

private:
	void ReadHeader();
	void ReadBody(Packet* packet);
	void WriteHeader();
	void WriteBody(Packet *packet);

	void CleanUpClose() {
		Close(IOClosure::LOCAL_CLOSE);
	}
};
