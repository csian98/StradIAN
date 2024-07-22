/**
 * @file		system_manager.cpp
 * @brief		SystemManager class for StradIAN
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-26
 */

#include "stradian/system_manager.h"

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

unsigned int stradian::SystemManager::uid = 0;

stradian::SystemManager::SystemManager(void)
	: systemDB("system") {
	this->systemDB.create_tables();
	
	SystemManager::uid = this->update_uid();
}

std::pair<bool, bool>
stradian::SystemManager::login(const std::string& user,
							   const std::string& pswd) {
	bool logined = false, sudo = false;
	
	std::string sql =
		"SELECT user, passwd, auth FROM users WHERE user = " + user;
	MariaDB::RESULT_TYPE result;

	this->systemDB.query(sql, result);

	if (result->isNull(1)) {
		logined = false;
	} else {
		if (result->getString(2) == SystemManager::sha256(pswd)) {
			logined = true;
			sudo = result->getBoolean(3);
		} else {
			logined = false;
		}
	}

	delete result;
	return std::make_pair(logined, sudo);
}

unsigned int stradian::SystemManager::update_uid(void) {
	int uid = 0;
	std::string sql = "SELECT MAX(uid) FROM users;";
	
	MariaDB::RESULT_TYPE result;
	
	this->systemDB.query(sql, result);
	
    if (result->isNull(1)) {
		uid = 1;
	} else {
		uid = result->getInt(1) + 1;
	}
	
	delete result;
	return uid;
}

void stradian::SystemManager::add_user(
	const std::string& user, const std::string& passwd,
	bool auth, double usd,
	std::optional<std::string_view> phone,
	std::optional<std::string_view> email,
	std::optional<std::string_view> slack_id) {

	double share = this->update_others_share(usd);

	std::stringstream ss;
	ss << "INSERT INTO users (uid, auth, user, passwd, share";
	if (phone)
		ss << ", phone";
	if (email)
		ss << ", email";
	if (slack_id)
		ss << ", slack_id";

	ss << ") VALUES ('"
	   << SystemManager::uid++ << "', '" << auth << "', '"
	   << user << "', '" << SystemManager::sha256(passwd) << "', '" << share << "'";

	if (phone)
		ss << ", '" << phone.value() <<"'";
	if (email)
		ss << ", '" << email.value() <<"'";
	if(slack_id)
		ss << ", '" << slack_id.value() <<"'";

	ss << ");";

	Logger logger("SystemManager: new user " + user + " with $"
				 + std::to_string(usd), true);
	logger.log(LOGLEVEL::INFO);

	this->systemDB.query(ss.str());
}

void stradian::SystemManager::update_user_info(
	const unsigned int uid,
	const std::string& attribute,
	const std::optional<std::string_view> value) {
	std::stringstream ss;
	ss << "UPDATE users SET " << attribute << " = ";
	
	if (value)
		ss << "'NULL'";
	else
		ss << "'" << value.value() << "'";

	ss << " WHERE uid = " << uid <<";";

	this->systemDB.query(ss.str());
}

double stradian::SystemManager::update_others_share(double usd) {
	auto [key, other] = this->get_validation();
	double pri_total = key + other;
	double new_total = key + other + usd;

	std::map<int, double> shares;

	std::string sql = "SELECT uid, share FROM users;";
	MariaDB::RESULT_TYPE result = nullptr;
	
    systemDB.query(sql, result);
	do {
		shares.insert(std::make_pair(result->getInt(1), result->getDouble(2)));
	} while (result->next());

	delete result;

    for (auto iter = shares.begin(); iter != shares.end(); ++iter) {
		std::stringstream ss;
		ss << "UPDATE users SET share = '"
		   << (iter->second * pri_total / new_total)
		   << "' WHERE uid = " << iter->first <<";";
		this->systemDB.query(ss.str());
	}

	return usd / new_total;
}

void stradian::SystemManager::adjust_share(const unsigned int uid,
										   const double new_share) {
	std::map<int, double> shares;
	double remain_share = 0.0;
	double new_remain_share = 1.0 - new_share;
	
	std::string sql = "SELECT uid, share FROM users;";
	MariaDB::RESULT_TYPE result = nullptr;

	systemDB.query(sql, result);
	do {
		unsigned int i_uid = result->getInt(1);
		double i_share = result->getDouble(2);
		
		shares.insert(std::make_pair(i_uid, i_share));
		if (i_uid == uid)
			remain_share = 1.0 - i_share;
	} while (result->next());

	delete result;

	for (auto iter = shares.begin(); iter != shares.end(); ++iter) {
		std::stringstream ss;
		ss << "UPDATE users SET share = '"
		   << (iter->second * new_remain_share / remain_share)
		   << "' WHERE uid = " << iter->first <<";";
		this->systemDB.query(ss.str());
	}
}

void stradian::SystemManager::update_balance(void) {
	auto [key, other] = this->get_validation();
	
	std::stringstream ss;
	ss << "INSERT IGNORE INTO balance (date, USD, cash_ratio) VALUES ('"
	   << MariaDB::date() << "', '"
	   << (key + other) << "', '"
	   << key / (key + other) << "');";

	this->systemDB.query(ss.str());
}

void stradian::SystemManager::write_report(REPORT_TYPE report_type) {
	std::stringstream ss;
	ss <<
		"SELECT USD FROM balance WHERE date = (SELECT MAX(date) FROM balance)";

	MariaDB::RESULT_TYPE result;
	this->systemDB.query(ss.str(), result);

	double current_usd = result->getDouble(1);
	delete result;

	ss.clear();
	ss << "SELECT USD FROM balance WHERE date = "
	   << "DATE_ADD((SELECT MAX(date) FROM balance), "
	   << "INTERVAL -1 ";
	
	switch (report_type) {
	case REPORT_TYPE::DAILY:
		ss << "DAY";
		break;
	case REPORT_TYPE::WEEKLY:
		ss << "WEEK";
		break;
	case REPORT_TYPE::MONTLY:
		ss << "MONTH";
		break;
	case REPORT_TYPE::QUARTER:
		ss << "QUARTER";
		break;
	}

	ss << ");";
	
	this->systemDB.query(ss.str(), result);

	if (result->isNull(1)) {
		Logger logger("SystemManager: cannot report without enough balance data");
		logger.log(LOGLEVEL::WARN);

		delete result;
	} else {
		double past_usd = result->getDouble(1);

		delete result;

		Reporter reporter(current_usd, past_usd, report_type);
		reporter.report();
	}
}

void stradian::SystemManager::update_market(MARKETCODE code) {
	auto symbols = this->get_symbols(code);
	
	std::string table_name = to_string(code) + "_market";

	for (auto symbol : symbols) {
		std::stringstream ss;
		ss << "INSERT IGNORE INTO " << table_name << " (symbol) "
		   << "VALUES ('" << symbol << "');";
		this->systemDB.query(ss.str());
	}
}

void stradian::SystemManager::open_symbol(
	MARKETCODE code, const std::string& symbol) {
	std::string table_name = to_string(code) + "_market";

	std::stringstream ss;
	ss << "UPDATE " << table_name << " SET trade = 'true' "
	   << "WHERE symbol = '" << symbol << "';";
	this->systemDB.query(ss.str());
}

void stradian::SystemManager::close_symbol(
	MARKETCODE code, const std::string& symbol) {
	std::string table_name = to_string(code) + "_market";

	std::stringstream ss;
	ss << "UPDATE " << table_name << " SET trade = 'false' "
	   << "WHERE symbol = '" << symbol << "';";
	this->systemDB.query(ss.str());
}

std::string stradian::SystemManager::sha256(const std::string& input) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256ctx;
	SHA256_Init(&sha256ctx);
	SHA256_Update(&sha256ctx, input.c_str(), input.size());
	SHA256_Final(hash, &sha256ctx);

	std::stringstream ss;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		ss << std::hex << std::setw(2) << std::setfill('0')
		   << (int)hash[i];
	}
	return ss.str();
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
