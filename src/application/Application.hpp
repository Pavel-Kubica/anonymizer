
#pragma once
#include "MessageTransformer.hpp"
#include "MessageConsumer.hpp"
#include "PeriodicSender.hpp"

class Application
{
public:
    Application();
    void start();
    void stop();

private:
    AsyncQueue<MessageWrapper> queue;
    AsyncFileManager fileManager;
    MessageConsumer consumer;
    MessageTransformer transformer;
    DBManager dbManager;
    PeriodicSender sender;
};
