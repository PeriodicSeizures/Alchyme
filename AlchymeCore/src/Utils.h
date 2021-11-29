#ifndef UTILS_H
#define UTILS_H

#include <chrono>

using namespace std::chrono_literals;

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

//#ifdef _DEBUG //printf
#define LOG_DEBUG(s) std::cout << YELLOW << "(DEBUG) " << s << RESET << "\n";
//#else
//#define LOG_DEBUG(s) //
//#endif

#define LOG_ERROR(s) std::cerr << RED << "(ERROR) " << s << RESET << "\n";

#define LOG_INFO(s) std::cout << WHITE << "(INFO) " << s << RESET << "\n";

size_t constexpr StrHash(char const* input) {
	return *input ? static_cast<size_t>(*input) + 33 * StrHash(input + 1) : 5381;
}

#endif
