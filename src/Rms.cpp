#include "Rms.h"

Rms::Rms(std::shared_ptr<AsioSocket> socket) :
	m_socket(socket) {

}

void Rms::SyncFrom(Packet& packet) {

}
