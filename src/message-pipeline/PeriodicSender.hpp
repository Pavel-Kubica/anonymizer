
#pragma once
#include "AsyncFileManager.hpp"
#include "database/DBManager.hpp"
#include <thread>
#include <chrono>


class PeriodicSender
{
public:
    // With fewer than 3 seconds extra, sometimes the next request times out anyway
    constexpr static std::chrono::duration INTERVAL = std::chrono::seconds(60 + 3);
    // If we attempt to insert more than this number of rows, the request will fail with error 413: Request entity too large
    constexpr static size_t MAX_REQUEST_SIZE = 1000000;

    PeriodicSender(AsyncFileManager* fileManager, DBManager* dbManager);
    ~PeriodicSender();

    void start();
    void stop();

private:
    // Non owning pointers
    AsyncFileManager* fileManager;
    DBManager* dbManager;

    std::thread sendingThread;
    bool shouldRun;

    void threadFunc();
    bool sendAndWait(const std::vector<std::string>& fileNames);
};
