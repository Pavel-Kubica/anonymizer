
#pragma once
#include "AsyncQueue.hpp"
#include "AsyncFileManager.hpp"
#include "MessageWrapper.hpp"
#include "../util/http_log.capnp.h"
#include <http_log.capnp.h>
#include <vector>
#include <thread>

class MessageTransformer
{
public:
    MessageTransformer(AsyncQueue<MessageWrapper>* queue, AsyncFileManager* fileManager);
    ~MessageTransformer();
    void start();

private:
    // Non owning pointers
    AsyncQueue<MessageWrapper>* queue;
    AsyncFileManager* fileManager;
    std::thread storingThread;
    bool shouldRun;

    void threadFunc();

    static void transform(const HttpLogRecord::Reader& reader, HttpLogRecord::Builder& builder);
    static std::string anonymizeIPAddr(const capnp::Text::Reader& urlReader);
};
