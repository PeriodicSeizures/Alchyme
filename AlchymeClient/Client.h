#pragma once
#include "Socket.h"

//#include <asi>
#include "NetPeer.h"

class Client {
	std::thread m_ctxThread;
	asio::io_context m_ctx;

	NetPeer m_peer;

	std::atomic_bool m_alive = false;

public:
	Client();
	~Client();

	/*
	* start(): a blocking call to begin the client connect to
	* server and listen, assuming connect was successful
	*/
	void Start();

	/*
	* stop(): a blocking call which terminates all active
	* server threads (run, update, render)
	*/
	void Stop();

	/*
	* connect(...): a non blocking call to connect to a target
	* ssl encrypted server
	*/
	void Connect(std::string host, std::string port);

	// returns latency in ms
	uint16_t latency();

private:
	//void RPC_Print(Rpc* rpc, int blank);

	void ConnectCallback();

	void Update(float dt);
	// include delta for upmost accuracy, since tick speed cannot be entirely trusted
	// and depended upon, since lagg exists
	//virtual void Render() = 0;
};