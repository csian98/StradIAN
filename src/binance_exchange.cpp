/**
 * @file		binance_exchange.cpp
 * @brief		binance exchange class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-16
 */

#include <boost/json/src.hpp>
#include <chrono>
#include "stradian/binance_exchange.h"

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

unsigned long stradian::BinanceExchange::id = 0;

stradian::BinanceExchange::BinanceExchange(void)
	: ctx(boost::asio::ssl::context::sslv23_client), ws(ioc, ctx) {
	this->ctx.set_default_verify_paths();

	this->api_key = BinanceExchange::read_file(this->key_path);
	this->secret_key = BinanceExchange::read_file(this->secret_path);
	//
	this->connect();
	
	this->update();
}



stradian::BinanceExchange::~BinanceExchange(void) noexcept {
	this->disconnect();
}

void stradian::BinanceExchange::connect(void) {
	boost::asio::ip::tcp::resolver resolver(this->ioc);
	
	auto const results = resolver.resolve(this->host, this->port);
	
	auto& raw = boost::beast::get_lowest_layer(this->ws);
	auto ep = boost::asio::connect(raw, results);

	if (!SSL_set_tlsext_host_name(this->ws.next_layer().native_handle(), host.c_str())) {
		throw boost::system::system_error(
			::ERR_get_error(), boost::asio::error::get_ssl_category()
			);
	}
	this->ws.next_layer().handshake(boost::asio::ssl::stream_base::client);

	this->ws.set_option(
		boost::beast::websocket::stream_base::decorator(
			[](boost::beast::websocket::request_type& req) {
				req.set(boost::beast::http::field::user_agent,
						std::string(BOOST_BEAST_VERSION_STRING) +
						" websocket-client-coro");
			})
		);
	this->ws.handshake(this->host + ":" + std::to_string(ep.port()), this->target);
}

void stradian::BinanceExchange::disconnect(void) {
	this->ws.close(
		boost::beast::websocket::close_code::normal
		);
}

void stradian::BinanceExchange::update(void) {
	boost::json::value doc = {
		{"id", stradian::BinanceExchange::get_id()},
		{"method", "account.status"},
		{"params", {
				{"apiKey", this->api_key},
				{"timestamp", stradian::BinanceExchange::get_timestamp()}
			}}
	};

	stradian::BinanceExchange::signature(doc);

	auto result = this->send(doc);

	auto& object = result.get_object();

	auto unquote = [](std::string str) {
		return str.substr(1, str.size() - 2);
	};
	
	for (const auto& ptr : object.at("result").at("balances").as_array()) {
		auto& object = ptr.as_object();
		std::string symbol =
			unquote(boost::json::serialize(object.at("asset")));
		double free = std::stod(
			unquote(boost::json::serialize(object.at("free"))));
		double locked = std::stod(
			unquote(boost::json::serialize(object.at("locked"))));
		
		if (free + locked > 0.0) {
			this->assets.insert(std::make_pair(symbol,
											   std::make_pair(free, locked)));
		}
	}
}

void stradian::BinanceExchange::buy(Order& order) {
	auto [symbol, quantity] = order.get_item();
	
    
}

void stradian::BinanceExchange::sell(Order& order) {
	auto [symbol, qunatity] = order.get_item();
	
}

void stradian::BinanceExchange::handler(const Order& order) {
	
}

boost::json::value stradian::BinanceExchange::send(const boost::json::value& doc) {
	this->ws.write(boost::asio::buffer(boost::json::serialize(doc)));

	boost::beast::flat_buffer buffer;
	this->ws.read(buffer);

	std::string data(boost::asio::buffers_begin(buffer.data()),
					 boost::asio::buffers_end(buffer.data()));

	return boost::json::parse(data);
}

bool stradian::BinanceExchange::ping(void) {
	boost::json::value doc = {
		{"id", stradian::BinanceExchange::get_id()},
		{"method", "ping"}
	};

    auto result = this->send(doc);

	if (result.at("status") == 200) {
		return true;
	} else {
		return false;
	}
}

std::string stradian::BinanceExchange::read_file(const std::filesystem::path& path) {
	std::stringstream str;
	
	std::ifstream fp;
	fp.open(path);
    str << fp.rdbuf();
	fp.close();

	return str.str();
}

std::string stradian::BinanceExchange::get_id(void) {
	std::stringstream str;
	stradian::BinanceExchange::id++;
	str << "stradian" << std::setfill('0') << std::setw(8) << stradian::BinanceExchange::id;
	return str.str();
}

std::string stradian::BinanceExchange::get_timestamp(void) {
	long long ms_since_epoch = std::chrono::duration_cast<
		std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	return std::to_string(ms_since_epoch);
}

void stradian::BinanceExchange::signature(boost::json::value& json) const {
	auto& object = json.get_object();

	std::map<std::string, std::string> params;
	for (const auto& ptr : object.at("params").as_object()) {
		params.insert(std::make_pair(ptr.key(),
									 boost::json::serialize(ptr.value())));
	}

	std::string payload;
	for (auto iter = params.begin(); iter != params.end(); ++iter) {
		payload += iter->first + "=" + iter->second.substr(1, iter->second.size() - 2) + "&";
	}
    payload.pop_back();
	
	std::string signature = encryptHMAC(this->secret_key.c_str(), payload.c_str());

    object.at("params").as_object()["signature"] = signature;
	std::cout << boost::json::serialize(json);
}

std::string stradian::BinanceExchange::encryptHMAC(const char* key, const char* data) const {
	unsigned char* result;
	static char res_hexstring[64];
	int result_len = 32;
	std::string signature;

	result = HMAC(EVP_sha256(), key, strlen((char*)key),
				  const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(data)),
				  strlen((char*)data), NULL, NULL);

	for (int i = 0; i < result_len; ++i) {
		sprintf(&(res_hexstring[i * 2]), "%02x", result[i]);
	}

	for (int i = 0; i < 64; ++i) {
		signature += res_hexstring[i];
	}
	std::cout << signature << std::endl;
	return signature;
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
