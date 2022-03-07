#ifndef IGAME_H
#define IGAME_H

#include "Task.h"

class AlchymeGame {
	std::thread m_ctxThread;
	asio::io_context m_ctx;

	bool m_running = false;

	AsyncDeque<Task> m_taskQueue;

public:
	AlchymeGame();
	virtual ~AlchymeGame();

	virtual void Start();
	virtual void Stop();

	void RunTask(std::function<void()> event);
	void RunTaskLater(std::function<void()> event, std::chrono::steady_clock::time_point at);

private:
	virtual void Update(float delta) = 0;
	virtual void PreUpdate(float delta) = 0;
	virtual void ConnectCallback(Rpc* rpc, ConnResult result) = 0;
	virtual void DisconnectCallback(Rpc* rpc) = 0;
};

#endif