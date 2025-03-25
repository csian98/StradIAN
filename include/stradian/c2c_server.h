/**
 * @file		c2c_server.h
 * @brief		c2cServer class for SystemManager
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-07-14
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_C2C_SERVER_H_
#define _HEADER_C2C_SERVER_H_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <algorithm>
#include <utility>
#include <numeric>

#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>

#include <vector>
#include <filesystem>

#include "stradian/exception.h"
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

extern const std::string stradian::c2c::ipv4;

extern const int stradian::c2c::port;

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
	struct client {
		sian::ssl_client* ssl_client = nullptr;
		
		std::string user;

		bool logined = false;
		
		bool sudo = false;
		
		static std::string prompt;
	};
	
	class c2cServer {
	public:
	    c2cServer(void);

		virtual ~c2cServer(void) noexcept;

		void start(void);

		void stop(void);

		operator bool(void) const;
		
	protected:

		virtual std::pair<bool, bool> login(const std::string&,
											const std::string&) = 0;

	private:
		
		void thread_accept(void);

		void thread_read(void);

		void thread_login(sian::ssl_client*);

		void disconnect_client(sian::ssl_client*);
		
	    void send_motd(sian::ssl_client*) const;
//

		sian::SSL* ssl;

		std::mutex mtx;

	    bool status = false;

		std::vector<client> clients;

		const int max_client = 64;

		const int login_trial = 3;
		
		const std::filesystem::path motd_path = "etc/motd";

		const std::filesystem::path certificate_path = "etc/c2c/stradian.crt";

		const std::filesystem::path prvkey_path = "etc/c2c/stradian.key";
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
