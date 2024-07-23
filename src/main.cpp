
#include "MessageTransformer.hpp"
#include "MessageConsumer.hpp"
#include "PeriodicSender.hpp"

int main()
{
    AsyncQueue<MessageWrapper> q;
    AsyncFileManager fm;
    MessageConsumer consumer{&q};
    MessageCacher cacher{&q, &fm};
    PeriodicDecoder decoder{&fm, nullptr};

    consumer.start();
    cacher.start();
    decoder.start();
    sleep(1000);
    return 0;
}
