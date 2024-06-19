/**
 * @file		sample.h
 * @brief		
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-04-03
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_SAMPLEH_
#define _HEADER_SAMPLEH_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <utility>
#include <memory>
#include <thread>

#include <filesystem>

#include <string>
#include <string_view>
#include <algorithm>
#include <numeric>
#include <execution>

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
#ifndef SWAP
template <typename T>
inline void SWAP(T& a, T& b) {
	T tmp = std::move(a);
	a = std::move(b);
	b = std::move(tmp);
}
#endif

#ifndef MIN
#define MIN(a, b)	(a > b ? b : a)
#endif

#ifndef MAX
#define MAX(a, b)	(a > b ? a : b)
#endif

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
