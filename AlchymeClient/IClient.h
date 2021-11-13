#pragma once
#include "Socket.h"

//#include <asi>
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
	~IClient();

	/*
	* start(): blocks the current thread and invocates RPC functions
	*/
	void Start();

	/*
	* stop(): disconnect and cleanup
	*/
	void Stop();

	/*
	* connect(...): async connect to a server
	*/
	void Connect(std::string host, std::string port);

private:
	virtual void Update(float dt) = 0;
	virtual void ConnectCallback(Rpc* rpc, ConnResult result) = 0;
	virtual void DisconnectCallback(Rpc* rpc) = 0;
};