/**
 * @file		market.cpp
 * @brief		market virtual class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-15
 */

#include "stradian/market.h"

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

stradian::Market::Market(MARKETCODE code) : code(code) {}

stradian::Market::~Market(void) noexcept {
	
}

stradian::MARKETCODE stradian::Market::get_code(void) const {
	return this->code;
}

std::pair<double, double>
stradian::Market::get_validation(void) const {
    return this->exchange->get_validation();
}

std::map<std::string, std::pair<double, double>>
stradian::Market::get_items(void) const {
	return std::move(this->exchange->get_items());
}

std::vector<std::string>
stradian::Market::get_symbols(void) const {
	return std::move(this->exchange->get_symbols());
}

/* Functions definition */

std::string stradian::to_string(MARKETCODE code) {
	std::string s_code;
	switch (code) {
	case MARKETCODE::STOCK:
		s_code = "stock";
		break;
	case MARKETCODE::CRYPTO:
		s_code = "crypto";
		break;
	}

	return s_code;
}

#endif // OS dependency

/*
#ifdef __cplusplus
}
#endif
*/
