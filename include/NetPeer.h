#ifndef NETPEER_H
#define NETPEER_H

#include "Socket.h"
#include "Rpc.h"

class NetPeer {
	//std::shared_ptr<AsioSocket> m_socket;

	std::unique_ptr<Rpc> m_rpc;

	size_t m_uid;
	std::string m_key;
	std::string name;
	bool authorized = false;

public:
	NetPeer(std::shared_ptr<AsioSocket> socket);

	void Update();

	Rpc* Rpc() {
		return m_rpc.get();
	}
};

#endif
