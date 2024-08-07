
#include "DBManager.hpp"
#include <sstream>
#include <cpr/cpr.h>
#include <iostream>

DBManager::DBManager(const std::string_view& dbUrl) : dbUrl(dbUrl), currScript(Scripts::MAIN_TABLE_INSERT_START)
{}

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
    currScript += ss.str() + ',';
}

bool DBManager::doInsert()
{
    if (currScript.size() == Scripts::MAIN_TABLE_INSERT_START.size())
        return true;
    currScript[currScript.length() - 1] = ';'; // Replaces trailing extraneous comma
    bool retval = executeQuery(currScript);
    currScript = Scripts::MAIN_TABLE_INSERT_START;
    return retval;
}

bool DBManager::executeQuery(const std::string_view& query)
{
    auto response = cpr::Post(cpr::Url{dbUrl},
                              cpr::Header{CONTENT_TYPE_HEADER, {"Content-Length", std::to_string(query.length())}},
                              cpr::Timeout{TIMEOUT},
                              cpr::Body(query));
    if (response.status_code / 100 != 2)
    {
        std::cout << "DB write failed with code " << response.status_code << ":\n" << response.text << std::endl;
        return false;
    }
    std::cout << "DB write successful" << std::endl;
    return true;
}

size_t DBManager::currScriptSize() const
{
    return currScript.size();
}
