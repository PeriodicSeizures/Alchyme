#pragma once
#include "Socket.h"
#include "Rpc.h"

class NetPeer
{
public:
	std::shared_ptr<AsioSocket> m_socket;
	Rpc m_rpc;
	size_t m_uid;
	std::string name;

public:
	NetPeer(std::shared_ptr<AsioSocket> socket);
};

