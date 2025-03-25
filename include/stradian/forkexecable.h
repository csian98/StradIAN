/**
 * @file		forkexecable.h
 * @brief		ForkExecable Class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-04-03
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_FORKEXECABLE_H_
#define _HEADER_FORKEXECABLE_H_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <filesystem>
#include <thread>

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
#include <sys/ipc.h>
#include <sys/wait.h>
#include <signal.h>

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
	class ForkExecable {
	public:
		ForkExecable(void);
		
		virtual ~ForkExecable(void) noexcept;
		
		virtual void fork_exec(const std::filesystem::path&);

		virtual void wait_child(void);

		virtual void wait_async(void);

		virtual bool is_running(void) const;
		
	private:
		virtual void fork(void);

		virtual void exec(void);

		bool running = false;

		std::thread m_thread;

		std::filesystem::path path;
		
		pid_t pid;
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
