

#include "MessageWrapper.hpp"

MessageWrapper::MessageWrapper(RdKafka::Message* msg) : msg(msg)
{}

MessageWrapper::MessageWrapper(MessageWrapper&& other) noexcept : msg(other.msg)
{
    other.msg = nullptr;
}

MessageWrapper::~MessageWrapper()
{
    delete msg;
}

RdKafka::Message* MessageWrapper::get() const
{
    return msg;
}

