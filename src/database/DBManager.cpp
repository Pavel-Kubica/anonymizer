

#include "DBManager.hpp"

std::string DBManager::toTransformedSql(const HttpLogRecord::Reader& record)
{
    printf("(%lu, %lu, %lu, %lu, %hu, %s, %s, %s, %s)\n", record.getTimestampEpochMilli(),
           record.getResourceId(), record.getBytesSent(), record.getRequestTimeMilli(), record.getResponseStatus(),
           record.getCacheStatus().cStr(), record.getMethod().cStr(), record.getRemoteAddr().cStr(), record.getUrl().cStr());
    return std::string();
}
