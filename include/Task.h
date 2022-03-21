#ifndef TASK_H
#define TASK_H

struct Task {
	const std::function<void()> function;
	std::chrono::steady_clock::time_point at;

	// a period of 0 will denote no repeat
	const std::chrono::milliseconds period;

	bool repeats() {
		using namespace std::chrono_literals;
		return period > 0ms;
	}
};

#endif