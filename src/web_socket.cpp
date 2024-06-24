/**
 * @file		web_socket.cpp
 * @brief		web socket for stradian
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-19
 */

#include "stradian/web_socket.h"
#include <boost/json/src.hpp>

/* C & CPP */
/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#if _TARGET_OS == OS_WINDOWS

#elif _TARGET_OS == OS_LINUX

/* Definition */

/* ASM codes */
/*
asm(R"ASM(
  ; assembly code x86-64 AT&T(fastcall-cdecl)
func:
  .cfi_startproc
        pushq %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq %rsp, %rbp
        .cfi_def_cfa_register 6
        movl %edi, -4(%rbp)  ; %rdi -> %rsi -> %rdx -> %rcx ->
        movl %esi, -8(%rbp)  ; %r8  -> %r9  -> stack
; return value %eax
        popq %rbp
				.cf_def_cfa 7, 8
				ret
				.cfi_endproc
)ASM");
*/

/* Data structures definition - struct & class */

stradian::WebSocket::WebSocket(const std::string& host,
							   const std::string& port,
							   const std::string& target)
	: ctx(boost::asio::ssl::context::sslv23_client), ws(ioc, ctx),
	  host(host), port(port), target(target) {
	this->ctx.set_default_verify_paths();
	this->connect();
}

stradian::WebSocket::~WebSocket(void) noexcept {
	this->disconnect();
}

void stradian::WebSocket::connect(void) {
	boost::asio::ip::tcp::resolver resolver(this->ioc);
	auto const results = resolver.resolve(this->host, this->port);
	auto& raw = boost::beast::get_lowest_layer(this->ws);
	auto ep = boost::asio::connect(raw, results);

	if (!SSL_set_tlsext_host_name(this->ws.next_layer().native_handle(), this->host.c_str())) {
		throw boost::system::system_error(
			::ERR_get_error(), boost::asio::error::get_ssl_category()
			);
	}
	this->ws.next_layer().handshake(boost::asio::ssl::stream_base::client);

	this->ws.set_option(
		boost::beast::websocket::stream_base::decorator(
			[](boost::beast::websocket::request_type& req) {
				req.set(boost::beast::http::field::user_agent,
						std::string(BOOST_BEAST_VERSION_STRING) +
						" websocket-client-coro");
			})
		);

	this->ws.handshake(this->host + ":" +std::to_string(ep.port()), this->target);
}

void stradian::WebSocket::disconnect(void) {
	this->ws.close(
		boost::beast::websocket::close_code::normal
		);
}

boost::json::value stradian::WebSocket::request(const boost::json::value& req) {
	if (!this->ws.is_open())
		this->connect();
	
	this->ws.write(boost::asio::buffer(boost::json::serialize(req)));
	
	boost::beast::flat_buffer buffer;
	
	this->ws.read(buffer);
	
	std::string res(boost::asio::buffers_begin(buffer.data()),
					boost::asio::buffers_end(buffer.data()));

	return std::move(boost::json::parse(res));
}

void stradian::WebSocket::request(const boost::json::value& req,
								  Extractable<boost::json::value>* object) {
	object->extract(this->request(req));
}

/* Functions definition */

/*

void swap(Sample& lhs, Sample& rhs) noexcept {
	// shallow copy
    // using std::swap;
}

*/

#endif // OS dependency

/*
#ifdef __cplusplus
}
#endif
*/
