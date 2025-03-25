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
						 bool slack)
	: Exception(message), use_slack(slack) {}

void stradian::Logger::log(LOGLEVEL level) const {
	if (this->loglevel <= level) {
		std::stringstream str;
		std::string time = this->local_time();
		str << "[" << time <<
			"] (" << to_string(level) << ") " << this->message << '\n';
		
		std::ofstream fout(this->path, std::ios::app);
		fout << str.str();
		fout.close();
		
		if (this->use_slack)
			this->slack.write(str.str());
	}
}

void stradian::Logger::set_level(LOGLEVEL loglevel) {
	this->loglevel = loglevel;
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

stradian::TransactionRecord::TransactionRecord(const std::string& message)
	: Exception(message) {}

void stradian::TransactionRecord::record(void) const {
	std::stringstream str;
	std::string time = this->local_time();
	str << "[" << time << "] "  << this->message << '\n';
	
	std::ofstream fout(this->path, std::ios::app);
	fout << str.str();
	fout.close();

	slack.write(str.str());
}


const std::string stradian::TransactionRecord::local_time(void) const {
    auto now = std::chrono::system_clock::now();
	std::time_t t_now = std::chrono::system_clock::to_time_t(now);
	tm* t_tm = std::localtime(&t_now);

	char buffer[64];
	std::strftime(buffer, sizeof(buffer),
				  "%Y-%m-%d %H:%M:%S", t_tm);
	return std::string(buffer);
}

stradian::Reporter::Reporter(double current, double past,
							 REPORT_TYPE report_type)
	: report_type(report_type) {
	this->content << "[" <<this->date() << "]" ;
	
	switch (report_type) {
	case REPORT_TYPE::DAILY:
		this->path /= "daily.log";
	    this->content << "Daily";
		break;
	case REPORT_TYPE::WEEKLY:
		this->path /= "weekly.log";
		this->content << "Weekly";
		break;
	case REPORT_TYPE::MONTLY:
		this->path /= "montly.log";
		this->content << "Montly";
		break;
	case REPORT_TYPE::QUARTER:
		this->path /= "quarter.log";
		this->content << "QUARTER";
	}

	double inc = (current - past) / past;
	std::string inc_str;
	if (inc > 0.0) {
		inc_str = "+" + std::to_string(inc);
	} else {
		inc_str = std::to_string(inc);
	}
	
	this->content << " Report ;Current Asset Valuation: " << current
				  << "(" << inc_str << ")";
};

const std::string stradian::Reporter::date(void) const {
    auto now = std::chrono::system_clock::now();
	std::time_t t_now = std::chrono::system_clock::to_time_t(now);
	tm* t_tm = std::localtime(&t_now);

	char buffer[64];
	std::strftime(buffer, sizeof(buffer),
				  "%Y%m%d", t_tm);
	return std::string(buffer);
}

void stradian::Reporter::report(void) const {
	std::ofstream fout(this->path, std::ios::app);
	fout << this->content.str();
	fout.close();
	
	this->slack.write(this->content.str());
}

/* Functions definition */

stradian::Slack stradian::Logger::slack("./etc/slack/system");

stradian::Slack stradian::TransactionRecord::slack("./etc/slack/transaction");

stradian::Slack stradian::Reporter::slack("./etc/slack/report");

std::string stradian::to_string(LOGLEVEL log_level) {
	std::string s_level;

	switch (log_level) {
	case LOGLEVEL::INFO:
		s_level = "INFO";
		break;
	case LOGLEVEL::WARN:
		s_level = "WARN";
		break;
	case LOGLEVEL::ERROR:
		s_level = "ERROR";
		break;
	case LOGLEVEL::FATAL:
		s_level = "FATAL";
		break;
	}

	return s_level;
}

std::string stradian::to_string(REPORT_TYPE report_type) {
	std::string s_report_type;

	switch (report_type) {
	case REPORT_TYPE::DAILY:
		s_report_type = "Daily";
		break;
	case REPORT_TYPE::WEEKLY:
		s_report_type = "Weekly";
		break;
	case REPORT_TYPE::MONTLY:
		s_report_type = "Montly";
		break;
	case REPORT_TYPE::QUARTER:
		s_report_type = "Quarter";
		break;
	}

	return s_report_type;
}

#endif // OS dependency

/*
#ifdef __cplusplus
}
#endif
*/
