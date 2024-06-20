/**
 * @file		binance_exchange.cpp
 * @brief		binance exchange class
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-16
 */

#include "stradian/binance_exchange.h"
#include <chrono>

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

stradian::BinanceExchange::BinanceExchange(const std::string& host,
										   const std::string& port,
										   const std::string& target)
	: websocket(host, port, target) {
	this->api_key = BinanceExchange::read_file(this->key_path);
	this->secret_key = BinanceExchange::read_file(this->secret_path);

    if (this->ping()) {
		Logger logger("BinanceExchange: binance exchange connected");
		logger.log(LOGLEVEL::INFO);
	} else {
		Logger logger("BinanceExchange: binance exchange failed to connect");
		logger.log(LOGLEVEL::INFO);
		throw logger;
	}
}

void stradian::BinanceExchange::handler(const Order& order) {
	if (this->ping()) {
		//
		//
		//
		//
		//
		this->update();
		
	} else {
		Logger logger("BinanceExchange: order handle failed");
		logger.log(LOGLEVEL::ERROR);
		throw logger;
	}
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
	
	return signature;
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

bool stradian::BinanceExchange::status(const boost::json::value& res) {
	const auto& code = res.at("status");
	if (code == 200) {
		return true;
	} else {
		{
			Logger logger("BinanceExchange: invalid status code ["
						  + boost::json::serialize(code) + "]");
			logger.log(LOGLEVEL::WARN);
			throw logger;
		}
		
		return false;
	}
}

void stradian::BinanceExchange::limits(const boost::json::value& res) {
	const auto& arr = res.at("rateLimits").as_array();
		
	auto reach_percent = [](int count, int limit, double percent) {
		return count > (limit * percent);
	};
	
	for (auto val : arr) {
		int count = val.at("count").as_int64();
		int limit = val.at("limit").as_int64();

		if (reach_percent(count, limit, 0.9)) {
			Logger logger("BinanceExchange:   ["
						  + std::to_string(count) + "/"
						  + std::to_string(limit) + "]");
			logger.log(LOGLEVEL::WARN);
			
			throw logger;
		} else if (reach_percent(count, limit, 0.7)) {
			Logger logger("BinanceExchange: high ratelimits warning ["
						  + std::to_string(count) + "/"
						  + std::to_string(limit) + "]");
			logger.log(LOGLEVEL::WARN);
		}
	}
}

boost::json::value stradian::BinanceExchange::request_wrapper(const boost::json::value& req) {
	static int trial = 3, sleep_sec = 10;
	
	const auto& res = this->websocket.request(req);
	std::cout << req << std::endl;
	std::cout << res << std::endl;
	for (int i = 0; i < trial; ++i) {
		try {
			this->status(res);
			this->limits(res);

			return std::move(res);
		} catch (stradian::Logger& e) {
			std::this_thread::sleep_for(std::chrono::seconds(sleep_sec));
		}
	}

    Logger logger("BinanceExchange: websocket request fails consistently [" + std::to_string(trial * sleep_sec) + " seconds]" , true);
	logger.log(LOGLEVEL::FATAL);

	throw logger;
}

bool stradian::BinanceExchange::ping(void) {
	boost::json::value req = {
		{"id", stradian::BinanceExchange::get_id()},
		{"method", "ping"}
	};

    auto res = this->request_wrapper(req);

	this->limits(res);
    return this->status(res);
}

void stradian::BinanceExchange::update(void) {
	boost::json::value req = {
		{"id", stradian::BinanceExchange::get_id()},
		{"method", "account.status"},
		{"params", {
				{"apiKey", this->api_key},
				{"timestamp", stradian::BinanceExchange::get_timestamp()}
			}}
	};

	stradian::BinanceExchange::signature(req);

	auto res = this->request_wrapper(req);

	auto& object = res.get_object();

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

double stradian::BinanceExchange::price(Order& order) {
	boost::json::value req = {
		{"id", BinanceExchange::get_id()},
		{"method", "ticker.price"},
		{"params", {
				{"symbol", order.symbol}
			}}
	};

	auto res = this->request_wrapper(req);
	
	std::string dprice = boost::json::serialize(res.at("result").at("price"));
	return std::stod(dprice.substr(1, dprice.size() - 2));
}

void stradian::BinanceExchange::buy(Order& order) {
	std::string type = order.price > 0.0 ? "LIMIT" : "MARKET";
	
	boost::json::value req = {
		{"id", stradian::BinanceExchange::get_id()},
		{"method", "order.test"},
		//{"method", "order.place"},
		{"params", {
				{"symbol", order.symbol},
				{"side", "BUY"},
				{"type", type},
				{"quantity", to_string_with_precision(order.quantity)},
				{"apiKey", this->api_key},
				{"timestamp", stradian::BinanceExchange::get_timestamp()}
			}}
	};

	if (type == "LIMIT") {
		boost::json::object& obj = req.at("params").as_object();
		obj["price"] = to_string_with_precision(order.price);
		obj["timeInForce"] = "GTC";
	}

	stradian::BinanceExchange::signature(req);

	auto res = this->request_wrapper(req);

	std::stringstream str;
    if (type == "LIMIT") {
		str << "BinanceExchange: buy " << order.symbol << " order "
			<< order.quantity << " for " << order.price << " each";
	} else {
	    str << "BinanceExchange: buy " << order.symbol << " order "
			<< order.quantity << " for market price";
	}
    Logger logger(str.str(), true);
	logger.log(LOGLEVEL::INFO);
}

void stradian::BinanceExchange::sell(Order& order) {
	std::string type = order.price > 0.0 ? "LIMIT" : "MARKET";
	
	boost::json::value req = {
		{"id", stradian::BinanceExchange::get_id()},
		{"method", "order.test"},
		//{"method", "order.place"},
		{"params", {
				{"symbol", order.symbol},
				{"side", "SELL"},
				{"type", type},
				{"quantity", to_string_with_precision(order.quantity)},
				{"apiKey", this->api_key},
				{"timestamp", stradian::BinanceExchange::get_timestamp()}
			}}
	};

	if (type == "LIMIT") {
		boost::json::object& obj = req.at("params").as_object();
		obj["price"] = to_string_with_precision(order.price);
		obj["timeInForce"] = "GTC";
	}

	stradian::BinanceExchange::signature(req);

	auto res = this->request_wrapper(req);

	std::stringstream str;
    if (type == "LIMIT") {
		str << "BinanceExchange: sell " << order.symbol << " order "
			<< order.quantity << " for " << order.price << " each";
	} else {
	    str << "BinanceExchange: sell " << order.symbol << " order "
			<< order.quantity << " for market price";
	}
    Logger logger(str.str(), true);
	logger.log(LOGLEVEL::INFO);
	
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
