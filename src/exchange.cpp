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

void stradian::Exchange::start(void) const {
//	this->m_thread = std::move(std::thread(&Exchange::handle_function, this));
}

void stradian::Exchange::stop(void) {
	this->status.store(false);
}

void stradian::Exchange::buy_order(unsigned code, double quantity,
								   int priority) {
	std::unique_lock lck(this->mtx);
	order.push(Order(code, 0, quantity, priority));
	lck.unlock();
	
	this->cond_var.notify_one();
}

void stradian::Exchange::sell_order(unsigned code, double quantity,
									int priority) {
	std::unique_lock lck(this->mtx);
	order.push(Order(code, 1, quantity, priority));
	lck.unlock();
	
	this->cond_var.notify_one();
}

void stradian::Exchange::handle_function(void) {
    this->status.store(true);

    do {
		std::unique_lock lck(this->mtx);
		this->cond_var.wait(lck);

	    if (!this->order.empty()) {
			this->handler(this->order.top());
			this->order.pop();
		}

	    lck.unlock();
	} while (this->status);
}
#include <iostream>
void stradian::Exchange::handler(const Order&) const {
	std::cout << "This is for test" << std::endl;
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
