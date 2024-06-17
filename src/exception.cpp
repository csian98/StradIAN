/**
 * @file	    exception.cpp
 * @brief		exceptions for stradian
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-15
 */

#include "stradian/exception.h"

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

stradian::Exception::Exception(const std::string& message) : message(message) {}

const char* stradian::Exception::what(void) const noexcept {
	return this->message.c_str();
}

stradian::Logger::Logger(const std::string& message,
						 const std::filesystem::path& path)
	: Exception(message), path(path) {}

void stradian::Logger::write(std::string_view level) const {
	std::ofstream fout(this->path, std::ios::app);
	std::string time = this->local_time();
	fout << "[" << time <<
		"] (" << level << ") " << this->message << '\n';
}

const std::string stradian::Logger::local_time(void) const {
    auto now = std::chrono::system_clock::now();
	std::time_t t_now = std::chrono::system_clock::to_time_t(now);
	tm* t_tm = std::localtime(&t_now);

	char buffer[64];
	std::strftime(buffer, sizeof(buffer),
				  "%Y-%m-%d %H:%M:%S", t_tm);
	return std::string(buffer);
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
