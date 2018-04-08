/**
 * File: debug-utils.h
 * ---------------------------
 * Debugging toolbox. You can pass in macro SUPPRESS_DEBUG to temporarily suppress the output.
 */

#ifndef DEBUG_UTILS_H_
#define DEBUG_UTILS_H_

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <iostream>

static inline void __WRITE_FORMATTED(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args); 
}

#ifndef SUPPRESS_DEBUG

#define PIN std::cout << "[DEBUG] PIN " << __FILE__ << ": " << __LINE__ << " <<" << std::endl;

#define LOG(CONTENT) std::cout << "[DEBUG] LOG " << __FILE__ << ": " << __LINE__
                     << " " << (CONTENT) << " <<" << std::endl;

#define PRINTF(FORMAT, ...) std::cout << "[DEBUG] PRINTF " << __FILE__ << ": " << __LINE__ << " "; \
                            __WRITE_FORMATTED(FORMAT, __VA_ARGS__); std::cout << " <<" << std::endl;

#define ASSERT(CONDITION) \
    if (CONDITION) {} \
    else {            \
        std::cout << "[DEBUG] PANIC: " << __FILE__ << ": " << __LINE__ << " assertion " \
		    << #CONDITION << " is false, abort. <<" << std::endl; abort(); \
    }

#define NOT_REACHED() std::cout << "[DEBUG] PANIC: " << __FILE__ << ": " << __LINE__ \
                                << " executed an unreachable statement, abort. <<" << std::endl; abort();

#endif

#ifdef SUPPRESS_DEBUG

#define PIN ;
#define LOG(STRING) ;
#define PRINTF(FORMAT, ...) ;
#define ASSERT(CONDITION) ;
#define NOT_REACHED() ;

#endif

#endif
