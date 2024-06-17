/**
 * @file		order.cpp
 * @brief		Order class for Exchange
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-15
 */

#include "stradian/order.h"

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

stradian::Order::Order(const std::string& symbol, bool status,
					   double quantity, int priority)
	: symbol(symbol), status(status), quantity(quantity), priority(priority) {}

std::pair<std::string, double> stradian::Order::get_item(void) const {
	return std::make_pair(this->symbol, this->quantity);
}

bool stradian::Order::opeartor() const {
	return this->status;
}

bool stradian::Order::operator<(const Order& other) const {
	return this->priority < other.priority;
}

/* Functions definition */

#endif // OS dependency

/*
#ifdef __cplusplus
}
#endif
*/
