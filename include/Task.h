#ifndef TASK_H
#define TASK_H

struct Task {
	std::function<void()> function;
	std::chrono::steady_clock::time_point at;

	// a period of 0 will denote no repeat
	std::chrono::milliseconds period;
};

#endif