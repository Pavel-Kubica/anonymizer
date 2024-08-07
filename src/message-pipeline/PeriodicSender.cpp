
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
        auto fileNames = fileManager->getFileNames();
        std::vector<std::string> fileNamesToDelete;
        for (const auto& fileName : fileNames)
        {
            FILE* file = fopen(fileName.c_str(), "r");
            kj::FdInputStream fdStream(fileno(file));
            kj::BufferedInputStreamWrapper bufferedStream(fdStream);
            while (bufferedStream.tryGetReadBuffer() != nullptr)
            {
                if (dbManager->currScriptSize() > MAX_REQUEST_SIZE)
                {
                    std::cout << "Had too many rows, inserting only some" << std::endl;
                    if (sendAndWait(fileNamesToDelete))
                    {
                        fileNamesToDelete.clear();
                    }
                }
                auto reader = ::capnp::InputStreamMessageReader(bufferedStream);
                auto record = reader.getRoot<HttpLogRecord>();
                dbManager->addRow(record);
            }
            fclose(file);
            fileNamesToDelete.push_back(fileName);
        }
        sendAndWait(fileNamesToDelete);
    }
}

void PeriodicSender::stop()
{
    shouldRun = false;
    sendingThread.join();
}

bool PeriodicSender::sendAndWait(const std::vector<std::string>& fileNames)
{
    bool insertSuccessful = dbManager->doInsert();
    if (insertSuccessful)
    {
        fileManager->deleteFileNames(fileNames);
    }
    std::this_thread::sleep_for(INTERVAL);
    return insertSuccessful;
}

