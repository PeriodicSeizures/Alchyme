#include "AlchymeGame.h"

AlchymeGame::AlchymeGame() {

}

void AlchymeGame::Start() {
	while (m_running) {
		auto now = std::chrono::steady_clock::now();
		static auto last_tick = now;
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		last_tick = now;

		// Dispatch tasks
		while (!m_taskQueue.empty()) {
			const auto now = std::chrono::steady_clock::now();
			if (m_taskQueue.front().at < now)
				m_taskQueue.pop_front().function();
		}

		// Network IO RPC operations
		//	The difference between server and client here
		//	is that client has only 1 RPC,
		//	server has potentially many
		//	---
		//	So maybe add a injection here to call to a implemetation
		//	dependent RPC handler, either for client or server
		PreUpdate(elapsed / 1000000.f);
		//for (auto&& it = m_rpcs.begin(); it != m_rpcs.end();) {
		//	if ((*it)->m_socket->WasDisconnected()) {
		//		DisconnectCallback(it->get());
		//		it = m_rpcs.erase(it);
		//	}
		//	else {
		//		(*it)->Update();
		//		++it;
		//	}
		//}

		// UPDATE
		Update(elapsed / 1000000.f);

		// could instead create a performance analyzer that will
		// not delay when tps is low
		// Removing this will make cpu usage go to a lot more
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void AlchymeGame::RunTask(std::function<void()> task) {
	const auto now = std::chrono::steady_clock::now();
	m_taskQueue.push_back({ now, std::move(task) });
}

void AlchymeGame::RunTaskLater(std::function<void()> task, std::chrono::steady_clock::time_point at) {
	m_taskQueue.push_back({ at, std::move(task) });
}


