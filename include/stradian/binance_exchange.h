/**
 * @file		binance_exchange.h
 * @brief		binance exchange class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-16
 */

// #pragma once
// #pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_BINANCE_EXCHANGEH_
#define _HEADER_BINANCE_EXCHANGEH_

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

#include <fstream>
#include <sstream>
#include <iomanip>

#include <chrono>
#include <filesystem>
#include <map>

#include <openssl/hmac.h>

#include "stradian/exchange.h"
#include "stradian/web_socket.h"
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
	class BinanceExchange final : public Exchange {
	public:
		BinanceExchange(const std::string& host = "ws-api.binance.com",
						const std::string& port = "443",
						const std::string& target = "/ws-api/v3");

		virtual ~BinanceExchange(void) noexcept;

		std::pair<double, double> get_validation(void) override;

		std::map<std::string, std::pair<double, double>> get_items(void) override;

		std::vector<std::string> get_symbols(void) override;

	private:
		virtual void handler(const Order&) override;

		void signature(boost::json::value&) const;

		static std::string encryptHMAC(const char*, const char*);

		static std::string read_file(const std::filesystem::path&);
		
		static std::string get_id(void);

		static std::string get_timestamp(void);

		bool status(const boost::json::value&);

		void limits(const boost::json::value&);

		boost::json::value request_wrapper(const boost::json::value&);

		bool ping(void);
		
		void update(void);

		double price(const Order&);

		double price(const std::string&);

		void buy(const Order&);

		void sell(const Order&);

		std::vector<std::string> symbols(void);

		WebSocket websocket;

		static unsigned long id;

		std::map<std::string, std::pair<double, double>> assets;

		mutable std::mutex mtx;

		std::string api_key, secret_key;

		const std::filesystem::path key_path = "etc/exchange/binance_api_key";

		const std::filesystem::path secret_path = "etc/exchange/binance_api_secret";

		const std::string key_currency = "USDT";
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
