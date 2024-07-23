
#include "MessageCacher.hpp"
#include "MessageConsumer.hpp"
#include "PeriodicDecoder.hpp"

int main()
{
    AsyncQueue<MessageWrapper> q;
    AsyncFileManager fm;
    MessageConsumer consumer{&q};
    MessageCacher cacher{&q, &fm};
    PeriodicDecoder decoder{&fm};

    consumer.start();
    cacher.start();
    decoder.start();
    sleep(1000);
    return 0;
}
