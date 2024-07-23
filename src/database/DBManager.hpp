
#pragma once
#include "util/http_log.capnp.h"
#include <string>

class DBManager
{
public:
    static std::string toTransformedSql(const HttpLogRecord::Reader& record);

private:
};
