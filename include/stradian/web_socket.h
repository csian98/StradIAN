/**
 * @file		web_socket.h
 * @brief		web socket class for stradian
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-19
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
// #pragma comment(lib, "libpthread.so")

#ifndef _HEADER_WEB_SOCKET_H_
#define _HEADER_WEB_SOCKET_H_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>

#include "stradian/extractable.h"

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
	class WebSocket final {
	public:
		WebSocket(const std::string&,
				  const std::string&,
				  const std::string&);

		virtual ~WebSocket(void) noexcept;

		boost::json::value request(const boost::json::value&);

		void request(const boost::json::value&, Extractable<boost::json::value>*);
		
	private:

		virtual void connect(void);

		virtual void disconnect(void);

		const std::string host;

		const std::string port;

		const std::string target;

		boost::asio::io_context ioc;

		boost::asio::ssl::context ctx;

		boost::beast::websocket::stream<
			boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ws;

		std::mutex mtx;
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
//#include "stradian/sample.hpp"

#endif // Header duplicate
