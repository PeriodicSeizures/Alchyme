#pragma once
#include "NetPeer.h"
#include <string>

class Net
{
	// list of clients
	//std::vector< std::shared_ptr<AsioSocket>> m_
	//std::shared_ptr<AsioSocket> m_socket;
	std::vector<NetPeer> m_peers;

	// will contain a 


public:
	Net();

	NetPeer* GetPeer(size_t uid);
	NetPeer* GetPeer(std::string name);
	NetPeer* GetPeer(Rpc *rpc);

};

