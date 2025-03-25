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

stradian::Order::Order(ORDERCODE ordercode, unsigned int priority)
	: ordercode(ordercode), priority(priority) {}

void stradian::Order::market_order(const std::string& symbol,
							  bool is_buy, double quantity) {
	this->ordercode = ORDERCODE::BUYSELL;
	this->symbol = symbol;
	this->is_buy = is_buy;
	this->quantity = quantity;
}

void stradian::Order::limit_order(const std::string& symbol,
								  bool is_buy, double price, double quantity) {
	this->ordercode = ORDERCODE::BUYSELL;
	this->symbol = symbol;
	this->is_buy = is_buy;
	this->price = price;
	this->quantity = quantity;
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
