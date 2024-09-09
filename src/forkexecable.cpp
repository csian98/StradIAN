/**
 * @file		forkexecable.cpp
 * @brief		ForkExecable class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-07-01
 */

#include "stradian/forkexecable.h"

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

stradian::ForkExecable::ForkExecable(void) {}

stradian::ForkExecable::~ForkExecable(void) noexcept {
	if (this->running) {
		this->m_thread.join();
	}
}

void stradian::ForkExecable::fork_exec(
	const std::filesystem::path& path) {
	if (this->running) {
		Logger logger("ForkExecable: already running");
		logger.log(LOGLEVEL::WARN);
	} else {
		this->path = path;
		this->fork();

		if (this->pid == 0)
			this->exec();
	}
}

void stradian::ForkExecable::wait_child(void) {
	int status = 0;
	::waitpid(this->pid, &status, 0);

	if (WIFSIGNALED(status)) {
		Logger logger("ForkExecable: wait_pid failed", true);
		logger.log(LOGLEVEL::FATAL);
		throw logger;
	}

	this->running = false;
}

void stradian::ForkExecable::wait_async(void) {
    this->m_thread =
		std::move(std::thread(&ForkExecable::wait_child, this));
}

bool stradian::ForkExecable::is_running(void) const {
	return this->running;
}

void stradian::ForkExecable::fork(void) {
	if ((this->pid = ::fork()) == -1) {
		Logger logger("ForkExecable: fork failed", true);
		logger.log(LOGLEVEL::FATAL);
		throw logger;
	}

	this->running = true;
}

void stradian::ForkExecable::exec(void) {
	::execl(this->path.c_str(), this->path.c_str(), (char*)NULL);

	Logger logger("ForkExecable: execl failed", true);
	logger.log(LOGLEVEL::FATAL);
	throw logger;
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
