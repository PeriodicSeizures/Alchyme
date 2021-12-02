#pragma once
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

// https://stackoverflow.com/questions/11821723/keeping-code-in-a-client-server-game-organized

#include <Rpc.h>

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
	
	// Whether server is open
	bool m_running = true;

	AsyncDeque<Task> m_taskQueue;

public:
	IServer();
	virtual ~IServer();

	/*
	* StartListening(): a blocking call to begin the server and start
	* accepting incoming connections and listening
	*/
	virtual void Run();

	/*
	* Disconnect(): Will stop and block until io thread is killed
	* Will automatically restart for later reuse
	*/
	virtual void Stop();

	/*
	* Disconnect(...): sever a connection
	*/
	void Disconnect(Rpc *rpc);

	void RunTask(std::function<void()> event);
	void RunTaskLater(std::function<void()> event, std::chrono::steady_clock::time_point at);


private:
	/*
	 * To override
	 */
	virtual void Update(float dt) = 0;
	virtual void ConnectCallback(Rpc* rpc) = 0;
	virtual void DisconnectCallback(Rpc* rpc) = 0;

	// Run acceptor
	void DoAccept();

protected:
	void StartAccepting(uint_least16_t port);
};

#endif
