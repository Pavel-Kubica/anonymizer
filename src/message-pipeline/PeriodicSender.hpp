
#pragma once
#include "AsyncFileManager.hpp"
#include "database/DBManager.hpp"
#include <thread>
#include <chrono>


class PeriodicSender
{
public:
    const inline static std::chrono::duration INTERVAL = std::chrono::seconds(63);

    PeriodicSender(AsyncFileManager* fileManager, DBManager* dbManager);
    void start();

private:
    // Non owning pointers
    AsyncFileManager* fileManager;
    DBManager* dbManager;
    std::thread decodingThread;
    bool shouldRun;

    void threadFunc();
};
