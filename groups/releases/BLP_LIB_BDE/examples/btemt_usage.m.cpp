// btemt_usage.m.cpp
#include <btemt_channelqueuepool.h>
#include <btemt_message.h>
#include <bcec_queue.h>
#include <btemt_channelpoolconfiguration.h>
#include <bdef_vfunc4.h>
#include <bdefu_vfunc4.h>
#include <assert.h>
#include <iostream>

using namespace BloombergLP;

static
inline void parseMessages(int *numBytesConsumed, int *numBytesNeeded,
                          const char *data, int length)
{
    assert(numBytesConsumed);
    assert(data);
    assert(0 < length);

    int originalLength  = length;
    int processedMessages = 0;

    while(length > sizeof(int)) {
        int msgLength = ntohl(*(int*)data);  // decode message length
        // As a guard against malicious user, the 'msgLength' should be
        // limited in the protocol.
        assert(sizeof(int) < msgLength);

        if (length < msgLength) {
           *numBytesConsumed = originalLength - length;
           *numBytesNeeded = msgLength - length;
           return;
        }
        length -= msgLength;
        data += msgLength;
        ++processedMessages;
   }

   *numBytesConsumed = originalLength - length;
   *numBytesNeeded = sizeof(int) - length;
}

int main() {
    enum {
        SERVER_ID = 0xAB,            // a cookie
        PORT_NUMBER = 4564,
        MAX_CONNECTIONS = 10000
    };

    // Configure the channel
    btemt_ChannelPoolConfiguration config;
    config.setMaxThreads(4);
    config.setMaxConnections(MAX_CONNECTIONS);
    config.setReadTimeout(5.0);    // in seconds
    config.setWorkloadThreshold(75); // 75% busy
    config.setMetricsInterval(10.0); // seconds
    config.setMaxWriteCache(1<<10);  // 1Mb
    config.setIncomingMessageSizes(4, 100, 1024);
    config.setOutgoingMessageSizes(4, 100, 1024);

    std::cout << config;

    btemt_ChannelQueuePool::ParseMessagesCallback cb;
    bdefu_Vfunc4::makeF(&cb, parseMessages);

    bcec_Queue<btemt_Message> incoming, outgoing;
    btemt_ChannelQueuePool qp(&incoming, &outgoing, cb, config);

    assert(0 == qp.start());   // fails if no system resources.
    if (0 != qp.listen(PORT_NUMBER, MAX_CONNECTIONS, SERVER_ID)) {
        std::cout << "Can't open server port." << std::endl;
        qp.stop();
        return -1;
    }

    while(1) {
        btemt_Message msg = incoming.popFront();
        switch(msg.type()) {
          case btemt_Message::CHANNEL_STATE: {
            const btemt_ChannelMsg& data = msg.channelMsg();
            std::cout << data << std::endl;
          } break;
          case btemt_Message::POOL_STATE: {
            std::cout << msg.poolMsg() << std::endl;
          } break;
          case btemt_Message::TIMER: {
            std::cout << msg.timerMsg().timerId() << std::endl;
          } break;
          case btemt_Message::DATA: {
            // PROCESS DATA MESSAGE HERE
            outgoing.pushBack(msg);
          } break;
       }
    }
    return 0;
}

// END-OF-FILE
