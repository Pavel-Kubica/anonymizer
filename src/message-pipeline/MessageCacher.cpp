
#include "MessageCacher.hpp"
#include <capnp/serialize.h>
#include <http_log.capnp.h>
#include <iostream>

MessageCacher::MessageCacher(AsyncQueue<MessageWrapper>* queue, AsyncFileManager* fileManager) : queue(queue), fileManager(fileManager), shouldRun(false)
{}

MessageCacher::~MessageCacher()
{
    shouldRun = false;
    storingThread.join();
}

void MessageCacher::start()
{
    shouldRun = true;
    storingThread = std::thread(&MessageCacher::threadFunc, this);
}

void MessageCacher::threadFunc()
{
    while (shouldRun)
    {
        auto msg = queue->blockingPopFront();
        void* alignedMessage = malloc(msg.get()->len());
        memcpy(alignedMessage, msg.get()->payload(), msg.get()->len());

        auto arr = kj::ArrayPtr<capnp::word>(reinterpret_cast<capnp::word*>(alignedMessage), msg.get()->len() / sizeof(capnp::word));
        ::capnp::FlatArrayMessageReader reader(arr);
        auto record = reader.getRoot<HttpLogRecord>();

        ::capnp::MallocMessageBuilder message;
        auto builder = message.initRoot<HttpLogRecord>();
        transform(record, builder);
        fileManager->writeCapnpMessage(message);
        free(alignedMessage);
    }
}

void MessageCacher::transform(const HttpLogRecord::Reader& reader, HttpLogRecord::Builder& builder)
{
    builder.setTimestampEpochMilli(reader.getTimestampEpochMilli());
    builder.setResourceId(reader.getResourceId());
    builder.setBytesSent(reader.getBytesSent());
    builder.setRequestTimeMilli(reader.getRequestTimeMilli());
    builder.setResponseStatus(reader.getResponseStatus());
    builder.setCacheStatus(reader.getCacheStatus());
    builder.setMethod(reader.getMethod());
    builder.setUrl(reader.getUrl());
    builder.setRemoteAddr(anonymizeIPAddr(reader.getRemoteAddr()));
}

std::string MessageCacher::anonymizeIPAddr(const capnp::Text::Reader& addrReader)
{
    std::string str = addrReader.cStr();
    while (str.back() != '.')
    {
        str.pop_back();
        if (str.empty())
        {
            std::cout << "Couldn't format IP???" << std::endl;
            return "";
        }
    }
    str.push_back('X');
    return str;
}
