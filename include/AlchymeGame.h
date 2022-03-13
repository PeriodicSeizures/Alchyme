#ifndef IGAME_H
#define IGAME_H

#include <thread>
#include <asio.hpp>
#include "AsyncDeque.hpp"
#include "Task.h"
#include "Rpc.h"

using namespace asio::ip;

class AlchymeGame {
protected:
	static constexpr int MAGIC = 0xABCDEF69;

	std::thread m_ctxThread;
	asio::io_context m_ctx;

	bool m_running = false;

	AsyncDeque<Task> m_tasks;

public:
	const bool m_isServer;

public:
	static AlchymeGame* Get();
	static void RunClient();
	static void RunServer();

	AlchymeGame(bool isServer);
	//virtual ~AlchymeGame();

	virtual void Start();
	virtual void Stop();

	void StartIOThread();
	void StopIOThread();

	void RunTask(std::function<void()> task);
	void RunTaskLater(std::function<void()> task, std::chrono::milliseconds after);
	void RunTaskAt(std::function<void()> task, std::chrono::steady_clock::time_point at);
	void RunTaskLaterRepeat(std::function<void()> task, std::chrono::milliseconds after, std::chrono::milliseconds period);
	void RunTaskAtRepeat(std::function<void()> task, std::chrono::steady_clock::time_point at, std::chrono::milliseconds period);

private:
	virtual void Update(float delta) = 0;
	virtual void ConnectCallback(Rpc* rpc) = 0;
	virtual void DisconnectCallback(Rpc* rpc) = 0;
};

#endif