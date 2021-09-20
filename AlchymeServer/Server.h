#pragma once
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

// https://stackoverflow.com/questions/11821723/keeping-code-in-a-client-server-game-organized

#include <unordered_set>
#include "Socket.h"
#include "NetPeer.h"

using namespace asio::ip;

class Server {
	std::vector<NetPeer> m_peers;

	// Asio stuff
	std::thread m_ctxThread; // run ctx async
	asio::io_context m_ctx;
	tcp::acceptor m_acceptor;

	// Whether server is open
	std::atomic_bool m_alive = false;

	double m_elapsedTime;

public:
	Server(unsigned short port);
	~Server();

	/*
	* start(): a blocking call to begin the server and start
	* accepting incoming connections and listening
	*/
	void Start();

	/*
	* stop(): Will stop and block until io thread is killed
	* Will automatically restart for later reuse
	*/
	void Stop();

	/*
	* is_alive(): whether the server is open and
	* accepting connections
	*/
	bool IsAlive();

	/*
	* disconnect(...): sever and cleanup a connection
	*/
	//void disconnect(TCPConnection::ptr connection);

private:
	/*
	* Server logic methods
	*/
	void Update(double dt);

	// Run acceptor
	void DoAccept();

	/*
	* Client based events / methods
	*/
	// how is virtual allowed with final class
	bool OnJoin(AsioSocket::ptr);
	void OnQuit(AsioSocket::ptr);

};

#endif
