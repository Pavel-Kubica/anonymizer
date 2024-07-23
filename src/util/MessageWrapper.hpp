
#pragma once
#include <librdkafka/rdkafkacpp.h>

/**
 * Simple wrapper around RdKafka::Message to take care of destructing
 */
class MessageWrapper
{
public:
    MessageWrapper(RdKafka::Message* msg);
    MessageWrapper(const MessageWrapper& other) = delete;
    MessageWrapper(MessageWrapper&& other) noexcept;
    ~MessageWrapper();
    [[nodiscard]] RdKafka::Message* get() const;

private:
    RdKafka::Message* msg;
};
