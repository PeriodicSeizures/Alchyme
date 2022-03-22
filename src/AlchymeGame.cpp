#include "AlchymeGame.h"
#include "AlchymeClient.h"
#include "AlchymeServer.hpp"

static std::unique_ptr<AlchymeGame> impl;

AlchymeGame* AlchymeGame::Get() {
	return impl.get();
}

void AlchymeGame::RunClient() {
	impl = std::make_unique<AlchymeClient>();
	impl->Start();
}

void AlchymeGame::RunServer() {
	impl = std::make_unique<AlchymeServer>();
	impl->Start();
}

AlchymeGame::AlchymeGame(bool isServer)
	: m_taskBaton(1), m_isServer(m_isServer)
{}

void AlchymeGame::StartIOThread() {
	m_ctxThread = std::thread([this]() {
		el::Helpers::setThreadName("io");
		m_ctx.run();
	});

#ifdef _WIN32
	void* pThr = m_ctxThread.native_handle();
	SetThreadDescription(pThr, L"IO Thread");
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
		static auto last_tick = now; // Initialized to this once
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		last_tick = now;

		// to avoid blocking the main thread
		if (m_taskBaton.try_acquire()) {
			const auto now = std::chrono::steady_clock::now();
			for (auto itr = m_tasks.begin(); itr != m_tasks.end();) {


				if (itr->at < now) {
					itr->function();
					if (itr->repeats()) {
						itr->at += itr->period;
						++itr;
					}
					else
						itr = m_tasks.erase(itr);
				}
				else
					++itr;
			}
			m_taskBaton.release();
		}
		// Dispatch tasks
		//while (!m_tasks.empty()) {
		//
		//	const auto now = std::chrono::steady_clock::now();
		//	auto front = m_tasks.front(); // because tasks can repeat, this is not the best approach
		//	if (front.at < now) {
		//		front.function();
		//
		//		m_tasks.pop_front();
		//
		//		if (front.repeats()) {
		//			front.at += front.period;
		//			//bool was_empty = m_tasks.empty();
		//			m_tasks.push_back(std::move(front));
		//			//if (was_empty) // just bad, remove me sometime
		//			//	break;
		//		}
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

void AlchymeGame::Stop() {
	m_running = false;
}

/*
 * Async tasks would require
 * A thread pool of executors
*/

void AlchymeGame::RunTask(std::function<void()> task) {
	RunTaskLater(task, 0ms);
}

void AlchymeGame::RunTaskLater(std::function<void()> task, std::chrono::milliseconds after) {
	RunTaskLaterRepeat(task, after, 0ms);
}

void AlchymeGame::RunTaskAt(std::function<void()> task, std::chrono::steady_clock::time_point at) {
	RunTaskAtRepeat(task, at, 0ms);
}

void AlchymeGame::RunTaskLaterRepeat(std::function<void()> task, std::chrono::milliseconds after, std::chrono::milliseconds period) {
	RunTaskAtRepeat(task, std::chrono::steady_clock::now() + after, period);
}

void AlchymeGame::RunTaskAtRepeat(std::function<void()> task, std::chrono::steady_clock::time_point at, std::chrono::milliseconds period) {
	m_taskBaton.acquire();
	m_tasks.push_back({ task, at, period });
	m_taskBaton.release();
}
