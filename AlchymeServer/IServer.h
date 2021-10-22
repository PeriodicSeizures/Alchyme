#pragma once
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

// https://stackoverflow.com/questions/11821723/keeping-code-in-a-client-server-game-organized

#include "Rpc.h"

using namespace asio::ip;

class IServer {
	// Could also store as a map<string ip, Rpc>
	std::vector<std::unique_ptr<Rpc>> m_rpcs;

	// Asio stuff
	std::thread m_ctxThread; // run ctx async
	asio::io_context m_ctx;
	tcp::acceptor m_acceptor;

	// Whether server is open
	std::atomic_bool m_alive = false;

	double m_timeSinceStart;

public:
	IServer(unsigned short port);
	virtual ~IServer();

	/*
	* Start(): a blocking call to begin the server and start
	* accepting incoming connections and listening
	*/
	void Start();

	/*
	* Stop(): Will stop and block until io thread is killed
	* Will automatically restart for later reuse
	*/
	void Stop();

	/*
	* IsAlive(): whether the server is open and
	* accepting connections
	*/
	bool IsAlive();

	/*
	* Disconnect(...): sever a connection
	*/
	void Disconnect(Rpc *rpc); // , bool doCloseAfterSends = false

	double getTimeSinceStart();

private:
	/*
	 * To override
	 */
	virtual void Update(float dt) = 0;
	virtual void ConnectCallback(Rpc* rpc) = 0;
	virtual void DisconnectCallback(Rpc* rpc) = 0;

	// Run acceptor
	void DoAccept();
};

#endif
