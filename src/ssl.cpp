/**
 * @file		ssl.cpp
 * @brief       SSL
 * @author	    Jeong Hoon (Sian) Choi
 * @version	    1.0.0
 * @date		2024-04-30
 */

#include "sian/ssl.h"

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

sian::SSL::SSL(const IPv4& ipv4,
			   std::string_view certificate,
			   std::string_view privatekey)
	: TCP(ipv4), certificate(certificate), privatekey(privatekey) {
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
}

bool sian::SSL::ssl_auth(SSL_CTX* ctx) {
	if (SSL_CTX_use_certificate_file(ctx, this->certificate.c_str(),
									 SSL_FILETYPE_PEM) <= 0) {
		throw network_error("SSL_CTX_use_certificate_file() failed");
		return false;
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, this->privatekey.c_str(),
									SSL_FILETYPE_PEM) <= 0) {
		throw network_error("SSL_CTX_use_PrivateKey_file() failed");
		return false;
	}

	return true;
}

void sian::SSL::accept(ssl_client& clnt) {
	TCP::accept(clnt);
	
	this->method = SSLv23_server_method();
	
	clnt.ctx = SSL_CTX_new(this->method);

	if (clnt.ctx == NULL) {
		throw network_error("SSL_CTX_new() failed");
	}

	if (!this->ssl_auth(clnt.ctx)) {
		throw network_error("sslAuth() failed");

		return;
	}

	clnt.ssl = SSL_new(clnt.ctx);
	SSL_set_fd(clnt.ssl, clnt.sock_fd);

	if (SSL_accept(clnt.ssl) <= 0) {
		throw network_error("SSL_accept() failed");
	}
}

void sian::SSL::connect(void) {
	TCP::connect();
	this->method = SSLv23_client_method();
	
	this->ctx = SSL_CTX_new(this->method);

	if (this->ctx == NULL) {
		throw network_error("SSL_CTX_new() failed");
	}

	this->ssl = SSL_new(this->ctx);
	SSL_set_fd(this->ssl, this->sock_fd);

	if (SSL_connect(this->ssl) <= 0) {
		throw network_error("SSL_connect() failed");
	}
}

void sian::SSL::send(const std::string& msg,
					 ssl_client* clnt) const {
	if (clnt == nullptr) {
		SSL_write(this->ssl, msg.c_str(), msg.size());
	} else {
		SSL_write(clnt->ssl, msg.c_str(), msg.size());
	}
}

int sian::SSL::recv(std::string& msg,
					ssl_client* clnt) const {
	char buffer[PACKET_SIZE];
	memset(buffer, 0, sizeof(buffer));
	ssize_t rcv;
	
  if (clnt == nullptr) {
		rcv = SSL_read(this->ssl, buffer, sizeof(buffer));
	} else {
		rcv = SSL_read(clnt->ssl, buffer, sizeof(buffer));
	}

	if (rcv <= 0) {
		return rcv;
	}
	msg = buffer;
	return 1;
}

void sian::SSL::close(void) {
	SSL_shutdown(this->ssl);
	SSL_free(this->ssl);
	SSL_CTX_free(ctx);

	::close(this->sock_fd);
}

void sian::SSL::close(ssl_client& clnt) {
	SSL_free(clnt.ssl);
	SSL_CTX_free(clnt.ctx);

	::close(clnt.sock_fd);
}

/* Functions declare */

/*

void swap(Sample& src, Sample& dst) noexcept {
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
