/**
 * @file		c2c_server.cpp
 * @brief		c2cServer class for StradIAN
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-07-14
 */

#include "stradian/c2c_server.h"

/* C & CPP */
/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#if _TARGET_OS == OS_WINDOWS

#elif _TARGET_OS == OS_LINUX

/* Definition */

std::string stradian::client::prompt = "~ >>";

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

stradian::c2cServer::c2cServer(void) {
	sian::IPv4 ipv4(sian::TYPE::SERVER,
					c2c::ipv4,
					c2c::port);
	
	if (!std::filesystem::exists(this->certificate_path) ||
		!std::filesystem::exists(this->prvkey_path)) {
		Logger logger("c2cServer: ssl file not exists");
		logger.log(LOGLEVEL::FATAL);
		throw logger;
	}
	
	this->ssl = new sian::SSL(ipv4,
							  this->certificate_path.c_str(),
							  this->prvkey_path.c_str());
	
	this->ssl->socket();

	this->ssl->bind();

	this->ssl->listen(this->max_client);
}

stradian::c2cServer::~c2cServer(void) noexcept {
	for (client cl : this->clients) {
		this->ssl->close(*cl.ssl_client);

		delete cl.ssl_client;
	}
	
	this->ssl->close();
	delete this->ssl;
}

void stradian::c2cServer::start(void) {
	this->status = true;
	
	std::thread acceptor(&c2cServer::thread_accept, this);
	std::thread reader(&c2cServer::thread_read, this);

	acceptor.detach();
    reader.detach();
}

void stradian::c2cServer::stop(void) {
	this->status = false;
}

stradian::c2cServer::operator bool(void) const {
	return this->status;
}

void stradian::c2cServer::thread_accept(void) {
	sian::ssl_client* client = nullptr;
	while (true) {
		client = new sian::ssl_client;
		this->ssl->accept(*client);

		this->send_motd(client);

		std::thread logger(&c2cServer::thread_login, this, client);
		logger.detach();
	}
}

void stradian::c2cServer::thread_read(void) {
	int rcv = 0;

	while (true) {
		this->mtx.lock();
		
		for (const client& cl : this->clients) {
			std::string buffer;
			rcv = this->ssl->recv(buffer, cl.ssl_client);
			if (rcv > 0) {
				
				//

				std::string prompt = "[";
				if (cl.sudo)
					prompt += "sudo#";
				prompt += cl.user;
				prompt += "] ";
				prompt += client::prompt;

				this->ssl->send(prompt);
			}
		}
		
		this->mtx.unlock();
	}
}

void stradian::c2cServer::thread_login(sian::ssl_client* ssl_clnt) {
	std::string user, pswd;
	client cl;
	cl.ssl_client = ssl_clnt;
	for (int i = 0; i < this->login_trial; ++i) {
		this->ssl->send("\nLogin StradIAN\n");
	    this->ssl->send("id: ", cl.ssl_client);
		this->ssl->recv(user, cl.ssl_client);

		this->ssl->send("pw: ", cl.ssl_client);
		this->ssl->recv(pswd, cl.ssl_client);

		auto [logined, sudo] = this->login(user, pswd);
		if (!logined) {
			this->ssl->send("Login Failed\n");
		} else {
			cl.logined = logined;
			cl.sudo = sudo;

			cl.user = user;

			this->mtx.lock();
			this->clients.push_back(cl);
			this->mtx.unlock();

			return;
		}
	}

    this->disconnect_client(ssl_clnt);
	
	return;
}

void stradian::c2cServer::disconnect_client(sian::ssl_client* ssl_clnt) {
	this->ssl->close(*ssl_clnt);
	
	this->mtx.lock();

	this->clients.erase(std::remove_if(
		this->clients.begin(),
		this->clients.end(),
		[ssl_clnt](client& ocl) {
			return ocl.ssl_client == ssl_clnt;
		}));

	this->mtx.unlock();
}

void stradian::c2cServer::send_motd(sian::ssl_client* client) const {
	std::stringstream motd;
	std::ifstream fp(this->motd_path);
	motd << fp.rdbuf();
	this->ssl->send(motd.str(), client);
}

/* Functions definition */

#endif // OS dependency

/*
#ifdef __cplusplus
}
#endif
*/
