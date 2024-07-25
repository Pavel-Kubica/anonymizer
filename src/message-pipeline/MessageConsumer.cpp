
#include "MessageConsumer.hpp"
#include <string>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include "util/http_log.capnp.h"
#include <iostream>

MessageConsumer::MessageConsumer(AsyncQueue<MessageWrapper>* queue) : topic(nullptr), shouldRun(false), consumedMessageQueue(queue)
{
    std::string error;
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if (conf->set("bootstrap.servers", "localhost:9092", error) !=
        RdKafka::Conf::CONF_OK) {
        std::cerr << error << std::endl;
        throw std::runtime_error("Couldn't create Kafka MessageConsumer " + error);
    }
    consumer = RdKafka::Consumer::create(conf, error);
    if (!error.empty())
    {
        throw std::runtime_error("Couldn't create Kafka MessageConsumer " + error);
    }    
}

MessageConsumer::~MessageConsumer()
{
    shouldRun = false;
    consumingThread.join();
    consumer->stop(topic, RdKafka::Topic::PARTITION_UA);
    delete consumer;
    delete topic;
}

bool MessageConsumer::start()
{
    std::string error;
    topic = RdKafka::Topic::create(consumer, TOPIC_STR, nullptr, error);
    if (!error.empty())
    {
        return false;
    }
    auto errorCode = consumer->start(topic, 0, RdKafka::Topic::OFFSET_BEGINNING);
    if (errorCode != RdKafka::ERR_NO_ERROR)
    {
        return false;
    }
    shouldRun = true;
    consumingThread = std::thread(&MessageConsumer::threadFunc, this);
    return true;
}

void MessageConsumer::threadFunc()
{
    while (shouldRun)
    {
        auto message = consumer->consume(topic, 0, 10000);
        if (message->err() != RdKafka::ERR_NO_ERROR)
        {
            std::cerr << "WARNING: Failed to consume message. Code " << message->err() << ", Message: " << message->errstr() << std::endl;
        }
        else
        {
            consumedMessageQueue->emplace(MessageWrapper{message});
        }
    }
}

void MessageConsumer::stop()
{
    shouldRun = false;
    consumingThread.join();
}
