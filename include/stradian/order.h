/**
 * @file		order.h
 * @brief		Order class for Exchange
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-15
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"//  
// #pragma comment(lib, "libpthread.so")

#ifndef _HEADER_ORDER_H_
#define _HEADER_ORDER_H_

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
#include <string>
#include <string_view>
#include <sstream>

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
	enum class ORDERCODE {
		BUYSELL,
	};
	
	/*
	 *  #BUYSELL
	 *  	MARKET ORDER ( price < 0.0 )
	 *		- bool is_buy
	 *		- double quantity
	 *		
	 *		LIMIT ORDER ( price > 0.0 )
	 *		- bool is_buy
	 *		- double price
	 *		- double quantity
	 */
	
	struct Order {
		Order(ORDERCODE, unsigned int priority = 0);

		void market_order(const std::string&,
						  bool, double);

		void limit_order(const std::string&,
						 bool, double, double);
		
		ORDERCODE ordercode;

		unsigned int priority;
		
		std::string symbol;

		// STATUS

		// BUYSELL
		bool is_buy = false;
		
		double quantity = 0.0;
		
		double price = -1.0;

		bool operator<(const Order&) const;

		template <typename T>
		static std::string to_string_with_precision(const T, const int n = 8);
	};
}

/* Functions declaration */

namespace stradian {
	template <typename T>
	std::string to_string_with_precision(const T, const int n = 8);
}

/*
#ifdef __cplusplus
}
#endif
*/

#endif // OS dependency

/* Inline & Template Define Header */
#include "stradian/order.hpp"

#endif // Header duplicate
