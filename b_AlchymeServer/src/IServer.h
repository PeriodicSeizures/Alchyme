#pragma once
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <fstream>
#include "Rpc.h"

using namespace asio::ip;

class IServer {
	struct Task {
		std::chrono::steady_clock::time_point at;
		std::function<void()> function;
	};

	// Could also store as a map<string ip, Rpc>
	std::vector<std::unique_ptr<Rpc>> m_rpcs;

	// Asio stuff
	std::thread m_ctxThread; // run ctx async
	asio::io_context m_ctx;
	std::unique_ptr<tcp::acceptor> m_acceptor;
	
	bool m_running = true;

	AsyncDeque<Task> m_taskQueue;

public:
	virtual ~IServer();

	/**
	 * @brief Run(): start the server loop
	*/
	virtual void Run();

	/**
	 * @brief Stop(): Stop the server loop and packet listener
	*/
	virtual void Stop();

	/**
	 * @brief Disconnect(...): Disconnect a specific connected Rpc client
	 * @param rpc 
	*/
	void Disconnect(Rpc *rpc);

	/**
	 * @brief Execute a task later. Is thread safe
	 * @param event
	*/
	void RunTask(std::function<void()> event);
	void RunTaskLater(std::function<void()> event, std::chrono::steady_clock::time_point at);

private:
	/*
	 * To override
	 */
	virtual void Update(float dt) = 0;
	virtual void ConnectCallback(Rpc* rpc) = 0;
	virtual void DisconnectCallback(Rpc* rpc) = 0;

	void DoAccept();

protected:
	void StartAccepting(uint_least16_t port);
};

#endif
