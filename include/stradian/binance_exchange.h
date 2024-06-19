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

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>
//#include <boost/json/src.hpp>

#include "stradian/exchange.h"
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
		BinanceExchange(void);

		virtual ~BinanceExchange(void) noexcept;

	private:
		virtual void connect(void);

		virtual void disconnect(void);
		
		virtual void update(void) override;

		virtual void buy(Order&) override;

		virtual void sell(Order&) override;

		virtual void handler(const Order&) override;

		virtual boost::json::value send(const boost::json::value&);

		virtual bool ping(void);

		static std::string read_file(const std::filesystem::path&);
		
		static std::string get_id(void);

		static std::string get_timestamp(void);

	    void signature(boost::json::value&) const;

		std::string encryptHMAC(const char*, const char*) const;

		std::string api_key, secret_key;

		const std::filesystem::path key_path = "etc/key/binance_api_key";

		const std::filesystem::path secret_path = "etc/key/binance_api_secret";

		const std::string host = "ws-api.binance.com";
		
		std::string port = "443";

		const std::string target = "/ws-api/v3";

		boost::asio::io_context ioc;

		boost::asio::ssl::context ctx;

		boost::beast::websocket::stream<
			boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ws;
		
		static unsigned long id;
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
