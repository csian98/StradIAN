/**
 * @file		exchange.h
 * @brief		exchange virtual class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-15
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
// #pragma comment(lib, "libpthread.so")

#ifndef _HEADER_EXCHANGE_H_
#define _HEADER_EXCHANGE_H_

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
#include <map>
#include <unordered_map>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>

#include "stradian/order.h"

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
		
		virtual void start(void);

		virtual void stop(void);
		
		virtual std::pair<double, double> get_validation(void) = 0;

		virtual std::map<std::string, std::pair<double, double>> get_items(void) = 0;

		virtual std::vector<std::string> get_symbols(void) = 0;
		
	    virtual void buy_order(const std::string&, double, int priority = 0);

		virtual void buy_order(const std::string&, double, double, int priority = 0);

		virtual void sell_order(const std::string&, double, int priority = 0);

		virtual void sell_order(const std::string&, double, double, int priority = 0);

	protected:
		virtual void handler(const Order&) = 0;

	private:
		virtual void thread_function(void);
		
		std::priority_queue<Order> order;

		std::thread m_thread;

		std::mutex mtx;

		std::condition_variable cond_var;

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
