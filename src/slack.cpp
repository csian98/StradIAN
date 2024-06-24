/**
 * @file		slack.cpp
 * @brief		slack class for logger
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-18
 */

#include "stradian/slack.h"

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

stradian::Slack::Slack(const std::filesystem::path& channel_path) {
	this->token = this->read_file(this->token_path);
	this->channel = this->read_file(channel_path);
}

void stradian::Slack::write(const std::string& message) {
	CURL* curl;
	CURLcode res;
	std::string response;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (curl) {
		std::string url = "https://" + this->host + this->target;

		struct curl_httppost* formpost = NULL;
		struct curl_httppost* lastptr = NULL;

		curl_formadd(&formpost, &lastptr,
					 CURLFORM_COPYNAME, "token",
					 CURLFORM_COPYCONTENTS, this->token.c_str(),
					 CURLFORM_END);

		curl_formadd(&formpost, &lastptr,
					 CURLFORM_COPYNAME, "channel",
					 CURLFORM_COPYCONTENTS, this->channel.c_str(),
					 CURLFORM_END);

		curl_formadd(&formpost, &lastptr,
					 CURLFORM_COPYNAME, "text",
					 CURLFORM_COPYCONTENTS, message.c_str(),
					 CURLFORM_END);
		
		/*
		if (img) {
			curl_formadd(&formpost, &lastptr,
						 CURLFORM_COPYNAME, "file",
						 CURLFORM_COPYCONTENTS, this->img_path.c_str(),
						 CURLFORM_END);
		} else if (file != "") {
			curl_formadd(&formpost, &lastptr,
						 CURLFORM_COPYNAME, "file",
						 CURLFORM_COPYCONTENTS,
						 (this->file_path.string() + file).c_str(),
						 CURLFORM_END);
		}
		*/
		
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, this->agent.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);	   

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			std::cout << "error" << std::endl;
		}
		
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
}

std::string stradian::Slack::read_file(const std::filesystem::path& path) {
	std::stringstream str;

	std::ifstream fp;
	fp.open(path);
	str << fp.rdbuf();
	fp.close();

	return str.str();
}

/* Functions definition */

size_t stradian::write_callback(char* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

#endif // OS dependency

/*
#ifdef __cplusplus
}
#endif
*/
