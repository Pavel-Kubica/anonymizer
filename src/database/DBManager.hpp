
#pragma once
#include "util/http_log.capnp.h"
#include <chrono>
#include <string>
#include <vector>

class DBManager
{
public:
    explicit DBManager(const std::string_view& dbUrl);
    void addRow(const HttpLogRecord::Reader& record);
    [[nodiscard]] size_t currScriptSize() const;
    bool doInsert();

private:
    const inline static std::pair<std::string, std::string> CONTENT_TYPE_HEADER = {"Content-Type", "application/x-www-form-urlencoded"};
    constexpr static std::chrono::duration TIMEOUT = std::chrono::milliseconds(1000);
    std::string dbUrl;
    std::string currScript;

    bool executeQuery(const std::string_view& query);
};

namespace Scripts
{
    constexpr std::string_view MAIN_TABLE_INSERT_START = "INSERT INTO http_log VALUES\n";
}