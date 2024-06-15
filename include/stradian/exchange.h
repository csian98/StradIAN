/**
 * @file		exchange.h
 * @brief		exchange virtual class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-15
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_EXCHANGEH_
#define _HEADER_EXCHANGEH_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <queue>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>

#include "stradian/order.h"
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
	class Exchange {
	public:
		Exchange(void);

		virtual ~Exchange(void) noexcept;
		
		virtual void start(void) const;

		virtual void stop(void);
		
	    virtual void buy_order(unsigned, double, int priority = 0);

		virtual void sell_order(unsigned, double, int priority = 0);

	protected:
		std::priority_queue<Order> order;

		std::thread m_thread;

		std::mutex mtx;

		std::condition_variable cond_var;
		
		virtual void handle_function(void);

		virtual void handler(const Order&) const;

		std::atomic<bool> status;
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
