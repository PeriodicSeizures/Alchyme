#include "NetPeer.h"

NetPeer::NetPeer(std::shared_ptr<AsioSocket> socket)
	: m_socket(socket), m_rpc(socket) {}
