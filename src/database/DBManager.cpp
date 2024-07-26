
#include "DBManager.hpp"
#include <sstream>
#include <cpr/cpr.h>
#include <iostream>

DBManager::DBManager(const std::string_view& dbUrl) : dbUrl(dbUrl), tablesInitialized(false)
{}

bool DBManager::initializeTables()
{
    return tablesInitialized = executeQuery(Scripts::TABLE_INIT);
}

bool DBManager::successfullyInitialized() const
{
    return tablesInitialized;
}

void DBManager::addRow(const HttpLogRecord::Reader& record)
{
    std::stringstream ss;
    ss << "("
       <<        record.getTimestampEpochMilli()
       << ',' << record.getResourceId()
       << ',' << record.getBytesSent()
       << ',' << record.getRequestTimeMilli()
       << ',' << record.getResponseStatus()
       << ',' << '\'' << record.getCacheStatus().cStr() << '\''
       << ',' << '\'' << record.getMethod().cStr() << '\''
       << ',' << '\'' << record.getRemoteAddr().cStr() << '\''
       << ',' << '\'' << record.getUrl().cStr() << '\''
       << ")";
    rowsToInsert.push_back(ss.str());
}

bool DBManager::doInsert()
{
    std::string script = std::string{Scripts::MAIN_TABLE_INSERT_START};
    for (const std::string& row : rowsToInsert)
    {
        script += row + ",\n";
    }
    script[script.length() - 2] = ';'; // Replaces trailing extraneous comma
    return executeQuery(script);
}

bool DBManager::executeQuery(const std::string_view& query)
{
    auto response = cpr::Post(cpr::Url{dbUrl},
                              cpr::Header{CONTENT_TYPE_HEADER, {"Content-Length", std::to_string(query.length())}},
                              cpr::Timeout{TIMEOUT},
                              cpr::Body(query));
    if (response.status_code / 100 != 2)
    {
        std::cout << "Failed with code " << response.status_code << ":\n" << response.text << std::endl;
        return false;
    }
    std::cout << "DB write successful" << std::endl;
    return true;
}
