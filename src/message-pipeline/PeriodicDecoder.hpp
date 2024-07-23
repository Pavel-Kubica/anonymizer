
#pragma once
#include "AsyncFileManager.hpp"
#include "database/DBManager.hpp"
#include <thread>
#include <chrono>


class PeriodicDecoder
{
public:
    const inline static std::chrono::duration INTERVAL = std::chrono::seconds(10);

    PeriodicDecoder(AsyncFileManager* fileManager);
    void start();

private:
    // Non owning pointers
    AsyncFileManager* fileManager;
    DBManager* dbManager;
    std::thread decodingThread;
    bool shouldRun;

    void threadFunc();
};
