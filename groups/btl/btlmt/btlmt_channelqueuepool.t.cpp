// btlmt_channelqueuepool.t.cpp   -*-C++-*-

#include <btlmt_channelqueuepool.h>
#include <btlmt_message.h>
#include <btlso_ipv4address.h>

#include <btls_iovec.h>                         // for testing only
#include <btls_iovecutil.h>                     // for testing only

#include <bsls_platform.h>                      // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bsls_atomic.h>                   // for testing only

#include <bdlf_function.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace bsl;  // automatically added by script

using namespace BloombergLP;
using namespace bdlf::PlaceHolders;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component is a thin patch above the 'btlmt_channelpool' (well tested).
// The only added functionality is the ability to read segment messages
// according to a 'ParseMessagesCallback' and enqueue them into an incoming
// queue.  In addition to a thin data callback for the underlying channel pool,
// the ability to do zero-copy semantics by sending 0 into 'numBytesNeeded'
// must be checked and the message segmentation by the server must be verified
// also.
//-----------------------------------------------------------------------------
//=============================================================================

//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

btlso::IPv4Address serverAddress("127.0.0.1", 0);
bsls::AtomicInt    portNumber(0);

///Usage
///-----
// This section illustrates several 'parseMessages' callback function (of the
// nested 'ParseMessagesCallback' type) and then shows a simple echo server
// example.
//
///The 'parseMessages' Callback Function
///- - - - - - - - - - - - - - - - - - -
// The following snippets of code show three simple implementations of the
// 'parseMessages' callback.  The first example is when no protocol is present,
// and the application is able to process whatever was sent with no minimum
// (e.g., an echo server).  In this case, the data length is just loaded into
// 'numBytesConsumed':
//..
    static
    void parseMessages1(int        *numBytesConsumed,
                        int        *numBytesNeeded,
                        const char *data,
                        int         length)
        // Load the specified 'length' into the specified 'numBytesConsumed'.
    {
        ASSERT(numBytesConsumed);
        ASSERT(data);
        ASSERT(0 < length);
        *numBytesConsumed = length; // consume everything
        *numBytesNeeded = 0;
    }
//..
// The second example is for the case of a very simple protocol, in which each
// message contains its length (in network byte order) in the first 4 bytes;
// the length value includes the 'sizeof(int)' bytes (four bytes on all
// supported platforms) for the 'length' data (and, therefore, length >= 4).
// Here, the message enqueued is a lump of several protocol messages:
//..
    static
    void parseMessages2(int        *numBytesConsumed,
                        int        *numBytesNeeded,
                        const char *data,
                        int         length,
                        int         minIncomingMessageSize)
    {
        ASSERT(numBytesConsumed);
        ASSERT(data);
        ASSERT(0 < length);

        int originalLength  = length;
        int processedMessages = 0;

        while(length > (int)sizeof(int) && length <= minIncomingMessageSize) {
            int msgLength = ntohl(*(const int*)data);  // decode message length

            // As a guard against malicious user, the 'msgLength' should be
            // limited in the protocol.

            ASSERT((int)sizeof(int) < msgLength);

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
       *numBytesNeeded   = sizeof(int) - length;
    }
//..
// We also show how to simplify the logic by returning 'numNeeded' as zero,
// which will be used in the "Simple Echo Server" example below.  Note that we
// do not need to know the 'minIncomingMessageSize' because it has been
// ASSERTed somewhere else that this size is at least four bytes (needed to
// decode the length).
//..
    static
    void parseMessages3(int        *numBytesConsumed,
                        int        *numBytesNeeded,
                        const char *data,
                        int         length)
    {
        ASSERT(numBytesConsumed);
        ASSERT(data);
        ASSERT(0 < length);

        if (length < (int)sizeof(int)) { // not enough bytes for message length
            *numBytesConsumed = 0;
            *numBytesNeeded   = sizeof(int) - length;  // > 0
            return;
        }

        int msgLength = ntohl(*(const int*)data); // decode message length
        ASSERT((int)sizeof(int) < msgLength);

        if (length < msgLength) {  // not enough bytes for message
            *numBytesConsumed = length;
            *numBytesNeeded   = msgLength - length;  // > 0
            return;
        }

        *numBytesConsumed = msgLength;
        *numBytesNeeded   = 0;
    }
//..
// The logic of the callback is simpler, and it also takes advantage of the
// channel queue pool to solve the problem of the buffer boundary (unknown to
// the callback).
//
// Note that the same callback is used for all connections, and, therefore, all
// connections must use the same protocol.  There is no way to specify a
// callback for a connection.
//
///A Simple Echo Server Example
///- - - - - - - - - - - - - -
// The following usage example shows a simple echo server.  The underlying
// message protocol is message length followed by the data (see callback
// example above).  The main thread will wait for a message and echo it back
// immediately:
//..
    void usageExample(int version, bsls::TimeInterval endTime) {
using namespace bsl;  // automatically added by script

        enum {
            SERVER_ID       = 0xAB,            // a cookie
            PORT_NUMBER     = 4564,
            MAX_CONNECTIONS = 10000
        };

        bslma::TestAllocator ta(veryVeryVerbose);
        bdlmca::PooledBufferChainFactory factory(1024, &ta);

        // Configure the channel queue pool.

        btlmt::ChannelPoolConfiguration config;
        config.setMaxThreads(4);
        config.setMaxConnections(MAX_CONNECTIONS);
        config.setReadTimeout(5.0);    // in seconds
        config.setMetricsInterval(10.0); // seconds
        config.setMaxWriteCache(1<<10);  // 1Mb
        config.setIncomingMessageSizes(4, 100, 1024);

        if (veryVerbose) {
            P(config);
        }

        btlmt::ChannelQueuePool::ParseMessagesCallback cb;
        if (1 == version) {
            cb = btlmt::ChannelQueuePool::ParseMessagesCallback(
                        &parseMessages1, &ta);
        } else if (2 == version) {
            cb = bdlf::BindUtil::bindA( &ta
                                     , &parseMessages2
                                     , _1, _2, _3, _4
                                     , static_cast<int>(sizeof(int)));
        } else if (3 == version) {
            cb = btlmt::ChannelQueuePool::ParseMessagesCallback(
                        &parseMessages3, &ta);
        } else {
            ASSERT(0 && "Unkwown parseMessages version.");
        }

        // Create and start channel queue pool.

        bdlcc::Queue<btlmt::Message> incoming(&ta), outgoing(&ta);
        btlmt::ChannelQueuePool qp(&incoming, &outgoing, cb, config, &ta);

        ASSERT(0 == qp.start());   // fails if no system resources.
        if (0 != qp.listen(PORT_NUMBER, MAX_CONNECTIONS, SERVER_ID)) {
            if (verbose) Q("Can't open server port.");  // not a test failure
            qp.stop();
            return;
        }
        portNumber = qp.serverAddress(SERVER_ID)->portNumber();

        int numMessages = 0;
        int numTimeouts = 0;
        bsl::vector<int> channels(&ta);
        bsls::TimeInterval timeout(0.01);  // 10ms

        while (bdlt::CurrentTime::now() < endTime) {
            btlmt::Message msg(btlmt::Message::BTEMT_DATA);
            if (incoming.timedPopFront(&msg,
                                       bdlt::CurrentTime::now() + timeout)) {
                ++numTimeouts;
            }
            else {
                switch(msg.type()) {
                  case btlmt::Message::BTEMT_CHANNEL_STATE: {
                    const btlmt::ChannelMsg& data = msg.channelMsg();
                    if (verbose) P(data);
                    if (data.channelId() >= (int)channels.size()) {
                        channels.resize(data.channelId() + 1);
                    }
                    channels[data.channelId()] = 0;
                    if (data.event() == btlmt::ChannelMsg::BTEMT_CHANNEL_DOWN) {
                        qp.shutdown(data.channelId(),
                                    btlmt::ChannelPool::BTEMT_IMMEDIATE);
                    }
                  } break;
                  case btlmt::Message::BTEMT_POOL_STATE: {
                    if (verbose) P(msg.poolMsg());
                  } break;
                  case btlmt::Message::BTEMT_TIMER: {
                    if (verbose) P(msg.timerMsg().timerId());
                  } break;
                  case btlmt::Message::BTEMT_DATA: {
                    ++numMessages;
                    int channelId = msg.dataMsg().channelId();
                    ++channels[channelId];
                    if (veryVerbose || verbose && 0 == numMessages % 50) {
                        printf("Processing data msg # %d from %d"
                               " (chain offset %d, length %d)\n",
                               channels[channelId],
                               channelId,
                               msg.dataMsg().userDataField1(),
                               msg.dataMsg().userDataField2());
                    }
                    // PROCESS DATA MESSAGE HERE
                    int offset = msg.dataMsg().userDataField1();
                    int length = msg.dataMsg().userDataField2();
                    bdlmca::PooledBufferChain *chain = msg.dataMsg().data();
                    bdlmca::PooledBufferChain *newChain = factory.allocate(0);
                    newChain->replace(0, *chain, offset, length);

                    btlmt::DataMsg& dt = msg.dataMsg();
                    dt.setData(newChain, &factory, &ta);
                    dt.setUserDataField1(0);
                    dt.setUserDataField2(length);

                    outgoing.pushBack(msg);
                  } break;
                  default: {
                    Q(NOT HANDLED);
                  }
                }
            }
        }
        qp.stop();
        return;
    }
//..
// Note that since no clocks are registered in this sample application,
// timer messages are never enqueued.  We showed the 'switch' case here for
// completeness.

//=============================================================================
//                              TEST USAGE EXAMPLE
//-----------------------------------------------------------------------------

const int MAGIC = 0x01234560;
const int MSG_LENGTH = 327;

inline
int payloadHash(int channelId)
    // Return a hash of the specified 'channelId' to serve as a payload for a
    // message on that channel.
{
    return 32 + (channelId % 83);
}

static
void checkUsageExampleMsg(btlmt::DataMsg msg, int version)
    // Check that the specified 'msg' is consistent with the specified
    // 'version' of the 'parseMessages' callback.
{
    int channelId = msg.channelId();
    const char PAYLOAD = payloadHash(channelId);

    bdlmca::PooledBufferChain *chain = msg.data();
    int length = chain->length();
    int idx = 0;

    if (1 == version) {
        // In 'parseMessages1' (below), messages may begin inside a protocol
        // message or at a boundary.  Consume the initial PAYLOAD characters.

        for (; idx < chain->length() && (*chain)[idx] == PAYLOAD; ++idx) {
            --length;
        }
    }

    do {
        // In 'parseMessages2' and 'parseMessages3' (below), messages must
        // begin with a magic word and a length; with 'parseMessages1' we can
        // perform the same check but only if we have enough lookahead
        // (otherwise, not a failure).

        if (1 != version || (int)sizeof(int) <= length) {
            ASSERT((int)sizeof(int) <= length);
            char data[sizeof(int)];
            for (int i = 0; i < (int)sizeof(int); ++i, ++idx, --length) {
                data[i] = (*chain)[idx];
            }
            int magic = ntohl(*(int*)data);
            ASSERT(MAGIC == magic);
        } else
            return;  // version == 1 && length < (int)sizeof(int)

        int msgLength = -1;
        if (1 != version || (int)sizeof(int) <= idx) {
            ASSERT((int)sizeof(int) <= length);
            char data[sizeof(int)];
            for (int i = 0; i < (int)sizeof(int); ++i, ++idx, --length) {
                data[i] = (*chain)[idx];
            }
            msgLength = ntohl(*(int*)data);
            ASSERT(MSG_LENGTH== msgLength);
        } else
            return;  // version == 1 && length < (int)sizeof(int)

        // In all cases, the next 'min(msgLength,length)' bytes must equal
        // the 'PAYLOAD'.

        int remaining = msgLength;
        for (; 0 < remaining && 0 < length; ++idx, --length) {
            ASSERT(PAYLOAD == (*chain)[idx]);
        }

        // In 'parseMessages2' and 'parseMessages3' (below), messages must not
        // contain portions of logical messages, but integral numbers of them.

        ASSERT(1 == version || 0 == remaining);

        // In 'parseMessages3' (below), messages must contain a single logical
        // message.

        ASSERT(3 != version || 0 == length);

    } while (3 != version && 0 < length);
        // Both 'parseMessages1' and 'parseMessages2' allow to have multiple
        // messages lumped in a single message; in that case, repeat.
}

static
void readIncomingMessages(bdlcc::Queue<btlmt::Message> *incoming,
                          btlmt::ChannelQueuePool    *channelQueuePool,
                          bsls::AtomicInt            *numChannels,
                          int                        maxNumMessages,
                          int                        version)
    // Read messages continuously (up to the specified 'maxNumMessages') from
    // the specified 'incoming' queue; close the channel from the specified
    // 'channelQueuePool' if a channel state contains 'CHANNEL_DOWN', and
    // verify that data messages are consistent with the specified
    // 'version' of the 'parseMessages' callback.
{
    btlmt::ChannelQueuePool& qp = *channelQueuePool;
    int numMessages = 0;
    while(numMessages < maxNumMessages) {
        bsls::TimeInterval timeout(10.0); // 10s
        btlmt::Message msg(btlmt::Message::BTEMT_DATA);
        if (incoming->timedPopFront(&msg, bdlt::CurrentTime::now() + timeout)) {
            // timed out (Warning?)
            return;
        }
        switch(msg.type()) {
          case btlmt::Message::BTEMT_CHANNEL_STATE: {
            const btlmt::ChannelMsg& data = msg.channelMsg();
            if (data.event() == btlmt::ChannelMsg::BTEMT_CHANNEL_UP) {
                ++(*numChannels);
                return;
            }
            if (data.event() == btlmt::ChannelMsg::BTEMT_CHANNEL_DOWN) {
                qp.shutdown(data.channelId(),
                            btlmt::ChannelPool::BTEMT_IMMEDIATE);
                return;
            }
          } break;
          case btlmt::Message::BTEMT_DATA: {
            ++numMessages;
            // int channelId = msg.dataMsg().channelId();
            checkUsageExampleMsg(msg.dataMsg(), version);
          } break;
          default: {
            Q(NOT HANDLED);
          }
       }
    }
}

static
void writeMessages(bdlcc::Queue<btlmt::Message> *outgoingQueue,
                   int                        channelId,
                   int                        numMessages,
                   int                        msgLength,
                   char                       payloadChar,
                   bslma::Allocator          *allocator)
{
    bdlmca::PooledBufferChainFactory factory(msgLength, allocator);
    bdlmca::PooledBufferChain *chain = factory.allocate(msgLength);
    char *buffer = chain->buffer(0);

    char intData[sizeof(int)];
    *((int*)intData) = htonl(MAGIC);
    memcpy(buffer, intData, sizeof(int));
    *((int*)intData) = htonl(msgLength);
    memcpy(buffer + sizeof(int), intData, sizeof(int));

    for (int i = 2 * sizeof(int); i < msgLength; ++i) {
        buffer[i] = payloadChar;
    }

    btlmt::Message msg(btlmt::Message::BTEMT_DATA);
    btlmt::DataMsg& dt = msg.dataMsg();
    dt.setData(chain, &factory);
    dt.setChannelId(channelId);

    for (int i = 0; i < numMessages; ++i) {
        outgoingQueue->pushBack(dt);
    }
}

static
void testUsageExample(btlso::IPv4Address serverAddress,
                      int               numChannels,
                      int               numMessages,
                      int               length,
                      int               version)
    // Create a channel queue pool that will connect the specified
    // 'numChannels' to the echo server at the specified 'serverAddress', and
    // send on each channel the specified 'numMessages' with the specified
    // 'length' and contents dependent on the channel, and read back the echoed
    // messages and verify that they are identical to the sent messages and
    // segmented according to the specified 'version' of 'parseMessages' used.
{
    enum {
        NUM_CONNECT_ATTEMPTS = 5
      , CONNECT_TIMEOUT      = 10  // 10s
    };
    const int NUM_THREADS = numChannels;

    bslma::TestAllocator ta(veryVeryVerbose);

    // Configure the channel queue pool.

    btlmt::ChannelPoolConfiguration config;
    config.setMaxThreads(numChannels);
    config.setMaxConnections(2 * numChannels);
    config.setMaxWriteCache(1<<10);  // 1Mb
    config.setIncomingMessageSizes(4, 327, 4096);

    if (veryVerbose) {
        P(config);
    }

    bsl::vector<int> channelIds((int)NUM_THREADS, 0, &ta);

    btlmt::ChannelQueuePool::ParseMessagesCallback cb;
    if (1 == version) {
        cb = btlmt::ChannelQueuePool::ParseMessagesCallback(
                    &parseMessages1, &ta);
    }
    else if (2 == version) {
        cb = bdlf::BindUtil::bindA( &ta
                                 , &parseMessages2
                                 , _1, _2, _3, _4
                                 , sizeof(int));
    }
    else {
        ASSERT(3 == version);
        cb = btlmt::ChannelQueuePool::ParseMessagesCallback(
                    &parseMessages3, &ta);
    }

    // Create and start channel queue pool.

    bdlcc::Queue<btlmt::Message> incoming(&ta), outgoing(&ta);
    btlmt::ChannelQueuePool qp(&incoming, &outgoing, cb, config, &ta);

    ASSERT(0 == qp.start());   // fails if no system resources.

    // Connect channels to server.

    bsls::TimeInterval timeout(CONNECT_TIMEOUT);
    bsls::TimeInterval totalTimeout(NUM_CONNECT_ATTEMPTS * CONNECT_TIMEOUT);

    for (int i = 0; i < NUM_THREADS; ++i) {
        ASSERT(0 == qp.connect(serverAddress,
                               NUM_CONNECT_ATTEMPTS,
                               timeout,
                               i));
    }

    // Start reading.

    bsls::AtomicInt numConnections(0);

    bdlf::Function<void (*)()> invokeReadIncomingMessages(
            bdlf::BindUtil::bindA(&ta,
                                 &readIncomingMessages,
                                 &incoming,
                                 &qp,
                                 &numConnections,
                                 numMessages,
                                 version));

    bdlqq::ThreadUtil::Handle readHandle;
    ASSERT(0 == bdlqq::ThreadUtil::create(&readHandle,
                                         invokeReadIncomingMessages));

    while (numConnections < NUM_THREADS) {
        bdlqq::ThreadUtil::sleep(bsls::TimeInterval(1));
        bdlqq::ThreadUtil::yield();
    }

    // Send messages to each channel, in parallel.

    bsl::vector<bdlqq::ThreadUtil::Handle> handle(&ta);
    handle.resize(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; ++i) {
        bdlf::Function<void (*)()> invokeWriteMessages(
            bdlf::BindUtil::bindA(&ta,
                                 &writeMessages,
                                 &outgoing,
                                 channelIds[i],
                                 numMessages,
                                 MSG_LENGTH + 2 * i,
                                 static_cast<char>(payloadHash(channelIds[i])),
                                 &ta));

        ASSERT(0 == bdlqq::ThreadUtil::create(&handle[i], invokeWriteMessages));
    }

    // Wait until finished.

    for (int i = 0; i < NUM_THREADS; ++i) {
        ASSERT(0 == bdlqq::ThreadUtil::join(handle[i]));
    }

    ASSERT(0 == bdlqq::ThreadUtil::join(readHandle));
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 40: {
        // --------------------------------------------------------------------
        // TESTING 'parseMessage3'
        // Concern: The usage example must compile and work as designed.
        //
        // Plan: Using a test harness, exercise the usage example with the
        //   'parseMessage3' callback, by enqueuing messages and making sure
        //   that they are echoed to the sender by the echo server.
        //
        // Testing:
        //    void usageExample();
        //    void parseMessage3(...);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing parseMessages3" << endl
                          << "======================" << endl;

        enum {
            VERSION      = 3
          , NUM_CHANNELS = 5
          , NUM_MESSAGES = 1000
          , MSG_LENGTH   = 327
        };

        bslma::TestAllocator testAllocator(veryVeryVerbose);
        bsls::TimeInterval endTime(bdlt::CurrentTime::now());
        endTime += bsls::TimeInterval(20.0);  // 20s from now

        bdlf::Function<void (*)()> invokeUsageExample(
                bdlf::BindUtil::bindA(&testAllocator,
                                     &usageExample,
                                     (int)VERSION,
                                     endTime));

        bdlqq::ThreadUtil::Handle handle;
        if (bdlqq::ThreadUtil::create(&handle, invokeUsageExample))
        {
            ASSERT(0 && "ERROR: Could not start thread for 'usageExample'.");
            break;
        }

        while (!portNumber) {
            bdlqq::ThreadUtil::sleep(bsls::TimeInterval(1));
            bdlqq::ThreadUtil::yield();
        }
        serverAddress.setPortNumber(portNumber);

        testUsageExample(serverAddress,
                NUM_CHANNELS,
                NUM_MESSAGES,
                MSG_LENGTH,
                VERSION);

        ASSERT(bdlqq::ThreadUtil::join(handle));

      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING 'parseMessage2'
        // Concern: The usage example must compile and work as designed.
        //
        // Plan: Using a test harness, exercise the usage example with the
        //   'parseMessage2' callback, by enqueuing messages and making sure
        //   that they are echoed to the sender by the echo server.
        //
        // Testing:
        //    void usageExample();
        //    void parseMessage2(...);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing parseMessages2" << endl
                          << "======================" << endl;

        enum {
            VERSION      = 2
          , NUM_CHANNELS = 5
          , NUM_MESSAGES = 1000
          , MSG_LENGTH   = 327
        };

        bslma::TestAllocator testAllocator(veryVeryVerbose);
        bsls::TimeInterval endTime(bdlt::CurrentTime::now());
        endTime += bsls::TimeInterval(20.0);  // 20s from now

        bdlf::Function<void (*)()> invokeUsageExample(
                bdlf::BindUtil::bindA(&testAllocator,
                                     &usageExample,
                                     (int)VERSION,
                                     endTime));

        bdlqq::ThreadUtil::Handle handle;
        if (bdlqq::ThreadUtil::create(&handle, invokeUsageExample))
        {
            ASSERT(0 && "ERROR: Could not start thread for 'usageExample'.");
            break;
        }

        while (!portNumber) {
            bdlqq::ThreadUtil::sleep(bsls::TimeInterval(1));
            bdlqq::ThreadUtil::yield();
        }
        serverAddress.setPortNumber(portNumber);

        testUsageExample(serverAddress,
                NUM_CHANNELS,
                NUM_MESSAGES,
                MSG_LENGTH,
                VERSION);

        ASSERT(bdlqq::ThreadUtil::join(handle));

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'parseMessage1'
        // Concern: The usage example must compile and work as designed.
        //
        // Plan: Using a test harness, exercise the usage example with the
        //   'parseMessage1' callback, by enqueuing messages and making sure
        //   that they are echoed to the sender by the echo server.
        //
        // Testing:
        //    void usageExample();
        //    void parseMessage1(...);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing parseMessages1" << endl
                          << "======================" << endl;

        enum {
            VERSION      = 1
          , NUM_CHANNELS = 5
          , NUM_MESSAGES = 1000
          , MSG_LENGTH   = 327
        };

        bslma::TestAllocator testAllocator(veryVeryVerbose);
        bsls::TimeInterval endTime(bdlt::CurrentTime::now());
        endTime += bsls::TimeInterval(20.0);  // 20s from now

        bdlf::Function<void (*)()> invokeUsageExample(
                bdlf::BindUtil::bindA(&testAllocator,
                                    &usageExample,
                                    (int)VERSION,
                                    endTime));

        bdlqq::ThreadUtil::Handle handle;
        if (bdlqq::ThreadUtil::create(&handle, invokeUsageExample))
        {
            ASSERT(0 && "ERROR: Could not start thread for 'usageExample'.");
            break;
        }

        while (!portNumber) {
            bdlqq::ThreadUtil::sleep(bsls::TimeInterval(1));
            bdlqq::ThreadUtil::yield();
        }
        serverAddress.setPortNumber(portNumber);

        testUsageExample(serverAddress,
                NUM_CHANNELS,
                NUM_MESSAGES,
                MSG_LENGTH,
                VERSION);

        ASSERT(bdlqq::ThreadUtil::join(handle));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Testing:
        //   This test exercises basic functionality, but *tests* *nothing*.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
