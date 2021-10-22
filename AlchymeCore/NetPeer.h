#pragma once
#include "Socket.h"
#include "Rpc.h"

struct NetPeer {
	//std::shared_ptr<AsioSocket> m_socket;
	Rpc *m_rpc;
	size_t m_uid;
	std::string m_key;
	std::string name;
	bool authorized = false;
//public:
	NetPeer();
	NetPeer(Rpc* rpc);
	

};
