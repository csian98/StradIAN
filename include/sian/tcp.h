/**
 * @file		tcp.h
 * @brief		TCP
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-04-30
 */
	 
//#pragma once
//#pragma GCC diagnostic ignored "-Wstringop-truncation"
//#pragma comment(lib, "libpthread.so")

#ifndef _HEADER_TCPH_
#define _HEADER_TCPH_

/* OS dependent */
#define OS_WINDOWS	0
#define OS_LINUX	1

#ifdef _WIN32
#define _TARGET_OS	OS_WINDOWS
#else
	#define _TARGET_OS	OS_LINUX
#endif

/* Include */

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

#include <string.h>
#include <strings.h>

/* C & CPP */
/*
#ifdef __cplusplus
extern "C" {
#endif
*/

/* defines typedef & constant */

#define DOMAIN_TYPE	AF_INET
#define SOCKET_TYPE	SOCK_STREAM
#define PACKET_SIZE	2048

/* MACRO functions */
#ifndef SWAP
template <typename T>
inline void SWAP(T& a, T& b) {
	T tmp = std::move(a);
	a = std::move(b);
	b = std::move(tmp);
}
#endif

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
	enum class TYPE {
		CLIENT,
		SERVER
	};

	struct IPv4 {
		IPv4(TYPE type, const std::string& ipv4, int port) : type(type), ipv4(ipv4), port(port) {}
		
		TYPE type;
		
		std::string ipv4;
		
		int port;
	};

    class tcp_client {
	public:
		int sock_fd;

		struct sockaddr_in addr;
	};

	class network_error : public std::exception {
	public:
		explicit network_error(const std::string& msg) : msg(msg) {}

		virtual ~network_error(void) throw() {}

		virtual const char* what(void) const throw() {
			return msg.c_str();
		}
	private:
		std::string msg;
	};
	
	class TCP {
	public:
		TCP(const IPv4&);
		
		virtual ~TCP(void) noexcept = default;
		
		virtual void socket(void);
		
		virtual void bind(void);
		
		virtual void listen(int);
		
		virtual void accept(tcp_client&);
		
		virtual void connect(void);
		
		virtual void send(const std::string&, tcp_client* client = nullptr) const;
		
		virtual int recv(std::string&, tcp_client* client = nullptr) const;
		
		virtual void close(void);
		
		virtual void close(tcp_client&);

	protected:
		int sock_fd;
		
		struct sockaddr_in server_addr;
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
