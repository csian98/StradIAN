/**
 * @file		market.h
 * @brief		market virtual class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-15
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
// #pragma comment(lib, "libpthread.so")

#ifndef _HEADER_MARKET_H_
#define _HEADER_MARKET_H_

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
#include <string_view>

#include <memory>
#include <map>

#include "stradian/exchange.h"
#include "stradian/forkexecable.h"
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
	enum class MARKETCODE {
		STOCK,
		CRYPTO
	};
	
	class Market : public ForkExecable {
	public:
		Market(MARKETCODE);

		virtual ~Market(void) noexcept;

		virtual void fork_crawler(void) = 0;

		MARKETCODE get_code(void) const;

		std::pair<double, double> get_validation(void) const;

		std::map<std::string, std::pair<double, double>> get_items(void) const;

		std::vector<std::string> get_symbols(void) const;

    protected:
		std::unique_ptr<Exchange> exchange;

		const MARKETCODE code;
	};
}

/* Functions declaration */

namespace stradian {
	std::string to_string(MARKETCODE);
}

/*
#ifdef __cplusplus
}
#endif
*/

#endif // OS dependency

/* Inline & Template Define Header */
//#include "sample.hpp"

#endif // Header duplicate
