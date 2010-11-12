// btemt_channeloutstreambuf.h        -*-C++-*-
#ifndef INCLUDED_BTEMT_CHANNELOUTSTREAMBUF
#define INCLUDED_BTEMT_CHANNELOUTSTREAMBUF

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an output 'bsl::streambuf' for channel communication.
//
//@CLASSES:
//    btemt_ChannelOutStreamBuf: 'bsl::streambuf' for channel communication
//
//@AUTHOR: David Rubin (drubin6)
//
//@SEE_ALSO: btemt_channelpool, bcema_pooledbufferchain, btemt_DataMsg
//
//@DESCRIPTION: This class provides support for streaming data over TCP/IP
// communication channels.  'btemt_ChannelOutStreamBuf' implements the
// 'bsl::streambuf' protocol, and encapsulates a 'bcema_PooledBufferChain',
// which is the underlying data delivery mechanism employed by the
// 'btemt_ChannelPool' class.  Hence, clients can more easily send data between
// networked peers managed by a 'btemt_ChannelPool' using a common and
// ubiquitous streaming mechanism, ignoring the more complex details of the
// 'bcema_PooledBufferChain' and 'btemt_DataMsg' classes.
//
// Additionally, 'btemt_ChannelOutStreamBuf' enables clients to configure a
// secondary level of data caching by setting high- and low-water marks, and a
// "flush" timeout value, called the cache lifespan.  The high-water mark is
// specified as a multiple of the buffer size used to configure a
// 'bcema_PooledBufferChainFactory' from which the 'btemt_ChannelOutStreamBuf'
// allocates chained buffers, and indicates the maximum number of bytes the
// stream can buffer before data is sent due to an overflow condition.
// Similarly, the low-water mark indicates the minimum number of bytes required
// to be present in the streambuf before data is sent on a sync event.  This
// condition is mitigated by the cache lifespan, which allows data to be sent
// on a sync event if the cache lifespan is exceeded since the previous sync
// event.
//
// The cache configuration aspect of the 'btemt_ChannelOutStreamBuf' provides
// the benefits of increased throughput for small messages by caching writes
// (via the low-water mark), and decreased memory consumption for large
// messages by fixing the maximum number of buffers used (via the high-water
// mark).
//
///Class/Protocol Hierarchy
///------------------------
// The client streambuf implements a stream buffer, 'bsl::streambuf', coherent
// with the specifications in the C++ standard, section 27.5.
//..
//   ,-------------------------.
//  ( btemt_ChannelOutStreamBuf )
//   `-------------------------'
//               |
//               V
//        ,--------------.
//       ( bsl::streambuf )
//        `--------------'
//..
//
///Usage Examples
///--------------
// The first example presents a simple function which uses a
// 'btemt_ChannelOutStreamBuf' to echo a 'btemt_DataMsg'.  The second example
// focuses on the caching capabilities of the 'btemt_ChannelOutStreamBuf'.
//
///Example 1
///- - - - -
// This example illustrates how to use a 'btemt_ChannelOutStreamBuf' to echo a
// 'btemt_DataMsg'.  We present a function, 'echoMessage', which is
// parameterized by a 'btemt_DataMsg', a 'btemt_ChannelPool' which received the
// message, and a 'bcema_PooledBufferChainFactory'.  In this example, the data
// in the message is prepended by a 4-byte length.
//..
//  void echoMessage(
//          btemt_DataMsg&                  msg,
//          btemt_ChannelPool              *pool,
//          bcema_PooledBufferChainFactory *factory)
//  {
//      assert(pool);
//      assert(factory);
//
//      // Copy in message.
//      bsl::string message;
//      int         length     = msg.data()->length();
//      int         offset     = sizeof length;
//      int         bufferSize = msg.data()->bufferSize();
//      int         numBuffers = msg.data()->numBuffers();
//      for (int i = 0; i < numBuffers; ++i) {
//          int numBytes = (i < numBuffers - 1)
//                       ? bufferSize    // not the last buffer
//                       : (length - bufferSize * (numBuffers - 1)) %
//                                                            (1 + bufferSize);
//          message.append(msg.data()->buffer(i) + offset, numBytes);
//          offset = 0;
//      }
//
//      // Echo message.
//      enum {
//          // Setting the cache parameters to zero has the same
//          // effect as sending each buffer as it is filled.
//
//          LOW_WATER_MARK  = 0,
//          HIGH_WATER_MARK = 0,
//          CACHE_LIFESPAN  = 0
//      };
//      btemt_ChannelOutStreamBuf outbuf(msg.channelId(), pool, factory,
//                                       LOW_WATER_MARK,
//                                       HIGH_WATER_MARK,
//                                       CACHE_LIFESPAN);
//      bsl::ostream out(&outbuf);
//      out << message << flush;
//  }
//..
//
///Example 2
///- - - - -
// This example illustrates how to use a 'btemt_ChannelOutStreamBuf' object to
// cache and send objects represented as 'bdem_List's.  We present a server
// which accepts client connections and streams data to each client as the it
// becomes available.  Each datum is a 'bdem_List' containing a random number,
// and lifetime maximum and minimum value; and which conforms to the following
// schema:
//..
//  RECORD "Data" {
//    INT "value";
//  }
//..
//
// Each client connects to the server, and sends a message defining its
// throughput requirements.  This message is also a 'bdem_List', conforming to
// the following schema:
//..
//  RECORD "Connect" {
//    INT "cacheSize";
//    INT "maxDelay";
//  }
//..
// In this message, 'cacheSize' specifies the number of data messages the
// server should buffer before sending them to the client.  Additionally, the
// client specifies a 'maxDelay' between consecutive transmissions, after which
// the server should send any buffered data.  Practically speaking, the
// 'cacheSize' and 'maxDelay' should be functions of the client's real-time
// requirements, and inversely proportional to the client's bandwidth.
//
// The server is defined as follows:
//..
//  class my_DataServer {
//      // This class implements a server which delivers random numbers,
//      // generated at random intervals, to each client, according to
//      // client-specified bandwidth parameters.
//
//    private:
//      // TYPES
//      typedef bsl::map<int, btemt_ChannelOutStreamBuf*> MapType;
//          // This type is defined for notational convenience.  The first
//          // parameter specifies a channel ID.  The second parameter
//          // specifies a streambuf associated with the channel.
//
//    private:
//      int    d_maxClients;        // maximum number of pending connections
//      int    d_maxThreads;        // maximum number of worker threads
//      int    d_isRunning;         // 1 if channel pool threads are active
//      int    d_serverId;          // identifies the well-known server channel
//      int    d_pushClockId;       // ID of current push timer
//      double d_dataProbability;   // probability of generating data
//      int    d_bufferSize;        // size of pooled buffers
//
//      bslma_Allocator       *d_allocator_p;  // memory allocator (held)
//      btemt_ChannelPool     *d_pool_p;       // channel pool (owned)
//      bdem_Schema            d_schema;       // protocol schema
//      MapType                d_clients;      // map channel IDs to streambufs
//      bcemt_ReaderWriterLock d_lock;         // provide thread-safety
//      bdef_Function<void (*)()> d_dataCb;    // data generator callback
//      bdef_Function<void (*)()> d_pushCb;    // push timer callback
//      bdet_TimeInterval      d_lastPush;     // last push timer expiry
//
//      bcema_PooledBufferChainFactory
//                            *d_factory_p;    // shared by client streambufs
//
//    private:
//      // not implemented
//      my_DataServer(const my_DataServer&);
//      my_DataServer& operator=(const my_DataServer&);
//
//      // MANIPULATORS
//      void chanCb(int channelId, int serverId, int state, void *arg);
//      void dataCb(int *consumed, int *needed, btemt_DataMsg msg, void *arg);
//
//      void generateData();
//          // Generate a new data value (with some non-zero probability
//          // less than 1), and stream it to each client connection.  This
//          // function is called at each data clock expiry.
//
//      void pushData();
//          // Iterate over client connections, and flush the streambuf
//          // for each, potentially pushing out cached data.  This
//          // function is called periodically at each push timer expiry.
//
//      void updatePushTimer(int channelId = 0);
//          // Update the push timer by registering the lowest client-specified
//          // "maxDelay" (from the Connect request) with a clock ID
//          // corresponding to that clients' channel.  Optionally specify the
//          // 'channelId' of the most recently shut-down channel.  If
//          // 'channelId' is non-zero and equal to the current push timer
//          // clock ID, this clock ID must be deregistered before the new
//          // clock ID is registered.
//
//    public:
//      // CREATORS
//      my_DataServer(int              maxClients,
//                    int              maxThreads,
//                    double           dataProbability,
//                    bslma_Allocator *basicAllocator = 0);
//          // Create a server pool object which can process process any number
//          // of connections up to the specified 'maxClients', and service
//          // those connections using any number of threads up to the
//          // specified 'maxThreads'.  If there is at least one connection,
//          // generate (and stream) data every second with probability
//          // 'dataProbability'.  For example, a 'dataProbability' of 1/4
//          // indicates that the server should generate data every second with
//          // probability 0.25.  Optionally specify a 'basicAllocator' used
//          // to supply memory.  If 'basicAllocator' is 0, the default memory
//          // allocator is used.
//
//     ~my_DataServer();
//          // Terminate all open connections, and destroy this server.
//
//      // MANIPULATORS
//      int start(int                      serverId,
//                const bteso_IPv4Address& address,
//                int                      backlog);
//          // Start the server with the specified 'serverId' at the specified
//          // 'address', allowing a maximum number of pending connections
//          // specified by 'backlog'.  Return 0 on success, and a non-zero
//          // value if an error occurred.  The behavior is undefined unless
//          // 0 < 'backlog'.  In particular, the behavior is undefined if
//          // called on a server which is not stopped.
//
//      int stop();
//          // Stop this server.  Immediately terminate all connections, and
//          // remove all servers from listen-mode.  Return 0 if successful,
//          // and a non-zero value if an error occurs.  The behavior is
//          // undefined if called on a server which is stopped.
//  };
//
//..
//
// And the implementation:
//..
//  enum {
//      DATA_CLOCK_ID = -1
//          // ID of data generating clock.  Note that the push timer
//          // clock ID is equivalent to a channel ID, which is always
//          // positive.  Hence there can be no collisions.
//  };
//
//  void my_DataServer::chanCb(
//          int   channelId,
//          int   serverId,
//          int   state,
//          void *arg)
//  {
//      switch(state){
//        case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
//          d_lock.lockWrite();
//          MapType::iterator it = d_clients.find(channelId);
//          if (it != d_clients.end()) {
//              d_allocator_p->deleteObject(it->second);
//              d_clients.erase(it);
//          }
//          d_lock.unlock();
//          updatePushTimer(channelId);
//          d_pool_p->shutdown(channelId, btemt_ChannelPool::BTEMT_IMMEDIATE);
//        }  break;
//
//        case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
//          d_lock.lockWrite();
//          d_clients.insert(bsl::make_pair(channelId,
//                                            (btemt_ChannelOutStreamBuf *)0));
//          d_lock.unlock();
//        }  break;
//      }
//  }
//
//  void my_DataServer::dataCb(
//          int           *consumed,
//          int           *needed,
//          btemt_DataMsg  msg,
//          void          *arg)
//  {
//      // Validate 'msg' as a Connect request, and process it.
//
//      bcesb_PooledBufferChainStreamBuf inbuf(msg.data());
//      bdex_ByteInStreamFormatter       bis(&inbuf);
//      bdem_List                        message;
//      bis >> message;
//      assert(bis);
//
//      *needed   = 1;
//      *consumed = inbuf.pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::in);
//
//      if (!bdem_SchemaAggregateUtil::isListConformant(message,
//                                                      d_schema.record(0)))
//      {
//          d_pool_p->shutdown(msg.channelId(),
//                             btemt_ChannelPool::BTEMT_IMMEDIATE);
//          return;
//      }
//
//      bdem_ConstRowBinding connect(&message, &d_schema, "Connect");
//      int cacheLifespan = connect.theInt("maxDelay");
//      int highWaterMark = connect.theInt("cacheSize");
//      int lowWaterMark  = d_bufferSize * highWaterMark;
//
//      // The channel streambuf cannot be deleted while this thread holds a
//      // read lock, and no other thread can process a data callback for this
//      // channelId.  Therefore, we do not need a write lock.
//
//      d_lock.lockRead();
//      MapType::iterator it = d_clients.find(msg.channelId());
//      if (it == d_clients.end()) {
//          // The channel has already been closed.
//          d_lock.unlock();
//          return;
//      }
//
//      btemt_ChannelOutStreamBuf *outbuf = new (*d_allocator_p)
//                                   btemt_ChannelOutStreamBuf(msg.channelId(),
//                                                             d_pool_p,
//                                                             d_factory_p,
//                                                             lowWaterMark,
//                                                             highWaterMark,
//                                                             cacheLifespan,
//                                                             d_allocator_p);
//      it->second = outbuf;
//      d_lock.unlock();
//
//      updatePushTimer();
//  }
//
//  void my_DataServer::generateData()
//  {
//      int generate = (int)(bsl::rand() /
//                                (RAND_MAX + 1.0L) * (1 / d_dataProbability));
//      if (0 == generate) {
//          bdem_List datum;
//          int       value = rand();
//          datum.appendInt(value);
//          d_lock.lockWrite();
//          for (MapType::iterator it = d_clients.begin();
//               it != d_clients.end();
//               ++it)
//          {
//              bdex_ByteOutStreamFormatter bos(it->second);
//              bos << datum << bdexFlush;
//          }
//          d_lock.unlock();
//      }
//  }
//
//  void my_DataServer::pushData()
//  {
//      d_lock.lockWrite();
//      for (MapType::iterator it = d_clients.begin();
//           it != d_clients.end();
//           ++it)
//      {
//          it->second->pubsync();
//      }
//      d_lastPush = bdetu_SystemTime::now();
//      d_lock.unlock();
//  }
//
//  void my_DataServer::updatePushTimer(int channelId)
//  {
//      d_lock.lockWrite();
//      if (0 != channelId && channelId == d_pushClockId) {
//          d_pool_p->deregisterClock(d_pushClockId);
//      }
//
//      // Find channel with smallest maxDelay.
//      MapType::iterator it = d_clients.begin();
//      MapType::iterator kt = it;  // smallest maxDelay iterator
//      while (it != d_clients.end()) {
//          if (it->second->cacheLifespan() > 0
//           && it->second->cacheLifespan() < kt->second->cacheLifespan()) {
//              kt = it;
//          }
//          ++it;
//      }
//
//      if (kt != d_clients.end() && d_pushClockId != kt->first) {
//          d_pool_p->deregisterClock(d_pushClockId);
//
//          if (0 == d_pushClockId) {
//              // Start data generator clock.
//              d_pool_p->registerClock(d_dataCb,
//                                      bdetu_SystemTime::now(),
//                                      bdet_TimeInterval(1.0),
//                                      DATA_CLOCK_ID);
//              d_lastPush = bdetu_SystemTime::now();
//          }
//
//          if(0 < kt->second->cacheLifespan()) {
//              bdet_TimeInterval period(kt->second->cacheLifespan() / 1000.0);
//              d_pool_p->registerClock(d_pushCb,
//                                      d_lastPush + period,
//                                      period,
//                                      kt->first);
//              d_pushClockId = kt->first;
//          }
//      }
//
//      if (d_clients.empty()) {
//          d_pushClockId = 0;
//          d_pool_p->deregisterClock(DATA_CLOCK_ID);
//      }
//      d_lock.unlock();
//  }
//
//  // CREATORS
//  my_DataServer::my_DataServer(
//          int              maxClients,
//          int              maxThreads,
//          double           dataProbability,
//          bslma_Allocator *basicAllocator)
//  : d_maxClients(maxClients)
//  , d_maxThreads(maxThreads)
//  , d_dataProbability(dataProbability)
//  , d_isRunning(0)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
//  , d_pushClockId(0)
//  {
//      assert(0 < maxClients);
//      assert(0 < maxThreads);
//      assert(0 < dataProbability);
//
//      btemt_ChannelPool::ChannelStateChangeCallback ccb(
//              bdef_MemFnUtil::memFn(&my_DataServer::chanCb, this)
//            , d_allocator_p);
//
//      btemt_ChannelPool::DataReadCallback dcb(
//              bdef_MemFnUtil::memFn(&my_DataServer::dataCb, this)
//            , d_allocator_p);
//
//      btemt_ChannelPool::PoolStateChangeCallback pcb;
//
//      d_dataCb = bdef_Function<void (*)()>(
//              bdef_MemFnUtil::memFn(&my_DataServer::generateData, this)
//            , d_allocator_p);
//
//      d_pushCb = bdef_Function<void (*)()>(
//              bdef_MemFnUtil::memFn(&my_DataServer::pushData, this)
//            , d_allocator_p);
//
//      bdem_RecordDef *record = d_schema.createRecord("Connect");
//      assert(record->appendField(bdem_ElemType::BDEM_INT, "cacheSize"));
//      assert(record->appendField(bdem_ElemType::BDEM_INT, "maxDelay"));
//
//      record = d_schema.createRecord("Data");
//      assert(record->appendField(bdem_ElemType::BDEM_INT, "value"));
//
//      // Determine pooled buffer size using a mock connect request.
//      bdem_List connect;
//      connect.appendInt(0);
//      connect.appendInt(0);
//
//      bdex_ByteOutStream bos(d_allocator_p);
//      bos << connect;
//      d_bufferSize = bos.length();
//
//      d_factory_p = new (*d_allocator_p)
//                 bcema_PooledBufferChainFactory(d_bufferSize, d_allocator_p);
//
//      bslma_RawDeleterProctor<bcema_PooledBufferChainFactory,
//                              bslma_Allocator>
//                                     deleter(d_factory_p, d_allocator_p);
//
//      btemt_ChannelPoolConfiguration cpc;
//      cpc.setMaxConnections(d_maxClients);
//      cpc.setMaxThreads(d_maxThreads);
//      cpc.setMetricsInterval(10.0);
//      cpc.setIncomingMessageSizes(d_bufferSize, d_bufferSize, 1024);
//
//      d_pool_p = new (*d_allocator_p)
//                        btemt_ChannelPool(ccb, dcb, pcb, cpc, d_allocator_p);
//
//      deleter.release();
//  }
//
//  my_DataServer::~my_DataServer()
//  {
//      d_pool_p->stop();
//
//      // Stop servers.
//      if (0 < d_clients.size()) {
//          d_pool_p->deregisterClock(d_pushClockId);
//      }
//
//      // Deallocate streambufs.
//      for (MapType::iterator it = d_clients.begin();
//           it != d_clients.end();
//           ++it)
//      {
//          d_allocator_p->deleteObject(it->second);
//      }
//
//      d_allocator_p->deleteObject(d_factory_p);
//      d_allocator_p->deleteObject(d_pool_p);
//  }
//
//  // MANIPULATORS
//  int my_DataServer::start(
//          int                      serverId,
//          const bteso_IPv4Address& endpoint,
//          int                      backlog)
//  {
//      assert(0 < backlog);
//
//      int src = (d_isRunning) ? 0 : d_pool_p->start();
//      int lrc = d_pool_p->listen(endpoint, backlog, serverId);
//
//      if(0 == (src || lrc)){
//          d_serverId = serverId;
//          d_isRunning = 1;
//      }
//
//      return src || lrc;
//  }
//
//  int my_DataServer::stop()
//  {
//      int rc = d_pool_p->stop();
//
//      if(rc == 0){
//          d_isRunning = 0;
//      }
//      return rc;
//  }
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_POOLEDBUFFERCHAIN
#include <bcema_pooledbufferchain.h>
#endif

#ifndef INCLUDED_BTEMT_MESSAGE
#include <btemt_message.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

namespace BloombergLP {

                      // ===============================
                      // class btemt_ChannelOutStreamBuf
                      // ===============================

class btemt_ChannelPool;

class btemt_ChannelOutStreamBuf : public bsl::streambuf {
    // This class implements the output functionality of the 'bsl::streambuf'
    // protocol using a client-supplied 'bcema_PooledBufferChainFactory'.  TBD
    // doc

  public:
    // TYPES
    typedef bdef_Function<void (*)(const btemt_DataMsg&)> Callback;
        // This type defines a callback functor, which is invoked when data is
        // sent over the channel held by this streambuf.

  private:
    bslma_Allocator          *d_allocator_p;          // held
    bcema_PooledBufferChainFactory
                             *d_bufferChainFactory_p; // held
    bcema_PooledBufferChain  *d_bufferChain_p;        // owned
    btemt_ChannelPool        *d_channelPool_p;        // held
    btemt_DataMsg             d_dataMsg;         // message containing chain
    int                       d_channelId;       // allocated from channel pool
    int                       d_cacheLifespan;   // constraint on 'sync'; in ms
    int                       d_highWaterMark;   // constraint on 'overflow'
    int                       d_lowWaterMark;    // constraint on 'sync'
    Callback                  d_callback;        // user-specified callback
    int                       d_putBufferIndex;  // index into put area
    bsls_PlatformUtil::Int64  d_timeOfLastSend;

  private:
    // not implemented
    btemt_ChannelOutStreamBuf(const btemt_ChannelOutStreamBuf&);
    btemt_ChannelOutStreamBuf& operator=(const btemt_ChannelOutStreamBuf&);

  private:
    // PRIVATE MANIPULATORS
    int sendDataOverChannel();
        // Send the data in this streambuf over the channel held by this
        // object.  Return 0 if successful, and a non-zero value otherwise.

    void setPutPosition(off_type offset);
        // Set the next position for a writing into this streambuf to the
        // specified 'offset'.

    // PRIVATE ACCESSORS
    int calculatePutPosition() const;
        // Return the current position in the put area.

    int checkInvariant() const;
        // Return 0 if the invariants of this channel output stream buffer are
        // true.  The behavior is undefined unless the invariants of this
        // channel output stream buffer are true.

  protected:
    // PROTECTED MANIPULATORS
    virtual int_type overflow(int_type insertionChar =
                                           bsl::streambuf::traits_type::eof());
        // Append the optionally specified 'insertionChar' to this streambuf,
        // and return 'insertionChar'.  By default, 'traits_type::eof()' is
        // appended.

    virtual pos_type seekoff(
                           off_type                offset,
                           bsl::ios_base::seekdir  fixedPosition,
                           bsl::ios_base::openmode which = bsl::ios_base::in
                                                         | bsl::ios_base::out);
        // Set the location at which the next output will be written to the
        // specified 'offset' from the location indicated by the specified
        // 'fixedPosition'.  Return the location at which output will be next
        // written if 'which' is equal to the flag 'bsl::ios_base::out', and
        // 0 <= 'fixedPosition' + 'offset' and 'fixedPosition' + 'offset' <
        // 'length()'.  Return 'pos_type(off_type(-1))' otherwise.

    virtual pos_type seekpos(
                           pos_type                position,
                           bsl::ios_base::openmode which = bsl::ios_base::in
                                                         | bsl::ios_base::out);
        // Set the location at which the next output will be written to the
        // specified 'position'.  Return the location at which output will be
        // next written if 'which' includes the flag 'bsl::ios_base::out', and
        // 0 <= 'position' and 'position' < 'length()'.  Return
        // 'pos_type(off_type(-1))' otherwise.

    virtual int sync();
        // Transmit all data cached in this streambuf unless the data length is
        // less than the configured low water mark, and the time since the last
        // data transmission is less than the configured cache life span.
        // Return 0 if successful, and -1 otherwise.

    virtual bsl::streamsize xsputn(const char_type *source,
                                   bsl::streamsize  numChars);
        // Copy the specified 'numChars' from the specified 'source' to the
        // buffer chain held by this streambuf, starting at the current put
        // area location.  Cache or send the data to the peer associated with
        // the bound channel ID whenever writing would cause an overflow.
        // Return the total number of characters successfully written.  The
        // behavior is undefined unless 0 <= 'numChars'.

  public:
    // CREATORS
    btemt_ChannelOutStreamBuf(
                           int                             channelId,
                           btemt_ChannelPool              *channelPool,
                           bcema_PooledBufferChainFactory *bufferChainFactory,
                           int                             lowWaterMark,
                           int                             highWaterMark,
                           int                             cacheLifespan,
                           bslma_Allocator                *basicAllocator = 0);
        // Create a 'btemt_ChannelOutStreamBuf' object which can transmit on
        // the channel specified by 'channelId', which is associated with the
        // specified 'channelPool'.  Buffers for data messages generated by the
        // streambuf are allocated from the specified 'bufferChainFactory'.
        // 'lowWaterMark' indicates the minimum number of bytes which must be
        // cached in the streambuf before 'sync' will transmit the data.
        // 'highWaterMark' indicates the maximum number of bytes (specified as
        // a multiple of the 'bufferChainFactory''s buffer size) which can be
        // cached before 'overflow' transmits the data.  'cacheLifespan'
        // specifies the life span of the cached data in milliseconds.  If
        // 0 < 'cacheLifespan', and the streambuf length is less than
        // 'lowWaterMark', 'sync'ing the streambuf will *not* transmit the data
        // unless the 'cacheLifespan' has been exceeded since the last data
        // transmission (as a result of either 'sync' or 'overflow').
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the default memory allocator is used.  The
        // behavior is undefined unless '0 <= lowWaterMark',
        // '0 <= highWaterMark', and '0 <= cacheLifeSpan'.

    ~btemt_ChannelOutStreamBuf();
        // Destroy this streambuf.  Transmit any cached data over the channel.

    // MANIPULATORS
    int forceSync();
        // 'sync' this streambuf, ignoring all cache constraints.  Return 0 on
        // success, and a non-zero value otherwise.

    void setSendNotificationCallback(const Callback& callback);
        // Set the send notification callback to the specified 'callback'.

    // ACCESSORS
    int cacheLifespan() const;
        // Return the cache lifespan specified for this streambuf.

    int highWaterMark() const;
        // Return the high water mark specified for this streambuf.

    bsl::streamsize length() const;
        // Return the number of valid bytes in this streambuf.

    int lowWaterMark() const;
        // Return the low water mark specified for this streambuf.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

// PRIVATE ACCESSORS
inline
int btemt_ChannelOutStreamBuf::calculatePutPosition() const
{
    int bufferSize = d_bufferChain_p->bufferSize();
    return d_putBufferIndex * bufferSize + (pptr() - pbase());
}

// MANIPULATORS
inline
void btemt_ChannelOutStreamBuf::setSendNotificationCallback(
        const btemt_ChannelOutStreamBuf::Callback& callback)
{
    d_callback = callback;
}

// ACCESSORS
inline
int btemt_ChannelOutStreamBuf::cacheLifespan() const
{
    return d_cacheLifespan;
}

inline
int btemt_ChannelOutStreamBuf::highWaterMark() const
{
    return d_highWaterMark;
}

inline
bsl::streamsize btemt_ChannelOutStreamBuf::length() const
{
    return calculatePutPosition();
}

inline
int btemt_ChannelOutStreamBuf::lowWaterMark() const
{
    return d_lowWaterMark;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
