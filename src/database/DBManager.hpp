
#pragma once
#include "util/http_log.capnp.h"
#include <chrono>
#include <string>
#include <vector>

class DBManager
{
public:
    explicit DBManager(const std::string_view& dbUrl);
    bool initializeTables();
    void addRow(const HttpLogRecord::Reader& record);
    bool doInsert();
    bool successfullyInitialized() const;

private:
    const inline static std::pair<std::string, std::string> CONTENT_TYPE_HEADER = {"Content-Type", "application/x-www-form-urlencoded"};
    constexpr static std::chrono::duration TIMEOUT = std::chrono::milliseconds(1000);
    std::string dbUrl;
    bool tablesInitialized;
    std::vector<std::string> rowsToInsert;

    bool executeQuery(const std::string_view& query);
};

namespace Scripts
{
    constexpr std::string_view TABLE_INIT = "CREATE TABLE IF NOT EXISTS http_log\n"
                                                   "("
                                                   "  timestamp DateTime,\n"
                                                   "  resource_id UInt64,\n"
                                                   "  bytes_sent UInt64,\n"
                                                   "  request_time_milli UInt64,\n"
                                                   "  response_status UInt16,\n"
                                                   "  cache_status LowCardinality(String),\n"
                                                   "  method LowCardinality(String),\n"
                                                   "  remote_addr String,\n"
                                                   "  url String\n"
                                                   ")\n"
                                                   "ENGINE = MergeTree()\n"
                                                   "ORDER BY (resource_id, response_status, cache_status, remote_addr);";
    constexpr std::string_view MAIN_TABLE_INSERT_START = "INSERT INTO http_log VALUES\n";
}