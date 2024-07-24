
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
    std::vector<std::string> getFileNames();
    void deleteFileNames(const std::vector<std::string>& files);

private:
    void setupNewFile();

    std::vector<std::string> fileNameCache;
    FILE* currFile;
    std::mutex mtx;

    struct FileNameGenerator
    {
        inline static size_t counter = 0;
        static std::string getNextFileName()
        {
            return (std::string("cache/file") + std::to_string(counter++));
        }
    };
};
