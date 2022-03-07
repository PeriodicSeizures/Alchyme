#ifndef TASK_H
#define TASK_H

struct Task {
	std::chrono::steady_clock::time_point at;
	std::function<void()> function;
};

#endif