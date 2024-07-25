
#include "MessageTransformer.hpp"
#include "MessageConsumer.hpp"
#include "PeriodicSender.hpp"
#include <csignal>

int main()
{
    AsyncQueue<MessageWrapper> queue;
    AsyncFileManager fileManager;
    MessageConsumer consumer{&queue};
    MessageTransformer transformer{&queue, &fileManager};
    DBManager dbManager("http://localhost:8124");
    PeriodicSender sender{&fileManager, &dbManager};

    consumer.start();
    transformer.start();
    dbManager.initializeTables();
    sender.start();
    sleep(1000);
    return 0;
}
