#ifndef ICLIENT_H
#define ICLIENT_H

#include "Socket.h"
#include <asio.hpp>

#include "NetPeer.h"

enum class ConnResult {
	OK,
	TIMEOUT,
	ABORT,
	NOT_FOUND,
	OTHER
};

class IClient {
	std::thread m_ctxThread;
	asio::io_context m_ctx;

	std::unique_ptr<Rpc> m_rpc;

	std::atomic_bool m_alive = false;

	double m_timeSinceStart = 0;

public:
	IClient();
	virtual ~IClient();

	/*
	* start(): blocks the current thread and invocates RPC functions
	*/
	//void StartListening();

	/*
	* stop(): disconnect and cleanup
	*/
	void Disconnect();

	/*
	* connect(...): async connect to a server
	*/
	void Connect(std::string host, std::string port);

	virtual void Update();

private:
	virtual void ConnectCallback(Rpc* rpc, ConnResult result) = 0;
	virtual void DisconnectCallback(Rpc* rpc) = 0;
};

#endif