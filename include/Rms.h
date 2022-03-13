#pragma once

#include <memory>
#include "Socket.h"

class Rms {
	std::shared_ptr<AsioSocket> m_socket;

public:
	Rms(std::shared_ptr<AsioSocket> socket);

	void SyncFrom(Packet &packet);

};