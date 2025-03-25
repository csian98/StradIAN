/**
 * @file		crypto_market.h
 * @brief		crypto market for stradian
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-22
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"  
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_CRYPTO_MARKET_H_
#define _HEADER_CRYPTO_MARKET_H_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include "stradian/market.h"
#include "stradian/binance_exchange.h"

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
	class CryptoMarket final : public Market {
	public:
		CryptoMarket(void);

		virtual ~CryptoMarket(void) noexcept;

	    virtual void fork_crawler(void) override;
		
	private:
		
		const std::filesystem::path crawler_exec =
			"pylib/exec/crypto_crawler.py";
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
