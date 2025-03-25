/**
 * @file		c2c_client.h
 * @brief		c2cClient class for StradIAN
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-07-14
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_C2C_CLIENT_H_
#define _HEADER_C2C_CLIENT_H_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <ios>
#include <iostream>
#include <ostream>

#include <string>
#include <string_view>
#include <filesystem>

#include <thread>

#include "stradian/ipv4.h"

#include "sian/tcp.h"
#include "sian/ssl.h"

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

extern const std::string stradian::c2c::ipv4;

extern const int stradian::c2c::port;

namespace stradian {
	class c2cClient {
	public:
		c2cClient(std::string_view, std::string_view,
				  std::ostream& out = std::cout);

		virtual ~c2cClient(void) noexcept;

		void start(void);

	private:
		void thread_read(void);
		
		sian::SSL* ssl;

		std::ostream& out;

		const std::filesystem::path certificate_path;

		const std::filesystem::path prvkey_path;
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
