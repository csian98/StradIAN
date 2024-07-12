/**
 * @file		exchange.cpp
 * @brief		exchange virtual class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-15
 */

#include "stradian/exchange.h"

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

stradian::Exchange::Exchange(void) : status(false) {}

stradian::Exchange::~Exchange(void) noexcept {
	if (this->m_thread.joinable()) {
		m_thread.join();
	}
}

void stradian::Exchange::start(void) {
	this->m_thread = std::move(std::thread(&Exchange::thread_function, this));
}

void stradian::Exchange::stop(void) {
	while (!this->order.empty()) {}
	
	this->status.store(false);

	this->cond_var.notify_one();
}

void stradian::Exchange::buy_order(const std::string& symbol,
								   double quantity, int priority) {
	{
		std::unique_lock lck(this->mtx);
		Order rcp(stradian::ORDERCODE::BUYSELL, priority);
		rcp.market_order(symbol, true, quantity);
		this->order.push(rcp);
	}
	
	this->cond_var.notify_one();
}

void stradian::Exchange::buy_order(const std::string& symbol,
								   double price, double quantity, int priority) {
	{
		std::unique_lock lck(this->mtx);
		Order rcp(stradian::ORDERCODE::BUYSELL, priority);
		rcp.limit_order(symbol, true, price, quantity);
		this->order.push(rcp);
	}

	this->cond_var.notify_one();
}

void stradian::Exchange::sell_order(const std::string& symbol,
									double quantity, int priority) {
	{
		std::unique_lock lck(this->mtx);
		Order rcp(stradian::ORDERCODE::BUYSELL, priority);
	    rcp.market_order(symbol, false, quantity);
		this->order.push(rcp);
	}
	
	this->cond_var.notify_one();
}

void stradian::Exchange::sell_order(const std::string& symbol,
									double price, double quantity, int priority) {
	{
		std::unique_lock lck(this->mtx);
		Order rcp(stradian::ORDERCODE::BUYSELL, priority);
		rcp.limit_order(symbol, false, price, quantity);
		this->order.push(rcp);
	}

	this->cond_var.notify_one();
}

void stradian::Exchange::thread_function(void) {
    this->status.store(true);

    do {
		{
			std::unique_lock lck(this->mtx);
			this->cond_var.wait(lck);
//								,[this](){
//									return !this->order.empty();
//								});
			while (!this->order.empty()) {
				this->handler(this->order.top());
				this->order.pop();
			}
		}
	} while (this->status);
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
