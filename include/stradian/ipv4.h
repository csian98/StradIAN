/**
 * @file		ipv4.h
 * @brief		ipv4 for c2cServer and c2cClient
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-07-15
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_IPV4_H_
#define _HEADER_IPV4_H_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <string>
#include <filesystem>

#if __has_include(<iostream>)
#include <iostream>
#endif

#if _TARGET_OS == OS_WINDOWS
// Windows header
#include <Windows.h>

#elif _TARGET_OS == OS_LINUX
// Linux header
#include <sys/types.h>
#include <unistd.h>

/* C & CPP */
/*
#ifdef __cplusplus
extern "C" {
#endif
*/

/* defines typedef & constant */

/* MACRO functions */


/* Inline define */

namespace stradian {
	namespace c2c {
		const std::string ipv4 = "127.0.0.1";

		const int port = 9083;
	}
}

/* Attributes */
/*
[[noreturn]]
[[deprecated]]
[[fallthrough]]
[[nodiscard]]
[[maybe_unused]]
*/

/* Declaration */

/* ASM codes */
// extern "C" int func(int x, int y);

/* Data structures declaration - struct & class */

/*

class Sample {
	friend void swap(Sample&, Sample&) noexcept;

public:
	Sample(void) = default;
	
	Sample(std::initializer_list<int>);

	virtual ~Sample(void) noexcept = default;

	Sample(const Sample&);

	Sample& operator=(const Sample&);

	Sample(Sample&&) noexcept;

	Sample& operator=(Sample&&) noexcept;
protected:
	
private:
	
};

*/

/* Functions declaration */

// void swap(Sample&, Sample&) noexcept;

/*
#ifdef __cplusplus
}
#endif
*/

#endif // OS dependency

/* Inline & Template Define Header */
//#include "sample.hpp"

#endif // Header duplicate
