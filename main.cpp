#include <iostream>

// sqlite3 c++ wrapper
#include <SQLite3Cpp/SQLite3Cpp.h>

// Command line parser
#include <cmdparser/cmdparser.hpp>

// FTP winapi
#include <Windows.h>
#include <wininet.h>
#pragma comment(lib, "Wininet")

int main(int argc, char *argv[])
{
	// Setupe cmd args
	cli::Parser parser(argc, argv);
	parser.set_required<std::string>("d", "db", "Database name.", true);
	parser.set_required<std::string>("hh", "host", "FTP server host.", true);
	parser.set_required<int>("pp", "port", "FTP server port.", true);
	parser.set_required<std::string>("u", "username", "FTP server username.", true);
	parser.set_required<std::string>("p", "password", "FTP server password.", true);
	parser.run_and_exit_if_error();

	// Open database
	std::string dbName = parser.get<std::string>("d");
    SQLite3Cpp sqlite;
    std::cout << "Opening database " << dbName << "... ";
    bool ret = sqlite.Open(dbName);
    if (ret) std::cout << "OK\n";
    else exit(1);

	// Create table top_scorers
    std::string tableName = "top_scorers";
    std::cout << "Creating table " << tableName << "... ";
    std::string sql = std::string("CREATE TABLE IF NOT EXISTS top_scorers(") +
        "id         INTEGER PRIMARY KEY AUTOINCREMENT    NOT NULL, " +
        "first_name CHAR(50)            NOT NULL, " +
        "last_name  CHAR(50)            NOT NULL, " +
        "num_goals  INT                 DEFAULT 0);";

    ret = sqlite.Exec(sql);
    if (ret) std::cout << "OK\n";
    else exit(1);

	// Insert dummy data
    std::cout << "Inserting data... ";
    for (int i = 0; i < 100; ++i)
    {
        std::string sql = std::string("INSERT INTO top_scorers(first_name, last_name, num_goals) VALUES('")
            + "Firtsname" + std::to_string(i) + "', '"
            + "Lastname" + std::to_string(i) + "', "
            + std::to_string(i) + ");";

        ret = sqlite.Exec(sql);
        if (!ret) exit(1);
    }
    std::cout << "OK\n";

	// Dump database
    std::string outFile = dbName + ".sql";
    std::cout << "Dumping data " << outFile << "... ";
    ret = sqlite.Dump(outFile);
    if (ret)
    {
        std::cout << "OK\n";
    }
    else
    {
        std::cerr << "FAIL\n";
        exit(1);
    }

	// Upload database file to ftp server
	std::string host = parser.get<std::string>("hh");
	int port = parser.get<int>("pp");
	std::string username = parser.get<std::string>("u");
	std::string password = parser.get<std::string>("p");
	HINTERNET hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	HINTERNET hFtpSession = InternetConnect(hInternet, host.c_str(), port, username.c_str(), password.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	std::string dstFile = std::string("/") + outFile;
	std::cout << "Uploading " << outFile << " to " << host << ":" << port << dstFile << "... ";
	bool b = FtpPutFile(hFtpSession, outFile.c_str(), dstFile.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);
	int ex = 0;
	if (!b)
	{
		ex = -1;
		std::cout << "Failed" << std::endl;
	}
	else 
	{
		std::cout << "OK" << std::endl;
	}
	InternetCloseHandle(hFtpSession);
	InternetCloseHandle(hInternet);

    return ex;
}
