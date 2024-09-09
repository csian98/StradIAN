/**
 * @file		stradian.h
 * @brief		StradIAN class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-24
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
// #pragma comment(lib, "libpthread.so")

#ifndef _HEADER_STRADIAN_H_
#define _HEADER_STRADIAN_H_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <vector>
#include <string>
#include <string_view>

#include "stradian/system_manager.h"
#include "stradian/crypto_market.h"
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
	class StradIAN final : public SystemManager {
	public:
		StradIAN(void);

		virtual ~StradIAN(void) noexcept;

		void run(void);

	private:

		virtual std::pair<double, double> get_validation(void) const override;
		
		virtual std::pair<double, double> get_validation(MARKETCODE) const override;

		virtual std::map<std::string, std::pair<double, double>> get_items(void) const override;

		virtual std::map<std::string, std::pair<double, double>> get_items(MARKETCODE) const override;

		virtual std::vector<std::string> get_symbols(void) const override;

		virtual std::vector<std::string> get_symbols(MARKETCODE) const override;

		virtual void run_crawler(void) const;

		virtual void run_crawler(MARKETCODE) const;

		virtual void update_system_manager(void) override;
		
		std::shared_ptr<Market> get_market(MARKETCODE) const;

		std::vector<std::shared_ptr<Market>> markets;
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
