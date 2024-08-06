
#include "PeriodicSender.hpp"
#include "util/http_log.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <iostream>

PeriodicSender::PeriodicSender(AsyncFileManager* fileManager, DBManager* dbManager)
        : fileManager(fileManager), dbManager(dbManager), shouldRun(false)
{

}

PeriodicSender::~PeriodicSender()
{
    if (shouldRun)
    {
        stop();
    }
}

void PeriodicSender::start()
{
    shouldRun = true;
    sendingThread = std::thread(&PeriodicSender::threadFunc, this);
}

void PeriodicSender::threadFunc()
{
    while (shouldRun)
    {
        std::this_thread::sleep_for(INTERVAL);
        auto fileNames = fileManager->getFileNames();
        if (fileNames.empty()) continue;

        for (const auto& fileName : fileNames)
        {
            FILE* file = fopen(fileName.c_str(), "r");
            kj::FdInputStream fdStream(fileno(file));
            kj::BufferedInputStreamWrapper bufferedStream(fdStream);
            while (bufferedStream.tryGetReadBuffer() != nullptr)
            {
                auto reader = ::capnp::InputStreamMessageReader(bufferedStream);
                auto record = reader.getRoot<HttpLogRecord>();
                dbManager->addRow(record);
            }
            fclose(file);
        }
        if (dbManager->doInsert())
        {
            fileManager->deleteFileNames(fileNames);
        }
    }
}

void PeriodicSender::stop()
{
    shouldRun = false;
    sendingThread.join();
}

