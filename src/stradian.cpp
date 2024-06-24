/**
 * @file		stradian.cpp
 * @brief		StradIAN class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-24
 */

#include "stradian/stradian.h"

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

stradian::StradIAN::StradIAN(void) {
	try {
		this->markets.push_back(
			std::make_shared<CryptoMarket>()
			);
	} catch (Exception& e) {
		std::cerr << e.what() << std::endl;
	}

	Logger logger("StradIAN: stradian started successfully");
	logger.log(LOGLEVEL::INFO);
}

stradian::StradIAN::~StradIAN(void) noexcept {
    while (!this->markets.empty()) {
	    this->markets.back().reset();
		this->markets.pop_back();
	}

	Logger logger("StradIAN:: stradian terminated stably");
	logger.log(LOGLEVEL::INFO);
}

std::pair<double, double>
stradian::StradIAN::asset(void) const {
	double key = 0.0, other = 0.0;
	for (auto market : this->markets) {
		auto [nkey, nother] = market->get_asset();
		key += nkey;
		other += nother;
	}
	return std::make_pair(key, other);
}

std::pair<double, double>
stradian::StradIAN::asset(MARKETCODE code) const {
	return this->get_market(code)->get_asset();
}

std::shared_ptr<stradian::Market>
stradian::StradIAN::get_market(MARKETCODE code) const {
	for (auto market : this->markets) {
		if (market->get_code() == code) {
			return market;
		}
	}
	return nullptr;
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
