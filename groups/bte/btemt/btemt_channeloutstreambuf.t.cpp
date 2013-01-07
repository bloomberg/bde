// btemt_channeloutstreambuf.t.cpp            -*-C++-*-

#include <btemt_channeloutstreambuf.h>
#include <btemt_channelpoolconfiguration.h>
#include <btemt_channelpool.h>
#include <btemt_message.h>

#include <bdex_instreamfunctions.h>
#include <bdex_outstreamfunctions.h>

#include <bdem_binding.h>
#include <bdem_list.h>
#include <bdem_schema.h>
#include <bdem_schemaaggregateutil.h>
#include <bslma_allocator.h>
#include <bslma_rawdeleterproctor.h>
#include <bslma_default.h>
#ifdef BSLS_PLATFORM_CMP_MSVC
// some crazy thing defines ERROR to 0 !!!
#undef ERROR
#endif
#include <bsls_platformutil.h>
#include <bsls_timeutil.h>
#include <bdesb_fixedmeminstreambuf.h>
#include <bdesb_fixedmemoutstreambuf.h>
#include <bdex_byteinstreamformatter.h>
#include <bdex_byteoutstream.h>
#include <bdex_byteoutstreamformatter.h>
#include <bdex_bytestreamimputil.h>
#include <bcema_pooledbufferchain.h>
#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcemt_readerwriterlock.h>
#include <bcemt_thread.h>
#include <bcesb_pooledbufferchainstreambuf.h>
#include <bteso_ipv4address.h>
#include <bteso_inetstreamsocketfactory.h>
#include <bteso_streamsocket.h>
#include <btesos_tcpchannel.h>
#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>
#include <bdef_placeholder.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_cstdio.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;
using bsl::flush;

#ifdef BSLS_PLATFORM_CMP_MSVC
#undef IN
#undef OUT
#undef min
#undef max
#endif

using namespace BloombergLP;
using namespace bdef_PlaceHolders;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//
// 'btemt_ChannelOutStreamBuf' is exercised in this test driver from a
// server-side perspective, using a generic server framework, 'my_ServerPool'.
// The 'my_ServerPool' class allows clients to bind client-specific data
// callback functions.  This feature is used to bind a case-specific data
// callback function for each test case.  Almost all test cases conform to the
// following format:
//..
//   1. Instantiate a 'my_ServerPool' object bound to a case- specific data
//      callback function.
//   2. Create a server with specified buffer size.
//   3. Send one or more messages to the server.
//   4. Receive responses, or block on 'read' until test is complete.
//..
// The tests and verifications take place within the case-specific data
// callback function.
//-----------------------------------------------------------------------------
// 'btemt_ChannelOutStreamBuf' private interface
// MANIPULATORS
// [ 2] void sendDataOverChannel();
// [ 7] void setPutPosition(off_type offset);
//
// ACCESSORS
// [ 2] int calculatePutPosition() const;
// [ 2] int checkInvariant() const;
//
// 'btemt_ChannelOutStreamBuf' protected interface
// MANIPULATORS
// [ 6] int_type overflow(int_type insertionChar = traits_type::eof());
// [ 7] pos_type seekoff(off_type                offset,
//                       bsl::ios_base::seekdir  fixedPosition,
//                       bsl::ios_base::openmode which = bsl::ios_base::in
//                                                     | bsl::ios_base::out);
// [ 8] pos_type seekpos(pos_type                position,
//                       bsl::ios_base::openmode which = bsl::ios_base::in
//                                                     | bsl::ios_base::out);
// [ 6] int sync();
// [11] bsl::streamsize xsputn(const char_type *source,
//                             bsl::streamsize  numChars);
//
// 'btemt_ChannelOutStreamBuf' public interface
// CREATORS
// [ 2] btemt_ChannelOutStreamBuf(
//              int                             channelId,
//              btemt_ChannelPool              *channelPool,
//              bcema_PooledBufferChainFactory *bufferChainFactory,
//              int                             minBytesToSend,
//              int                             maxBuffers,
//              int                             msBetweenSends,
//              bslma_Allocator                *basicAllocator = 0);
// [ 2] ~btemt_ChannelOutStreamBuf();
//
// MANIPULATORS
// [12] int forceSync();
// [ 2] void setSendNotificationCb(const Callback& callback);
//
// ACCESSORS
// [ 4] int cacheLifespan() const;
// [ 4] int highWaterMark() const;
// [ 4] bsl::streamsize length() const;
// [ 4] int lowWaterMark() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] CONCERN: Data is sent when streambuf is destroyed
// [10] CONCERN: Cache lifespan
// [13] CONCERN: Failed write is handled
// [14] USAGE EXAMPLE 1
// [-1] USAGE EXAMPLE 2
//-----------------------------------------------------------------------------

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\t" \
                    << #M << ": " << M << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_()  cout << '\t' << flush;          // Print tab w/o newline.

// The following macros facilitate thread-safe streaming to standard output.

#define MTCOUT   coutMutex.lock(); { bsl::cout \
                                           << bcemt_ThreadUtil::selfIdAsInt() \
                                           << ": "
#define MTENDL   bsl::endl;  } coutMutex.unlock()
#define MTFLUSH  bsl::flush; } coutMutex.unlock()

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
typedef btemt_ChannelOutStreamBuf                        Obj;
typedef bcema_PooledBufferChainFactory                   BufferFactory;
typedef bcema_PooledBufferChain                          Chain;
typedef btemt_ChannelPoolConfiguration                   Config;
typedef btemt_ChannelPool                                Pool;
typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> AddressFactory;
typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

static bcemt_Mutex coutMutex;

//=============================================================================
//               GLOBAL CLASSES AND HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                            // ===================
                            // class my_ServerPool
                            // ===================

class my_ServerPool {
    // This class implements a pool of servers.  Each server is bound to the
    // same user-configurable data callback function, and optionally
    // configurable channel state callback function.  Each server in the pool
    // runs on top of a single 'btemt_ChannelPool' instance.

  public:
    // TYPES
    typedef btemt_ChannelPool::DataReadCallback DataReadCallback;
        // This type defines a functor which can be instantiated by a user,
        // and bound to the data callback function using the 'setDataCallback'
        // member function.  The data callback is called whenever a server
        // has data to process.

    typedef btemt_ChannelPool::ChannelStateChangeCallback ChannelCallback;
        // This type defines a functor which can be instantiated by a user,
        // and bound to the channel state callback function, using the
        // 'setChannelCallback' member function.  The channel state callback
        // is called whenever a new channel is created.

  private:
    // TYPES
    typedef bsl::map<int, bcema_PooledBufferChainFactory*> MapType;
        // This type is defined for notational convenience.  The first
        // parameter specifies a server ID, and the second parameter
        // specifies a pooled buffer chain factory.  Whenever a channel
        // is established for a particular server, the associated factory
        // is bound to the channel context.

  private:
    int d_maxClients;     // maximum number of pending connections
    int d_maxThreads;     // maximum number of worker threads
    int d_isRunning;      // 1 if channel pool threads are active

    bslma_Allocator       *d_allocator_p;  // memory allocator (held)
    btemt_ChannelPool     *d_pool_p;       // channel pool (owned)
    ChannelCallback        d_chanCb;       // user channel callback
    DataReadCallback       d_dataCb;       // user data callback
    MapType                d_factories;    // map servers to pooled
                                           // buffer chain factories
    bcemt_ReaderWriterLock d_lock;         // provide thread-safety

  private:
    // not implemented
    my_ServerPool(const my_ServerPool&);
    my_ServerPool& operator=(const my_ServerPool&);

    // MANIPULATORS
    void chanCb(int channelId, int serverId, int state, void *arg);
    void dataCb(int *consumed, int *needed, btemt_DataMsg msg, void *arg);

  public:
    // CREATORS
    my_ServerPool(int              maxClients,
                  int              maxThreads,
                  bslma_Allocator *basicAllocator = 0);
        // Create a server pool object which can process process any number
        // of connections up to the specified 'maxClients', and service those
        // connections using any number of threads up to the specified
        // 'maxThreads'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the default memory allocator is
        // used.

   ~my_ServerPool();
        // Terminate all open connections, and destroy this server.

    // MANIPULATORS
    void setChannelCallback(const ChannelCallback& callback);
        // Set the channel callback to the specified 'callback'.  The
        // 'callback' is executed in response to channel state changes from
        // within 'chanCb'.  The behavior is undefined if this function is
        // called while the server is running.

    void setDataCallback(const DataReadCallback& callback);
        // Set the data callback to the specified 'callback'.  The 'callback'
        // 'is executed in response to channel state changes from within
        // 'dataCb'.  The behavior is undefined if this function is called
        // while the server is running.

    int start(int                      serverId,
              int                      bufferSize,
              const bteso_IPv4Address& address,
              int                      backlog);
        // Create a server with the specified 'serverId' having a pooled buffer
        // chain factory with the specified 'bufferSize'.  Start the server at
        // the specified 'address', allowing a maximum number of pending
        // connections specified by 'backlog'.  Return 0 on success, and a
        // non-zero value if an error occurred.  The behavior is undefined
        // unless 0 < 'backlog'.  In particular, the behavior is undefined if
        // called on a server which is not stopped.

    int stop();
        // Stop this server pool.  Immediately terminate all connections for
        // all servers, and remove all servers from listen-mode.  Return 0 if
        // successful, and a non-zero value if an error occurs.  The behavior
        // is undefined if called on a server which is stopped.

    // ACCESSORS
    btemt_ChannelPool *pool() const;
        // Return the address of the channel pool owned by this server.
        // This is useful for attaching channel state callbacks such as
        // metrics monitors.

};

//-----------------------------------------------------------------------------
//                                IMPLEMENTATION
//-----------------------------------------------------------------------------

// PRIVATE MANIPULATORS
void my_ServerPool::chanCb(int channelId, int serverId, int state, void *arg)
{
    if (d_chanCb) {
        d_chanCb(channelId, serverId, state, d_pool_p);
    }

    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
          d_pool_p->shutdown(channelId, btemt_ChannelPool::BTEMT_IMMEDIATE);
      }  break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
          d_lock.lockRead();
          MapType::iterator it = d_factories.find(serverId);
          ASSERT(it != d_factories.end());
          d_pool_p->setChannelContext(channelId, it->second);
          d_lock.unlock();
      }  break;
    }
}

void my_ServerPool::dataCb(
        int           *consumed,
        int           *needed,
        btemt_DataMsg  msg,
        void          *arg)
{
    ASSERT(d_dataCb);

    d_dataCb(consumed, needed, msg, arg);
}

// CREATORS
my_ServerPool::my_ServerPool(
        int              maxClients,
        int              maxThreads,
        bslma_Allocator *basicAllocator)
: d_maxClients(maxClients)
, d_maxThreads(maxThreads)
, d_isRunning(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    ASSERT(0 < maxClients);
    ASSERT(0 < maxThreads);

    btemt_ChannelPool::ChannelStateChangeCallback ccb(
            bdef_MemFnUtil::memFn(&my_ServerPool::chanCb, this)
          , d_allocator_p);

    btemt_ChannelPool::DataReadCallback dcb(
            bdef_MemFnUtil::memFn(&my_ServerPool::dataCb, this)
          , d_allocator_p);

    btemt_ChannelPoolConfiguration cpc;
    cpc.setMaxConnections(d_maxClients);
    cpc.setMaxThreads(d_maxThreads);
    cpc.setMetricsInterval(10.0);
    cpc.setIncomingMessageSizes(4, 4, 1024);

    btemt_ChannelPool::PoolStateChangeCallback pcb;

    d_pool_p = new (*d_allocator_p)
                          btemt_ChannelPool(ccb, dcb, pcb, cpc, d_allocator_p);
}

my_ServerPool::~my_ServerPool()
{
    d_pool_p->stop();

    // Stop servers.
    for (MapType::iterator it = d_factories.begin();
         it != d_factories.end();
         ++it)
    {
        d_pool_p->close(it->first);
    }

    // Deallocate factories.
    for (MapType::iterator it = d_factories.begin();
         it != d_factories.end();
         ++it)
    {
        d_allocator_p->deleteObject(it->second);
    }

    d_allocator_p->deleteObjectRaw(d_pool_p);
}

// MANIPULATORS
int my_ServerPool::start(
        int                      serverId,
        int                      bufferSize,
        const bteso_IPv4Address& endpoint,
        int                      backlog)
{
    ASSERT(0 < bufferSize);
    ASSERT(0 < backlog);

    d_lock.lockReadReserveWrite();
    MapType::iterator it = d_factories.find(serverId);
    if (it != d_factories.end()) {
        d_lock.unlock();
        return -666;
    }
    d_lock.upgradeToWriteLock();

    bcema_PooledBufferChainFactory *factory = new (*d_allocator_p)
                     bcema_PooledBufferChainFactory(bufferSize, d_allocator_p);

    bslma_RawDeleterProctor<bcema_PooledBufferChainFactory, bslma_Allocator>
                                               deleter(factory, d_allocator_p);

    bsl::pair<MapType::iterator,bool> result =
                         d_factories.insert(bsl::make_pair(serverId, factory));
    deleter.release();

    int src = (d_isRunning) ? 0 : d_pool_p->start();
    int lrc = d_pool_p->listen(endpoint, backlog, serverId);

    if(0 == (src || lrc)){
        d_isRunning = 1;
    }
    else {
        if (result.second) {
            d_allocator_p->deleteObject(factory);
            d_factories.erase(result.first);
        }
    }
    d_lock.unlock();

    return src || lrc;
}

int my_ServerPool::stop()
{
    int rc = d_pool_p->stop();

    if(rc == 0){
        d_isRunning = 0;
    }
    return rc;
}

inline
void my_ServerPool::setChannelCallback(const ChannelCallback& callback)
{
    d_chanCb = callback;
}

inline
void my_ServerPool::setDataCallback(const DataReadCallback& callback)
{
    d_dataCb = callback;
}

// ACCESSORS
btemt_ChannelPool *my_ServerPool::pool() const
{
    return d_pool_p;
}

//-----------------------------------------------------------------------------
static
void channelStateMonitor(int channelId, int serverId, int state, void *arg)
{
    // Monitor the channel state transitions of the specified channel
    // pool, pointed to by 'arg', which has type 'btemt_ChannelPool'.
    // Display the 'channelId' and network address of channels whose
    // 'state' is UP, and the 'channelId' of channels whose 'state'
    // is DOWN.

    ASSERT(arg);

    btemt_ChannelPool *pool = static_cast<btemt_ChannelPool*>(arg);

    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
          MTCOUT << "Connection terminated:"
                 << " serverId="  << bsl::hex << serverId << bsl::dec
                 << " channelId=" << channelId
                 << MTENDL;
      }  break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
          bteso_IPv4Address channel;
          char              address[16+1];

          pool->getPeerAddress(&channel, channelId);
          channel.loadIpAddress(address);
          MTCOUT << "Connection established:"
                 << " serverId="  << bsl::hex << serverId << bsl::dec
                 << " channelId=" << channelId
                 << " address="   << address << ":" << channel.portNumber()
                 << MTENDL;
          pool->setChannelContext(channelId, 0);
      }  break;
    }
}

//=============================================================================
//       CASE-SPECIFIC TYPES, HELPER FUNCTIONS, AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------
void echoMessage(
        btemt_DataMsg&                  msg,
        btemt_ChannelPool              *pool,
        bcema_PooledBufferChainFactory *factory)
{
    ASSERT(pool);
    ASSERT(factory);

    // Copy in message.
    bsl::string message;
    int         length     = msg.data()->length();
    int         offset     = sizeof length;
    int         bufferSize = msg.data()->bufferSize();
    int         numBuffers = msg.data()->numBuffers();
    for (int i = 0; i < numBuffers; ++i) {
        int numBytes = (i < numBuffers - 1)
                     ? bufferSize    // not the last buffer
                     : (length - bufferSize * (numBuffers - 1)) %
                                                              (1 + bufferSize);
        message.append(msg.data()->buffer(i) + offset, numBytes);
        offset = 0;
    }

    // Echo message.
    enum {
        // Setting the cache parameters to zero has the same
        // effect as sending each buffer as it is filled.

        LOW_WATER_MARK  = 0,
        HIGH_WATER_MARK = 0,
        CACHE_LIFESPAN  = 0
    };
    btemt_ChannelOutStreamBuf outbuf(msg.channelId(), pool, factory,
                                     LOW_WATER_MARK,
                                     HIGH_WATER_MARK,
                                     CACHE_LIFESPAN);
    bsl::ostream out(&outbuf);
    out << message << flush;
}

static
void case14DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    if (veryVerbose) {
        MTCOUT << "Received message:"
               << " channelId=" << msg.channelId()
               << " length="    << msg.data()->length()
               << MTENDL;
    }

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    // Find Message Boundary
    int length;
    bdex_ByteStreamImpUtil::getInt32(&length, msg.data()->buffer(0));
    ASSERT(0 < length);
    if(length > (int)(msg.data()->length() - sizeof length)){
        *needed = length - msg.data()->length() + sizeof length;
        *consumed = 0;
        return;
    }
    *needed = sizeof length;
    *consumed = length + sizeof length;

    echoMessage(msg, pool, factory);
}

static
void case13SendNotification(
        const btemt_DataMsg& msg,
        int                 *buffersSent)
{
    if (veryVerbose) {
        MTCOUT << "Sending data: "
               << "length = "     << msg.data()->length()     << ", "
               << "numBuffers = " << msg.data()->numBuffers() << ", "
               << "bufferSize = " << msg.data()->bufferSize() << ", "
               << "totalSent = "  << *buffersSent + msg.data()->numBuffers()
               << MTENDL;
    }
    *buffersSent += msg.data()->numBuffers();
}

static
void case13ChannelStateCallback(
    int                 channelId,
    int                 serverId,
    int                 state,
    void               *arg,
    btemt_ChannelPool **poolAddr,
    bcemt_Barrier      *barrier)
{
    // Monitor the channel state transitions of the specified channel
    // pool, pointed to by 'arg', which has type 'btemt_ChannelPool'.
    // Display the 'channelId' and network address of channels whose
    // 'state' is UP, and the 'channelId' of channels whose 'state'
    // is DOWN.  Upon receiving a 'CHANNEL_UP' event, stream data until
    // the stream becomes invalid.

    ASSERT(poolAddr && *poolAddr);
    ASSERT(barrier);

    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (verbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }

        ASSERT(arg);
        BufferFactory   *factory = static_cast<BufferFactory*>(arg);
        bslma_Allocator *deleter = bslma_Default::allocator();
        deleter->deleteObject(factory);
        barrier->wait();
      }  break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        btemt_ChannelPool *pool = *poolAddr;
        if (verbose) {
            bteso_IPv4Address  channel;
            char               address[16+1];

            pool->getPeerAddress(&channel, channelId);
            channel.loadIpAddress(address);
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << " address="   << address << ":" << channel.portNumber()
                   << MTENDL;
        }

        enum {
            BUFFER_SIZE     = 256,
            LOW_WATER_MARK  = INT_MAX,
            HIGH_WATER_MARK = INT_MAX,
            CACHE_LIFESPAN  = 0
        };

        bslma_Allocator *allocator = bslma_Default::allocator();
        BufferFactory   *factory = new (*allocator)
                                         BufferFactory(BUFFER_SIZE, allocator);
        {
            Obj mX(channelId, pool, factory,
                   LOW_WATER_MARK,
                   HIGH_WATER_MARK,
                   CACHE_LIFESPAN,
                   allocator);

            int           buffersSent = 0;
            Obj::Callback cb(bdef_BindUtil::bindA( allocator
                                                 , &case13SendNotification
                                                 , _1
                                                 , &buffersSent));

            mX.setSendNotificationCallback(cb);

            const bsl::string HASHMARKS(BUFFER_SIZE, '#');
            bsl::ostream      out(&mX);

            while(out << HASHMARKS << flush) {
                ASSERT(0 == mX.length());
            }

            ASSERT(out.fail());
            out << HASHMARKS << flush;
            ASSERT(out.fail());

            if (verbose) {
                MTCOUT << buffersSent << " buffers sent." << MTENDL;
            }
        }
        pool->setChannelContext(channelId, factory);
        ASSERT(0 == pool->shutdown(channelId,
                                   btemt_ChannelPool::BTEMT_IMMEDIATE));
      }  break;

      case btemt_ChannelPool::BTEMT_WRITE_BUFFER_FULL: {
        if (verbose) {
            MTCOUT << "Write buffer is full for " << channelId << MTENDL;
        }
      }
    }
}

static
void case12SendNotification(
        const btemt_DataMsg& msg,
        int                 *buffersSent)
{
    if (veryVerbose) {
        MTCOUT << "Data sent: "
               << "length = "     << msg.data()->length()     << ", "
               << "numBuffers = " << msg.data()->numBuffers() << ", "
               << "bufferSize = " << msg.data()->bufferSize()
               << MTENDL;
    }
    *buffersSent += msg.data()->numBuffers();
}

static
void case12DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    if (veryVerbose) {
        MTCOUT << "Received message:"
               << " channelId=" << msg.channelId()
               << " length="    << msg.data()->length()
               << MTENDL;
    }

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    // Find buffer size.
    int bufferSize = 0;
    bdex_ByteStreamImpUtil::getInt32(&bufferSize, msg.data()->buffer(0));
    ASSERT(0 < bufferSize);
    *needed = 1;
    *consumed = msg.data()->length();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        MTCOUT << "\nTesting 'forceSync'."
               << "\n--------------------" << MTENDL;
    }
    {
        enum {
            NUM_ITERATIONS  = 6,    // number of 'sync' test iterations
            HIGH_WATER_MARK = 2,
            CACHE_LIFESPAN  = 3000  // 3 seconds
        };
        const int BUFFER_SIZE    = bufferSize;
        const int LOW_WATER_MARK = 2 * BUFFER_SIZE;

        Obj mX(msg.channelId(), pool, factory,
               LOW_WATER_MARK, HIGH_WATER_MARK, CACHE_LIFESPAN, allocator);
        const Obj& X = mX;

        int           buffersSent = 0;
        Obj::Callback cb(bdef_BindUtil::bindA( allocator
                                             , &case12SendNotification
                                             , _1
                                             , &buffersSent));
        mX.setSendNotificationCallback(cb);

        const bsl::string HASHMARKS(BUFFER_SIZE - 2, '#');
        bsl::ostream      out(&mX);
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            for (int j = 0; j < BUFFER_SIZE; ++j) {
                const int LENGTH = j + 1;
                out << '#' << flush;
                LOOP2_ASSERT(i, j, LENGTH == X.length());
            }
            LOOP_ASSERT(i, BUFFER_SIZE == X.length());
            LOOP_ASSERT(i, 0 == buffersSent);

            out << flush;
            LOOP_ASSERT(i, BUFFER_SIZE == X.length());
            LOOP_ASSERT(i, 0 == buffersSent);

            out << HASHMARKS << ends << flush;
            LOOP_ASSERT(i, 2 * BUFFER_SIZE - 1 == X.length());
            LOOP_ASSERT(i, 0 == buffersSent);

            mX.forceSync();
            LOOP_ASSERT(i, 0 == X.length());
            LOOP_ASSERT(i, 2 == buffersSent);
            buffersSent = 0;
        }
    }
}

static
void case11SendNotification(
        const btemt_DataMsg& msg,
        int                 *buffersSent)
{
    if (veryVerbose) {
        MTCOUT << "Data sent: "
               << "length = "     << msg.data()->length()     << ", "
               << "numBuffers = " << msg.data()->numBuffers() << ", "
               << "bufferSize = " << msg.data()->bufferSize()
               << MTENDL;
    }
    *buffersSent += msg.data()->numBuffers();
}

static
void case11DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    if (veryVerbose) {
        MTCOUT << "Received message:"
               << " channelId=" << msg.channelId()
               << " length="    << msg.data()->length()
               << MTENDL;
    }

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    // Find buffer size.
    int bufferSize = 0;
    bdex_ByteStreamImpUtil::getInt32(&bufferSize, msg.data()->buffer(0));
    ASSERT(0 < bufferSize);
    *needed = 1;
    *consumed = msg.data()->length();

    enum {
        LOW_WATER_MARK  = 0,
        HIGH_WATER_MARK = 3,
        CACHE_LIFESPAN  = 3000  // 3 seconds
    };
    const int BUFFER_SIZE = bufferSize;

    Obj mX(msg.channelId(), pool, factory,
           LOW_WATER_MARK, HIGH_WATER_MARK, CACHE_LIFESPAN, allocator);
    const Obj& X = mX;

    int           buffersSent = 0;
    Obj::Callback cb(bdef_BindUtil::bindA( allocator
                                         , &case11SendNotification
                                         , _1
                                         , &buffersSent));

    mX.setSendNotificationCallback(cb);

    const bsl::string HASHMARKS(BUFFER_SIZE, '#');
    for (int i = 0; i < HIGH_WATER_MARK; ++i) {
        const int LENGTH = i * BUFFER_SIZE;
        LOOP_ASSERT(i, LENGTH == X.length());

        if (veryVerbose) {
            MTCOUT << "i = "          << i      << ", "
                   << "LENGTH = "     << LENGTH << ", "
                   << "X.length() = " << X.length()
                   << MTENDL;
        }
        LOOP_ASSERT(i, BUFFER_SIZE == mX.sputn(HASHMARKS.c_str(),
                                               HASHMARKS.length()));
    }
    ASSERT(HIGH_WATER_MARK * BUFFER_SIZE == X.length());

    const int BUFFERS_SENT = X.length() / BUFFER_SIZE;
    ASSERT(HIGH_WATER_MARK == BUFFERS_SENT);

    mX.pubsync();

    ASSERT(BUFFERS_SENT == buffersSent);
    if (veryVerbose) {
        MTCOUT << "HWM = "          << HIGH_WATER_MARK     << ", "
               << "BUFFERS_SENT = " << BUFFERS_SENT        << ", "
               << "buffersSent = "  << buffersSent
               << MTENDL;
    }
}

static
void case10SendNotification(
        const btemt_DataMsg& msg,
        int                 *buffersSent)
{
    if (veryVerbose) {
        MTCOUT << "Data sent: "
               << "length = "     << msg.data()->length()     << ", "
               << "numBuffers = " << msg.data()->numBuffers() << ", "
               << "bufferSize = " << msg.data()->bufferSize()
               << MTENDL;
    }
    *buffersSent += msg.data()->numBuffers();
}

static
void case10DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    if (veryVerbose) {
        MTCOUT << "Received message:"
               << " channelId=" << msg.channelId()
               << " length="    << msg.data()->length()
               << MTENDL;
    }

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    // Find buffer size and cache lifespan.
    int bufferSize;
    int cacheLifespan;
    if ((int)(2 * sizeof(int)) > msg.data()->length()) {
        *needed = 2 * sizeof(int) - msg.data()->length();
        *consumed = 0;
        return;
    }
    bdex_ByteStreamImpUtil::getInt32(&bufferSize, msg.data()->buffer(0));
    bdex_ByteStreamImpUtil::getInt32(&cacheLifespan,
                                    msg.data()->buffer(0) + sizeof bufferSize);
    ASSERT(3 <= bufferSize);
    ASSERT(500 <= cacheLifespan);
    *needed = 1;
    *consumed = sizeof bufferSize + sizeof cacheLifespan;

    enum {
        MICROSECS_PER_SEC = 1000000,
        NANOSECS_PER_SEC  = 1000000000,
        RESOLUTION        = NANOSECS_PER_SEC / 1000,

        NUM_ITERATIONS    = 3,    // number of 'sync's before data is sent
        HIGH_WATER_MARK   = 2
    };
    const int BUFFER_SIZE       = bufferSize;
    const int CACHE_LIFESPAN    = cacheLifespan;          // in nanosecs
    const int CACHE_LIFESPAN_US = 1000 * CACHE_LIFESPAN;  // in microsecs
    const int LOW_WATER_MARK    = BUFFER_SIZE * HIGH_WATER_MARK;
    const int MAX_DATA_SIZE     = LOW_WATER_MARK - 2;
    const int PART_TIME         = CACHE_LIFESPAN_US / NUM_ITERATIONS - 50000;

    if (veryVerbose) {
        MTCOUT << "BUFFER_SIZE = "    << BUFFER_SIZE    << ", "
               << "CACHE_LIFESPAN = " << CACHE_LIFESPAN << ", "
               << "PART_TIME = "      << PART_TIME/1000 << ", "
               << "NUM_ITERATIONS = " << NUM_ITERATIONS << ", "
               << "BUFFERS_SENT = "   << HIGH_WATER_MARK
               << MTENDL;
    }

    Obj mX(msg.channelId(), pool, factory,
           LOW_WATER_MARK, HIGH_WATER_MARK, CACHE_LIFESPAN, allocator);
    const Obj& X = mX;

    int buffersSent = 0;
    Obj::Callback cb(bdef_BindUtil::bindA( allocator
                                         , &case10SendNotification
                                         , _1
                                         , &buffersSent));

    mX.setSendNotificationCallback(cb);

    bsl::string  HASHMARKS(MAX_DATA_SIZE, '#');
    bsl::ostream out(&mX);
    out << HASHMARKS << ends;
    ASSERT(LOW_WATER_MARK - 1 == X.length());
    ASSERT(0 == buffersSent);

    const bsls_PlatformUtil::Int64 START =
                                        bsls_TimeUtil::getTimer() / RESOLUTION;
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        mX.pubsync();
        const bsls_PlatformUtil::Int64 NOW =
                                        bsls_TimeUtil::getTimer() / RESOLUTION;
        if (NOW - START < CACHE_LIFESPAN) {
            // Only raise an error if we can *guarantee** that the condition
            // should not have occurred.  Long delays in nightly builds can
            // make this loop run longer than the lifespan, despite the
            // 50ms buffered into PART_TIME to prevent this happening.

            LOOP_ASSERT(i, LOW_WATER_MARK - 1 == X.length());
            LOOP_ASSERT(i, 0 == buffersSent);
        }
        bcemt_ThreadUtil::microSleep(PART_TIME % MICROSECS_PER_SEC,
                                     PART_TIME / MICROSECS_PER_SEC);
        if (veryVerbose) { P_(X.length()); P(buffersSent); }
    }
    const int LEFTOVER = CACHE_LIFESPAN_US - NUM_ITERATIONS * PART_TIME
                       + 50000;
    bcemt_ThreadUtil::microSleep(LEFTOVER % MICROSECS_PER_SEC,
                                 LEFTOVER / MICROSECS_PER_SEC);

    const bsls_PlatformUtil::Int64 END =
                                        bsls_TimeUtil::getTimer() / RESOLUTION;
    ASSERT(CACHE_LIFESPAN < END - START);

    mX.pubsync();
    ASSERT(0 == X.length());
    ASSERT(HIGH_WATER_MARK == buffersSent);
}

static
void case9DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    if (veryVerbose) {
        MTCOUT << "Received message:"
               << " channelId=" << msg.channelId()
               << " length="    << msg.data()->length()
               << MTENDL;
    }

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    // Find Message Boundary
    int length;
    bdex_ByteStreamImpUtil::getInt32(&length, msg.data()->buffer(0));
    ASSERT(0 < length);
    if(length > (int)(msg.data()->length() - sizeof length)){
        *needed = length - msg.data()->length() + sizeof length;
        *consumed = 0;
        return;
    }
    *needed = sizeof length;
    *consumed = length + sizeof length;

    // Copy in message.
    bsl::string message;
    int         offset = sizeof length;
    int         bufferSize = msg.data()->bufferSize();
    int         numBuffers = msg.data()->numBuffers();
    for (int i = 0; i < numBuffers; ++i) {
        int numBytes = (i < numBuffers - 1)
                     ? bufferSize    // not the last buffer
                     : (length - bufferSize * (numBuffers - 1)) %
                                                              (1 + bufferSize);
        message.append(msg.data()->buffer(i) + offset, numBytes);
        offset = 0;
    }
    ASSERT(length == (int)message.length());

    if (veryVerbose) {
        MTCOUT << "message.length() = " << message.length() << ", "
               << "message = "          << message
               << MTENDL;
    }

    // Echo message.
    enum {
        LOW_WATER_MARK  = 32,
        HIGH_WATER_MARK = 2,
        CACHE_LIFESPAN  = 3000    // 3 seconds
    };
    Obj mX(msg.channelId(), pool, factory,
           LOW_WATER_MARK, HIGH_WATER_MARK, CACHE_LIFESPAN, allocator);

    bsl::ostream out(&mX);
    out << message << ends;  // DO NOT FLUSH
}

static
void case8DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    // Find buffer size.
    int bufferSize = 0;
    bdex_ByteStreamImpUtil::getInt32(&bufferSize, msg.data()->buffer(0));
    ASSERT(0 < bufferSize);
    *needed = 1;
    *consumed = msg.data()->length();

    // Create streambuf.
    enum {
        HIGH_WATER_MARK = 3,
        CACHE_LIFESPAN  = 0
    };
    const int BUFFER_SIZE    = bufferSize;
    const int LOW_WATER_MARK = BUFFER_SIZE;
    const int MAX_DATA_SIZE  = BUFFER_SIZE * HIGH_WATER_MARK;

    Obj mX(msg.channelId(), pool, factory,
           LOW_WATER_MARK, HIGH_WATER_MARK, CACHE_LIFESPAN, allocator);

    // Fill streambuf.
    const bsl::string HASHMARKS(MAX_DATA_SIZE, '#');
    bsl::ostream      out(&mX);
    out << HASHMARKS;
    ASSERT(MAX_DATA_SIZE == mX.length());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    const int OUT  = bsl::ios_base::out;
    const int IN   = bsl::ios_base::in;
    const int FAIL = bsl::streambuf::pos_type(bsl::streambuf::off_type(-1));
    const int BS   = BUFFER_SIZE;

    ASSERT(BUFFER_SIZE >= 3);

    const struct {
        int d_line;             // source line number
        int d_position;         // seek position
        int d_which;            // seek direction
        int d_result;           // expected result from 'seek'
    } DATA[] = {
        // Verify 'seek' when 'which' includes 'IN'.

        //Line  Position     Which   Result
        //----  --------     -----   ------
        { L_,   0,           IN,     FAIL, },
        { L_,   0,           IN|OUT, 0,    },

        // Verify various 'seek's.

        //Line  Position     Which   Result
        //----  --------     -----   ------
        { L_,   0,           OUT,    0,          },
        { L_,   1 * BS,      OUT,    1 * BS,     },
        { L_,   2 * BS,      OUT,    2 * BS,     },
        { L_,   3 * BS,      OUT,    3 * BS,     },
        { L_,   -1,          OUT,    FAIL,       },
        { L_,   3 * BS + 1,  OUT,    FAIL,       },

        { L_,   1 * BS - 1,  OUT,    1 * BS - 1, },
        { L_,   2,           OUT,    2,          },
        { L_,   1 * BS - 3,  OUT,    1 * BS - 3, },
        { L_,   2 * BS - 5,  OUT,    2 * BS - 5, },
        { L_,   6,           OUT,    6,          },
    };
    enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

    if (veryVerbose) {
        T_(); P_(BUFFER_SIZE); P_(HIGH_WATER_MARK); P(MAX_DATA_SIZE);
    }

    for (int i = 0; i < DATA_SIZE; ++i) {
        const int LINE     = DATA[i].d_line;
        const int POSITION = DATA[i].d_position;
        const int WHICH    = DATA[i].d_which;
        const int RESULT   = DATA[i].d_result;

        int result = mX.pubseekpos(POSITION, (bsl::ios_base::openmode)WHICH);
        LOOP2_ASSERT(i, LINE, RESULT == result);

        if (veryVerbose) {
            T_(); P_(i); P_(LINE);
            P_(POSITION); P_(WHICH); P_(RESULT); P(result);
        }
    }
    pool->shutdown(msg.channelId(), btemt_ChannelPool::BTEMT_IMMEDIATE);
}

static
void case7DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    // Find buffer size.
    int bufferSize = 0;
    bdex_ByteStreamImpUtil::getInt32(&bufferSize, msg.data()->buffer(0));
    ASSERT(0 < bufferSize);
    *needed = 1;
    *consumed = msg.data()->length();

    // Create streambuf.
    enum {
        HIGH_WATER_MARK = 3,
        CACHE_LIFESPAN  = 0
    };
    const int BUFFER_SIZE    = bufferSize;
    const int LOW_WATER_MARK = BUFFER_SIZE;
    const int MAX_DATA_SIZE  = BUFFER_SIZE * HIGH_WATER_MARK;

    Obj mX(msg.channelId(), pool, factory,
           LOW_WATER_MARK, HIGH_WATER_MARK, CACHE_LIFESPAN, allocator);

    // Fill streambuf.
    const bsl::string HASHMARKS(MAX_DATA_SIZE, '#');
    bsl::ostream      out(&mX);
    out << HASHMARKS;
    ASSERT(MAX_DATA_SIZE == mX.length());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    const int CUR  = bsl::ios_base::cur;
    const int BEG  = bsl::ios_base::beg;
    const int END  = bsl::ios_base::end;
    const int OUT  = bsl::ios_base::out;
    const int IN   = bsl::ios_base::in;
    const int FAIL = bsl::streambuf::pos_type(bsl::streambuf::off_type(-1));
    const int BS   = BUFFER_SIZE;

    ASSERT(BUFFER_SIZE >= 3);

    const struct {
        int d_line;             // source line number
        int d_offset;           // seek offset
        int d_fixedPosition;    // starting position of seek
        int d_which;            // seek direction
        int d_result;           // expected result from 'seek'
    } DATA[] = {
        // Verify 'seek' when 'which' includes 'IN'.

        //Line  Offset       Fixed Position  Which   Result
        //----  ------       --------------  -----   ------
        { L_,   0,           BEG,            IN,     FAIL,  },
        { L_,   0,           CUR,            IN,     FAIL,  },
        { L_,   0,           END,            IN,     FAIL,  },
        { L_,   0,           BEG,            IN|OUT, 0,     },
        { L_,   0,           CUR,            IN|OUT, 0,     },
        { L_,   0,           END,            IN|OUT, 3 * BS, },

        // Verify various 'seek's when 'which' == 'BEG'.

        //Line  Offset       Fixed Position  Which   Result
        //----  ------       --------------  -----   ------
        { L_,   0,           BEG,            OUT,    0,      },
        { L_,   1 * BS,      BEG,            OUT,    1 * BS, },
        { L_,   2 * BS,      BEG,            OUT,    2 * BS, },
        { L_,   3 * BS,      BEG,            OUT,    3 * BS, },
        { L_,   -1,          BEG,            OUT,    FAIL,   },
        { L_,   3 * BS + 1,  BEG,            OUT,    FAIL,   },

        // Verify various 'seek' when 'which' == 'END'.

        //Line  Offset       Fixed Position  Which   Result
        //----  ------       --------------  -----   ------
        { L_,   0,           END,            OUT,    3 * BS, },
        { L_,   -1 * BS,     END,            OUT,    2 * BS, },
        { L_,   -2 * BS,     END,            OUT,    1 * BS, },
        { L_,   -3 * BS,     END,            OUT,    0,      },
        { L_,   1,           END,            OUT,    FAIL,   },
        { L_,   -3 * BS - 1, END,            OUT,    FAIL,   },

        // Verify 'seek' when 'which' == 'CUR'.  We use previously
        // verified 'seek's from 'BEG' and 'END' to adjust the
        // current position.

        //Line  Offset       Fixed Position  Which   Result
        //----  ------       --------------  -----   ------
        { L_,   0,           BEG,            OUT,    0,      },
        { L_,   0,           CUR,            OUT,    0,      },

        { L_,   0,           END,            OUT,    3 * BS, },
        { L_,   0,           CUR,            OUT,    3 * BS, },

        { L_,   0,           BEG,            OUT,    0,      },
        { L_,   1 * BS,      CUR,            OUT,    1 * BS, },
        { L_,   1 * BS,      CUR,            OUT,    2 * BS, },
        { L_,   1 * BS,      CUR,            OUT,    3 * BS, },
        { L_,   1,           CUR,            OUT,    FAIL,   },

        { L_,   0,           END,            OUT,    3 * BS, },
        { L_,   -1 * BS,     CUR,            OUT,    2 * BS, },
        { L_,   -1 * BS,     CUR,            OUT,    1 * BS, },
        { L_,   -1 * BS,     CUR,            OUT,    0,      },
        { L_,   -1,          CUR,            OUT,    FAIL,   },

        { L_,   0,           BEG,            OUT,    0,          },
        { L_,   1 * BS - 1,  CUR,            OUT,    1 * BS - 1, },
        { L_,   2,           CUR,            OUT,    1 * BS + 1, },
        { L_,   -1,          CUR,            OUT,    1 * BS,     },
        { L_,   1 * BS - 3,  CUR,            OUT,    2 * BS - 3, },
        { L_,   6,           CUR,            OUT,    2 * BS + 3, },

    };
    enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

    if (veryVerbose) {
        T_(); P_(BUFFER_SIZE); P_(HIGH_WATER_MARK); P(MAX_DATA_SIZE);
    }

    for (int i = 0; i < DATA_SIZE; ++i) {
        const int LINE     = DATA[i].d_line;
        const int OFFSET   = DATA[i].d_offset;
        const int POSITION = DATA[i].d_fixedPosition;
        const int WHICH    = DATA[i].d_which;
        const int RESULT   = DATA[i].d_result;

        int result = mX.pubseekoff(OFFSET,
                                   (bsl::ios_base::seekdir)POSITION,
                                   (bsl::ios_base::openmode)WHICH);
        LOOP2_ASSERT(i, LINE, RESULT == result);

        if (veryVerbose) {
            T_(); P_(i); P_(LINE);
            P_(OFFSET); P_(POSITION); P_(WHICH); P_(RESULT); P(result);
        }
    }
    pool->shutdown(msg.channelId(), btemt_ChannelPool::BTEMT_IMMEDIATE);
}

static
void case6SendNotification(
        const btemt_DataMsg& msg,
        int                 *buffersSent)
{
    if (veryVerbose) {
        MTCOUT << "Data sent: "
               << "length = "     << msg.data()->length()     << ", "
               << "numBuffers = " << msg.data()->numBuffers() << ", "
               << "bufferSize = " << msg.data()->bufferSize()
               << MTENDL;
    }
    *buffersSent += msg.data()->numBuffers();
}

static
void case6DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    if (veryVerbose) {
        MTCOUT << "Received message:"
               << " channelId=" << msg.channelId()
               << " length="    << msg.data()->length()
               << MTENDL;
    }

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    // Find buffer size.
    int bufferSize = 0;
    bdex_ByteStreamImpUtil::getInt32(&bufferSize, msg.data()->buffer(0));
    ASSERT(0 < bufferSize);
    *needed = 1;
    *consumed = msg.data()->length();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        MTCOUT << "\nTesting 'sync'."
               << "\n---------------" << MTENDL;
    }
    {
        enum {
            NUM_ITERATIONS  = 3,    // number of 'sync' test iterations
            HIGH_WATER_MARK = 0,
            CACHE_LIFESPAN  = 3000  // 3 seconds
        };
        const int BUFFER_SIZE    = bufferSize;
        const int LOW_WATER_MARK = BUFFER_SIZE;

        Obj mX(msg.channelId(), pool, factory,
               LOW_WATER_MARK, HIGH_WATER_MARK, CACHE_LIFESPAN, allocator);
        const Obj& X = mX;

        int           buffersSent = 0;
        Obj::Callback cb(bdef_BindUtil::bindA( allocator
                                             , &case6SendNotification
                                             , _1
                                             , &buffersSent));

        mX.setSendNotificationCallback(cb);

        bsl::ostream out(&mX);
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            for (int j = 0; j < BUFFER_SIZE - 1; ++j) {
                const int LENGTH = j + 1;
                out << '#' << flush;
                LOOP2_ASSERT(i, j, LENGTH == X.length());
            }
            out << ends;
            LOOP_ASSERT(i, BUFFER_SIZE == X.length());
            LOOP_ASSERT(i, 0 == buffersSent);
            out << flush;

            LOOP_ASSERT(i, 0 == X.length());
            LOOP_ASSERT(i, 1 == buffersSent);
            buffersSent = 0;
        }
    }

    if (veryVerbose) {
        MTCOUT << "\nTesting 'overflow'."
               << "\n-------------------" << MTENDL;
    }
    {
        enum {
            NUM_ITERATIONS  = 3,    // number of 'overload' test iterations
            LOW_WATER_MARK  = 0,
            HIGH_WATER_MARK = 3,
            CACHE_LIFESPAN  = 3000  // 3 seconds
        };
        const int BUFFER_SIZE   = bufferSize;
        const int MAX_DATA_SIZE = BUFFER_SIZE * HIGH_WATER_MARK;

        Obj mX(msg.channelId(), pool, factory,
               LOW_WATER_MARK, HIGH_WATER_MARK, CACHE_LIFESPAN, allocator);
        const Obj& X = mX;

        int           buffersSent = 0;
        Obj::Callback cb(bdef_BindUtil::bindA( allocator
                                             , &case6SendNotification
                                             , _1
                                             , &buffersSent));

        mX.setSendNotificationCallback(cb);

        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            for (int j = 0; j < HIGH_WATER_MARK; ++j) {
                for (int k = 0; k < BUFFER_SIZE; ++k) {
                    const int LENGTH = j * BUFFER_SIZE + k;
                    LOOP3_ASSERT(i, j, k, LENGTH == X.length());
                    mX.sputc('#');
                }
            }
            LOOP_ASSERT(i, MAX_DATA_SIZE == X.length());
            LOOP_ASSERT(i, 0 == buffersSent);

            bsl::ostream out(&mX);

            const bsl::string HASHMARKS(BUFFER_SIZE - 1, '#');
            out << HASHMARKS;

            LOOP_ASSERT(i, BUFFER_SIZE - 1 == X.length());
            LOOP_ASSERT(i, HIGH_WATER_MARK == buffersSent);

            out << ends << flush;
            LOOP_ASSERT(i, 0 == X.length());
            LOOP_ASSERT(i, 1 + HIGH_WATER_MARK == buffersSent);
            buffersSent = 0;
        }
    }
}

static
void case4SendNotification(
        const btemt_DataMsg& msg,
        int                 *buffersSent)
{
    if (veryVerbose) {
        MTCOUT << "length = "     << msg.data()->length()     << ", "
               << "numBuffers = " << msg.data()->numBuffers() << ", "
               << "bufferSize = " << msg.data()->bufferSize()
               << MTENDL;
    }
    *buffersSent += msg.data()->numBuffers();
}

static
void case4DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    if (veryVerbose) {
        MTCOUT << "Received message:"
               << " channelId=" << msg.channelId()
               << " length="    << msg.data()->length()
               << MTENDL;
    }

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    // Find buffer size.
    int bufferSize = 0;
    bdex_ByteStreamImpUtil::getInt32(&bufferSize, msg.data()->buffer(0));
    ASSERT(0 < bufferSize);
    *needed = 1;
    *consumed = msg.data()->length();

    enum {
        LOW_WATER_MARK  = 0,
        HIGH_WATER_MARK = 3,
        CACHE_LIFESPAN  = 3000  // 3 seconds
    };
    const int BUFFER_SIZE = bufferSize;

    Obj mX(msg.channelId(), pool, factory,
           LOW_WATER_MARK, HIGH_WATER_MARK, CACHE_LIFESPAN, allocator);
    const Obj& X = mX;

    ASSERT(CACHE_LIFESPAN  == X.cacheLifespan());
    ASSERT(HIGH_WATER_MARK == X.highWaterMark());
    ASSERT(LOW_WATER_MARK  == X.lowWaterMark());

    int           buffersSent = 0;
    Obj::Callback cb(bdef_BindUtil::bindA( allocator
                                         , &case4SendNotification
                                         , _1
                                         , &buffersSent));
    mX.setSendNotificationCallback(cb);

    for (int i = 0; i < HIGH_WATER_MARK; ++i) {
        for (int j = 0; j < BUFFER_SIZE; ++j) {
            const int LENGTH = i * BUFFER_SIZE + j;
            LOOP2_ASSERT(i, j, LENGTH == X.length());

            if (veryVerbose) {
                MTCOUT << "i = "          << i      << ", "
                       << "j = "          << j      << ", "
                       << "LENGTH = "     << LENGTH << ", "
                       << "X.length() = " << X.length()
                       << MTENDL;
            }
            mX.sputc('#');
        }
    }
    ASSERT(HIGH_WATER_MARK * BUFFER_SIZE == X.length());

    const int BUFFERS_SENT = X.length() / BUFFER_SIZE;
    ASSERT(HIGH_WATER_MARK == BUFFERS_SENT);

    mX.pubsync();

    ASSERT(BUFFERS_SENT == buffersSent);
    if (veryVerbose) {
        MTCOUT << "HWM = "          << HIGH_WATER_MARK     << ", "
               << "BUFFERS_SENT = " << BUFFERS_SENT        << ", "
               << "buffersSent = "  << buffersSent
               << MTENDL;
    }
}

static
void case2SendNotification(
        const btemt_DataMsg& msg,
        const int            CASE,
        const int            LINE,
        int                 *buffersSent)
{
    if (veryVerbose) {
        MTCOUT << "i = "          << CASE                     << ", "
               << "LINE = "       << LINE                     << ", "
               << "length = "     << msg.data()->length()     << ", "
               << "numBuffers = " << msg.data()->numBuffers() << ", "
               << "bufferSize = " << msg.data()->bufferSize()
               << MTENDL;
    }
    *buffersSent += msg.data()->numBuffers();
}

static
void case2DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    const struct {
        int         d_line;            // source line number
        int         d_lowWaterMark;    // minimum message size
        int         d_highWaterMark;   // size of streambuf cache in buffers
        const char *d_message;         // message to send
    } DATA[] = {
        //Line  Low Water Mark  High Water Mark  Message
        //----  --------------  ---------------  -------
        { L_,   1,              1,               "a",    },
        { L_,   2,              2,               "bc",   },
        { L_,   3,              3,               "def",  },
        { L_,   4,              4,               "ghij", },
    };
    enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

    // Decode query
    int request;
    bdex_ByteStreamImpUtil::getInt32(&request, msg.data()->buffer(0));
    *needed = sizeof request;
    *consumed = sizeof request;

    enum { GET_DATA_SIZE = -1 };    // query size of test set

    if (GET_DATA_SIZE == request) {
        // Hijack 'factory' to ensure the chained buffer is large enough
        // to return 'DATA_SIZE'.
        factory = new (*allocator)
                                BufferFactory(1 + sizeof DATA_SIZE, allocator);
        {
            char buffer[sizeof DATA_SIZE];

            bdex_ByteStreamImpUtil::putInt32(buffer, DATA_SIZE);
            Obj mX(msg.channelId(), pool, factory, 1, 0, 0);
            ASSERT(sizeof buffer == mX.sputn(buffer, sizeof buffer));
            ASSERT(0 == mX.pubsync());
        }
        allocator->deleteObject(factory);
        return;
    }

    // Execute test.
    int i = request;
    {
        const int         LINE = DATA[i].d_line;
        const int         LWM  = DATA[i].d_lowWaterMark;
        const int         HWM  = DATA[i].d_highWaterMark;
        const bsl::string MESSAGE(DATA[i].d_message);

        // Find buffer size for 'factory'.
        Chain *chain = factory->allocate(0);
        const int BUFFER_SIZE = chain->bufferSize();
        factory->deleteObject(chain);

        if (veryVerbose) {
            MTCOUT << "i = "       << i    << ", "
                   << "LINE = "    << LINE << ", "
                   << "LWM = "     << LWM  << ", "
                   << "HWM = "     << HWM  << ", "
                   << "MESSAGE = " << MESSAGE
                   << MTENDL;
        }

        enum { CACHE_LIFESPAN = 0 };    // always send data on 'sync'
        Obj mX(msg.channelId(), pool, factory,
               LWM, HWM, CACHE_LIFESPAN, allocator);

        int           buffersSent = 0;
        Obj::Callback cb(bdef_BindUtil::bindA( allocator
                                             , &case2SendNotification
                                             , _1
                                             , i
                                             , LINE
                                             , &buffersSent));

        mX.setSendNotificationCallback(cb);

        bsl::ostream out(&mX);
        out << MESSAGE << ends << flush;

        const int BUFFERS_SENT = 1 + MESSAGE.length() / BUFFER_SIZE;
        ASSERT(BUFFERS_SENT == buffersSent);
        if (veryVerbose) {
            MTCOUT << "i = "            << i            << ", "
                   << "LINE = "         << LINE         << ", "
                   << "BUFFER_SIZE = "  << BUFFER_SIZE  << ", "
                   << "BUFFERS_SENT = " << BUFFERS_SENT << ", "
                   << "buffersSent = "  << buffersSent
                   << MTENDL;
        }
    }
}

static
void case1DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg,
    btemt_ChannelPool *pool,
    bslma_Allocator   *allocator)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    if (veryVerbose) {
        MTCOUT << "Received message:"
               << " channelId=" << msg.channelId()
               << " length="    << msg.data()->length()
               << MTENDL;
    }

    BufferFactory *factory = static_cast<BufferFactory*>(arg);

    // Find Message Boundary
    int length;
    bdex_ByteStreamImpUtil::getInt32(&length, msg.data()->buffer(0));
    ASSERT(0 < length);
    if(length > (int)(msg.data()->length() - sizeof length)){
        *needed = length - msg.data()->length() + sizeof length;
        *consumed = 0;
        return;
    }
    *needed = sizeof length;
    *consumed = length + sizeof length;

    // Copy in message.
    bsl::string message;
    int         offset = sizeof length;
    int         bufferSize = msg.data()->bufferSize();
    int         numBuffers = msg.data()->numBuffers();
    for (int i = 0; i < numBuffers; ++i) {
        int numBytes = (i < numBuffers - 1)
                     ? bufferSize    // not the last buffer
                     : (length - bufferSize * (numBuffers - 1)) %
                                                              (1 + bufferSize);
        message.append(msg.data()->buffer(i) + offset, numBytes);
        offset = 0;
    }
    ASSERT(length == (int)message.length());

    if (veryVerbose) {
        MTCOUT << "message.length() = " << message.length() << ", "
               << "message = "          << message
               << MTENDL;
    }

    // Echo message.
    enum {
        LOW_WATER_MARK  = 32,
        HIGH_WATER_MARK = 2,
        CACHE_LIFESPAN  = 3000    // 3 seconds
    };
    Obj mX(msg.channelId(), pool, factory,
           LOW_WATER_MARK, HIGH_WATER_MARK, CACHE_LIFESPAN, allocator);

    bsl::ostream out(&mX);
    out << message << ends << flush;
}

//=============================================================================
//           CLASSES AND HELPER FUNCTIONS FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

                            // ===================
                            // class my_DataServer
                            // ===================

class my_DataServer {
    // This class implements a server which delivers random numbers,
    // generated at random intervals, to each client, according to
    // client-specified bandwidth parameters.

  private:
    // TYPES
    typedef bsl::map<int, btemt_ChannelOutStreamBuf*> MapType;
        // This type is defined for notational convenience.  The first
        // parameter specifies a channel ID.  The second parameter
        // specifies a streambuf associated with the channel.

  private:
    int    d_maxClients;        // maximum number of pending connections
    int    d_maxThreads;        // maximum number of worker threads
    int    d_isRunning;         // 1 if channel pool threads are active
    int    d_serverId;          // identifies the well-known server channel
    int    d_pushClockId;       // ID of current push timer
    double d_dataProbability;   // probability of generating data
    int    d_bufferSize;        // size of pooled buffers

    bslma_Allocator       *d_allocator_p;  // memory allocator (held)
    btemt_ChannelPool     *d_pool_p;       // channel pool (owned)
    bdem_Schema            d_schema;       // protocol schema
    MapType                d_clients;      // map channel IDs to streambufs
    bcemt_ReaderWriterLock d_lock;         // provide thread-safety
    bdef_Function<void (*)()> d_dataCb;       // data generator callback
    bdef_Function<void (*)()> d_pushCb;       // push timer callback
    bdet_TimeInterval      d_lastPush;     // last push timer expiry

    bcema_PooledBufferChainFactory
                          *d_factory_p;    // shared by client streambufs

  private:
    // not implemented
    my_DataServer(const my_DataServer&);
    my_DataServer& operator=(const my_DataServer&);

    // MANIPULATORS
    void chanCb(int channelId, int serverId, int state, void *arg);
    void dataCb(int *consumed, int *needed, btemt_DataMsg msg, void *arg);

    void generateData();
        // Generate a new data value (with some non-zero probability
        // less than 1), and stream it to each client connection.  This
        // function is called at each data clock expiry.

    void pushData();
        // Iterate over client connections, and flush the streambuf
        // for each, potentially pushing out cached data.  This
        // function is called periodically at each push timer expiry.

    void updatePushTimer(int channelId = 0);
        // Update the push timer by registering the lowest client-specified
        // "maxDelay" (from the Connect request) with a clock ID
        // corresponding to that clients' channel.  Optionally specify the
        // 'channelId' of the most recently shut-down channel.  If
        // 'channelId' is non-zero and equal to the current push timer
        // clock ID, this clock ID must be deregistered before the new
        // clock ID is registered.

  public:
    // CREATORS
    my_DataServer(int              maxClients,
                  int              maxThreads,
                  double           dataProbability,
                  bslma_Allocator *basicAllocator = 0);
        // Create a server pool object which can process process any number
        // of connections up to the specified 'maxClients', and service
        // those connections using any number of threads up to the
        // specified 'maxThreads'.  If there is at least one connection,
        // generate (and stream) data every second with probability
        // 'dataProbability'.  For example, a 'dataProbability' of 1/4
        // indicates that the server should generate data every second with
        // probability 0.25.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the default memory
        // allocator is used.

   ~my_DataServer();
        // Terminate all open connections, and destroy this server.

    // MANIPULATORS
    int start(int                      serverId,
              const bteso_IPv4Address& address,
              int                      backlog);
        // Start the server with the specified 'serverId' at the specified
        // 'address', allowing a maximum number of pending connections
        // specified by 'backlog'.  Return 0 on success, and a non-zero
        // value if an error occurred.  The behavior is undefined unless
        // 0 < 'backlog'.  In particular, the behavior is undefined if
        // called on a server which is not stopped.

    int stop();
        // Stop this server.  Immediately terminate all connections, and
        // remove all servers from listen-mode.  Return 0 if successful,
        // and a non-zero value if an error occurs.  The behavior is
        // undefined if called on a server which is stopped.
};

//-----------------------------------------------------------------------------
//                               IMPLEMENTATION
//-----------------------------------------------------------------------------
enum {
    DATA_CLOCK_ID = -1
        // ID of data generating clock.  Note that the push timer
        // clock ID is equivalent to a channel ID, which is always
        // positive.  Hence there can be no collisions.
};

void my_DataServer::chanCb(
        int   channelId,
        int   serverId,
        int   state,
        void *arg)
{
    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        d_lock.lockWrite();
        MapType::iterator it = d_clients.find(channelId);
        if (it != d_clients.end()) {
            d_allocator_p->deleteObject(it->second);
            d_clients.erase(it);
        }
        d_lock.unlock();
        updatePushTimer(channelId);
        d_pool_p->shutdown(channelId, btemt_ChannelPool::BTEMT_IMMEDIATE);
      }  break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        d_lock.lockWrite();
        d_clients.insert(bsl::make_pair(channelId,
                                          (btemt_ChannelOutStreamBuf *)0));
        d_lock.unlock();
      }  break;
    }
}

void my_DataServer::dataCb(
        int           *consumed,
        int           *needed,
        btemt_DataMsg  msg,
        void          *arg)
{
    // Validate 'msg' as a Connect request, and process it.

    bcesb_PooledBufferChainStreamBuf inbuf(msg.data());
    bdex_ByteInStreamFormatter       bis(&inbuf);
    bdem_List                        message;
    int                              version;
    bis.getVersion(version);
    bdex_InStreamFunctions::streamIn(bis, message, version);
    ASSERT(bis);

    *needed   = 1;
    *consumed = inbuf.pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::in);

    if (!bdem_SchemaAggregateUtil::isListConformant(message,
                                                    d_schema.record(0)))
    {
        d_pool_p->shutdown(msg.channelId(),
                           btemt_ChannelPool::BTEMT_IMMEDIATE);
        return;
    }

    bdem_ConstRowBinding connect(&message, &d_schema, "Connect");
    int cacheLifespan = connect.theInt("maxDelay");
    int highWaterMark = connect.theInt("cacheSize");
    int lowWaterMark  = d_bufferSize * highWaterMark;

    // The channel streambuf cannot be deleted while this thread holds a
    // read lock, and no other thread can process a data callback for this
    // channelId.  Therefore, we do not need a write lock.

    d_lock.lockRead();
    MapType::iterator it = d_clients.find(msg.channelId());
    if (it == d_clients.end()) {
        // The channel has already been closed.
        d_lock.unlock();
        return;
    }

    btemt_ChannelOutStreamBuf *outbuf = new (*d_allocator_p)
                                 btemt_ChannelOutStreamBuf(msg.channelId(),
                                                           d_pool_p,
                                                           d_factory_p,
                                                           lowWaterMark,
                                                           highWaterMark,
                                                           cacheLifespan,
                                                           d_allocator_p);
    it->second = outbuf;
    d_lock.unlock();

    updatePushTimer();
}

void my_DataServer::generateData()
{
    int generate = (int)(bsl::rand() /
                              (RAND_MAX + 1.0L) * (1 / d_dataProbability));
    if (0 == generate) {
        bdem_List datum;
        int       value = bsl::rand();
        datum.appendInt(value);
        d_lock.lockWrite();
        for (MapType::iterator it = d_clients.begin();
             it != d_clients.end();
             ++it)
        {
            bdex_ByteOutStreamFormatter bos(it->second);
            const int VERSION = bdem_List::maxSupportedBdexVersion();
            bos.putVersion(VERSION);
            bdex_OutStreamFunctions::streamOut(bos, datum, VERSION);
            bos << bdexFlush;
        }
        d_lock.unlock();
    }
}

void my_DataServer::pushData()
{
    d_lock.lockWrite();
    for (MapType::iterator it = d_clients.begin();
         it != d_clients.end();
         ++it)
    {
        it->second->pubsync();
    }
    d_lastPush = bdetu_SystemTime::now();
    d_lock.unlock();
}

void my_DataServer::updatePushTimer(int channelId)
{
    d_lock.lockWrite();
    if (0 != channelId && channelId == d_pushClockId) {
        d_pool_p->deregisterClock(d_pushClockId);
    }

    // Find channel with smallest maxDelay.
    MapType::iterator it = d_clients.begin();
    MapType::iterator kt = it;  // smallest maxDelay iterator
    while (it != d_clients.end()) {
        if (it->second->cacheLifespan() > 0
         && it->second->cacheLifespan() < kt->second->cacheLifespan()) {
            kt = it;
        }
        ++it;
    }

    if (kt != d_clients.end() && d_pushClockId != kt->first) {
        d_pool_p->deregisterClock(d_pushClockId);

        if (0 == d_pushClockId) {
            // Start data generator clock.
            d_pool_p->registerClock(d_dataCb,
                                    bdetu_SystemTime::now(),
                                    bdet_TimeInterval(1.0),
                                    DATA_CLOCK_ID);
            d_lastPush = bdetu_SystemTime::now();
        }

        if (0 < kt->second->cacheLifespan()) {
            bdet_TimeInterval period(kt->second->cacheLifespan() / 1000.0);
            d_pool_p->registerClock(d_pushCb,
                                    d_lastPush + period,
                                    period,
                                    kt->first);
            d_pushClockId = kt->first;
        }
    }

    if (d_clients.empty()) {
        d_pushClockId = 0;
        d_pool_p->deregisterClock(DATA_CLOCK_ID);
    }
    d_lock.unlock();
}

// CREATORS
my_DataServer::my_DataServer(
        int              maxClients,
        int              maxThreads,
        double           dataProbability,
        bslma_Allocator *basicAllocator)
: d_maxClients(maxClients)
, d_maxThreads(maxThreads)
, d_isRunning(0)
, d_pushClockId(0)
, d_dataProbability(dataProbability)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    ASSERT(0 < maxClients);
    ASSERT(0 < maxThreads);
    ASSERT(0 < dataProbability);

    btemt_ChannelPool::ChannelStateChangeCallback ccb(
            bdef_MemFnUtil::memFn(&my_DataServer::chanCb, this)
          , d_allocator_p);

    btemt_ChannelPool::DataReadCallback dcb(
            bdef_MemFnUtil::memFn(&my_DataServer::dataCb, this)
          , d_allocator_p);

    btemt_ChannelPool::PoolStateChangeCallback pcb;

    d_dataCb = bdef_Function<void (*)()>(
            bdef_MemFnUtil::memFn(&my_DataServer::generateData, this)
          , d_allocator_p);

    d_pushCb = bdef_Function<void (*)()>(
            bdef_MemFnUtil::memFn(&my_DataServer::pushData, this)
          , d_allocator_p);

    bdem_RecordDef *record = d_schema.createRecord("Connect");
    ASSERT(record->appendField(bdem_ElemType::BDEM_INT, "cacheSize"));
    ASSERT(record->appendField(bdem_ElemType::BDEM_INT, "maxDelay"));

    record = d_schema.createRecord("Data");
    ASSERT(record->appendField(bdem_ElemType::BDEM_INT, "value"));

    // Determine pooled buffer size using a mock connect request.
    bdem_List connect;
    connect.appendInt(0);
    connect.appendInt(0);

    bdex_ByteOutStream bos(d_allocator_p);
    const int VERSION = bdem_List::maxSupportedBdexVersion();
    bos.putVersion(VERSION);
    bdex_OutStreamFunctions::streamOut(bos, connect, VERSION);
    d_bufferSize = bos.length();

    d_factory_p = new (*d_allocator_p)
               bcema_PooledBufferChainFactory(d_bufferSize, d_allocator_p);

    bslma_RawDeleterProctor<bcema_PooledBufferChainFactory, bslma_Allocator>
                                          deleter(d_factory_p, d_allocator_p);

    btemt_ChannelPoolConfiguration cpc;
    cpc.setMaxConnections(d_maxClients);
    cpc.setMaxThreads(d_maxThreads);
    cpc.setMetricsInterval(10.0);
    cpc.setIncomingMessageSizes(d_bufferSize, d_bufferSize, 1024);

    d_pool_p = new (*d_allocator_p)
                      btemt_ChannelPool(ccb, dcb, pcb, cpc, d_allocator_p);

    deleter.release();
}

my_DataServer::~my_DataServer()
{
    if (0 < d_clients.size()) {
        d_pool_p->deregisterClock(d_pushClockId);
    }
    d_pool_p->stop();

    // Deallocate streambufs.
    for (MapType::iterator it = d_clients.begin();
         it != d_clients.end();
         ++it)
    {
        d_allocator_p->deleteObject(it->second);
    }

    d_allocator_p->deleteObject(d_factory_p);
    d_allocator_p->deleteObject(d_pool_p);
}

// MANIPULATORS
int my_DataServer::start(
        int                      serverId,
        const bteso_IPv4Address& endpoint,
        int                      backlog)
{
    ASSERT(0 < backlog);

    int src = (d_isRunning) ? 0 : d_pool_p->start();
    int lrc = d_pool_p->listen(endpoint, backlog, serverId);

    if(0 == (src || lrc)){
        d_serverId = serverId;
        d_isRunning = 1;
    }

    return src || lrc;
}

int my_DataServer::stop()
{
    int rc = d_pool_p->stop();

    if(rc == 0){
        d_isRunning = 0;
    }
    return rc;
}

//-----------------------------------------------------------------------------

                              // ================
                              // class my_DataSet
                              // ================

class my_DataSet {
    // This class is a container for a set of integer values, and its
    // minimum and maximum.

    bslma_Allocator *d_allocator_p;    // held
    bsl::vector<int> d_values;         // list of values received
    int              d_min;            // minimum value
    int              d_max;            // maximum value

  private:
    // not implemented
    my_DataSet(const my_DataSet&);
    my_DataSet& operator=(const my_DataSet&);

  public:
    // CREATORS
    my_DataSet(bslma_Allocator *basicAllocator = 0);
        // Create a 'my_DataSet' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator'
        // is 0, the default memory allocator is used.

    ~my_DataSet();
        // Destroy this object.

    // MANIPULATORS
    void append(int value);
        // Append 'value' to the current list of values, and update
        // the minimum and maximum if necessary.

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&     stream,
                                    const my_DataSet& dataSet);
        // Stream 'dataSet' to the specified 'stream' as a square-bracket
        // enclosed list of values, presented in the order they were
        // 'append'ed, followed by the minimum and maximum values; and
        // return 'stream'
};

// CREATORS
my_DataSet::my_DataSet(bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_values(d_allocator_p)
, d_min(INT_MAX)
, d_max(INT_MIN)
{
}

my_DataSet::~my_DataSet()
{
}

// MANIPULATORS
void my_DataSet::append(int value)
{
    d_values.push_back(value);
    d_min = bsl::min(d_min, value);
    d_max = bsl::max(d_max, value);
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const my_DataSet& dataSet)
{
    stream << "[ ";
    bsl::copy(dataSet.d_values.begin(),
              dataSet.d_values.end(),
              bsl::ostream_iterator<int>(stream, " "));
    stream << "] " << dataSet.d_min << ' ' << dataSet.d_max;
    return stream;
}

void ue2ChannelStateCallback(
        int                 channelId,
        int                 serverId,
        int                 state,
        void               *arg,
        btemt_ChannelPool **poolAddr,
        bsl::vector<int>   *channels,
        bcemt_Barrier      *barrier)
{
    ASSERT(poolAddr && *poolAddr);
    ASSERT(channels);
    ASSERT(barrier);

    btemt_ChannelPool *pool = *static_cast<btemt_ChannelPool**>(poolAddr);

    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVeryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }

        // The channel pool has already invalidated 'channelId', so we
        // must retrieve 'dataSet' from 'arg', bound in the 'CHANNEL_UP'
        // branch, rather than call 'channelContext'.

        bslma_Allocator *deleter = bslma_Default::allocator();
        my_DataSet      *dataSet = static_cast<my_DataSet*>(arg);
        ASSERT(dataSet);
        deleter->deleteObject(dataSet);
      }  break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        if (veryVeryVerbose) {
            bteso_IPv4Address channel;
            char              address[16+1];

            pool->getPeerAddress(&channel, channelId);
            channel.loadIpAddress(address);
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << " address="   << address << ":" << channel.portNumber()
                   << MTENDL;
        }

        bslma_Allocator *allocator = bslma_Default::allocator();
        my_DataSet      *dataSet = new (*allocator) my_DataSet(allocator);
        pool->setChannelContext(channelId, dataSet);
        channels->push_back(channelId);
        barrier->wait();
      }  break;
    }
}

void ue2DataCallback(
    int               *consumed,
    int               *needed,
    btemt_DataMsg      msg,
    void              *arg)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(arg);

    my_DataSet *dataSet = static_cast<my_DataSet*>(arg);

    // Find message.
    bcesb_PooledBufferChainStreamBuf inbuf(msg.data());
    bdex_ByteInStreamFormatter       bis(&inbuf);
    bdem_List                        datum;
    int                              bytesUsed = 0;

    *needed   = 1;
    *consumed = bytesUsed;

    while (true) {
        int version;
        bis.getVersion(version);
        bdex_InStreamFunctions::streamIn(bis, datum, version);

        if (!bis) {
            *needed = msg.data()->bufferSize();
            break;
        }
        bytesUsed = inbuf.pubseekoff(0, bsl::ios_base::cur, bsl::ios_base::in);
        int value = datum.theInt(0);
        dataSet->append(value);
    }
    *consumed = bytesUsed;

    if (0 < *consumed) {
        MTCOUT << "channelId=" << msg.channelId() << ' '
               << *dataSet
               << MTENDL;
    }
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = (argc > 1) ? bsl::atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 14: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.  Make use of existing test apparatus by instantiating
        //   objects with a 'bslma_TestAllocator' object where applicable.
        //   Additionally, replace all calls to 'assert' in the usage example
        //   with calls to 'ASSERT'.  This now becomes the source, which is
        //   then "copied" back to the header file by reversing the above
        //   process.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Usage Example 1" << endl
                 << "=======================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 2,
                MAX_THREADS  = 2,

                SERVER_ID    = 0xc01d50da,
                BUFFER_SIZE  = 4,
                BACKLOG      = 1
            };
            my_ServerPool               sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case14DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));

            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == sp.start(SERVER_ID, BUFFER_SIZE, ADDRESS, BACKLOG));
            const int PORT = sp.pool()->serverAddress(SERVER_ID)->portNumber();
            const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            AddressFactory     factory;
            Socket            *socket = factory.allocate();
            btesos_TcpChannel  channel(socket);

            ASSERT(0 == socket->connect(SERVER_ADDRESS));
            ASSERT(0 == channel.isInvalid());
            {
                const bsl::string MESSAGE = "0123456789abcdef";

                // Write message.
                char length[sizeof(int)];
                bdex_ByteStreamImpUtil::putInt32(length, MESSAGE.length());
                channel.write(length, sizeof length);
                channel.write(MESSAGE.c_str(), MESSAGE.length());

                // read and verify message.
                char buf[16];
                channel.read(buf, sizeof buf);

                bsl::string message(buf, sizeof buf);
                ASSERT(MESSAGE == message);

                if (veryVerbose) {
                    T_(); P_(MESSAGE); P(message);
                }
            }
            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case -13: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: FAILED WRITE IS HANDLED
        //
        // Concerns:
        //   That the 'btemt_ChannelOutStreamBuf' is able to handle the case
        //   when the output stream becomes invalid due to a write error.
        //
        // Plan:
        //   Create a server which allocates a modifiable
        //   'btemt_ChannelOutStreamBuf' object,  'mX' with a fixed high water
        //   mark, low water mark, and cache lifespan.  Set a send
        //   notification callback function for 'mX' that is bound to a
        //   counter, and increments the counter on each buffer transmission.
        //   Whenever a client connects to the server, the server steams data
        //   until the stream becomes invalid, and then closes the connection.
        //
        // Testing:
        //   Concern: Failed write is handled
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Concern: Failed Write Is Handled" << endl
                 << "========================================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 2,
                MAX_THREADS  = 2,

                SERVER_ID    = 0xc01d50da,
                BUFFER_SIZE  = 32,
                BACKLOG      = 1
            };

            bcemt_Barrier      barrier(2);
            bsl::vector<int>   channels(&ta);
            btemt_ChannelPool *poolAddr = 0;

            Pool::ChannelStateChangeCallback ccb(
                    bdef_BindUtil::bindA( &ta
                                        , &case13ChannelStateCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , &poolAddr
                                        , &barrier));

            Pool::DataReadCallback        dcb;
            Pool::PoolStateChangeCallback pcb;

            Config cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setIncomingMessageSizes(6, 216, 1024);
            cpc.setWriteCacheWatermarks(0, 256);

            Pool pool(ccb, dcb, pcb, cpc, &ta);
            poolAddr = &pool;
            ASSERT(0 == pool.start());

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == pool.listen(ADDRESS, BACKLOG, SERVER_ID));
            const int PORT = pool.serverAddress(SERVER_ID)->portNumber();
            const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            AddressFactory     factory;
            Socket            *socket = factory.allocate();
            btesos_TcpChannel  channel(socket);

            ASSERT(0 == socket->connect(SERVER_ADDRESS));
            ASSERT(0 == channel.isInvalid());

            barrier.wait();

            channel.invalidate();
            factory.deallocate(socket);

            ASSERT(0 == pool.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'forceSync' FUNCTION
        //
        // Concerns:
        //   * That 'forceSync' sends cached data, regardless of the cache
        //     configuration.
        //
        // Plan:
        //   Create a server which allocates a modifiable
        //   'btemt_ChannelOutStreamBuf' object,  'mX' with a fixed high water
        //   mark, low water mark, and cache lifespan.  Set a send
        //   notification callback function for 'mX' that is bound to a
        //   counter, and increments the counter on each buffer transmission.
        //   Fill 'mX' to just below the low water mark, and verify that
        //   calling 'pubsync' does not cause a data transmission.  Call
        //   'forceSync', and verify that data was transmitted by confirming
        //   the length and the number of buffers sent.
        //
        // Testing:
        //   int forceSync();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing 'forceSync' Function" << endl
                 << "============================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            const struct {
                int d_line;        // source line number
                int d_serverId;
                int d_bufferSize;  // size of buffer chain buffers
                int d_backlog;     // listen port backlog
            } DATA[] = {
                //Line  Server Id   Buffer Size  Backlog
                //----  ---------   -----------  -------
                { L_,   0xc001d00d, 3,           2,     },
                { L_,   0xc01d50da, 6,           2,     },
                { L_,   0xbabef00d, 9,           2,     },
                { L_,   0xd099f00d, 12,          2,     },
            };
            enum { NUM_SERVERS = sizeof DATA / sizeof *DATA };
            int portNumbers[NUM_SERVERS];

            enum {
                MAX_CLIENTS  = NUM_SERVERS,
                MAX_THREADS  = NUM_SERVERS
            };
            my_ServerPool sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case12DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));

            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            if (veryVerbose) {
                cout << "\n\tStart servers."
                     << "\n\t--------------" << endl;
            }
            for (int i = 0 ; i < NUM_SERVERS; ++i) {
                const int LINE        = DATA[i].d_line;
                const int SERVER_ID   = DATA[i].d_serverId;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;
                const int BACKLOG     = DATA[i].d_backlog;

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "i = "         << i    << ", "
                           << "LINE = "      << LINE << ", "
                           << "SERVER_ID = "
                           << bsl::hex << SERVER_ID << bsl::dec << ", "
                           << "BUFFER_SIZE = " << BUFFER_SIZE
                           << MTENDL;
                }

                const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
                LOOP2_ASSERT(i, LINE, 0 == sp.start(SERVER_ID, BUFFER_SIZE,
                                                    ADDRESS, BACKLOG));
                const int PORT = sp.pool()->serverAddress(SERVER_ID)
                                          ->portNumber();
                const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

                portNumbers[i] = PORT;
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            if (veryVerbose) {
                cout << "\n\tExecute tests."
                     << "\n\t--------------" << endl;
            }

            AddressFactory factory;

            for (int i = 0; i < NUM_SERVERS; ++i) {
                const int SERVER_ID   = DATA[i].d_serverId;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;
                const int PORT        = portNumbers[i];

                const bteso_IPv4Address ADDRESS("127.0.0.1", PORT);
                if (veryVerbose) {
                    MTCOUT << "\n\t"
                           << "Connecting to " << ADDRESS << ", "
                           << "SERVER_ID = "
                           << bsl::hex << SERVER_ID << bsl::dec
                           << MTENDL;
                }
                Socket            *socket = factory.allocate();
                btesos_TcpChannel  channel(socket);
                ASSERT(0 == socket->connect(ADDRESS));
                ASSERT(0 == channel.isInvalid());

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "SERVER_ID = "
                           << bsl::hex << SERVER_ID << bsl::dec << ", "
                           << "BUFFER_SIZE = " << BUFFER_SIZE   << ", "
                           << MTENDL;
                }

                // Initiate test.
                char length[sizeof(int)];
                bdex_ByteStreamImpUtil::putInt32(length, BUFFER_SIZE);
                channel.write(length, sizeof length);

                enum { NUM_ITERATIONS = 6 };  // see 'case12DataCallback'

                char buf[1];
                for (int j = 0; j < NUM_ITERATIONS; ++j) {
                    // Read null-terminated response
                    do {
                        int n = channel.readRaw(buf, sizeof buf);
                        ASSERT(0 < n);
                    } while (0 != buf[0]);
                }
                channel.invalidate();
                factory.deallocate(socket);
            }
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'xsputn' FUNCTION
        //
        // Concerns:
        //   * That 'xsputn' allocates the necessary number of chained buffers,
        //     and obeys the caching configuration.
        //
        // Plan:
        //   Create a server which allocates a modifiable
        //   'btemt_ChannelOutStreamBuf' object,  'mX' with a fixed high water
        //   mark, low water mark, and cache lifespan.  Set a send
        //   notification callback function for 'mX' that is bound to a
        //   counter, and increments the counter on each buffer transmission.
        //   Fill 'mX' to the high water mark using 'sputn'.  Verify that
        //   'overflow' is called appropriately by monitoring the length of
        //   'mX', and the number of buffers sent.  Flush 'mX', and verify the
        //   length and the number of buffers sent.
        //
        // Testing:
        //   bsl::streamsize xsputn(const char_type *source,
        //                          bsl::streamsize  numChars);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing 'xsputn' Function" << endl
                 << "=========================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 2,
                MAX_THREADS  = 2,

                SERVER_ID    = 0xc01d50da,
                BUFFER_SIZE  = 32,
                BACKLOG      = 1
            };
            my_ServerPool               sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case11DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));

            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == sp.start(SERVER_ID, BUFFER_SIZE, ADDRESS, BACKLOG));
            const int PORT = sp.pool()->serverAddress(SERVER_ID)->portNumber();
            const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            AddressFactory     factory;
            Socket            *socket = factory.allocate();
            btesos_TcpChannel  channel(socket);

            ASSERT(0 == socket->connect(SERVER_ADDRESS));
            ASSERT(0 == channel.isInvalid());
            {
                // Initiate test.
                char length[sizeof(int)];
                bdex_ByteStreamImpUtil::putInt32(length, BUFFER_SIZE);
                channel.write(length, sizeof length);

                // Block, while test executes.
                char buf[1];
                channel.read(buf, sizeof buf);
            }

            // TBD -- if this delay is NOT introduced
            // the process may get SIGPIPE (the error is intermittent).
            // This should be considered as a bug in the test driver.
            // The temporary fix -- put a delay so that all writes
            // are done.
            bcemt_ThreadUtil::microSleep(10000, 0);
            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: CACHE LIFESPAN
        //
        // Concerns:
        //   * That 'sync'd data is sent when the cache lifespan expires, even
        //     though the data length is below the low water mark.
        //
        // Plan:
        //  Create a number of servers with a fixed buffer size and varying
        //  cache lifespans.  Within each server, allocate a modifiable
        //  'btemt_ChannelOutStreamBuf' object, 'mX', with fixed high and low
        //  water marks.  Set a send notification callback function for 'mX'
        //  that is bound to a counter, and increments the counter on each
        //  buffer transmission.  Fill 'mX' to its high water mark.  Then,
        //  flush  'mX', sleeping intermittently, until the cache lifespan is
        //  exceeded.  Verify the number of buffers which are sent.  On the
        //  client side, verify that the total delay for each server is within
        //  some epsilon of the configured cache lifespan.
        //
        //  Note that several instances of this test driver will compete for
        //  the port number on the localhost, and so we add a random number to
        //  diminish the probability that they conflict.  With up to 10 test
        //  drivers running concurrently on, e.g., SunOS-sparc-5.9, one needs a
        //  range of ports in the thousand to diminish the probability that
        //  two test drivers will conflict to less than 5%.  (See the Birthday
        //  Paradox on e.g., Wikipedia, to get probability estimates.)  Note
        //  that all instances will have a different process name (argv[0]) so
        //  we can use a hash of this instead of a random number.
        //
        // Testing:
        //   Concern: Cache lifespan.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Concern: Cache Lifespan" << endl
                 << "===============================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 2,
                MAX_THREADS  = 2,

                SERVER_ID    = 0xc01d50da,
                BUFFER_SIZE  = 53,
                BACKLOG      = 1
            };
            my_ServerPool               sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case10DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));

            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == sp.start(SERVER_ID, BUFFER_SIZE, ADDRESS, BACKLOG));
            const int PORT = sp.pool()->serverAddress(SERVER_ID)->portNumber();
            const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            const struct {
                int d_line;             // source line number
                int d_cacheLifespan;    // maximum lifespan of data cached in
                                        // a streambuf before it is sent on
                                        // 'sync' in milliseconds
            } DATA[] = {
                 //Line  Cache Lifespan
                 //----  --------------
                 { L_,   1000,         },
                 { L_,   2000,         },
                 { L_,   2250,         },
                 { L_,   2500,         },
                 { L_,   3000,         },
                 { L_,   4000,         },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            enum {
                NANOSECS_PER_SEC = 1000000000,
                RESOLUTION       = NANOSECS_PER_SEC / 1000
            };

            AddressFactory     factory;
            Socket            *socket = factory.allocate();
            btesos_TcpChannel  channel(socket);

            ASSERT(0 == socket->connect(SERVER_ADDRESS));
            ASSERT(0 == channel.isInvalid());

            const int MAX_RETRY = 5;
            int retries = 0;
            for (int i = 0; i < DATA_SIZE; (0 < retries) || ++i) {
                const int LINE           = DATA[i].d_line;
                const int CACHE_LIFESPAN = DATA[i].d_cacheLifespan;

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "i = "              << i           << ", "
                           << "LINE = "           << LINE        << ", "
                           << "BUFFER_SIZE = "    << BUFFER_SIZE << ", "
                           << "CACHE_LIFESPAN = " << CACHE_LIFESPAN
                           << MTENDL;
                }

                const bsls_PlatformUtil::Int64 START =
                                    bsls_TimeUtil::getTimer() / RESOLUTION;

                // Initiate test.
                char buffer[sizeof BUFFER_SIZE + sizeof CACHE_LIFESPAN];
                bdex_ByteStreamImpUtil::putInt32(buffer, BUFFER_SIZE);
                bdex_ByteStreamImpUtil::putInt32(buffer + sizeof(int),
                                                 CACHE_LIFESPAN);
                channel.write(buffer, sizeof buffer);

                bsl::string message;
                do {
                    char buf[128];
                    int  n = channel.readRaw(buf, sizeof buf);
                    LOOP_ASSERT(n, 0 < n);
                    message.append(buf, n);
                } while (0 != *(message.end() - 1));
                message.erase(message.end() - 1);

                const bsls_PlatformUtil::Int64 END =
                                    bsls_TimeUtil::getTimer() / RESOLUTION;
                const bsls_PlatformUtil::Int64 EPSILON = 300;  // 300 ms
                const int DELTA =  END - START - CACHE_LIFESPAN;
                if (DELTA < EPSILON) {
                    retries = 0; // Reset for next iteration (and trigger ++i).
                } else {
                    if (++retries < MAX_RETRY) {
                        if (veryVerbose) {
                            MTCOUT << "\tWARNING: timing delays prevented"
                                   << " case 10 to succeed.  Retrying."
                                   << MTENDL;
                        }
                    } else {
                        if (veryVerbose) {
                            MTCOUT << "\tERROR: too many timing errors from"
                                   << " case 10.  Failing." << MTENDL;
                        }
                        ASSERT(DELTA < EPSILON); // Always fails (else clause).
                        retries = 0; // Reset and move on to next ++i.
                    }
                }

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "message.length() = " << message.length() << ", "
                           << "RTT = "              << END - START      << ", "
                           << "DELTA = "            << DELTA
                           << MTENDL;
                }
            }
            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      }  break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: Data is sent when streambuf is destroyed
        //
        // Concerns:
        //   * That buffered data is sent when the streambuf object is
        //     destroyed.
        //
        // Plan:
        //   Create a server with a fixed buffer size that allocates a
        //   modifiable 'btemt_ChannelOutStreamBuf' object,  'mX', with fixed
        //   high and low water marks and cache lifespan.  Fill 'mX' to its
        //   high water mark, and then allow 'mX' to go out of scope.  Set a
        //   send notification callback function for 'mX' that is bound to a
        //   counter, and increments the counter on each buffer transmission.
        //   Verify the number of buffers which are sent.
        //
        // Testing:
        //   Concern: Data is sent when streambuf is destroyed.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Concern: Data is sent when streambuf is destroyed"
                 << endl
                 << "========================================================="
                 << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 2,
                MAX_THREADS  = 2,

                SERVER_ID    = 0xc01d50da,
                BUFFER_SIZE  = 29,
                BACKLOG      = 1
            };
            my_ServerPool               sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case9DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));

            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == sp.start(SERVER_ID, BUFFER_SIZE, ADDRESS, BACKLOG));
            const int PORT = sp.pool()->serverAddress(SERVER_ID)->portNumber();
            const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            AddressFactory     factory;
            Socket            *socket = factory.allocate();
            btesos_TcpChannel  channel(socket);

            ASSERT(0 == socket->connect(SERVER_ADDRESS));
            ASSERT(0 == channel.isInvalid());
            {
                const bsl::string MESSAGE = "0123456789abcdefgh"
                                            "ijklmnopqrstuvwxyz";

                // Write message.
                char length[sizeof(int)];
                bdex_ByteStreamImpUtil::putInt32(length, MESSAGE.length());
                channel.write(length, sizeof length);
                channel.write(MESSAGE.c_str(), MESSAGE.length());

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "MESSAGE.length() = " << MESSAGE.length() << ", "
                           << "MESSAGE = "          << MESSAGE
                           << MTENDL;
                }

                // Read and verify response.
                bsl::string message;

                do {
                    char buf[128];
                    int  n = channel.readRaw(buf, sizeof buf);
                    ASSERT(0 < n);
                    message.append(buf, n);
                } while (0 != *(message.end() - 1));
                message.erase(message.end() - 1);

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "message.length() = " << message.length() << ", "
                           << "message = "          << message
                           << MTENDL;
                }
                ASSERT(MESSAGE == message);
            }
            // TBD -- if this delay is NOT introduced
            // the process may get SIGPIPE (the error is intermittent).
            // This should be considered as a bug in the test driver.
            // The temporary fix -- put a delay so that all writes
            // are done.
            bcemt_ThreadUtil::microSleep(10000, 0);
            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'seekpos' FUNCTION
        //
        // Concerns:
        //   * That 'seekpos' fails when 'which' includes 'bsl::ios_base::in'.
        //
        //   * That 'seekpos' fails when the calculated position exceeds the
        //     buffer size.  This should be the case when the calculated
        //     position is less than zero or greater than the pooled buffer
        //     chain length.
        //
        //   * That 'seekpos' succeeds when the calculated offset is within
        //     the buffer size.
        //
        // Plan:
        //   Perform a variety of seeks, each testing different settings so
        //   as to address various combinations of the above concerns.
        //
        // Testing:
        //   pos_type seekpos(
        //                 off_type                position,
        //                 bsl::ios_base::openmode which = bsl::ios_base::in
        //                                               | bsl::ios_base::out);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing 'seekpos' Function" << endl
                 << "===========================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 2,
                MAX_THREADS  = 2,

                SERVER_ID    = 0xc01d50da,
                BUFFER_SIZE  = 37,
                BACKLOG      = 1
            };
            my_ServerPool               sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case8DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));

            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == sp.start(SERVER_ID, BUFFER_SIZE, ADDRESS, BACKLOG));
            const int PORT = sp.pool()->serverAddress(SERVER_ID)->portNumber();
            const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            AddressFactory     factory;
            Socket            *socket = factory.allocate();
            btesos_TcpChannel  channel(socket);

            ASSERT(0 == socket->connect(SERVER_ADDRESS));
            ASSERT(0 == channel.isInvalid());
            {
                // Initiate test.
                char length[sizeof(int)];
                bdex_ByteStreamImpUtil::putInt32(length, BUFFER_SIZE);
                channel.write(length, sizeof length);

                // Block, while test executes.
                char buf[1];
                channel.read(buf, sizeof buf);
            }

            // TBD -- if this delay is NOT introduced
            // the process may get SIGPIPE (the error is intermittent).
            // This should be considered as a bug in the test driver.
            // The temporary fix -- put a delay so that all writes
            // are done.
            bcemt_ThreadUtil::microSleep(10000, 0);
            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'seekoff' FUNCTION
        //
        // Concerns:
        //   * That 'seekoff' fails when 'which' includes 'bsl::ios_base::in'.
        //
        //   * That 'seekoff' fails when the calculated position exceeds the
        //     buffer size.  This should be the case when the calculated
        //     position is less than zero or greater than the pooled buffer
        //     chain length.
        //
        //   * That 'seekoff' succeeds when the calculated offset is within
        //     the buffer size.
        //
        // Plan:
        //   Perform a variety of seeks, using representative test vectors from
        //   the cross-product of offset categories beginning-pointer, current-
        //   pointer and end-pointer, with direction categories negative-
        //   forcing-past-beginning, negative-falling-within-bounds, 0,
        //   positive-falling-within bounds, and positive-forcing-past-end.
        //
        // Testing:
        //   pos_type seekoff(
        //                 off_type                offset,
        //                 bsl::ios_base::seekdir  fixedPosition,
        //                 bsl::ios_base::openmode which = bsl::ios_base::in
        //                                               | bsl::ios_base::out);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing 'seekoff' Function" << endl
                 << "===========================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 2,
                MAX_THREADS  = 2,

                SERVER_ID    = 0xc01d50da,
                BUFFER_SIZE  = 32,
                BACKLOG      = 1
            };
            my_ServerPool               sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case7DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));

            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == sp.start(SERVER_ID, BUFFER_SIZE, ADDRESS, BACKLOG));
            const int PORT = sp.pool()->serverAddress(SERVER_ID)->portNumber();
            const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            AddressFactory     factory;
            Socket            *socket = factory.allocate();
            btesos_TcpChannel  channel(socket);

            ASSERT(0 == socket->connect(SERVER_ADDRESS));
            ASSERT(0 == channel.isInvalid());
            {
                // Initiate test.
                char length[sizeof(int)];
                bdex_ByteStreamImpUtil::putInt32(length, BUFFER_SIZE);
                channel.write(length, sizeof length);

                // Block, while test executes.
                char buf[1];
                channel.read(buf, sizeof buf);
            }

            // TBD -- if this delay is NOT introduced
            // the process may get SIGPIPE (the error is intermittent).
            // This should be considered as a bug in the test driver.
            // The temporary fix -- put a delay so that all writes
            // are done.
            bcemt_ThreadUtil::microSleep(10000, 0);
            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'sync' AND 'overflow' FUNCTIONS
        //
        // Concerns:
        //   * That 'sync' buffers data, but does not send data, if the
        //     low-water-mark is not exceeded.
        //
        //   * That 'sync' sends data and resets the put area if the
        //     high-water-mark is exceeded.
        //
        //   * That 'overflow' buffers data, but does not send data, when the
        //     current chained buffer is overflowed, but the high-water-mark
        //     is not exceeded.
        //
        //   * That 'overflow' sends data and resets the put area when the
        //     current chained buffer is overflowed, and the high-water-mark
        //     is exceeded.
        //
        // Plan:
        //   Create a number of servers with varying buffer sizes.  Within each
        //   server, allocate a modifiable 'btemt_ChannelOutStreamBuf' object,
        //   'mX'.  Set a send notification callback function for 'mX' that
        //   is bound to a counter, and increments the counter on each buffer
        //   transmission.  Incrementally fill 'mX' to its high water mark,
        //   and verify that 'overflow' is called at the expected time by
        //   monitoring the length of 'mX', and the number of buffers sent.
        //   Flush 'mX', and verify the length and the number of buffers sent.
        //
        // Testing:
        //   int_type overflow(int_type insertionChar = traits_type::eof());
        //   int sync();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing 'sync' and 'overflow' Functions" << endl
                 << "=======================================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            const struct {
                int d_line;        // source line number
                int d_serverId;
                int d_bufferSize;  // size of buffer chain buffers
                int d_backlog;     // listen port backlog
            } DATA[] = {
                //Line  Server Id   Buffer Size  Backlog
                //----  ---------   -----------  -------
                { L_,   0xc001d00d, 3,           2,     },
                { L_,   0xc01d50da, 6,           2,     },
                { L_,   0xbabef00d, 9,           2,     },
                { L_,   0xd099f00d, 12,          2,     },
            };
            enum { NUM_SERVERS = sizeof DATA / sizeof *DATA };
            int portNumbers[NUM_SERVERS];

            enum {
                MAX_CLIENTS  = NUM_SERVERS,
                MAX_THREADS  = NUM_SERVERS
            };
            my_ServerPool               sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case6DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));
            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            if (veryVerbose) {
                cout << "\n\tStart servers."
                     << "\n\t--------------" << endl;
            }
            for (int i = 0 ; i < NUM_SERVERS; ++i) {
                const int LINE        = DATA[i].d_line;
                const int SERVER_ID   = DATA[i].d_serverId;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;
                const int BACKLOG     = DATA[i].d_backlog;

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "i = "         << i    << ", "
                           << "LINE = "      << LINE << ", "
                           << "SERVER_ID = "
                           << bsl::hex << SERVER_ID << bsl::dec << ", "
                           << "BUFFER_SIZE = " << BUFFER_SIZE
                           << MTENDL;
                }

                const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
                LOOP2_ASSERT(i, LINE, 0 == sp.start(SERVER_ID, BUFFER_SIZE,
                                                    ADDRESS, BACKLOG));
                const int PORT = sp.pool()->serverAddress(SERVER_ID)
                                          ->portNumber();
                const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

                portNumbers[i] = PORT;
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            if (veryVerbose) {
                cout << "\n\tExecute tests."
                     << "\n\t--------------" << endl;
            }

            AddressFactory factory;

            for (int i = 0; i < NUM_SERVERS; ++i) {
                const int SERVER_ID   = DATA[i].d_serverId;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;
                const int PORT        = portNumbers[i];

                const bteso_IPv4Address ADDRESS("127.0.0.1", PORT);
                if (veryVerbose) {
                    MTCOUT << "\n\t"
                           << "Connecting to " << ADDRESS << ", "
                           << "SERVER_ID = "
                           << bsl::hex << SERVER_ID << bsl::dec
                           << MTENDL;
                }
                Socket            *socket = factory.allocate();
                btesos_TcpChannel  channel(socket);
                ASSERT(0 == socket->connect(ADDRESS));
                ASSERT(0 == channel.isInvalid());

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "SERVER_ID = "
                           << bsl::hex << SERVER_ID << bsl::dec << ", "
                           << "BUFFER_SIZE = " << BUFFER_SIZE   << ", "
                           << MTENDL;
                }

                // Initiate test.
                char length[sizeof(int)];
                bdex_ByteStreamImpUtil::putInt32(length, BUFFER_SIZE);
                channel.write(length, sizeof length);

                enum { NUM_ITERATIONS = 6 };  // see 'case6DataCallback'

                char *buf = (char *)ta.allocate(BUFFER_SIZE);
                for (int j = 0; j < NUM_ITERATIONS; ++j) {
                    // Read null-terminated response
                    bsl::string message;
                    do {
                        int n = channel.readRaw(buf, BUFFER_SIZE);
                        ASSERT(0 < n);
                        message.append(buf, n);
                    } while (0 != *(message.end() - 1));
                }
                ta.deallocate(buf);
                channel.invalidate();
                factory.deallocate(socket);
            }
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //   Verify the correct behavior of the output operator.
        //
        //   Currently, there is no 'operator<<' defined for
        //   'btemt_ChannelOutStreamBuf', so this test case remains empty.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Output (<<) Operator" << endl
                 << "============================" << endl;
        }
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //   * That 'length' reports the length of the buffer chain, event
        //     when the chain consists of multiple buffers.
        //
        // Plan:
        //   Create a server which allocates a modifiable
        //   'btemt_ChannelOutStreamBuf' object,  'mX' with a fixed high water
        //   mark, low water mark, and cache lifespan.  Create a non-modifiable
        //   reference to 'mX' named 'X'.  Using 'X', verify the high and low
        //   water marks, and the cache lifespan.  Fill 'mX' to the high water
        //   mark.  Verify the length using 'X'.  Set a send notification
        //   callback function for 'mX' that is bound to a counter, and
        //   increments the counter on each buffer transmission.  Flush 'mX',
        //   and verify the number of buffers sent.
        //
        // Testing:
        //   int cacheLifespan() const;
        //   int highWaterMark() const;
        //   bsl::streamsize length() const;
        //   int lowWaterMark() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Basic Accessors" << endl
                 << "=======================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 2,
                MAX_THREADS  = 2,

                SERVER_ID    = 0xc01d50da,
                BUFFER_SIZE  = 16,
                BACKLOG      = 1
            };
            my_ServerPool               sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case4DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));

            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == sp.start(SERVER_ID, BUFFER_SIZE, ADDRESS, BACKLOG));
            const int PORT = sp.pool()->serverAddress(SERVER_ID)->portNumber();
            const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            AddressFactory     factory;
            Socket            *socket = factory.allocate();
            btesos_TcpChannel  channel(socket);

            ASSERT(0 == socket->connect(SERVER_ADDRESS));
            ASSERT(0 == channel.isInvalid());
            {
                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "BUFFER_SIZE = " << BUFFER_SIZE
                           << MTENDL;
                }

                // Initiate test.
                char length[sizeof(int)];
                bdex_ByteStreamImpUtil::putInt32(length, BUFFER_SIZE);
                channel.write(length, sizeof length);

                // Block, while test executes.
                char buf[1];
                channel.read(buf, sizeof buf);
            }

            // TBD -- if this delay is NOT introduced
            // the process may get SIGPIPE (the error is intermittent).
            // This should be considered as a bug in the test driver.
            // The temporary fix -- put a delay so that all writes
            // are done.
            bcemt_ThreadUtil::microSleep(10000, 0);
            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY TEST APPARATUS
        //
        // Concerns:
        //   Verify the correct behavior of the primary test apparatus.
        //
        //   Currently, there is no primary test apparatus to verify, so this
        //   test case remains empty.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Primary Test Apparatus" << endl
                 << "==============================" << endl;
        }
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //   * That it is possible to instantiate a 'btemt_ChannelOutStreamBuf'
        //     object with a variety of 'minBytesToSend' and 'maxBuffers'
        //     parameters, and use the object to send data.
        //
        //   * That a registered send notification callback function is called.
        //
        // Plan:
        //   Create a number of servers with varying buffer sizes.  Within each
        //   server, allocate a modifiable 'btemt_ChannelOutStreamBuf' object,
        //   'mX'.  Set a send notification callback function for 'mX' that
        //   is bound to a counter, and increments the counter on each buffer
        //   transmission.  Iterate over a set of test vectors varying in high
        //   and low water marks.  For each test vector, send a series of
        //   messages, and verify the that the number of buffers sent is equal
        //   to one plus the message length divided by the buffer size.
        //
        // Testing:
        //   btemt_ChannelOutStreamBuf(
        //           int                             channelId,
        //           btemt_ChannelPool              *channelPool,
        //           bcema_PooledBufferChainFactory *bufferChainFactory,
        //           int                             minBytesToSend,
        //           int                             maxBuffers,
        //           int                             msBetweenSends,
        //           bslma_Allocator                *basicAllocator = 0);
        //   ~btemt_ChannelOutStreamBuf();
        //   void setSendNotificationCb(const Callback& callback);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Primary Manipulators" << endl
                 << "============================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            const struct {
                int d_line;        // source line number
                int d_serverId;
                int d_bufferSize;  // size of buffer chain buffers
                int d_backlog;     // listen port backlog
            } DATA[] = {
                //Line  Server Id   Buffer Size  Backlog
                //----  ---------   -----------  -------
                { L_,   0xc001d00d, 1,           2,     },
                { L_,   0xc01d50da, 2,           2,     },
                { L_,   0xbabef00d, 3,           2,     },
                { L_,   0xd099f00d, 4,           2,     },
            };
            enum { NUM_SERVERS = sizeof DATA / sizeof *DATA };
            int portNumbers[NUM_SERVERS];

            enum {
                MAX_CLIENTS  = NUM_SERVERS,
                MAX_THREADS  = NUM_SERVERS
            };
            my_ServerPool               sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case2DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));

            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            if (veryVerbose) {
                cout << "\n\tStart servers."
                     << "\n\t--------------" << endl;
            }
            for (int i = 0 ; i < NUM_SERVERS; ++i) {
                const int LINE        = DATA[i].d_line;
                const int SERVER_ID   = DATA[i].d_serverId;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;
                const int BACKLOG     = DATA[i].d_backlog;

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "i = "         << i    << ", "
                           << "LINE = "      << LINE << ", "
                           << "SERVER_ID = "
                           << bsl::hex << SERVER_ID << bsl::dec << ", "
                           << "BUFFER_SIZE = " << BUFFER_SIZE
                           << MTENDL;
                }

                const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
                LOOP2_ASSERT(i, LINE, 0 == sp.start(SERVER_ID, BUFFER_SIZE,
                                                    ADDRESS, BACKLOG));
                const int PORT = sp.pool()->serverAddress(SERVER_ID)
                                          ->portNumber();
                const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

                portNumbers[i] = PORT;
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            if (veryVerbose) {
                cout << "\n\tExecute tests."
                     << "\n\t--------------" << endl;
            }

            AddressFactory factory;

            for (int i = 0; i < NUM_SERVERS; ++i) {
                const int SERVER_ID   = DATA[i].d_serverId;
                const int BUFFER_SIZE = DATA[i].d_bufferSize;
                const int PORT        = portNumbers[i];

                const bteso_IPv4Address ADDRESS("127.0.0.1", PORT);
                if (veryVerbose) {
                    MTCOUT << "\n\t"
                           << "Connecting to " << ADDRESS << ", "
                           << "SERVER_ID = "
                           << bsl::hex << SERVER_ID << bsl::dec
                           << MTENDL;
                }
                Socket            *socket = factory.allocate();
                btesos_TcpChannel  channel(socket);
                ASSERT(0 == socket->connect(ADDRESS));
                ASSERT(0 == channel.isInvalid());

                enum { GET_DATA_SIZE = -1 };    // query size of test set

                int  message = GET_DATA_SIZE;
                char buffer[sizeof(int)];
                bdex_ByteStreamImpUtil::putInt32(buffer, message);
                channel.write(buffer, sizeof buffer);

                // Read and verify response.
                int DATA_SIZE;
                ASSERT(sizeof(int) == channel.read(buffer, sizeof buffer));
                bdex_ByteStreamImpUtil::getInt32(&DATA_SIZE, buffer);
                ASSERT(0 < DATA_SIZE);

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "SERVER_ID = "
                           << bsl::hex << SERVER_ID << bsl::dec << ", "
                           << "BUFFER_SIZE = " << BUFFER_SIZE   << ", "
                           << "DATA_SIZE = "   << DATA_SIZE
                           << MTENDL;
                }

                for (int j = 0; j < DATA_SIZE; ++j) {
                    // Initiate next test.
                    bdex_ByteStreamImpUtil::putInt32(buffer, j);
                    channel.write(buffer, sizeof buffer);

                    // Read null-terminated response
                    bsl::string message;
                    do {
                        char buf[128];
                        int  n = channel.readRaw(buf, sizeof buf);
                        ASSERT(0 < n);
                        message.append(buf, n);
                    } while (0 != *(message.end() - 1));

                    if (veryVerbose) {
                        MTCOUT << '\t'
                               << "SERVER_ID = "
                               << bsl::hex << SERVER_ID << bsl::dec    << ", "
                               << "BUFFER_SIZE = " << BUFFER_SIZE      << ", "
                               << "j = "           << j                << ", "
                               << "message.length() = "
                                                   << message.length() << ", "
                               << "message = "     << message
                               << MTENDL;
                    }
                }
                channel.invalidate();
                factory.deallocate(socket);
            }
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise the basic functionality of the
        //   'btemt_ChannelOutStreamBuf' class.  We want to ensure that channel
        //   output streambuf objects can be instantiated and destroyed.  We
        //   also want to exercise the primary manipulators and accessors.
        //
        // Plan:
        //   Create a modifiable 'btemt_ChannelOutStreamBuf' object 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.
        //
        // Testing:
        //   Exercise basic functionality.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "BREATHING TEST" << endl
                 << "==============" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 2,
                MAX_THREADS  = 2,

                SERVER_ID    = 0xc01d50da,
                BUFFER_SIZE  = 32,
                BACKLOG      = 1
            };
            my_ServerPool               sp(MAX_CLIENTS, MAX_THREADS, &ta);
            my_ServerPool::DataReadCallback dcb(
                    bdef_BindUtil::bindA( &ta
                                        , &case1DataCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , sp.pool()
                                        , &ta));
            sp.setDataCallback(dcb);

            if (veryVerbose) {
                my_ServerPool::ChannelCallback ccb(&channelStateMonitor, &ta);
                sp.setChannelCallback(ccb);
            }

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == sp.start(SERVER_ID, BUFFER_SIZE, ADDRESS, BACKLOG));
            const int PORT = sp.pool()->serverAddress(SERVER_ID)->portNumber();
            const bteso_IPv4Address SERVER_ADDRESS("127.0.0.1", PORT);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            AddressFactory     factory;
            Socket            *socket = factory.allocate();
            btesos_TcpChannel  channel(socket);

            ASSERT(0 == socket->connect(SERVER_ADDRESS));
            ASSERT(0 == channel.isInvalid());
            {
                const bsl::string MESSAGE = "0123456789abcdefgh"
                                            "ijklmnopqrstuvwxyz"
                                            "!@#$%^&*()[]{}<>=+"
                                            "ABCDEFGHIJKLMNOPQR";

                // Write message.
                char length[sizeof(int)];
                bdex_ByteStreamImpUtil::putInt32(length, MESSAGE.length());
                channel.write(length, sizeof length);
                channel.write(MESSAGE.c_str(), MESSAGE.length());

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "MESSAGE.length() = " << MESSAGE.length() << ", "
                           << "MESSAGE = "          << MESSAGE
                           << MTENDL;
                }

                // Read and verify response.
                bsl::string message;

                do {
                    char buf[128];
                    int  n = channel.readRaw(buf, sizeof buf);
                    ASSERT(0 < n);
                    message.append(buf, n);
                } while (0 != *(message.end() - 1));
                message.erase(message.end() - 1);

                if (veryVerbose) {
                    MTCOUT << '\t'
                           << "message.length() = " << message.length() << ", "
                           << "message = "          << message
                           << MTENDL;
                }
                ASSERT(MESSAGE == message);
            }
            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == sp.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.  Make use of existing test apparatus by instantiating
        //   objects with a 'bslma_TestAllocator' object where applicable.
        //   Additionally, replace all calls to 'assert' in the usage example
        //   with calls to 'ASSERT'.  This now becomes the source, which is
        //   then "copied" back to the header file by reversing the above
        //   process.
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Usage Example 2" << endl
                 << "=======================" << endl;
        }

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 8,
                MAX_THREADS  = 8,

                SERVER_ID = 0xc01d50da,
                PORT      = 111066,
                BACKLOG   = 8,
                DATA_PROB = 3,  // reciprocal probability of generating data

                NUM_ATTEMPTS     = 5, // maximum connection attempts per client
                ATTEMPT_INTERVAL = 1, // connection attempt interval (seconds)

                BUFFER_SIZE = 256  // local pooled buffer size
            };
            my_DataServer ds(MAX_CLIENTS, MAX_THREADS, 1.0 / DATA_PROB, &ta);

            const bteso_IPv4Address ADDRESS("127.0.0.1", PORT);
            ASSERT(0 == ds.start(SERVER_ID, ADDRESS, BACKLOG));

            btemt_ChannelPool *poolAddr = 0;
            bcemt_Barrier      barrier(2);
            bsl::vector<int>   channels(&ta);

            Pool::ChannelStateChangeCallback ccb(
                    bdef_BindUtil::bindA( &ta
                                        , &ue2ChannelStateCallback
                                        , _1
                                        , _2
                                        , _3
                                        , _4
                                        , &poolAddr
                                        , &channels
                                        , &barrier));

            Pool::DataReadCallback          dcb(&ue2DataCallback, &ta);
            Pool::PoolStateChangeCallback   pcb;

            Config cpc;
            cpc.setMaxConnections(BACKLOG);
            cpc.setMaxThreads(BACKLOG);
            cpc.setMetricsInterval(10.0);
            cpc.setIncomingMessageSizes(6, 216, 1024);

            Pool pool(ccb, dcb, pcb, cpc, &ta);
            poolAddr = &pool;
            ASSERT(0 == pool.start());

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            static
            const struct {
                int d_line;      // source line number
                int d_cacheSize; // number of data items to buffer at server
                int d_maxDelay;  // max delay for receiving cached data (ms)
            } DATA[] = {
                //Line  Cache Size  Max Delay
                //----  ----------  ---------
                { L_,   4,          10000,   },
                { L_,   3,          5000,    },
                { L_,   0,          0,       },
                { L_,   1,          1000,    },
                { L_,   30,         29500,   },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            BufferFactory factory(BUFFER_SIZE, &ta);

            for (int i = 0; i < DATA_SIZE; ++i) {
                const int LINE       = DATA[i].d_line;
                const int CACHE_SIZE = DATA[i].d_cacheSize;
                const int MAX_DELAY  = DATA[i].d_maxDelay;

                if (veryVerbose) {
                    MTCOUT << 't'
                           << "i = "          << i          << ", "
                           << "LINE = "       << LINE       << ", "
                           << "CACHE_SIZE = " << CACHE_SIZE << ", "
                           << "MAX_DELAY = "  << MAX_DELAY
                           << MTENDL;
                }

                bdet_TimeInterval interval(ATTEMPT_INTERVAL);
                LOOP2_ASSERT(i, LINE, 0 == pool.connect(ADDRESS,
                                                        NUM_ATTEMPTS,
                                                        interval,
                                                        SERVER_ID));
                barrier.wait();

                // Send Connect message.
                Chain                      *chain = factory.allocate(1);
                bdesb_FixedMemOutStreamBuf  outbuf(chain->buffer(0),
                                                   chain->bufferSize());
                bdex_ByteOutStreamFormatter bos(&outbuf);
                bdem_List                   connect;
                connect.appendInt(CACHE_SIZE);
                connect.appendInt(MAX_DELAY);
                const int VERSION = bdem_List::maxSupportedBdexVersion();
                bos.putVersion(VERSION);
                bdex_OutStreamFunctions::streamOut(bos, connect, VERSION);
                bos << bdexFlush;

                chain->setLength(outbuf.length());
                btemt_DataMsg msg(chain, &factory, channels[i]);
                ASSERT(0 == pool.write(msg.channelId(), msg));
            }

            bcemt_ThreadUtil::sleep(bdet_TimeInterval(30.5));
            for (int i = 0; i < (int)channels.size(); ++i) {
                pool.shutdown(channels[i], btemt_ChannelPool::BTEMT_IMMEDIATE);
            }
            ASSERT(0 == ds.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
