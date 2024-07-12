/**
 * @file		extractable.h
 * @brief		Extractable class for WebSocket
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-21
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
// #pragma comment(lib, "libpthread.so")

#ifndef _HEADER_EXTRACTABLE_H_
#define _HEADER_EXTRACTABLE_H_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <boost/json.hpp>

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

namespace stradian {
	template <typename T>
	class Extractable {
	public:
		virtual void extract(const T&) = 0;
	};
}

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
