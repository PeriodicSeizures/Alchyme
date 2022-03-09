#ifndef IGAME_H
#define IGAME_H

#include <thread>
#include <asio.hpp>
#include "AsyncDeque.hpp"
#include "Task.h"
#include "Rpc.h"
#include "IOResult.h"

using namespace asio::ip;

class World;

class AlchymeGame {
protected:
	std::thread m_ctxThread;
	asio::io_context m_ctx;

	bool m_running = false;

	AsyncDeque<Task> m_tasks;

	//std::unique_ptr<World> world;

public:
	AlchymeGame();
	//virtual ~AlchymeGame();

	virtual void Start();
	virtual void Stop();

	void StartIOThread();
	void StopIOThread();

	void RunTask(std::function<void()> event);
	void RunTaskLater(std::function<void()> event, std::chrono::steady_clock::time_point at);

private:
	virtual void PreUpdate(float delta) = 0;
	virtual void Update(float delta) = 0;
	virtual void ConnectCallback(Rpc* rpc) = 0;
	virtual void DisconnectCallback(Rpc* rpc) = 0;
};

#endif