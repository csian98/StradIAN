/**
 * @file		mariadb.cpp
 * @brief		mariadb class for market
 * @author		Jeong Hoon (Sian) Choi
 * @version		1.0.0
 * @date		2024-06-22
 */

#include "stradian/mariadb.h"

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

stradian::MariaDB::MariaDB(const std::string& db)
	: db(db), json_path("etc/json/" + db) {
    this->user = MariaDB::read_file(user_path);
	this->password = MariaDB::read_file(passwd_path);
	
	this->connect();
}

stradian::MariaDB::~MariaDB(void) noexcept {
    this->disconnect();
}

void stradian::MariaDB::query(const std::string& sql) {
	std::unique_ptr<sql::Statement> statement(
		this->conn->createStatement());

	try {
		statement->executeUpdate(sql);
	} catch (sql::SQLException& e) {
		Logger logger("MariaDB: " + std::string(e.what()), true);
		logger.log(LOGLEVEL::WARN);

		throw logger;
	}
}

void stradian::MariaDB::query(const std::string& sql,
							  sql::ResultSet*& result) {
	std::unique_ptr<sql::Statement> statement(
		this->conn->createStatement());

	try {
		result = statement->executeQuery(sql);
		result->next();
	} catch (sql::SQLException& e) {
		Logger logger("MariaDB: " + std::string(e.what()), true);
		logger.log(LOGLEVEL::WARN);

		throw logger;
	}
}

void stradian::MariaDB::create_tables(void) {
	for (std::filesystem::directory_iterator iter(this->json_path);
		 iter != std::filesystem::end(iter); ++iter) {
		const std::filesystem::directory_entry& entry = *iter;

		if (entry.is_regular_file() &&
			entry.path().extension() == ".json") {
			this->create_table(entry.path());
		}
	}
}

void stradian::MariaDB::alter_table(const std::string& json_file) {
	std::filesystem::path path = this->json_path / json_file;
	std::filesystem::directory_entry entry(path);

	if (entry.is_regular_file() &&
		entry.path().extension() == ".json") {
		std::ifstream in(entry.path(), std::ios_base::in);
		std::stringstream ss;
		ss << in.rdbuf();
		in.close();

		boost::json::value json = boost::json::parse(ss.str());
		if (json.at("is_format").as_bool()) {
			std::string created_query = MariaDB::alter_table_query(json);
			this->query(created_query);
		}
		
	}
}

const std::string stradian::MariaDB::date(void) {
	auto now = std::chrono::system_clock::now();
	std::time_t t_now = std::chrono::system_clock::to_time_t(now);
	tm* t_tm = std::localtime(&t_now);

	char buffer[64];
	std::strftime(buffer, sizeof(buffer),
				  "%Y-%m-%d", t_tm);
	return std::string(buffer);
}

const std::string stradian::MariaDB::date_time(void) {
	auto now = std::chrono::system_clock::now();
	std::time_t t_now = std::chrono::system_clock::to_time_t(now);
	tm* t_tm = std::localtime(&t_now);

	char buffer[64];
	std::strftime(buffer, sizeof(buffer),
				  "%Y-%m-%d %H:%M:%S", t_tm);
	return std::string(buffer);
}

const std::string timestamp(void) {
	auto now = std::chrono::system_clock::now();
	return
		std::to_string(
			std::chrono::duration_cast<std::chrono::seconds>(
				now.time_since_epoch()).count()
			);
}

void stradian::MariaDB::connect(void) {
	try {
		sql::Driver* driver = sql::mariadb::get_driver_instance();
		sql::SQLString url(
			"jdbc:mariadb://" + this->host + ":" +
			std::to_string(this->port) + "/" + this->db
			);
		sql::Properties properties({
				{"user", this->user},
				{"password", this->password}
			});
		
		std::unique_ptr<sql::Connection> conn(
			driver->connect(url, properties));
		
		this->conn = std::move(conn);
	} catch (sql::SQLException& e) {
		Logger logger("MariaDB: " + std::string(e.what()), true);
		logger.log(LOGLEVEL::FATAL);

		throw logger;
	}
}

void stradian::MariaDB::disconnect(void) {
	try {
		this->conn->close();
	} catch (sql::SQLException& e) {
		Logger logger("MariaDB: " + std::string(e.what()), true);
		logger.log(LOGLEVEL::FATAL);

		throw logger;
	}
}

void stradian::MariaDB::create_table(const std::filesystem::path& path) {
	std::ifstream in(path, std::ios_base::in);
	std::stringstream ss;
	ss << in.rdbuf();
	in.close();

	boost::json::value json = boost::json::parse(ss.str());
	if (json.at("is_format").as_bool()) {
		std::string created_query = MariaDB::create_table_query(json);
		this->query(created_query);
	}
}

std::string stradian::MariaDB::read_file(
	const std::filesystem::path& path) {
	std::stringstream str;

	std::ifstream fp;
	fp.open(path);
	str << fp.rdbuf();
	fp.close();

	return str.str();
}

std::string stradian::MariaDB::create_table_query(
	const boost::json::value& json) {
	std::stringstream create_query;
	std::string table_name = boost::json::serialize(json.at("name"));
	table_name = table_name.substr(1, table_name.size() - 2);
	create_query << "CREATE TABLE IF NOT EXISTS " << table_name << " (";
	
	for (const auto& ptr :  json.at("attributes").as_object()) {
		std::string value = boost::json::serialize(ptr.value());
		value = value.substr(1, value.size() - 2);
		create_query << ptr.key() << " " << value << ", ";
	}

	create_query << "PRIMARY KEY (";
    for (const auto& ptr: json.at("primary").as_array()) {
		std::string pk = boost::json::serialize(ptr);
		pk = pk.substr(1, pk.size() - 2);
	    create_query << pk << ", ";
	}

	std::string sql = create_query.str();
	sql = sql.substr(0, sql.size() - 2) + "));";
	return sql;
}

std::string stradian::MariaDB::alter_table_query(
	const boost::json::value& json) {
	std::stringstream create_query;
	std::string table_name = boost::json::serialize(json.at("name"));
	table_name = table_name.substr(1, table_name.size() - 2);
	create_query << "ALTER TABLE " << table_name << " (";

	for (const auto& ptr :  json.at("attributes").as_object()) {
		std::string value = boost::json::serialize(ptr.value());
		value = value.substr(1, value.size() - 2);
		create_query << ptr.key() << " " << value << ", ";
	}

	create_query << "PRIMARY KEY (";
    for (const auto& ptr: json.at("primary").as_array()) {
		std::string pk = boost::json::serialize(ptr);
		pk = pk.substr(1, pk.size() - 2);
	    create_query << pk << ", ";
	}

	std::string sql = create_query.str();
	sql = sql.substr(0, sql.size() - 2) + "));";
	return sql;
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
