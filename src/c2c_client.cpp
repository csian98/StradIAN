/**
 * @file		c2c_client.cpp
 * @brief		c2cClient class for StradIAN
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-07-15
 */

#include "stradian/c2c_client.h"

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

stradian::c2cClient::c2cClient(std::string_view certificate_path,
							   std::string_view prvkey_path,
							   std::ostream& out)
	: certificate_path(certificate_path), prvkey_path(prvkey_path), out(out) {
	sian::IPv4 ipv4(sian::TYPE::CLIENT,
					c2c::ipv4,
					c2c::port);

    this->ssl = new sian::SSL(ipv4,
							  this->certificate_path.c_str(),
							  this->prvkey_path.c_str());
	
	this->ssl->socket();

	this->ssl->connect();
}

stradian::c2cClient::~c2cClient(void) noexcept {
	this->ssl->close();
}

void stradian::c2cClient::start(void) {
	std::thread thread(&c2cClient::thread_read, *this);

	thread.detach();

	std::cin.tie(nullptr);

	std::string in;
	do {
		in.clear();
		
		std::cin >> in;

		this->ssl->send(in);
	} while(in != "quit");
}

void stradian::c2cClient::thread_read(void) {
	while (true) {
		std::string msg;
		int rcv = this->ssl->recv(msg);
		if (rcv == -1) break;
		
	    this->out << msg;
	}
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
