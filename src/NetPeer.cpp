#include "NetPeer.h"
#include "AlchymeGame.h"

NetPeer::NetPeer(std::shared_ptr<AsioSocket> socket)
	//: m_rpc(std::make_unique<Rpc>(socket))
	{}

void NetPeer::Update() {
	if (m_rpc && m_rpc->m_socket) {
		int max = 100;
		while (m_rpc->m_socket->GotNewData() && max-- >= 0) {
			auto packet = std::shared_ptr<Packet>(m_rpc->m_socket->Next());

			//int size = packet.GetSize();
			uint8_t type = 0;
			packet->Read(type);

			// Rpc
			if (type == 0) {
				m_rpc->Process(packet.get());
			}
		}

		if (m_rpc->m_socket->Status() == IOStatus::CLOSED) {
			//AlchymeGame::DisconnectCallback(m_rpc.get());
			// will now merge the two projects to allow for core functionality
			// to bridge between both client and server
			//rpc.reset();
		}
	}
}
