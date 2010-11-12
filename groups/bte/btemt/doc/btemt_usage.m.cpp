// btemt_usage.m.cpp           -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT("$Id$ $CSID$")

#include <btemt_channelqueuepool.h>
#include <btemt_message.h>
#include <bcec_queue.h>
#include <btemt_channelpoolconfiguration.h>
#include <bcef_vfunc4.h>
#include <bcefu_vfunc4.h>
#include <bsls_assert.h>
#include <iostream>

using namespace BloombergLP;

static
inline void parseMessages(int        *numBytesConsumed,
                          int        *numBytesNeeded,
                          const char *data,
                          int         length)
     // Parse the specified message 'data' of the specified 'length'.  Load
     // into the specified 'numBytesConsumed' the number of bytes of 'data'
     // that comprise the number of *complete* (logical) messages within 'data'
     // and load into the specified 'numBytesNeeded' the number of bytes needed
     // to complete a trailing partial logical message within 'data' or load 0
     // if the last logical message in 'data' was complete.
{
    BSLS_ASSERT(numBytesConsumed);
    BSLS_ASSERT(data);
    BSLS_ASSERT(0 < length);

    int originalLength    = length;
    int processedMessages = 0;

    while(length > sizeof(int)) {
        int msgLength = ntohl(*(int*)data);  // decode message length
        // As a guard against malicious user, the 'msgLength' should be
        // limited in the protocol.
        BSLS_ASSERT(sizeof(int) < msgLength);

        if (length < msgLength) {
           *numBytesConsumed = originalLength - length;
           *numBytesNeeded   = msgLength - length;
           return;
        }
        length -= msgLength;
        data   += msgLength;
        ++processedMessages;
   }

   *numBytesConsumed = originalLength - length;
   *numBytesNeeded   = sizeof(int) - length;
}

int main()
{
    enum {
        SERVER_ID       = 0xAB,    // a cookie
        PORT_NUMBER     = 4564,
        MAX_CONNECTIONS = 10000
    };

    // Configure the channel
    btemt_ChannelPoolConfiguration config;
    config.setMaxThreads(4);
    config.setMaxConnections(MAX_CONNECTIONS);
    config.setReadTimeout(5.0);                   // in seconds
    config.setWorkloadThreshold(75);              // 75% busy
    config.setMetricsInterval(10.0);              // seconds
    config.setMaxWriteCache(1<<10);               // 1Mb
    config.setIncomingMessageSizes(4, 100, 1024);
    config.setOutgoingMessageSizes(4, 100, 1024);

    std::cout << config;

    btemt_ChannelQueuePool::ParseMessagesCallback cb;
    bcefu_Vfunc4::makeF(&cb, parseMessages);

    bcec_Queue<btemt_Message> incoming, outgoing;
    btemt_ChannelQueuePool qp(&incoming, &outgoing, cb, config);

    BSLS_ASSERT(0 == qp.start());   // fails if no system resources.
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
