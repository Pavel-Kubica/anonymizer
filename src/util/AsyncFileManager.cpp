

#include "AsyncFileManager.hpp"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>
#include "http_log.capnp.h"

AsyncFileManager::AsyncFileManager()
{
    setupNewFile();
}

void AsyncFileManager::writeCapnpMessage(::capnp::MessageBuilder& message)
{
    std::lock_guard lk(mtx);
    ::capnp::writeMessageToFd(fileno(currFile), message);
}

std::vector<std::string> AsyncFileManager::getFileNames()
{
    std::unique_lock lk(mtx);
    auto copy = fileNameCache;
    fileNameCache.clear();
    fclose(currFile);
    setupNewFile();
    lk.unlock();
    return copy;
}

void AsyncFileManager::deleteFileNames(const std::vector<std::string>& files)
{
    std::lock_guard lk(mtx);
    for (const std::string& s : files)
    {
        remove(s.c_str());
    }
}

void AsyncFileManager::setupNewFile()
{
    std::string fileName = FileNameGenerator::getNextFileName();
    fileNameCache.push_back(fileName);
    currFile = fopen(fileName.c_str(), "a");
}