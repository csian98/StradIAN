/**
 * @file		system_manager.h
 * @brief		SystemManager class for StradIAN
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-26
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations" 
// #pragma comment(lib, "libpthread.so")

#ifndef _HEADER_SYSTEM_MANAGER_H_
#define _HEADER_SYSTEM_MANAGER_H_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

#include <utility>

#include <string>
#include <string_view>
#include <sstream>
#include <ios>

#include <openssl/sha.h>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "stradian/mariadb.h"
#include "stradian/market.h"
#include "stradian/c2c_server.h"
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
	class SystemManager : public c2cServer {
	public:
		SystemManager(void);

		virtual ~SystemManager(void) noexcept = default;

	protected:
		virtual std::pair<double, double> get_validation(void) const = 0;

		virtual std::pair<double, double> get_validation(MARKETCODE) const = 0;

		virtual std::map<std::string, std::pair<double, double>> get_items(void) const = 0;

		virtual std::map<std::string, std::pair<double, double>> get_items(MARKETCODE) const = 0;

		virtual std::vector<std::string> get_symbols(void) const = 0;

		virtual std::vector<std::string> get_symbols(MARKETCODE) const = 0;

		virtual void update_system_manager(void) = 0;

		// c2cServer
		virtual std::pair<bool, bool> login(const std::string&,
											const std::string&) override;
		
		// users
		// needs to write again share parts;
		// share update required to process after assets updated
	    virtual unsigned int update_uid(void);
		
	    virtual void add_user(
			const std::string&, const std::string&,
			bool auth = false, double usd = 0.0,
			std::optional<std::string_view> phone = std::nullopt,
			std::optional<std::string_view> email = std::nullopt,
			std::optional<std::string_view> slack_id = std::nullopt);

	    virtual void update_user_info(
			const unsigned int,
			const std::string&,
			std::optional<std::string_view> value = std::nullopt);

		virtual double update_others_share(double usd);

		virtual void adjust_share(const unsigned int,
								  const double);

		// balance
		virtual void update_balance(void);

		virtual void write_report(REPORT_TYPE);

		// valuations
		

		// XXXX_market
		virtual void update_market(MARKETCODE);

		virtual void open_symbol(MARKETCODE, const std::string&);

		virtual void close_symbol(MARKETCODE, const std::string&);

		static std::string sha256(const std::string&);

		static unsigned int uid;
		
		stradian::MariaDB systemDB;
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
