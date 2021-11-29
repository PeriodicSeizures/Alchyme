#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include "Packet.h"

#include <asio.hpp>
#include "AsyncQueue.hpp"
#include <memory>

//enum class DisconnectResult {
//	SEVER,
//	QUIT,
//	TIMEOUT,
//	SHUTDOWN // similar to sever, but sever is targeted intentional
//};

class ISocket {
public:
	//virtual ~ISocket() = 0;

	virtual bool IsConnected() = 0;			// Whether socket is connected to client/server
	virtual bool WasDisconnected() = 0;
	virtual void Send(Packet pkg) = 0;		// Send a packet
	virtual Packet Recv() = 0;				// Returns the next packet; implementation dependent
	virtual int GetSendQueueSize() = 0;		// Get how many packets are queued for sending
	//virtual int GetCurrentSendRate() = 0;	//
	//virtual bool IsHost() = 0;
	virtual bool GotNewData() = 0;
	virtual void Close() = 0;
	//virtual std::string GetEndPointString() = 0;
	//virtual void GetAndResetStats(int &totalSent, int &totalRecv) = 0;
	/*
	* localQuality:		Steam UDP measured			: hard to record with tcp
	* remoteQuality:	Steam UDP measured			: hard to record with tcp
	* ping:				Response times				: possible
	* outByteSec:		Steam UDP measured			: hard to record with tcp
	* inByteSec			Steam UDP measured			: hard to record with tcp
	*/
	//virtual void GetConnectionQuality(float &localQuality, float &remoteQuality, int &ping, float &outByteSec, float &inByteSec) = 0;
	virtual int GetPing() = 0;
	virtual void Accept() = 0;
	virtual uint_least16_t GetHostPort() = 0;
	//virtual bool Flush() = 0; // Send all data and clear packet queue
	virtual std::string GetHostName() = 0;
	//virtual DisconnectResult GetDisconnectResult() = 0;
};

/**
 * @brief Custom socket implementation which makes use of Asio tcp socket
 * 
*/
class AsioSocket : ISocket, public std::enable_shared_from_this<AsioSocket> {
	//asio::io_context _io_context;
	//tcp::acceptor _acceptor;

	std::string address;
	uint_least16_t port;

	asio::ip::tcp::socket m_socket;
	AsyncDeque<Packet> m_sendQueue;
	AsyncDeque<Packet> m_recvQueue;

	std::atomic<bool> m_connected = false;
	std::atomic<bool> m_wasDisconnected = false;
	//std::atomic<bool> doCloseAfterSends = false;

	Packet m_inPacket;
	//int temp_size;

	asio::steady_timer m_pingTimer;
	asio::steady_timer m_pongTimer;
	std::atomic<int> m_ping;
	std::chrono::steady_clock::time_point m_last_ping;

	//DisconnectResult disconnectResult;

public:
	typedef std::shared_ptr<AsioSocket> ptr;

	AsioSocket(asio::io_context& ctx, asio::ip::tcp::socket socket);
	AsioSocket(asio::io_context& ctx);
	~AsioSocket();

	bool IsConnected() override;
	bool WasDisconnected() override;
	void Send(Packet pkg) override;
	Packet Recv() override;
	int GetSendQueueSize() override;
	//int GetCurrentSendRate() override;
	//bool IsHost() override;
	bool GotNewData() override;
	void Close() override;
	//void CloseAfterNextSends();
	//std::string GetEndPointString() override;
	//void GetAndResetStats(int& totalSent, int& totalRecv) override;
	//void GetConnectionQuality(float& localQuality, float& remoteQuality, int& ping, float& outByteSec, float& inByteSec) override;
	int GetPing() override;
	//void Accept() override;
	uint_least16_t GetHostPort() override;
	//bool Flush() override; // Send all data and clear packet queue
	std::string GetHostName() override;

	//void ConnectToHost(asio::io_context& ctx, std::string host, std::string port);

	void Accept() override;

	//DisconnectResult GetDisconnectResult() override;

	asio::ip::tcp::socket &GetSocket();

private:
	void CheckPing();
	void CheckPong();

	void ReadHeader();
	void ReadBody();
	void WriteHeader();
	void WriteBody(Packet front);
};

#endif
