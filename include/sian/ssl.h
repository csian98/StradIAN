/**
 * @file		ssl.h
 * @brief		SSL
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-04-30
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_SSLH_
#define _HEADER_SSLH_

/* OS dependent */
#include <ratio>
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */
#include "sian/tcp.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#include <exception>
#include <string>
#include <string_view>
#include <optional>
#include <filesystem>

#if __has_include(<iostream>)
#include <iostream>
#endif

#if _TARGET_OS == OS_WINDOWS
// Windows header
#include <Windows.h>

#elif _TARGET_OS == OS_LINUX
// Linux header
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

/* C & CPP */
/*
#ifdef __cplusplus
extern "C" {
#endif
*/

/* defines typedef & constant */

/* MACRO functions */

#ifndef MIN
#define MIN(a, b)	(a > b ? b : a)
#endif

#ifndef MAX
#define MAX(a, b)	(a > b ? a : b)
#endif

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

/* Data structures definition - struct & class */

namespace sian {
    class ssl_client : public tcp_client {
	public:
		::SSL* ssl;

		SSL_CTX* ctx;
	};

	class SSL : public TCP {
	public:
		SSL(const sian::IPv4&,
			std::string_view certificate = "server.crt",
			std::string_view privatekey = "server.key");

		virtual ~SSL(void) noexcept = default;

		virtual void accept(ssl_client&);

		virtual void connect(void) override;

		virtual void send(const std::string&, ssl_client* clnt = nullptr) const;

		virtual int recv(std::string&, ssl_client* clnt = nullptr) const;

		virtual void close(void) override;

		virtual void close(ssl_client&);
	private:
		bool ssl_auth(SSL_CTX*);

		const SSL_METHOD* method;

		std::string certificate, privatekey;

		::SSL* ssl;

		SSL_CTX* ctx;
	};
}

/* Functions declare */

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
