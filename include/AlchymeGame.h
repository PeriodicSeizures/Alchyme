#ifndef IGAME_H
#define IGAME_H

#include <thread>
#include <asio.hpp>
#include "AsyncDeque.hpp"
#include <semaphore>
#include "Task.h"
#include "Rpc.h"

using namespace asio::ip;

enum class ConnectMode : uint8_t {
	STATUS,
	LOGIN,
};

class AlchymeGame {
private:
	bool m_running = false;

	// perfect structure for this job
	// https://stackoverflow.com/questions/2209224/vector-vs-list-in-stl
	std::list<Task> m_tasks;

	std::counting_semaphore<3> m_taskBaton;

protected:
	static constexpr int MAGIC = 0xABCDEF69;
	static constexpr const char* VERSION = "1.0.0";

	std::thread m_ctxThread;
	asio::io_context m_ctx;

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