#pragma once

#ifdef _DEBUG
#define LOG_DEBUG printf
#else
#define LOG_DEBUG //
#endif

size_t constexpr StrHash(char const* input) {
	return *input ? static_cast<size_t>(*input) + 33 * StrHash(input + 1) : 5381;
}