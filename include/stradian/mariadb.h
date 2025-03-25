/**
 * @file		mariadb.h
 * @brief		mariadb class for market
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-22
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"  
// #pragma comment(lib, "libpthread.so")

#ifndef _HEADER_MARIADB_H_
#define _HEADER_MARIADB_H_

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

#include <string>
#include <string_view>
#include <filesystem>
#include <chrono>

#include <mariadb/conncpp.hpp>

#include "stradian/extractable.h"
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
	class MariaDB {
	public:
		typedef sql::ResultSet* RESULT_TYPE;
		
		MariaDB(const std::string& db = "system");

		virtual ~MariaDB(void) noexcept;

		void query(const std::string&);

		void query(const std::string&, sql::ResultSet*&);

		void create_tables(void);

		void alter_table(const std::string&);

		static const std::string date(void);

		static const std::string date_time(void);

		static const std::string timestamp(void);

	private:
		
		void connect(void);

		void disconnect(void);

		void create_table(const std::filesystem::path&);

		static std::string read_file(const std::filesystem::path&);

		static std::string create_table_query(const boost::json::value&);

		static std::string alter_table_query(const boost::json::value&);
		
		std::unique_ptr<sql::Connection> conn;

		const std::filesystem::path user_path = "etc/dbms/mariadb_user";

		const std::filesystem::path passwd_path =
			"etc/dbms/mariadb_passwd";

		const std::filesystem::path json_path;
		
	    std::string user;

		std::string password;

		const std::string host = "localhost";

		const std::string db;

		const int port = 3306;
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
