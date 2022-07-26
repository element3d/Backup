#ifndef __SQLITE3_H__
#define __SQLITE3_H__

#include <sqlite3/sqlite3.h>
#include <string>

class SQLite3Cpp
{
public:
    SQLite3Cpp();
    ~SQLite3Cpp();

    bool Open(const std::string& filename);
    bool Exec(const std::string& cmd);
    bool Dump(const std::string& dstFilename);

private:
     sqlite3* mHandle = nullptr;
};

#endif // __SQLITE3_H__
