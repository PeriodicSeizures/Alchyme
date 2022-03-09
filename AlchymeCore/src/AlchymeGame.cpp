#include "AlchymeGame.h"
#include "World.h"

AlchymeGame::AlchymeGame() {}

void AlchymeGame::StartIOThread() {
	m_ctxThread = std::thread([this]() {
		el::Helpers::setThreadName("io");
		m_ctx.run();
	});

#ifdef _WIN32
	void* pThr = m_ctxThread.native_handle();
	SetThreadDescription(pThr, L"IOThread");
#endif
}

void AlchymeGame::StopIOThread() {
	m_ctx.stop();

	if (m_ctxThread.joinable())
		m_ctxThread.join();

	m_ctx.restart();
}

void AlchymeGame::Start() {
	m_running = true;

	while (m_running) {
		auto now = std::chrono::steady_clock::now();
		static auto last_tick = now;
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		last_tick = now;

		// Dispatch tasks
		while (!m_tasks.empty()) {
			const auto now = std::chrono::steady_clock::now();
			if (m_tasks.front().at < now)
				m_tasks.pop_front().function();
		}

		PreUpdate(elapsed / 1000000.f);

		// UPDATE
		Update(elapsed / 1000000.f);

		// could instead create a performance analyzer that will
		// not delay when tps is low
		// Removing this will make cpu usage go to a lot more
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void AlchymeGame::Stop() {
	m_running = false;
	StopIOThread();
}

void AlchymeGame::RunTask(std::function<void()> task) {
	const auto now = std::chrono::steady_clock::now();
	m_tasks.push_back({ now, std::move(task) });
}

void AlchymeGame::RunTaskLater(std::function<void()> task, std::chrono::steady_clock::time_point at) {
	m_tasks.push_back({ at, std::move(task) });
}
