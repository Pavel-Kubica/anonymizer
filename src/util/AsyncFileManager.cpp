

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
    setupNewFile();
    lk.unlock();
    return copy;
}

void AsyncFileManager::returnFileNames(const std::vector<std::string>& files)
{
    std::lock_guard lk(mtx);
    std::copy(files.begin(), files.end(), std::back_inserter(fileNameCache));
}

void AsyncFileManager::setupNewFile()
{
    std::string fileName = FileNameGenerator::getNextFileName();
    fileNameCache.push_back(fileName);
    currFile = fopen(fileName.c_str(), "a");
}