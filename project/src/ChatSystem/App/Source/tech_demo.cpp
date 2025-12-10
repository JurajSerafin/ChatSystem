#include <iostream>

#include <wx/wx.h>
#include <wx/version.h>

#include <boost/version.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <openssl/ssl.h>
#include <openssl/opensslv.h>

#include <pqxx/pqxx>

#include <sqlite3.h>

#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "tech_demo.hpp"

void TechDemo::print_separator(const std::string& title)
{
	std::cout << "\n========================================\n";
	std::cout << title << "\n";
	std::cout << "========================================\n";
}

void TechDemo::test_wxwidgets()
{
	print_separator("Testing wxWidgets");

	std::cout << "wxWidgets Version: " << reinterpret_cast<const char*>(wxVERSION_STRING) << "\n";

	if (wxInitialize()) {
		std::cout << "✓ wxWidgets runtime initialized successfully\n";
		wxUninitialize();
	}
	else {
		std::cout << "✗ wxWidgets failed to initialize\n";
	}
}


void TechDemo::test_boost()
{
	print_separator("Testing Boost");

	try {
		boost::asio::io_context io;
		std::cout << "✓ Boost.Asio: IO context created\n";
	}
	catch (const std::exception& e) {
		std::cout << "✗ Boost.Asio failed: " << e.what() << "\n";
	}

	try {
		bool thread_ran = false;
		boost::thread worker([&thread_ran]() {
			thread_ran = true;
		});
		worker.join();
		if (thread_ran) {
			std::cout << "✓ Boost.Thread: Thread executed successfully\n";
		}
	}
	catch (const std::exception& e) {
		std::cout << "✗ Boost.Thread failed: " << e.what() << "\n";
	}
}

void TechDemo::test_openssl()
{
	print_separator("Testing OpenSSL");
	std::cout << "OpenSSL Version: " << OPENSSL_VERSION_TEXT << "\n";

	SSL_library_init();
	const SSL_METHOD* method = TLS_client_method();

	if (SSL_CTX* ctx = SSL_CTX_new(method)) {
		std::cout << "✓ OpenSSL: SSL context created\n";
		SSL_CTX_free(ctx);
	}
	else {
		std::cout << "✗ OpenSSL: Failed to create SSL context\n";
	}
}

void TechDemo::test_postgresql()
{
	print_separator("Testing PostgreSQL (libpqxx)");
	std::cout << "libpqxx Version: " << PQXX_VERSION << "\n";

	try {
		pqxx::connection dummy_connection{ "" };
		pqxx::work dummy_transaction{ dummy_connection, nullptr };

		std::cout << "✓ libpqxx: Core classes constructable\n";
	}
	catch (const std::exception& e) {
		std::cout << "✗ libpqxx object test failed: " << e.what() << "\n";
	}
}



void TechDemo::test_sqlite()
{
	print_separator("Testing SQLite");
	std::cout << "SQLite Version: " << sqlite3_libversion() << "\n";

	sqlite3* database = nullptr;

	if (int reaction = sqlite3_open(":memory:", &database); reaction == SQLITE_OK) {
		std::cout << "✓ SQLite: In-memory database opened\n";

		auto query = "CREATE TABLE test (id INTEGER PRIMARY KEY, name TEXT);";
		char* err_msg = nullptr;
		reaction = sqlite3_exec(database, query, nullptr, nullptr, &err_msg);

		if (reaction == SQLITE_OK) {
			std::cout << "✓ SQLite: Table created successfully\n";

			query = "INSERT INTO test (name) VALUES ('Test');";
			reaction = sqlite3_exec(database, query, nullptr, nullptr, &err_msg);

			if (reaction == SQLITE_OK) {
				std::cout << "✓ SQLite: Data inserted successfully\n";

				query = "SELECT COUNT(*) FROM test;";
				sqlite3_stmt* stmt;
				reaction = sqlite3_prepare_v2(database, query, -1, &stmt, nullptr);

				if (reaction == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
					int count = sqlite3_column_int(stmt, 0);
					std::cout << "✓ SQLite: Query executed, row count: " << count << "\n";
				}
				sqlite3_finalize(stmt);
			}
		}

		if (err_msg) {
			std::cout << "✗ SQLite error: " << err_msg << "\n";
			sqlite3_free(err_msg);
		}

		sqlite3_close(database);
	}
	else {
		std::cout << "✗ SQLite: Failed to open database\n";
	}
}

void TechDemo::test_json()
{
	print_separator("Testing nlohmann/json");

	try {
		const nlohmann::json json_array = {
			{"name", "ChatSystem"},
			{"version", "1.0.0"},
			{"features", {
				"chat", "authentication", "encryption"}}
		};

		std::cout << "✓ JSON: Object created\n";
		std::cout << "  Content: " << json_array.dump(2) << "\n";

		std::string json_str = R"(
			{
				"test": "value",
				"number": 42
			}
		)";

		if (auto parsed = nlohmann::json::parse(json_str); parsed["test"] == "value" && parsed["number"] == 42) {
			std::cout << "✓ JSON: Parsing and access working\n";
		}
	}
	catch (const std::exception& e) {
		std::cout << "✗ JSON failed: " << e.what() << "\n";
	}
}

void TechDemo::test_spdlog()
{
	print_separator("Testing spdlog");

	try {
		auto console = spdlog::stdout_color_mt("console");

		console->info("Testing spdlog logger");
		console->warn("This is a warning");
		console->error("This is an error (test only)");

		std::cout << "✓ spdlog: Logging working!" << "\n";

		spdlog::drop("console");
	}
	catch (const std::exception& e) {
		std::cout << "✗ spdlog failed: " << e.what() << "\n";
	}
}

void TechDemo::run_all()
{
	print_separator("Checking all libraries usability: ");

	test_wxwidgets();
	test_boost();
	test_json();
	test_openssl();
	test_postgresql();
	test_sqlite();
	test_spdlog();
}
