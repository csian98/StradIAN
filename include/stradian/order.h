/**
 * @file		order.h
 * @brief		Order class for Exchange
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-15
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_ORDERH_
#define _HEADER_ORDERH_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <utility>

#include "stradian/exception.h"

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
	class Order {
	public:
		Order(unsigned, bool, double, int priority = 0);

		std::pair<unsigned, double> get_item(void) const;

		bool opeartor() const;
		
		bool operator<(const Order&) const;
		
	private:
		bool status;	// 0 for buy, 1 for sell
		
		unsigned code;

		double quantity;
		
		int priority = 0;	// high priority takes precedence
	};
}

/* Functions declaration */

/*
#ifdef __cplusplus
}
#endif
*/

#endif // OS dependency

/* Inline & Template Define Header */
//#include "sample.hpp"

#endif // Header duplicate
