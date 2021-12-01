#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <easylogging++.h>

using namespace std::chrono_literals;

size_t constexpr StrHash(char const* input) {
	return *input ? static_cast<size_t>(*input) + 33 * StrHash(input + 1) : 5381;
}

#endif
