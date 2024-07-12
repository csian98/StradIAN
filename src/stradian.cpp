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

	this->update_system_manager();

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
stradian::StradIAN::get_validation(void) const {
	double key = 0.0, other = 0.0;
	for (auto market : this->markets) {
		auto [nkey, nother] = market->get_validation();
		key += nkey;
		other += nother;
	}
	return std::make_pair(key, other);
}

std::pair<double, double>
stradian::StradIAN::get_validation(MARKETCODE code) const {
	return this->get_market(code)->get_validation();
}

std::map<std::string, std::pair<double, double>>
stradian::StradIAN::get_items(void) const {
	std::map<std::string, std::pair<double, double>> merged;

	for (auto market : this->markets) {
		auto market_item = market->get_items();
		merged.insert(market_item.begin(), market_item.end());
	}

	return merged;
}

std::map<std::string, std::pair<double, double>>
stradian::StradIAN::get_items(MARKETCODE code) const {
	return this->get_market(code)->get_items();
}

std::vector<std::string>
stradian::StradIAN::get_symbols(void) const {
	std::vector<std::string> merged;
	
	for (auto market : this->markets) {
		auto market_symbol = market->get_symbols();
		merged.insert(merged.end(), market_symbol.begin(), market_symbol.end());
	}

	return merged;
}

std::vector<std::string>
stradian::StradIAN::get_symbols(MARKETCODE code) const {
	return this->get_market(code)->get_symbols();
}

void stradian::StradIAN::run_crawler(void) const {
	for (auto market : this->markets) {
	    market->fork_crawler();
	}
}

void stradian::StradIAN::run_crawler(MARKETCODE code) const {
	this->get_market(code)->fork_crawler();
}

void stradian::StradIAN::update_system_manager(void) {
	this->update_balance();

	for (auto market : this->markets) {
		this->update_market(market->get_code());
	}
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
