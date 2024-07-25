
#pragma once
#include <librdkafka/rdkafkacpp.h>
#include "AsyncQueue.hpp"
#include "MessageWrapper.hpp"
#include <thread>
#include <string>

class MessageConsumer
{
public:
    MessageConsumer(AsyncQueue<MessageWrapper>* queue);
    ~MessageConsumer();
    const inline static std::string TOPIC_STR = "http_log";
    bool start();
    void stop();

private:
    RdKafka::Consumer* consumer;
    RdKafka::Topic* topic;
    std::thread consumingThread;
    bool shouldRun;
    // Non owning pointer
    AsyncQueue<MessageWrapper>* consumedMessageQueue;

    void threadFunc();
};

