/**
 * @file		slack.h
 * @brief		slack class for logger
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-18
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"  
// #pragma comment(lib, "libpthread.so")

#ifndef _HEADER_SLACK_H_
#define _HEADER_SLACK_H_

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

#include <fstream>
#include <filesystem>
#include <sstream>

#include <curl/curl.h>

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
	class Slack {
	public:
		Slack(const std::filesystem::path&);

		virtual ~Slack(void) noexcept = default;

		void write(const std::string&);
		
	private:
		
		static std::string read_file(const std::filesystem::path&);

		const std::string agent = "StradIAN Kyra SlackBot/1.0";

		const std::string host = "slack.com";

		std::string port = "443";

		const std::string target = "/api/chat.postMessage";

		std::string token;

		std::string channel;

		const std::filesystem::path token_path = "./etc/slack/oauth";
	};
}

/* Functions declaration */

namespace stradian {
	size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data);
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
