#ifndef IORESULT_H
#define IORESULT_H

enum class IOResult {
	OK,			// no error
	TIMEOUT,	// timedout waiting for PONG
	ABORT,		// handlers aborted
	NOT_FOUND,	// server not found
	OTHER		// not found
};

#endif