
#include "PeriodicSender.hpp"
#include "util/http_log.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize.h>

PeriodicSender::PeriodicSender(AsyncFileManager* fileManager, DBManager* dbManager)
        : fileManager(fileManager), dbManager(dbManager), shouldRun(false)
{

}

void PeriodicSender::start()
{
    sendingThread = std::thread(&PeriodicSender::threadFunc, this);
    shouldRun = true;
}

void PeriodicSender::threadFunc()
{
    while (shouldRun)
    {
        std::this_thread::sleep_for(INTERVAL);

        if (!dbManager->successfullyInitialized())
        { // Clickhouse doesn't allow multiqueries, therefore we have to do this separately
            dbManager->initializeTables();
            continue;
        }

        auto fileNames = fileManager->getFileNames();
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

