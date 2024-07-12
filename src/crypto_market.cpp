/**
 * @file		sample.cpp
 * @brief		
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-04-03
 */

#include "stradian/crypto_market.h"

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

stradian::CryptoMarket::CryptoMarket(void)
	: Market(MARKETCODE::CRYPTO) {
	std::unique_ptr<Exchange> exchange(
		new BinanceExchange
		);
	
	this->exchange = std::move(exchange);
	
	this->exchange->start();
}

stradian::CryptoMarket::~CryptoMarket(void) noexcept {
	this->exchange->stop();
}

void stradian::CryptoMarket::fork_crawler(void) {
	this->fork_exec(this->crawler_exec);

	this->wait_async();
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
