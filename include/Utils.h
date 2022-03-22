#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <easylogging++.h>

using namespace std::chrono_literals;
namespace Utils {

	size_t constexpr StrHash(char const* input) {
		return *input ? static_cast<size_t>(*input) + 33 * StrHash(input + 1) : 5381;
	}

	void initLogger();

	std::vector<std::string_view> split(std::string_view s, std::string_view delimiter);

}

#endif
