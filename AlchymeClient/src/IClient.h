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
	struct Task {
		std::chrono::steady_clock::time_point at;
		std::function<void()> function;
	};

	std::thread m_ctxThread;
	asio::io_context m_ctx;

	std::unique_ptr<Rpc> m_rpc;

	bool m_running = false;

	AsyncDeque<Task> m_taskQueue;

public:
	IClient();
	virtual ~IClient();

	/*
	* start(): blocks the current thread and invocates RPC functions
	*/
	virtual void Run();

	virtual void Stop();

	/*
	* stop(): disconnect and cleanup
	*/
	void Disconnect();

	/*
	* connect(...): async connect to a server
	*/
	void Connect(std::string host, std::string port);

	Rpc* GetRpc();

	void RunTask(std::function<void()> event);
	void RunTaskLater(std::function<void()> event, std::chrono::steady_clock::time_point at);

private:
	virtual void Update(float delta) = 0;
	virtual void ConnectCallback(Rpc* rpc, ConnResult result) = 0;
	virtual void DisconnectCallback(Rpc* rpc) = 0;
};

#endif
