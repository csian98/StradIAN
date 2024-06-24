/**
 * @file		exception.h
 * @brief		exceptions for stradian
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-15
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_EXCEPTIONH_
#define _HEADER_EXCEPTIONH_

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
#include <ios>
#include <ostream>
#include <fstream>
#include <sstream>

#include <execution>
#include <stdexcept>
#include <filesystem>

#include <chrono>
#include <ctime>

#include "stradian/order.h"
#include "stradian/slack.h"

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
	class Exception : public std::exception {
	public:
		Exception(const std::string&);

		virtual const char* what(void) const noexcept override;
		
	protected:
		std::string message;
	};

	enum class LOGLEVEL {
		INFO,
		WARN,
		ERROR,
		FATAL
	};

	class Logger final : public Exception {
	public:
		Logger(const std::string&, bool slack = false);

	    void log(LOGLEVEL level = LOGLEVEL::INFO) const;

		void set_level(LOGLEVEL loglevel);

	private:
		const std::string local_time(void) const;

		LOGLEVEL loglevel = LOGLEVEL::INFO;
		
		const std::filesystem::path path = "./var/log/system.log";

		static Slack slack;

		bool use_slack;
	};

	class TransactionRecord final : public Exception {
	public:
		TransactionRecord(const std::string&);

	    void record(void) const;
		
	private:
	    const std::string local_time(void) const;

		const std::filesystem::path path = "./var/log/transaction.log";

		static Slack slack;
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
