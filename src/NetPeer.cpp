#include "NetPeer.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <memory>
#include "NetRpc.hpp"

namespace Alchyme {
	namespace Net {
		Peer::Peer(AsioSocket::Ptr socket)
			: m_isServer(Game::Get()->m_isServer),
			m_socket(socket),
			m_rpc(std::make_unique<Rpc>(socket))
		{}

		void Peer::Update() {
			m_rpc->Update(this);
		}

		/// TODO Move this to AlchymeServer
		void Peer::Kick(std::string reason) {
			assert(m_isServer);

			if (m_authorized) {
				m_rpc->Invoke("KickNotify", reason);
				DisconnectLater();
			}
		}

		void Peer::Disconnect() {
			m_socket->Close();
		}

		void Peer::DisconnectLater() {
			Game::Get()->RunTaskLater([this]() {
				Disconnect();
				//m_socket->Close();
			}, 1s);
		}

		bool Peer::IsOnline() {
			return m_socket->IsOnline();
		}
	}
}
