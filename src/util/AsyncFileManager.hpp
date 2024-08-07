
#pragma once
#include <vector>
#include <cstdio>
#include <fstream>
#include <string>
#include <mutex>
#include <capnp/serialize.h>
#include "http_log.capnp.h"

// MessageTransformer will call writeCapnpMessage every time it receives a new message. Whenever we are ready to send to database,
// DBManager will ask for all the files that were not written to DB yet, and we stop tracking them here.
// If the DB write fails, the files will be returned for further caching, until they can finally be written to DB
// Mutexes should guarantee that no record gets grabbed halfway through being written.
class AsyncFileManager
{
public:
    AsyncFileManager();
    void writeCapnpMessage(::capnp::MessageBuilder& message);
    // If file names are not deleted, they will be returned again upon the next get.
    // If a file's contents are already written to DB, do not call this function again before calling deleteFileNames with that file, otherwise duplicate records will be stored.
    std::vector<std::string> getFileNames();
    void deleteFileNames(const std::vector<std::string>& files);

private:
    void setupNewFile();
    void recoverOldFiles();

    std::vector<std::string> fileNameCache;
    FILE* currFile;
    std::mutex mtx;

    constexpr static std::string_view CACHE_DIR_NAME = "cache";
    struct FileNameGenerator
    {
        inline static size_t counter = 0;
        static std::string getNextFileName()
        {
            return (std::string(CACHE_DIR_NAME) + '/' + std::to_string(counter++));
        }
    };
};
