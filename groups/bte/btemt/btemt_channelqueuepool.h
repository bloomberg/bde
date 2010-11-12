// btemt_channelqueuepool.h           -*-C++-*-
#ifndef INCLUDED_BTEMT_CHANNELQUEUEPOOL
#define INCLUDED_BTEMT_CHANNELQUEUEPOOL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide protocol-neutral queue-based message transport.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@SEE_ALSO: btemt_message btemt_channelpool btemt_channelpoolconfiguration
//
//@CLASSES:
//   btemt_ChannelQueuePool: queue-based message transport.
//
//@DESCRIPTION: This component provides a queue-based message transport over
// IPv4-addressed byte-stream communication channels.  The channels are created
// automatically when the appropriate events occur, and destroyed based on user
// requests.  A new channel is allocated automatically when an incoming
// connection is accepted, or when a user explicitly requests a connection to a
// server.  The channel queue pool provides both client (aka connector) and
// server (aka acceptor) functionality.  The channel pool manages efficient
// delivery of messages to/from the user based on configuration information
// supplied at construction (see the 'btemt_channelpool' component for
// details).  The outgoing messages are taken from the user-installed queue (in
// FIFO order) and incoming messages are pushed onto the user-installed queue.
// Clients must supply a 'ParseMessageCallback', used by the channel queue pool
// to determine the message boundaries in the incoming data, such that the
// channel queue pool can ensure only complete messages are enqueued on the
// incoming queue.  Generally speaking, an operation on the channel queue pool
// is initiated by a direct method call, with the return status indicating
// whether or not an operation has been successfully initiated.  The result of
// the operation, whether one time or periodic, is delivered through the
// incoming message queue.
//
///'btemt' Messages
///----------------
// The messages emitted by the channel queue pool can be divided in two
// categories: data messages and control messages.  Data messages carry user
// data (i.e., data read from a channel, or data to be delivered to a channel);
// control messages carry information about certain events observed by the
// pool.  The category of control messages consists of pool messages (i.e.,
// pool-wide events like channel capacity), channel messages (i.e., a state
// change on a channel), and clock messages (representing a single tick).  Only
// data messages should be enqueued onto the _outgoing_ queue; a control
// message popped off the outgoing queue by the pool is silently discarded.
//
///Data Messages
///- - - - - - -
// The 'btemt_Message' class provides two similar means of encoding data
// messages, the 'btemt_DataMsg' type (built around 'bcema_PooledBufferChain')
// and the 'btemt_BlobMsg' type (built around 'bcema_Blob').  Both
// 'btemt_DataMsg' and 'btemt_Blob' provide a means of combining individual
// memory buffers into a complete data message.
//
// A 'btemt_ChannelPoolQueue' will enqueue data messages *only* as
// 'btemt_DataMsg' objects on the _incoming_ queue, but the client can enqueue
// both 'btemt_DataMsg' and 'btemt_BlobMsg' objects on the _outgoing_ queue.
// Both message types consist of a shared-pointer to the data (either a
// 'bcema_PooledBufferChain' or 'bcema_Blob'), along with the data length, and
// the source channel ID.
//
// A client implicitly adopts ownership of the underlying
// 'bcema_PooledBufferChain' when it removes a 'btemt_DataMsg' from the
// incoming queue.  When the client destroys the last copy of the incoming
// 'btemt_DataMsg' object (i.e., the reference count of the contained shared
// pointer reaches 0), the memory for the pooled buffer chain is reclaimed.
// When creating a data message to place on the outgoing queue, the data
// buffers of the outgoing message should *not* be deallocated by the client.
// Once the data message is placed on the outgoing queue the memory is adopted
// by the 'btemt_ChannelQueuePool' and will be deallocated automatically (using
// the encorporated allocator) once the message has been sent.  Generally
// speaking, whenever a data message is enqueued, the ownership of the embedded
// buffer is transferred from one entity (e.g., the user application) to
// another (e.g., channel pool).
//
///btemt_DataMsg' Objects on the Incoming Queue
///- - - - - - - - - - - - - - - - - - - - - -
// One potentially confusing aspect of this component is that data messages (of
// type 'btemt_DataMsg') read from the channel queue pool's incoming message
// queue *may* *not* start at the first byte of the contained
// 'bcema_PooledBufferChain' and *may* *not* be of length
// 'bcema_PooledBufferChain::length()'.  Instead, the channel queue pool
// encodes the offset of the beginning of the message, and the actual length of
// the message, in the user data fields of the 'btemt_DataMsg' object.  Each
// message actually begins at the 'btemt_DataMsg::userDataField1()' byte of the
// contained buffer chain, and is of length 'userDataField2()'.  This behavior
// was introduced to efficiently provide zero-copy semantics, which is
// described in more detail in the 'Implementing "ParseMessageCallback"'
// section.  Note that clients that do not use 'Zero-Copy' semantics (e.g.,
// clients written before the feature was introduced) can continue to assume
// that the messages begin at byte 0 of the buffer chain and span the entire
// length of the buffer chain.
//
///Message boundaries and 'ParseMessagesCallback'
///----------------------------------------------
// The channel queue pool is protocol-neutral, meaning that it supports message
// delivery using any application-level (i.e., OSI levels 5-7) protocol.  The
// channel queue pool uses a user-installed callback of the
// 'ParseMessagesCallback' type to distinguish message boundaries in the
// incoming data.  The callback is invoked when data is read from a channel.
// The prototype of a 'ParseMessageCallback' operation looks like:
//..
//  void parseMessageCallback(int        *numBytesConsumed,
//                            int        *numBytesNeeded,
//                            const char *firstDataBuffer,
//                            int         totalLength);
//..
// The callback loads into 'numBytesConsumed' the number of bytes (between 0
// and the 'totalLength') that can be processed, as determined from the
// contents of the buffer (according to whatever message protocol is being
// used).  The callback loads into 'numBytesNeeded' the minimal length of
// additional data that must be received (in addition to the number of
// available bytes already passed to the callback) until the callback can be
// invoked again; note that this value may be 0 (see the section "Implementing
// 'ParseMessageCallback'").  The 'dataBuffer' contains the *first* buffer of a
// chain of data buffers read from the channel.  The 'totalLength' contains the
// *total* length of *all* the data in that buffer chain.  This means that
// 'firstDataBuffer' *may* contain *less* then 'totalLength' sequential bytes.
// The actual number of sequential bytes available in 'firstDataBuffer' is
// guaranteed to be at least the minimum of 'totalLength' and the
// 'minIncomingMessageSize()' value in the 'btemt_ChannelPoolConfiguration'
// object supplied to the channel queue pool on construction.
//
// Upon returning from the callback, the first 'numBytesConsumed' bytes of the
// incoming data is enqueued as a data message onto the incoming queue, and the
// remaining data is cached inside the channel queue pool and is provided to
// this same callback when the minimum additional data (as specified from
// 'numBytesNeeded') is read from the channel.  Note that the
// 'ParseMessagesCallback' serves only to determine the boundaries of a message
// (or messages) and does not need to read data bytes beyond what is needed to
// determine the length of the message to be enqueued; actual data processing
// should be handled by removing the data messages from the incoming queue.
//
///Implementing 'ParseMessageCallback' (with 'Zero-Copy' semantics).
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following section describes how a client might implement the
// 'ParseMessageCallback' to respond to a series of incoming data.  Note that
// zero-copy semantics was introduced after this component was originally
// released, and avoids defects and provides more efficient behavior than the
// original implementation of the component.
//
// A key element of the 'Zero-Copy' semantics is that, if the client supplied
// 'ParseMessageCallback' loads 0 into 'numBytesNeeded', the channel queue pool
// will immediately invoke the 'ParseMessageCallback' again with the remaining
// (unconsumed) data.
//
// The table below examines how 'ParseMessageCallback' would populate
// 'numBytesNeeded' and 'numBytesConsumed'.  The example uses a hypothetical
// message protocol with a 12 byte header, each header contains a value
// indicating the length of the entire message.  For simplicity, we assume that
// all the messages received on our channel happen to be 100 bytes (i.e., a 12
// byte header and 88 bytes of data).
//..
//  |total bytes|    load into   |  load into   |
//  |   received|numBytesConsumed|numBytesNeeded| explanation
//  |-----------+----------------+--------------+-----------------------------
//  |          5|               0|             7| 7 more bytes needed for a
//  |           |                |              | complete (12 byte) header.
//  |-----------+----------------+--------------+-----------------------------
//  |         21|               0|            79| Full header, 79 bytes left
//  |           |                |              | for full message (100 - 21)
//  |-----------+----------------+--------------+-----------------------------
//  |        100|             100|            12| one msg enqueued (100 bytes)
//  |           |                |              | 12 bytes needed for
//  |           |                |              | subsequent header.
//  |-----------+----------------+--------------+-----------------------------
//  |        105|             100|             7| one msg enqueued (100 byes),
//  |           |                |              | 7 bytes needed for
//  |           |                |              | subsequent header.
//  |-----------+----------------+--------------+-----------------------------
//  |call 1: 121|             100|             0| one msg enqueued (100 byes),
//  |call 2:  21|               0|            79| 0 needed (so callback is
//  |           |                |              | invoked again with remaining
//  |           |                |              | bytes), 79 bytes needed for
//  |           |                |              | next message.
//  |-----------+----------------+--------------+-----------------------------
//  |call 1: 300|             100|             0| three 100 byte message
//  |call 2: 200|             100|             0| enqueued in a series of
//  |call 3: 100|             100|            12| callbacks (12 bytes need
//  |           |                |              | for subsequent header).
//  |-----------+----------------+--------------+-----------------------------
//  |call 1: 350|             100|             0| three 100 byte message
//  |call 2: 250|             100|             0| enqueued in a series of
//  |call 3: 150|             100|             0| callbacks, 50 bytes needed
//  |call 4:  50|               0|            50| to complete next message.
//  +-----------+----------------+--------------+-----------------------------
//..
// Note that the 'ParseMessageCallback' can only access the first
// 'min(minIncomingMessageSize(), totalLength)' bytes of the provided buffer
// (where 'minIncomingMessageSize()' is a configuration element of the
// 'btemt_ChannelPoolConfiguration' suplied to the channel queue pool on
// construction).  Therefore, for this example protocol, the configured
// 'minIncomingMessageSize()' *must* be *at* *least* 12; i.e., enough space for
// the message header (so that the callback can determine the size of a
// complete message).
//
// A by-product of using zero-copy semantics it that data messages (of type
// 'btemt_DataMsg') read from the channel queue pool's incoming message queue
// *may* *not* start at the first byte of the contained
// 'bcema_PooledBufferChain' and *may* *not* be of length
// 'bcema_PooledBufferChain::length()'.  Instead, the channel queue pool
// encodes the offset of the beginning of the message, and the actual length of
// the message, in the user data fields of the 'btemt_DataMsg' object.  Each
// message actually begins at the 'btemt_DataMsg::userDataField1()' byte of the
// contained buffer chain, and is of length 'userDataField2()'.
//
///'ParseMessageCallback' - Original Semantics (Deprecated)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Note that this section describes the original semantics of this component.
// The described behavior is still supported, but it is less efficient and
// causes problems under certain circumstances.
//
// A key difference between the original semantics of this component (described
// in this section) and the newer 'zero-copy' semantics (described in the
// previous section), is that a client who expects the original semantics
// should implement a 'ParseMessageCallback' that does *not* load 0 into the
// 'numBytesNeeded'.  This impacts how messages are enqueued on the incoming
// queue.  It guarantee that messages on the incoming queue begin at byte 0 of
// the 'bcema_PooledBufferChain' and are the length of the buffer chain.  It
// also means that the 'btemt_DataMsg' objects enqueued on the incoming queue
// may contain more than one protocol-defined message.  Finally, when
// implementing 'ParseMessageCallback' the client is guaranteed access to the
// first 'maxIncomingMessageSize()' bytes of the provided buffer (rather than
// the first 'minIncomingMessageSize()' bytes, as guaranteed when using
// 'zero-copy' semantics).
//
// The table below examines how 'ParseMessageCallback', using the (deprecated)
// original semantics of this component, would populate 'numBytesNeeded' and
// 'numBytesConsumed'.  The example uses a hypothetical message protocol with a
// 12 byte header, each header contains a value indicating the length of the
// entire message.  For simplicity, we assume that all the messages received
// happen to be 100 bytes (i.e., a 12 byte header and 88 bytes of data).  We
// also assume that the 'maxIncomingMessageSize()' configuration value is 1000.
//..
//  |total bytes|                |
//  |   received|numBytesConsumed|numBytesNeeded| explanation
//  |-----------+----------------+ -------------+------------------------------
//  |          5|               0|             7| 7 more bytes needed for a
//  |           |                |              | complete (12 byte) header.
//  |-----------+----------------+ -------------+------------------------------
//  |         11|               0|            79| Full header, 79 bytes left
//  |           |                |              | for full message (100 - 11)
//  |-----------+----------------+ -------------+------------------------------
//  |        100|             100|            12| one msg enqueued (100 bytes)
//  |-----------+----------------+ -------------+------------------------------
//  |        105|             100|             7| one msg enqueued (100 byes),
//  |           |                |              | 7 bytes for new header.
//  |-----------+----------------+ -------------+------------------------------
//  |        120|             100|            80| one msg enqueued (100 byes),
//  |           |                |              | 80 bytes for next message.
//  |-----------+----------------+ -------------+------------------------------
//  |        300|             300|            12| enqueued 3 (100 byte)
//  |           |                |              | messages in a *single*
//  |           |                |              | 'btemt_DataMsg' object.
//  |-----------+----------------+ -------------+------------------------------
//  |       1200|            1000|             1| enqueued 10 (100 byte)
//  |           |                |              | messages.  Note: only the
//  |           |                |              | first 1000 bytes (i.e.,
//  |           |                |              | 'maxIncomingMessageSize()'),
//  |           |                |              | can be accessed in the
//  |           |                |              | callback.
//  |           |                |              | (* see below)
//  |-----------+----------------+ -------------+------------------------------
//..
// Note in the last two cases, where more than one (protocol-defined) message
// is enqueued, those messages are enqueued in a single 'btemt_DataMsg' object
// and must be broken up later by the client when processing the _incoming_
// queue.  Also note that in the case of 1200 bytes read, only the first 1000
// bytes of data, i.e., the configured 'maxIncomingMessageSize()', are
// accessible.  Since we cannot inspect the message headers past the 1000th
// byte, we must leave the remaining 200 bytes for a later callback.
// Unfortunately, we return '*numBytesNeeded' as 1, so 'ParseMessageCallback'
// will be invoked again when the 1301st byte is read from the channel (or
// never, if no more bytes are read by the channel).  This is a *defect* in the
// original behavior of this component that is handled by the zero-copy
// semantics.
//
///Usage
///-----
// This section illustrates several 'parseMessages' callback functions (of the
// nested 'ParseMessagesCallback' type) and then shows a simple echo server
// example.
//
///The 'parseMessages' Callback Function
///- - - - - - - - - - - - - - - - - - -
// The following snippet of code shows an implementations of the
// 'parseMessages' callback.  The example assumes a very simple message
// protocol, in which each message contains its length (in network byte order)
// in the first 4 bytes; the length value includes the length of this 4 byte
// header (therefore, 'length >= 4').  Note that this example is using the
// newer zero-copy semantics.
//..
//  static
//  inline void parseMessages(int        *numBytesConsumed,
//                            int        *numBytesNeeded,
//                            const char *data,
//                            int         length)
//  {
//      assert(numBytesConsumed);
//      assert(data);
//      assert(0 < length);
//
//      if (length < 4) {  // not enough bytes for message length
//          *numConsumed = 0;
//          *numBytesNeeded = sizeof(int) - length;  // > 0
//      }
//
//
//      int msgLength = ntohl(*(int*)data); // decode message length
//      assert(msgLength >= 4);
//
//      if (length < msgLength) {  // not enough bytes for message
//          *numConsumed = length;
//          *numBytesNeeded = msgLength - length;  // > 0
//          return;
//      }
//
//      *numConsumed = msgLength;
//      *numBytesNeeded = 0;
//      return 0;
//  }
//..
// Note that the same callback is used for all connections, and, therefore, all
// connections must use the same protocol.  There is no way to specify a
// callback for a particular connection.
//
///A Simple Echo Server Example
///- - - - - - - - - - - - - - -
// The following usage example shows a simple echo server using the
// 'parseMessages' callback implemented above.  The underlying message protocol
// is message length followed by the data (see callback example above).  The
// main thread will wait for messages to be enqueued on the incoming queue, and
// echo those messages back to the client:
//..
//  int main() {
//      using namespace std;
//
//      enum {
//          SERVER_ID = 0xAB,            // an arbitrary value
//          PORT_NUMBER = 4564,
//          MAX_CONNECTIONS = 10000
//      };
//
//      // Configure the channel queue pool.
//
//      btemt_ChannelPoolConfiguration config;
//      config.setMaxThreads(4);
//      config.setMaxConnections(MAX_CONNECTIONS);
//      config.setReadTimeout(5.0);    // in seconds
//      config.setMetricsInterval(10.0); // seconds
//      config.setMaxWriteCache(1<<10);  // 1Mb
//..
// Using 'setIncomingMessageSizes()' we configure the
// 'minIncomingMessageSize()' of 'config' to be 4 bytes (i.e., the size of our
// protocol header).  This ensures that we can access the first 4 bytes of the
// 'data' buffer passed to 'parseMessages', and can therefore determine the
// length of the message.
//..
//      config.setIncomingMessageSizes(4, 100, 1024);
//
//      cout << config;
//
//      btemt_ChannelQueuePool::ParseMessagesCallback cb(
//              bdef_BindUtil::bindA(&testAllocator,
//                                   &cb,
//                                   _1, _2, _3, _4,
//                                   parseMessages3));
//
//      // Create and start channel queue pool.
//
//      bcema_PooledBlobBufferFactory factory(1024);
//      bcec_Queue<btemt_Message> incoming, outgoing;
//      btemt_ChannelQueuePool qp(&incoming, &outgoing, cb, config);
//
//      assert(0 == qp.start());   // fails if no system resources.
//      if (0 != qp.listen(PORT_NUMBER, MAX_CONNECTIONS, SERVER_ID)) {
//          cout << "Can't open server port." << endl;
//          qp.stop();
//          return -1;
//      }
//
//      while(1) {
//          btemt_Message msg = incoming.popFront();
//          switch(msg.type()) {
//            case btemt_Message::BTEMT_CHANNEL_STATE: {
//              const btemt_ChannelMsg& data = msg.channelMsg();
//              cout << data << endl;
//            } break;
//            case btemt_Message::BTEMT_POOL_STATE: {
//              cout << msg.poolMsg() << endl;
//            } break;
//            case btemt_Message::BTEMT_TIMER: {
//              cout << msg.timerMsg().timerId() << endl;
//            } break;
//..
// Note that since no clocks are registered in this sample application, timer
// messages are never enqueued.  We showed the 'switch' case here for
// completeness.  The interesting case is that of a data message:
//..
//            case btemt_Message::BTEMT_DATA: {
//              // PROCESS DATA MESSAGE HERE
//              int offset = msg.dataMsg().userDataField1();
//              int length = msg.dataMsg().userDataField2();
//..
// Note that before we enqueue the message back onto the 'outgoing' queue, we
// must trim it so that it begins at the offset given by
// 'msg.dataMsg().userDataField1()' and of length given by
// 'msg.dataMsg().userDataField2()':
//..
//              bcema_PooledBufferChain *chain = msg.dataMsg().data();
//              bcema_PooledBufferChain *newChain = factory.allocate(0);
//              newChain.replace(0, *chain, offset, length);
//..
// and we may construct a data message holding the 'newChain' and enqueue it to
// the 'outgoing' chain:
//..
//              btemt_DataMsg& dt = msg.dataMsg();
//              dt.setData(newChain, &ta);
//              dt.setUserDataField1(0);
//              dt.setUserDataField1(length);
//
//              outgoing.pushBack(msg);
//            } break;
//..
// Note that this actually copies data into the 'newChain'.  However, it is
// also possible to convert this into a blob, and alias the shared pointer to
// give the blob the same lifetime as the 'chain'; this avoids any copying at
// all, and uses a new capability of 'btemt_channelqueuepool' to handle data
// messages of the 'btemt_BlobMsg' type.
//..
//          }
//      }
//  }
//..
//
///The 'parseMessages' Function Using the (deprecated) Original Semantics
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This second implementation of the 'parseMessages' function uses the original
// semantics of this component.  The example assumes the same message protocol
// as the original 'parseMessages' (i.e., a 4 byte header with the message
// length, followed by data).  This implementation requires an extra parameter,
// 'maxIncomingMessageSize', which must be bound (in the callback-functor
// object passed to the channel queue pool) to the 'maxIncomingMessageSize()'
// configuration value of the channel queue pool.  Note that we can only access
// the first 'min(length, maxIncomingMessageSize)' of the specified 'data':
//..
//  static
//  inline void deprecatedParseMessages(int        *numBytesConsumed,
//                                      int        *numBytesNeeded,
//                                      const char *data,
//                                      int         length,
//                                      int         maxIncomingMessageSize)
//  {
//      assert(numBytesConsumed);
//      assert(data);
//      assert(0 < length);
//
//      int originalLength  = length;
//      int processedMessages = 0;
//
//      while (length >= 4 && length < maxIncomingMessageSize) {
//          int msgLength = ntohl(*(int*)data);  // decode message length
//
//          // As a guard against malicious user, the 'msgLength' should be
//          // limited in the protocol.
//          assert(msgLength >= 4);
//
//          if (length < msgLength) {
//              *numConsumed = originalLength - length;
//              *numBytesNeeded = msgLength - length;
//              return;
//          }
//          length -= msgLength;
//          data += msgLength;
//          ++processedMessages;
//     }
//
//     *numConsumed = originalLength - length;
//     *numBytesNeeded = 4 - length;
//  }
//..
// Here, the message enqueued on the incoming queue is a lump of several
// protocol messages.
//
// We again define our echo server main-loop.  In this instance we bind the max
// incoming message size value to our message parsing function
// 'deprecatedParseMessages'.
//..
//  enum {
//      SERVER_ID = 0xAB,            // a cookie
//      PORT_NUMBER = 4564,
//      MAX_CONNECTIONS = 10000
//  };
//
//  // Configure the channel
//  btemt_ChannelPoolConfiguration config;
//  config.setMaxThreads(4);
//  config.setMaxConnections(MAX_CONNECTIONS);
//  config.setReadTimeout(5.0);    // in seconds
//  config.setMetricsInterval(10.0); // seconds
//  config.setMaxWriteCache(1<<10);  // 1Mb
//  config.setIncomingMessageSizes(4, 100, 1024);
//
//
//  btemt_ChannelQueuePool::ParseMessagesCallback cb(
//          bdef_BindUtil::bindA(&testAllocator,
//                               &cb,
//                               _1, _2, _3, _4,
//                               deprecatedParseMessages,
//                               config.maxIncomingMessageSize()));
//
//  bcec_Queue<btemt_Message> incoming, outgoing;
//  btemt_ChannelQueuePool qp(&incoming, &outgoing, cb, config);
//
//  assert(0 == qp.start());   // fails if no system resources.
//  if (0 != qp.listen(PORT_NUMBER, MAX_CONNECTIONS, SERVER_ID)) {
//      cout << "Can't open server port." << endl;
//      qp.stop();
//      return -1;
//  }
//
//  while(1) {
//      btemt_Message msg = incoming.popFront();
//      switch(msg.type()) {
//        case btemt_Message::BTEMT_CHANNEL_STATE: {
//          const btemt_ChannelMsg& data = msg.channelMsg();
//          cout << data << endl;
//        } break;
//        case btemt_Message::BTEMT_POOL_STATE: {
//          cout << msg.poolMsg() << endl;
//        } break;
//        case btemt_Message::BTEMT_TIMER: {
//          cout << msg.timerMsg().timerId() << endl;
//        } break;
//        case btemt_Message::BTEMT_DATA: {
//          // PROCESS DATA MESSAGE HERE
//..
// Because we are not using zero-copy semantics, the incoming 'msg' may contain
// more than one protocol defined message; however, since we are simply echoing
// data back to the client, we can safely ignore this possible complication.
//..
//          outgoing.pushBack(msg);
//        } break;
//     }
//  }
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTEMT_CHANNELPOOL
#include <btemt_channelpool.h>
#endif

#ifndef INCLUDED_BCEC_QUEUE
#include <bcec_queue.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

namespace BloombergLP {

class bslma_Allocator;
class btemt_Message;

                        // ============================
                        // class btemt_ChannelQueuePool
                        // ============================

class btemt_ChannelQueuePool {
    // This class provides a queue-based message transport similar to a channel
    // pool (see the component 'btemt_channelpool').  This channel queue pool
    // allows the establishment of both server channels (see section "Server
    // part" of the manipulators section) via the 'listen' and 'close' methods,
    // and client channels (see section "Client part") via the 'connect'
    // method, and provides the 'shutdown' method for managing the channels
    // once they have been created (see the section "Channel management").
    // Unlike channel pools, messages cannot be directly read from or written
    // to a channel; instead, they are read from and written into channels
    // using an incoming and outgoing queue passed at construction of a channel
    // queue pool.  Like channel pools, this channel queue pool can dispatch
    // events to be executed at different times at recurring intervals (see the
    // section "Clock management"), can be started or stopped (see the section
    // "Threads management") with the same effects as with a channel pool, and
    // keeps a set of metrics (see the "Metrics" section).  It can be
    // configured at construction by passing a 'btemt_ChannelPoolConfiguration'
    // object.

  public:
    typedef bdef_Function<void (*)(int *, int *, const char *, int)>
                                                         ParseMessagesCallback;
        // The callback of this type is invoked by the channel queue pool to
        // distinguish the message boundaries of the incoming data according to
        // the application level protocol.  The prototype for a
        // 'ParseMessageCallback' might look like:
        //..
        //  void parseMessageCallback(int        *numBytesConsumed,
        //                            int        *numBytesNeeded,
        //                            const char *firstDataBuffer,
        //                            int         totalLength);
        //..
        // Such a callback should load into the specified 'numBytesConsumed'
        // the number of bytes that make up the next protocol-defined message,
        // and load into 'numBytesNeeded' the number of additional bytes
        // required before the subsequent message boundary can be determined,
        // using the specified 'firstDataBuffer' of a chain of data buffers of
        // the specified 'totalLength', containing the incoming data read from
        // the channel.  If the returned '*numBytesConsumed > 0' the channel
        // queue pool will enqueue a 'btemt_DataMsg' containing the first
        // '*numBytesConsumed' bytes of the data chain, the remaining bytes in
        // the data chain (i.e., 'totalLength - *numBytesConsumed') will be
        // cached by the channel pool.  The channel queue pool will invoke this
        // callback again once '*numBytesNeeded' additional bytes have been
        // read from the channel or, if '*numBytesNeeded' is 0, will invoke
        // this callback again immediately with the remaining (unconsumed)
        // bytes.  If the implementation of 'ParseMessageCallback' sets
        // '*numBytesNeeded' to 0 (i.e., uses the 'zero-copy' semantics),
        // 'firstDataBuffer' is guaranteed to contain the *minimum* of
        // 'totalLength' and the configured 'minIncomingMessageSize()'
        // sequential bytes; the behavior is undefined if an address past this
        // point in 'firstDataBuffer' is accessed.  If the implemented callback
        // never sets '*numBytesNeeded' to 0 (i.e., uses the deprecated
        // original semantics of this component), 'firstDataBuffer' is
        // guaranteed to contain the *minimum* of 'totalLength' and the
        // configured 'maxIncomingMessageSize()' sequential bytes; the behavior
        // is undefied if an address past this point in 'firstDataBuffer' is
        // accessed.

  private:
    // DATA
    btemt_ChannelPool                     *d_channelPool_p;
    btemt_ChannelPool::PoolStateChangeCallback
                                           d_poolCbFunctor;
    btemt_ChannelPool::ChannelStateChangeCallback
                                           d_channelCbFunctor;
    btemt_ChannelPool::DataReadCallback    d_dataCbFunctor;

    bcec_Queue<btemt_Message>             *d_incomingQueue_p;
    bcec_Queue<btemt_Message>             *d_outgoingQueue_p;

    ParseMessagesCallback                  d_userCallback;
    int                                    d_minIncomingMessageSize;

    bcemt_ThreadUtil::Handle               d_processorHandle;
    bces_AtomicUtil::Int                   d_runningFlag;
    bdet_TimeInterval                      d_workTimeout;
    bslma_Allocator                       *d_allocator_p;

    // PRIVATE MANIPULATORS
    void poolStateCb(int state, int source, int severity);
        // Create a pool event message having the specified 'state', and the
        // specified 'source' and enqueue it onto the incoming queue.

    void channelStateCb(int channelId, int sourceId, int state, void *context);
        // Create a channel event message having the specified 'state', and
        // associated with the channel with the specified 'channelId' (created
        // through the specified 'sourceId', and enqueue it onto the incoming
        // queue.

    void dataCb(int           *numBytesConsumed,
                int           *numNeeded,
                btemt_DataMsg  msg,
                void          *channelContext);
        // Create a data message, if at least one application-level message is
        // present in the specified 'msg', load into the specified
        // 'numBytesConsumed' the number of bytes consumed, and enqueue the
        // data message onto the incoming queue.

    void timerCb(int timerId);
        // Create a timer message associated with the timer with the specified
        // 'timerId' and enqueue it onto the incoming queue.

  private:
    // NOT IMPLEMENTED
    btemt_ChannelQueuePool(const btemt_ChannelQueuePool& original);
    btemt_ChannelQueuePool& operator=(const btemt_ChannelQueuePool& rhs);

  public:
    // CREATORS
    btemt_ChannelQueuePool(
                    bcec_Queue<btemt_Message>             *incomingQueue,
                    bcec_Queue<btemt_Message>             *outgoingQueue,
                    ParseMessagesCallback                  callback,
                    const btemt_ChannelPoolConfiguration&  parameters,
                    bslma_Allocator                       *basicAllocator = 0);
        // Create a channel queue pool that uses the specified 'incomingQueue'
        // for incoming messages, the specified 'outgoingQueue' for outgoing
        // messages, the specified 'callback' to distinguish message
        // boundaries, and the specified configuration 'parameters.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless both 'incomingQueue' and
        // 'outgoingQueue' are not 0 and are not the same.  The behavior is
        // also undefined unless 'parameters.minIncomingMessageSize()' is large
        // enough such that the size of a complete message can be determined
        // from the first 'minIncomingMessageSize()' bytes of a received
        // message in the protocol message format.

    ~btemt_ChannelQueuePool();
        // Destroy this channel queue pool.

    // MANIPULATORS
                                   // *** Server part ***

    int close(int serverId);
        // Close the listening socket corresponding to the specified
        // 'serverId'.  Return 0 on success, and a non-zero value otherwise.
        // Note that closing a listening socket has no effect on any channels
        // managed by this pool.

    int listen(int                      port,
               int                      backlog,
               int                      serverId,
               int                      reuseAddress = 1);
    int listen(int                      port,
               int                      backlog,
               int                      serverId,
               const bdet_TimeInterval& timeout,
               int                      reuseAddress = 1);
        // Establish a listening socket having the specified 'backlog' maximum
        // number of pending connections on the specified 'port' on all local
        // interfaces and associate this newly established socket with the
        // specified 'serverId'.  Optionally, specify a 'timeout' for accepting
        // a connection, after which a pool state message is enqueued with
        // event equal to 'ACCEPT_TIMEOUT'.  Optionally specify a
        // 'reuseAddress' value to be used in setting REUSEADDRESS socket
        // option.  If 'reuseAddress' is not specified, 1 (i.e., REUSEADDRESS
        // is enabled) is used.  Return 0 on success, a positive value if there
        // is a listening socket associated with 'serverId' (i.e., 'serverId'
        // is not unique) and a negative value if an error occurred.  Every
        // time a connection is accepted by this pool on this
        // (newly-established) listening socket, 'serverId' is passed to the
        // callback provided in the configuration at construction.  The
        // behavior is undefined unless 0 < backlog.

    int listen(const bteso_IPv4Address& endpoint,
               int                      backlog,
               int                      id,
               int                      reuseAddress = 1);
    int listen(const bteso_IPv4Address& endpoint,
               int                      backlog,
               int                      id,
               const bdet_TimeInterval& timeout,
               int                      reuseAddress = 1);
        // Establish a listening socket having the specified 'backlog' maximum
        // number of pending connections on the specified 'endpoint' and
        // associate this newly established socket with the specified
        // 'serverId'.  Optionally, specify a 'timeout' for accepting a
        // connection, after which a pool state message is enqueued with event
        // equal to 'ACCEPT_TIMEOUT'.  Optionally specify a 'reuseAddress'
        // value to be used in setting REUSEADDRESS socket option.  If
        // 'reuseAddress' is not specified, 1 (i.e., REUSEADDRESS is enabled)
        // is used.  Return 0 on success, a positive value if there is a
        // listening socket associated with 'serverId' (i.e., 'serverId' is not
        // unique) and a negative value if an error occurred.  Every time a
        // connection is accepted by this pool on this (newly-established)
        // listening socket, 'serverId' is passed to the callback provided in
        // the configuration at construction.  The behavior is undefined unless
        // 0 < backlog.

                                   // *** Client part ***

    int connect(const bteso_IPv4Address& server,
                int                      numAttempts,
                const bdet_TimeInterval& interval,
                int                      id);
        // Asynchronously issue up to the specified 'numAttempts' connection
        // requests to a server at the specified 'address' with at least the
        // specified time 'interval' between each attempt.  Return 0 on
        // successful initiation and a non-zero value otherwise.  Whenever this
        // connection state changes (i.e., is established), the specified 'id'
        // is passed to a user-installed callback.  The behavior is undefined
        // unless 0 < numAttempts.

                                   // *** Channel management ***

    int shutdown(int channelId, btemt_ChannelPool::ShutdownMode mode);
        // Shut down the communication channel having the specified 'channelId'
        // in the specified 'mode' and return 0 on success, and a non-zero
        // value otherwise.  Note that shutting down a channel will deallocate
        // all system resources associated with 'channel' and subsequent
        // references to channel will result in undefined behavior.

                                   // *** Thread management ***

    int start();
        // Create internal threads that monitor network events and invoke
        // corresponding callbacks supplied (in the configuration) at
        // construction.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined if the internal threads are created (as
        // reflected by the pool's state), see 'state'.

    int stop();
        // Gracefully terminate the worker threads; return 0 on success and a
        // non-zero value otherwise.  If all attempts to terminate the threads
        // "gracefully" fail, a negative value is returned and the threads are
        // destroyed anyway.  The behavior is undefined unless the threads
        // exist (i.e., were created successfully).  Note that this function
        // has no effect on the state of any channel managed by this pool.

    int processOutgoingQueue();
        // Process messages from the outgoing queue.  NOTE: This function is
        // not to be called directly, and the behavior is undefined if it is
        // called from the user code.

                                   // *** Outgoing messages ***

    int clearSendBuffer(int channelId);
        // Clear the internal buffer for outgoing messages on the 'channel'
        // having the specified 'channelId' without corrupting the byte stream.
        // Return 0 on success, and a non-zero value otherwise.

                                   // *** Socket Options ***

    int getServerSocketOption(int *result,
                              int  option,
                              int  level,
                              int  serverId);
        // Load into the specified 'result' the value of the specified 'option'
        // of the specified 'level' socket option on the server socket having
        // the specified 'serverId'.  Return 0 on success and a non-zero value
        // otherwise.  (See 'bteso_SocketOptUtil' for the set of commonly-used
        // options.)

    int getSocketOption(int *result, int option, int level, int channelId);
        // Load into the specified 'result' the value of the specified 'option'
        // of the specified 'level' socket option on the channel having the
        // specified 'channelId'.  Return 0 on success and a non-zero value
        // otherwise.  (See 'bteso_SocketOptUtil' for the set of commonly-used
        // options.)

    int getLingerOption(bteso_SocketOptUtil::LingerData *result,
                        int                              channelId);
        // Load into the specified 'result', the value of the linger option for
        // the channel having the specified 'channelId'.  Return 0 on success
        // and a non-zero value otherwise.  The behavior is undefined if
        // 'result' is 0.

    int setSocketOption(int option, int level, int value, int channelId);
        // Set the specified 'option' (of the specified 'level') socket option
        // on the channel with the specified 'channelId' to the specified
        // 'value'.  Return 0 on success and a non-zero value otherwise.  (See
        // 'bteso_socketoptutil' for the list of commonly supported options.)

    int setServerSocketOption(int option, int level, int value, int serverId);
        // Set the specified 'option' (of the specified 'level') socket option
        // on the listening socket with the specified 'serverId' to the
        // specified 'value'.  Return 0 on success and a non-zero value
        // otherwise.  (See 'bteso_socketoptutil' for the list of commonly
        // supported options.)

    int setLingerOption(const bteso_SocketOptUtil::LingerData& value,
                        int                                    channelId);
        // Set the linger option on the channel with the specified 'channelId'
        // to the specified 'value'.  Return 0 on success and a non-zero value
        // otherwise.

                                  // *** Clock management ***

    int registerClock(const bdet_TimeInterval& startTime,
                      const bdet_TimeInterval& period,
                      int                      clockId);
        // Enqueue a timer message onto the incoming queue periodically with
        // the specified 'period' starting at the specified 'startTime'
        // absolute time and embed the specified 'clockId' into such a message.
        // Return 0 on success and a non-zero value otherwise; the return value
        // of 1 is reserved to indicate that 'clockId' is already registered.

    void deregisterClock(int clockId);
        // Stop enqueueing timer messages associated with the clock having the
        // specified 'clockId'.

    // ACCESSORS
                                   // *** Metrics ***

    int busyMetrics() const;
        // Return the (percent) value in the range [0..100] (inclusive) that
        // reflects the workload of this channel pool (e.g., how busy it is for
        // the last period).  0 indicates that the pool is idle and 100
        // indicates that pool operates at the configured capacity.

    void getLocalAddress(bteso_IPv4Address *result, int channelId);
        // Load, into the specified 'result', the complete IP address
        // associated with the local (i.e., this process) end-point of the
        // communication channel having the specified 'channelId'.

    void getPeerAddress(bteso_IPv4Address *result, int channelId);
        // Load, into the specified 'result', the complete IP address
        // associated with the remote (i.e., peer process) end-point of the
        // communication channel having the specified 'channelId'.

    int numChannels() const;
        // Return the number of channels currently managed by this channel
        // pool.

    int minIncomingMessageSize();
        // Return the minimum incoming message size specified at construction
        // of this object.  Note that this is used to guarantee the length of
        // the buffer that can safely be accessed in the 'parseMessages'
        // callback specified at construction.

    const bteso_IPv4Address *serverAddress(int id) const;
        // Return the address of the server IPv4 address for the server with
        // the specified 'id' that is managed by this channel pool if the
        // server is established, and 0 otherwise.  TBD - very awkward.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// MANIPULATORS
inline
int btemt_ChannelQueuePool::close(int serverId)
{
    return d_channelPool_p->close(serverId);
}

inline
int btemt_ChannelQueuePool::listen(int port, int backlog,
                                   int serverId, int reuseAddress)
{
    return d_channelPool_p->listen(port, backlog, serverId, reuseAddress);
}

inline
int btemt_ChannelQueuePool::listen(int port, int backlog, int serverId,
                                   const bdet_TimeInterval& timeout,
                                   int reuseAddress)
{
    return d_channelPool_p->listen(port, backlog, serverId, timeout,
                                   reuseAddress);
}

inline
int btemt_ChannelQueuePool::listen(const bteso_IPv4Address& endpoint,
                                   int backlog, int id, int reuseAddress)
{
    return d_channelPool_p->listen(endpoint, backlog, id, reuseAddress);
}

inline
int btemt_ChannelQueuePool::listen(const bteso_IPv4Address& endpoint,
                                   int backlog, int id,
                                   const bdet_TimeInterval& timeout,
                                   int reuseAddress)
{
    return d_channelPool_p->listen(endpoint, backlog, id, timeout,
                                   reuseAddress);
}

inline
int btemt_ChannelQueuePool::connect(const bteso_IPv4Address& server,
                                    int numAttempts,
                                    const bdet_TimeInterval& interval, int id)
{
    return d_channelPool_p->connect(server, numAttempts, interval, id);
}

inline
int btemt_ChannelQueuePool::shutdown(int channelId,
                                     btemt_ChannelPool::ShutdownMode mode) {
    return d_channelPool_p->shutdown(channelId, mode);
}

inline void btemt_ChannelQueuePool::deregisterClock(int clockId)
{
    d_channelPool_p->deregisterClock(clockId);
}

inline
int btemt_ChannelQueuePool::getServerSocketOption(int *result, int option,
                                                  int level,
                                                  int serverId)
{
    return d_channelPool_p->getServerSocketOption(result, option,
                                                  level, serverId);
}

inline
int btemt_ChannelQueuePool::getSocketOption(int *result, int option,
                                            int level, int channelId)
{
    return d_channelPool_p->getSocketOption(result, option,
                                            level, channelId);
}

inline
int btemt_ChannelQueuePool::getLingerOption(
        bteso_SocketOptUtil::LingerData *result,
        int channelId)
{
    return d_channelPool_p->getLingerOption(result, channelId);
}

inline
int btemt_ChannelQueuePool::setSocketOption(int option, int level, int value,
                                            int channelId)
{
    return d_channelPool_p->setSocketOption(option, level, value, channelId);
}

inline
int btemt_ChannelQueuePool::setServerSocketOption(int option, int level,
                                                  int value, int serverId)
{
    return d_channelPool_p->setServerSocketOption(option, level, value,
                                                  serverId);
}

inline
int btemt_ChannelQueuePool::setLingerOption(
        const bteso_SocketOptUtil::LingerData& value, int channelId)
{
    return d_channelPool_p->setLingerOption(value, channelId);
}

// ACCESSORS
inline
int btemt_ChannelQueuePool::busyMetrics() const
{
    return d_channelPool_p->busyMetrics();
}

inline
int btemt_ChannelQueuePool::numChannels() const
{
    return d_channelPool_p->numChannels();
}

inline
const bteso_IPv4Address *btemt_ChannelQueuePool::serverAddress(int id) const
{
    return d_channelPool_p->serverAddress(id);
}

inline
void btemt_ChannelQueuePool::getLocalAddress(bteso_IPv4Address *result,
                                             int                channelId)
{
    d_channelPool_p->getLocalAddress(result, channelId);
}

inline
void btemt_ChannelQueuePool::getPeerAddress(bteso_IPv4Address *result,
                                            int                channelId)
{
    d_channelPool_p->getPeerAddress(result, channelId);
}

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
