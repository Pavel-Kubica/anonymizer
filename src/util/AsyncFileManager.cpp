

#include "AsyncFileManager.hpp"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

AsyncFileManager::AsyncFileManager()
{
    if (fs::status(CACHE_DIR_NAME).type() != fs::file_type::directory)
    {
        if (!fs::create_directory(CACHE_DIR_NAME))
        {
            throw std::runtime_error("Could not find or create directory for caching files.");
        }
    }
    recoverOldFiles();
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
        auto it = std::find(fileNameCache.begin(), fileNameCache.end(), s);
        if (it != fileNameCache.end())
        {
            fileNameCache.erase(it);
        }
        remove(s.c_str());
    }
}

void AsyncFileManager::setupNewFile()
{
    std::string fileName = FileNameGenerator::getNextFileName();
    fileNameCache.push_back(fileName);
    currFile = fopen(fileName.c_str(), "a");
}

void AsyncFileManager::recoverOldFiles()
{
    for (auto it = fs::directory_iterator(CACHE_DIR_NAME); it != fs::directory_iterator(); ++it)
    {
        std::string fileName = it->path().filename();
        try
        {
            size_t fileNum = static_cast<size_t>(std::stol(fileName));
            if (fileNum >= FileNameGenerator::counter)
            {
                FileNameGenerator::counter = fileNum + 1;
            }
            fileNameCache.push_back(std::string(CACHE_DIR_NAME) + '/' + it->path().filename().c_str());
        }
        catch (const std::invalid_argument& ex)
        {
            std::cout << "Warning: Unknown file in " << CACHE_DIR_NAME << " : " << fileName << ". Skipping it." << std::endl;
        }
    }
}
