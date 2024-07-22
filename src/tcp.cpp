/**
 * @file		tcp.cpp
 * @brief       TCP
 * @author	    Jeong Hoon (Sian) Choi
 * @version 	1.0.0
 * @date		2023-11-26
 */

#include "sian/tcp.h"

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

sian::TCP::TCP(const IPv4& addr) {
	bzero((char*)&this->server_addr, sizeof(this->server_addr));

	this->server_addr.sin_family = DOMAIN_TYPE;
	if (addr.type == TYPE::CLIENT) {
		if (inet_pton(DOMAIN_TYPE, addr.ipv4.c_str(), &this->server_addr.sin_addr) <= 0) {
			throw sian::network_error("inet_pton() failed");
		}
	} else {
		this->server_addr.sin_addr.s_addr = INADDR_ANY;
	}
	this->server_addr.sin_port = htons(addr.port);
}

void sian::TCP::socket(void) {
	this->sock_fd=::socket(DOMAIN_TYPE, SOCKET_TYPE, 0);

	if (this->sock_fd == -1) {
		throw sian::network_error("socket() failed");
	}
}

void sian::TCP::bind(void) {
	if (::bind(this->sock_fd,
			   (struct sockaddr*)&this->server_addr,
			   sizeof(this->server_addr)) == -1) {
		throw sian::network_error("bind() failed");
	}
}

void sian::TCP::listen(int nclnt) {
	if(::listen(this->sock_fd, nclnt) == -1) {
		throw sian::network_error("listen() failed");
	}
}

void sian::TCP::accept(tcp_client& thrclnt) {
	int clnt_addr_size = sizeof(thrclnt.addr);
	
	thrclnt.sock_fd = ::accept(this->sock_fd,
							   (struct sockaddr*)&thrclnt.addr,
							   (socklen_t*)&clnt_addr_size);
	
	if (thrclnt.sock_fd == -1) {
		throw network_error("accept() failed");
	}
}

void sian::TCP::connect(void) {
	if (::connect(
		   this->sock_fd,
		   (struct sockaddr*)&this->server_addr,
		   sizeof(this->server_addr)
		   )) {
		throw sian::network_error("connect() failed");
	}
}

void sian::TCP::send(const std::string& msg,
					 tcp_client* clnt) const {
	if (clnt == nullptr) {
		::send(this->sock_fd, msg.c_str(), msg.size(), 0);
	} else {
		::send(clnt->sock_fd, msg.c_str(), msg.size(), 0);
	}
}

int sian::TCP::recv(std::string& msg,
					tcp_client* clnt) const {
	char buffer[PACKET_SIZE];
	
	ssize_t rcv;
	if (clnt == nullptr) {
		rcv = ::recv(this->sock_fd, buffer, sizeof(buffer)-1,
					 MSG_DONTWAIT);
	} else {
		rcv = ::recv(clnt->sock_fd, buffer, sizeof(buffer)-1,
					 MSG_DONTWAIT);
	}

	if (rcv <= 0) return rcv;

	buffer[rcv] = '\0';
	msg = buffer;
	return 1;
}

void sian::TCP::close(void) {
	::close(this->sock_fd);
}

void sian::TCP::close(tcp_client& clnt) {
	::close(clnt.sock_fd);
}

#endif // OS dependency

/*
#ifdef __cplusplus
}
#endif
*/
