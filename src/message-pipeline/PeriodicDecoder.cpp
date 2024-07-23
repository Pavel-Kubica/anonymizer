
#include "PeriodicDecoder.hpp"
#include "util/http_log.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize.h>

PeriodicDecoder::PeriodicDecoder(AsyncFileManager* fileManager) : fileManager(fileManager), shouldRun(false)
{

}

void PeriodicDecoder::start()
{
    decodingThread = std::thread(&PeriodicDecoder::threadFunc, this);
    shouldRun = true;
}

void PeriodicDecoder::threadFunc()
{
    while (shouldRun)
    {
        std::this_thread::sleep_for(INTERVAL);
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
                DBManager::toTransformedSql(record);
            }
        }
    }
}
