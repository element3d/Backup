#include "SQLite3Cpp.h"
#include <iostream>
#include <fstream>
#include <functional>

SQLite3Cpp::SQLite3Cpp()
{

}

SQLite3Cpp::~SQLite3Cpp()
{
    if (mHandle) sqlite3_close(mHandle);
}

bool SQLite3Cpp::Open(const std::string& filename)
{
    int err = 0;
    err = sqlite3_open(filename.c_str(), &mHandle);
    if (err)
    {
        std::cerr << sqlite3_errmsg(mHandle) << std::endl;
        return false;
    }
    return true;
}

bool SQLite3Cpp::Exec(const std::string& cmd)
{
    char* messaggeError;
    int err = sqlite3_exec(mHandle, cmd.c_str(), NULL, 0, &messaggeError);
    if (err)
    {
        std::cerr << messaggeError << std::endl;
        sqlite3_free(messaggeError);
        return false;
    }
    return true;
}

bool SQLite3Cpp::Dump(const std::string& dstFilename)
{
    int ret = 0;
    int index = 0;
    std::string cmd;

    std::ofstream ofs;
    ofs.open(dstFilename);
    if (!ofs.is_open())
    {
        return false;
    }

    sqlite3_stmt *pStmtTable = NULL;
    sqlite3_stmt *pStmtData = NULL;
    std::function<void(void)> fCleanup = [pStmtData, pStmtTable, &ofs](){
        if (pStmtData) sqlite3_finalize (pStmtData);
        if (pStmtTable) sqlite3_finalize (pStmtTable);
        ofs.close();
    };

    // Get table creation sql commands
    ret = sqlite3_prepare_v2 (mHandle, "SELECT sql,tbl_name FROM sqlite_master WHERE type = 'table';", -1, &pStmtTable, NULL);
    if (ret != SQLITE_OK)
    {
        fCleanup();
        return false;
    }


    // Process tables
    std::string tableName;
    const unsigned char *pTableName = NULL;
    const unsigned char *pData = NULL;
    ret = sqlite3_step (pStmtTable);
    while (ret == SQLITE_ROW)
    {
        pData = sqlite3_column_text (pStmtTable, 0);
        pTableName = sqlite3_column_text (pStmtTable, 1);
        if (!pData || !pTableName)
        {
            fCleanup();
            return false;
        }
        tableName = std::string((const char*)pTableName);

        // Create table command
        ofs << pData << ";" <<std::endl;

        // Select table data
        cmd = std::string("SELECT * from ") + tableName + ";";

        ret = sqlite3_prepare_v2 (mHandle, cmd.c_str(), -1, &pStmtData, NULL);
        if (ret != SQLITE_OK)
        {
            fCleanup();
            return ret;
        }

        ret = sqlite3_step (pStmtData);
        while (ret == SQLITE_ROW)
        {
            cmd = "INSERT INTO " + tableName + " VALUES(";
            int numCols = sqlite3_column_count(pStmtData);
            for (index = 0; index < numCols; index++)
            {
                if (index) cmd += ",";

                pData = sqlite3_column_text (pStmtData, index);

                if (pData)
                {
                    if (sqlite3_column_type(pStmtData, index) == SQLITE_TEXT)
                    {
                        cmd += "'";
                        cmd += (const char*)pData;
                        cmd += "'";
                    }
                    else
                    {
                         cmd += (const char*)pData;
                    }
                }
                else
                {
                    cmd += "NULL";
                }
            }
            ofs << cmd << ");" << std::endl;
            ret = sqlite3_step (pStmtData);
        }

        ret = sqlite3_step (pStmtTable);
    }

    return ret;
}
