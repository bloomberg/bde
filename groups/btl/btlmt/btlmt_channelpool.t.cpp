// btlmt_channelpool.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlmt_channelpool.h>

#include <btlmt_channelpoolconfiguration.h>
#include <btlmt_asyncchannel.h>

#include <btls_iovecutil.h>
#include <btlso_flag.h>
#include <btlso_inetstreamsocketfactory.h>
#include <btlso_ipv4address.h>
#include <btlso_resolveutil.h>
#include <btlso_streamsocket.h>
#include <btlso_socketoptions.h>
#include <btlso_socketoptutil.h>
#include <btlsos_tcpchannel.h>
#include <btlsos_tcptimedacceptor.h>
#include <btlsos_tcptimedchannel.h>

#include <bdlcc_queue.h>
#include <bslma_testallocator.h>
#include <btlb_blobutil.h>
#include <bdlma_concurrentpool.h>
#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>
#include <bdlmt_fixedthreadpool.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bdlf_memfn.h>
#include <bdlb_hashutil.h>
#include <bdlb_print.h>
#include <bdlb_tokenizer.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bslx_byteoutstream.h>
#include <bslx_byteinstream.h>
#include <bslx_instreamfunctions.h>
#include <bslx_marshallingutil.h>
#include <bslx_outstreamfunctions.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_functional.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsls_assert.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <bsl_c_signal.h>
#include <sys/resource.h>
#endif

using namespace bsl;  // automatically added by script

using namespace BloombergLP;
using namespace bdlf::PlaceHolders;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver has grown in an ad-hoc manner and is still incomplete.  A
// thorough review is still required, but some test cases have been
// individually strengthened to the point that confidence is reasonable that
// core functionality is working.  Among the tests that should still be
// performed:
//   o enabling the read and write limits and test the channel pool in the same
//     manner as a standard BSLMA_EXCEPTION_TEST.
//   o all the missing functions below should be exercised
//   o FILL ME
//
// We begin by a simple breathing test which ensures that a channel pool can be
// created, started, and listen on a specified port, and that data sent to that
// port is received by the channel pool.  Next, we test each function in
// isolation (connect, import, shutdown) or in isolated pairs (start/stop,
// listen/close, registerClock/deregisterClock, enableRead/disableRead).  The
// remaining concerns are about various facilities such as the statistics
// section, concerns about the interplay of various functions.
//-----------------------------------------------------------------------------
// CLASS 'btlmt::ChannelPool'
// [ 2]  btlmt::ChannelPool(...);
// [ 2]  ~btlmt::ChannelPool();
// [ 3]  int btlmt::ChannelPool::start(...);
// [ 3]  int btlmt::ChannelPool::stop(...);
// [ 4]  int btlmt::ChannelPool::connect(...);
// [ 5]  int btlmt::ChannelPool::listen(int port, ...);
// [ 4]  int btlmt::ChannelPool::listen(const IPv4Address&  address, ...);
// [ 5]  int btlmt::ChannelPool::close(int serverID);
// [ 6]  int btlmt::ChannelPool::import(...);
// [ 8]  int btlmt::ChannelPool::shutdown();
// [ 9]  int btlmt::ChannelPool::write(btes::Iovecs, ...);
// [ 9]  int btlmt::ChannelPool::write(btes::Ovecs, ...);
// [10]  int btlmt::ChannelPool::registerClock(...);
// [10]  int btlmt::ChannelPool::deregisterClock(...);
// [11]  int btlmt::ChannelPool::enableRead(int channelId);
// [11]  int btlmt::ChannelPool::disableRead(int channelId);
// [25]  int btlmt::ChannelPool::setWriteCacheHiWatermark(int, int);
// [25]  int btlmt::ChannelPool::setWriteCacheLowWatermark(int, int);
// [25]  int btlmt::ChannelPool::setWriteCacheWatermarks(int, int, int);
// [  ]  void *btlmt::ChannelPool::channelContext(int channelId);
// [  ]  void btlmt::ChannelPool::setChannelContext(int channelId, ...);
// [  ]  int btlmt::ChannelPool::outboundBufferFactory();
// [  ]  int btlmt::ChannelPool::outboundBlobBufferFactory();
// [  ]  int btlmt::ChannelPool::set*Options(...);
// [  ]  int btlmt::ChannelPool::get*Options(...) const;
// [  ]  int btlmt::ChannelPool::numChannels() const;
// [  ]  int btlmt::ChannelPool::numEvents() const;
// [  ]  int btlmt::ChannelPool::numThreads() const;
// [13]  double btlmt::ChannelPool::reportWeightedAverageReset();
// [28]  int btlmt::ChannelPool::busyMetrics() const;
// [14]  int btlmt::ChannelPool::getChannelStatistics*(...);
// [14]  int btlmt::ChannelPool::numBytes*(...);
// [14]  int btlmt::ChannelPool::totalBytes*(...);
// [  ]  const btlso::IPv4Address *ChannelPool::serverAddress(...) const;
//
// CLASS 'btlmt::ChannelPool_MessageUtil'
// [26] btlmt::ChannelPool_IovecArray<>
// [26] bsls::Types::Int64 IovecArray<>::length() const;
// [26] static bsls::Types::Int64 length(...);
// [26] static int write(btlso::StreamSocket<btlso::IPv4Address> *, ... )
// [26] static int loadIovec(btls::Iovec *, ... );
// [26] static int loadBlob(btlb::Blob *, ... );
// [26] static void appendToBlob(btlb::Blob *, ... );
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] CONCERN: Import half-closed socket pair
// [12] CONCERN: Flow control
// [15] CONCERN: Shutdown inside channel state callback
// [11] CONCERN: Number of channels is as specified in configuration
// [17] CONCERN: shutdown() results in correct channel Id in the callback
// [18] CONCERN: Ensure that Channel pool respects max connections limit
// [19] CONCERN: Confirm failure to accept connections if FD limit is reached
// [20] CONCERN: Confirm that 'import' works under high load
// [21] CONCERN: Ssl-like sockets
// [22] CONCERN: Stress test
// [23] CONCERN: Half-open connections
// [27] CONCERN: Read timeout
// [28] TESTING: 'busyMetrics' and time metrics collection.
// [28] CONCERN: Event Manager Allocation
// [30] Implementing a QueueProcessor
// [37] USAGE EXAMPLE
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
namespace {
int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

}  // close unnamed namespace

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

static bslmt::Mutex coutMutex;

#define MTCOUT   { coutMutex.lock(); cout \
                                      << bslmt::ThreadUtil::selfIdAsUint64()  \
                                      << ": "
#define MTENDL   endl << bsl::flush ;  coutMutex.unlock(); }
#define MTFLUSH  bsl::flush; } coutMutex.unlock()

#define PT(X) { MTCOUT << #X " = " << (X) << MTENDL; }
#define PT2(X,Y) { MTCOUT << #X " = " << (X) << ", " \
                          << #Y " = " << (Y) << MTENDL; }
#define PT3(X,Y,Z) { MTCOUT << #X " = " << (X) << ", " \
                            << #Y " = " << (Y) << ", " \
                            << #Z " = " << (Z) << MTENDL; }
                                                 // Print identifier and value.

// The following macro facilitates outputting a vector of events.

template <class TYPE>
void printVector(const bsl::vector<TYPE>& vec, const char *name)
{
    cout << name << " = {";
    copy(vec.begin(), vec.end(), ostream_iterator<TYPE>(cout, "\n\t"));
    cout << "\n}" << endl;
}

#define PV(X) printVector( (X), #X);

#define MTASSERT(X) { \
  if (!(X)) { MTCOUT; aSsErT(1, #X, __LINE__); cout << MTFLUSH; } }

#define MTLOOP_ASSERT(I,X) { \
  if (!(X)) { MTCOUT; P(I); aSsErT(1, #X, __LINE__); cout << MTFLUSH; }}

#define MTLOOP2_ASSERT(I,J,X) { \
  if (!(X)) { MTCOUT; P_(I); P(J); aSsErT(1, #X, __LINE__); cout << MTFLUSH; }}

namespace BloombergLP {
namespace bslma {
bsl::ostream& operator<<(bsl::ostream& out, const TestAllocator& ta)
{
    ta.print();
    return out;
}
}  // close namespace bslma
}  // close enterprise namespace

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef btlmt::ChannelPool   Obj;
typedef btlso::SocketOptUtil SockOpt;
typedef btlso::SocketOptions SocketOptions;
typedef btlso::LingerOptions LingerOptions;
typedef bsls::Types::Uint64  ThreadId;

const ThreadId NULL_THREAD_ID = (ThreadId) (long long) -1;

static void assertCb() {
    ASSERT("This function should NOT be called" && 0);
}

static int          verbose = 0;
static int          veryVerbose = 0;
static int          veryVeryVerbose = 0;
static int          ARGC = 0;
static char       **ARGV = 0;

static bsl::function<void()> NULL_CB(&assertCb);

//=============================================================================
//                       TESTING FUNCTIONS/CLASSES HELPER
//-----------------------------------------------------------------------------

template <class A1, class A2, class A3>
struct NullFn3
{
    void operator()(A1, A2, A3) {}
};

template <class A1, class A2, class A3, class A4>
struct NullFn4
{
    void operator()(A1, A2, A3, A4) {}
};

template <class A1, class A2, class A3>
void makeNull(bsl::function<void(A1, A2, A3)> * f)
{
    *f = NullFn3<A1, A2, A3>();
}

template <class A1, class A2, class A3>
void makeNull(bslma::Allocator *a, bsl::function<void(A1, A2, A3)> * f)
{
    *f = bsl::function<void(A1, A2, A3)>(
                       bsl::allocator_arg_t(),
                       bsl::allocator<bsl::function<void(A1, A2, A3)> >(a),
                       NullFn3<A1, A2, A3>());
}

template <class A1, class A2, class A3, class A4>
void makeNull(bsl::function<void(A1, A2, A3, A4)> * f)
{
    *f = NullFn4<A1, A2, A3, A4>();
}

template <class A1, class A2, class A3, class A4>
void makeNull(bslma::Allocator *a, bsl::function<void(A1, A2, A3, A4)> *f)
{
    *f = bsl::function<void(A1, A2, A3, A4)>(
                       bsl::allocator_arg_t(),
                       bsl::allocator<bsl::function<void(A1, A2, A3, A4)> >(a),
                       NullFn4<A1, A2, A3, A4>());
}

struct ChannelState {
    enum Type {
        BTEMT_CHANNEL_DOWN = 0,   // the connection is broken
        BTEMT_CHANNEL_UP,         // the connection is established
        BTEMT_READ_TIMEOUT,       // timed out waiting for incoming data
        BTEMT_SEND_BUFFER_FULL,   // the outgoing buffer is full *OBSOLETE*
        BTEMT_MESSAGE_DISCARDED,  // message is discarded (can't be sent)
        BTEMT_AUTO_READ_ENABLED,  // automatic reading is enabled on a channel
        BTEMT_AUTO_READ_DISABLED, // automatic reading disabled on a channel
        BTEMT_WRITE_CACHE_LOWWAT, // write cache low watermark
        BTEMT_WRITE_CACHE_HIWAT   // write cache hi watermark
           = BTEMT_SEND_BUFFER_FULL
    };
};

struct PoolState {
    enum Type {
        BTEMT_ACCEPT_TIMEOUT = 0,           // timed out accepting a connection
        BTEMT_ERROR_ACCEPTING,              // error accepting a connection
        BTEMT_ERROR_CONNECTING,             // error connecting to the peer
        BTEMT_CHANNEL_LIMIT,                // channel limit reached
        BTEMT_CAPACITY_LIMIT,               // capacity limit reached
        BTEMT_ERROR_BINDING_CLIENT_ADDR,    // error binding client address
        BTEMT_ERROR_SETTING_OPTIONS,        // error setting socket options
        BTEMT_EVENT_MANAGER_LIMIT           // event manager limit reached
    };
};

struct my_ChannelEvent {
    bsls::TimeInterval          d_when;
    bslmt::ThreadUtil::Handle   d_thread;
    bsls::Types::Uint64         d_threadId;
    int                         d_channelId;
    int                         d_allocatorId;
    ChannelState::Type          d_event;
    void                       *d_context;
};

bsl::ostream& operator<<(bsl::ostream& s, const my_ChannelEvent& event) {
    s << "{ "
      << event.d_when     << ", "
      << event.d_threadId << ", "
      << event.d_context
      << " }";
    return s;
}

struct my_PoolEvent {
    bsls::TimeInterval         d_when;
    bslmt::ThreadUtil::Handle  d_thread;
    bsls::Types::Uint64        d_threadId;
    int                        d_sourceId;
    PoolState::Type            d_state;
};

bsl::ostream& operator<<(bsl::ostream& s, const my_PoolEvent& event) {
    s << "{ "
      << event.d_when     << ", "
      << event.d_threadId << ", "
      << "}";
    return s;
}

static
btlso::IPv4Address getLocalAddress() {
    // On Cygwin or Windows binding to btlso::IPv4Address() doesn't seem to
    // work.  Wants to bind to localhost/127.0.0.1.

#if defined(BSLS_PLATFORM_OS_CYGWIN) || defined(BSLS_PLATFORM_OS_WINDOWS)
    return btlso::IPv4Address("127.0.0.1", 0);
#else
    return btlso::IPv4Address();
#endif

}

static btlso::IPv4Address getServerLocalAddress(btlmt::ChannelPool *pool, int serverId)
{
    // On Cygwin or Windows binding to btlso::IPv4Address() doesn't seem to
    // work.  Wants to bind to localhost/127.0.0.1.

#ifdef BSLS_PLATFORM_OS_WINDOWS
    btlso::IPv4Address tmp;
    pool->getServerAddress(&tmp, serverId);
    btlso::IPv4Address peer(getLocalAddress());
    peer.setPortNumber(tmp.portNumber());
    return peer;
#else
    btlso::IPv4Address peer;
    pool->getServerAddress(&peer, serverId);
    return peer;
#endif
}

static
void recordChannelState(int                           channelId,
                        int                           sourceId,
                        int                           state,
                        void                         *context,
                        bsl::vector<my_ChannelEvent> *results,
                        bslmt::Mutex                 *resultsLock)
    // Record the occurrence of this event into the specified 'results'
    // array of events using the specified 'resultsLock' for synchronization.
{
    ASSERT(results);
    ASSERT(resultsLock);

    my_ChannelEvent event;
    event.d_when = bdlt::CurrentTime::now();
    event.d_thread = bslmt::ThreadUtil::self();
    event.d_threadId = bslmt::ThreadUtil::selfIdAsUint64();
    event.d_channelId = channelId;
    event.d_allocatorId = sourceId;
    event.d_event = (ChannelState::Type) state;
    event.d_context = context;
    bslmt::LockGuard<bslmt::Mutex> lock(resultsLock);
    results->push_back(event);
}

static
void recordPoolState(int                        state,
                     int                        sourceId,
                     int,
                     bsl::vector<my_PoolEvent> *results,
                     bslmt::Mutex              *resultsLock)
    // Record the occurrence of this event into the specified 'results'
    // array of events using the specified 'resultsLock' for synchronization.
{
    ASSERT(results);
    ASSERT(resultsLock);

    my_PoolEvent event;
    event.d_when = bdlt::CurrentTime::now();
    event.d_thread = bslmt::ThreadUtil::self();
    event.d_threadId = bslmt::ThreadUtil::selfIdAsUint64();
    event.d_sourceId = sourceId;
    event.d_state = (PoolState::Type) state;

    bslmt::LockGuard<bslmt::Mutex> lock(resultsLock);
    results->push_back(event);
}

class ChannelPoolStateCbTester {
    // This class is a wrapper around a 'btlmt::ChannelPool' that provides a
    // 'waitForState' method for testing whether a particular channel pool
    // state event was delivered by the channel pool via the channel state
    // callback.  On construction, a 'ChannelPoolStateCbTester' initializes a
    // channel pool using the supplied channel pool configuration.  Clients can
    // then access and modify the created channel pool, and call 'waitForState'
    // to wait for a particular channel state callback.

  public:
    // PUBLIC TYPES
    struct ChannelState {
        // A channel state object represents the parameters passed to the
        // 'channelStateCb'.

        int                 d_channelId;   // channel id for the state callback
        int                 d_sourceId;    // source id for the state callback
        int                 d_state;       // state for the state callback
        bsls::Types::Uint64 d_threadId;    // thread id for the state callback

        ChannelState(int                 channelId,
                     int                 sourceId,
                     int                 state,
                     bsls::Types::Uint64 threadId)
        : d_channelId(channelId)
        , d_sourceId(sourceId)
        , d_state(state)
        , d_threadId(threadId)
        {
        }
    };

  private:
    // DATA
    bslma::ManagedPtr<btlmt::ChannelPool>
                        d_channelPool_p;  // wrapped pool

    int                 d_lastChannelId;  // id of the most recently open
                                          // channel

    bsl::vector<ChannelState>
                        d_channelStates;  // sequence of states since last
                                          // 'waitForState'

    bslmt::Mutex        d_mutex;          // synchronize access to data

    bslmt::Condition    d_condition;      // wait for a state callback

    void channelStateCb(int channelId, int sourceId, int state, void *)
        // Append the specified 'state' to the queue of channel states, and if
        // 'waitForState' is currently blocked waiting for a state, wake up the
        // waiting thread to return the updated state information.
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        d_channelStates.push_back(
                         ChannelState(channelId,
                                      sourceId,
                                      state,
                                      bslmt::ThreadUtil::selfIdAsUint64()));
        switch (state) {
          case btlmt::ChannelPool::e_CHANNEL_DOWN: {
              if (veryVerbose) {
                  MTCOUT << "Connection terminated:"
                         << " serverId="  << bsl::hex << sourceId << bsl::dec
                         << " channelId=" << channelId
                         << MTENDL;
              }
              d_channelPool_p->shutdown(channelId,
                                        btlmt::ChannelPool::e_IMMEDIATE);

          } break;
          case btlmt::ChannelPool::e_CHANNEL_UP: {
              if (veryVerbose) {
                  MTCOUT << "Connection established:"
                         << " serverId="  << bsl::hex << sourceId << bsl::dec
                         << " channelId=" << channelId
                         << MTENDL;
              }
              d_lastChannelId = channelId;

          } break;
          case btlmt::ChannelPool::e_READ_TIMEOUT: {
              if (veryVerbose) {
                  MTCOUT << "READ TIMEOUT " << MTENDL;
              }
          } break;
          case btlmt::ChannelPool::e_WRITE_CACHE_HIWAT: {
              if (veryVerbose) {
                  MTCOUT << "HIWAT " << MTENDL;
              }
          } break;
          case btlmt::ChannelPool::e_WRITE_CACHE_LOWWAT: {
              if (veryVerbose) {
                  MTCOUT << "LOWWAT " << MTENDL;
              }
          } break;
        }
        d_condition.broadcast();
    }

  public:
    // CREATORS
    ChannelPoolStateCbTester(
                    const btlmt::ChannelPoolConfiguration&  config,
                    bslma::Allocator                       *basicAllocator = 0)
        // Initialize a channel pool state callback tester using the specified
        // 'config'.  This object will manage an underlying
        // 'btlmt::ChannelPool', configured with 'config', and will report any
        // state changes to that channel pool using the 'waitForState' method.
        // The underlying channel pool will be initialized with a null data
        // and pool-state callback.
    : d_channelPool_p(0)
    , d_lastChannelId(0)
    , d_mutex()
    , d_condition()
    {
        bslma::Allocator *ma = bslma::Default::allocator(basicAllocator);
        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
      bsl::allocator_arg_t(),
      bsl::allocator<btlmt::ChannelPool::ChannelStateChangeCallback>(ma),
      bdlf::MemFnUtil::memFn(&ChannelPoolStateCbTester::channelStateCb, this));

        btlmt::ChannelPool::BlobBasedReadCallback    dataCb;
        btlmt::ChannelPool::PoolStateChangeCallback  poolCb;

        makeNull(&dataCb);
        makeNull(&poolCb);

        d_channelPool_p.load(
           new (*ma) btlmt::ChannelPool(channelCb, dataCb, poolCb, config, ma),
           ma);
    }

    ChannelPoolStateCbTester(
               const btlmt::ChannelPoolConfiguration&       config,
               btlmt::ChannelPool::BlobBasedReadCallback    dataCb,
               btlmt::ChannelPool::PoolStateChangeCallback  poolCb,
               bslma::Allocator                            *basicAllocator = 0)
        // Initialize a channel pool state callback tester using the specified
        // 'config', 'dataCb', and 'poolCb'.  This object will manage an
        // underlying 'btlmt::ChannelPool', configured with 'config', and will
        // report any state changes to that channel pool using the
        // 'waitForState' method.  The underlying channel pool will be
        // initialized with the provided data and pool-state callbacks.
    : d_channelPool_p(0)
    , d_lastChannelId(0)
    , d_mutex()
    , d_condition()
    {
        bslma::Allocator *ma = bslma::Default::allocator(basicAllocator);
        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
      bsl::allocator_arg_t(),
      bsl::allocator<btlmt::ChannelPool::ChannelStateChangeCallback>(ma),
      bdlf::MemFnUtil::memFn(&ChannelPoolStateCbTester::channelStateCb, this));

        d_channelPool_p.load(
           new (*ma) btlmt::ChannelPool(channelCb, dataCb, poolCb, config, ma),
           ma);
    }

    // MANIPULATORS
    btlmt::ChannelPool& pool()  { return *d_channelPool_p; }

    // ACCESSORS
    int waitForState(bsl::vector<ChannelState> *stateChanges,
                     int                        state,
                     const bsls::TimeInterval&  elapsedTime)
        // Wait for up to the specified 'elapsedTime' for a client to receive
        // a channel state callback indicating the specified 'state'; return 0
        // and append to the specified 'stateChanges' the list of channel
        // states since the previous successful call to 'waitForState' on
        // success; return a non-zero value if the 'elapsedTime' passes
        // without seeing the indicated 'state'.
    {
        bsls::TimeInterval             now     = bdlt::CurrentTime::now();
        bsls::TimeInterval             timeout = now + elapsedTime;
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        int                            idx   = 0;
        bool                           found = false;

        while (!found && now < timeout) {
            for (; idx < (int) d_channelStates.size(); ++idx) {
                if (d_channelStates[idx].d_state == state) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                d_condition.timedWait(&d_mutex,
                                      now + bsls::TimeInterval(0.01));
            }
            now = bdlt::CurrentTime::now();
        }
        if (found) {
            *stateChanges = d_channelStates;
            d_channelStates.clear();
            return 0;                                                 // RETURN
        }
        return -1;
    }

    int lastOpenedChannelId()
        // Return the identifier for the most recent channel to receive the
        // 'CHANNEL_UP' state.
    { return d_lastChannelId; }
};

void populateMessage(btlb::Blob       *msg,
                     int               length,
                     bslma::Allocator *allocator)
    // Populate the specified 'msg' with a buffer of the specified 'length'
    // containing arbitrary data (starting at -1 (0xFF) and decreasing by 1 per
    // element) using the specified 'allocator' to supply memory;
{
    ASSERT(0 < length);

    bsl::shared_ptr<char> buffer =
            bslstl::SharedPtrUtil::createInplaceUninitializedBuffer(length,
                                                                    allocator);

    char  value = -1; // 0xff
    char *c     = buffer.get();
    char *end   = c + length;

    for (; c < end; ++c) {
        *c = value--;
    }
    btlb::BlobBuffer blobBuffer(buffer, length);
    msg->appendDataBuffer(blobBuffer);
}

//-----------------------------------------------------------------------------
//                                  TEST_CASE_CTOR_TAKING_FACTORY
//-----------------------------------------------------------------------------

namespace TEST_CASE_CTOR_TAKING_FACTORY {

void poolStateCb(int state, int source, int severity)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Pool state callback called with"
                  << " State: " << state
                  << " Source: "  << source
                  << " Severity: " << severity << bsl::endl;
    }
}

void channelStateCb(int             channelId,
                    int             serverId,
                    int             state,
                    void           *,
                    int            *id,
                    bslmt::Barrier *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }
    if (btlmt::ChannelPool::e_CHANNEL_UP == state) {
        *id = channelId;
        barrier->wait();
    }
}

void blobBasedReadCb(int            *needed,
                     btlb::Blob     *msg,
                     int             channelId,
                     void           *,
                     bslmt::Barrier *barrier,
                     btlb::Blob     *savedMsg)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }
    *needed = 1;

    barrier->wait();

    *savedMsg = *msg;
    msg->removeAll();
}

struct ReadData {
    btlso::StreamSocket<btlso::IPv4Address> *d_socket_p;
    int                                      d_numBytes;
};

void *readData(void *data)
{
    ReadData& td = *static_cast<ReadData *>(data);

    btlso::StreamSocket<btlso::IPv4Address> *socket    = td.d_socket_p;
    const int                                NUM_BYTES = td.d_numBytes;

    bsl::vector<char> buffer(NUM_BYTES, 0);

    int numRemaining = NUM_BYTES;
    do {
        int rc = socket->read(buffer.data(), numRemaining);
        if (rc != btlso::SocketHandle::e_ERROR_WOULDBLOCK) {
            numRemaining -= rc;
        }
        bslmt::ThreadUtil::microSleep(1000, 0);
    } while (numRemaining > 0);
    return 0;
}

class ReadServer
{
    // A read server.

    enum { SERVER_ID = 1066 };

    int                      d_port;
    btlb::Blob               d_lastMsg;
    int                      d_msgSize;
    bslma::Allocator        *d_allocator_p;
    btlmt::ChannelPool      *d_cp_p;
    bslmt::Semaphore         d_msgSema;

  private:
    // ChannelPool Callback Functions
    void poolCB(int state, int source, int severity);

    void chanCB(int channelId, int serverId, int state, void *arg);

    void blobBasedReadCb(int        *numNeeded,
                         btlb::Blob *msg,
                         int         channelId,
                         void       *context);
        // Blob based read callback.

    // Not Implemented
    ReadServer(const ReadServer&);
    ReadServer& operator=(const ReadServer&);

  public:
    // CREATORS
    ReadServer(btlb::BlobBufferFactory *factory,
               int                      msgSize,
               bslma::Allocator        *allocator = 0);
        // Create a server using the specified 'factory' for blob buffers
        // and the specified 'allocator' to supply other memory.

   ~ReadServer();
        // Terminate all open connections, and destroy the server.

    // MANIPULATORS
    int start();
        // Start the server.  Return 0 on success, and a non-zero value if
        // an error occurred.  The server is started in listen-mode with a
        // backlog equal to 'd_maxClients'.

    int stop();
        // Stop the server.  Immediately terminate all connections, but
        // leave server in listen-mode.  Returns zero if successful,
        // and non-zero if an error occurs.

    void msgWait()
    {
        d_msgSema.wait();
    }


    // ACCESSORS
    int portNumber() const;
        // Return the port number on the local host on which this server
        // listens to connections.

    const btlb::Blob& lastMsg() {
        return d_lastMsg;
    }
};

ReadServer::ReadServer(btlb::BlobBufferFactory *factory,
                       int                      msgSize,
                       bslma::Allocator        *allocator)
: d_port(0)
, d_lastMsg(allocator)
, d_msgSize(msgSize)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    btlmt::ChannelPoolConfiguration cpc;
    cpc.setMaxConnections(20);
    cpc.setMaxThreads(20);
    cpc.setMetricsInterval(10.0);
    cpc.setIncomingMessageSizes(1, 5, 10);

    btlmt::ChannelPool::PoolStateChangeCallback poolCb(
    bsl::allocator_arg_t(),
    bsl::allocator<btlmt::ChannelPool::PoolStateChangeCallback>(d_allocator_p),
    bdlf::MemFnUtil::memFn(&ReadServer::poolCB, this));

    btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
 bsl::allocator_arg_t(),
 bsl::allocator<btlmt::ChannelPool::ChannelStateChangeCallback>(d_allocator_p),
 bdlf::MemFnUtil::memFn(&ReadServer::chanCB, this));

    btlmt::ChannelPool::BlobBasedReadCallback dataFunctor =
                           bdlf::MemFnUtil::memFn(&ReadServer::blobBasedReadCb,
                                                  this);

    d_cp_p = new (*d_allocator_p) btlmt::ChannelPool(factory,
                                                     channelCb,
                                                     dataFunctor,
                                                     poolCb,
                                                     cpc,
                                                     allocator);
}

ReadServer::~ReadServer()
{
    d_cp_p->stop();
    d_allocator_p->deleteObject(d_cp_p);
}

int ReadServer::start()
{
    int    src = d_cp_p->start();
    int    lrc = d_cp_p->listen(0, 5, SERVER_ID);
    btlso::IPv4Address address = getServerLocalAddress(d_cp_p, SERVER_ID);
    d_port = address.portNumber();
    return src || lrc;
}

int ReadServer::stop()
{
    return d_cp_p->stop();
}

void ReadServer::poolCB(int, int, int)
{
}

void ReadServer::chanCB(int channelId, int, int state, void *)
{
    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        d_cp_p->shutdown(channelId, btlmt::ChannelPool::e_IMMEDIATE);
      } break;
    }
}

void ReadServer::blobBasedReadCb(int *needed, btlb::Blob *msg, int, void *)
{
    if (veryVerbose) {
        bsl::cout << "Blob Based Read Cb called with message of length: "
                  << msg->length() << bsl::endl;
    }

    if (veryVeryVerbose) {
        btlb::BlobUtil::hexDump(bsl::cout, *msg);
    }

    ASSERT(needed);
    ASSERT(msg);

    btlb::BlobUtil::append(&d_lastMsg, *msg);
    msg->removeAll();
    *needed = bsl::max(1, d_msgSize - d_lastMsg.length());

    if (d_lastMsg.length() >= d_msgSize) {
        d_msgSema.post();
    }
}

// ACCESSORS
int ReadServer::portNumber() const
{
    return d_port;
}

}  // close namespace TEST_CASE_CTOR_TAKING_FACTORY

//-----------------------------------------------------------------------------
//                                  TEST_CASE_TESTING_PEER_ADDRESS
//-----------------------------------------------------------------------------

namespace TEST_CASE_TESTING_PEER_ADDRESS {

btlmt::ChannelPool *d_pool_p = 0;
btlso::IPv4Address  d_peerAddress;

void poolStateCb(int state, int source, int severity)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Pool state callback called with"
                  << " State: " << state
                  << " Source: "  << source
                  << " Severity: " << severity << bsl::endl;
    }
}

void channelStateCb(int             channelId,
                    int             serverId,
                    int             state,
                    void           *,
                    int            *id,
                    bslmt::Barrier *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }
    if (btlmt::ChannelPool::e_CHANNEL_UP == state) {
        *id = channelId;
        d_pool_p->getPeerAddress(&d_peerAddress, channelId);
        barrier->wait();
    }
    else if (btlmt::ChannelPool::e_CHANNEL_DOWN == state) {
        btlso::IPv4Address peer;
        d_pool_p->getPeerAddress(&peer, channelId);
        ASSERT(d_peerAddress != peer);
        barrier->wait();
    }
}

void blobBasedReadCb(int            *needed,
                     btlb::Blob     *msg,
                     int             channelId,
                     void           *,
                     bslmt::Barrier *)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }
    *needed = 1;

    btlso::IPv4Address peer;
    d_pool_p->getPeerAddress(&peer, channelId);
    LOOP2_ASSERT(d_peerAddress, peer, d_peerAddress == peer);

    msg->removeAll();
}

}  // close namespace TEST_CASE_TESTING_PEER_ADDRESS

//-----------------------------------------------------------------------------
//                                  TEST_CASE_STOP_AND_REMOVE_CHANNELS
//-----------------------------------------------------------------------------

namespace TEST_CASE_STOP_AND_REMOVE_CHANNELS {

bslmt::Mutex                         mapMutex;
bsl::map<int, int>                   sourceIdToChannelIdMap;
typedef bsl::map<int, int>::iterator MapIter;

bsls::AtomicInt numUp(0);

void poolStateCb(int state, int source, int severity)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Pool state callback called with"
                  << " State: " << state
                  << " Source: "  << source
                  << " Severity: " << severity << bsl::endl;
    }
}

void channelStateCb(int              channelId,
                    int              sourceId,
                    int              state,
                    void            *)
{
    if (veryVerbose) {
        MTCOUT << "Channel state callback called with"
               << " Channel Id: " << channelId
               << " Server Id: "  << sourceId
               << " State: " << state << MTENDL;
    }

    if (btlmt::ChannelPool::e_CHANNEL_UP == state) {
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&mapMutex);
            sourceIdToChannelIdMap[sourceId] = channelId;
        }
    }
}

void blobBasedReadCb(int            *needed,
                     btlb::Blob     *msg,
                     int             channelId,
                     void           *)
{
    if (veryVerbose) {
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }
    *needed = 1;

    msg->removeAll();
}

const int NT = 5;

bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *> clientSockets(NT);
btlso::InetStreamSocketFactory<btlso::IPv4Address>     factory;

struct ConnectData {
    int                d_index;
    btlso::IPv4Address d_serverAddress;
};

void *connectFunction(void *args)
{
    ConnectData              data      = *(const ConnectData *) args;
    const int                INDEX     = data.d_index;
    const btlso::IPv4Address ADDRESS   = data.d_serverAddress;

    clientSockets[INDEX] = factory.allocate();

    ASSERT(0 == clientSockets[INDEX]->connect(ADDRESS));

    return 0;
}

bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *> serverSockets(NT);
bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *> acceptSockets(NT);

struct ListenData {
    int d_index;
};

void *listenFunction(void *args)
{
    ListenData data  = *(const ListenData *) args;
    const int  INDEX = data.d_index;

    serverSockets[INDEX] = factory.allocate();

    ASSERT(0 == serverSockets[INDEX]->bind(getLocalAddress()));
    ASSERT(0 == serverSockets[INDEX]->listen(1));

    serverSockets[INDEX]->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);

    btlso::StreamSocket<btlso::IPv4Address> *client;
    ASSERT(0 == serverSockets[INDEX]->accept(&client));
    ASSERT(0 == client->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE));

    acceptSockets[INDEX] = client;

    return 0;
}

void *dataFunction(void *args)
{
    btlso::StreamSocket<btlso::IPv4Address> *socket =
                              (btlso::StreamSocket<btlso::IPv4Address> *) args;

    const int NUM_BYTES = 1024 * 1024; // 1MB

    bsl::vector<char> buffer(NUM_BYTES);

    int numRemaining = NUM_BYTES;
    do {
        int rc = socket->read(buffer.data(), numRemaining);
        if (rc != btlso::SocketHandle::e_ERROR_WOULDBLOCK) {
            numRemaining -= rc;
        }

        rc = socket->write(buffer.data(), numRemaining);

        bslmt::ThreadUtil::microSleep(1000, 0);
    } while (numRemaining > 0);
    return 0;
}

}  // close namespace TEST_CASE_STOP_AND_REMOVE_CHANNELS


//-----------------------------------------------------------------------------
//                     TEST_CASE_LOWWAT_AFTER_ENQUEUEMARK_EXCEEDED
//-----------------------------------------------------------------------------

namespace TEST_CASE_LOWWAT_AFTER_ENQUEUEMARK_EXCEEDED {

void poolStateCb(int state, int source, int severity)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Pool state callback called with"
                  << " State: " << state
                  << " Source: "  << source
                  << " Severity: " << severity << bsl::endl;
    }
}

void channelStateCb(int             channelId,
                    int             serverId,
                    int             state,
                    void           *,
                    int            *id,
                    int            *numTimesLowWatCalled,
                    bslmt::Barrier *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }
    if (btlmt::ChannelPool::e_CHANNEL_UP == state) {
        *id = channelId;
        barrier->wait();
    }
    if (btlmt::ChannelPool::e_WRITE_CACHE_LOWWAT == state) {
        ++*numTimesLowWatCalled;
        barrier->wait();
    }
}

void blobBasedReadCb(int            *needed,
                     btlb::Blob     *msg,
                     int             channelId,
                     void           *,
                     bslmt::Barrier *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }
    *needed = 1;

    barrier->wait();

    msg->removeAll();
}

struct ReadData {
    btlso::StreamSocket<btlso::IPv4Address> *d_socket_p;
    int                                      d_numBytes;
};

void *readData(void *data)
{
    ReadData& td = *static_cast<ReadData *>(data);

    btlso::StreamSocket<btlso::IPv4Address> *socket    = td.d_socket_p;
    const int                                NUM_BYTES = td.d_numBytes;

    bsl::vector<char> buffer(NUM_BYTES, 0);

    int numRemaining = NUM_BYTES;
    do {
        int rc = socket->read(buffer.data(), numRemaining);
        if (rc != btlso::SocketHandle::e_ERROR_WOULDBLOCK) {
            numRemaining -= rc;
        }
        bslmt::ThreadUtil::microSleep(1000, 0);
    } while (numRemaining > 0);
    return 0;
}

}  // close namespace TEST_CASE_LOWWAT_AFTER_ENQUEUEMARK_EXCEEDED


//-----------------------------------------------------------------------------
//                       TEST_CASE_ADDING_WRITE_STATS
//-----------------------------------------------------------------------------

namespace TEST_CASE_ADDING_WRITE_STATS {

void poolStateCb(int state, int source, int severity)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Pool state callback called with"
                  << " State: " << state
                  << " Source: "  << source
                  << " Severity: " << severity << bsl::endl;
    }
}

void channelStateCb(int             channelId,
                    int             serverId,
                    int             state,
                    void           *,
                    int            *id,
                    bslmt::Barrier *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }
    if (btlmt::ChannelPool::e_CHANNEL_UP == state) {
        *id = channelId;
        barrier->wait();
    }
}

void blobBasedReadCb(int            *needed,
                     btlb::Blob     *msg,
                     int             channelId,
                     void           *,
                     string         *data,
                     bslmt::Barrier *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }
    *needed = 1;

    barrier->wait();

    ostringstream os;
    btlb::BlobUtil::asciiDump(os, *msg);
    data->append(os.str());

    msg->removeAll();
}

struct ReadData {
    btlso::StreamSocket<btlso::IPv4Address> *d_socket_p;
    bslmt::Mutex                            *d_mutex_p;
    int                                     *d_stopReading_p;
};

void *readData(void *data)
{
    ReadData& td = *static_cast<ReadData *>(data);

    btlso::StreamSocket<btlso::IPv4Address> *socket      = td.d_socket_p;
    bslmt::Mutex&                            mutex       = *td.d_mutex_p;
    int&                                     stopReading = *td.d_stopReading_p;

    const int BUF_SIZE = 1000 * 100;
    char buffer[BUF_SIZE];

    bsls::Types::Uint64 br = 0;
    while (1) {
        int rc = socket->read(buffer, BUF_SIZE);
        if (rc != btlso::SocketHandle::e_ERROR_WOULDBLOCK) {
            br += rc;
        }
        bslmt::LockGuard<bslmt::Mutex> lock(&mutex);
        if (stopReading) {
            break;
        }
    }
    return 0;
}

struct WriteData {
    btlmt::ChannelPool                      *d_pool_p;
    int                                      d_channelId;
    btlso::StreamSocket<btlso::IPv4Address> *d_socket_p;
    btlb::Blob                              *d_blob_p;
};

void *writeData(void *data)
{
    WriteData& td = *static_cast<WriteData *>(data);

    btlmt::ChannelPool&  pool      = *td.d_pool_p;
    int                  channelId = td.d_channelId;;
    btlb::Blob          *blob      = td.d_blob_p;

    int curr = 0, max = 0;
    int rc = pool.getChannelWriteCacheStatistics(&max, &curr, channelId);

    if (verbose) cout << "\nFIRST SET"
                      << "\n=========" << endl;

    for (int i = 0; i < 100; ++i) {
        bslmt::ThreadUtil::microSleep(1000);

        rc = pool.write(channelId, *blob);
        LOOP_ASSERT(rc, !rc);

        rc = pool.getChannelWriteCacheStatistics(&max, &curr, channelId);
        LOOP_ASSERT(rc, !rc);
        LOOP_ASSERT(max, max >= 0);
        LOOP_ASSERT(curr, curr >= 0);
//         LOOP2_ASSERT(max, curr, curr <= max);
        if (veryVerbose) {
            MTCOUT << "Max: " << max << " Curr: " << curr << MTENDL;
        }
    }

    if (verbose) cout << "\nSECOND SET"
                      << "\n==========" << endl;

    rc = pool.resetRecordedMaxWriteCacheSize(channelId);
    LOOP_ASSERT(rc, !rc);

    for (int i = 0; i < 100; ++i) {
        bslmt::ThreadUtil::microSleep(1000);

        rc = pool.write(channelId, *blob);
        LOOP_ASSERT(rc, !rc);

        rc = pool.getChannelWriteCacheStatistics(&max, &curr, channelId);
        LOOP_ASSERT(rc, !rc);
        LOOP_ASSERT(max, max >= 0);
        LOOP_ASSERT(curr, curr >= 0);
//         LOOP2_ASSERT(max, curr, curr <= max);
        if (veryVerbose) {
            MTCOUT << "Max: " << max << " Curr: " << curr << MTENDL;
        }
    }

    if (verbose) cout << "\nTHIRD SET"
                      << "\n=========" << endl;

    rc = pool.resetRecordedMaxWriteCacheSize(channelId);
    LOOP_ASSERT(rc, !rc);

    for (int i = 0; i < 100; ++i) {
        bslmt::ThreadUtil::microSleep(1000);

        rc = pool.resetRecordedMaxWriteCacheSize(channelId);
        LOOP_ASSERT(rc, !rc);

        rc = pool.write(channelId, *blob);
        LOOP_ASSERT(rc, !rc);

        rc = pool.getChannelWriteCacheStatistics(&max, &curr, channelId);
        LOOP_ASSERT(rc, !rc);
        LOOP_ASSERT(max, max >= 0);
        LOOP_ASSERT(curr, curr >= 0);
//         LOOP2_ASSERT(max, curr, curr <= max);
        if (veryVerbose) {
            MTCOUT << "Max: " << max << " Curr: " << curr << MTENDL;
        }
    }
    return 0;
}

}  // close namespace TEST_CASE_ADDING_WRITE_STATS

//-----------------------------------------------------------------------------
//                             TEST_CASE_DISABLE_READ
//-----------------------------------------------------------------------------

namespace TEST_CASE_DISABLE_READ {

btlmt::ChannelPool *d_pool_p = 0;
int                 numRead = 0;
bslmt::Mutex        dataMutex;
ostringstream       dataStream;

void poolStateCb(int state, int source, int severity)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Pool state callback called with"
                  << " State: " << state
                  << " Source: "  << source
                  << " Severity: " << severity << bsl::endl;
    }
}

void channelStateCb(int   channelId,
                    int   serverId,
                    int   state,
                    void *,
                    int  *id)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }

    if (btlmt::ChannelPool::e_CHANNEL_UP == state) {
        *id = channelId;
    }
}

void blobBasedReadCb(int            *needed,
                     btlb::Blob     *msg,
                     int             channelId,
                     void           *,
                     int            *numTimesCbCalled,
                     bslmt::Barrier *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }

    *numTimesCbCalled = *numTimesCbCalled + 1;

    if (*numTimesCbCalled <= 2) {
        d_pool_p->disableRead(channelId);
    }

    if (*numTimesCbCalled <= 3) {
        barrier->wait();
    }

    *needed = 1;

    btlb::BlobUtil::asciiDump(dataStream, *msg);
    msg->removeAll();
}

void populateText(char *text, int length)
{
    const char *data = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int   dataLength = strlen(data);

          char *curr = text;
    const char *end  = text + length;

    while (end - curr > dataLength) {
        bsl::memcpy(curr, data, dataLength);
        curr += dataLength;
    }
    memset(curr, 'A', end - curr);
}

}  // close namespace TEST_CASE_DISABLE_READ

//-----------------------------------------------------------------------------
//                                  TEST_CASE_DISABLE_READ_WHEN_NUM_NEEDED_ZERO
//-----------------------------------------------------------------------------

namespace TEST_CASE_DISABLE_READ_WHEN_NUM_NEEDED_ZERO {

void poolStateCb(int state, int source, int severity)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Pool state callback called with"
                  << " State: " << state
                  << " Source: "  << source
                  << " Severity: " << severity << bsl::endl;
    }
}

void channelStateCb(int              channelId,
                    int              serverId,
                    int              state,
                    void            *,
                    int             *id,
                    bslmt::Barrier  *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }
    if (btlmt::ChannelPool::e_CHANNEL_UP         == state
     || btlmt::ChannelPool::e_AUTO_READ_DISABLED == state) {
        *id = channelId;
        barrier->wait();
    }
}

void blobBasedReadCb(int             *needed,
                     btlb::Blob      *msg,
                     int              channelId,
                     void            *,
                     int             *numTimesCbCalled,
                     string          *data,
                     bslmt::Barrier  *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }
    *needed = 1;

    ostringstream os;
    btlb::BlobUtil::asciiDump(os, *msg);
    data->append(os.str());

    msg->removeAll();
    ++*numTimesCbCalled;
    barrier->wait();
}

}  // close namespace TEST_CASE_DISABLE_READ_WHEN_NUM_NEEDED_ZERO

//-----------------------------------------------------------------------------
//                                  TEST_CASE_TESTING_SOCKET_OPTIONS
//-----------------------------------------------------------------------------

namespace TEST_CASE_TESTING_SOCKET_OPTIONS {

class SocketPool {

    btlso::InetStreamSocketFactory<btlso::IPv4Address> d_factory;

public:

    int connectWithOptions(Obj                 *pool,
                           btlso::IPv4Address   serverAddr,
                           int                  sourceId,
                           const SocketOptions *opt,
                           bool                 setManually)
    {
        int rc;
        if (setManually && opt) {
            typedef btlso::StreamSocketFactoryDeleter Deleter;

            bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                            socket(d_factory.allocate(),
                                   &d_factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

            if (0 != btlso::SocketOptUtil::setSocketOptions(socket->handle(),
                                                            *opt)) {
                return -1;                                            // RETURN
            }
            rc = pool->connect(serverAddr, 1, bsls::TimeInterval(1),
                               sourceId, &socket);

        }
        else {
            rc = pool->connect(serverAddr, 1, bsls::TimeInterval(1),
                               sourceId, true, Obj::e_CLOSE_BOTH, opt);
        }
        ASSERT(0 == rc);
        return 0;
    }

    int connectWithOptions(Obj                *pool,
                           const char         *host,
                           int                 port,
                           int                 sourceId,
                           const               SocketOptions *opt,
                           bool                setManually)
    {
        int rc;
        if (setManually && opt) {
            typedef btlso::StreamSocketFactoryDeleter Deleter;

            bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                socket(d_factory.allocate(),
                       &d_factory,
                       &Deleter::deleteObject<btlso::IPv4Address>);

            if (0 != btlso::SocketOptUtil::setSocketOptions(socket->handle(),
                                                            *opt)) {
                return -1;                                            // RETURN
            }
            rc = pool->connect(host, port, 1, bsls::TimeInterval(1),
                               sourceId, &socket);

        }
        else {
            rc = pool->connect(host, port, 1, bsls::TimeInterval(1),
                               sourceId, Obj::e_RESOLVE_ONCE, true,
                               Obj::e_CLOSE_BOTH, opt);
        }
        ASSERT(0 == rc);
        return 0;
    }
};

void poolStateCb(int             state,
                 int             source,
                 int             severity,
                 bslmt::Barrier *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Pool state callback called with"
                  << " State: " << state
                  << " Source: "  << source
                  << " Severity: " << severity << bsl::endl;
    }
    barrier->wait();
}

void channelStateCb(int              channelId,
                    int              serverId,
                    int              state,
                    void            *,
                    int             *id,
                    bslmt::Barrier  *barrier)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }
    if (btlmt::ChannelPool::e_CHANNEL_UP == state) {
        *id = channelId;
        barrier->wait();
    }
}

void blobBasedReadCb(int        *needed,
                     btlb::Blob *msg,
                     int         channelId,
                     void       *)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(&coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }
    *needed = 1;
    msg->removeAll();
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure a 'btlso::SocketOptions' according to a custom language.
// A tilde ('~') indicates that the logical (but not necessarily
// physical) state of the object is to be set to its initial, empty state (via
// the 'removeAll' method).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ELEMENT> | <ELEMENT><LIST>
//
// <ELEMENT>    ::= <SEND_BUF_SIZE> | <RECV_BUF_SIZE> | <MIN_SEND_BUF_SIZE>
//                  <MIN_RECV_BUF_SIZE> | <SEND_TIMEOUT> | <RECV_TIMEOUT>
//                  <DEBUG> | <ALLOW_BROADCASTING> | <REUSE_ADDR> |
//                  <KEEP_ALIVE> | <BYPASS_ROUTING> |
//                  <LEAVE_OUT_OF_BAND_DATA_INLINE> | LINGER
//
// <ELEMENT>    ::= <SPEC><VALUE> | <SPEC><BOOLEAN> | <SPEC><LINGER>
//
// <SPEC>       ::= 'A'              // <SEND_BUF_SIZE>
//                | 'B'              // <RECV_BUF_SIZE>
//                | 'C'              // <MIN_SEND_BUF_SIZE>
//                | 'D'              // <MIN_RECV_BUF_SIZE>
//                | 'E'              // <SEND_TIMEOUT>
//                | 'F'              // <RECV_TIMEOUT>
//                | 'G'              // <DEBUG>
//                | 'H'              // <ALLOW_BROADCASTING>
//                | 'I'              // <REUSE_ADDR>
//                | 'J'              // <KEEP_ALIVE>
//                | 'K'              // <BYPASS_ROUTING>
//                | 'L'              // <LEAVE_OUT_OF_BAND_INLINE>
//                | 'M'              // <LINGER>
//
// <VALUE>      ::= '0' | '1' | '2'
// <BOOLEAN>    ::= 'Y' | 'N'
// <LINGER>     ::= <BOOLEAN><VALUE>
//
//-----------------------------------------------------------------------------

int TIMEOUTS[] = { 0, 1, 2 };
int DATA[] = { 16, 64, 128 };
int SIZES[] = { 1024, 2048, 4096 };

int setOption(SocketOptions *options, const char *specString)
    // Set the spec.  Return the number of characters read.
{
    const char spec  = *specString;
    const char value = *(specString + 1);
    int        numRead = 2;
    switch (spec) {
      case 'A': {  // SEND_BUF_SIZE
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setSendBufferSize(SIZES[value - '0']);
      } break;
      case 'B': {  // RECV_BUF_SIZE
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setReceiveBufferSize(SIZES[value - '0']);
      } break;
      case 'C': {  // MIN_SEND_BUF_SIZE
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setMinimumSendBufferSize(SIZES[value - '0']);
      } break;
      case 'D': {  // MIN_RECV_BUF_SIZE
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setMinimumReceiveBufferSize(SIZES[value - '0']);
      } break;
      case 'E': {  // SEND_TIMEOUT
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setSendTimeout(TIMEOUTS[value - '0']);
      } break;
      case 'F': {  // RECEIVE_TIMEOUT
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setReceiveTimeout(TIMEOUTS[value - '0']);
      } break;
      case 'G': {  // DEBUG
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setDebugFlag('Y' == value ? true : false);
      } break;
      case 'H': {  // ALLOW BROADCASTING
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setAllowBroadcasting('Y' == value ? true : false);
      } break;
      case 'I': {  // REUSE ADDR
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setReuseAddress('Y' == value ? true : false);
      } break;
      case 'J': {  // KEEP ALIVE
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setKeepAlive('Y' == value ? true : false);
      } break;
      case 'K': {  // BYPASS ROUTING
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setBypassNormalRouting('Y' == value ? true : false);
      } break;
      case 'L': {  // LEAVE_OUT_OF_BAND_DATA_INLINE
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setLeaveOutOfBandDataInline('Y' == value ? true : false);
      } break;
      case 'M': {  // LINGER
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        LingerOptions linger;
        linger.setLingerFlag('Y' == value ? true : false);
        const char nextValue = *(specString + 2);
        LOOP_ASSERT(nextValue, '0' == nextValue
                    || '1' == nextValue || '2' == nextValue);
        linger.setTimeout(DATA[nextValue - '0']);
        options->setLinger(linger);
        ++numRead;
      } break;
      default: {
        LOOP2_ASSERT(spec, value, 0);
        return 0;                                                     // RETURN
      } break;
    }
    return numRead;
}

int ggg(SocketOptions *object, const char *spec)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator functions 'setXXX'.  Return the index
    // of the first invalid character, and a negative value otherwise.  Note
    // that this function is used to implement 'gg' as well as allow for
    // verification of syntax error detection.
{
    enum { SUCCESS = -1 };
    while (*spec) {
        const int numRead = setOption(object, spec);
        if (!numRead) {
            return numRead;                                           // RETURN
        }
        spec += numRead;
   }
   return SUCCESS;
}

SocketOptions& gg(SocketOptions *object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

SocketOptions g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    SocketOptions object;
    return gg(&object, spec);
}

int verify(const Obj&                  pool,
           int                         channelId,
           const btlso::SocketOptions& options)
    // Verify that the socket options on the specified 'handle' match the
    // specified 'options'.  Return 0 on success and a non-zero value
    // otherwise.
{
    if (!options.debugFlag().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                       &result,
                                       btlso::SocketOptUtil::k_DEBUGINFO,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.debugFlag().value(),
                     (bool) result == options.debugFlag().value());
    }

    if (!options.allowBroadcasting().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                       &result,
                                       btlso::SocketOptUtil::k_BROADCAST,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.allowBroadcasting().value(),
                     (bool) result == options.allowBroadcasting().value());
    }

    if (!options.reuseAddress().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                      &result,
                                      btlso::SocketOptUtil::k_REUSEADDRESS,
                                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                                      channelId);
        if (rc) {
            return rc;                                                // RETURN
        }

#ifndef BSLS_PLATFORM_OS_CYGWIN
        LOOP2_ASSERT((bool) result, options.reuseAddress().value(),
                     (bool) result == options.reuseAddress().value());
#endif
    }

    if (!options.keepAlive().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                       &result,
                                       btlso::SocketOptUtil::k_KEEPALIVE,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.keepAlive().value(),
                     (bool) result == options.keepAlive().value());
    }

    if (!options.bypassNormalRouting().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                       &result,
                                       btlso::SocketOptUtil::k_DONTROUTE,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.bypassNormalRouting().value(),
                     (bool) result == options.bypassNormalRouting().value());
    }

    if (!options.linger().isNull()) {
        btlso::SocketOptUtil::LingerData lingerData;
        const int rc = pool.getLingerOption(&lingerData, channelId);
        if (rc) {
            return rc;                                                // RETURN
        }

        LOOP2_ASSERT((bool) lingerData.l_onoff,
                     options.linger().value().lingerFlag(),
         (bool) lingerData.l_onoff == options.linger().value().lingerFlag());
        LOOP2_ASSERT(lingerData.l_linger, options.linger().value().timeout(),
                    lingerData.l_linger == options.linger().value().timeout());
    }

    if (!options.leaveOutOfBandDataInline().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                       &result,
                                       btlso::SocketOptUtil::k_OOBINLINE,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.leaveOutOfBandDataInline().value(),
                  (bool) result == options.leaveOutOfBandDataInline().value());
    }

    if (!options.sendBufferSize().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                       &result,
                                       btlso::SocketOptUtil::k_SENDBUFFER,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.sendBufferSize().value(),
                     result >= options.sendBufferSize().value());
    }

    if (!options.receiveBufferSize().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                     &result,
                                     btlso::SocketOptUtil::k_RECEIVEBUFFER,
                                     btlso::SocketOptUtil::k_SOCKETLEVEL,
                                     channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.receiveBufferSize().value(),
                     result >= options.receiveBufferSize().value());
    }

    if (!options.minimumSendBufferSize().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                       &result,
                                       btlso::SocketOptUtil::k_SENDLOWATER,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.minimumSendBufferSize().value(),
                     result >= options.minimumSendBufferSize().value());
    }

    if (!options.minimumReceiveBufferSize().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                    &result,
                                    btlso::SocketOptUtil::k_RECEIVELOWATER,
                                    btlso::SocketOptUtil::k_SOCKETLEVEL,
                                    channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.minimumReceiveBufferSize().value(),
                     result >= options.minimumReceiveBufferSize().value());
    }

    if (!options.sendTimeout().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                       &result,
                                       btlso::SocketOptUtil::k_SENDTIMEOUT,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.sendTimeout().value(),
                     result == options.sendTimeout().value());
    }

    if (!options.receiveTimeout().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                    &result,
                                    btlso::SocketOptUtil::k_RECEIVETIMEOUT,
                                    btlso::SocketOptUtil::k_SOCKETLEVEL,
                                    channelId);

        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.receiveTimeout().value(),
                     result == options.receiveTimeout().value());
    }

    return 0;
}

}  // close namespace TEST_CASE_TESTING_SOCKET_OPTIONS

//-----------------------------------------------------------------------------
//                          TEST_CASE_PARTIAL_READS
//-----------------------------------------------------------------------------
namespace TEST_CASE_PARTIAL_READS {

class ReadServer
{
    // A read server.

    enum { SERVER_ID = 1066 };

    int                 d_port;         // well-known port for service requests
    string              d_data;
    int                 d_expDataSize;
    bslma::Allocator   *d_allocator_p;
    btlmt::ChannelPool *d_cp_p;
    bslmt::Mutex       *d_coutMutex;

  private:
    // ChannelPool Callback Functions
    void poolCB(int state, int source, int severity);

    void chanCB(int channelId, int serverId, int state, void *arg);

    void blobBasedReadCb(int        *numNeeded,
                         btlb::Blob *msg,
                         int         channelId,
                         void       *context);
        // Blob based read callback.

    // Not Implemented
    ReadServer(const ReadServer&);
    ReadServer& operator=(const ReadServer&);

  public:
    // CREATORS
    ReadServer(bslmt::Mutex                           *coutMutex,
               int                                     expDataSize,
               const btlmt::ChannelPoolConfiguration&  channelPoolConfig,
               bslma::Allocator                       *allocator = 0);
        // Create a server object which accepts connections
        // on localhost at the specified by 'port'.  The server
        // uses the specified 'allocator' for internal memory management.

   ~ReadServer();
        // Terminate all open connections, and destroy the server.

    // MANIPULATORS
    int start();
        // Start the server.  Return 0 on success, and a non-zero value if
        // an error occurred.  The server is started in listen-mode with a
        // backlog equal to 'd_maxClients'.

    int stop();
        // Stop the server.  Immediately terminate all connections, but
        // leave server in listen-mode.  Returns zero if successful,
        // and non-zero if an error occurs.

    string& data();
        // Return the data stored by this server.

    // ACCESSORS
    int portNumber() const;
        // Return the port number on the local host on which this server
        // listens to connections.

    const string& data() const;
        // Return the data stored by this server.
};

// CREATORS
ReadServer::ReadServer(
        bslmt::Mutex                           *coutMutex,
        int                                     expDataSize,
        const btlmt::ChannelPoolConfiguration&  channelPoolConfig,
        bslma::Allocator                       *allocator)
: d_port(0)
, d_data(allocator)
, d_expDataSize(expDataSize)
, d_allocator_p(bslma::Default::allocator(allocator))
, d_coutMutex(coutMutex)
{
    btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                   bsl::allocator_arg_t(),
                   bsl::allocator<btlmt::ChannelPool::PoolStateChangeCallback>(
                                                                d_allocator_p),
                   bdlf::MemFnUtil::memFn(&ReadServer::poolCB, this));

    btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                bsl::allocator_arg_t(),
                bsl::allocator<btlmt::ChannelPool::ChannelStateChangeCallback>(
                                                                d_allocator_p),
                bdlf::MemFnUtil::memFn(&ReadServer::chanCB, this));

    btlmt::ChannelPool::BlobBasedReadCallback dataFunctor =
                    bdlf::MemFnUtil::memFn(&ReadServer::blobBasedReadCb, this);

    d_cp_p = new (*d_allocator_p) btlmt::ChannelPool(channelCb,
                                                     dataFunctor,
                                                     poolCb,
                                                     channelPoolConfig,
                                                     allocator);
}

ReadServer::~ReadServer()
{
    d_cp_p->stop();
    d_allocator_p->deleteObjectRaw(d_cp_p);
}

int ReadServer::start()
{
    int    src = d_cp_p->start();
    int    lrc = d_cp_p->listen(d_port, 5, SERVER_ID);
    btlso::IPv4Address address = getServerLocalAddress(d_cp_p, SERVER_ID);
    d_port = address.portNumber();
    return src || lrc;
}

int ReadServer::stop()
{
    return d_cp_p->stop();
}

void ReadServer::poolCB(int, int, int)
{
}

void ReadServer::chanCB(int channelId, int serverId, int state, void *)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(d_coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        d_cp_p->shutdown(channelId, btlmt::ChannelPool::e_IMMEDIATE);
      } break;
      case btlmt::ChannelPool::e_CHANNEL_UP: {
      } break;
    }
}

void ReadServer::blobBasedReadCb(int        *needed,
                                 btlb::Blob *msg,
                                 int         channelId,
                                 void       *)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(d_coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }

    ASSERT(needed);
    ASSERT(msg);

    const int numBuffers = msg->numDataBuffers();
    const int length     = msg->length();

    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(d_coutMutex);
        btlb::BlobUtil::hexDump(bsl::cout, *msg, 0, length);
    }

    if (d_expDataSize != length) {
        *needed = 1;
        return;                                                       // RETURN
    }

    for (int i = 0; i < numBuffers - 1; ++i) {
        const btlb::BlobBuffer& buffer = msg->buffer(i);
        d_data.append(buffer.data(), buffer.size());
    }

    const btlb::BlobBuffer& buffer = msg->buffer(numBuffers - 1);
    d_data.append(buffer.data(), msg->lastDataBufferLength());

    btlb::BlobUtil::erase(msg, 0, length);

    *needed = 1;
}

// MANIPULATORS
bsl::string& ReadServer::data()
{
    return d_data;
}

// ACCESSORS
int ReadServer::portNumber() const
{
    btlso::IPv4Address address = getServerLocalAddress(d_cp_p, SERVER_ID);
    return address.portNumber();
}

const bsl::string& ReadServer::data() const
{
    return d_data;
}

}  // close namespace TEST_CASE_PARTIAL_READS

//-----------------------------------------------------------------------------
//                          TEST_CASE_READ
//-----------------------------------------------------------------------------

namespace TEST_CASE_READ {

                        // ================
                        // class DataReader
                        // ================

class DataReader {
    // This class implements a data reader whose callback functions are
    // invoked to read the data from a btlmt::ChannelPoolChannel object.

    int                  d_msgId;           // message id
    int                  d_msgLength;       // message length
    string               d_data;            // actual data
    mutable bslmt::Mutex d_mutex;           // mutex for data

  public:
    // CREATORS
    DataReader();
        // Construct a default reader.

    ~DataReader();
        // Destroy this reader.

    // MANIPULATORS
    void blobBasedReadCb(int        *numNeeded,
                         btlb::Blob *msg,
                         int         channelId,
                         void       *context);
        // Blob based read callback.

    // ACCESSORS
    const string& data() const;
        // Return the data stored in this reader.

    int msgId() const;
        // Return the message id of the data in this reader.

    int msgLength() const;
        // Return the message length of data expected by this reader.

    bool done() const;
        // Return 'true' if this reader has read the expected number of
        // data bytes, and 'false' otherwise.
};

class ReadServer
{
    // A read server.

    enum { SERVER_ID = 1066 };

    typedef bsl::map<int, int>           ChannelMap; // maps channel id to msg
                                                     // id
    typedef bsl::map<int, DataReader *>  DataMap;    // maps msg id to data

    int        d_port;           // well-known port for service requests

    bslma::Allocator        *d_allocator_p;
    btlmt::ChannelPool      *d_cp_p;
    ChannelMap               d_channelMap;
    mutable bslmt::Mutex     d_channelMapMutex;
    DataMap                  d_dataMap;
    mutable bslmt::Mutex     d_dataMapMutex;
    mutable bslmt::Mutex     d_generalMutex;
    bslmt::Mutex            *d_coutMutex;

  private:
    // ChannelPool Callback Functions
    void poolCB(int state, int source, int severity);

    void chanCB(int channelId, int serverId, int state, void *arg);

    void blobBasedReadCb(int        *numNeeded,
                         btlb::Blob *msg,
                         int         channelId,
                         void       *context);
        // Blob based read callback.

    // Not Implemented
    ReadServer(const ReadServer&);
    ReadServer& operator=(const ReadServer&);

  public:
    // CREATORS
    ReadServer(bslmt::Mutex      *coutMutex,
               int                port,
               bslma::Allocator  *allocator = 0);
        // Create a server object which accepts connections on localhost at the
        // specified by 'port'.  The server uses the specified 'allocator' for
        // internal memory management.

   ~ReadServer();
        // Terminate all open connections, and destroy the server.

    // MANIPULATORS
    int start();
        // Start the server.  Return 0 on success, and a non-zero value if
        // an error occurred.  The server is started in listen-mode with a
        // backlog equal to 'd_maxClients'.

    int stop();
        // Stop the server.  Immediately terminate all connections, but
        // leave server in listen-mode.  Returns zero if successful,
        // and non-zero if an error occurs.

    // ACCESSORS
    int portNumber() const;
        // Return the port number on the local host on which this server
        // listens to connections.

    int numCompletedMsgs() const;
        // Return the number of completed messages.

    DataReader *dataReader(int msgIdx);

    DataReader *reader(int channelId, bool createNew = false);
        // Return the data reader corresponding to the specified 'channelId'
        // or 0 on error.  If the specified 'createNew' is 'true' then create
        // a new data reader if one does not exist for 'channelId'.
};

ReadServer::ReadServer(
        bslmt::Mutex      *coutMutex,
        int                port,
        bslma::Allocator  *allocator)
: d_port(port)
, d_allocator_p(bslma::Default::allocator(allocator))
, d_coutMutex(coutMutex)
{
    btlmt::ChannelPoolConfiguration cpc;
    cpc.setMaxConnections(20);
    cpc.setMaxThreads(20);
    cpc.setMetricsInterval(10.0);
    cpc.setIncomingMessageSizes(1, 5, 10);

    btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                   bsl::allocator_arg_t(),
                   bsl::allocator<btlmt::ChannelPool::PoolStateChangeCallback>(
                                                                d_allocator_p),
                   bdlf::MemFnUtil::memFn(&ReadServer::poolCB, this));

    btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                bsl::allocator_arg_t(),
                bsl::allocator<btlmt::ChannelPool::ChannelStateChangeCallback>(
                                                                d_allocator_p),
                bdlf::MemFnUtil::memFn(&ReadServer::chanCB, this));

    btlmt::ChannelPool::BlobBasedReadCallback dataFunctor =
                           bdlf::MemFnUtil::memFn(&ReadServer::blobBasedReadCb,
                                                  this);

    d_cp_p = new (*d_allocator_p) btlmt::ChannelPool(channelCb,
                                                     dataFunctor,
                                                     poolCb,
                                                     cpc,
                                                     allocator);
}

ReadServer::~ReadServer()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_dataMapMutex);
    DataMap::const_iterator citer = d_dataMap.begin();
    for (;citer != d_dataMap.end(); ++citer) {
        const DataReader *reader = citer->second;
        d_allocator_p->deleteObjectRaw(reader);
    }

    d_cp_p->stop();
    d_allocator_p->deleteObjectRaw(d_cp_p);
}

int ReadServer::start()
{
    int    src = d_cp_p->start();
    int    lrc = d_cp_p->listen(d_port, 5, SERVER_ID);
    btlso::IPv4Address address = getServerLocalAddress(d_cp_p, SERVER_ID);
    d_port = address.portNumber();
    return src || lrc;
}

int ReadServer::stop()
{
    return d_cp_p->stop();
}

void ReadServer::poolCB(int, int, int)
{
}

void ReadServer::chanCB(int channelId, int serverId, int state, void *)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(d_coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        d_cp_p->shutdown(channelId, btlmt::ChannelPool::e_IMMEDIATE);
      } break;
      case btlmt::ChannelPool::e_CHANNEL_UP: {
        DataReader *r = reader(channelId, true);
        ASSERT(r);
      } break;
    }
}

void ReadServer::blobBasedReadCb(int        *needed,
                                 btlb::Blob *msg,
                                 int         channelId,
                                 void       *arg)
{
    if (veryVerbose) {
        bslmt::LockGuard<bslmt::Mutex> guard(d_coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }

    ASSERT(needed);
    ASSERT(msg);

    DataReader *r = reader(channelId);
    ASSERT(r);
    r->blobBasedReadCb(needed, msg, channelId, arg);
}

// MANIPULATORS
DataReader *ReadServer::reader(int channelId, bool createNew)
{
    int                            msgId;
    bslmt::LockGuard<bslmt::Mutex> guard1(&d_channelMapMutex);
    ChannelMap::const_iterator     citer = d_channelMap.find(channelId);
    if (citer == d_channelMap.end()) {
        if (!createNew) {
            bslmt::LockGuard<bslmt::Mutex> guard(d_coutMutex);
            bsl::cout << "Channel up callback called with"
                      << " Channel Id: " << channelId
                      << " which does not exist" << bsl::endl;
            return 0;                                                 // RETURN
        }
        else {
            msgId = d_channelMap.size();
            d_channelMap[channelId] = msgId;
        }
    }
    else {
        if (createNew) {
            bslmt::LockGuard<bslmt::Mutex> guard(d_coutMutex);
            bsl::cout << "Channel up callback called with"
                      << " Channel Id: " << channelId
                      << " which is already up" << bsl::endl;
            return 0;                                                 // RETURN
        }
        else {
            msgId = citer->second;
        }
    }
    guard1.release()->unlock();

    DataReader *reader = 0;
    bslmt::LockGuard<bslmt::Mutex> guard2(&d_dataMapMutex);
    DataMap::iterator iter = d_dataMap.find(msgId);
    if (iter == d_dataMap.end()) {
        if (!createNew) {
            bslmt::LockGuard<bslmt::Mutex> guard(d_coutMutex);
            bsl::cout << "Channel up callback called with"
                      << " Channel Id: " << channelId
                      << " which does not exist" << bsl::endl;
            return 0;                                                 // RETURN
        }
        else {
            reader = new (*d_allocator_p) DataReader();
            d_dataMap[msgId] = reader;
        }
    }
    else {
        if (createNew) {
            bslmt::LockGuard<bslmt::Mutex> guard(d_coutMutex);
            bsl::cout << "Channel up callback called with"
                      << " Channel Id: " << channelId
                      << " which is already up" << bsl::endl;
            return 0;                                                 // RETURN
        }
        else {
            reader = iter->second;
        }
    }
    return reader;
}

// ACCESSORS
int ReadServer::numCompletedMsgs() const
{
    int numCompletedMsgs = 0;
    bslmt::LockGuard<bslmt::Mutex> guard(&d_dataMapMutex);
    DataMap::const_iterator citer = d_dataMap.begin();
    for (;citer != d_dataMap.end(); ++citer) {
        const DataReader *reader = citer->second;
        if (reader->msgLength() == (int) reader->data().size()) {
            ++numCompletedMsgs;
        }
    }
    return numCompletedMsgs;
}

int ReadServer::portNumber() const
{
    btlso::IPv4Address address = getServerLocalAddress(d_cp_p, SERVER_ID);
    return address.portNumber();
}

DataReader *ReadServer::dataReader(int msgIdx)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_dataMapMutex);
    DataMap::const_iterator citer = d_dataMap.find(msgIdx);
    return citer != d_dataMap.end() ? citer->second : 0;
}

                            // ----------------
                            // class DataReader
                            // ----------------

// CREATORS
DataReader::DataReader()
: d_msgId(-1)
, d_msgLength(-1)
{
}

DataReader::~DataReader()
{
}

// MANIPULATORS
void DataReader::blobBasedReadCb(int        *numNeeded,
                                 btlb::Blob *msg,
                                 int         channelId,
                                 void       *)
{
    if (veryVerbose) {
        MTCOUT << "Blob based callback called with "
               << "for Channel Id: " << channelId
               << " with data of length: " << msg->length()
               << MTENDL;
    }

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    if (-1 == d_msgId) {
        if (msg->length() < (int) sizeof(int)) {
            *numNeeded = sizeof(int);
            return;                                                   // RETURN
        }

        bslx::MarshallingUtil::getInt32(&d_msgId, msg->buffer(0).data());
        ASSERT(0 <= d_msgId);
        btlb::BlobUtil::erase(msg, 0, sizeof(int));

        if (0 == msg->length()) {
            *numNeeded = sizeof(int);
            return;                                                   // RETURN
        }
    }

    if (-1 == d_msgLength) {
        if (msg->length() < (int) sizeof(int)) {
            *numNeeded = sizeof(int);
            return;                                                   // RETURN
        }

        bslx::MarshallingUtil::getInt32(&d_msgLength,
                                        msg->buffer(0).data());
        ASSERT(0 <= d_msgLength);
        btlb::BlobUtil::erase(msg, 0, sizeof(int));

        if (0 == msg->length()) {
            *numNeeded = d_msgLength;
            return;                                                   // RETURN
        }
    }

    string msgData;

    const int numDataBufs = msg->numDataBuffers();
    for (int i = 0; i < numDataBufs - 1; ++i) {
        msgData.append(msg->buffer(i).data(), msg->buffer(i).size());
    }
    msgData.append(msg->buffer(numDataBufs - 1).data(),
                   msg->lastDataBufferLength());

    btlb::BlobUtil::erase(msg, 0, msg->length());
    d_data.append(msgData);
    *numNeeded = 1;
}

// ACCESSORS
const string& DataReader::data() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_data;
}

int DataReader::msgId() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_msgId;
}

int DataReader::msgLength() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_msgLength;
}

bool DataReader::done() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return (int) d_data.size() == d_msgLength;
}

                    // ==============
                    // class TestData
                    // ==============

struct TestData {
    // This 'struct' represents the data passed the testing thread.

    int                                      d_threadIntId; // integer thread
                                                            // id

    string                                   d_expData;     // expected data

    btlso::IPv4Address                       d_address;     // ip address

    btlso::StreamSocket<btlso::IPv4Address> *d_socket_p;    // socket to write
                                                            // to

    bslmt::Barrier                          *d_barrier_p;   // barrier

    void run();
        // Run the test function.
};

extern "C" void* threadFunction(void *testData)
{
    ((TestData *) testData)->run();
    return 0;
}

                        // --------------
                        // class TestData
                        // --------------

void TestData::run()
{
    ASSERT(0 == d_socket_p->connect(d_address));

    d_barrier_p->wait();

    const int MAX_DATA_SIZE = 16384;
    const int DATA_SIZE     = d_expData.size();

    char data[MAX_DATA_SIZE];
    bsl::memset(data, 0, MAX_DATA_SIZE);
    LOOP2_ASSERT(DATA_SIZE, MAX_DATA_SIZE, DATA_SIZE <= MAX_DATA_SIZE);

    int offset = 0;
    bslx::MarshallingUtil::putInt32(data, d_threadIntId);
    offset += sizeof(int);

    bslx::MarshallingUtil::putInt32(data + offset, DATA_SIZE);
    offset += sizeof(int);

    const int TOTAL_SIZE = DATA_SIZE + offset;

    bsl::memcpy(data + offset, d_expData.data(), DATA_SIZE);

    int incr = 1, written = 0;
    offset = 0;
    while (written < TOTAL_SIZE) {
        LOOP2_ASSERT(d_threadIntId, incr,
                     incr == d_socket_p->write(data + offset, incr));
        written += incr;
        offset  += incr;
        if (written + incr + 1 > TOTAL_SIZE) {
            incr = TOTAL_SIZE - written;
        }
        else {
            ++incr;
        }
        bslmt::ThreadUtil::microSleep(100);
    }
}

}  // close namespace TEST_CASE_READ

//-----------------------------------------------------------------------------
// 'btlmt::ChannelPool::busyMetrics()'
//-----------------------------------------------------------------------------

namespace TEST_CASE_BUSY_METRICS {

void dummyDataCallbackWithDelay(int        *numNeeded,
                                btlb::Blob *message,
                                int           ,
                                void       *,
                                double      delayS)
{
    bslmt::ThreadUtil::sleep(bsls::TimeInterval(delayS));
    *numNeeded   = 1;
    btlb::BlobUtil::erase(message, 0, message->length());
}

}  // close namespace TEST_CASE_BUSY_METRICS
//-----------------------------------------------------------------------------
// 'btlmt::ChannelPool_MessageUtil'
//-----------------------------------------------------------------------------
namespace TEST_CASE_MESSAGEHELPER_NAMESPACE {

int createData(bsl::vector<bsl::vector<char> > *data,
               const  char                     *description)
    // Populate the specified 'data' with a series of characters whose lengths
    // are described by 'description', each array containing arbitrary data;
    // return the total number of bytes of data in 'data'.  The 'description'
    // must be a null-terminated string with a space separated list of lengths
    // (e.g., "1 2 3 4").  The arbitrary data will begin with the byte 0xFF and
    // decrease by 1 per element.
{
    int length = 0;
    bsl::vector<int> lengths;
    for (bdlb::Tokenizer it(bslstl::StringRef(description),
                            bslstl::StringRef(" "));
         it.isValid();
         ++it) {
        lengths.push_back(atoi(it.token().data()));
        length += lengths.back();
    }
    data->resize(lengths.size());

    char c = -1; // 0xFF

    bsl::vector<bsl::vector<char> >::iterator destIt = data->begin();
    bsl::vector<int>::const_iterator          lenIt  = lengths.begin();
    for (; lenIt != lengths.end(); ++lenIt, ++destIt) {
        destIt->resize(*lenIt);
        for (int i = 0; i < *lenIt; ++i) {
            (*destIt)[i] = c--;
        }
    }
    return length;
}

template <class IOVEC>
void loadIovecs(bsl::vector<IOVEC>               *iovecs,
                bsl::vector<bsl::vector<char> >&  data)
    // Load into the specified 'iovecs' the specified 'data' buffers.
{
    iovecs->resize(data.size());
    typename bsl::vector<IOVEC>::iterator     destIt = iovecs->begin();
    bsl::vector<bsl::vector<char> >::iterator srcIt  = data.begin();
    for (; srcIt != data.end(); ++srcIt, ++destIt) {
        destIt->setBuffer(&srcIt->front(), srcIt->size());
    }
}

void loadBlob(btlb::Blob                       *blob,
              bsl::vector<bsl::vector<char> >&  data)
    // Load into the specified 'blob' the specified 'data' buffers.
{
    bsl::vector<bsl::vector<char> >::iterator srcIt  = data.begin();
    for (; srcIt != data.end(); ++srcIt) {
        bsl::shared_ptr<char> sharedData(&srcIt->front(),
                                         bslstl::SharedPtrNilDeleter(),
                                         0);
        btlb::BlobBuffer buffer(sharedData, srcIt->size());
        blob->appendDataBuffer(buffer);
    }
}

bool isEqualByteSequence(
                const bsl::vector<bsl::vector<char> >& expectedSequence,
                int                                    expectedSequenceOffset,
                const btlb::Blob&                      blob,
                int                                    blobStartOffset,
                int                                    maximumLength = 0)

    // Return 'true' if each byte in the specified 'blob', starting at the
    // specified 'blobOffset', is equal to the corresponding byte in the
    // specified 'expectedSequence', starting at 'expectedSequenceOffset'.
    // Optionally specify a 'maximumLength' of the sequence to test, if
    // 'maximumLength' is 0, test the entire buffer (from 'blobStartOffset' to
    // 'blob.length()'.
{
    int blobIndex   = 0;
    int blobOffset  = 0;
    int arrayIndex  = 0;
    int arrayOffset = 0;

    int maximumOffset = blob.length();
    if (maximumLength != 0) {
        maximumOffset = bsl::min(blob.length(),
                                 blobStartOffset + maximumLength);
    }

    // Find the 'expectedSequenceOffset' in 'expectedSequence'.

    int length = 0;
    for (int i = 0; i < (int) expectedSequence.size(); ++i) {
        if (length + (int) expectedSequence[i].size()>expectedSequenceOffset) {
            arrayIndex  = i;
            arrayOffset = expectedSequenceOffset - length;
            break;
        }
        length += expectedSequence[i].size();
    }

    int blobPosition = 0;
    for (int i = 0; i < blob.numBuffers(); ++i) {
        if (blobPosition + blob.buffer(i).size() > blobStartOffset) {
            blobOffset   = blobStartOffset - blobPosition;
            blobPosition = blobStartOffset;
            blobIndex    = i;
            break;
        }
        blobPosition += blob.buffer(i).size();
    }

    while (blobPosition < maximumOffset &&
           arrayIndex   < (int) expectedSequence.size()) {
        const btlb::BlobBuffer&   buffer     = blob.buffer(blobIndex);
        const char               *bufferData = buffer.buffer().get();
        const bsl::vector<char>&  array      = expectedSequence[arrayIndex];

        while (blobOffset < (int) buffer.size() &&
                                            arrayOffset < (int) array.size()) {
            if (bufferData[blobOffset] != array[arrayOffset]) {
                bsl::cout << "Unequal bytes found at: ";
                P_(blobPosition); P_(blobIndex);
                P_(blobOffset);   P(blob.length());
                P_(arrayIndex);   P_(arrayOffset);
                P_(array.size())  P(expectedSequence.size());
                return false;                                         // RETURN
            }
            ++blobPosition;
            ++blobOffset;
            ++arrayOffset;
        }
        if (blobOffset >= (int) buffer.size()) {
            blobOffset = 0;
            ++blobIndex;
        }
        if (arrayOffset >= (int) array.size()) {
            arrayOffset = 0;
            ++arrayIndex;
        }
    }

    if (blobPosition  == maximumOffset) {
        return true;                                                  // RETURN
    }
    bsl::cout << "Unqueal end of buffer: ";
    P_(blobPosition); P_(blobIndex);
    P_(blobOffset);   P(blob.length());
    P_(arrayIndex);   P_(arrayOffset);
    P(expectedSequence.size());
    return false;
}

template <class IOVEC>
int loadTestVector(bsl::vector<IOVEC>  *vector,
                   bsl::vector<IOVEC>&  srcData,
                   const char          *description)
    // Load into the specified 'vector' elements from the specified 'srcData'
    // as indicated by the specified 'description'.  The 'description' must be
    // a null-terminated string with a space separated list of indices into the
    // 'srcData' vector (e.g., "1 2 3 4").  The behavior is undefined unless
    // each numeric element in 'description' is in the range [0,
    // 'srcData.size()').
{
    int length = 0;
    for (bdlb::Tokenizer it(bslstl::StringRef(description),
                            bslstl::StringRef(" "));
         it.isValid();
         ++it) {
        int index = atoi(it.token().data());
        vector->push_back(srcData[index]);
        length += srcData[index].length();
    }
    return length;

}

template <class IOVEC>
void testIovecArray()
    // Perform the standard value-semantic tests on a
    // 'btlmt::ChannelPool_IovecArray<IOVEC>'.
{
    typedef btlmt::ChannelPool_MessageUtil       Helper;
    typedef btlmt::ChannelPool_IovecArray<IOVEC> IovecArray;

    bslma::TestAllocator ta;

    // A series of dummy addresses used for this test
    char bufferA, bufferB, bufferC;

    IOVEC vecA(0, 0);
    IOVEC vecB(&bufferA, 1);
    IOVEC vecC(&bufferB, 64);
    IOVEC vecD(&bufferC, 256);

    bsl::vector<IOVEC> buffers(&ta);
    buffers.push_back(IOVEC(0, 0));
    buffers.push_back(IOVEC(&bufferA, 1));
    buffers.push_back(IOVEC(&bufferB, 64));
    buffers.push_back(IOVEC(&bufferC, 256));

    const char *VALUES[] = {
        "0",
        "0 0",
        "0 0 0",
        "0 0 1",
        "1 0 0",
        "0 1 0",
        "1 1 1",
        "1 0 2",
        "1 2 3",
        "0 1 2 3",
        "3 3 3 2 1 2 3"
    };
    const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

    if (veryVerbose) cout << "\tTesting IovecArray primary accessors"
                          << bsl::endl;
    {
        for (int i = 0; i < NUM_VALUES; ++i) {
            bsl::vector<IOVEC> mXdata(&ta);
            int length = loadTestVector(&mXdata, buffers, VALUES[i]);

            IovecArray mX(&mXdata.front(), mXdata.size());
            const IovecArray& MX = mX;

            ASSERT(length              == MX.length());
            ASSERT(&mXdata.front()     == MX.iovecs());
            ASSERT((int) mXdata.size() == MX.numIovecs());
        }
    }

    bsl::vector<bsl::vector<IOVEC> > data(&ta);
    data.resize(NUM_VALUES);
    for (int i = 0; i < NUM_VALUES; ++i) {
        loadTestVector(&data[i], buffers, VALUES[i]);
    }

    if (veryVerbose) cout << "\tTesting IovecArray comparison"
                          << bsl::endl;
    {

        for (int i = 0; i < NUM_VALUES; ++i) {
            IovecArray mX(&data[i].front(), data[i].size());
            const IovecArray& MX = mX;

            for (int j = 0; j < NUM_VALUES; ++j) {
                IovecArray mY(&data[j].front(), data[j].size());
                const IovecArray& MY = mY;

                bool isEqual = i == j;
                ASSERT(isEqual ==  (MX == MY));
                ASSERT(isEqual == !(MX != MY))
            }
        }
    }

    if (veryVerbose) cout << "\tTesting IovecArray copy construction"
                          << bsl::endl;
    {

        for (int i = 0; i < NUM_VALUES; ++i) {
            IovecArray mX(&data[i].front(), data[i].size());
            const IovecArray& MX = mX;

            IovecArray mW(&data[i].front(), data[i].size());
            const IovecArray& MW = mW;

            IovecArray mY(MX);
            const IovecArray& MY = mY;

            ASSERT(MX == MY);
            ASSERT(MW == MY);
        }
    }

    if (veryVerbose) cout << "\tTesting IovecArray assignment"
                          << bsl::endl;
    {

        for (int i = 0; i < NUM_VALUES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {

                IovecArray mU(&data[i].front(), data[i].size());
                const IovecArray& MU = mU;

                IovecArray mV(&data[j].front(), data[j].size());
                const IovecArray& MV = mV;

                IovecArray mW(MV);
                const IovecArray& MW = mW;

                mU = mV;

                ASSERT(MV == MU);
                ASSERT(MW == MU);
            }
        }

        // Test aliasing.
        for (int i = 0; i < NUM_VALUES; ++i) {
            IovecArray mU(&data[i].front(), data[i].size());
            const IovecArray& MU = mU;

            IovecArray mW(MU);
            const IovecArray& MW = mW;

            mU = MU;
            ASSERT(MW == MU);
        }
    }
}

}  // close namespace TEST_CASE_MESSAGEHELPER_NAMESPACE

//-----------------------------------------------------------------------------
// 'setWriteCache[Hi|Low]Watermark' & 'setWriteCacheWatermarks'
//-----------------------------------------------------------------------------
namespace TEST_CASE_CONCURRENCY_TEST {

int drainSocket(btlso::StreamSocket<btlso::IPv4Address> *clientSocket,
                int                                      numBytesExpected)
    // Read the specified 'numBytesExpected' bytes from the specified
    // 'clientSocket' and return the number of actual bytes read.
{
    enum { BUFF_SIZE = 512 };
    int rc = 0, numBytesRead = 0;
    char buffer[BUFF_SIZE];
    while (numBytesRead < numBytesExpected &&
           (rc = clientSocket->read(buffer, BUFF_SIZE)) > 0) {
        numBytesRead += rc;
    }
#ifndef BSLS_PLATFORM_OS_WINDOWS
    LOOP2_ASSERT(numBytesRead, numBytesExpected,
                                             numBytesRead == numBytesExpected);
#endif
    return numBytesRead;
}

enum {
    NUM_THREADS = 5
};

class TestCaseConcurrencyTest {

     typedef btlso::StreamSocket<btlso::IPv4Address> Socket;
     btlmt::ChannelPool * const d_pool_p;
     const int                  d_channelId;
     Socket * const             d_clientSocket_p;
     const int                  d_numThreads;
     bsls::AtomicInt            d_numBytesWritten;
     bsls::AtomicInt            d_done;
     bslmt::Barrier             d_barrier;
     bdlmt::FixedThreadPool     d_threadPool;

     void executeTest();
        // Perform the concurrency test: Write data to 'd_pool_p' and modify
        // the write cache of 'd_pool_p', increment 'd_numBytesWritten' with
        // the number of bytes written (as they are written), and increment
        // 'd_done' to indicate the operation has completed.

  public:

    TestCaseConcurrencyTest(
                       btlmt::ChannelPool                      *pool,
                       int                                      channelId,
                       btlso::StreamSocket<btlso::IPv4Address> *clientSocket,
                       int                                      numThreads,
                       bslma::Allocator                        *allocator = 0);

    void run();
        // Run 'executeTest' on multiple threads.

};

TestCaseConcurrencyTest::TestCaseConcurrencyTest(
                         btlmt::ChannelPool                      *pool,
                         int                                      channelId,
                         btlso::StreamSocket<btlso::IPv4Address> *clientSocket,
                         int                                      numThreads,
                         bslma::Allocator                        *allocator)
: d_pool_p(pool)
, d_channelId(channelId)
, d_clientSocket_p(clientSocket)
, d_numThreads(numThreads)
, d_numBytesWritten(0)
, d_done(0)
, d_barrier(numThreads + 1)
, d_threadPool(numThreads, 1000, allocator)
{
}

void TestCaseConcurrencyTest::executeTest()
{
    enum { LOW_WATERMARK = 64,
           HI_WATERMARK  = 1096,
           NUM_BYTES     = HI_WATERMARK * 25 };

    int rc = 0, totalBytesWritten = 0;
    btlb::Blob oneByteMsg;
    populateMessage(&oneByteMsg, 1, bslma::Default::allocator(0));

    d_barrier.wait();

    rc = d_pool_p->setWriteCacheWatermarks(d_channelId,
                                           LOW_WATERMARK, HI_WATERMARK);
    ASSERT(!rc);

    while (totalBytesWritten < NUM_BYTES) {
        int currentBytesWritten = 0;
        while (currentBytesWritten < (HI_WATERMARK / 4) &&
               0 == (rc = d_pool_p->write(d_channelId, oneByteMsg))) {
            currentBytesWritten += oneByteMsg.length();
            totalBytesWritten   += oneByteMsg.length();
            d_numBytesWritten   += oneByteMsg.length();
        }

        rc  = d_pool_p->setWriteCacheHiWatermark(d_channelId,
                                                 2 * HI_WATERMARK);
        rc |= d_pool_p->setWriteCacheLowWatermark(d_channelId,
                                                  2 * LOW_WATERMARK);
        ASSERT(!rc);

        currentBytesWritten = 0;
        while (currentBytesWritten < (HI_WATERMARK / 4) &&
               0 == (rc = d_pool_p->write(d_channelId, oneByteMsg))) {
            currentBytesWritten += oneByteMsg.length();
            totalBytesWritten   += oneByteMsg.length();
            d_numBytesWritten   += oneByteMsg.length();
        }

        rc = d_pool_p->setWriteCacheWatermarks(d_channelId,
                                               LOW_WATERMARK, HI_WATERMARK);
        ASSERT(!rc);
    }
    ++d_done;
}

void TestCaseConcurrencyTest::run()
{
    // Start 'd_numThreads' threads performing 'executeTest' then read data
    // from 'd_clientSocket_p' until all threads are completed (i.e.
    // 'd_done == d_numThreads') and all data has been read (i.e.
    // 'numBytesRead == d_numBytesWritten').
    d_numBytesWritten = 0;
    d_done            = 0;

    enum { BUFF_SIZE = 64 };
    char buffer[BUFF_SIZE];

    int numBytesRead = 0;
    d_threadPool.start();
    bsl::function<void()> job = bdlf::BindUtil::bind(
                                       &TestCaseConcurrencyTest::executeTest,
                                       this);
    for (int i = 0; i < d_numThreads; ++i) {
        d_threadPool.enqueueJob(job);
    }

    // Wait until all enqueued jobs are ready to begin writing to the
    // channel and then begin reading from the client socket.
    d_barrier.wait();
    while (d_numThreads != (int)d_done ||
           numBytesRead <  (int)d_numBytesWritten) {
        if (numBytesRead < (int)d_numBytesWritten) {
            int rc = d_clientSocket_p->read(buffer, BUFF_SIZE);
            ASSERT(rc > 0);
            numBytesRead += rc;
        }
        else {
            bslmt::ThreadUtil::microSleep(10);
        }
    }
    d_threadPool.drain();
    bslmt::ThreadUtil::microSleep(100);
    ASSERT(numBytesRead == (int)d_numBytesWritten);
    if (veryVerbose) {
        PT2((int)d_numBytesWritten, numBytesRead);
    }
}

}  // close namespace TEST_CASE_CONCURRENCY_TEST

//-----------------------------------------------------------------------------
// CASE -2 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_N2_NAMESPACE {
static
void caseN2ChannelStateCallback(int                  channelId,
                                int                  serverId,
                                int                  state,
                                void                *,
                                btlmt::ChannelPool **poolAddr,
                                int                **eventAddr,
                                bslmt::Barrier      *barrier,
                                int                 *channelId_p)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btlmt::ChannelPool *pool = *poolAddr;

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="    << serverId
                   << " channelId="   << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        *channelId_p = channelId;
        barrier->wait();
      } break;
    }
}
}  // close namespace TEST_CASE_N2_NAMESPACE

//-----------------------------------------------------------------------------
//                  TEST_CASE_STRESS_TEST supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_STRESS_TEST {

const int MAGIC          = 0x0ABCDEF0;

struct caseStressTestThreadInfo {
    bslmt::Barrier                  *d_barrier_p;
    Obj                             *d_channelPool_p;
    btlso::IPv4Address               d_serverAddress;
    bool                             d_isLeaderFlag;
    int                              d_groupId;
    bsls::AtomicInt                 *d_channelId;
    bsls::AtomicInt                 *d_oldChannelId;
    int                              d_bufferAlloc;
    int                              d_totalAlloc;
    int                              d_numIters;
    char                             d_write;
    btlb::PooledBlobBufferFactory   *d_factory_p;
    int                              d_messageLength;
    bslma::Allocator                *d_allocator_p;
};

extern "C" {

void *caseStressTestThread(void *arg)
{
    caseStressTestThreadInfo *info =
        static_cast<caseStressTestThreadInfo *>(arg);
    const int BUF_SIZE       = info->d_bufferAlloc;
    const int MSG_LENGTH     = info->d_messageLength;
    const int TOTAL_SIZE     = MSG_LENGTH ? MSG_LENGTH : info->d_totalAlloc;
    const int NUM_ITERATIONS = info->d_numIters;
    const char PAYLOAD_CHAR  = info->d_write;

    int retCode;
    btlmt::ChannelPool *mXp = info->d_channelPool_p;
    const btlso::IPv4Address ADDRESS = info->d_serverAddress;

    // Set up buffer to write to channel pool, of total size
    // TOTAL_SIZE, filled with the requested value.
    char *buffer = (char *) info->d_allocator_p->allocate(TOTAL_SIZE);
    bsl::memset(buffer, PAYLOAD_CHAR, TOTAL_SIZE);
    if (MSG_LENGTH) {
        MTASSERT((unsigned)BUF_SIZE >= sizeof MAGIC + sizeof MSG_LENGTH);

        bsl::memcpy(buffer,                &MAGIC,      sizeof MAGIC);
        bsl::memcpy(buffer + sizeof MAGIC, &MSG_LENGTH, sizeof MSG_LENGTH);
    }

    // Be careful to round numVecs upward and compute lastVec size correctly.
    const int numVecs = (TOTAL_SIZE + BUF_SIZE - 1) / BUF_SIZE;
    const int LAST_VEC_SIZE = (0 < TOTAL_SIZE % BUF_SIZE)
                              ? TOTAL_SIZE % BUF_SIZE
                              : BUF_SIZE;  // size of the last buffer

    btlb::Blob blob(info->d_factory_p, info->d_allocator_p);
    for (int i = 0 ;i < numVecs; ++i) {
        btlb::BlobUtil::append(&blob,
                                 buffer + i * BUF_SIZE,
                                 (i < numVecs-1) ? BUF_SIZE : LAST_VEC_SIZE);
    }

    info->d_allocator_p->deallocate(buffer);

    // If keeping track of message length, put it into the first eight bytes.
    if (info->d_isLeaderFlag) {
        if (verbose) {
            MTCOUT << "\tInitial connection: channelId=" << *info->d_channelId
                   << ", groupId=" << info->d_groupId << MTENDL;
        }
        retCode = mXp->connect(ADDRESS,
                               100, bsls::TimeInterval(1.0), // 1s
                               info->d_groupId);
        MTLOOP_ASSERT(retCode, 0 == retCode);
        bslmt::ThreadUtil::yield();
        bslmt::ThreadUtil::microSleep(0,  1); // 1s
    }

    // Wait until all threads have initialized their buffers.
    info->d_barrier_p->wait();

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        int channelId = *info->d_channelId;

        if (!MSG_LENGTH) {
            // Leading thread shuts down channel every 10 iterations, unless
            // we're tracking the message size (in that case, closing the
            // channel would prevent reading the echoed messages back for
            // checking them).

            if (info->d_isLeaderFlag && 0 == i % 10 && 0 < i) {
                *info->d_oldChannelId = channelId;
                *info->d_channelId = 0;
                if (verbose) {
                    MTCOUT << "\tShutting down: channelId=" << channelId
                           << ", groupId=" << info->d_groupId
                           << ", iter=" << i
                           << ", numChannels=" << mXp->numChannels() << MTENDL;
                }
                retCode = mXp->shutdown(channelId,
                                        btlmt::ChannelPool::e_IMMEDIATE);
                if (verbose) {
                    MTCOUT << "\tAfter shutting down: channelId=" << channelId
                           << ", numChannels=" << mXp->numChannels()
                           << ", shutdown=" << retCode
                           << MTENDL;
                }
                MTLOOP2_ASSERT(retCode, channelId,
                               0 == retCode || -1 == retCode);

                // All threads wait while channel is down, but not
                // indefinitely.

                int countDown = 1000;
                do {
                    channelId = *info->d_channelId;
                    --countDown;
                    if (!channelId) {
                        bslmt::ThreadUtil::yield();
                        bslmt::ThreadUtil::microSleep(10000);  // 10ms
                    }
                } while (countDown && !channelId);

                MTASSERT(channelId);

                if (!channelId) {
                    // There is no point continuing.  We already raised an
                    // error above.

                    break;
                }
            }
        }

        retCode = mXp->write(channelId, blob);
        MTLOOP_ASSERT(retCode,
                       0 == retCode ||  // o.k.
                      -2 == retCode ||  // reached high-water mark
                      -3 == retCode ||  // channel down
                      -5 == retCode);   // channel unknown

        if (veryVerbose) {
            if (0 == retCode) {
                MTCOUT << "\t\tWrote " << blob.length()
                       << " bytes to channel " << channelId << MTENDL;
            }
            else {
                MTCOUT << "\t\tDroppped " << blob.length()
                       << " bytes to channel " << channelId << MTENDL;
            }
        }
    }
    return 0;
}

} // closing extern "C"

static
void caseStressTestPoolStateCallback(
    int             state,
    int             sourceId,
    int             severity)
{
    switch (state) {
      case PoolState::BTEMT_ACCEPT_TIMEOUT: {
        if (verbose) {
            MTCOUT << "\tAccept timed out:"
                   << " sourceId=" << sourceId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case PoolState::BTEMT_ERROR_CONNECTING: {
        if (verbose) {
            MTCOUT << "\tError Connecting:"
                   << " sourceId=" << sourceId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case PoolState::BTEMT_ERROR_ACCEPTING: {
        if (verbose) {
            MTCOUT << "\tError Accepting:"
                   << " sourceId=" << sourceId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case PoolState::BTEMT_CHANNEL_LIMIT: {
        if (verbose) {
            MTCOUT << "\tChannel Limit Reached:"
                   << " sourceId=" << sourceId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
    }
}

struct caseStressTestCallbackInfo {
    Obj                **d_channelPool_p;
    btlso::IPv4Address   d_serverAddress;
    bsls::AtomicInt     *d_channelId1;
    bsls::AtomicInt     *d_channelId2;
    bsls::AtomicInt     *d_oldChannelId1;
    bsls::AtomicInt     *d_oldChannelId2;
    char                 d_payloadChar1;
    char                 d_payloadChar2;
    int                  d_numNeeded;
    int                  d_msgSize;
};

static
void caseStressTestChannelStateCallback(int                 channelId,
                                        int                 sourceId,
                                        int                 state,
                                        void               *,
                                        caseStressTestCallbackInfo *info)
{
    MTASSERT(info);
    btlmt::ChannelPool        *mXp = *info->d_channelPool_p;
    const btlso::IPv4Address&  ADDRESS = info->d_serverAddress;
    const int                 oldChannelId1 = *info->d_oldChannelId1;
    const int                 oldChannelId2 = *info->d_oldChannelId2;

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (oldChannelId1 == channelId) {
            int retCode = mXp->connect(ADDRESS,
                                       10, bsls::TimeInterval(0.010), // 10ms
                                       0); // clientId first group
            MTLOOP_ASSERT(retCode, 0 == retCode);
        }
        else if (oldChannelId2 == channelId) {
            int retCode = mXp->connect(ADDRESS,
                                       10, bsls::TimeInterval(0.010), // 10ms
                                       1); // clientId second group
            MTLOOP_ASSERT(retCode, 0 == retCode);
        }
        else {
            if (verbose) {
                MTCOUT << "\tServer connection closed:"
                       << " sourceId="  << sourceId
                       << " channelId=" << channelId
                       << " numChannels=" << mXp->numChannels()
                       << MTENDL;
            }
            break; // don't issue verbose "Connection reinitiated" below
        }
        if (verbose) {
            MTCOUT << "\tConnection terminated:"
                   << " sourceId="  << sourceId
                   << " channelId=" << channelId
                   << " numChannels=" << mXp->numChannels()
                   << MTENDL;
        }
        if (verbose) {
            MTCOUT << "\tConnection reinitiated:"
                   << " sourceId="  << sourceId
                   << " oldChannelId=" << channelId
                   << " numChannels=" << mXp->numChannels()
                   << MTENDL;
        }
      } break;
      case btlmt::ChannelPool::e_CHANNEL_UP: {
        MTASSERT(0 == mXp->setSocketOption(
                                       btlso::SocketOptUtil::k_SENDBUFFER,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       32768,
                                       channelId));
        if (verbose) {
            MTCOUT << "\tConnection established:"
                   << " sourceId="    << sourceId
                   << " channelId="   << channelId
                   << " numChannels=" << mXp->numChannels()
                   << MTENDL;
        }
        if (0 == sourceId) {
            *info->d_channelId1 = channelId;
            *info->d_oldChannelId1 = 0;
        }
        else if (1 == sourceId) {
            *info->d_channelId2 = channelId;
            *info->d_oldChannelId2 = 0;
        }
        // Else the connection is server-side, nothing to do.

      } break;
    }
}

void caseStressTestDataCallback(int                *numNeeded,
                                btlb::Blob         *msg,
                                int                 channelId,
                                void               *,
                                caseStressTestCallbackInfo *info)
{
    // This is an echo server, the clients merely receive the data but don't
    // echo it.  They do only minimal checking.  We need to adjust the
    // channelId just in case the channel has been shut down, in which case
    // channelId is 0 but oldChannelId still has the good data.
    btlmt::ChannelPool  *mXp = *(info->d_channelPool_p);
    const int           channelId1 = *info->d_channelId1
                                   ? *info->d_channelId1
                                   : *info->d_oldChannelId1;
    const int           channelId2 = *info->d_channelId2
                                   ? *info->d_channelId2
                                   : *info->d_oldChannelId2;
    const char          PAYLOAD_CHAR1 = info->d_payloadChar1;
    const char          PAYLOAD_CHAR2 = info->d_payloadChar2;

    MTASSERT(numNeeded);
    MTASSERT(msg);
    MTLOOP_ASSERT(msg->length(), 0 < msg->length());

    bsl::shared_ptr<char> buffer =
        bslstl::SharedPtrUtil::createInplaceUninitializedBuffer(msg->length());
    char *dataBuffer = buffer.get();
    btlb::BlobUtil::copy(dataBuffer, *msg, 0, msg->length());

    if (channelId == channelId1 || channelId == channelId2) {
        if (0 < info->d_msgSize) {
            // Keep track of message length read by each thread.  The first
            // four bytes of a message indicate its length and must be equal to
            // MSG_LEN.  The remaining bytes must be equal to the PAYLOAD_CHAR.

            int length = msg->length();
            const int MSG_LENGTH = info->d_msgSize;

            int numConsumed = 0;
            *numNeeded      = MSG_LENGTH;

            if (veryVerbose) {
                MTCOUT << "\t\tClient receiving back " << length
                       << " bytes on channel " << channelId << MTENDL;
            }

            while (length >= MSG_LENGTH) {
                union {
                    int d_msg;
                    char msg_buffer[sizeof(int)];
                } x;
                x.msg_buffer[0] = dataBuffer[numConsumed];
                x.msg_buffer[1] = dataBuffer[numConsumed + 1];
                x.msg_buffer[2] = dataBuffer[numConsumed + 2];
                x.msg_buffer[3] = dataBuffer[numConsumed + 3];
                MTLOOP2_ASSERT(channelId, x.d_msg, MAGIC == x.d_msg);

                x.msg_buffer[0] = dataBuffer[numConsumed + 4];
                x.msg_buffer[1] = dataBuffer[numConsumed + 5];
                x.msg_buffer[2] = dataBuffer[numConsumed + 6];
                x.msg_buffer[3] = dataBuffer[numConsumed + 7];
                MTLOOP2_ASSERT(channelId, x.d_msg, MSG_LENGTH == x.d_msg);

                const char PAYLOAD_CHAR = (channelId == channelId1)
                                        ? PAYLOAD_CHAR1 : PAYLOAD_CHAR2;

                for (int i = 8; i < MSG_LENGTH; ++i) {
                    MTLOOP2_ASSERT(PAYLOAD_CHAR,
                                   dataBuffer[numConsumed + i],
                    PAYLOAD_CHAR == dataBuffer[numConsumed + i]);

                    if (PAYLOAD_CHAR != dataBuffer[i]) {
                        x.msg_buffer[0] = dataBuffer[numConsumed + i];
                        x.msg_buffer[1] = dataBuffer[numConsumed + i + 1];
                        x.msg_buffer[2] = dataBuffer[numConsumed + i + 2];
                        x.msg_buffer[3] = dataBuffer[numConsumed + i + 3];
                        if (MAGIC == x.d_msg) {
                            MTLOOP2_ASSERT(channelId, i,
                                           0 == "Client receiving MAGIC word");
                        }
                        if (verbose) {
                            PT3(__LINE__, (int)PAYLOAD_CHAR,
                                          (int)dataBuffer[i]);
                        }
                        numConsumed += i;
                        btlb::BlobUtil::erase(msg, 0, numConsumed);
                        return;                                       // RETURN
                    }
                }

                numConsumed += MSG_LENGTH;  // but not if we return early
                length -= MSG_LENGTH;
            }
            *numNeeded = MSG_LENGTH - length;
        }
        else {
            // In this mode, numNeeded is driven by the test case, always,
            // regardless of how many messages we have read, and of what
            // length.  Also, we always expect the char read to be the
            // PAYLOAD_CHAR.

            int numConsumed = msg->length();
            *numNeeded   = info->d_numNeeded;

            if (veryVerbose) {
                MTCOUT << "\t\tClient receiving back " << numConsumed
                       << " bytes on channel " << channelId << MTENDL;
            }
            const char PAYLOAD_CHAR = (channelId == channelId1)
                                    ? PAYLOAD_CHAR1 : PAYLOAD_CHAR2;
            for (int i = 0; i < numConsumed; ++i) {
                MTLOOP2_ASSERT(channelId, i,
                                   PAYLOAD_CHAR == dataBuffer[i]);
                if (PAYLOAD_CHAR != dataBuffer[i]) {
                    if (verbose) {
                        PT3(__LINE__, (int)PAYLOAD_CHAR,
                                      (int)dataBuffer[i]);
                    }
                    btlb::BlobUtil::erase(msg, 0, numConsumed);
                    return;                                           // RETURN
                }
            }
        }
    }
    else {
        int numConsumed = msg->length();
        *numNeeded   = info->d_numNeeded;

        int retCode = mXp->write(channelId, *msg);
        MTLOOP_ASSERT(retCode, 0 == retCode ||
                              -2 == retCode ||  // reached high-water mark
                              -3 == retCode);   // channel shut down
        if (0 == retCode) {
            if (veryVerbose) {
                MTCOUT << "\t\tServer echoing back " << numConsumed
                       << " bytes on channel " << channelId << MTENDL;
            }
        }
        else {
            int retCode = mXp->shutdown(channelId,
                                        btlmt::ChannelPool::e_IMMEDIATE);
            MTLOOP_ASSERT(retCode, 0 == retCode || -1 == retCode);
            if (verbose) {
                MTCOUT << "\tServer connection closed:"
                       << " channelId=" << channelId
                       << " numChannels=" << mXp->numChannels()
                       << " shutdown=" << retCode
                       << MTENDL;
            }
            if (veryVerbose) {
                MTCOUT << "\t\tServer dropped " << numConsumed
                       << " bytes on channel " << channelId << MTENDL;
            }
        }

        btlb::BlobUtil::erase(msg, 0, numConsumed);
    }
}

static
void runTestCaseStressTest(
                   char                                           *,
                   int                                             numThreads,
                   int                                             numIters,
                   int                                             bufferSize,
                   int                                             msgSize,
                   int                                             serverSize,
                   int                                             srvrNeeded,
                   btlso::StreamSocketFactory<btlso::IPv4Address> *,
                   bslma::TestAllocator                           *allocator)
{
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Test Initialization

    enum {
        SERVER_ID           = 1013410001,
        BACKLOG             = 100,
        REUSE_ADDRESS       = 1,
        PAYLOAD_CHAR1       = 'a',
        PAYLOAD_CHAR2       = 'Z'
    };
    const int NUM_THREADS   = numThreads;
    const int NUM_ITERS     = numIters;
    const int BUFFER_ALLOC  = bufferSize;
    const int RECEIVE_ALLOC = serverSize;
    const int SEND_ALLOC    = msgSize > 0 ? msgSize : - msgSize;

    // Channel pool configuration.

    const btlso::IPv4Address ADDRESS("127.0.0.1", 0);

    btlmt::ChannelPool *poolAddr;
    bsls::AtomicInt     channelId1(0);
    bsls::AtomicInt     channelId2(0);
    bsls::AtomicInt     oldChannelId1(0);
    bsls::AtomicInt     oldChannelId2(0);

    caseStressTestCallbackInfo info;
    info.d_channelPool_p = &poolAddr;
    info.d_serverAddress = ADDRESS;
    info.d_channelId1    = &channelId1;
    info.d_channelId2    = &channelId2;
    info.d_oldChannelId1 = &oldChannelId1;
    info.d_oldChannelId2 = &oldChannelId2;
    info.d_payloadChar1  = (char)PAYLOAD_CHAR1;
    info.d_payloadChar2  = (char)PAYLOAD_CHAR2;
    info.d_numNeeded     = srvrNeeded;
    info.d_msgSize       = msgSize < 0 ? -msgSize : 0;

    btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
            bdlf::BindUtil::bind( &caseStressTestChannelStateCallback
                                , _1, _2, _3, _4
                                , &info));

    btlmt::ChannelPool::BlobBasedReadCallback  dataCb(
            bdlf::BindUtil::bind( &caseStressTestDataCallback
                                , _1, _2, _3, _4
                                , &info));

    btlmt::ChannelPool::PoolStateChangeCallback poolCb(
        bsl::allocator_arg_t(),
        bsl::allocator<btlmt::ChannelPool::PoolStateChangeCallback>(allocator),
        &caseStressTestPoolStateCallback);

    btlmt::ChannelPoolConfiguration cpc;
    cpc.setMaxConnections(4 * NUM_ITERS / 10);
       // this very high number is very pessimistic but allows maximum overlap
       // of a closing connection and its bouncing next connections - unlike
       // basapi_tcpclient, connections are not synchronous and accepted
       // channels may take a while to be destroyed (the server shuts down a
       // channel in the readDataCb when the read fails, which could happen
       // after the specified timeout: to witness faster server shutdowns,
       // simply shorten the readTimeout below).

    cpc.setMaxThreads(NUM_THREADS);
    cpc.setMetricsInterval(100.0);

    int maxIncomingMessageSize = RECEIVE_ALLOC;
    cpc.setIncomingMessageSizes((maxIncomingMessageSize + 1) / 2,
                                3 * (maxIncomingMessageSize + 1) / 4,
                                maxIncomingMessageSize);
    cpc.setReadTimeout(0.1);

    int maxOutgoingMessageSize = SEND_ALLOC;
    cpc.setOutgoingMessageSizes(0,
                                (maxOutgoingMessageSize + 1) / 2,
                                maxOutgoingMessageSize);

    btlb::PooledBlobBufferFactory bufferFactory(BUFFER_ALLOC, allocator);

    // Starting server.
    {
        Obj mX(channelCb, dataCb, poolCb, cpc, allocator);  const Obj& X = mX;
        poolAddr = &mX;

        ASSERT(0 == mX.start());
        ASSERT(0 == mX.listen(ADDRESS, BACKLOG, SERVER_ID, REUSE_ADDRESS));
        ASSERT(0 == X.numChannels());
        btlso::IPv4Address PEER = getServerLocalAddress(&mX, SERVER_ID);
        info.d_serverAddress = PEER;

        ASSERT(0 == mX.setServerSocketOption(
                            btlso::SocketOptUtil::k_SENDBUFFER,
                            btlso::SocketOptUtil::k_SOCKETLEVEL,
                            RECEIVE_ALLOC, SERVER_ID));
        ASSERT(0 == mX.setServerSocketOption(
                            btlso::SocketOptUtil::k_RECEIVEBUFFER,
                            btlso::SocketOptUtil::k_SOCKETLEVEL,
                            RECEIVE_ALLOC, SERVER_ID));

        bslmt::ThreadUtil::Handle  *threads = (bslmt::ThreadUtil::Handle*)
         allocator->allocate(sizeof(bslmt::ThreadUtil::Handle)  * NUM_THREADS);
        caseStressTestThreadInfo *info = (caseStressTestThreadInfo*)
           allocator->allocate(sizeof(caseStressTestThreadInfo) * NUM_THREADS);
        bslmt::Barrier  barrier(NUM_THREADS + 1);
        for (int i = 0; i < NUM_THREADS; ++i) {
            info[i].d_barrier_p     = &barrier;
            info[i].d_channelPool_p = &mX;
            info[i].d_serverAddress = PEER;
            info[i].d_isLeaderFlag  = (0 == i || (NUM_THREADS / 2) == i);
            info[i].d_groupId       = (i < NUM_THREADS / 2) ? 0 : 1;
            info[i].d_channelId     = (i < NUM_THREADS / 2)
                                    ? &channelId1 : &channelId2;
            info[i].d_oldChannelId  = (i < NUM_THREADS / 2)
                                    ? &oldChannelId1 : &oldChannelId2;
            info[i].d_bufferAlloc   = BUFFER_ALLOC;
            info[i].d_totalAlloc    = SEND_ALLOC;
            info[i].d_write         = (i < NUM_THREADS / 2)
                                    ? PAYLOAD_CHAR1 : PAYLOAD_CHAR2;
            info[i].d_numIters      = NUM_ITERS;
            info[i].d_factory_p     = &bufferFactory;
            info[i].d_allocator_p   = allocator;
            info[i].d_messageLength = msgSize < 0 ? -msgSize : 0;

            ASSERT(0  == bslmt::ThreadUtil::create(&threads[i],
                                                   &caseStressTestThread,
                                                   &info[i]));
        }

        barrier.wait();

        // Give some time for channel pool to read all the data, then stop.

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
        }

        if (verbose) {
            MTCOUT << "\tShutting down channel pool:" << MTFLUSH;

            bsls::Types::Int64 numBytesRead;
            bsls::Types::Int64 numBytesWritten;
            bsls::Types::Int64 numBytesRequestedToBeWritten;

            X.totalBytesRead(&numBytesRead);
            X.totalBytesWritten(&numBytesWritten);
            X.totalBytesRequestedToBeWritten(&numBytesRequestedToBeWritten);

            MTCOUT << "\n\t\tread "      << numBytesRead
                   << "\n\t\twrote "     << numBytesWritten
                   << "\n\t\tserver requested " << numBytesRequestedToBeWritten
                   << MTENDL;
        }
        mX.stop();

        allocator->deallocate(threads);
        allocator->deallocate(info);
    }
}

}  // close namespace TEST_CASE_STRESS_TEST

//-----------------------------------------------------------------------------
//                  TEST_CASE_SSL_SOCKETS supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_SSL_SOCKETS {

template <class ADDRESS>
class bteso_SslLikeStreamSocketFactory;

                       // =========================
                       // class SslLikeStreamSocket
                       // =========================

template <class ADDRESS>
class bteso_SslLikeStreamSocket : public btlso::StreamSocket<ADDRESS> {
    // This class implements the 'btlso::StreamSocket<ADDRESS>' protocol to
    // provide stream-based socket communications.  It holds an underlying
    // socket to which it forwards all function calls.  The only addition to
    // the 'btlso::StreamSocket' is that writing and reading go through an
    // internal buffer whose size is provided at construction.  As such, there
    // may not be a socket event to signal that new data is available, since
    // the data has already been put into the internal buffer.  Similarly, data
    // written to the socket will not be written until the write buffer is
    // full, or a call to flush is invoked.  This is useful to test if the
    // channel pool is reading a socket correctly and completely before waiting
    // for the next socket event.  In addition, this socket accepts a
    // specified limit on the number of bytes it will read or write before the
    // connection will shutdown.  This is useful to test if the channel pool
    // correctly handles unexpected events leading to a channel shutdown.

  private:
    // PRIVATE DATA MEMBERS
    btlso::StreamSocket<ADDRESS>
                     *d_socket_p;              // held, not owned

    int               d_bufferSize;            // internal buffers size

    char             *d_readBuffer;            // internal read buffer (owned)

    int               d_readBytesAvailable;    // number of data bytes in
                                               // internal read buffer

    int               d_maxReadBytesAvailable; // number of data bytes that
                                               // should be read until the
                                               // read limit is reached (or
                                               // -1 if there is no read limit)

    char             *d_writeBuffer;           // internal write buffer (owned)

    int               d_writeOffset;           // offset for next write

    int               d_maxWriteBytes;         // maximum number of bytes this
                                               // socket will accept for write
                                               // until write limit is reached
                                               // (or -1 if there is no write
                                               // limit)

    bslma::Allocator *d_allocator_p;           // held, not owned

    friend class bteso_SslLikeStreamSocketFactory<ADDRESS>;

    // CREATORS
    bteso_SslLikeStreamSocket(int                          internalBufferSize,
                              btlso::StreamSocket<ADDRESS> *socket,
                              bslma::Allocator            *allocator);
        // Create a ssl-like stream socket wrapper around the specified
        // 'socket' that uses the specified 'allocator' to supply memory.  If
        // 'allocator' is 0, the currently installed default allocator is used.
        // The behavior is undefined unless socket 'handle' refers to a valid
        // system socket.

    // Not implemented
    bteso_SslLikeStreamSocket(
                           const bteso_SslLikeStreamSocket<ADDRESS>& original);
    bteso_SslLikeStreamSocket<ADDRESS>& operator=(
                                const bteso_SslLikeStreamSocket<ADDRESS>& rhs);

  public:
    // CREATORS
    ~bteso_SslLikeStreamSocket<ADDRESS>();
        // Destroy this 'bteso_SslLikeStreamSocket' object.

    // MANIPULATORS
    virtual int accept(btlso::StreamSocket<ADDRESS> **result);
    virtual int accept(btlso::StreamSocket<ADDRESS> **socket,
                       ADDRESS                      *peerAddress );
        // Return the result of calling 'accept' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int bind(const ADDRESS& address);
        // Return the result of calling 'bind' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int connect(const ADDRESS& address);
        // Return the result of calling 'connect' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int listen(int backlog);
        // Return the result of calling 'listen' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int read(char *buffer, int length);
        // Copy the specified 'length' bytes from the internal read buffer to
        // the specified 'buffer', or the number of available bytes in the
        // internal read buffer if this number is smaller.  Return the number
        // of bytes copied.  If the internal read buffer is exhausted, first
        // attempt to refill it by calling 'read' on the underlying
        // 'btlso::StreamSocket' object, and return the error code if that call
        // returns an error code.  In particular, if there is no data in the
        // internal buffer, either block (in blocking mode) or return
        // 'ERROR_WOULDBLOCK' if reading on the underlying socket would block.

    virtual int readv(const btls::Iovec *buffers, int numBuffers);
        // Fill in the up to 'numBuffers' specified 'buffers' with data from
        // the internal read buffer, up to the number of bytes available in the
        // internal read buffer if this number is smaller that the total size
        // of the 'buffers'.  Return the number of bytes copied.  If the
        // internal read buffer is exhausted, first attempt to refill it by
        // calling 'read' on the underlying 'btlso::StreamSocket' object, and
        // return the error code if that call returns an error code.  In
        // particular, if there is no data in the internal buffer, either block
        // (in blocking mode) or return 'ERROR_WOULDBLOCK' if reading on the
        // underlying socket would block.

    virtual int write(const char *buffer, int length);
        // Copy the specified 'length' bytes from the specified 'buffer' to the
        // internal write buffer, or the number of free bytes in the internal
        // write buffer if this number is smaller.  If the internal write
        // buffer gets filled up, subsequently attempt to flush it by calling
        // 'flush' on the underlying 'btlso::StreamSocket' object, and return
        // the error code if that call returns an error code.  Otherwise,
        // return the number of bytes copied.  In particular, if there is no
        // data in the internal buffer, either block (in blocking mode) or
        // return 'ERROR_WOULDBLOCK' if reading on the underlying socket would
        // block.  Note that a successful call to this function does not
        // guarantee that the data has been transmitted successfully, but
        // simply that the data was successfully written to the underlying
        // socket's transmit buffers.

    virtual int writev(const btls::Iovec *buffers, int numBuffers);
    virtual int writev(const btls::Ovec *buffers, int numBuffers);
        // Copy up to the specified 'numBuffers' of the specified 'buffers'
        // from the internal write buffer, or the number of free bytes in the
        // internal write buffer if this number is smaller than the total size
        // of the 'buffers'.  If the internal write buffer gets filled up,
        // subsequently attempt to flush it by calling 'write' on the
        // underlying 'btlso::StreamSocket' object, and return the error code
        // if that call returns an error code.  Otherwise, return the number of
        // bytes copied.  In particular, if there is no data in the internal
        // buffer, either block (in blocking mode) or return 'ERROR_WOULDBLOCK'
        // if reading on the underlying socket would block.  Note that a
        // successful call to this function does not guarantee that the data
        // has been transmitted successfully, but simply that the data was
        // successfully written to this socket's transmit buffers.  Use
        // 'writeFlush' to make sure that the data is actually written to the
        // underlying socket.

    int writeFlush();
        // Flush the write buffer to the underlying socket, and return the
        // number of bytes written from the internal buffer if some data
        // was written, zero if the internal buffer is empty, or return the
        // error code of the underlying 'write' call if that call returns an
        // error code.  In particular, if there is data in the internal buffer,
        // either block until all data is flushed (in blocking mode) or return
        // 'ERROR_WOULDBLOCK' if reading on the underlying socket would block.

    virtual int setBlockingMode(btlso::Flag::BlockingMode mode);
        // Return the result of calling 'setBlockingMode' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int shutdown(btlso::Flag::ShutdownType streamOption);
        // Return the result of calling 'shutdown' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int waitForConnect(const bsls::TimeInterval& timeout);
        // Return the result of calling 'waitForConnect' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int waitForAccept(const bsls::TimeInterval& timeout);
        // Return the result of calling 'waitForAccept' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int waitForIO(btlso::Flag::IOWaitType   type,
                          const bsls::TimeInterval& timeout);
    virtual int waitForIO(btlso::Flag::IOWaitType type);
        // Return the result of calling 'waitForIO' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int setLingerOption(
                           const btlso::SocketOptUtil::LingerData& options);
        // Return the result of calling 'setLingerOption' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int setOption(int level, int option, int value);
        // Return the result of calling 'setOption' on the underlying
        // 'btlso::StreamSocket' object.

    void setReadLimit(int numBytes);
        // Set this socket to accept at most the specified 'numBytes' number of
        // bytes in subsequent write calls, and to shut down the connection if
        // this limit is reached.

    void setWriteLimit(int numBytes);
        // Set this socket to read at most the specified 'numBytes' number of
        // bytes in subsequent read calls, and to shut down the connection if
        // this limit is reached.

    // ACCESSORS
    virtual int blockingMode(btlso::Flag::BlockingMode *result) const;
        // Return the result of calling 'blockingMode' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int connectionStatus() const;
        // Return the result of calling 'connectionStatus' on the underlying
        // 'btlso::StreamSocket' object.

    virtual btlso::SocketHandle::Handle handle() const;
        // Return the result of calling 'handle' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int localAddress(ADDRESS *result) const;
        // Return the result of calling 'localAddress' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int peerAddress(ADDRESS *result) const;
        // Return the result of calling 'peerAddress' on the underlying
        // 'btlso::StreamSocket' object.

    virtual int lingerOption(btlso::SocketOptUtil::LingerData *result) const;
        // Return the result of calling 'lingerOption' on the underlying
        // 'btlso::StreamSocket' object.
    virtual int socketOption(int *result, int level, int option) const;
        // Return the result of calling 'socketOption' on the underlying
        // 'btlso::StreamSocket' object.
};

                    // ================================
                    // class SslLikeStreamSocketFactory
                    // ================================

template <class ADDRESS>
class bteso_SslLikeStreamSocketFactory
: public btlso::InetStreamSocketFactory<ADDRESS>
{
    // Provide an implementation for a factory to allocate and deallocate
    // TCP-based OpenSsl-like stream socket objects of type
    // 'bteso_SslLikeStreamSocket<ADDRESS>'.

    int               d_bufferSize;  // used to allocate ssl-like sockets
    bslma::Allocator *d_allocator_p; // holds (but doesn't own) object

    public:
    // CREATORS
    bteso_SslLikeStreamSocketFactory(int               internalBufferSize,
                                     bslma::Allocator *basicAllocator = 0);
        // Create a stream socket factory for ssl-like sockets with an internal
        // buffer of the specified 'internalBufferSize'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    virtual ~bteso_SslLikeStreamSocketFactory();
        // Destroy this factory.  The stream sockets created by this factory
        // will not be destroyed when this factory is destroyed.

    // MANIPULATORS
    virtual btlso::StreamSocket<ADDRESS> *allocate();
        // Create a stream-based socket of type
        // 'bteso_SslLikeStreamSocket<ADDRESS>'.  Return the address of the
        // newly-created socket on success, and 0 otherwise.

    virtual btlso::StreamSocket<ADDRESS> *allocate(
                                           btlso::SocketHandle::Handle handle);
        // Create a stream-based socket of type
        // 'bteso_SslLikeStreamSocket<ADDRESS>' attached to the specified
        // socket 'handle'.  Return the address of the newly-created socket on
        // success, and 0 otherwise.  The behavior is undefined unless 'handle'
        // refers to a valid TCP-based stream socket with the address class
        // 'ADDRESS'.

    virtual void deallocate(btlso::StreamSocket<ADDRESS> *socket);
    virtual void deallocate(btlso::StreamSocket<ADDRESS> *socket, bool);
        // Return the specified 'socket' back to this factory.  The behavior is
        // undefined unless 'socket' was allocated using this factory or was
        // created through an 'accept' from a stream socket created using this
        // factory, and has not already been deallocated.  Note that the
        // underlying TCP-based stream socket will be closed.
};

              // ================================================
              // class SslLikeStreamSocketFactory_AutoCloseSocket
              // ================================================

template <class ADDRESS>
class bteso_SslLikeStreamSocket_AutoCloseSocket {
    // This class implements a proctor that automatically deallocates the
    // managed socket at destruction unless its 'release' method is
    // invoked.

    btlso::StreamSocket<ADDRESS>    *d_socket_p;  // socket under proctorship
    bteso_SslLikeStreamSocketFactory<ADDRESS>
                                   *d_factory_p; // used to deallocate socket
    int                             d_valid;     // true until 'release' called

    // Not implemented
    bteso_SslLikeStreamSocket_AutoCloseSocket(
                             const bteso_SslLikeStreamSocket_AutoCloseSocket&);
    bteso_SslLikeStreamSocket_AutoCloseSocket& operator=(
                             const bteso_SslLikeStreamSocket_AutoCloseSocket&);
  public:
    // CREATORS
    bteso_SslLikeStreamSocket_AutoCloseSocket(
                            btlso::StreamSocket<ADDRESS>              *socket,
                            bteso_SslLikeStreamSocketFactory<ADDRESS> *factory)
        // Create a proctor object to manage socket having the specified
        // 'socketHandle'.
        : d_socket_p(socket)
        , d_factory_p(factory)
        , d_valid(1) { }

    ~bteso_SslLikeStreamSocket_AutoCloseSocket()
        // Destroy this proctor object and, unless the 'release' method has
        // been previously called, close the managed socket.
    {
        if (d_valid) {
            d_factory_p->deallocate(d_socket_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release from management the socket currently managed by
        // this proctor.  If no socket is currently being managed, this
        // operation has no effect.
    {
        d_valid = 0;
    }
};

                        // -------------------------
                        // class SslLikeStreamSocket
                        // -------------------------

// CREATORS
template <class ADDRESS>
bteso_SslLikeStreamSocket<ADDRESS>::bteso_SslLikeStreamSocket(
        int                          internalBufferSize,
        btlso::StreamSocket<ADDRESS> *socket,
        bslma::Allocator            *basicAllocator)
: d_socket_p(socket)
, d_bufferSize(internalBufferSize)
, d_readBytesAvailable(0)
, d_writeOffset(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bsl::auto_ptr<char> readBuffer((char *)d_allocator_p->
                                                       allocate(d_bufferSize));
    d_writeBuffer = (char *)d_allocator_p->allocate(d_bufferSize);
    d_readBuffer  = readBuffer.release();
}

template <class ADDRESS>
bteso_SslLikeStreamSocket<ADDRESS>::~bteso_SslLikeStreamSocket()
{
    d_allocator_p->deallocate(d_readBuffer);
    d_allocator_p->deallocate(d_writeBuffer);
}

// MANIPULATORS
template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::accept(
        btlso::StreamSocket<ADDRESS> **result)
{
    return d_socket_p->accept(result);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::accept(
        btlso::StreamSocket<ADDRESS> **socket,
        ADDRESS                      *peerAddress)
{
    return d_socket_p->accept(socket, peerAddress);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::bind(const ADDRESS& address)
{
    return d_socket_p->bind(address);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::connect(const ADDRESS& address)
{
    return d_socket_p->connect(address);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::listen(int backlog)
{
    return d_socket_p->listen(backlog);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::read(char *buffer, int length)
{
    BSLS_ASSERT(0 < length);

    if (0 == d_readBytesAvailable) {
        int retCode = d_socket_p->read(d_readBuffer, d_bufferSize);
        if (retCode < 0) {
            return retCode;                                           // RETURN
        }
        if (veryVerbose) {
            MTCOUT << "\tSslLikeStreamSocket::read: " << retCode << "bytes."
                   << MTENDL;
        }
        d_readBytesAvailable = retCode;
    }

    if (0 < d_readBytesAvailable) {
        length = bsl::min(length, d_readBytesAvailable);
        bsl::memcpy(buffer, d_readBuffer, length);
        d_readBytesAvailable -= length;
        bsl::memmove(d_readBuffer, d_readBuffer + length,
                     d_readBytesAvailable);
        return length;                                                // RETURN
    }

    return btlso::SocketHandle::e_ERROR_WOULDBLOCK;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::readv(const btls::Iovec *buffers,
                                              int               numBuffers)
{
    BSLS_ASSERT(0 < numBuffers);

    if (0 == d_readBytesAvailable) {
        int retCode = d_socket_p->read(d_readBuffer, d_bufferSize);
        if (retCode < 0) {
            return retCode;                                           // RETURN
        }
        if (veryVerbose) {
            MTCOUT << "\tSslLikeStreamSocket::readv: " << retCode << "bytes."
                   << MTENDL;
        }
        d_readBytesAvailable = retCode;
    }

    if (0 < d_readBytesAvailable) {
        int length = btls::IovecUtil::scatter(buffers, numBuffers,
                                             d_readBuffer,
                                             d_readBytesAvailable);
        BSLS_ASSERT(length < d_readBytesAvailable);
        d_readBytesAvailable -= length;
        bsl::memmove(d_readBuffer, d_readBuffer + length,
                     d_readBytesAvailable);
        return length;                                                // RETURN
    }

    return btlso::SocketHandle::e_ERROR_WOULDBLOCK;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::write(const char *buffer, int length)
{
    BSLS_ASSERT(0 < length);

    if (d_writeOffset == d_bufferSize) {
        int retCode = d_socket_p->write(d_writeBuffer, d_bufferSize);
        if (retCode < 0) {
            return retCode;                                           // RETURN
        }
        if (veryVerbose) {
            MTCOUT << "\tSslLikeStreamSocket::write: " << retCode << "bytes."
                   << MTENDL;
        }
        bsl::memmove(d_writeBuffer, d_writeBuffer + retCode,
                     d_bufferSize - retCode);
        d_writeOffset -= retCode;
    }

    if (d_writeOffset < d_bufferSize) {
        length = bsl::min(length, d_bufferSize - d_writeOffset);
        bsl::memcpy(d_writeBuffer + d_writeOffset, buffer, length);
        d_writeOffset += length;
        return length;                                                // RETURN
    }

    return btlso::SocketHandle::e_ERROR_WOULDBLOCK;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::writev(const btls::Iovec *buffers,
                                               int               numBuffers)
{
    BSLS_ASSERT(0 < numBuffers);

    if (d_writeOffset == d_bufferSize) {
        int retCode = d_socket_p->write(d_writeBuffer, d_bufferSize);
        if (retCode < 0) {
            return retCode;                                           // RETURN
        }
        if (veryVerbose) {
            MTCOUT << "\tSslLikeStreamSocket::writev: " << retCode << "bytes."
                   << MTENDL;
        }
        bsl::memmove(d_writeBuffer, d_writeBuffer + retCode,
                     d_bufferSize - retCode);
        d_writeOffset -= retCode;
    }

    if (d_writeOffset < d_bufferSize) {
        int length = btls::IovecUtil::gather(d_writeBuffer + d_writeOffset,
                                            d_bufferSize - d_writeOffset,
                                            buffers, numBuffers);
        d_writeOffset += length;
        return length;                                                // RETURN
    }

    return btlso::SocketHandle::e_ERROR_WOULDBLOCK;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::writev(const btls::Ovec *buffers,
                                               int              numBuffers)
{
    BSLS_ASSERT(0 < numBuffers);

    if (d_writeOffset == d_bufferSize) {
        int retCode = d_socket_p->write(d_writeBuffer, d_bufferSize);
        if (retCode < 0) {
            return retCode;                                           // RETURN
        }
        if (veryVerbose) {
            MTCOUT << "\tSslLikeStreamSocket::writev: " << retCode << "bytes."
                   << MTENDL;
        }
        bsl::memmove(d_writeBuffer, d_writeBuffer + retCode,
                     d_bufferSize - retCode);
        d_writeOffset -= retCode;
    }

    if (d_writeOffset < d_bufferSize) {
        int length = btls::IovecUtil::gather(d_writeBuffer + d_writeOffset,
                                            d_bufferSize - d_writeOffset,
                                            buffers, numBuffers);
        d_writeOffset += length;
        return length;                                                // RETURN
    }

    return btlso::SocketHandle::e_ERROR_WOULDBLOCK;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::writeFlush()
{
    if (0 == d_writeOffset) {
        return 0;                                                     // RETURN
    }

    int retCode = d_socket_p->write(d_writeBuffer, d_writeOffset);
    if (0 < retCode) {
        if (veryVerbose) {
            MTCOUT << "\tSslLikeStreamSocket::writeFlush: " << retCode
                   << "bytes." << MTENDL;
        }
        bsl::memmove(d_writeBuffer, d_writeBuffer + retCode,
                     d_writeOffset - retCode);
        d_writeOffset -= retCode;
        return retCode;                                               // RETURN
    }

    BSLS_ASSERT(retCode < 0);
    return retCode;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::setBlockingMode(
        btlso::Flag::BlockingMode mode)
{
    return d_socket_p->setBlockingMode(mode);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::shutdown(
        btlso::Flag::ShutdownType streamOption)
{
    return d_socket_p->shutdown(streamOption);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::waitForConnect(
        const bsls::TimeInterval& timeout)
{
    return d_socket_p->waitForConnect(timeout);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::waitForAccept(
        const bsls::TimeInterval& timeout)
{
    return d_socket_p->waitForAccept(timeout);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::waitForIO(
        btlso::Flag::IOWaitType   type,
        const bsls::TimeInterval& timeout)
{
    return d_socket_p->waitForIO(type, timeout);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::waitForIO(
        btlso::Flag::IOWaitType type)
{
    return d_socket_p->waitForIO(type);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::setLingerOption(
        const btlso::SocketOptUtil::LingerData& options)
{
    return d_socket_p->setLingerOption(options);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::setOption(int level,
                                                  int option,
                                                  int value)
{
    return d_socket_p->setOption(level, option, value);
}

// ACCESSORS
template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::blockingMode(
                                       btlso::Flag::BlockingMode *result) const
{
    return d_socket_p->blockingMode(result);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::connectionStatus() const
{
    return d_socket_p->connectionStatus();
}

template <class ADDRESS>
btlso::SocketHandle::Handle bteso_SslLikeStreamSocket<ADDRESS>::handle() const
{
    return d_socket_p->handle();
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::localAddress(ADDRESS *result) const
{
    return d_socket_p->localAddress(result);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::peerAddress(ADDRESS *result) const
{
    return d_socket_p->peerAddress(result);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::lingerOption(
                                btlso::SocketOptUtil::LingerData *result) const
{
    return d_socket_p->lingerOption(result);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::socketOption(int *result,
                                                     int  level,
                                                     int  option) const
{
    return d_socket_p->socketOption(result, level, option);
}

                    // --------------------------------
                    // class SslLikeStreamSocketFactory
                    // --------------------------------

// CREATORS
template <class ADDRESS>
bteso_SslLikeStreamSocketFactory<ADDRESS>::bteso_SslLikeStreamSocketFactory(
        int               internalBufferSize,
        bslma::Allocator *basicAllocator)
: btlso::InetStreamSocketFactory<ADDRESS>(basicAllocator)
, d_bufferSize(internalBufferSize)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class ADDRESS>
bteso_SslLikeStreamSocketFactory<ADDRESS>::~bteso_SslLikeStreamSocketFactory()
{
}

    // MANIPULATORS
template <class ADDRESS>
btlso::StreamSocket<ADDRESS> *
bteso_SslLikeStreamSocketFactory<ADDRESS>::allocate()
{
    btlso::StreamSocket<ADDRESS> *socket =
                           btlso::InetStreamSocketFactory<ADDRESS>::allocate();

    bteso_SslLikeStreamSocket_AutoCloseSocket<ADDRESS>
                                                 deallocateGuard(socket, this);

    bteso_SslLikeStreamSocket<ADDRESS> *sslLikeSocket = new (*d_allocator_p)
                             bteso_SslLikeStreamSocket<ADDRESS>(d_bufferSize,
                                                                socket,
                                                                d_allocator_p);

    deallocateGuard.release();
    return sslLikeSocket;
}

template <class ADDRESS>
btlso::StreamSocket<ADDRESS> *
bteso_SslLikeStreamSocketFactory<ADDRESS>::allocate(
                                            btlso::SocketHandle::Handle handle)
{
    btlso::StreamSocket<ADDRESS> *socket =
                     btlso::InetStreamSocketFactory<ADDRESS>::allocate(handle);

    bteso_SslLikeStreamSocket_AutoCloseSocket<ADDRESS>
                                                 deallocateGuard(socket, this);

    bteso_SslLikeStreamSocket<ADDRESS> *sslLikeSocket = new (*d_allocator_p)
                             bteso_SslLikeStreamSocket<ADDRESS>(d_bufferSize,
                                                                socket,
                                                                d_allocator_p);

    deallocateGuard.release();
    return sslLikeSocket;
}

template <class ADDRESS>
void bteso_SslLikeStreamSocketFactory<ADDRESS>::deallocate(
                                          btlso::StreamSocket<ADDRESS> *socket)
{
    bteso_SslLikeStreamSocket<ADDRESS> *sSocket =
                reinterpret_cast<bteso_SslLikeStreamSocket<ADDRESS> *>(socket);

    btlso::InetStreamSocketFactory<ADDRESS>::deallocate(sSocket->d_socket_p);
    sSocket->~bteso_SslLikeStreamSocket<ADDRESS>();
    d_allocator_p->deallocate(socket);
}

template <class ADDRESS>
void bteso_SslLikeStreamSocketFactory<ADDRESS>::deallocate(
        btlso::StreamSocket<ADDRESS> *socket, bool closeHandleFlag)
{
    bteso_SslLikeStreamSocket<ADDRESS> *sSocket =
                reinterpret_cast<bteso_SslLikeStreamSocket<ADDRESS> *>(socket);

    if (closeHandleFlag) {
      btlso::InetStreamSocketFactory<ADDRESS>::deallocate(sSocket->d_socket_p);
    }
    sSocket->~bteso_SslLikeStreamSocket<ADDRESS>();
    d_allocator_p->deallocate(socket);
}

}  // close namespace TEST_CASE_SSL_SOCKETS

//-----------------------------------------------------------------------------
//                          TEST_CASE_IMPORT function helpers
//-----------------------------------------------------------------------------
namespace TEST_CASE_IMPORT {

static
void caseImportDataCallback(int        *numNeeded,
                            btlb::Blob *msg,
                            int         ,
                            void       *)
{
    btlb::BlobUtil::erase(msg, 0, msg->length());
    *numNeeded = 1;

    // bslmt::ThreadUtil::microSleep(1000);  // 1ms
}

static
void caseImportChannelStateCallback(int                  channelId,
                                    int                  serverId,
                                    int                  state,
                                    void                *,
                                    btlmt::ChannelPool **poolAddr,
                                    int                **eventAddr,
                                    bslmt::Barrier      *barrier,
                                    int                 *channelId_p)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btlmt::ChannelPool *pool = *poolAddr;

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
      } break;
      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="    << serverId
                   << " channelId="   << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        *channelId_p = channelId;
        barrier->wait();
      } break;
    }
}

}  // close namespace TEST_CASE_IMPORT

//-----------------------------------------------------------------------------
//                  TEST_CASE_ACCEPT supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_ACCEPT {

static
void caseAcceptPoolStateCallback(int                state,
                                 int                serverId,
                                 int                severity,
                                 bsls::AtomicInt64 *acceptErrors)
{
    ASSERT(acceptErrors);

    switch (state) {
      case PoolState::BTEMT_ERROR_ACCEPTING: {
        if (veryVerbose) {
            MTCOUT << "Error Accepting:"
                   << " serverId=" << serverId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
        ++*acceptErrors;
      } break;
    }
}

}  // close namespace TEST_CASE_ACCEPT

//-----------------------------------------------------------------------------
//                  TEST_CASE_MAX_CONNS_LIMIT supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_MAX_CONNS_LIMIT {

static
void caseMaxConnsChannelStateCallback(int                  channelId,
                                      int                  serverId,
                                      int                  state,
                                      void                *,
                                      btlmt::ChannelPool **poolAddr,
                                      int                **eventAddr,
                                      bslmt::Barrier      *barrier)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btlmt::ChannelPool *pool = *poolAddr;

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        pool->shutdown(channelId, btlmt::ChannelPool::e_IMMEDIATE);
        **eventAddr = -1;
        barrier->wait();
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="    << serverId
                   << " channelId="   << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        barrier->wait();
      } break;
    }
}

static
void caseMaxConnsPoolStateCallback(int                state,
                                   int                serverId,
                                   int                severity,
                                   int              **eventAddr,
                                   bsls::AtomicInt   *limitReachedFlag)
{
    ASSERT(eventAddr && *eventAddr);
    ASSERT(limitReachedFlag);

    **eventAddr = state;

    switch (state) {
      case PoolState::BTEMT_ACCEPT_TIMEOUT: {
        if (veryVerbose) {
            MTCOUT << "Accept timed out:"
                   << " serverId=" << serverId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case PoolState::BTEMT_ERROR_CONNECTING: {
        if (veryVerbose) {
            MTCOUT << "Error Connecting:"
                   << " serverId=" << serverId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case PoolState::BTEMT_ERROR_ACCEPTING: {
        if (veryVerbose) {
            MTCOUT << "Error Accepting:"
                   << " serverId=" << serverId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case PoolState::BTEMT_CHANNEL_LIMIT: {
        if (veryVerbose) {
            MTCOUT << "Channel Limit Reached:"
                   << " serverId=" << serverId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
        *limitReachedFlag = true;
      } break;
    }
}

}  // close namespace TEST_CASE_MAX_CONNS_LIMIT

//-----------------------------------------------------------------------------
//                  TEST_CASE_CALLBACKS_TEST supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_CALLBACKS_TEST {

static
void caseCallbacksChannelStateCallback(int             channelId,
                                       int             serverId,
                                       int             state,
                                       void           *,
                                       int            *value,
                                       bslmt::Barrier *barrier)
{
    ASSERT(value);
    ASSERT(barrier);

    switch(state){
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        ASSERT(channelId == *value);
        ASSERT(0x701d50da == (unsigned)serverId);
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        barrier->wait();
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        ASSERT(0 == *value);
        ASSERT(0x701d50da == (unsigned)serverId);
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        *value = channelId;
        barrier->wait();
      } break;
    }
}

}  // close namespace TEST_CASE_CALLBACKS_TEST

//-----------------------------------------------------------------------------
//                  TEST_CASE_SHUTDOWN_IN_USER_CALLBACK classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_SHUTDOWN_IN_USER_CALLBACK {

static
void caseShutdownChannelStateCallback(int                  channelId,
                                      int                  serverId,
                                      int                  state,
                                      void                *,
                                      int                  expServerId,
                                      btlmt::ChannelPool **poolAddr,
                                      int                 *value,
                                      bslmt::Barrier      *barrier)
{
    ASSERT(poolAddr && *poolAddr);
    ASSERT(barrier);

    ASSERT(expServerId == serverId);
    switch(state){
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        ASSERT(channelId == *value);
        ASSERT(0x701d50da == (unsigned)serverId);
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        barrier->wait();
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        ASSERT(0 == *value);
        ASSERT(0x701d50da == (unsigned)serverId);
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        *value = channelId;
        (*poolAddr)->shutdown(channelId, btlmt::ChannelPool::e_IMMEDIATE);
      } break;
    }
}

}  // close namespace TEST_CASE_SHUTDOWN_IN_USER_CALLBACK

//-----------------------------------------------------------------------------
//                  CASE 15 supporting classes and methods
//-----------------------------------------------------------------------------

bsl::ostream&
operator<<(bsl::ostream& s,
           const bsl::vector<btlmt::ChannelPool::HandleInfo>& handles)
{
    s << "{";
    for (int i = 0; i < (int) handles.size(); ++i) {
        if (i) s << ",";
        s << " { "
          << handles[i].d_handle << ", "
          << handles[i].d_channelType << ", "
          << handles[i].d_channelId << ", "
//          << handles[i].d_threadHandle << ", "
          << handles[i].d_userId
          << " }";
    }
    s << " }";
    return s;
}

namespace TEST_CASE_GET_HANDLE_STATS {

enum {
    SERVER_ID            = 0x701d50da,
    CLIENT_ID1           = 0x12345678,
    CLIENT_ID2           = 0x23456789
};

struct LessThanByType {
    typedef btlmt::ChannelPool::HandleInfo first_argument_type;
    typedef btlmt::ChannelPool::HandleInfo second_argument_type;
    typedef bool result_type;

    result_type operator()(const first_argument_type&  lhs,
                           const second_argument_type& rhs) const {
        return lhs.d_channelType < rhs.d_channelType;
    }
};

static
void caseHandleStatsChannelStateCallback(int   channelId,
                                int   sourceId,
                                int   state,
                                void *,
                                int  *acceptedChannelId1,
                                int  *channelId1,
                                int  *channelId2,
                                int   veryVerbose)
{
    ASSERT(acceptedChannelId1);
    ASSERT(channelId1);
    ASSERT(channelId2);

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << sourceId
                   << " channelId=" << channelId
                   << MTENDL;
        }
        switch (sourceId) {
          case SERVER_ID: {
            *acceptedChannelId1 = -1;
          } break;
          case CLIENT_ID1: {
            *channelId1 = -1;
          } break;
          case CLIENT_ID2: {
            *channelId2 = -1;
          } break;
          default: ASSERT(0);
        }
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="    << sourceId
                   << " channelId="   << channelId
                   << MTENDL;
        }
        switch (sourceId) {
          case SERVER_ID: {
            *acceptedChannelId1 = channelId;
          } break;
          case CLIENT_ID1: {
            *channelId1 = channelId;
          } break;
          case CLIENT_ID2: {
            *channelId2 = channelId;
          } break;
          default: ASSERT(0);
        }
      } break;
    }
}

}  // close namespace TEST_CASE_GET_HANDLE_STATS

//-----------------------------------------------------------------------------
//                  TEST_CASE_GET_CHANNEL_STATS supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_GET_CHANNEL_STATS {

static
void caseChannelStatsChannelDataCallback(int               *numNeeded,
                               btlb::Blob        *msg,
                               int                channelId,
                               void              *,
                               Obj              **channelPool_p,
                               bsls::AtomicInt   *fail)
{
    ASSERT(numNeeded);
    ASSERT(msg);
    ASSERT(0 < msg->length());

    if (0 == (*channelPool_p)->write(channelId, *msg)) {
        *fail = 1;
    }

    btlb::BlobUtil::erase(msg, 0, msg->length());
    *numNeeded = 1;
}

static
void caseChannelStatsChannelStateCallback(int                  channelId,
                                int                  serverId,
                                int                  state,
                                void                *,
                                btlmt::ChannelPool **poolAddr,
                                int                **eventAddr,
                                bslmt::Barrier      *barrier,
                                int                 *channelId_p)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btlmt::ChannelPool *pool = *poolAddr;

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        barrier->wait();
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="    << serverId
                   << " channelId="   << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        *channelId_p = channelId;
        barrier->wait();
      } break;
    }
}

}  // close namespace TEST_CASE_GET_CHANNEL_STATS

//-----------------------------------------------------------------------------
//                  TEST_CASE_REPORT_AVG_RESET supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_REPORT_AVG_RESET {

static
void caseAvgResetChannelStateCallback(int                  channelId,
                                      int                  serverId,
                                      int                  state,
                                      void                *,
                                      btlmt::ChannelPool **poolAddr,
                                      int                **eventAddr,
                                      bslmt::Barrier      *barrier)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btlmt::ChannelPool *pool = *poolAddr;

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        barrier->wait();
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="    << serverId
                   << " channelId="   << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        barrier->wait();
      } break;
    }
}

}  // close namespace TEST_CASE_REPORT_AVG_RESET

//-----------------------------------------------------------------------------
//                  TEST_CASE_FLOW_CONTROL supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_FLOW_CONTROL {

static
void caseFlowControlChannelCallback(
                      int                                      channelId,
                      int                                      sourceId,
                      int                                      state,
                      void                                    *,
                      bslmt::Barrier                          *barrier,
                      int                                     *numBytesWritten,
                      const btlmt::ChannelPoolConfiguration&   ,
                      btlb::PooledBlobBufferFactory           *factory,
                      btlmt::ChannelPool                     **instance)
{
    ASSERT(barrier);
    ASSERT(numBytesWritten);
    ASSERT(factory);
    ASSERT(instance);

    switch(state){
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << sourceId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        (*instance)->shutdown(channelId, btlmt::ChannelPool::e_IMMEDIATE);
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << sourceId << bsl::dec
                   << " channelId=" << channelId
                << MTENDL;
        }



        btlb::Blob blob(factory);
        blob.setLength(factory->bufferSize());
        bsl::memset(blob.buffer(0).data(), 0xab, factory->bufferSize());
        while ((*instance)->write(channelId, blob) == 0) {
            *numBytesWritten += blob.length();
        }
        if (verbose) {
            P(*numBytesWritten);
        }
        barrier->wait();
      } break;
      case btlmt::ChannelPool::e_WRITE_CACHE_HIWAT: {
        if (verbose) {
            MTCOUT << "HIWAT " << MTENDL;
        }
      } break;
      case btlmt::ChannelPool::e_WRITE_CACHE_LOWWAT: {
        if (verbose) {
            MTCOUT << "LOWWAT " << MTENDL;
        }
      } break;
    }
}

}  // close namespace TEST_CASE_FLOW_CONTROL

//-----------------------------------------------------------------------------
//                  TEST_CASE_ENABLE_DISABLE_CHANNELS classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_ENABLE_DISABLE_CHANNELS {

static
void runTestCaseEnableDisable(
                       btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                       bslma::TestAllocator&                           ta)
{
    enum {
        SERVER_ID   = 1013410001,
        BACKLOG     = 1
    };
    const int NUM_SOCKETS = 5;  // an enum would confuse bsl::fill_n below

    bsl::vector<my_ChannelEvent> channelEvents;
    bslmt::Mutex                 channelEventsMutex;
    bsl::vector<my_PoolEvent>    poolEvents;
    bslmt::Mutex                 poolEventsMutex;

    btlmt::ChannelPoolConfiguration config;
    config.setMaxThreads(4);
    config.setMetricsInterval(10.0);
    config.setMaxConnections(NUM_SOCKETS);
    config.setIncomingMessageSizes(1, 1, 1);
    config.setReadTimeout(0);

    btlmt::ChannelPool::BlobBasedReadCallback         dataCb;
    makeNull(&dataCb);

    btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
            bdlf::BindUtil::bind( recordChannelState
                                , _1, _2, _3, _4
                                , &channelEvents
                                , &channelEventsMutex));

    btlmt::ChannelPool::PoolStateChangeCallback    poolCb(
            bdlf::BindUtil::bind( recordPoolState
                                , _1, _2, _3
                                , &poolEvents
                                , &poolEventsMutex));

    btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
    if (verbose) {
        P(config);
    }
    ASSERT(0 == mX.start());

    makeNull(&channelCb);
    makeNull(&poolCb);

    // Establish server
    btlmt::ChannelPool mY(channelCb, dataCb, poolCb, config, &ta);
    mY.start();

    const btlso::IPv4Address ADDRESS("127.0.0.1", 0);
    ASSERT(0 == mY.listen(ADDRESS, BACKLOG, SERVER_ID));
    btlso::IPv4Address PEER = getServerLocalAddress(&mY, SERVER_ID);

    // Establish clients and connect them to server
    for (int i = 0; i < NUM_SOCKETS; ++i) {
        if (0 == i % 2) {
            btlso::StreamSocket<btlso::IPv4Address> *socket =
                factory->allocate();
            ASSERT(0 == socket->connect(PEER));

            typedef btlso::StreamSocketFactoryDeleter Deleter;

            bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                     managedSocket(socket,
                                   factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

            mX.import(&managedSocket, i);
        } else {
            // for some of the sockets, provide a ManagedPtr to exercise
            // the ManagedPtr-based import() function
            typedef btlso::StreamSocketFactoryDeleter Deleter;

            bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                socket(factory->allocate(),
                       factory,
                       &Deleter::deleteObject<btlso::IPv4Address>);

            ASSERT(0 == socket->connect(PEER));
            mX.import(&socket, i);
        }
    }

    while (1) {
        bslmt::LockGuard<bslmt::Mutex> lock(&channelEventsMutex);
        const int NUM_EVENTS = (int) channelEvents.size();
        if (NUM_EVENTS / 2 >= NUM_SOCKETS) {
            break;
        }
    }

    ASSERT(NUM_SOCKETS     == mX.numChannels());
    ASSERT(2 * NUM_SOCKETS == channelEvents.size());

    int numEvents = channelEvents.size();
    if (veryVerbose) { P(numEvents); PV(channelEvents); }

    int importedFlag[NUM_SOCKETS];
    bsl::fill_n((int*)importedFlag, NUM_SOCKETS, 0);

    for (int i = 0; i < numEvents; ++i) {
        LOOP_ASSERT(i, 0 <= channelEvents[i].d_allocatorId);
        if (veryVerbose) { P(channelEvents[i].d_allocatorId); }

        LOOP_ASSERT(i, ChannelState::BTEMT_CHANNEL_UP ==
                                             channelEvents[i].d_event ||
                       ChannelState::BTEMT_AUTO_READ_ENABLED ==
                                             channelEvents[i].d_event);
        LOOP_ASSERT(i, NUM_SOCKETS > channelEvents[i].d_allocatorId);

        if (channelEvents[i].d_event ==
                                     ChannelState::BTEMT_AUTO_READ_ENABLED)
        {
            LOOP_ASSERT(i, 1 ==
                          importedFlag[channelEvents[i].d_allocatorId]);
            importedFlag[channelEvents[i].d_allocatorId] = 3;
        }
        else {
            LOOP_ASSERT(i, 0 ==
                          importedFlag[channelEvents[i].d_allocatorId]);
            importedFlag[channelEvents[i].d_allocatorId] = 1;
        }
    }

    for (int i = 0; i < NUM_SOCKETS; ++i) {
        LOOP2_ASSERT(i, importedFlag[i], 3 == importedFlag[i]);
    }

    bsl::fill_n(importedFlag, NUM_SOCKETS, 0);
    bsl::vector<my_ChannelEvent> backup;

    for (int i = 0; i < (int)channelEvents.size(); ++i) {
        if (ChannelState::BTEMT_AUTO_READ_ENABLED !=
                                               channelEvents[i].d_event)
        {
            backup.push_back(channelEvents[i]);
        }
    }

    if (verbose)
        cout << "\tDisabling channels." << endl;
    {
        channelEvents.clear();
        for (int i = 0; i < (int)backup.size(); ++i) {
            if (veryVerbose) { P(backup[i].d_channelId); }
            LOOP_ASSERT(i, 0 == mX.disableRead(backup[i].d_channelId));
        }

        while (1) {
            bslmt::LockGuard<bslmt::Mutex> lock(&channelEventsMutex);
            const int NUM_EVENTS = (int) channelEvents.size();
            if (NUM_EVENTS >= NUM_SOCKETS) {
                break;
            }
        }

        numEvents = channelEvents.size();
        if (veryVerbose) {
            P(numEvents);
            PV(channelEvents);
        }

        bsl::fill_n(importedFlag, NUM_SOCKETS, 0);
        for (int i = 0; i < numEvents; ++i) {
            LOOP_ASSERT(i, 0 <= channelEvents[i].d_allocatorId);
            if (veryVerbose) {
                P(channelEvents[i].d_allocatorId);
            }
            LOOP_ASSERT(i, ChannelState::BTEMT_AUTO_READ_DISABLED ==
                                              channelEvents[i].d_event);
            LOOP_ASSERT(i, NUM_SOCKETS >
                                        channelEvents[i].d_allocatorId);
            importedFlag[channelEvents[i].d_allocatorId] = 1;
        }

        LOOP2_ASSERT(NUM_SOCKETS, numEvents, NUM_SOCKETS == numEvents);
        for (int i = 0; i < numEvents; ++i) {
            LOOP_ASSERT(i, 1 == importedFlag[i]);
        }
    }

    if (verbose)
        cout << "\tEnabling channels." << endl;
    {
        channelEvents.clear();
        for (int i = 0; i < (int)backup.size(); ++i) {
            if (veryVerbose) {
                P(backup[i].d_channelId);
            }
            LOOP_ASSERT(i, 0 == mX.enableRead(backup[i].d_channelId));
        }

        while (1) {
            bslmt::LockGuard<bslmt::Mutex> lock(&channelEventsMutex);
            const int NUM_EVENTS = (int) channelEvents.size();
            if (NUM_EVENTS >= NUM_SOCKETS) {
                break;
            }
        }

        numEvents = channelEvents.size();
        if (veryVerbose) {
            P(numEvents);
            PV(channelEvents);
        }

        bsl::fill_n(importedFlag, NUM_SOCKETS, 0);
        for (int i = 0; i < numEvents; ++i) {
            LOOP_ASSERT(i, 0 <= channelEvents[i].d_allocatorId);
            LOOP_ASSERT(i, ChannelState::BTEMT_AUTO_READ_ENABLED ==
                                              channelEvents[i].d_event);
            LOOP_ASSERT(i, NUM_SOCKETS >
                                        channelEvents[i].d_allocatorId);
            importedFlag[channelEvents[i].d_allocatorId] = 1;
        }

        LOOP2_ASSERT(NUM_SOCKETS, numEvents, NUM_SOCKETS == numEvents);
        for (int i = 0; i < numEvents; ++i) {
            LOOP_ASSERT(i, 1 == importedFlag[i]);
        }
    }

    for (int i = 0; i < (int)backup.size(); ++i) {
        mX.shutdown(backup[i].d_channelId,
                    btlmt::ChannelPool::e_IMMEDIATE);
    }
    mX.stop();

    mY.close(SERVER_ID);
    mY.stop();
}

}  // close namespace TEST_CASE_ENABLE_DISABLE_CHANNELS

//-----------------------------------------------------------------------------
//                  TEST_CASE_CLOCK_FUNCTIONS supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_CLOCK_FUNCTIONS {

const ThreadId DONT_CHECK_THREAD_ID = NULL_THREAD_ID;

struct my_ClockState {
    bslmt::ThreadUtil::Handle d_threadHandle;
    bsls::TimeInterval        d_startTime;
    bsls::TimeInterval        d_timeout;
    int                       d_numInvocations;
    int                       d_maxNumInvocations;
};

static
void caseMyClockCallback(my_ClockState      *state,
                         int                 clockId,
                         btlmt::ChannelPool *pool,
                         int                 deregister,
                         ThreadId            checkThreadId)
{
    bsls::TimeInterval now = bdlt::CurrentTime::now();
    if (veryVerbose) {
        Q("================================");
        P_(clockId);
        P_(now); P(state->d_startTime);
        P(state->d_timeout);
        P_(state->d_numInvocations);
        P(state->d_maxNumInvocations);
    }

    if (0 == state->d_numInvocations) {
        ASSERT(state->d_startTime < now);
    }
    else {
        // 'd_startTime' represents the last invocation.

        ASSERT(state->d_timeout > now - state->d_startTime);
    }
    ++state->d_numInvocations;

    if (DONT_CHECK_THREAD_ID != checkThreadId) {
        ASSERT(checkThreadId == bslmt::ThreadUtil::selfIdAsUint64());
    }

    state->d_startTime += state->d_timeout;
    if (deregister && state->d_numInvocations == state->d_maxNumInvocations) {
        if (veryVerbose) {
            cout << "Deregistering " << clockId << endl;
        }
        pool->deregisterClock(clockId);
    }
}

static
void caseChannelStateCallback(int                  channelId,
                              int                  sourceId,
                              int                  state,
                              void                *,
                              int                 *channelId1,
                              bsls::Types::Uint64 *threadId1,
                              int                 *channelId2,
                              bsls::Types::Uint64 *threadId2,
                              int                  veryVerbose)
{
    ASSERT(channelId1 && threadId1);
    ASSERT(channelId2 && threadId2);

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << sourceId
                   << " channelId=" << channelId
                   << MTENDL;
        }
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="    << sourceId
                   << " channelId="   << channelId
                   << MTENDL;
        }
        if (1 == sourceId) {
            *channelId1 = channelId;
            *threadId1  = bslmt::ThreadUtil::selfIdAsUint64();
        }
        else {
            *channelId2 = channelId;
            *threadId2  = bslmt::ThreadUtil::selfIdAsUint64();
        }
      } break;
    }
}

}  // close namespace TEST_CASE_CLOCK_FUNCTIONS

//-----------------------------------------------------------------------------
//                  TEST_CASE_WRITE supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_WRITE {

struct caseThreadInfo {
    bslmt::Barrier   *d_barrier;
    bsls::AtomicInt  *d_threadCount;
    Obj              *d_channelPool_p;
    int               d_channelId;
    int               d_write;
    int               d_bufferAlloc;
    int               d_totalAlloc;
    bslma::Allocator *d_allocator_p;
};

struct caseThreadInfo2 {
    bslmt::Barrier                  *d_barrier;
    bsls::AtomicInt                 *d_threadCount;
    Obj                             *d_channelPool_p;
    int                              d_channelId;
    int                              d_write;
    int                              d_bufferAlloc;
    int                              d_totalAlloc;
    bslma::Allocator                *d_allocator_p;
    btlb::PooledBlobBufferFactory   *d_factory_p;
};

extern "C" {
void *caseThread0(void *arg)
{
    caseThreadInfo2 *info = (caseThreadInfo2 *) arg;
    const int BUF_SIZE = info->d_bufferAlloc;
    const int TOTAL_SIZE = info->d_totalAlloc;

    // Set up buffer to write to channel pool, of total size
    // TOTAL_SIZE, filled with the requested value.

    char *buffer = (char *) info->d_allocator_p->allocate(TOTAL_SIZE);
    bsl::memset(buffer, info->d_write, TOTAL_SIZE);
    bsl::shared_ptr<char> sharedBuffer(buffer,
                                       info->d_allocator_p);

    // Be careful to round numVecs upward and compute lastVec size correctly.

    const int numVecs = (TOTAL_SIZE + BUF_SIZE - 1) / BUF_SIZE;
    const int LAST_VEC_SIZE = (0 < TOTAL_SIZE % BUF_SIZE)
                            ? TOTAL_SIZE % BUF_SIZE
                            : BUF_SIZE;  // size of the last buffer

    // Segment buffer into btlb::Blob of individual buffers of BUF_SIZE.

    btlb::Blob blob(info->d_allocator_p);
    for (int i = 0 ;i < numVecs; ++i) {
        bsl::shared_ptr<char> buf(sharedBuffer, buffer + i * BUF_SIZE);
        blob.appendBuffer(btlb::BlobBuffer(buf,
                                           (i < numVecs-1) ? BUF_SIZE
                                                           : LAST_VEC_SIZE));
    }
    blob.setLength(TOTAL_SIZE);

    // Wait until all threads have initialized their buffers.

    info->d_barrier->wait();

    // Now write in a short burst, until write succeeds.

    while (0 != info->d_channelPool_p->write(info->d_channelId, blob)) {
        bslmt::ThreadUtil::yield();
    }

    // Signal effective termination to write.

    ++(*(info->d_threadCount));
    info->d_barrier->wait();

    return NULL;
}
}

extern "C" {
void *caseThread2(void *arg)
{
    caseThreadInfo *info = (caseThreadInfo *) arg;
    const int BUF_SIZE = info->d_bufferAlloc;
    const int TOTAL_SIZE = info->d_totalAlloc;

    // Set up buffer to write to channel pool, of total size
    // info->d_bufferAlloc, filled with the requested value.

    char *buffer = (char *) info->d_allocator_p->allocate(TOTAL_SIZE);
    bsl::memset(buffer, info->d_write, TOTAL_SIZE);

    // Be careful to round numVecs upward and compute lastVec size correctly.

    const int numVecs = (TOTAL_SIZE + BUF_SIZE - 1) / BUF_SIZE;
    const int LAST_VEC_SIZE = (0 < TOTAL_SIZE % BUF_SIZE)
                            ? TOTAL_SIZE % BUF_SIZE
                            : BUF_SIZE;  // size of the last buffer

    // Segment buffer into Iovecs of individual buffers of BUF_SIZE.

    btls::Iovec *vecs = (btls::Iovec *) info->d_allocator_p->allocate(numVecs *
                                                           sizeof(btls::Ovec));
    for (int i = 0; i < numVecs; ++i) {
        vecs[i].setBuffer(buffer + i * BUF_SIZE,
                          (i < numVecs-1) ? BUF_SIZE : LAST_VEC_SIZE);
    }

    // Wait until all threads have initialized their buffers.

    info->d_barrier->wait();

    // Now write in a short burst, until write succeeds.

    while (0 != info->d_channelPool_p->write(info->d_channelId,
                                             vecs, numVecs)) {
        bslmt::ThreadUtil::yield();
    }

    // Signal effective termination to write.

    ++(*(info->d_threadCount));
    info->d_barrier->wait();

    info->d_allocator_p->deallocate(vecs);
    info->d_allocator_p->deallocate(buffer);
    return NULL;
}
}

extern "C" {

void *caseThread3(void *arg)
{
    caseThreadInfo2 *info = (caseThreadInfo2 *) arg;
    const int BUF_SIZE = info->d_bufferAlloc;
    const int TOTAL_SIZE = info->d_totalAlloc;

    // Set up buffer to write to channel pool, of total size
    // info->d_bufferAlloc, filled with the requested value.

    char *buffer = (char *) info->d_allocator_p->allocate(TOTAL_SIZE);
    bsl::memset(buffer, info->d_write, TOTAL_SIZE);
    bsl::shared_ptr<char> sharedBuffer;
    if (0 == info->d_write % 2) {
        sharedBuffer.reset(buffer, info->d_allocator_p);
    }

    // Be careful to round numVecs upward and compute lastVec size correctly.

    const int numVecs = (TOTAL_SIZE + BUF_SIZE - 1) / BUF_SIZE;
    const int LAST_VEC_SIZE = (0 < TOTAL_SIZE % BUF_SIZE)
                            ? TOTAL_SIZE % BUF_SIZE
                            : BUF_SIZE;  // size of the last buffer

    btlb::Blob blob(info->d_allocator_p);
    btlb::Blob chain(info->d_allocator_p);

    btls::Iovec *vecs = (btls::Iovec *) info->d_allocator_p->allocate(
                                                numVecs * sizeof(btls::Iovec));

    for (int i = 0 ;i < numVecs; ++i) {
        switch (info->d_write % 2) {
          case 0: {
            bsl::shared_ptr<char> buf(sharedBuffer, buffer + i * BUF_SIZE);
            blob.appendBuffer(btlb::BlobBuffer(buf,
                                               (i < numVecs-1) ? BUF_SIZE
                                                             : LAST_VEC_SIZE));
          } break;
          case 1: {
            vecs[i].setBuffer(buffer + i * BUF_SIZE,
                              (i < numVecs-1) ? BUF_SIZE : LAST_VEC_SIZE);
          } break;
          default: ASSERT(0 && "Unreachable");
        }
    }
    switch (info->d_write % 2) {
      case 0: {
        blob.setLength(TOTAL_SIZE);
      } break;
      case 1: {
      } break;
      default: ASSERT(0 && "Unreachable");
    }

    // Wait until all threads have initialized their buffers.

    info->d_barrier->wait();

    // Now write in a short burst, until write succeeds.

    switch (info->d_write % 2) {
      case 0: {
        while (0 != info->d_channelPool_p->write(info->d_channelId, blob)) {
            bslmt::ThreadUtil::yield();
        }
      } break;
      case 1: {
        while (0 != info->d_channelPool_p->write(info->d_channelId,
                                                 vecs, numVecs)) {
            bslmt::ThreadUtil::yield();
        }
      } break;
      default: ASSERT(0 && "Unreachable");
    }

    // Signal effective termination to write.

    ++(*(info->d_threadCount));
    info->d_barrier->wait();

    info->d_allocator_p->deallocate(vecs);
    if (0 != info->d_write % 2) {
        info->d_allocator_p->deallocate(buffer);
    }
    return NULL;
}

}

struct caseReadInfo {
    btlsos::TcpChannel *d_channel_p;
    int                 d_concurrentReadFlag;
    int                 d_numThreads;
    bsls::AtomicInt    *d_threadCount;
    int                 d_bufferAlloc;
    int                 d_totalAlloc;
    bslma::Allocator   *d_allocator_p;
};

extern "C" {
void *caseRead(void *arg)
{
    caseReadInfo *info = (caseReadInfo*) arg;
    const int NUM_THREADS = info->d_numThreads;
#ifdef BSLS_PLATFORM_OS_AIX
    // AIX is doing something very very weird.  The following
    // read() calls block even if the client's buffer is full or has
    // more data than we request.  Reading very small chunks works
    // but it is slow.
    const int BUF_SIZE = bsl::min(1024, info->d_bufferAlloc);
#else
    const int BUF_SIZE = info->d_bufferAlloc;
#endif
    const int TOTAL_SIZE = info->d_totalAlloc;

    // If specified, wait until all writes have completed
    if (!info->d_concurrentReadFlag) {
        while (*(info->d_threadCount) < NUM_THREADS) {
            bslmt::ThreadUtil::yield();
        }
    }

    // Then read from channel and assert that the structure is as expected.
    char *buffer = (char *)info->d_allocator_p->allocate(BUF_SIZE);
    for (int i = 0; i < NUM_THREADS; ++i) {
        bool firstFlag = true;
        int  total     = 0;
        char cur       = 0;

        while (total != TOTAL_SIZE) {
            int bytesToRead = bsl::min(BUF_SIZE, TOTAL_SIZE - total);
            int read = info->d_channel_p->read(buffer, bytesToRead);
            ASSERT(read > 0);
            total += read;
            if (firstFlag) {
                cur = *buffer;
                firstFlag = false;
                int firstByteToDiffer = 1;
                for ( ; firstByteToDiffer < read; ++firstByteToDiffer) {
                    if (buffer[firstByteToDiffer] != cur) {
                        break;
                    }
                }
                LOOP4_ASSERT(i, total, read, firstByteToDiffer,
                             firstByteToDiffer == read);
                if (firstByteToDiffer < read) {
                    if (verbose) {
                        bdlb::Print::hexDump(bsl::cout, buffer, read);
                    }
                    cur = buffer[firstByteToDiffer];
                }
            }
            else {
                int firstByteToDiffer = 0;
                for ( ; firstByteToDiffer < read; ++firstByteToDiffer) {
                    if (buffer[firstByteToDiffer] != cur) {
                        break;
                    }
                }
                LOOP4_ASSERT(i, total, read, firstByteToDiffer,
                             firstByteToDiffer == read);
                if (firstByteToDiffer < read) {
                    if (verbose) {
                        bdlb::Print::hexDump(bsl::cout, buffer, read);
                    }
                    cur = buffer[firstByteToDiffer];
                }
            }
        }
    }

    info->d_allocator_p->deallocate(buffer);
    return NULL;
}
}

static
void caseChannelStateCallback(int                  channelId,
                               int                  serverId,
                               int                  state,
                               void                *,
                               btlmt::ChannelPool **poolAddr,
                               int                **eventAddr,
                               bslmt::Barrier      *barrier,
                               int                 *channelId_p)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btlmt::ChannelPool *pool = *poolAddr;

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        barrier->wait();
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="    << serverId
                   << " channelId="   << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        *channelId_p = channelId;
        barrier->wait();
      } break;
    }
}

static
void runTestCase(char                                           *,
                  btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                  bslma::TestAllocator&                           ta)
{
    using namespace TEST_CASE_SSL_SOCKETS; // for dynamic_cast(SslLikeSocket)
    typedef bteso_SslLikeStreamSocketFactory<btlso::IPv4Address>
                                                          SslLikeSocketFactory;
    const int IS_SSL = (dynamic_cast<SslLikeSocketFactory*>(factory) != 0);

    const struct {
        int d_line;
        int d_concurrentReadFlag;
        int d_bufferAlloc;
        int d_bigBufferAlloc;
        int d_smallBufferAlloc;
    } DATA[] = {
        // LINE READ  BUF_SIZE  BIG_TOTAL_SIZE     SMALL_TOTAL_SIZE

        // Control - makes sure everythings goes well with direct writes.
        { L_,   0,    1024,     1024,              1024           },
#ifndef BSLS_PLATFORM_OS_AIX
        // Bad AIX seems to have problems read and writing from the same
        // process, takes forever.  Works eventually, but takes several
        // seconds for case above!!!
        { L_,   0,    1024,     1024 - 128,        1024 - 128     },
        { L_,   0,    1024,     1024 - 128,        1024           },
        { L_,   0,    1024,     1024,              1024 - 128     },
        { L_,   0,    1024,     1024 + 128,        1024           },
        { L_,   0,    1024,     1024,              1024 + 128     },
        { L_,   0,    1024,     1024 + 128,        1024 + 128     },
#endif

        // On most systems, BTEMT_MAX_IOVEC_SIZE is 16, so we make sure we
        // exercise the callback inside btlmt::Channel::writeMessage or
        // btlmt::Channel::writeVecMessage.
#ifndef BSLS_PLATFORM_OS_AIX
        { L_,   0,    1024,     1024 * 2,          1024           },
        { L_,   0,    1024,     1024 * 4,          1024           },
        { L_,   0,    1024,     1024 * 8,          1024           },
        { L_,   0,    1024,     1024 * 15,         1024           },
#endif
        { L_,   0,    1024,     1024 * 17,         1024           },
#ifndef BSLS_PLATFORM_OS_AIX
        { L_,   0,    1024,     1024 * 18,         1024           },
        { L_,   0,    1024,     1024 * 31,         1024           },
        { L_,   0,    1024,     1024 * 32,         1024           },
        { L_,   0,    1024,     1024 * 33,         1024           },
#endif

        // These tests are to make sure buffer boundaries are treated
        // properly, by writing *non* multiple of buffer sizes, both below
        // and above the multiples.
        // These cases were disabled for the truss traces, but are enabled
        // for the nightly builds.
        { L_,   0,    1024,     1024 * 17 - 128,   1024 - 128     },
#ifndef BSLS_PLATFORM_OS_AIX
        { L_,   0,    1024,     1024 * 17 - 128,   1024           },
        { L_,   0,    1024,     1024 * 17,         1024 - 128     },
        { L_,   0,    1024,     1024 * 17 + 128,   1024           },
        { L_,   0,    1024,     1024 * 17,         1024 + 128     },
#endif
        { L_,   0,    1024,     1024 * 17 + 128,   1024 + 128     },

#ifndef BSLS_PLATFORM_OS_AIX
        { L_,   0,    1024,     1024 * 31 - 128,   1024 - 128     },
        { L_,   0,    1024,     1024 * 31 - 128,   1024           },
        { L_,   0,    1024,     1024 * 31,         1024 - 128     },
        { L_,   0,    1024,     1024 * 31 + 128,   1024           },
        { L_,   0,    1024,     1024 * 31,         1024 + 128     },
        { L_,   0,    1024,     1024 * 31 + 128,   1024 + 128     },

        { L_,   0,    1024,     1024 * 32 - 128,   1024 - 128     },
        { L_,   0,    1024,     1024 * 32,         1024 - 128     },
        { L_,   0,    1024,     1024 * 32 - 128,   1024           },
        { L_,   0,    1024,     1024 * 32,         1024 + 128     },
        { L_,   0,    1024,     1024 * 32 + 128,   1024           },
        { L_,   0,    1024,     1024 * 32 + 128,   1024 + 128     },

        { L_,   0,    1024,     1024 * 33 - 128,   1024 - 128     },
        { L_,   0,    1024,     1024 * 33,         1024 - 128     },
        { L_,   0,    1024,     1024 * 33 - 128,   1024           },
        { L_,   0,    1024,     1024 * 33,         1024 + 128     },
        { L_,   0,    1024,     1024 * 33 + 128,   1024           },
        { L_,   0,    1024,     1024 * 33 + 128,   1024 + 128     },
#endif

        // This one is specifically to test with wrong boundaries, to ensure
        // that writev will not be able to write exactly multiple of buffer.
        // These numbers are all prime, big alloc is between 29 and 33 times
        // bigger than buffer alloc, and small alloc about 2.2 times bigger.
#ifndef BSLS_PLATFORM_OS_AIX
        { L_,   0,    1229,     35897,             2687           },
        { L_,   0,    1229,     39499,             2687           },
#endif

        // And finally, the stress test.  For this one, we need to read and
        // write concurrently, otherwise the outgoing system queue is filled
        // to capacity and without reading the writers will block.
#if 0
        { L_,   0,    32633,    1111247,           35897          },
        // TODO: THIS CASE BLOCKS - TEST DRIVER? OR COMPONENT?
        // NOTE: Blocks on AIX (even when the rest works) as well as SunOS.
#endif
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    for (int i = 0; i < NUM_DATA; ++i) {
        const int LINE               = DATA[i].d_line;
        const int CONCURRENT_READ    = IS_SSL
                                     ? 1 // always concurrent read
                                     : DATA[i].d_concurrentReadFlag;
        const int BUFFER_ALLOC       = DATA[i].d_bufferAlloc;
        const int BIG_BUFFER_ALLOC   = DATA[i].d_bigBufferAlloc;
        const int SMALL_BUFFER_ALLOC = DATA[i].d_smallBufferAlloc;

        if (verbose) cout << "TEST LINE " << LINE
                          << " (BIG_BUFFER_ALLOC = " << BIG_BUFFER_ALLOC
                          << ", SMALL_BUFFER_ALLOC = " << SMALL_BUFFER_ALLOC
                          << ", BUFFER_ALLOC = " << BUFFER_ALLOC
                          << ")" << endl;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Test Initialization

        enum {
            MAX_THREADS        = 1,
            SERVER_ID          = 1013410001,
            BACKLOG            = 1,
            WRITING_THREADS    = 3
        };

        bslmt::Barrier  channelBarrier(2);

        btlmt::ChannelPool *poolAddr;
        int                 poolEvent = -1;
        int                *eventAddr = &poolEvent;
        int                 channelId = -1;

        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                bdlf::BindUtil::bind( &caseChannelStateCallback
                                    , _1, _2, _3, _4
                                    , &poolAddr
                                    , &eventAddr
                                    , &channelBarrier
                                    , &channelId));

        btlmt::ChannelPool::PoolStateChangeCallback poolCb;
        btlmt::ChannelPool::BlobBasedReadCallback   dataCb;

        makeNull(&poolCb);
        makeNull(&dataCb);

        btlmt::ChannelPoolConfiguration cpc;
        cpc.setMaxConnections(MAX_THREADS);
        cpc.setMaxThreads(MAX_THREADS);
        cpc.setMetricsInterval(100.0);

        int maxOutgoingMessageSize = BUFFER_ALLOC;
        cpc.setOutgoingMessageSizes(0,
                                    (maxOutgoingMessageSize+1)/2,
                                    maxOutgoingMessageSize);

        Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;
        poolAddr = &mX;

        const btlso::IPv4Address ADDRESS("127.0.0.1", 0);

        int retCode = mX.start();
        LOOP_ASSERT(retCode, 0 == retCode);
        retCode = mX.listen(ADDRESS, BACKLOG, SERVER_ID);
        LOOP_ASSERT(retCode, 0 == retCode);
        ASSERT(0 == X.numChannels());
        btlso::IPv4Address PEER = getServerLocalAddress(&mX, SERVER_ID);

        typedef btlso::StreamSocket<btlso::IPv4Address>       Socket;
        typedef bteso_SslLikeStreamSocket<btlso::IPv4Address> SslLikeSocket;
        typedef btlsos::TcpChannel                            Channel;

        Socket  *socket = factory->allocate();
        Channel  channel(socket, &ta);

        SslLikeSocket *sSocket = dynamic_cast<SslLikeSocket*>(socket);

        int connectRetCode =  socket->connect(PEER);
        ASSERT(0 == connectRetCode);
        ASSERT(0 == channel.isInvalid());

        if (0 == connectRetCode) {
            channelBarrier.wait();
            ASSERT(-1 != channelId);
        }
        else {
            cout << "Error: Failed to connect to channelpool.  Abort.\n";
            return;                                                   // RETURN
        }

        ASSERT(0 == mX.setServerSocketOption(
                                       btlso::SocketOptUtil::k_SENDBUFFER,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       1024, SERVER_ID));

        ASSERT(0 == mX.setServerSocketOption(
                                       btlso::SocketOptUtil::k_TCPNODELAY,
                                       btlso::SocketOptUtil::k_TCPLEVEL,
                                       1, SERVER_ID));

        ASSERT(-5 == mX.write(31312313, (btls::Iovec*)NULL, 0));
        btlb::PooledBlobBufferFactory bufferFactory(BUFFER_ALLOC, &ta);

        if (veryVerbose)
            cout << "Writing using blobs\n"
                 << "-------------------" << endl;
        {
            if (veryVerbose)
                cout << "\tWriting BIG_BUFFER_ALLOC" << endl;

            bsls::AtomicInt count(0);
            bslmt::ThreadUtil::Handle threadsHandle[WRITING_THREADS + 1];
            bslmt::Barrier            threadBarrier(WRITING_THREADS);
            bslmt::ThreadAttributes   attr;
            caseThreadInfo2          info[WRITING_THREADS];
            for (int i = 0; i < WRITING_THREADS; ++i) {
                info[i].d_barrier       = &threadBarrier;
                info[i].d_threadCount   = &count;
                info[i].d_channelPool_p = &mX;
                info[i].d_channelId     = channelId;
                info[i].d_write         = i;
                info[i].d_bufferAlloc   = BUFFER_ALLOC;
                info[i].d_totalAlloc    = BIG_BUFFER_ALLOC;
                info[i].d_allocator_p   = &ta;
                info[i].d_factory_p     = &bufferFactory;
                ASSERT(0 == bslmt::ThreadUtil::create(&threadsHandle[i],
                                                     attr, &caseThread0,
                                                     &info[i]));
            }

            if (veryVerbose) {
                cout << "\tReading BIG_BUFFER_ALLOC" << endl;
            }
            caseReadInfo infoRead;
            infoRead.d_channel_p          = &channel;
            infoRead.d_threadCount        = &count;
            infoRead.d_concurrentReadFlag = CONCURRENT_READ;
            infoRead.d_numThreads         = WRITING_THREADS;
            infoRead.d_bufferAlloc        = BUFFER_ALLOC;
            infoRead.d_totalAlloc         = BIG_BUFFER_ALLOC;
            infoRead.d_allocator_p        = &ta;
            ASSERT(0 ==
                   bslmt::ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                             attr, &caseRead, &infoRead));

            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::join(threadsHandle[i],  0));
            }

            count = 0;
            if (veryVerbose) {
                cout << "\tWriting SMALL_BUFFER_ALLOC" << endl;
            }

            for (int i = 0; i < WRITING_THREADS; ++i) {
                info[i].d_barrier       = &threadBarrier;
                info[i].d_threadCount   = &count;
                info[i].d_channelPool_p = &mX;
                info[i].d_channelId     = channelId;
                info[i].d_write         = i;
                info[i].d_bufferAlloc   = BUFFER_ALLOC;
                info[i].d_totalAlloc    = SMALL_BUFFER_ALLOC;
                info[i].d_allocator_p   = &ta;
                info[i].d_factory_p     = &bufferFactory;
                ASSERT(0 == bslmt::ThreadUtil::create(&threadsHandle[i],
                                                      attr, &caseThread0,
                                                      &info[i]));
            }

            if (veryVerbose) {
                cout << "\tReading SMALL_BUFFER_ALLOC" << endl;
            }
            infoRead.d_channel_p          = &channel;
            infoRead.d_threadCount        = &count;
            infoRead.d_concurrentReadFlag = CONCURRENT_READ;
            infoRead.d_numThreads         = WRITING_THREADS;
            infoRead.d_bufferAlloc        = BUFFER_ALLOC;
            infoRead.d_totalAlloc         = SMALL_BUFFER_ALLOC;
            infoRead.d_allocator_p        = &ta;
            ASSERT(0 ==
                   bslmt::ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                             attr, &caseRead, &infoRead));

            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::join(threadsHandle[i],  0));
            }

        }

        if (veryVerbose)
            cout << "Writing using iovecs\n"
                 << "--------------------" << endl;
        {
            if (veryVerbose)
                cout << "\tWriting BIG_BUFFER_ALLOC" << endl;

            bsls::AtomicInt count(0);
            bslmt::ThreadUtil::Handle threadsHandle[WRITING_THREADS+1];
            bslmt::Barrier            threadBarrier(WRITING_THREADS);
            bslmt::ThreadAttributes   attr;
            caseThreadInfo           info[WRITING_THREADS];

            for (int i = 0; i < WRITING_THREADS; ++i) {
                info[i].d_barrier       = &threadBarrier;
                info[i].d_threadCount   = &count;
                info[i].d_channelPool_p = &mX;
                info[i].d_channelId     = channelId;
                info[i].d_write         = i;
                info[i].d_bufferAlloc   = BUFFER_ALLOC;
                info[i].d_totalAlloc    = BIG_BUFFER_ALLOC;
                info[i].d_allocator_p   = &ta;
                ASSERT(0 == bslmt::ThreadUtil::create(&threadsHandle[i],
                                                      attr, &caseThread2,
                                                      &info[i]));
            }

            if (veryVerbose) {
                cout << "\tReading BIG_BUFFER_ALLOC" << endl;
            }
            caseReadInfo infoRead;
            infoRead.d_channel_p          = &channel;
            infoRead.d_threadCount        = &count;
            infoRead.d_concurrentReadFlag = CONCURRENT_READ;
            infoRead.d_numThreads         = WRITING_THREADS;
            infoRead.d_bufferAlloc        = BUFFER_ALLOC;
            infoRead.d_totalAlloc         = BIG_BUFFER_ALLOC;
            infoRead.d_allocator_p        = &ta;
            ASSERT(0 ==
                   bslmt::ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                             attr, &caseRead, &infoRead));

            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::join(threadsHandle[i],  0));
            }

            if (veryVerbose) {
                cout << "\tWriting SMALL_BUFFER_ALLOC" << endl;
            }

            count = 0;
            for (int i = 0; i < WRITING_THREADS; ++i) {
                info[i].d_barrier       = &threadBarrier;
                info[i].d_channelPool_p = &mX;
                info[i].d_channelId     = channelId;
                info[i].d_write         = i;
                info[i].d_bufferAlloc   = BUFFER_ALLOC;
                info[i].d_totalAlloc    = SMALL_BUFFER_ALLOC;
                info[i].d_allocator_p   = &ta;
                ASSERT(0 == bslmt::ThreadUtil::create(&threadsHandle[i],
                                                      attr, &caseThread2,
                                                      &info[i]));
            }

            if (veryVerbose) {
                cout << "\tReading SMALL_BUFFER_ALLOC" << endl;
            }
            infoRead.d_channel_p          = &channel;
            infoRead.d_threadCount        = &count;
            infoRead.d_concurrentReadFlag = CONCURRENT_READ;
            infoRead.d_numThreads         = WRITING_THREADS;
            infoRead.d_bufferAlloc        = BUFFER_ALLOC;
            infoRead.d_totalAlloc         = SMALL_BUFFER_ALLOC;
            infoRead.d_allocator_p        = &ta;
            ASSERT(0 ==
                   bslmt::ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                             attr, &caseRead, &infoRead));

            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::join(threadsHandle[i],  0));
            }
        }

        if (veryVerbose)
            cout << "Writing using both flavors of write()\n"
                 << "---------------------------------------" << endl;

        {
            if (veryVerbose) {
                cout << "\tWriting BIG_BUFFER_ALLOC" << endl;
            }

            bsls::AtomicInt count(0);
            bslmt::ThreadUtil::Handle threadsHandle[WRITING_THREADS+1];
            bslmt::Barrier            threadBarrier(WRITING_THREADS);
            bslmt::ThreadAttributes   attr;
            caseThreadInfo2          info[WRITING_THREADS];

            for (int i = 0; i < WRITING_THREADS; ++i) {
                info[i].d_barrier       = &threadBarrier;
                info[i].d_threadCount   = &count;
                info[i].d_channelPool_p = &mX;
                info[i].d_channelId     = channelId;
                info[i].d_write         = i;
                info[i].d_bufferAlloc   = BUFFER_ALLOC;
                info[i].d_totalAlloc    = BIG_BUFFER_ALLOC;
                info[i].d_allocator_p   = &ta;
                info[i].d_factory_p     = &bufferFactory;
                ASSERT(0 == bslmt::ThreadUtil::create(&threadsHandle[i],
                                                        attr, &caseThread3,
                                                        &info[i]));
            }

            if (veryVerbose) {
                cout << "\tReading BIG_BUFFER_ALLOC" << endl;
            }
            caseReadInfo infoRead;
            infoRead.d_channel_p          = &channel;
            infoRead.d_threadCount        = &count;
            infoRead.d_concurrentReadFlag = CONCURRENT_READ;
            infoRead.d_numThreads         = WRITING_THREADS;
            infoRead.d_bufferAlloc        = BUFFER_ALLOC;
            infoRead.d_totalAlloc         = BIG_BUFFER_ALLOC;
            infoRead.d_allocator_p        = &ta;
            ASSERT(0 ==
                   bslmt::ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                             attr, &caseRead, &infoRead));

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::join(threadsHandle[i],  0));
            }

            if (sSocket) {
                sSocket->writeFlush();
            }

            if (veryVerbose) {
                cout << "\tWriting SMALL_BUFFER_ALLOC" << endl;
            }

            count = 0;
            for (int i = 0; i < WRITING_THREADS; ++i) {
                info[i].d_barrier       = &threadBarrier;
                info[i].d_threadCount   = &count;
                info[i].d_channelPool_p = &mX;
                info[i].d_channelId     = channelId;
                info[i].d_write         = i;
                info[i].d_bufferAlloc   = BUFFER_ALLOC;
                info[i].d_totalAlloc    = SMALL_BUFFER_ALLOC;
                info[i].d_allocator_p   = &ta;
                info[i].d_factory_p     = &bufferFactory;
                ASSERT(0 == bslmt::ThreadUtil::create(&threadsHandle[i],
                                                      attr, &caseThread3,
                                                      &info[i]));
            }

            if (veryVerbose) {
                cout << "\tReading SMALL_BUFFER_ALLOC" << endl;
            }
            infoRead.d_channel_p          = &channel;
            infoRead.d_threadCount        = &count;
            infoRead.d_concurrentReadFlag = CONCURRENT_READ;
            infoRead.d_numThreads         = WRITING_THREADS;
            infoRead.d_bufferAlloc        = BUFFER_ALLOC;
            infoRead.d_totalAlloc         = SMALL_BUFFER_ALLOC;
            infoRead.d_allocator_p        = &ta;
            ASSERT(0 ==
                   bslmt::ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                             attr, &caseRead, &infoRead));

            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::join(threadsHandle[i],  0));
            }
        }

        channel.invalidate();
        factory->deallocate(socket);
        channelBarrier.wait();
    }
}

}  // close namespace TEST_CASE_WRITE

//-----------------------------------------------------------------------------
//                  TEST_CASE_SHUTDOWN supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_SHUTDOWN {

struct caseCallbackInfo {
    bslmt::Barrier    *d_barrier_p;
    bsls::AtomicInt    d_channelId;
    bsls::AtomicInt    d_channelDownReadFlag;
    bsls::AtomicInt    d_channelDownWriteFlag;
};

static
void caseChannelStateCallback(int                channelId,
                               int                serverId,
                               int                state,
                               void              *,
                               caseCallbackInfo *info)
{
    ASSERT(info);

    switch(state){
      case btlmt::ChannelPool::e_CHANNEL_DOWN_READ: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated (read only):"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        ++info->d_channelDownReadFlag;
        info->d_barrier_p->wait();
      } break;

      case btlmt::ChannelPool::e_CHANNEL_DOWN_WRITE: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated (write only):"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        ++info->d_channelDownWriteFlag;
        info->d_barrier_p->wait();
      } break;

      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        info->d_barrier_p->wait();
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        info->d_channelId = channelId;
        info->d_barrier_p->wait();
      } break;
    }
}

}  // close namespace TEST_CASE_SHUTDOWN

//-----------------------------------------------------------------------------
//                  TEST_CASE_IMPORT_HALF_CLOSED_CONN classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_IMPORT_HALF_CLOSED_CONN {

static
void caseChannelStateCallback(int              channelId,
                               int              serverId,
                               int              state,
                               void            *,
                               bslmt::Barrier  *barrier)
{
    ASSERT(barrier);

    switch(state){
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        barrier->wait();
      } break;

      case btlmt::ChannelPool::e_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
      } break;
    }
}

}  // close namespace TEST_CASE_IMPORT_HALF_CLOSED_CONN

//-----------------------------------------------------------------------------
//                  TEST_CASE_CONNECT supporting classes and methods
//-----------------------------------------------------------------------------

namespace TEST_CASE_CONNECT {

void poolStateCb(int state, int source, int severity)
{
    if (veryVerbose) {
        MTCOUT << "Pool state callback called with"
               << " State: " << state
               << " Source: "  << source
               << " Severity: " << severity << MTENDL;
    }
}

void channelStateCb(int              channelId,
                    int              serverId,
                    int              state,
                    void            *,
                    int             *id,
                    bslmt::Barrier  *barrier)
{
    if (veryVerbose) {
        MTCOUT << "Channel state callback called with"
               << " Channel Id: " << channelId
               << " Server Id: "  << serverId
               << " State: " << state << MTENDL;
    }

    if (btlmt::ChannelPool::e_CHANNEL_UP == state) {
        *id = channelId;
        barrier->wait();
    }
}

void blobBasedReadCb(int        *needed,
                     btlb::Blob *msg,
                     int         channelId,
                     void       *)
{
    if (veryVerbose) {
        MTCOUT << "Blob Based Read Cb called with"
               << " Channel Id: " << channelId
               << " of length: "  << msg->length() << MTENDL;
    }
    *needed = 1;
    msg->removeAll();
}

struct caseWorkerInfo {
    int                        d_i;
    int                        d_expUserId;
    int                        d_expNumFailures;
    int                        d_queueSize;
    int                        d_timeOut;
    bsls::AtomicInt            d_portNumber;
    btlso::IPv4Address         d_address;
    bslmt::Barrier            *d_barrier_p;
    bslmt::ThreadUtil::Handle *d_worker_p;
};

extern "C"
void *caseOpenConnectThread(void *arg) {
    caseWorkerInfo *info = (caseWorkerInfo*) arg;

    const int i            = info->d_i;
    const int QUEUE_SIZE   = info->d_queueSize;
    const int TIMEOUT      = info->d_timeOut;
    const int PORT_NUMBER  = info->d_portNumber;
    ASSERT(0 != PORT_NUMBER);

    btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
    btlso::IPv4Address serverAddress;
//     serverAddress.setPortNumber(PORT_NUMBER);

    btlsos::TcpTimedAcceptor acceptor(&factory);

    if (veryVerbose) {
        MTCOUT << bdlt::CurrentTime::now() << ": OPENING SERVER@ "
               << PORT_NUMBER << endl << MTENDL;
    }
    if (0 != acceptor.open(serverAddress, QUEUE_SIZE)) {
        MTCOUT << "bdlt::CurrentTime::now() = " << bdlt::CurrentTime::now()
               << ": ERROR OPENING SERVER.\n"
               << MTENDL;
        // Leave port number to non 0, to signal opening error in main thread.
        return 0;                                                     // RETURN
    }

    int status;
    bsls::TimeInterval timeout = bdlt::CurrentTime::now() + TIMEOUT * 3;
    btlsc::Channel *channel = acceptor.timedAllocate(&status, timeout);

    if (channel) {
        if (veryVerbose) {
            MTCOUT << "bdlt::CurrentTime::now() = " << bdlt::CurrentTime::now()
                   << ": CLIENT CONNECTED.\n"
                   << MTENDL;
        }
        info->d_barrier_p->wait();
        if (channel) {
            acceptor.deallocate(channel);
        }
        if (veryVerbose) {
            MTCOUT << bdlt::CurrentTime::now() << ": CLOSING SERVER@ "
                   << PORT_NUMBER << endl << MTENDL;
        }
        LOOP_ASSERT(i, 0 == acceptor.close());
        info->d_portNumber = 0;
    }
    else {
        LOOP_ASSERT(i, 0 == status); // allocate timed out
    }
    return 0;
}

static void casePoolStateCb(int              poolState,
                             int              sourceId,
                             int              severity,
                             // additional arguments follow
                             bsls::AtomicInt *numFailures,
                             caseWorkerInfo *info)
{
    if (veryVerbose) {
        MTCOUT << "PoolStateCb: bdlt::CurrentTime::now() = "
               << bdlt::CurrentTime::now() << endl;
        switch(poolState) {
#define CASE(X) case X: cout << "\tstate = " << #X << endl; break;
            CASE(PoolState::BTEMT_ACCEPT_TIMEOUT);
            CASE(PoolState::BTEMT_ERROR_ACCEPTING);
            CASE(PoolState::BTEMT_ERROR_CONNECTING);
            CASE(PoolState::BTEMT_CHANNEL_LIMIT);
            CASE(PoolState::BTEMT_CAPACITY_LIMIT);
#undef CASE
            default: cout << "Unknown pool state: "; P(poolState); break;
        }
        T_(); P_(sourceId); P_(info->d_expUserId); P(severity);
        T_(); P_(*numFailures); P(info->d_expNumFailures);
        cout << MTENDL;
    }

    if (PoolState::BTEMT_CHANNEL_LIMIT == poolState) {
        ASSERT(btlmt::ChannelPool::e_ALERT == severity);  // ALERT
        return;                                                       // RETURN
    }

    ASSERT(PoolState::BTEMT_ERROR_CONNECTING == poolState);
    ASSERT(0 <= *numFailures);
    ++(*numFailures);
    LOOP3_ASSERT(*numFailures, info->d_expNumFailures, info->d_portNumber,
            *numFailures <= info->d_expNumFailures || info->d_portNumber);
    ASSERT(info->d_expUserId == sourceId);

    if (*numFailures == info->d_expNumFailures) {
        // Now is the time to let the last connect attempt succeed.

        bslmt::ThreadUtil::create(info->d_worker_p,  &caseOpenConnectThread,
                                 (void *)info);
    }
}

static void caseChannelCb(int                  channelId,
                           int                  sourceId,
                           int                  state,
                           void                *,
                           int                 *channelIdPtr,
                           bsls::AtomicInt     *isInvokedFlag,
                           bsls::AtomicInt     *numFailures,
                           caseWorkerInfo     *info,
                           btlmt::ChannelPool **pool)
{
    if (veryVerbose) {
        MTCOUT << "ChannelCb: bdlt::CurrentTime::now() = "
               << bdlt::CurrentTime::now() << endl;
        switch(state) {
#define CASE(X) case X: cout << "\tstate = " << #X << endl; break;
            CASE(btlmt::ChannelPool::e_CHANNEL_UP);
            CASE(btlmt::ChannelPool::e_CHANNEL_DOWN);
            CASE(btlmt::ChannelPool::e_READ_TIMEOUT);
            CASE(btlmt::ChannelPool::e_WRITE_BUFFER_FULL);
            CASE(btlmt::ChannelPool::e_AUTO_READ_ENABLED);
            CASE(btlmt::ChannelPool::e_AUTO_READ_DISABLED);
            CASE(btlmt::ChannelPool::e_WRITE_CACHE_LOWWAT);
            CASE(btlmt::ChannelPool::e_CHANNEL_DOWN_READ);
            CASE(btlmt::ChannelPool::e_CHANNEL_DOWN_WRITE);
#undef CASE
            default: cout << "Unknown channel state: "; P(state); break;
        }
        T_(); P_(channelId); P(sourceId);
        T_(); P(info->d_expUserId);
        T_(); P_(*numFailures); P(info->d_expNumFailures);
        cout << MTENDL;
    }

    LOOP2_ASSERT(info->d_expUserId, sourceId, info->d_expUserId == sourceId);

    switch (state) {
      case btlmt::ChannelPool::e_CHANNEL_UP:
        ASSERT(*isInvokedFlag == 0);
        ASSERT(*numFailures <= info->d_expNumFailures);

        // NOTE: if port was open by another test driver running concurrently,
        // we may connect earlier than we should: do not assert.

        *channelIdPtr = channelId;
        *isInvokedFlag += 1;
        break;

      case btlmt::ChannelPool::e_CHANNEL_DOWN:
        (*pool)->close(channelId);
        break;

      case btlmt::ChannelPool::e_AUTO_READ_ENABLED:
        *isInvokedFlag += 2;
        break;

      default:
        ASSERT(!"Unexpected channel state callback in caseChannelCallback");
        P(state);
    }
}

static void caseErrorPoolStateCb(int              poolState,
                                  int              sourceId,
                                  int              severity,
                                  int              expectedSourceId,
                                  int              expectedSeverity,
                                  bsls::AtomicInt *isInvokedFlag)
{
    ASSERT(expectedSourceId == sourceId);
    ASSERT(expectedSeverity == severity);
    ASSERT(PoolState::BTEMT_ERROR_CONNECTING == poolState);
    if (veryVerbose) {
        PT(bdlt::CurrentTime::now());
        PT(poolState);
        PT(sourceId);
        PT(severity);
    }
    ++(*isInvokedFlag);
}

}  // close namespace TEST_CASE_CONNECT

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// monitorPool is reused.
//-----------------------------------------------------------------------------

static void monitorPool(bslmt::Mutex              *coutLock,
                        const btlmt::ChannelPool&  pool,
                        int                        numTimes,
                        bool                       verbose)
    // With the period of 2 seconds up to the specified 'numTimes' times,
    // output the percent busy of the specified channel 'pool' to the standard
    // output using the specified 'coutLock' for synchronizing access to a
    // standard output stream.
{
    while (0 <= --numTimes) {
        bslmt::ThreadUtil::yield();
//         bslmt::ThreadUtil::microSleep(0,  2);  // 2 seconds
        if (verbose) {
            coutLock->lock();
            bsl::cout << bdlt::CurrentTime::utc() << "\n"
                      << "The pool is " << pool.busyMetrics() << "% busy ("
                      << pool.numThreads() << " threads).  "
                      << pool.numChannels() << " channels.  Allocations =";
            int numThreads = pool.numThreads();
            for (int i = 0; i < numThreads; ++i) {
                int numEvents = pool.numEvents(i);
                bsl::cout << ' ' << numEvents;
            }
            bsl::cout << bsl::endl;
            coutLock->unlock();
        }
    }
}

namespace QUEUE_PROCESSOR_NAMESPACE {

static
int parseMessages(int *numNeeded, btlb::Blob *blob)
{
    ASSERT(numNeeded);

    int   length            = blob->length();
    int   originalLength    = length;
    int   processedMessages = 0;
    char *data              = blob->buffer(0).data();

    while (length > (int) sizeof(int)) {
        int msgLength;
        memcpy(&msgLength, data, sizeof(int));
        msgLength = ntohl(msgLength);
        ASSERT(0 <= msgLength);

        // As a guard against malicious user, the msgLength should be limited
        // in the protocol.

        if (length < msgLength) {
            *numNeeded = msgLength - length;
            return originalLength - length;                           // RETURN
        }
        length -= msgLength;
        data   += msgLength;
        ++processedMessages;
    }
    *numNeeded = sizeof(int) - length;
    return originalLength - length;
}

class my_QueueProcessor {
    // The class implements a queue-based message TCP/IP server.

    enum {
        SERVER_ID = 0x7A
    };

    typedef bsl::pair<int, btlb::Blob> BlobTypeWithId;

    btlmt::ChannelPoolConfiguration  d_config;          // pool's configuration
    btlmt::ChannelPool              *d_channelPool_p;   // managed pool
    bslma::Allocator                *d_allocator_p;     // memory manager
    bslmt::Mutex                    *d_coutLock_p;
    bdlcc::Queue<BlobTypeWithId>    *d_incomingQueue_p;
    bdlcc::Queue<BlobTypeWithId>    *d_outgoingQueue_p;
    bslmt::ThreadUtil::Handle        d_processorHandle;
    bsls::AtomicOperations::AtomicTypes::Int
                                     d_runningFlag;

  private:

    // Callback functions:

    void poolCb(int state, int source, int severity);
    void channelStateCb(int channelId, int sourceId, int state, void *context);
    void blobCB(int *numNeeded, btlb::Blob *msg, int channelId, void *arg);

    // Not implemented:

    my_QueueProcessor(const my_QueueProcessor& original);        // Not impl.
    my_QueueProcessor& operator=(const my_QueueProcessor& rhs);  // Not impl.

  public:
    // CREATORS
    my_QueueProcessor(bdlcc::Queue<BlobTypeWithId> *incomingQueue,
                      bdlcc::Queue<BlobTypeWithId> *outgoingQueue,
                      bslmt::Mutex                 *coutLock,
                      int                           portNumber,
                      int                           numConnections,
                      bslma::Allocator             *basicAllocator = 0);

    ~my_QueueProcessor();
        // Destroy this server.

    // MANIPULATORS
    int processOutgoingQueue();

    int startProcessor();

    int stopProcessor();

    const btlmt::ChannelPool& pool() { return *d_channelPool_p; }

    // ACCESSORS
    int portNumber();
};

extern "C" {
    static void *queueProc(void *arg) {
        my_QueueProcessor *proc = (my_QueueProcessor*)arg;
        ASSERT(proc);
        return (void*)proc->processOutgoingQueue();
    }
}

my_QueueProcessor::my_QueueProcessor(
                                  bdlcc::Queue<BlobTypeWithId> *incomingQueue,
                                  bdlcc::Queue<BlobTypeWithId> *outgoingQueue,
                                  bslmt::Mutex                 *coutLock,
                                  int                           portNumber,
                                  int                           numConnections,
                                  bslma::Allocator             *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_coutLock_p(coutLock)
, d_incomingQueue_p(incomingQueue)
, d_outgoingQueue_p(outgoingQueue)
{
    bsls::AtomicOperations::initInt(&d_runningFlag, 0);
    d_config.setMaxThreads(4);
    d_config.setMaxConnections(numConnections);
    d_config.setReadTimeout(5.0);      // in seconds
    d_config.setMetricsInterval(10.0); // seconds
    d_config.setWriteCacheWatermarks(0, 1 << 20);  // 1Mb
    d_config.setIncomingMessageSizes(4, 100, 1024);

    if (verbose) {
        PT(d_config);
    }

    btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
             bsl::allocator_arg_t(),
             bsl::allocator<btlmt::ChannelPool::ChannelStateChangeCallback>(
                                                               basicAllocator),
             bdlf::MemFnUtil::memFn(&my_QueueProcessor::channelStateCb, this));

    btlmt::ChannelPool::BlobBasedReadCallback      dataCb(
                     bsl::allocator_arg_t(),
                     bsl::allocator<btlmt::ChannelPool::BlobBasedReadCallback>(
                                                               basicAllocator),
                     bdlf::MemFnUtil::memFn(&my_QueueProcessor::blobCB, this));

    btlmt::ChannelPool::PoolStateChangeCallback    poolCb(
                   bsl::allocator_arg_t(),
                   bsl::allocator<btlmt::ChannelPool::PoolStateChangeCallback>(
                                                               basicAllocator),
                   bdlf::MemFnUtil::memFn(&my_QueueProcessor::poolCb, this));

    d_channelPool_p = new (*d_allocator_p) btlmt::ChannelPool(channelCb,
                                                              dataCb,
                                                              poolCb,
                                                              d_config,
                                                              basicAllocator);

    if (verbose) {
        bsl::cout << "Starting queue processor, listening on port "
                  << portNumber << "." << bsl::endl;
    }
    ASSERT(0 == d_channelPool_p->start());
    ASSERT(0 == d_channelPool_p->listen(portNumber,
                                        numConnections,
                                        SERVER_ID));
}

my_QueueProcessor::~my_QueueProcessor() {
    d_channelPool_p->stop();
    d_allocator_p->deleteObjectRaw(d_channelPool_p);
}

int my_QueueProcessor::processOutgoingQueue() {
    while (bsls::AtomicOperations::getInt(&d_runningFlag)) {
        bsls::TimeInterval TIMEOUT(bdlt::CurrentTime::now());
        TIMEOUT += 0.5; // 0.5s
        BlobTypeWithId msg;
        if (0 == d_outgoingQueue_p->timedPopBack(&msg, TIMEOUT)) {
            if (0 != d_channelPool_p->write(msg.first, msg.second)) {
                if (veryVerbose) {
                    d_coutLock_p->lock();
                    bsl::cout << "Channel " << msg.first << " is down."
                              << bsl::endl;
                    d_coutLock_p->unlock();
                }
                d_channelPool_p->shutdown(msg.first,
                                          btlmt::ChannelPool::e_IMMEDIATE);
            }
        }
    }
    return 0;
}

int my_QueueProcessor::startProcessor() {
    bslmt::ThreadAttributes attributes;
    attributes.setDetachedState(bslmt::ThreadAttributes::e_CREATE_JOINABLE);
    bsls::AtomicOperations::setInt(&d_runningFlag, 1);
    return bslmt::ThreadUtil::create(&d_processorHandle, attributes,
                                     &queueProc, (void*)this);
}

int my_QueueProcessor::stopProcessor() {
    bsls::AtomicOperations::setInt(&d_runningFlag, 0);
    return bslmt::ThreadUtil::join(d_processorHandle);
}

int my_QueueProcessor::portNumber() {
    btlso::IPv4Address address = getServerLocalAddress(d_channelPool_p,
                                                       SERVER_ID);
    return address.portNumber();
}

void my_QueueProcessor::poolCb(int state, int source, int severity) {
    if (veryVerbose) {
        MTCOUT << "Pool state changed ("
               << source << ", " << severity << ", " << state
               << ")." << MTENDL;
    }

}

void my_QueueProcessor::channelStateCb(int   channelId,
                                       int   sourceId,
                                       int   state,
                                       void *)
{
    ASSERT(SERVER_ID == sourceId);

    switch(state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
          btlso::IPv4Address peer;
          d_channelPool_p->getPeerAddress(&peer, channelId);
          if (veryVerbose) {
              MTCOUT << "Client " << channelId << " from " << peer
                     << " has disconnected on server " << sourceId
                     << "." << MTENDL;
          }
          d_channelPool_p->shutdown(channelId,
                                    btlmt::ChannelPool::e_IMMEDIATE);
      } break;
      case btlmt::ChannelPool::e_CHANNEL_UP: {
          btlso::IPv4Address peer;
          d_channelPool_p->getPeerAddress(&peer, channelId);
          if (veryVerbose) {
              MTCOUT << "Client " << channelId << " connected from " << peer
                     << " on server " << sourceId << "." << MTENDL;
          }
      } break;
    }
}

void my_QueueProcessor::blobCB(int          *numNeeded,
                               btlb::Blob   *msg,
                               int           channelId,
                               void         *)
{
    ASSERT(numNeeded);
    ASSERT(msg);

    int length = msg->length();
    ASSERT(0 < length);

    int numConsumed = parseMessages(numNeeded, msg);

    if (veryVeryVerbose) {
        MTCOUT << "Received " << msg->length() << " bytes from "
               << channelId << " " << *numNeeded
               << " needed to complete a message." << MTENDL;
    }

    if (numConsumed) {
        BlobTypeWithId data(channelId, *msg);
        d_incomingQueue_p->pushBack(data);
    }
}

}  // close namespace QUEUE_PROCESSOR_NAMESPACE

namespace USAGE_EXAMPLE_NAMESPACE {

///Usage Example 2
///- - - - - - - -
// The following usage example shows a possible implementation of a multi-user
// echo server.  An echo server accepts connections and, for every connection,
// sends any received data back to the client (until the connection is
// terminated).  This server requires that data be read from an accepted
// connection within a certain time interval or else it is dropped on timeout.
// The echo server is implemented as a separate class ('my_EchoServer') that
// owns a channel pool and its configuration parameters.  The configuration
// parameters are compile-time constants within this class.  The definition
// for 'my_EchoServer' follows:
//..
    class my_EchoServer {
        // This class implements a multi-user multi-threaded echo server.

        enum {
            SERVER_ID = 0xAB   // An (arbitrary) constant (passed to 'listen')
                               // that identifies the channel pool operation
                               // associated with a pool state or channel
                               // state callback.
        };

        // DATA
        btlmt::ChannelPoolConfiguration  d_config;        // pool's config
        btlmt::ChannelPool              *d_channelPool_p; // managed pool
        bslma::Allocator                *d_allocator_p;   // memory manager
        bslmt::Mutex                    *d_coutLock_p;    // synchronize 'cout'

      private:
        // Callback functions:
        void poolStateCb(int state, int source, int severity);
            // Output a message to 'stdout' indicating the specified 'state'
            // associated with the specified 'source' has occurred, with the
            // specified 'severity'.  Note that 'state' is one of the
            // 'PoolState' constants (see 'btlmt_message'), 'source'
            // identifies the channel pool operation associated with this state
            // (in this case, this must be the 'SERVER_ID' passed to
            // 'listen()'), and 'severity' is one of the
            // 'btlmt::ChannelPool::Severity' values.

        void channelStateCb(int channelId, int sourceId, int state, void *ctx);
            // Output a message to 'stdout' indicating the specified 'state',
            // associated with the specified 'channelId' and 'sourceId', has
            // occured.  If 'state' is 'btlmt::ChannelPool::e_CHANNEL_DOWN'
            // then shutdown the channel.  Note that the 'channelId' is a
            // unique identifier chosen by the channel pool for each connection
            // channel, 'sourceId' identifies the channel pool operation
            // responsible for creating the channel (in this case, this must
            // be the 'SERVER_ID' passed to 'listen()'), state is a
            // 'btlmt::ChannelPool::ChannelState' enumeration value, and 'ctx'
            // is the address of a context object provided for the channel
            // (using 'setChannelContext()'), in this example we do not
            // specify a context, so the value will be 0.

        void blobCB(int        *numNeeded,
                    btlb::Blob *msg,
                    int         channelId,
                    void       *arg);
            // Echo the specified 'msg' to the client on the channel identified
            // by 'channelId' channel, load into the specified 'numNeeeded' the
            // minimum length of additional data that is needed to complete a
            // message, and close the communication channel.  Because this echo
            // server is not interested in a discrete messages in a particular
            // message format, 'numNeeded' will be set to 1 (indicating this
            // callback should be invoked again as soon as any new data is
            // read).

        // NOT IMPLEMENTED
        my_EchoServer(const my_EchoServer&);
        my_EchoServer& operator=(const my_EchoServer&);

      public:
        my_EchoServer(bslmt::Mutex     *coutLock,
                      int               portNumber,
                      int               numConnections,
                      bslma::Allocator *basicAllocator = 0);
            // Create an echo server that listens for incoming connections on
            // the specified 'portNumber' managing up to the specified
            // 'numConnections' simultaneous connections.  The echo server
            // will use the specified 'coutLock' to synchronize access to the
            // standard output.  Optionally specify a 'basicAllocator' used to
            // supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.  The behavior is undefined
            // unless 'coutLock' is a valid address for a mutex object.

        ~my_EchoServer();
            // Destroy this server.

        // MANIPULATORS
        const btlmt::ChannelPool& pool() { return *d_channelPool_p; }
    };
//..
// In the constructor of 'my_EchoServer', the configuration is initialized,
// the channel pool is created, configured, and started.  The listening port
// is established:
//..
    my_EchoServer::my_EchoServer(bslmt::Mutex     *coutLock,
                                 int               portNumber,
                                 int               numConnections,
                                 bslma::Allocator *basicAllocator)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    , d_coutLock_p(coutLock)
    {
        d_config.setMaxThreads(4);
        d_config.setMaxConnections(numConnections);
        d_config.setReadTimeout(5.0);       // in seconds
        d_config.setMetricsInterval(10.0);  // seconds
        d_config.setWriteCacheWatermarks(0, 1<<10); // 1Mb
        d_config.setIncomingMessageSizes(1, 100, 1024);

        btlmt::ChannelPool::ChannelStateChangeCallback channelStateFunctor(
                bsl::allocator_arg_t(),
                bsl::allocator<btlmt::ChannelPool::ChannelStateChangeCallback>(
                                                               basicAllocator),
                bdlf::MemFnUtil::memFn(&my_EchoServer::channelStateCb, this));

        btlmt::ChannelPool::PoolStateChangeCallback    poolStateFunctor(
                   bsl::allocator_arg_t(),
                   bsl::allocator<btlmt::ChannelPool::PoolStateChangeCallback>(
                                                               basicAllocator),
                   bdlf::MemFnUtil::memFn(&my_EchoServer::poolStateCb, this));

        btlmt::ChannelPool::BlobBasedReadCallback      dataFunctor(
                     bsl::allocator_arg_t(),
                     bsl::allocator<btlmt::ChannelPool::BlobBasedReadCallback>(
                                                               basicAllocator),
                     bdlf::MemFnUtil::memFn(&my_EchoServer::blobCB, this));

        d_channelPool_p = new (*d_allocator_p) btlmt::ChannelPool(
                                                           channelStateFunctor,
                                                           dataFunctor,
                                                           poolStateFunctor,
                                                           d_config,
                                                           basicAllocator);

        ASSERT(0 == d_channelPool_p->start());
        ASSERT(0 == d_channelPool_p->listen(portNumber,
                                            numConnections,
                                            SERVER_ID));
    }
//..
// Destructor just stops the pool and destroys it:
//..
    my_EchoServer::~my_EchoServer() {
        d_channelPool_p->stop();
        d_allocator_p->deleteObjectRaw(d_channelPool_p);
    }
//..
// The pool state callback will just print the new state.  The channel state
// callback will report a new state and the address of the peer.  The data
// state callback will immediately write data back to the channel pool.  These
// methods are documented in the example header, and the implementation for
// these methods is shown below:
//..
    void my_EchoServer::poolStateCb(int state, int source, int severity) {
        ASSERT(source == SERVER_ID);
        d_coutLock_p->lock();
        MTCOUT << "Pool state changed: ("
               << source << ", "
               << severity << ", "
               << state << ") " << MTENDL;
        d_coutLock_p->unlock();
    }
//
    void my_EchoServer::channelStateCb(int   channelId,
                                       int   sourceId,
                                       int   state,
                                       void *)
    {
        ASSERT(SERVER_ID == sourceId);

        switch(state) {
          case btlmt::ChannelPool::e_CHANNEL_DOWN: {
              btlso::IPv4Address peer;
              d_channelPool_p->getPeerAddress(&peer, channelId);
              d_coutLock_p->lock();
              MTCOUT << "Client from " << peer << " has disconnected."
                     << MTENDL;
              d_coutLock_p->unlock();
              d_channelPool_p->shutdown(channelId,
                                        btlmt::ChannelPool::e_IMMEDIATE);
          } break;
          case btlmt::ChannelPool::e_CHANNEL_UP: {
              btlso::IPv4Address peer;
              d_channelPool_p->getPeerAddress(&peer, channelId);
              d_coutLock_p->lock();
              MTCOUT << "Client connected from " << peer << MTENDL;
              d_coutLock_p->unlock();
          } break;
        }
    }

    void my_EchoServer::blobCB(int        *numNeeded,
                               btlb::Blob *msg,
                               int         channelId,
                               void       *)
    {
        ASSERT(numNeeded);
        ASSERT(msg);
        ASSERT(0 < msg->length());

        ASSERT(0 == d_channelPool_p->write(channelId, *msg));
        btlb::BlobUtil::erase(msg, 0, msg->length());
        *numNeeded   = 1;
        MTCOUT << "Shutdown " << channelId << MTENDL;
        d_channelPool_p->shutdown(channelId,
                                  btlmt::ChannelPool::e_IMMEDIATE);
    }
//..
// The implementation of an echo server is now complete.  Let's create
// a small program that uses it.  We will create a server object, then
// the main thread will monitor the channel pool and periodically print
// its busy metrics.  For simplicity, we will use the following function
// for monitoring:
//..
    static void monitorPool(bslmt::Mutex              *coutLock,
                            const btlmt::ChannelPool&  pool,
                            int                        numTimes)
        // Every 10 seconds, output the percent busy of the specified channel
        // 'pool' to the standard output, using the specified 'coutLock' to
        // synchronizing access to the standard output stream; return to the
        // caller after 'numTimes' output operations (i.e. numTimes * 10
        // seconds).
    {
        while(--numTimes > 0) {
            coutLock->lock();
            if (verbose) {
                cout << "The pool is " << pool.busyMetrics() << "% busy ("
                     << pool.numThreads() << " threads)." << endl;
            }
            coutLock->unlock();
        }
    }
//..

}  // close namespace USAGE_EXAMPLE_NAMESPACE

namespace QUEUE_CLIENT_NAMESPACE {

typedef bsl::pair<int, btlb::Blob> BlobTypeWithId;

const int versionSelector = 20140601;

static void
generateMessage(btlb::Blob& msg, bslma::Allocator *basicAllocator = 0)
{
    using bslx::OutStreamFunctions::bdexStreamOut;

    bsls::TimeInterval now = bdlt::CurrentTime::now();
    bslx::ByteOutStream stream(versionSelector, basicAllocator);
    bdexStreamOut(stream, now, 1);
    int streamedLength = stream.length();
    msg.setLength(sizeof(int) + streamedLength);
    btlb::BlobUtil::append(&msg, stream.data(), sizeof(int), streamedLength);
    char *buffer = msg.buffer(0).data();
    int tmp = htonl(streamedLength + sizeof(int));
    memcpy(buffer, &tmp, sizeof(int));
}

class my_QueueClient {
    // This class implements a queue-based message TCP/IP server.

    enum {
        CLIENT_BASE_ID = 0xAB      // Actual clientId is between this and
                                   // CLIENT_BASE_ID + d_numConnections.
    };

    btlmt::ChannelPoolConfiguration  d_config;          // pool's configuration
    btlmt::ChannelPool              *d_channelPool_p;   // managed pool
    bslma::Allocator                *d_allocator_p;     // memory manager
    bslmt::Mutex                    *d_coutLock_p;
    bdlcc::Queue<BlobTypeWithId>    *d_incomingQueue_p;
    bdlcc::Queue<BlobTypeWithId>    *d_outgoingQueue_p;
    bslmt::ThreadUtil::Handle        d_processorHandle;
    bsls::AtomicOperations::AtomicTypes::Int
                                     d_runningFlag;
    btlb::Blob                       d_initialMessage;
    btlso::IPv4Address               d_peer;
    int                              d_numConnections;

  private:
    // Callback functions:
    void poolStateCb(int state, int source, int severity);
    void channelStateCb(int channelId, int sourceId, int state, void *context);
    void blobCB(int *numNeeded, btlb::Blob *msg, int channelId, void *);

    // Not implemented:
    my_QueueClient(const my_QueueClient& original);        // Not impl.
    my_QueueClient& operator=(const my_QueueClient& rhs);  // Not impl.
  public:
    my_QueueClient(bdlcc::Queue<BlobTypeWithId> *incomingQueue,
                   bdlcc::Queue<BlobTypeWithId> *outgoingQueue,
                   bslmt::Mutex                 *coutLock,
                   const char                   *hostname,
                   int                           portNumber,
                   int                           numConnections,
                   const btlb::Blob&             initialMessage,
                   bslma::Allocator             *basicAllocator = 0);

    ~my_QueueClient();
        // Destroy this server.

    int processOutgoingQueue();

    int shutdown(int channelId) {
        return d_channelPool_p->shutdown(channelId,
                                         btlmt::ChannelPool::e_IMMEDIATE);
    }

    int startProcessor();

    int stopProcessor();

    // MANIPULATORS
    const btlmt::ChannelPool& pool() { return *d_channelPool_p; }
};

extern "C" {
    static void *queueClientProc(void *arg) {
        my_QueueClient *proc = (my_QueueClient*)arg;
        ASSERT(proc);
        return (void*)proc->processOutgoingQueue();
    }
}

my_QueueClient::my_QueueClient(bdlcc::Queue<BlobTypeWithId> *incomingQueue,
                               bdlcc::Queue<BlobTypeWithId> *outgoingQueue,
                               bslmt::Mutex                 *coutLock ,
                               const char                   *hostname,
                               int                           portNumber,
                               int                           numConnections,
                               const btlb::Blob&             initialMessage,
                               bslma::Allocator             *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_coutLock_p(coutLock)
, d_incomingQueue_p(incomingQueue)
, d_outgoingQueue_p(outgoingQueue)
, d_initialMessage(initialMessage)
, d_peer(hostname, portNumber)
, d_numConnections(numConnections)
{
    bsls::AtomicOperations::initInt(&d_runningFlag, 0);
    d_config.setMaxThreads(4);
    d_config.setMaxConnections(numConnections);
    d_config.setReadTimeout(5.0);      // in seconds
    d_config.setMetricsInterval(10.0); // seconds
    d_config.setWriteCacheWatermarks(0, 1<<10); // 1Mb
    d_config.setIncomingMessageSizes(4, 100, 1024);

    if (verbose) {
        P(d_config);
    }

    btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                bsl::allocator_arg_t(),
                bsl::allocator<btlmt::ChannelPool::ChannelStateChangeCallback>(
                                                               basicAllocator),
                bdlf::MemFnUtil::memFn(&my_QueueClient::channelStateCb, this));

    btlmt::ChannelPool::PoolStateChangeCallback    poolCb(
                   bsl::allocator_arg_t(),
                   bsl::allocator<btlmt::ChannelPool::PoolStateChangeCallback>(
                                                               basicAllocator),
                   bdlf::MemFnUtil::memFn(&my_QueueClient::poolStateCb, this));

    btlmt::ChannelPool::BlobBasedReadCallback      dataCb(
                     bsl::allocator_arg_t(),
                     bsl::allocator<btlmt::ChannelPool::BlobBasedReadCallback>(
                                                               basicAllocator),
                     bdlf::MemFnUtil::memFn(&my_QueueClient::blobCB, this));

    d_channelPool_p = new (*d_allocator_p) btlmt::ChannelPool(channelCb,
                                                              dataCb,
                                                              poolCb,
                                                              d_config,
                                                              basicAllocator);
    ASSERT(0 == d_channelPool_p->start());
    for (int i = 0; i < numConnections; ++i) {
        MTCOUT << "Connecting to " << hostname << ":" << portNumber << MTENDL;
        ASSERT(0 == d_channelPool_p->connect(d_peer,
                                             10,
                                             bsls::TimeInterval(5.0),
                                             CLIENT_BASE_ID + i));
    }
}

my_QueueClient::~my_QueueClient() {
    d_channelPool_p->stop();
    d_allocator_p->deleteObjectRaw(d_channelPool_p);
}

int my_QueueClient::processOutgoingQueue() {
    while (bsls::AtomicOperations::getInt(&d_runningFlag)) {
        bsls::TimeInterval TIMEOUT(bdlt::CurrentTime::now());
        TIMEOUT += 5.0; // 5s
        BlobTypeWithId msg;
        if (0 == d_outgoingQueue_p->timedPopBack(&msg, TIMEOUT)) {
            ASSERT(0 == d_channelPool_p->write(msg.first, msg.second));
        }
    }
    return 0;
}

int my_QueueClient::startProcessor() {
    bslmt::ThreadAttributes attributes;
    attributes.setDetachedState(bslmt::ThreadAttributes::e_CREATE_JOINABLE);
    bsls::AtomicOperations::setInt(&d_runningFlag, 1);
    return bslmt::ThreadUtil::create(&d_processorHandle, attributes,
                                     &queueClientProc, (void*)this);
}

int my_QueueClient::stopProcessor() {
    bsls::AtomicOperations::setInt(&d_runningFlag, 0);
    return bslmt::ThreadUtil::join(d_processorHandle);
}

void my_QueueClient::poolStateCb(int state, int source, int severity) {

    if (veryVerbose) {
        MTCOUT << "Pool state changed ("
               << source << ", " << severity << ", " << state
               << ")." << MTENDL;
    }
}

void my_QueueClient::channelStateCb(int   channelId,
                                    int   sourceId,
                                    int   state,
                                    void *)
{
    ASSERT(CLIENT_BASE_ID <= sourceId &&
           sourceId < CLIENT_BASE_ID + d_numConnections);

    switch(state) {
      case btlmt::ChannelPool::e_CHANNEL_DOWN: {
          // btlso::IPv4Address peer;
          // d_channelPool_p->getPeerAddress(&peer, channelId);
          if (veryVerbose) {
              MTCOUT << "Client disconnect from " << d_peer
                     << " on channel " << channelId << "." << MTENDL;
          }
          ASSERT(0 == d_channelPool_p->connect(d_peer,
                                               10,
                                               bsls::TimeInterval(5.0),
                                               sourceId));
          if (veryVerbose) {
              MTCOUT << "Re-establishing connection." << MTENDL;
          }
      } break;
      case btlmt::ChannelPool::e_CHANNEL_UP: {
          btlso::IPv4Address peer;
          d_channelPool_p->getPeerAddress(&peer, channelId);
          if (veryVerbose) {
              MTCOUT << "Client connected to " << peer
                     << " on channel " << channelId << "." << MTENDL;
          }
          if (0 != d_channelPool_p->write(channelId, d_initialMessage)) {
              d_channelPool_p->shutdown(channelId,
                                        btlmt::ChannelPool::e_IMMEDIATE);
          }
      } break;
    }
}

void my_QueueClient::blobCB(int        *numNeeded,
                            btlb::Blob *msg,
                            int         channelId,
                            void       *)
{
    ASSERT(numNeeded);
    ASSERT(msg);
    ASSERT(0 < msg->length());

    int numConsumed = QUEUE_PROCESSOR_NAMESPACE::parseMessages(numNeeded, msg);

    if (numConsumed) {
        BlobTypeWithId data(channelId, *msg);
        d_incomingQueue_p->pushBack(data);
    }
}


struct caseMinusOneInfo {
    int   d_numConnections;
    int   d_portNumber;
    char *d_hostname;
    int   d_numMessages;
    int   d_numIters;
};

extern "C"
void *usageExampleMinusOne(void *arg)
{
    caseMinusOneInfo *info = (caseMinusOneInfo *) arg;

    const int NUM_CONNECTIONS = info->d_numConnections;
    const int PORT_NUMBER     = info->d_portNumber;
    char * const HOSTNAME     = info->d_hostname;
    const int NUM_MESSAGES    = info->d_numMessages;
    const int NUM_ITERS       = info->d_numIters;

    bslma::TestAllocator ta(veryVeryVerbose);
    if (verbose) {
        P(NUM_CONNECTIONS);
        P(HOSTNAME);
        P(PORT_NUMBER);
        P(NUM_MESSAGES);
    }
    bdlcc::Queue<BlobTypeWithId> incoming, outgoing;

    btlb::PooledBlobBufferFactory blobFactory(100, &ta);
    btlb::Blob                    blob(&blobFactory);
    btlb::Blob                    initialMessage(blob, &ta);

    generateMessage(initialMessage, &ta);

    my_QueueClient qp(&incoming, &outgoing, &coutMutex,
                      HOSTNAME, PORT_NUMBER, NUM_CONNECTIONS,
                      initialMessage);

    ASSERT(0 == qp.startProcessor());

    typedef bsl::vector<bsls::TimeInterval> my_ChannelStats;
    bsl::map<int, my_ChannelStats> channelStats(bsl::less<int>(),
                                                &ta);

    int numConnections = NUM_CONNECTIONS * NUM_ITERS;
    while (numConnections) {
        using bslx::InStreamFunctions::bdexStreamIn;

        BlobTypeWithId msg = incoming.popFront();

        // Process message here
        bsls::TimeInterval now = bdlt::CurrentTime::now();
        char *msgData = msg.second.buffer(0).data();
        msgData += sizeof(int);
        bslx::ByteInStream stream(msgData, msg.second.length()
                                     - sizeof(int));
        bsls::TimeInterval initialTime;
        bdexStreamIn(stream, initialTime, 1);
        now -= initialTime;
        int channelId = msg.first;
        if (veryVerbose) {
            MTCOUT << "Processed message from " << channelId << "."
                   << " Delay = " << now << MTENDL;
        }
        if (veryVerbose) {
            T_(); PT(initialTime);
        }
        channelStats[channelId - 1].push_back(now);
        if ((int) channelStats[channelId - 1].size() < NUM_MESSAGES) {
            generateMessage(msg.second, &ta);
            outgoing.pushBack(msg);
        }
        else {
            int i = 0;
            double delay = 0.0;
            for (; i < (int) channelStats[channelId - 1].size(); ++i) {
                delay += channelStats[channelId - 1][i].totalSecondsAsDouble();
            }
            delay = delay / i;
            if (verbose) {
                MTCOUT << "Processed message from " << channelId << "."
                    << " Average delay = " << delay << MTENDL;
            }
            channelStats[channelId - 1].clear();
            qp.shutdown(channelId);
            --numConnections;
        }
    }
    ASSERT(0 == qp.stopProcessor());
    return NULL;
}

}  // close namespace QUEUE_CLIENT_NAMESPACE


// ============================================================================
//                     GLOBAL 'class' FOR TESTING
// ----------------------------------------------------------------------------

class TestDriver {
    // This 'class' provide a namespace for testing 'btlmt::ChannelPool's
    // functionality.

  public:
    // TEST CASES
    static void testCase37();
        // Test usage example.

    static void testCase36();
        // Test the new constructor form that takes a BlobBufferFactory.

    static void testCase35();
        // Test that 'stopAndRemoveAllChannels' correctly releases all
        // resources held by channels.

    static void testCase34();
        // Test that 'BTEMT_WRITE_CACHE_LOWWAT' is invoked after the
        // 'enqueueWatermark' is exceeded on the 'write'.

    static void testCase33();
        // Test that additional write statistics are reported as expected.

    static void testCase32();
        // Test that 'peerAddress' is correctly returned.

    static void testCase31();
        // Test that 'disableRead' when called from dispatcher thread stops
        // reading from the socket.

    static void testCase30();
        // Test a queue processor implementation.

    static void testCase29();
        // Test that channel pool does not read extra data even after
        // numNeeded is set to 0.

    static void testCase28();
        // Test that providing socket options sets the appropriate options on
        // the socket.

    static void testCase27();
        // Test that reading into a partially filled blob works as expected.

    static void testCase26();
        // Test that blob-based 'read' works as expected.

    static void testCase25();
        // Test that the allocation of event managers works as expected.

    static void testCase24();
        // Test that the time metrics are correctly computed.

    static void testCase23();
        // Test that read operations time out correctly.

    static void testCase22();
        // Test that 'btlmt::ChannelPool_MessageUtil' class works as expected.

    static void testCase21();
        // Test that 'setWriteCache[Hi|Low]Watermark' and
        // 'setWriteCacheWatermarks' work as expected.

    static void testCase20();
        // Test that reading from OpenSSL sockets works as expected.

    static void testCase19();
        // Test that 'import' works under high load.

    static void testCase18();
        // Test that channel pool fails to accept connections when its file
        // descriptor limit is reached.

    static void testCase17();
        // Test that channel pool only creates 'maxConnections' channels.

    static void testCase16();
        // Test that CHANNEL_UP and CHANNEL_DOWN callbacks are correctly
        // delivered.

    static void testCase15();
        // Test that 'shutdown' is correctly processed from within a channel
        // state callback.

    static void testCase14();
        // Test that the 'getHandleStatistics' returns the correct statistics.

    static void testCase13();
        // Test that 'numBytes*', 'totalBytes*' and related accessors return
        // the right values.

    static void testCase12();
        // Test that 'reportWeightedAverageReset' works as expected.

    static void testCase11();
        // Test that HIWAT and LOWWAT callbacks are correctly invoked.

    static void testCase10();
        // Test that 'enableRead' and 'disableRead' work as expected.

    static void testCase9();
        // Test that 'registerClock' AND 'deregisterClock' work as expected.

    static void testCase8();
        // Test that 'writeMessage' works as expected.

    static void testCase7();
        // Test that 'shutdown' works as expected.

    static void testCase6();
        // Test that a half-closed socket pair can be correctly imported and
        // that the channel is correctly recognized as half-closed.

    static void testCase5();
        // Test that 'import' works as expected.

    static void testCase4();
        // Test that 'listen' works as expected.

    static void testCase3();
        // Test that 'connect' works as expected.

    static void testCase2();
        // Test that 'start' and 'stop' work as expected.

    static void testCase1();
        // Breathing test.
};


                               // --------------
                               // TEST APPARATUS
                               // --------------

void TestDriver::testCase37()
{
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.  Additionally, replace all calls to 'assert' in the usage
        //   example with calls to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING USAGE EXAMPLE - AN ECHO SERVER"
                 << "\n======================================" << endl;

        using namespace USAGE_EXAMPLE_NAMESPACE;

        enum {
            MAX_CONNECTIONS = 1000,
            NUM_MONITOR     = 10
        };

        my_EchoServer echoServer(&coutMutex, 0, MAX_CONNECTIONS);

        if (verbose) {
            MTCOUT << "monitor pool: count=" << NUM_MONITOR << MTENDL;
        }
        monitorPool(&coutMutex, echoServer.pool(), NUM_MONITOR);
}

void TestDriver::testCase36()
{
    // --------------------------------------------------------------------
    // TESTING: constructor that accepts BlobBufferFactory
    //
    // Concerns:
    //: 1 Blob buffers are dispensed from the provided factory and outlive
    //:   the channel pool.
    //:
    //: 2 No memory leaks in this usage mode
    //
    // Plan:
    //: 1 Create a BlobBufferFactory.
    //:
    //: 2 Create a TestAllocator and an enclosing scope for the
    //:   channel pool.
    //:
    //: 3 Using the scope and allocator from step 2, and the blob buffer
    //:   factory from step 1, create a channel pool object.
    //:
    //: 4 Open a channel by connecting to a listening socket configured
    //:   in step 3.
    //:
    //: 5 Send a message through the channel to a callback that saves the
    //:   resulting blob outside the scope created in step 2.
    //:
    //: 6 Shut down the pool and close the scope from step 2.
    //:
    //: 7 Verify the contents of the blob are still valid, and that no
    //:   memory has been leaked.
    //:
    //
    // Testing:
    //   btlmt::ChannelPool(bslma::ManagedPtr<btlb::BlobBufferFactory>*,
    //                      ChannelStateChangeCallback,
    //                      BlobBasedReadCallback,
    //                      PoolStateChangeCallback,
    //                      const btlmt::ChannelPoolConfiguration&,
    //                      bslma::Allocator *);
    //-------------------------------------------------------------------

    bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
    bslma::DefaultAllocatorGuard taGuard(&da);
    {
        if (verbose) {
            cout << "\nTESTING constructor with BlobBufferFactory"
                 << "\n=========================================="
                 << endl;
        }
        using namespace TEST_CASE_CTOR_TAKING_FACTORY;

        btlmt::ChannelPoolConfiguration config;
        config.setMaxThreads(1);

        config.setReadTimeout(0);        // in seconds
        if (verbose) {
            P(config);
        }

        bslmt::Barrier  channelCbBarrier(2);
        int             channelId;

        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                    bdlf::BindUtil::bind(&channelStateCb,
                                                        _1, _2, _3, _4,
                                                        &channelId,
                                                        &channelCbBarrier));

        btlmt::ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

        bslma::TestAllocator ta("testAllocator", veryVeryVerbose);

        const int SIZE = 4444;
        const char FILL = 0xBB;
        BSLS_ASSERT(0 == SIZE%2); // test invariant
        btlb::PooledBlobBufferFactory factory(SIZE/2);

        btlb::Blob    blob;
        bslmt::Barrier  dataCbBarrier(2);

        btlmt::ChannelPool::BlobBasedReadCallback dataCb(
                                     bdlf::BindUtil::bind(&blobBasedReadCb,
                                                         _1, _2, _3, _4,
                                                         &dataCbBarrier,
                                                         &blob));
        {
            ReadServer server(&factory, SIZE, &ta);
            ASSERT(0 == server.start());
            const int PORT = server.portNumber();
            btlso::IPv4Address address(getLocalAddress());
            address.setPortNumber(PORT);

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                            factory.allocate();

            ASSERT(0 == socket->connect(address));
            ASSERT(0 == socket->setBlockingMode(
                                          btlso::Flag::e_BLOCKING_MODE));

            char data[SIZE];
            bsl::memset(data, FILL, SIZE);
            ASSERT(SIZE == socket->write(data, SIZE));

            server.msgWait();
            blob = server.lastMsg();
            factory.deallocate(socket);
        }
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 != ta.numAllocations());

        LOOP_ASSERT(blob.length(), SIZE == blob.length());
        for (int i = 0; i < blob.numBuffers(); ++i) {
            ASSERT(SIZE/2 == blob.buffer(i).size());
            for (int j = 0; j < SIZE/2; ++j) {
                LOOP3_ASSERT(i, j, blob.buffer(i).data()[j],
                             FILL == blob.buffer(i).data()[j]);
            }
        }
    }
    ASSERT(0 == da.numBytesInUse());
    ASSERT(0 != da.numAllocations());
}

void TestDriver::testCase35()
{
        // --------------------------------------------------------------------
        // Testing 'stopAndRemoveAllChannels'
        //
        // Concerns:
        //: 1 All running worker threads are stopped after the call.
        //:
        //: 2 Any resources associated with any channel is released.
        //
        // Plan:
        //: 1 Create a channel pool object, mX.
        //:
        //: 2 Open a pre-defined number of listening sockets in mX.
        //:
        //: 3 Create a number of threads that each connect to one of the
        //:   listening sockets in mX.
        //:
        //: 4 Create a number of threads that each listen on a socket.
        //:
        //: 5 Open a channel in mX by connecting a listening sockets created
        //:   in step 4.
        //:
        //: 6 Write large amount of data through mX across all the open
        //:   channels.li
        //:
        //: 7 Invoke 'stopAndRemoveAllChannels' and confirm that no channels
        //:   or threads are outstanding.
        //:
        //
        // Testing:
        //   int stopAndRemoveAllChannels();
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'stopAndRemoveAllChannels'"
                 << "\n=================================="
                 << endl;

        using namespace TEST_CASE_STOP_AND_REMOVE_CHANNELS;

        btlmt::ChannelPoolConfiguration config;
        config.setMaxThreads(4 * NT);
        config.setWriteCacheWatermarks(0, 1024 * 1024 * 1024);
        config.setReadTimeout(0);        // in seconds
        config.setCollectTimeMetrics(false);
        if (verbose) {
            P(config);
        }

        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                            _1, _2, _3, _4));

        btlmt::ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

        btlmt::ChannelPool::BlobBasedReadCallback dataCb(
                                     bdlf::BindUtil::bind(&blobBasedReadCb,
                                                          _1, _2, _3, _4));

        Obj mX(channelCb, dataCb, poolCb, config);

        ASSERT(0 == mX.start());

        const int SERVER_ID = 100;

        for (int i = 0; i < NT; ++i) {
            ASSERT(0 == mX.listen(0, 1, SERVER_ID + i));
        }

        bslmt::ThreadUtil::Handle connectThreads[NT];
        ConnectData               connectData[NT];

        for (int i = 0; i < NT; ++i) {
            connectData[i].d_index = i;
            connectData[i].d_serverAddress = getServerLocalAddress(
                                                               &mX,
                                                               SERVER_ID + i);
            ASSERT(0 == bslmt::ThreadUtil::create(&connectThreads[i],
                                                  &connectFunction,
                                                  (void *) &connectData[i]));
        }

        for (int i = 0; i < NT; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(connectThreads[i]));
        }

        bslmt::ThreadUtil::Handle listenThreads[NT];
        ListenData                listenData[NT];

        for (int i = 0; i < NT; ++i) {
            listenData[i].d_index = i;
            ASSERT(0 == bslmt::ThreadUtil::create(&listenThreads[i],
                                                  &listenFunction,
                                                  (void *) &listenData[i]));

            bslmt::ThreadUtil::microSleep(100, 0);
        }

        bslmt::ThreadUtil::microSleep(0, 1);

        const int CLIENT_ID = 200;

        for (int i = 0; i < NT; ++i) {
            btlso::IPv4Address serverAddr;
            ASSERT(0 == serverSockets[i]->localAddress(&serverAddr));

            ASSERT(0 == mX.connect(serverAddr,
                                   10,
                                   bsls::TimeInterval(1),
                                   CLIENT_ID + i));
        }

        for (int i = 0; i < NT; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(listenThreads[i]));
        }

        for (int i = 0; i < NT; ++i) {
            btlso::StreamSocket<btlso::IPv4Address> *socket = clientSockets[i];

            ASSERT(0 == bslmt::ThreadUtil::create(&connectThreads[i],
                                                  &dataFunction,
                                                  (void *) socket));
        }

        for (int i = 0; i < NT; ++i) {
            btlso::StreamSocket<btlso::IPv4Address> *socket = acceptSockets[i];

            ASSERT(0 == bslmt::ThreadUtil::create(&listenThreads[i],
                                                  &dataFunction,
                                                  (void *) socket));
        }

        bslmt::ThreadUtil::microSleep(0, 1);

        const int SIZE      = 1024 * 1024;
        const int NUM_BYTES = SIZE * 10;
        btlb::PooledBlobBufferFactory f(SIZE);
        btlb::Blob                    b(&f);
        b.setLength(NUM_BYTES);

        mapMutex.lock();
        for (int i = 0; i < NT; ++i) {
            MapIter iter = sourceIdToChannelIdMap.find(SERVER_ID + i);
            ASSERT(iter != sourceIdToChannelIdMap.end());
            ASSERT(0 == mX.write(iter->second, b));

            iter = sourceIdToChannelIdMap.find(CLIENT_ID + i);
            ASSERT(iter != sourceIdToChannelIdMap.end());
            ASSERT(0 == mX.write(iter->second, b));
        }
        mapMutex.unlock();

        ASSERT(0 != mX.numChannels());
        ASSERT(0 != mX.numThreads());

        bsls::Types::Int64 numRead = 0, numWritten = 0;
        mX.totalBytesRead(&numRead);
        mX.totalBytesWritten(&numWritten);

        ASSERT(0 == mX.stopAndRemoveAllChannels());

        ASSERT(0 == mX.numChannels());
        ASSERT(0 == mX.numThreads());
}

void TestDriver::testCase34()
{
        // --------------------------------------------------------------------
        // Testing LOWWAT called when 'enqueueWatermark' exceeded
        //
        // Concerns:
        //: 1 The low-water mark is invoked after the enqueue water mark is
        //:   exceeded and the write cache size drops below the low-water mark.
        //
        // Plan:
        //: 1 Write a message greater than the enqueue cache size and confirm
        //:   that the low-water mark is invoked after the write completes.
        //:   Repeat a similar write again and assert that low-water mark is
        //:   invoked.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING LOWWAT called when 'enqueueWatermark' exceeded"
                 << "\n======================================================"
                 << endl;

        using namespace TEST_CASE_LOWWAT_AFTER_ENQUEUEMARK_EXCEEDED;

        btlmt::ChannelPoolConfiguration config;
        config.setMaxThreads(1);
        config.setWriteCacheWatermarks(0, 1024 * 1024 * 1024);
        config.setReadTimeout(0);        // in seconds
        if (verbose) {
            P(config);
        }

        bslmt::Barrier  channelCbBarrier(2);
        int             channelId;
        int             numTimesLowWatCalled = 0;
        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                         _1, _2, _3, _4,
                                                         &channelId,
                                                         &numTimesLowWatCalled,
                                                         &channelCbBarrier));

        btlmt::ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

        bslmt::Barrier  dataCbBarrier(2);

        btlmt::ChannelPool::BlobBasedReadCallback dataCb(
                                     bdlf::BindUtil::bind(&blobBasedReadCb,
                                                         _1, _2, _3, _4,
                                                         &dataCbBarrier));

        btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

        ASSERT(0 == pool.start());

        const int SERVER_ID = 100;

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlso::StreamSocket<btlso::IPv4Address> *socket = factory.allocate();
        ASSERT(0 == socket->bind(getLocalAddress()));
        ASSERT(0 == socket->listen(5));

        btlso::IPv4Address serverAddr;
        ASSERT(0 == socket->localAddress(&serverAddr));

        int rc = pool.connect(serverAddr,
                              10,
                              bsls::TimeInterval(1),
                              SERVER_ID);
        ASSERT(!rc);

        ASSERT(0 == socket->setBlockingMode(btlso::Flag::e_BLOCKING_MODE));

        btlso::StreamSocket<btlso::IPv4Address> *client;
        rc = socket->accept(&client);
        ASSERT(!rc);
        ASSERT(0 == client->setBlockingMode(
                                          btlso::Flag::e_NONBLOCKING_MODE));

        channelCbBarrier.wait();

        const int SIZE = 1024 * 1024 * 10;  // 10 MB
        btlb::PooledBlobBufferFactory f(SIZE);
        btlb::Blob                    b(&f);
        b.setLength(SIZE);

        rc = pool.write(channelId, b);
        ASSERT(!rc);

#ifdef BSLS_PLATFORM_OS_WINDOWS
        rc = pool.write(channelId, b);

        rc = pool.write(channelId, b);
#endif

        rc = pool.write(channelId, b, 100);
        ASSERT(rc);

        ReadData rd;
        rd.d_socket_p      = client;
        rd.d_numBytes      = SIZE;

        bslmt::ThreadUtil::Handle  handle;
        bslmt::ThreadUtil::create(&handle, &readData, &rd);

        channelCbBarrier.wait();

        ASSERT(0 == bslmt::ThreadUtil::join(handle));
        LOOP_ASSERT(numTimesLowWatCalled, 1 == numTimesLowWatCalled);

        rc = pool.write(channelId, b);
        ASSERT(!rc);

        rc = pool.write(channelId, b, 100);
        ASSERT(rc);

        bslmt::ThreadUtil::create(&handle, &readData, &rd);

        channelCbBarrier.wait();

        ASSERT(0 == bslmt::ThreadUtil::join(handle));
        LOOP_ASSERT(numTimesLowWatCalled, 2 == numTimesLowWatCalled);
}

void TestDriver::testCase33()
{
        // --------------------------------------------------------------------
        // ADDING WRITE STATISTICS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nADDING WRITE STATISTICS"
                 << "\n======================="
                 << endl;

        using namespace TEST_CASE_ADDING_WRITE_STATS;

        btlmt::ChannelPoolConfiguration config;
        config.setMaxThreads(1);
        config.setWriteCacheWatermarks(0, 1024 * 1024 * 1024);
        config.setReadTimeout(0);        // in seconds
        if (verbose) {
            P(config);
        }

        bslmt::Barrier  channelCbBarrier(2);
        int             channelId;
        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

        btlmt::ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

        string         data;
        bslmt::Barrier dataCbBarrier(2);

        btlmt::ChannelPool::BlobBasedReadCallback dataCb(
                                     bdlf::BindUtil::bind(&blobBasedReadCb,
                                                         _1, _2, _3, _4,
                                                         &data,
                                                         &dataCbBarrier));

        btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

        ASSERT(0 == pool.start());

        const int SERVER_ID = 100;

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlso::StreamSocket<btlso::IPv4Address> *socket = factory.allocate();
        ASSERT(0 == socket->bind(getLocalAddress()));
        ASSERT(0 == socket->listen(5));

        btlso::IPv4Address serverAddr;
        ASSERT(0 == socket->localAddress(&serverAddr));

        int rc = pool.connect(serverAddr,
                              10,
                              bsls::TimeInterval(1),
                              SERVER_ID);
        ASSERT(!rc);

        channelCbBarrier.wait();

        btlso::StreamSocket<btlso::IPv4Address> *client;
        rc = socket->accept(&client);
        ASSERT(!rc);
        ASSERT(0 == client->setBlockingMode(
                                          btlso::Flag::e_NONBLOCKING_MODE));

        const int SIZE = 1000;
        btlb::PooledBlobBufferFactory f(SIZE);
        btlb::Blob                    b(&f);
        b.setLength(SIZE * 10);

        WriteData wd;
        wd.d_pool_p    = &pool;
        wd.d_channelId = channelId;
        wd.d_socket_p  = client;
        wd.d_blob_p    = &b;

        const int NUM_THREADS = 5;
        bslmt::ThreadUtil::Handle handles[NUM_THREADS + 1];
        for (int i = 0; i < NUM_THREADS; ++i) {
            bslmt::ThreadUtil::create(&handles[i], &writeData, &wd);
        }

        bslmt::Mutex  mutex;
        int         stopReading = 0;

        ReadData rd;
        rd.d_socket_p      = client;
        rd.d_mutex_p       = &mutex;
        rd.d_stopReading_p = &stopReading;

        bslmt::ThreadUtil::create(&handles[NUM_THREADS], &readData, &rd);

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(handles[i]));
        }

        mutex.lock();
        stopReading = 1;
        mutex.unlock();

        ASSERT(0 == bslmt::ThreadUtil::join(handles[NUM_THREADS]));
}

void TestDriver::testCase32()
{
        // --------------------------------------------------------------------
        // Testing 'peerAddress' is correctly returned
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'peerAddress' is correctly returned"
                 << "\n==========================================="
                 << endl;

        using namespace TEST_CASE_TESTING_PEER_ADDRESS;

        btlmt::ChannelPoolConfiguration config;
        config.setMaxThreads(1);
        config.setWriteCacheWatermarks(0, 1024 * 1024);
        config.setReadTimeout(0);        // in seconds
        if (verbose) {
            P(config);
        }

        bslmt::Barrier channelCbBarrier(2);
        int            channelId;
        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                         _1, _2, _3, _4,
                                                         &channelId,
                                                         &channelCbBarrier));

        btlmt::ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

        bslmt::Barrier dataCbBarrier(2);

        btlmt::ChannelPool::BlobBasedReadCallback dataCb(
                                     bdlf::BindUtil::bind(&blobBasedReadCb,
                                                         _1, _2, _3, _4,
                                                         &dataCbBarrier));

        btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);
        d_pool_p = &pool;

        ASSERT(0 == pool.start());

        const btlso::IPv4Address ADDRESS("127.0.0.1", 0);

        enum {
            SERVER_ID           = 1013410001,
            BACKLOG             = 100
        };

        int rc = pool.listen(ADDRESS, BACKLOG, SERVER_ID);
        LOOP_ASSERT(rc, !rc);

        btlso::IPv4Address SA = getServerLocalAddress(&pool, SERVER_ID);

        typedef btlso::StreamSocket<btlso::IPv4Address>            Socket;
        typedef btlsos::TcpChannel                                Channel;
        typedef btlso::InetStreamSocketFactory<btlso::IPv4Address> Factory;

        Factory factory;

        Socket  *socket = factory.allocate();
        Channel  channel(socket);

        rc = socket->connect(SA);
        ASSERT(!rc);

        channelCbBarrier.wait();
        ASSERT(-1 != channelId);

        btlso::IPv4Address exp;  const btlso::IPv4Address& EXP = exp;
        rc = pool.getPeerAddress(&exp, channelId);
        ASSERT(!rc);

        const char data[] = "socket";
        rc = socket->write(data, sizeof data);
        ASSERT(rc);

        btlso::IPv4Address peer;  const btlso::IPv4Address& PEER = peer;
        rc = pool.getPeerAddress(&peer, channelId);
        ASSERT(!rc);
        LOOP2_ASSERT(EXP, PEER, EXP == PEER);

        rc = socket->write(data, sizeof data);
        ASSERT(rc);

        rc = pool.getPeerAddress(&peer, channelId);
        ASSERT(!rc);
        LOOP2_ASSERT(EXP, PEER, EXP == PEER);

        rc = socket->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
        ASSERT(!rc);

        btlso::IPv4Address other;
        rc = pool.getPeerAddress(&other, channelId);

        channelCbBarrier.wait();
}

void TestDriver::testCase31()
{
        // --------------------------------------------------------------------
        // TESTING 'disableRead'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int disableRead(int channelId);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'disableRead'"
                          << "\n====================="
                          << endl;

        using namespace TEST_CASE_DISABLE_READ;

        btlmt::ChannelPoolConfiguration config;
        config.setMaxThreads(1);
        config.setReadTimeout(0);        // in seconds
        if (verbose) {
            P(config);
        }

        int channelId;
        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                            _1, _2, _3, _4,
                                                            &channelId));

        btlmt::ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

        bslmt::Barrier barrier(2);
        int numTimesDataCbCalled = 0;
        btlmt::ChannelPool::BlobBasedReadCallback dataCb(
                                     bdlf::BindUtil::bind(
                                                       &blobBasedReadCb,
                                                       _1, _2, _3, _4,
                                                       &numTimesDataCbCalled,
                                                       &barrier));

        btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

        ASSERT(0 == pool.start());

        const int SERVER_ID = 100;

        btlso::IPv4Address serverAddr;
        int rc = pool.listen(serverAddr, 5, SERVER_ID);
        ASSERT(!rc);

        serverAddr = getServerLocalAddress(&pool, SERVER_ID);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlso::StreamSocket<btlso::IPv4Address> *socket = factory.allocate();

        ASSERT(0 == socket->connect(serverAddr));

        d_pool_p = &pool;

        const int  LEN  = 1024 * 10;
        char       TEXT[LEN];

        populateText(TEXT, LEN);

        const int NUM_TIMES = 100;

        socket->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
        for (int i = 0; i < NUM_TIMES; ++i) {
            int rc = socket->write(TEXT, LEN);
            if (rc < 0) {
                break;
            }
        }

        // Wait for the dispatcher thread to process the deregister the READ
        // event.

        barrier.wait();

        LOOP_ASSERT(numTimesDataCbCalled, 1 == numTimesDataCbCalled);

        pool.enableRead(channelId);

        barrier.wait();

        LOOP_ASSERT(numTimesDataCbCalled, 2 == numTimesDataCbCalled);

        pool.enableRead(channelId);

        barrier.wait();

        LOOP_ASSERT(numTimesDataCbCalled, numTimesDataCbCalled > 2);

        if (veryVerbose) {
            P(dataStream.str())
        }
}

void TestDriver::testCase30()
{
        // --------------------------------------------------------------------
        // Implementing a QueueProcessor
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose)
            cout << "\nIMPLEMENTING A QUEUE PROCESSOR"
                 << "\n==============================" << endl;

        using namespace QUEUE_PROCESSOR_NAMESPACE;
        using namespace QUEUE_CLIENT_NAMESPACE;

        if (verbose) {
            cout << "In another window, run this test driver's case -1, e.g.:";
            cout << "\n\t" << ARGV[0] << " -1  10  2564  127.0.0.1  10000\n";
            cout << "For bigger jobs (i.e, stress test), try:";
            cout << "\n\t" << ARGV[0] << " -1  10  2564  127.0.0.1  10000\n";
            cout << "For non-null testing in verbose mode, try:";
            cout << "\n\t" << ARGV[0] << " 24 -1\n";
        }

        enum {
            NUM_CONNECTIONS = 3,
            MAX_CONNECTIONS = 10,
            NUM_MESSAGES    = 3,  // per connection
            NUM_ITERS       = 3
        };

        bslma::TestAllocator ta(veryVeryVerbose);
        bdlma::ConcurrentPool pool(100, &ta);

        bdlcc::Queue<BlobTypeWithId> incoming, outgoing;
        my_QueueProcessor qp(&incoming, &outgoing, &coutMutex,
                             0, MAX_CONNECTIONS, &ta);
        ASSERT(0 == qp.startProcessor());
        const int PORT_NUMBER = qp.portNumber();

        bslmt::ThreadUtil::Handle usageMinusOneHandle;
        if (verbose < 0) {
            caseMinusOneInfo info;
            info.d_numConnections = NUM_CONNECTIONS;
            info.d_portNumber     = PORT_NUMBER;
            info.d_hostname       = const_cast<char *>("127.0.0.1");
            info.d_numMessages    = NUM_MESSAGES;
            info.d_numIters       = NUM_ITERS;
            bslmt::ThreadUtil::create(&usageMinusOneHandle,
                                     &usageExampleMinusOne,
                                     (void *)&info);
        }

        bslmt::ThreadUtil::microSleep(100); // 1s, to let my_QueueClients
                                            // enqueue messages into incoming
                                            // queue.

        while (0 < incoming.queue().length()) {
            BlobTypeWithId msg = incoming.popFront(); // get msg from client
            if (veryVerbose) {
                PT(msg.second.length());
            }
            ASSERT(msg.second.length());
            if (veryVeryVerbose) {
                MTCOUT << "Processing message from "
                       << msg.first << MTENDL;
            }
            bslmt::ThreadUtil::microSleep(100);
            outgoing.pushBack(msg); // will send back to corresponding client
        }

        if (verbose < 0) {
            bslmt::ThreadUtil::join(usageMinusOneHandle);
        }
        ASSERT(0 == qp.stopProcessor());

}

void TestDriver::testCase29()
{
        // --------------------------------------------------------------------
        // TESTING 'disableRead' is called if 'numNeeded' is 0
        //   Reproducing that even when numNeeded is specified as 0 channel
        //   pool can continue reading because it fails to disableRead in all
        //   cases.  After reproduction check that the fix corrects the bug.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int disableRead(int channelId);
        // --------------------------------------------------------------------

        if (verbose)
                cout << "\nTESTING 'disableRead' is called if 'numNeeded' is 0"
                     << "\n==================================================="
                     << endl;

        using namespace TEST_CASE_DISABLE_READ_WHEN_NUM_NEEDED_ZERO;

        btlmt::ChannelPoolConfiguration config;
        config.setMaxThreads(1);
        config.setReadTimeout(0);        // in seconds
        if (verbose) {
            P(config);
        }

        bslmt::Barrier  channelCbBarrier(2);
        int             channelId;
        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

        btlmt::ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

        bslmt::Barrier  dataCbBarrier(2);
        int             numTimesDataCbCalled = 0;
        string          data;
        btlmt::ChannelPool::BlobBasedReadCallback dataCb(
                                     bdlf::BindUtil::bind(&blobBasedReadCb,
                                                         _1, _2, _3, _4,
                                                         &numTimesDataCbCalled,
                                                         &data,
                                                         &dataCbBarrier));

        btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

        ASSERT(0 == pool.start());

        const int SERVER_ID = 100;

        btlso::IPv4Address serverAddr;

        int rc = pool.listen(serverAddr, 5, SERVER_ID);
        ASSERT(!rc);

        serverAddr = getServerLocalAddress(&pool, SERVER_ID);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlso::StreamSocket<btlso::IPv4Address> *socket = factory.allocate();

        ASSERT(0 == socket->connect(serverAddr));

        channelCbBarrier.wait();

        const char *TEXT = "Hello World";
        const int   LEN  = strlen(TEXT);
        rc = socket->write(TEXT, LEN);
        LOOP2_ASSERT(LEN, rc, LEN == rc);

        dataCbBarrier.wait();

        LOOP_ASSERT(numTimesDataCbCalled,    1   == numTimesDataCbCalled);
        LOOP3_ASSERT(LEN, data.size(), data, LEN == (int) data.size());

        if (veryVerbose) {
            P(data);
        }

        numTimesDataCbCalled = 0;

        rc = pool.disableRead(channelId);
        ASSERT(!rc);

        channelCbBarrier.wait();

        rc = socket->write(TEXT, LEN);
        LOOP2_ASSERT(LEN, rc, LEN == rc);

        bsls::TimeInterval time = bdlt::CurrentTime::now() + 1.0;
        rc = dataCbBarrier.timedWait(time);

        // The wait should time out as we dont expect a data callback after we
        // disabled read.

        ASSERT(rc);

        LOOP_ASSERT(numTimesDataCbCalled, 0 == numTimesDataCbCalled);

        if (veryVerbose) {
            P(data);
        }
}

void TestDriver::testCase28()
{
        // --------------------------------------------------------------------
        // TESTING 'connect' & 'listen' with SocketOpts and clientAddress
        //   Ensure that the 'connect' and 'listen' functions have the
        //   specified socket options and client address.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int connect(host, ...., *socketOptions, *clientAddr);
        //   int connect(serverAddr, ...., *socketOptions, *clientAddr);
        //   int listen(port, ..., *socketOptions);
        //   int listen(port, ..., timeout, ..., *socketOptions);
        //   int listen(serverAddr, ..., *socketOptions);
        //   int listen(serverAddr, ..., timeout, ..., *socketOptions);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\n'connect' & 'listen' with SocketOpts & clientAddress"
                 << "\n===================================================="
                 << endl;

        using namespace TEST_CASE_TESTING_SOCKET_OPTIONS;

        SocketPool sp;

        const struct {
            int         d_line;
            const char *d_spec_p;
            int         d_expReturnCode;
        } DATA[] = {
            // Line   Spec        Exp Return Code
            // ----   ----        ---------------

            {   L_,   "GN",         0 },

#ifdef BSLS_PLATFORM_OS_LINUX
            {   L_,   "GY",        -1 },
#else
            {   L_,   "GY",         0 },
#endif

#if defined(BSLS_PLATFORM_OS_CYGWIN) || defined(BSLS_PLATFORM_OS_WINDOWS)
            {   L_,   "HN",         -1 },
#else
            {   L_,   "HN",         0 },
#endif

#if !defined(BSLS_PLATFORM_OS_AIX)                 \
 && !defined(BSLS_PLATFORM_OS_CYGWIN)              \
 && !defined(BSLS_PLATFORM_OS_WINDOWS)
            {   L_,   "HY",         0 },
#endif

            {   L_,   "IN",         0 },
            {   L_,   "IY",         0 },

#if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_WINDOWS)
            {   L_,   "JN",         0 },
            {   L_,   "JY",         0 },
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
            {   L_,   "JY",         0 },
#endif

#if !defined(BSLS_PLATFORM_OS_WINDOWS)
            {   L_,   "KN",         0 },
#endif
            {   L_,   "KY",         0 },

            {   L_,   "LN",         0 },
            {   L_,   "LY",         0 },

#if defined(BSLS_PLATFORM_OS_CYGWIN) || defined(BSLS_PLATFORM_OS_WINDOWS)
            {   L_,   "GNHN",      -1 },
            {   L_,   "GNIN",       0 },
            {   L_,   "GNIYKY",     0 },
#else
            {   L_,   "GNHN",       0 },
            {   L_,   "GNHYIN",     0 },
            {   L_,   "GNHYIYKY",   0 },
#endif

            {   L_,   "A0",         0 },
            {   L_,   "A1",         0 },
            {   L_,   "A2",         0 },

#ifndef BSLS_PLATFORM_OS_AIX
            {   L_,   "B0",         0 },
            {   L_,   "B1",         0 },
            {   L_,   "B2",         0 },
#endif

#if !defined(BSLS_PLATFORM_OS_SOLARIS)          \
 && !defined(BSLS_PLATFORM_OS_LINUX)            \
 && !defined(BSLS_PLATFORM_OS_WINDOWS)
            // Cannot be changed on Linux and not specified on Sun

            {   L_,   "C0",         0 },
            {   L_,   "C1",         0 },
            {   L_,   "C2",         0 },
#else
            {   L_,   "C0",        -1 },
            {   L_,   "C1",        -1 },
            {   L_,   "C2",        -1 },
#endif

#if defined(BSLS_PLATFORM_OS_SOLARIS)            \
 || defined(BSLS_PLATFORM_OS_CYGWIN)             \
 || defined(BSLS_PLATFORM_OS_WINDOWS)
            {   L_,   "D0",        -1 },
            {   L_,   "D1",        -1 },
            {   L_,   "D2",        -1 },
#elif !defined(BSLS_PLATFORM_OS_AIX)
            {   L_,   "D0",         0 },
            {   L_,   "D1",         0 },
            {   L_,   "D2",         0 },
#endif

#if defined(BSLS_PLATFORM_OS_CYGWIN) || defined(BSLS_PLATFORM_OS_WINDOWS)
            {   L_,   "E0",         0 },
            {   L_,   "E1",         0 },
            {   L_,   "E2",         0 },

            {   L_,   "F0",         0 },
            {   L_,   "F1",         0 },
            {   L_,   "F2",         0 },
#else
            {   L_,   "E0",        -1 },
            {   L_,   "E1",        -1 },
            {   L_,   "E2",        -1 },

            {   L_,   "F0",        -1 },
            {   L_,   "F1",        -1 },
            {   L_,   "F2",        -1 },
#endif

#if defined(BSLS_PLATFORM_OS_AIX)
            // Works only on IBM.  On other platforms although the return
            // code is 0, the timeout is not set correctly.

            {   L_,   "MN1",       0 },
#endif

            {   L_,   "MY2",       0 },

#ifndef BSLS_PLATFORM_OS_AIX
            {   L_,   "A1B2MY2",   0 },
#endif
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "connect(IPv4Address...) with options" << endl;
        {
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            if (verbose) {
                P(config);
            }

            bslmt::Barrier  channelCbBarrier(2);
            int             channelId;
            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

            bslmt::Barrier    poolCbBarrier(2);
            btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                                          bdlf::BindUtil::bind(&poolStateCb,
                                                              _1, _2, _3,
                                                              &poolCbBarrier));

            btlmt::ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            vector<btlso::StreamSocket<btlso::IPv4Address> *>  sockets;
            int numExpChannels = 0;
            for (int i = 0; i < NUM_DATA; ++i) {
                for (int j = 0; j < 2; ++j) {
                    const int   LINE   = DATA[i].d_line;
                    const char *SPEC   = DATA[i].d_spec_p;
                    const int   EXP_RC = DATA[i].d_expReturnCode;

                    SocketOptions opt = g(SPEC);
                    const SocketOptions& OPT = opt;

                    if (veryVerbose) { P(LINE) }

                    const int SOURCE_ID = 200;

                    btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                            factory.allocate();
                    ASSERT(socket);
                    sockets.push_back(socket);

                    ASSERT(0 == socket->bind(getLocalAddress()));

                    ASSERT(0 == socket->listen(5));

                    btlso::IPv4Address serverAddr;
                    ASSERT(0 == socket->localAddress(&serverAddr));

                    bool setManually = (bool)j;
                    int rc = sp.connectWithOptions(&pool,
                                                   serverAddr,
                                                   SOURCE_ID + i,
                                                   &OPT, setManually);
                    if (EXP_RC) {
                        if (setManually) {
                            LOOP2_ASSERT(LINE, j, EXP_RC == rc);
                        }
                        else {
                            if (veryVerbose) {
                                MTCOUT << "Waiting on pool callback" << MTENDL;
                            }
                            poolCbBarrier.wait();
                        }
                    }
                    else {
                        if (veryVerbose) {
                            MTCOUT << "Waiting on channel callback" << MTENDL;
                        }
                        channelCbBarrier.wait();
                        ++numExpChannels;
                        LOOP2_ASSERT(LINE, j, !verify(pool, channelId, OPT));
                    }
                    LOOP3_ASSERT(LINE, j, numExpChannels,
                                 numExpChannels == pool.numChannels());
                }
            }
            for (int i = 0; i < (int) sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }

        if (verbose) cout << "connect(hostname...) with options" << endl;
        {
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            if (verbose) {
                P(config);
            }

            bslmt::Barrier  channelCbBarrier(2);
            int             channelId;
            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

            bslmt::Barrier    poolCbBarrier(2);
            btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                                          bdlf::BindUtil::bind(&poolStateCb,
                                                              _1, _2, _3,
                                                              &poolCbBarrier));

            btlmt::ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            vector<btlso::StreamSocket<btlso::IPv4Address> *> sockets;
            for (int i = 0; i < NUM_DATA; ++i) {
                for (int j = 0; j < 2; ++j) {
                    const int   LINE   = DATA[i].d_line;
                    const char *SPEC   = DATA[i].d_spec_p;
                    const int   EXP_RC = DATA[i].d_expReturnCode;

                    SocketOptions opt = g(SPEC);
                    const SocketOptions& OPT = opt;

                    if (veryVerbose) { P(LINE) P(OPT) }

                    const int SOURCE_ID = 200;

                    btlso::StreamSocket<btlso::IPv4Address> *socket =
                        factory.allocate();
                    ASSERT(socket);
                    sockets.push_back(socket);

                    ASSERT(0 == socket->bind(getLocalAddress()));
                    ASSERT(0 == socket->listen(5));

                    btlso::IPv4Address serverAddr;
                    ASSERT(0 == socket->localAddress(&serverAddr));

                    const char *host = "127.0.0.1";
                    bool setManually = (bool)j;
                    int rc = sp.connectWithOptions(&pool,
                                                   host,
                                                   serverAddr.portNumber(),
                                                   SOURCE_ID + i,
                                                   &OPT, setManually);
                    if (EXP_RC) {
                        if (setManually) {
                            LOOP2_ASSERT(LINE, j, EXP_RC == rc);
                        }
                        else {
                            if (veryVerbose) {
                                MTCOUT << "Waiting on pool callback" << MTENDL;
                            }
                            poolCbBarrier.wait();
                        }
                    }
                    else {
                        if (veryVerbose) {
                            MTCOUT << "Waiting on channel callback" << MTENDL;
                        }
                        channelCbBarrier.wait();
                        LOOP2_ASSERT(LINE, j, !verify(pool, channelId, OPT));
                    }
                }
            }

            for (int i = 0; i < (int) sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }

        if (verbose) cout << "connect(IPv4Address...) with client address"
                          << endl;
        {
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            if (verbose) {
                P(config);
            }

            bslmt::Barrier  channelCbBarrier(2);
            int             channelId;
            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

            bslmt::Barrier    poolCbBarrier(2);
            btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                                          bdlf::BindUtil::bind(&poolStateCb,
                                                              _1, _2, _3,
                                                              &poolCbBarrier));

            btlmt::ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            const int SOURCE_ID = 200;

            btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                            factory.allocate();

            ASSERT(0 == socket->bind(getLocalAddress()));
            ASSERT(0 == socket->listen(5));

            btlso::IPv4Address serverAddr;
            ASSERT(0 == socket->localAddress(&serverAddr));

            btlso::IPv4Address exp_ca, ca;
            exp_ca.setIpAddress("127.0.0.1");
            exp_ca.setPortNumber(45000);
            int rc = pool.connect(serverAddr,
                                  1,
                                  bsls::TimeInterval(1),
                                  SOURCE_ID,
                                  true,
                                  Obj::e_CLOSE_BOTH,
                                  0,
                                  &exp_ca);

            ASSERT(!rc);

            channelCbBarrier.wait();

            pool.getLocalAddress(&ca, channelId);

            LOOP2_ASSERT(exp_ca, ca, exp_ca == ca);
            factory.deallocate(socket);
        }

        if (verbose) cout << "connect(hostname...) with client address"
                          << endl;
        {
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            if (verbose) {
                P(config);
            }

            bslmt::Barrier  channelCbBarrier(2);
            int             channelId;
            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

            bslmt::Barrier    poolCbBarrier(2);
            btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                                          bdlf::BindUtil::bind(&poolStateCb,
                                                              _1, _2, _3,
                                                              &poolCbBarrier));

            btlmt::ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            const int SOURCE_ID = 200;

            btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                            factory.allocate();
            ASSERT(0 == socket->bind(getLocalAddress()));
            ASSERT(0 == socket->listen(5));

            btlso::IPv4Address serverAddr;
            ASSERT(0 == socket->localAddress(&serverAddr));

            btlso::IPv4Address exp_ca, ca;
            exp_ca.setIpAddress("127.0.0.1");
            exp_ca.setPortNumber(45000);

            const char *host = "127.0.0.1";
            int rc = pool.connect(host,
                                  serverAddr.portNumber(),
                                  1,
                                  bsls::TimeInterval(1),
                                  SOURCE_ID,
                                  Obj::e_RESOLVE_ONCE,
                                  true,
                                  Obj::e_CLOSE_BOTH,
                                  0,
                                  &exp_ca);

            ASSERT(!rc);

            channelCbBarrier.wait();

            pool.getLocalAddress(&ca, channelId);

            LOOP2_ASSERT(exp_ca, ca, exp_ca == ca);
            factory.deallocate(socket);
        }

        if (verbose) cout << "listen with socket options" << endl;
        {
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            if (verbose) {
                P(config);
            }

            bslmt::Barrier  channelCbBarrier(2);
            int             channelId;
            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

            bslmt::Barrier    poolCbBarrier(2);
            btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                                          bdlf::BindUtil::bind(&poolStateCb,
                                                              _1, _2, _3,
                                                              &poolCbBarrier));

            btlmt::ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            vector<btlso::StreamSocket<btlso::IPv4Address> *> sockets;
            int numExpChannels = 0;

            enum {
                AMBIGUOUS_REUSE_ADDRESS  = -11,
                SET_SOCKET_OPTION_FAILED = -10
            };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE    = DATA[i].d_line;
                const char *SPEC    = DATA[i].d_spec_p;
                const int   EXP_RC  = DATA[i].d_expReturnCode;

                SocketOptions opt = g(SPEC); const SocketOptions& OPT = opt;

                if (veryVerbose) { P_(LINE) P(OPT) }

                const int SERVER_PORT = 1700;
                const int SERVER_ID   = 100;

                btlso::IPv4Address serverAddr = getLocalAddress();
                serverAddr.setPortNumber(SERVER_PORT + i);
                int rc = pool.listen(serverAddr,
                                     5,
                                     SERVER_ID + i,
                                     bsls::TimeInterval(1),
                                     true,
                                     true,
                                     Obj::e_CLOSE_BOTH,
                                     &OPT);

                if (!OPT.reuseAddress().isNull()
                 && false == OPT.reuseAddress().value()) {
                    LOOP_ASSERT(LINE, AMBIGUOUS_REUSE_ADDRESS == rc);
                    continue;
                }

                if (EXP_RC) {
                    LOOP_ASSERT(LINE, SET_SOCKET_OPTION_FAILED == rc);
                    continue;
                }

                LOOP_ASSERT(LINE, !rc);
                ASSERT(0 == pool.getServerAddress(&serverAddr, SERVER_ID + i));

                btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                            factory.allocate();
                sockets.push_back(socket);

                ASSERT(0 == socket->connect(serverAddr));

                channelCbBarrier.wait();

                ++numExpChannels;
                LOOP_ASSERT(LINE, !verify(pool, channelId, OPT));
                LOOP2_ASSERT(LINE, numExpChannels,
                             numExpChannels == pool.numChannels());
            }

            for (int i = 0; i < (int) sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }

        if (verbose) cout << "listen with time out and socket options"
                          << endl;
        {
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            if (verbose) {
                P(config);
            }

            bslmt::Barrier  channelCbBarrier(2);
            int             channelId;
            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

            bslmt::Barrier    poolCbBarrier(2);
            btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                                          bdlf::BindUtil::bind(&poolStateCb,
                                                              _1, _2, _3,
                                                              &poolCbBarrier));

            btlmt::ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            vector<btlso::StreamSocket<btlso::IPv4Address> *> sockets;
            int numExpChannels = 0;

            enum {
                AMBIGUOUS_REUSE_ADDRESS  = -11,
                SET_SOCKET_OPTION_FAILED = -10
            };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE    = DATA[i].d_line;
                const char *SPEC    = DATA[i].d_spec_p;
                const int   EXP_RC  = DATA[i].d_expReturnCode;

                SocketOptions opt = g(SPEC); const SocketOptions& OPT = opt;

                if (veryVerbose) { P(LINE)  P(OPT) }

                const int SERVER_PORT = 1700;
                const int SERVER_ID   = 100;

                btlso::IPv4Address serverAddr = getLocalAddress();
                serverAddr.setPortNumber(SERVER_PORT + i);
                int rc = pool.listen(serverAddr,
                                     5,
                                     SERVER_ID + i,
                                     bsls::TimeInterval(1),
                                     true,
                                     true,
                                     Obj::e_CLOSE_BOTH,
                                     &OPT);

                if (!OPT.reuseAddress().isNull()
                 && false == OPT.reuseAddress().value()) {
                    LOOP_ASSERT(LINE, AMBIGUOUS_REUSE_ADDRESS == rc);
                    continue;
                }

                if (EXP_RC) {
                    LOOP_ASSERT(LINE, SET_SOCKET_OPTION_FAILED == rc);
                    continue;
                }

                LOOP_ASSERT(LINE, !rc);

                ASSERT(0 == pool.getServerAddress(&serverAddr, SERVER_ID + i));

                btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                            factory.allocate();
                sockets.push_back(socket);

                ASSERT(0 == socket->connect(serverAddr));

                channelCbBarrier.wait();

                ++numExpChannels;
                LOOP_ASSERT(LINE, !verify(pool, channelId, OPT));
                LOOP2_ASSERT(LINE, numExpChannels,
                             numExpChannels == pool.numChannels());
            }

            for (int i = 0; i < (int) sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }

        if (verbose) cout << "listen(IPv4Address...) and socket options"
                          << endl;
        {
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            if (verbose) {
                P(config);
            }

            bslmt::Barrier  channelCbBarrier(2);
            int             channelId;
            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

            bslmt::Barrier    poolCbBarrier(2);
            btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                                          bdlf::BindUtil::bind(&poolStateCb,
                                                              _1, _2, _3,
                                                              &poolCbBarrier));

            btlmt::ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            vector<btlso::StreamSocket<btlso::IPv4Address> *> sockets;
            int numExpChannels = 0;

            enum {
                AMBIGUOUS_REUSE_ADDRESS  = -11,
                SET_SOCKET_OPTION_FAILED = -10
            };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE    = DATA[i].d_line;
                const char *SPEC    = DATA[i].d_spec_p;
                const int   EXP_RC  = DATA[i].d_expReturnCode;

                SocketOptions opt = g(SPEC); const SocketOptions& OPT = opt;

                if (veryVerbose) { P(LINE)  P(OPT) }

                const int SERVER_ID = 100;

                btlso::IPv4Address serverAddr = getLocalAddress();
                int rc = pool.listen(serverAddr,
                                     5,
                                     SERVER_ID + i,
                                     true,
                                     true,
                                     &OPT);

                if (!OPT.reuseAddress().isNull()
                 && false == OPT.reuseAddress().value()) {
                    LOOP_ASSERT(LINE, AMBIGUOUS_REUSE_ADDRESS == rc);
                    continue;
                }

                if (EXP_RC) {
                    LOOP_ASSERT(LINE, SET_SOCKET_OPTION_FAILED == rc);
                    continue;
                }

                LOOP_ASSERT(LINE, !rc);
                ASSERT(0 == pool.getServerAddress(&serverAddr, SERVER_ID + i));

                btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                            factory.allocate();
                sockets.push_back(socket);

                ASSERT(0 == socket->connect(serverAddr));

                channelCbBarrier.wait();

                ++numExpChannels;
                LOOP_ASSERT(LINE, !verify(pool, channelId, OPT));
                LOOP2_ASSERT(LINE, numExpChannels,
                             numExpChannels == pool.numChannels());
            }

            for (int i = 0; i < (int) sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }

        if (verbose) cout << "listen(IPv4Address...) with time out and options"
                          << endl;
        {
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            if (verbose) {
                P(config);
            }

            bslmt::Barrier  channelCbBarrier(2);
            int             channelId;
            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdlf::BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

            bslmt::Barrier    poolCbBarrier(2);
            btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                                          bdlf::BindUtil::bind(&poolStateCb,
                                                              _1, _2, _3,
                                                              &poolCbBarrier));

            btlmt::ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            vector<btlso::StreamSocket<btlso::IPv4Address> *> sockets;
            int numExpChannels = 0;

            enum {
                AMBIGUOUS_REUSE_ADDRESS  = -11,
                SET_SOCKET_OPTION_FAILED = -10
            };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE    = DATA[i].d_line;
                const char *SPEC    = DATA[i].d_spec_p;
                const int   EXP_RC  = DATA[i].d_expReturnCode;

                SocketOptions opt = g(SPEC); const SocketOptions& OPT = opt;

                if (veryVerbose) { P(LINE)  P(OPT) }

                const int SERVER_ID = 100;

                btlso::IPv4Address serverAddr = getLocalAddress();
                int rc = pool.listen(serverAddr,
                                     5,
                                     SERVER_ID + i,
                                     bsls::TimeInterval(1),
                                     true,
                                     true,
                                     Obj::e_CLOSE_BOTH,
                                     &OPT);

                if (!OPT.reuseAddress().isNull()
                 && false == OPT.reuseAddress().value()) {
                    LOOP_ASSERT(LINE, AMBIGUOUS_REUSE_ADDRESS == rc);
                    continue;
                }

                if (EXP_RC) {
                    LOOP_ASSERT(LINE, SET_SOCKET_OPTION_FAILED == rc);
                    continue;
                }

                LOOP_ASSERT(LINE, !rc);
                ASSERT(0 == pool.getServerAddress(&serverAddr, SERVER_ID + i));

                btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                            factory.allocate();
                sockets.push_back(socket);

                ASSERT(0 == socket->connect(serverAddr));

                channelCbBarrier.wait();

                ++numExpChannels;
                LOOP_ASSERT(LINE, !verify(pool, channelId, OPT));
                LOOP2_ASSERT(LINE, numExpChannels,
                             numExpChannels == pool.numChannels());
            }

            for (int i = 0; i < (int) sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }
}

void TestDriver::testCase27()
{
        // --------------------------------------------------------------------
        // TESTING that partial reads work correctly
        //
        // Concern: Channel pool reads can fail when reading into a partially
        //          filled blob.  This can happen when the user does not take
        //          complete ownership of the blob passed to the read callback.
        //
        // Plan:
        //   Create a Read Server that creates a channel pool and opens a
        //   listening socket.  Independently we create a connection to the
        //   server's listening port.  Then we incremently write data to that
        //   socket.  In the Read Server's read callback function we do not
        //   read the data (or take ownership of the blob) until all the data
        //   has been loaded.  After all the data is read from the socket the
        //   read server stores that data in an internal string.  At the end
        //   of the test we confirm that the data received was the same as the
        //   data sent.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING that partial reads work correctly"
                 << "\n=========================================" << endl;

        using namespace TEST_CASE_PARTIAL_READS;

        typedef btlso::InetStreamSocketFactory<btlso::IPv4Address> IPv4Factory;
        typedef btlso::StreamSocket<btlso::IPv4Address>            Socket;

        const char *TEXT = "AAAAAAAAAABBBBBCCCCCDDDDDDDDDD";// 30 characters
        const int   LEN  = bsl::strlen(TEXT);

        btlmt::ChannelPoolConfiguration config;
        config.setMaxConnections(1);
        config.setMaxThreads(1);
        config.setIncomingMessageSizes(1, 3, 5);

        bslmt::Mutex         coutMutex;
        bslma::TestAllocator ta(veryVeryVerbose);
        ReadServer           server(&coutMutex, LEN, config, &ta);

        ASSERT(0 == server.start());

        const int         PORT = server.portNumber();
        btlso::IPv4Address address;
        address.setIpAddress("127.0.0.1");
        address.setPortNumber(PORT);

        IPv4Factory factory;
        Socket *socket = factory.allocate();
        ASSERT(0 == socket->connect(address));

        const int MAX_DATA_SIZE = 16384;

        char data[MAX_DATA_SIZE];
        bsl::memset(data, 0, MAX_DATA_SIZE);
        bsl::memcpy(data, TEXT, LEN);

        const int NUM_WRITTEN = 3;
        for (int i = 0, offset = 0; i < 10; ++i) {
            ASSERT(NUM_WRITTEN == socket->write(data + offset, NUM_WRITTEN));
            offset += NUM_WRITTEN;
            bslmt::ThreadUtil::sleep(bsls::TimeInterval(0.1));
        }

        bslmt::ThreadUtil::sleep(bsls::TimeInterval(2));

        const string& DATA = server.data();

        LOOP_ASSERT(DATA, DATA == string(TEXT));
}

void TestDriver::testCase26()
{
        // --------------------------------------------------------------------
        // READ TEST
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nREAD TESTING"
                 << "\n============" << endl;

        using namespace TEST_CASE_READ;

        const struct {
            int           d_line;
            const char   *d_text;
        } DATA[] = {
            // Line  Text
            {  L_,   "Z" },
            {  L_,   "BDE" },
            {  L_,   "hello world"},
            {  L_,   "This is a short test."},
            {  L_,   "This is a somewhat longer test than the one above."},
            {  L_,   "This is the longest test which we hope "
                     "will not pose much of a problem for the "
                     "channel pool read functionality." },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        typedef btlso::InetStreamSocketFactory<btlso::IPv4Address> IPv4Factory;
        typedef btlso::StreamSocket<btlso::IPv4Address>            Socket;

        {
            for (int type = 0; type < 2; ++type) {
                bslmt::Mutex  coutMutex;
                bslma::TestAllocator ta(veryVeryVerbose);
                ReadServer server(&coutMutex, 0, &ta);
                ASSERT(0 == server.start());
                const int PORT = server.portNumber();
                btlso::IPv4Address address(getLocalAddress());
                address.setPortNumber(PORT);

                IPv4Factory               factory;
                bslmt::ThreadUtil::Handle handles[NUM_DATA];
                bsl::vector<TestData>     tests(NUM_DATA);
                bslmt::Barrier            barrier(NUM_DATA);

                for (int i = 0; i < NUM_DATA; ++i) {
                    const string& TEXT = DATA[i].d_text;

                    TestData& testData      = tests[i];
                    testData.d_threadIntId  = i;
                    testData.d_expData      = TEXT;
                    testData.d_address      = address;
                    testData.d_socket_p     = factory.allocate();
                    testData.d_barrier_p    = &barrier;

                    bslmt::ThreadUtil::create(&handles[i],
                                              threadFunction,
                                              &testData);
                }

                for (int i = 0; i < NUM_DATA; ++i) {
                    ASSERT(0 == bslmt::ThreadUtil::join(handles[i]));
                }

                while (NUM_DATA != server.numCompletedMsgs()) {
                    bslmt::ThreadUtil::yield();
                }

                for (int i = 0; i < NUM_DATA; ++i) {
                    DataReader        *READER = server.dataReader(i);
                    const int          MSG_ID = READER->msgId();
                    const string       MSG    = READER->data();

                    const int          LINE      = DATA[MSG_ID].d_line;
                    const string       EXP_MSG   = DATA[MSG_ID].d_text;

                    LOOP3_ASSERT(LINE, MSG, EXP_MSG, MSG == EXP_MSG);
                }

                ASSERT(0 == server.stop());
            }
        }
}

void TestDriver::testCase25()
{
        // --------------------------------------------------------------------
        // CONCERN: Event Manager Allocation
        //
        // Concerns (white box):  That connections are assigned to different
        //      event managers either by picking the event manager with the
        //      fewest registered socket events (if metrics collection is
        //      disable) or by the least usage (if metrics
        //      collection is enabled).
        //
        // Plan:  This test uses the 'ChannelPoolStateCbTester' to record the
        //    thread identifiers of the calls to the channel-state callback.
        //    The channel state callback is called on the event-manager's
        //    thread, so it is an indirect (white-box) indicator of the
        //    event-manager a channel was assigned to.  Both tests are
        //    performed over a series of test values for the maximum number of
        //    threads ('MAX_THREADS').
        //
        //  1. Fewest Socket Events -
        //         Import '2*MAX_THREADS' sockets into a
        //         'ChannelPoolStateCbTester' configured with metrics disabled
        //         (i.e., with fewest socket-events scheduling).  Use
        //         'waitForState()' to determine the thread that the
        //         'CHANNEL_UP' event was received.  Verify the thread ids
        //         that the 'CHANNEL_UP' event was received in a round-robin
        //         fashion through the 'MAX_THREADS' different thread ids.
        //
        //  2. Least Workload Scheduling -
        //         Configure a 'ChannelPoolStateCbTester' with metrics enabled
        //         (i.e., least-workload scheduling), and with a data callback
        //         that will delay (to simulate processing), then import
        //         'MAX_THREADS' sockets.  Then, Send a message to all but the
        //         last channel that was added, such that the channel pool
        //         should see simulated processing on all but the last channel
        //         (and its corresponding event manager).  Then, import a new
        //         channel, and verify that its 'CHANNEL_UP' event is received
        //         on the thread for the last event manager.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: event manager allocation" << endl
                          << "=================================" << endl;

        // For 'dummyDataCallbackWithDelay'.
        using namespace TEST_CASE_BUSY_METRICS;

        bslma::TestAllocator ta;

        int       NUM_THREADS[]   = { 2,  4,  8 };
        const int NUM_NUM_THREADS = sizeof NUM_THREADS / sizeof *NUM_THREADS;

        if (veryVerbose) {
            cout << "\tTesting round-robin (metrics collection off)" << endl;
        }
        for (int i = 0; i < NUM_NUM_THREADS; ++i) {
            const int MAX_THREADS           = NUM_THREADS[i];

            if (veryVeryVerbose) {
                P(MAX_THREADS);
            }

            enum {
                NUM_SOCKETS   = 16,
                LOW_WATERMARK = 512,
                HI_WATERMARK  = 4096
            };

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&ta);
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(MAX_THREADS);
            config.setCollectTimeMetrics(false);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setWriteCacheWatermarks(LOW_WATERMARK, HI_WATERMARK);

            if (verbose)         { P(MAX_THREADS); }
            if (veryVeryVerbose) { P(config); }

            ChannelPoolStateCbTester mX(config, &ta);
            Obj& pool = mX.pool();

            ASSERT(0 == pool.start());

            // Keep track of the client sockets (so we can deallocate them),
            // and the set of thread ids, and the number of connections
            // assigned to each thread id.
            bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *> cltSockets;
            bsl::map<ThreadId, int>                              threadIdCount;
            for (int j = 0; j < 2 * MAX_THREADS; ++j) {
                if (veryVeryVerbose) {
                    P(j);
                }

                int ret  = 0;
                btlso::SocketHandle::Handle handles[2];

                ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                           handles, btlso::SocketImpUtil::k_SOCKET_STREAM);

                ASSERT(0 == ret);

                // The following socket options are set only if necessary.
                ret = btlso::SocketOptUtil::setOption(handles[1],
                                                     SockOpt::k_TCPLEVEL,
                                                     SockOpt::k_TCPNODELAY,
                                                     1);
                ASSERT(0 == ret);
                btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
                ASSERT(serverSocket);
                btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
                ASSERT(clientSocket);
                cltSockets.push_back(clientSocket);

                typedef btlso::StreamSocketFactoryDeleter Deleter;

                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
               managedServerSocket(serverSocket,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

                ASSERT(0 == pool.import(&managedServerSocket, 0));

                // Wait for the channel to come up.
                bsl::vector<ChannelPoolStateCbTester::ChannelState>
                                                                   states(&ta);
                ASSERT(0 == mX.waitForState(
                                          &states,
                                          btlmt::ChannelPool::e_CHANNEL_UP,
                                          bsls::TimeInterval(1.0)));

                // For any CHANNEL_UP message, record the thread to which the
                // new channel was assinged.
                for (int k = 0; k < (int) states.size(); ++k) {
                    if (veryVerbose) {
                        P_(k); P_(states[k].d_channelId);
                        P_(states[k].d_state); P(states[k].d_threadId);
                    }
                    if (states[k].d_state ==
                                        btlmt::ChannelPool::e_CHANNEL_UP) {
                        threadIdCount[states[k].d_threadId]++;
                    }
                }

                // Verify that as channel's are added, we move sequentially
                // through the available threads up to MAX_THREADS.
                if (j < MAX_THREADS) {
                    LOOP_ASSERT(threadIdCount.size(),
                                 j + 1 == (int) threadIdCount.size());
                }
                else {
                    LOOP_ASSERT(threadIdCount.size(),
                                MAX_THREADS == (int) threadIdCount.size());
                    bsl::map<ThreadId, int>::const_iterator it =
                                                        threadIdCount.begin();
                    for (; it != threadIdCount.end(); ++it) {
                        LOOP_ASSERT(it->second,
                                    0 < it->second && it->second <= 2);
                    }
                }
                bslmt::ThreadUtil::sleep(bsls::TimeInterval(.05));
            }
            pool.stop();
            for (int j = 0; j < (int) cltSockets.size(); ++j) {
                factory.deallocate(cltSockets[j]);
            }
        }

        if (veryVerbose) {
            cout << "\tTesting least usage (metrics collection on)" << endl;
        }
        for (int i = 0; i < NUM_NUM_THREADS; ++i) {
            const int MAX_THREADS = NUM_THREADS[i];

            if (veryVeryVerbose) {
                P(MAX_THREADS);
            }

            enum {
                NUM_SOCKETS   = 16,
                LOW_WATERMARK = 512,
                HI_WATERMARK  = 4096
            };

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&ta);
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(MAX_THREADS);
            config.setCollectTimeMetrics(true);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setWriteCacheWatermarks(LOW_WATERMARK, HI_WATERMARK);

            // Note that the metrics interval must be short enough that
            // metrics will be updated during this test.
            config.setMetricsInterval(.1);

            if (verbose)         { P(MAX_THREADS); }
            if (veryVeryVerbose) { P(config); }

            // Supply a data callback that will delay, to simulate processing.
            btlmt::ChannelPool::BlobBasedReadCallback         dataCb(
                    bdlf::BindUtil::bind( &dummyDataCallbackWithDelay
                                        , _1, _2, _3, _4
                                        , 0.5));

            btlmt::ChannelPool::PoolStateChangeCallback    poolCb;

            ChannelPoolStateCbTester mX(config, dataCb, poolCb, &ta);

            Obj& pool = mX.pool();
            ASSERT(0 == pool.start());

            ThreadId lastClientSocketThreadId;
            bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *> cltSockets;
            bsl::map<ThreadId, int>                              threadIdCount;
            for (int j = 0; j < MAX_THREADS; ++j) {
                int ret  = 0;
                btlso::SocketHandle::Handle handles[2];
                ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                           handles,
                           btlso::SocketImpUtil::k_SOCKET_STREAM);

                ASSERT(0 == ret);

                // The following socket options are set only if necessary.
                ret = btlso::SocketOptUtil::setOption(handles[1],
                                                     SockOpt::k_TCPLEVEL,
                                                     SockOpt::k_TCPNODELAY,
                                                     1);
                ASSERT(0 == ret);
                btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
                ASSERT(serverSocket);
                btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
                ASSERT(clientSocket);
                cltSockets.push_back(clientSocket);

                typedef btlso::StreamSocketFactoryDeleter Deleter;

                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
               managedServerSocket(serverSocket,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

                ASSERT(0 == pool.import(&managedServerSocket, 0));

                // Wait for the channel to come up.
                bsl::vector<ChannelPoolStateCbTester::ChannelState>
                                                                   states(&ta);
                ASSERT(0 == mX.waitForState(
                                          &states,
                                          btlmt::ChannelPool::e_CHANNEL_UP,
                                          bsls::TimeInterval(1.0)));

                // For any CHANNEL_UP message, record the thread to which the
                // new channel was assinged.
                for (int k = 0; k < (int) states.size(); ++k) {
                    if (veryVerbose) {
                        P_(k); P_(states[k].d_channelId);
                        P_(states[k].d_state); P(states[k].d_threadId);
                    }
                    if (states[k].d_state ==
                                        btlmt::ChannelPool::e_CHANNEL_UP) {
                        threadIdCount[states[k].d_threadId]++;
                        if (j == MAX_THREADS - 1) {
                            lastClientSocketThreadId = states[k].d_threadId;
                        }
                    }
                }
                LOOP_ASSERT(threadIdCount.size(),
                            j + 1 == (int) threadIdCount.size());
                bslmt::ThreadUtil::sleep(bsls::TimeInterval(.05));
            }

            // Write data to all but the last channel, to simulate processing
            // on all but the last channel.
            char ONE_BYTE_MSG[] = { (char) 0xff };
            for (int j = 0; j < (int) cltSockets.size() - 1; ++j) {
                ASSERT(1 == cltSockets[j]->write(ONE_BYTE_MSG, 1));
            }
            bslmt::ThreadUtil::sleep(bsls::TimeInterval(.25));

            if (veryVerbose) {
                P(pool.busyMetrics());
            }

            // Create a new socket to import.
            int ret  = 0;
            btlso::SocketHandle::Handle handles[2];
            ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                           handles,
                           btlso::SocketImpUtil::k_SOCKET_STREAM);

            ASSERT(0 == ret);

            // The following socket options are set only if necessary.
            ret = btlso::SocketOptUtil::setOption(handles[1],
                                                 SockOpt::k_TCPLEVEL,
                                                 SockOpt::k_TCPNODELAY, 1);
            ASSERT(0 == ret);
            btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
            ASSERT(serverSocket);
            btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
            ASSERT(clientSocket);

            typedef btlso::StreamSocketFactoryDeleter Deleter;

            bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
               managedServerSocket(serverSocket,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

            ASSERT(0 == pool.import(&managedServerSocket, 0));

            // Wait for the channel to come up.
            bsl::vector<ChannelPoolStateCbTester::ChannelState> states(&ta);
            ASSERT(0 == mX.waitForState(&states,
                                        btlmt::ChannelPool::e_CHANNEL_UP,
                                        bsls::TimeInterval(1.0)));

#if  defined(BSLS_PLATFORM_OS_LINUX)           \
 &&  defined(BDE_BUILD_TARGET_OPT)              \
 &&  defined(BSLS_PLATFORM_CPU_64_BIT)
            // 64-bit opt builds on Linux this check that the latest imported
            // socket is assigned to the lastClientSocketThreadId fails.  The
            // allocation to a specific event manager thread is not an error
            // per se.  It happens only in one specific build mode and where
            // the allocation to event managers is based on the cpu
            // utilization of the process.
#else
            // Verify that the newly imported socket was assigned to the
            // thread (i.e., the event manager) of the channel that was not
            // simulating processing (i.e., the last channel).
            for (int k = 0; k < (int) states.size(); ++k) {
                if (states[k].d_state ==
                                        btlmt::ChannelPool::e_CHANNEL_UP) {
                    LOOP2_ASSERT(lastClientSocketThreadId,
                                 states[k].d_threadId,
                                 lastClientSocketThreadId ==
                                                         states[k].d_threadId);
                }
            }
#endif

            pool.stop();
            factory.deallocate(clientSocket);
            for (int j = 0; j < (int) cltSockets.size(); ++j) {
                factory.deallocate(cltSockets[j]);
            }

        }
}

void TestDriver::testCase24()
{
        // --------------------------------------------------------------------
        // TESTING: 'busyMetrics' and time metrics collection.
        //
        // Concerns:
        //    o That 'busyMetrics' returns an accurate reflection of the
        //      percent of time spent processing data.
        //
        //    o That timing metrics will not be collected if they are not
        //      required (i.e. the timing metrics will not be collected if the
        //      configured 'maxThreads()' is 1, and 'requireTimeMetrics()' is
        //      'false').
        //
        // Plan:
        //   For a series of configuration values for 'maxThreads()' and
        //   'requireTimeMetrics()':
        //     1) Create a ChannelPool and supply it a dummy data cb that
        //       will sleep for a long period of time (simulating a long time
        //       spent processing data).
        //     2) Import one-half of a socket-pair, and write to the other
        //        half.
        //     3) The dummy data callback should be called, simulating 1
        //        thread 100% busy for a period of time.
        //     4) Call 'busyMetrics()' and verify that (1) if timing metrics
        //        should be collected, that the returned value reflects 1
        //        thread (out of the configured max threads) was ~100% busy),
        //        and if timing metrics should not be colled, the value
        //        returned is ~0.
        //
        // Testing:
        //    int busyMetrics() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: busyMetrics()" << endl
                          << "======================" << endl;

        using namespace TEST_CASE_BUSY_METRICS;

        bslma::TestAllocator ta(veryVeryVerbose);

        int       NUM_THREADS[] = { 1,  2,  4 };
        const int NUM_NUM_THREADS = sizeof NUM_THREADS / sizeof *NUM_THREADS;
        bool      COLLECT_METRICS[] = { false, true };
        const int NUM_COLLECT_METRICS = sizeof COLLECT_METRICS /
                                        sizeof *COLLECT_METRICS;

        for (int i = 0; i < NUM_NUM_THREADS; ++i) {
            for (int j = 0; j < NUM_COLLECT_METRICS; ++j) {
                const int MAX_THREADS           = NUM_THREADS[i];
                const bool COLLECT_TIME_METRICS = COLLECT_METRICS[j];

                enum {
                    NUM_SOCKETS   = 1,
                    LOW_WATERMARK = 512,
                    HI_WATERMARK  = 4096
                };

                btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                                  factory(&ta);
                btlmt::ChannelPoolConfiguration config;
                config.setMaxThreads(MAX_THREADS);
                config.setCollectTimeMetrics(COLLECT_TIME_METRICS);
                config.setMetricsInterval(10.0);
                config.setMaxConnections(NUM_SOCKETS);
                config.setIncomingMessageSizes(1, 1, 1);
                config.setWriteCacheWatermarks(LOW_WATERMARK, HI_WATERMARK);

                // Note that the metrics interval must be short enough that
                // metrics will be updated during this test.
                config.setMetricsInterval(.1);

                if (verbose) { P(config); }

                btlmt::ChannelPool::BlobBasedReadCallback         dataCb(
                        bdlf::BindUtil::bind( &dummyDataCallbackWithDelay
                                            , _1, _2, _3, _4
                                            , 0.5));

                btlmt::ChannelPool::PoolStateChangeCallback    poolCb;
                makeNull(&poolCb);

                ChannelPoolStateCbTester mX(config, dataCb, poolCb, &ta);
                Obj& pool = mX.pool(); const Obj& POOL = mX.pool();
                btlb::PooledBlobBufferFactory outFactory(HI_WATERMARK / 64,
                                                           &ta);

                ASSERT(0 == pool.start());

                int ret  = 0;
                int code = 0;
                btlso::SocketHandle::Handle handles[2];

                ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                    handles, btlso::SocketImpUtil::k_SOCKET_STREAM);

                ASSERT(0 == ret);
                ASSERT(0 == code);

                // The following socket options are set only if necessary.
                ret = btlso::SocketOptUtil::setOption(handles[1],
                                                     SockOpt::k_TCPLEVEL,
                                                     SockOpt::k_TCPNODELAY,
                                                     1);

                ASSERT(0 == ret);
                btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
                ASSERT(serverSocket);
                btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
                ASSERT(clientSocket);

                typedef btlso::StreamSocketFactoryDeleter Deleter;

                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
               managedServerSocket(serverSocket,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

                ASSERT(0 == pool.import(&managedServerSocket, 0));

                char ONE_BYTE_MSG[] = { (char) 0xff };

                // Wait for the channel to come up and obtain the channel id.
                bsl::vector<ChannelPoolStateCbTester::ChannelState>
                                                                 states(&ta);
                ASSERT(0 == mX.waitForState(
                                          &states,
                                          btlmt::ChannelPool::e_CHANNEL_UP,
                                          bsls::TimeInterval(1.0)));

                int channelId = mX.lastOpenedChannelId();
                ASSERT(0 != channelId);

                ASSERT(1 == clientSocket->write(ONE_BYTE_MSG, 1));

                bslmt::ThreadUtil::sleep(bsls::TimeInterval(.3));

                pool.stop();

                int percent = POOL.busyMetrics();

                if (COLLECT_TIME_METRICS) {
                    // 1 thread should be 100 percent busy.  Note that our
                    // expected % busy must account for idle threads.
                    int minExpectedPercentBusy = (80 / MAX_THREADS);

                    if (veryVerbose) {
                        P_(percent); P(minExpectedPercentBusy);
                    }
                    LOOP2_ASSERT(percent,
                                 minExpectedPercentBusy,
                                 percent > minExpectedPercentBusy);
                }
                else {
                    if (veryVerbose) {
                        P(percent);
                    }

                    LOOP_ASSERT(percent, percent <= 1);
                }
                factory.deallocate(clientSocket);
            }
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBytesInUse());
            if (veryVerbose) { P(ta); }
        }
}

void TestDriver::testCase23()
{
        // --------------------------------------------------------------------
        // TESTING: readTimeout configuration
        //
        // Concerns:
        //     o a channel pool times out read operations appropriately
        //
        //     o with a configuration value of 0, the pool will never time out.
        //
        // Plan:
        //     For a set of test of timeout values (including 0.0, indicating
        //     a disabled read timeout): configure a
        //     'ChannelPoolStateCbTester', which allows the testing the
        //     channel state callback, with the  timeout.  Create a pair of
        //     sockets, and import one into the test 'ChannelPool'.  Once the
        //     channel is up, wait for a period of time, and verify if the a
        //     READ_TIMEOUT was correctly delivered (or not delivered).  Write
        //     one byte to the channel, and again test the delivery of the
        //     'READ_TIMEOUT'.
        // Testing:
        //   Configured read timeout.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING read timeout" << endl
                          << "====================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);

        double    TEST_VALUES[]   = { 0.0, 0.1, 0.25 };
        const int NUM_TEST_VALUES = sizeof TEST_VALUES / sizeof *TEST_VALUES;
        for (int i = 0; i < NUM_TEST_VALUES; ++i) {

            double READ_TIMEOUT = TEST_VALUES[i];
            enum {
                NUM_SOCKETS   = 1,
                LOW_WATERMARK = 512,
                HI_WATERMARK  = 4096
            };

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&ta);

            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setReadTimeout(READ_TIMEOUT);
            config.setWriteCacheWatermarks(LOW_WATERMARK,
                                           HI_WATERMARK);
            if (verbose) { P(config); }

            ChannelPoolStateCbTester mX(config, &ta);
            btlmt::ChannelPool& pool = mX.pool();

            ASSERT(0 == pool.start());

            int ret  = 0;
            int code = 0;
            btlso::SocketHandle::Handle handles[2];

            ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                           handles,
                           btlso::SocketImpUtil::k_SOCKET_STREAM);

            ASSERT(0 == ret);
            ASSERT(0 == code);

            // The following socket options are set only if necessary.

            ret = btlso::SocketOptUtil::setOption(handles[1],
                                                 SockOpt::k_TCPLEVEL,
                                                 SockOpt::k_TCPNODELAY, 1);

            ASSERT(0 == ret);
            btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
            ASSERT(serverSocket);
            btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
            ASSERT(clientSocket);

            typedef btlso::StreamSocketFactoryDeleter Deleter;

            bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
               managedServerSocket(serverSocket,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

            ASSERT(0 == pool.import(&managedServerSocket, 0));

            btlb::Blob oneByteMsg(&ta);
            populateMessage(&oneByteMsg, 1, &ta);
            ASSERT(1 == oneByteMsg.length());

            // Wait for the channel to come up and obtain the channel id.

            bsl::vector<ChannelPoolStateCbTester::ChannelState> states(&ta);

            ASSERT(0 == mX.waitForState(&states,
                                        btlmt::ChannelPool::e_CHANNEL_UP,
                                        bsls::TimeInterval(1.0)));

            int channelId = mX.lastOpenedChannelId();
            ASSERT(0 != channelId);

            bool timeoutShouldOccur = 0.0 != READ_TIMEOUT;

            bsls::Stopwatch timer;
            timer.start();

            // Test whether the timeout callback is registed for initial read.

            LOOP_ASSERT(i,
                        timeoutShouldOccur ==
                        (0 == mX.waitForState(
                                        &states,
                                        btlmt::ChannelPool::e_READ_TIMEOUT,
                                        bsls::TimeInterval(2.0))));

            timer.stop();

            // Test whether the timeout callback is registed when the read
            // timeout is invoked.
            timer.reset();
            timer.start();
            pool.write(channelId, oneByteMsg);

            MTLOOP_ASSERT(i,
                          timeoutShouldOccur ==
                          (0 == mX.waitForState(
                                        &states,
                                        btlmt::ChannelPool::e_READ_TIMEOUT,
                                        bsls::TimeInterval(2.0))));

            timer.stop();

            pool.stop();
            factory.deallocate(clientSocket);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase22()
{
        // --------------------------------------------------------------------
        // TESTING: struct 'btlmt::ChannelPool_MessageUtil' and
        //          'btlmt::ChannelPool_IovecArray'.
        //
        // Concerns:
        //     o That the nested class 'IovecArray' is a correctly defined
        //       in-core value-semantic type.
        //
        //     o That each of the static operations defined by
        //       'btlmt::ChannelPool_MessageUtil' behaves correctly for each
        //       data type.
        //
        // Plan:
        //     o Create a standard value-semantic container test for
        //      'IovecArray<>' in a templatized external test method.  Invoke
        //       this method for both 'btls::Ovec' and 'btls::Iovec'.
        //
        //     o Create a set of test data, composed of a series of buffer
        //        sizes that will be filled with arbitrary data.  For each
        //        test data point, create a series of buffers matching the
        //        test data point description, and use it to populate a
        //        'IovecArray' ans a 'btlb::Blob'.  Then invoke the functions
        //        defined in 'btlmt::ChannelPool_MessageUtil' on each of the
        //        three message container types.
        //
        //        1. For 'length()' simply verify the returned length.
        //        2. For 'loadBlob' increment through the valid offsets in the
        //           test data, and load a blob using that offset.
        //        3. For 'appendToBlob', iterate over the test data set, and
        //           create a blob (prefixed) with that data, then append the
        //           message container to the blob.
        //        4. For 'loadIovec' simply compare the loaded iovec with the
        //           original test data, verify that data is truncated using
        //           'BTEMT_MAX_IOVEC_SIZE'.
        //        5. For 'write' create a socket pair, and write the message
        //           container.  Verify that the received data matches the test
        //           data (truncated by 'BTEMT_MAX_IOVEC_SIZE'.
        //
        // Testing:
        //
        //  btlmt::ChannelPool_IovecArray
        //  ----------------------------
        //   btlmt::ChannelPool_IovecArray<IOVEC>(const IOVEC *, int );
        //   btlmt::ChannelPool_IovecArray(btlmt::ChannelPool_IovecArray& );
        //   btlmt::ChannelPool_IovecArray& operator=(
        //                              const btlmt::ChannelPool_IovecArray& );
        //   const IOVEC *iovecs() const;
        //   int numIovecs() const;
        //   bsls::Types::Int64 length() const;
        //
        //  btlmt::ChannelPool_MessageUtil
        //  -----------------------------
        //   static bsls::Types::Int64 length(const IovecArray<IOVEC>& );
        //   static bsls::Types::Int64 length(const btlb::Blob& );
        //   static int write(btlso::StreamSocket<btlso::IPv4Address> *,
        //                   btls::Iovec                            *,
        //                   const IovecArray<IOVEC>&               );
        //   static int write(btlso::StreamSocket<btlso::IPv4Address> *,
        //                   btls::Iovec                            *,
        //                  const btlb::Blob&                      );
        //   static int loadIovec(btls::Iovec *, const btlb::Blob& );
        //   static int loadBlob(lob *, const IovecArray<IOVEC>&, int);
        //   static int loadBlob(btlb::Blob *, const btlb::Blob&, int);
        //   static void appendToBlob(Blob *, const IovecArray<IOVEC>&);
        //   static void appendToBlob(btlb::Blob *, const btlb::Blob&);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: btlmt::ChannelPool_MessageUtil\n"
                          << "========================================\n";

        using namespace TEST_CASE_MESSAGEHELPER_NAMESPACE;

        bslma::TestAllocator ta;
        typedef btlmt::ChannelPool_MessageUtil Helper;
        typedef btlmt::ChannelPool_IovecArray<btls::Ovec>   OvecArray;
        typedef btlmt::ChannelPool_IovecArray<btls::Iovec>  IovecArray;

        {
            if (veryVerbose)
                cout << "Testing IovecArray constants" << bsl::endl;

#if defined(BSLS_PLATFORM_OS_UNIX) && defined(IOV_MAX) && IOV_MAX > 32
            ASSERT(Helper::e_MAX_IOVEC_SIZE == 32);
#elif defined(BSLS_PLATFORM_OS_UNIX) && defined(IOV_MAX)
            ASSERT(Helper::e_MAX_IOVEC_SIZE == IOV_MAX);
#else
            ASSERT(Helper::e_MAX_IOVEC_SIZE == 16);
#endif
        }

        if (veryVerbose) cout << "Testing IovecArray<btls::Ovec>" << bsl::endl;
        testIovecArray<btls::Ovec>();

        if (veryVerbose) cout << "Testing IovecArray<btls::Iovec>"
                              << bsl::endl;
        testIovecArray<btls::Iovec>();

        struct TestCaseData {
            int         d_line;
            const char *d_description;
        } VALUES[] = {
           { __LINE__, "1 1 1" },
           { __LINE__, "2 2 2" },
           { __LINE__, "10 10 10" },
           { __LINE__, "13 13 13" },
           { __LINE__, "64 65 64" },
           { __LINE__, "65 65 65" },
           { __LINE__, "256 128 64" },
           { __LINE__, "64 64 64 64 64 64" },
           { __LINE__, "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 " }, // 16 elements
           { __LINE__, "4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 " }, // 16 elements
           { __LINE__, "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 "
                       "1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 " }, // 32 elements
           { __LINE__, "4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 "
                       "4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 " }, // 32 elements
           { __LINE__, "4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 "
                       "4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 "
                       "4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 " }, // 48 elements
        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        if (veryVerbose) cout << "Testing MessageHelper" << bsl::endl;

        enum { BLOCK_SIZE = 64 };

        btlb::PooledBlobBufferFactory blobFactory(BLOCK_SIZE, &ta);

        for (int i = 0; i < NUM_VALUES; ++i) {
            // ------------------- Create Test Data ---------------------------
            bsl::vector<bsl::vector<char> > data(&ta);
            const int LINE   = VALUES[i].d_line;
            const int LENGTH = createData(&data, VALUES[i].d_description);

            if (veryVerbose) {
                P_(VALUES[i].d_description); P(LENGTH);
            }

            // Create an 'IovecArray', 'MX'.
            bsl::vector<btls::Iovec> iovecs(&ta);
            loadIovecs(&iovecs, data);
            IovecArray mX(&iovecs.front(),iovecs.size());
            const IovecArray& MX = mX;

            // Create a 'btlb::Blob', 'MY'.
            btlb::Blob mY(&blobFactory, &ta); const btlb::Blob& MY = mY;
            loadBlob(&mY, data);

            // ------------- Test MessageHelper::length() ---------------------
            ASSERT(LENGTH == Helper::length(MX));
            ASSERT(LENGTH == Helper::length(MY));

            // ------------- Test MessageHelper::loadBlob() -------------------

            for (int offset = 0; offset < LENGTH; ++offset) {
                btlb::Blob cX(&blobFactory, &ta);const btlb::Blob& CX = cX;
                btlb::Blob cY(&blobFactory, &ta);const btlb::Blob& CY = cY;

                int rcX = Helper::loadBlob(&cX, MX, offset);
                int rcY = Helper::loadBlob(&cY, MY, offset);

                // We need to precompute the expected prefix of the returned
                // by 'loadBlob'.

                int expectedPrefixY = 0;
                int length          = 0;
                for (int i = 0; i < (int) data.size(); ++i) {
                    if (length + (int) data[i].size() <= offset) {
                        length += data[i].size();
                    }
                    else {
                        expectedPrefixY = offset - length;
                        break;
                    }
                }

                ASSERT(0               == rcX);
                ASSERT(expectedPrefixY == rcY);

                ASSERT(LENGTH - offset                   == CX.length());
                ASSERT(LENGTH - offset + expectedPrefixY == CY.length());

                ASSERT(rcX < CX.buffer(0).size());
                ASSERT(rcY < CY.buffer(0).size());

                ASSERT(rcX < CX.length());
                ASSERT(rcY < CY.length());

                LOOP_ASSERT(LINE,
                       isEqualByteSequence(data, offset, CX, 0));
                LOOP_ASSERT(LINE,
                       isEqualByteSequence(data, offset, CY, expectedPrefixY));
            }

            // ------------- Test MessageHelper::appendToBlob() --------------
            for (int j = 0; j < NUM_VALUES; ++j) {
                bsl::vector<bsl::vector<char> > prefixData(&ta);
                const int PREFIX_LEN = createData(&prefixData,
                                                  VALUES[j].d_description);

                btlb::Blob dX(&blobFactory, &ta);const btlb::Blob& DX = dX;
                btlb::Blob dY(&blobFactory, &ta);const btlb::Blob& DY = dY;

                loadBlob(&dX, prefixData);
                loadBlob(&dY, prefixData);

                ASSERT(isEqualByteSequence(prefixData, 0, DX, 0));
                ASSERT(isEqualByteSequence(prefixData, 0, DY, 0));

                Helper::appendToBlob(&dX, MX);
                Helper::appendToBlob(&dY, MY);

                ASSERT(LENGTH + PREFIX_LEN == DX.length());
                ASSERT(LENGTH + PREFIX_LEN == DY.length());

                // Sanity check that the initial data wasn't changed
                ASSERT(isEqualByteSequence(prefixData, 0, DX, 0, PREFIX_LEN));
                ASSERT(isEqualByteSequence(prefixData, 0, DY, 0, PREFIX_LEN));

                ASSERT(isEqualByteSequence(data, 0, DX, PREFIX_LEN));
                ASSERT(isEqualByteSequence(data, 0, DY, PREFIX_LEN));
            }

            // ------------- Test MessageHelper::loadIovec() --------------
            {
                const int NUM_IOVECS = 2 * Helper::e_MAX_IOVEC_SIZE;
                btls::Iovec dY[NUM_IOVECS];

                int rY = Helper::loadIovec(dY, MY);

                ASSERT(bsl::min(MY.numBuffers(),
                                (int)Helper::e_MAX_IOVEC_SIZE) == rY);

                // To utilize the existing comparison operation, load the data
                // into a blob
                btlb::Blob bY(&blobFactory, &ta);const btlb::Blob& BY = bY;

                btls::IovecUtil::appendToBlob(&bY, dY, rY, 0);

                ASSERT(isEqualByteSequence(data, 0, BY, 0));
            }

            // ------------- Test MessageHelper::write() ----------------------
            {
                const int NUM_IOVECS = 2 * Helper::e_MAX_IOVEC_SIZE;
                btls::Iovec temp[NUM_IOVECS];

                // Create two sockets, one to receive the message and the
                // other to send it.
                btlso::SocketHandle::Handle handles[2];
                btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                                  factory(&ta);
                int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                           handles, btlso::SocketImpUtil::k_SOCKET_STREAM);

                ASSERT(0 == ret);

                // The following socket options are set only if necessary.
                ret = btlso::SocketOptUtil::setOption(
                                                 handles[1],
                                                 SockOpt::k_TCPLEVEL,
                                                 SockOpt::k_TCPNODELAY, 1);

                ASSERT(0 == ret);
                btlso::StreamSocket<btlso::IPv4Address> *sndSocket =
                                                 factory.allocate(handles[0]);
                ASSERT(sndSocket);
                btlso::StreamSocket<btlso::IPv4Address> *rcvSocket =
                                                 factory.allocate(handles[1]);
                ASSERT(rcvSocket);

                // The actual length by 'Helper::write'  may be less that the
                // original data if the number of buffers is greater than
                // 'BTEMT_MAX_IOVEC_SIZE'.

                int LENGTHX, LENGTHY;
                {
                    btls::Iovec vY[NUM_IOVECS];

                    int numBufX = bsl::min(MX.numIovecs(),
                                           (int)Helper::e_MAX_IOVEC_SIZE);
                    int numBufY = Helper::loadIovec(vY, MY);

                    LENGTHX = btls::IovecUtil::length(MX.iovecs(), numBufX);
                    LENGTHY = btls::IovecUtil::length(vY, numBufY);
                }

                sndSocket->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);
                rcvSocket->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);

                bsl::vector<char> rcvBufferX(2 * LENGTH, 0, &ta);
                bsl::vector<char> rcvBufferY(2 * LENGTH, 0, &ta);

                // Send the iovec message.
                int bytesX = 0;
                ASSERT(LENGTHX == Helper::write(sndSocket, temp, MX));
                while (bytesX < LENGTHX) {
                    int rc = rcvSocket->read(&rcvBufferX[bytesX], 2 * LENGTH);
                    ASSERT(0 <= rc);
                    bytesX += rc;
                }

                // Send the 'btlb::Blob' message.
                int bytesY = 0;
                ASSERT(LENGTHY == Helper::write(sndSocket, temp, MY));
                while (bytesY < LENGTHY) {
                    int rc = rcvSocket->read(&rcvBufferY[bytesY], 2 * LENGTH);
                    ASSERT(0 <= rc);
                    bytesY += rc;
                }

                // Verify the correct data was received (load it into a
                // 'btlb::Blob' to use the existing memory comparison
                // function).
                ASSERT(bytesX == LENGTHX);
                ASSERT(bytesY == LENGTHY);

                bsl::shared_ptr<char> spX(&rcvBufferX.front(),
                                          bslstl::SharedPtrNilDeleter(), 0);
                bsl::shared_ptr<char> spY(&rcvBufferY.front(),
                                          bslstl::SharedPtrNilDeleter(), 0);

                btlb::Blob bX(&blobFactory, &ta);const btlb::Blob& BX = bX;
                btlb::Blob bY(&blobFactory, &ta);const btlb::Blob& BY = bY;

                bX.appendDataBuffer(btlb::BlobBuffer(spX, bytesX));
                bY.appendDataBuffer(btlb::BlobBuffer(spY, bytesY));

                ASSERT(isEqualByteSequence(data, 0, BX, 0, LENGTHX));
                ASSERT(isEqualByteSequence(data, 0, BY, 0, LENGTHY));

                factory.deallocate(sndSocket);
                factory.deallocate(rcvSocket);
            }
        }
}

void TestDriver::testCase21()
{
        // --------------------------------------------------------------------
        // TESTING: 'setWriteCache[High|Low]Watermark[s]'
        //
        // Concerns:
        //   o That 'setWriteCacheHiWatermark', 'setWriteCacheLowWatermark',
        //     and 'setWriteCacheWatermarks' modify the high-water mark,
        //     low-water mark, and both water marks, respectively, of the
        //     specified channel (and only the specified channel).
        //   o That the 'HIWAT' alert is delivered properly if the specified
        //     water mark is smaller than the cache size.
        //   o That the 'HIWAT' alert is delivered if the cache size limit
        //     has been reached, then it is expanded, and it is reached again.
        //   o That 'setWriteCache[Hi|Low]Watermark', 'setWriteCacheWatermarks'
        //     are thread-safe.
        //
        // Plan:
        //   Create an instance under test.  Create a local socket pair and
        //   import it into the channel pool.  Then:
        //
        //   1. Verify that 'setWriteCache[Hi|Low]Watermark' and
        //      'setWriteCacheWatermarks' succeed or fail appropriately based
        //      on channelId and numBytes.
        //   2. Fill the write cache, verify the 'HIWAT' alert is delivered
        //      and no more data can be written.
        //   3. Double the write cache size, verify that the 'HIWAT' alert
        //      is not delivered.  Then refill the cache and verify that the
        //      cache increased by the expected amount and that 'HIWAT' is
        //      delivered.
        //   4. Double the write cache size again, add one byte (to verify
        //      it is not full), then reduce the write cache size to the
        //      original size and verify the 'HIWAT' alert is generated.
        //   5. Increase the write cache size back to 2 * HI_WATERMARK (one
        //      byte less than is currently in the cache); verify that the
        //      'HIWAT' alert is generated and no data can be written to the
        //      cache.
        //   6. Empty the write cache and perform a concurrency test.
        //
        // Testing:
        //   int setWriteCacheHiWatermark(int, int);
        //   int setWriteCacheLowWatermark(int, int);
        //   int setWriteCacheWatermarks(int, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: 'setWriteCache[High|Low]Watermark[s]'"
                          << endl
                          << "=============================================="
                          << endl;

        using namespace TEST_CASE_CONCURRENCY_TEST;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                NUM_SOCKETS   =    1,
                LOW_WATERMARK =  512,
                HI_WATERMARK  = 4096
            };

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&ta);

            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setReadTimeout(100000);
            config.setWriteCacheWatermarks(LOW_WATERMARK, HI_WATERMARK);
            if (verbose) { P(config); }

            ChannelPoolStateCbTester mX(config, &ta);
            btlmt::ChannelPool& pool = mX.pool();

            ASSERT(0 == pool.start());

            int ret  = 0;
            int code = 0;
            btlso::SocketHandle::Handle handles[2];

            ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                           handles, btlso::SocketImpUtil::k_SOCKET_STREAM);

            ASSERT(0 == ret);
            ASSERT(0 == code);

            // The following socket options are set only if necessary.
            ret = btlso::SocketOptUtil::setOption(handles[1],
                                                 SockOpt::k_TCPLEVEL,
                                                 SockOpt::k_TCPNODELAY, 1);

            ASSERT(0 == ret);
            btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
            ASSERT(serverSocket);
            btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
            ASSERT(clientSocket);

            typedef btlso::StreamSocketFactoryDeleter Deleter;

            bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
               managedServerSocket(serverSocket,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

            ASSERT(0 == pool.import(&managedServerSocket, 0));

            btlb::Blob oneByteMsg(&ta);
            populateMessage(&oneByteMsg, 1, &ta);
            ASSERT(1 == oneByteMsg.length());

            // Wait for the channel to come up and obtain the channel id.
            bsl::vector<ChannelPoolStateCbTester::ChannelState> states(&ta);
            ASSERT(0 == mX.waitForState(&states,
                                        btlmt::ChannelPool::e_CHANNEL_UP,
                                        bsls::TimeInterval(1.0)));

            int channelId = mX.lastOpenedChannelId();
            ASSERT(0 != channelId);

            // ------------------ We are now ready to perform tests. ----------

            // 1. Verify that 'setWriteCache[Hi|Low]Watermark' and
            //    'setWriteCacheWatermarks' succeed or fail appropriately based
            //     on channelId and numBytes.

            if (verbose) {
                bsl::cout << "\tVerify invalid arguments are rejected"
                          << bsl::endl;
            }
            ASSERT(0 != pool.setWriteCacheHiWatermark(channelId + 1,
                                                      LOW_WATERMARK + 1));
            ASSERT(0 == pool.setWriteCacheHiWatermark(channelId,
                                                      LOW_WATERMARK + 1));
            ASSERT(0 == pool.setWriteCacheHiWatermark(channelId,
                                                      LOW_WATERMARK));

            pool.setWriteCacheHiWatermark(channelId, HI_WATERMARK);
            ASSERT(0 != pool.setWriteCacheLowWatermark(channelId + 1,
                                                       LOW_WATERMARK + 1));
            ASSERT(0 == pool.setWriteCacheLowWatermark(channelId,
                                                       LOW_WATERMARK + 1));
            ASSERT(0 == pool.setWriteCacheLowWatermark(channelId,
                                                       LOW_WATERMARK));
            ASSERT(0 == pool.setWriteCacheLowWatermark(channelId,
                                                       HI_WATERMARK));
            ASSERT(0 != pool.setWriteCacheLowWatermark(channelId,
                                                       HI_WATERMARK + 1));

            ASSERT(0 != pool.setWriteCacheHiWatermark(channelId,
                                                      LOW_WATERMARK - 1));
            ASSERT(0 == pool.setWriteCacheHiWatermark(channelId,
                                                      HI_WATERMARK));

            ASSERT(0 == pool.setWriteCacheLowWatermark(channelId,
                                                       LOW_WATERMARK));
            ASSERT(0 == pool.setWriteCacheLowWatermark(channelId,
                                                       LOW_WATERMARK - 1));
            ASSERT(0 == pool.setWriteCacheLowWatermark(channelId,
                                                       LOW_WATERMARK + 1));

            pool.setWriteCacheLowWatermark(channelId, LOW_WATERMARK);
            ASSERT(0 != pool.setWriteCacheWatermarks(channelId + 1,
                                                     LOW_WATERMARK + 1,
                                                     HI_WATERMARK - 1));
            ASSERT(0 == pool.setWriteCacheWatermarks(channelId,
                                                     LOW_WATERMARK + 1,
                                                     HI_WATERMARK - 1));

            ASSERT(0 == pool.setWriteCacheWatermarks(channelId,
                                                     LOW_WATERMARK,
                                                     HI_WATERMARK));
            ASSERT(0 == pool.setWriteCacheWatermarks(channelId,
                                                     LOW_WATERMARK,
                                                     LOW_WATERMARK));

            pool.setWriteCacheWatermarks(channelId,
                                         LOW_WATERMARK, HI_WATERMARK);
            ASSERT(0 == pool.setWriteCacheWatermarks(channelId,
                                                     HI_WATERMARK,
                                                     HI_WATERMARK));

            pool.setWriteCacheWatermarks(channelId,
                                         LOW_WATERMARK, HI_WATERMARK);
            ASSERT(0 == pool.setWriteCacheWatermarks(channelId,
                                                     LOW_WATERMARK - 2,
                                                     LOW_WATERMARK));

            pool.setWriteCacheWatermarks(channelId,
                                         LOW_WATERMARK, HI_WATERMARK);
            ASSERT(0 == pool.setWriteCacheWatermarks(channelId,
                                                     LOW_WATERMARK - 2,
                                                     LOW_WATERMARK - 1));

            pool.setWriteCacheWatermarks(channelId,
                                         LOW_WATERMARK, HI_WATERMARK);
            ASSERT(0 == pool.setWriteCacheWatermarks(channelId,
                                                     HI_WATERMARK,
                                                     HI_WATERMARK + 2));

            pool.setWriteCacheWatermarks(channelId,
                                         LOW_WATERMARK, HI_WATERMARK);
            ASSERT(0 == pool.setWriteCacheWatermarks(channelId,
                                                     HI_WATERMARK + 1,
                                                     HI_WATERMARK + 2));

            pool.setWriteCacheWatermarks(channelId,
                                         LOW_WATERMARK, HI_WATERMARK);

            // 2. Fill the write cache, verify the 'HIWAT' alert is delivered
            //    and no more data can be written.

            int rc = 0, numBytesWritten = 0, totalBytesWritten = 0;

            // We attempt to fill the write cache over a period of 1 second.
            // The delay is required, otherwise the cache may temporarily fill,
            // but will empty as data is transmitted to the client (and not
            // read).  Subsequent tests require that the cache starts full.
            for (int i = 0; i < 4; ++i) {

                while (0 == (rc = pool.write(channelId, oneByteMsg))) {
                    numBytesWritten   += oneByteMsg.length();
                    totalBytesWritten += oneByteMsg.length();
                }
                bslmt::ThreadUtil::sleep(bsls::TimeInterval(.1));
            }
            if (veryVerbose) {
                PT2(numBytesWritten, totalBytesWritten);
            }
            int sts = pool.write(channelId, oneByteMsg);
            LOOP_ASSERT(sts, 0 != sts);
            ASSERT(0 == mX.waitForState(
                                   &states,
                                   btlmt::ChannelPool::e_WRITE_CACHE_HIWAT,
                                   bsls::TimeInterval(0.25)));

            // 3. Double the write cache size, verify that the 'HIWAT' alert
            //    is not delivered.  Then refill the cache and verify that the
            //    cache increased by the expected amount and that 'HIWAT' is
            //    delivered.
            if (verbose) {
                bsl::cout << "\tVerify increasing the cache size" << bsl::endl;
            }
            ASSERT(0 ==
                   pool.setWriteCacheHiWatermark(channelId, 2 * HI_WATERMARK));
            ASSERT(0 != mX.waitForState(
                                   &states,
                                   btlmt::ChannelPool::e_WRITE_CACHE_HIWAT,
                                   bsls::TimeInterval(0.25)));

            numBytesWritten = 0;
            while (0 == (rc = pool.write(channelId, oneByteMsg))) {
                numBytesWritten   += oneByteMsg.length();
                totalBytesWritten += oneByteMsg.length();
            }
            if (veryVerbose) {
                PT2(numBytesWritten, totalBytesWritten);
            }
            LOOP2_ASSERT(HI_WATERMARK, numBytesWritten,
                                              HI_WATERMARK == numBytesWritten);
            ASSERT(0 != pool.write(channelId, oneByteMsg));
            ASSERT(0 == mX.waitForState(
                                   &states,
                                   btlmt::ChannelPool::e_WRITE_CACHE_HIWAT,
                                   bsls::TimeInterval(0.25)));

            // 4. Double the write cache size again, add one byte (to verify
            //    it is not full), then reduce the write cache size to the
            //    original size and verify the 'HIWAT' alert is generated.
            if (verbose) {
                bsl::cout << "\tVerify decreasing the cache size & generating "
                          << "a 'HIWAT' alert" << bsl::endl;
            }
            ASSERT(0 ==
                   pool.setWriteCacheHiWatermark(channelId, 4 * HI_WATERMARK));

            ASSERT(0 == pool.write(channelId, oneByteMsg));
            totalBytesWritten += oneByteMsg.length();
            ASSERT(0 != mX.waitForState(
                                   &states,
                                   btlmt::ChannelPool::e_WRITE_CACHE_HIWAT,
                                   bsls::TimeInterval(0.25)));

            ASSERT(0 ==
                   pool.setWriteCacheHiWatermark(channelId, HI_WATERMARK));
            ASSERT(0 == mX.waitForState(
                                   &states,
                                   btlmt::ChannelPool::e_WRITE_CACHE_HIWAT,
                                   bsls::TimeInterval(0.25)));
            ASSERT(0 != pool.write(channelId, oneByteMsg));

            // 5. Increase the write cache size back to 2 * HI_WATERMARK (one
            //    byte less than is currently in the cache); verify that no
            //    'HIWAT' alert is generated and no data can be written to the
            //    cache.
            if (verbose) {
                bsl::cout << "\tVerify a 'HIWAT' alert is not generated when "
                          << "the cache size grows to a value smaller than "
                          << "the cache" << bsl::endl;
            }
            ASSERT(0 ==
                   pool.setWriteCacheHiWatermark(channelId, 2 * HI_WATERMARK));
            ASSERT(0 != mX.waitForState(
                                   &states,
                                   btlmt::ChannelPool::e_WRITE_CACHE_HIWAT,
                                   bsls::TimeInterval(0.25)));
            const int retCode = pool.write(channelId, oneByteMsg);
#ifndef BSLS_PLATFORM_OS_WINDOWS
            ASSERT(0 != retCode);
#endif
            // 6. Empty the write cache and perform a concurrency test.
            if (verbose) {
                bsl::cout << "\tConcurrency Test" << bsl::endl;
            }
            int numBytesRead = drainSocket(clientSocket, totalBytesWritten);

#ifndef BSLS_PLATFORM_OS_WINDOWS
            LOOP2_ASSERT(numBytesRead, totalBytesWritten,
                                            numBytesRead == totalBytesWritten);
#else
            LOOP2_ASSERT(numBytesRead, totalBytesWritten,
                         numBytesRead == totalBytesWritten + 1);
#endif
            TestCaseConcurrencyTest concurrencyTest(&pool,
                                                    channelId,
                                                    clientSocket,
                                                    NUM_THREADS);
            concurrencyTest.run();
            pool.stop();
            factory.deallocate(clientSocket);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }
}

void TestDriver::testCase20()
{
        // --------------------------------------------------------------------
        // TESTING CONCERN: OpenSSL-like sockets
        //
        // Concern: That a socket which holds a (perhaps large) internal buffer
        //   and fills it up before writing, or fills much more than it can
        //   when reading, is not going to trigger any polling when data is
        //   available, and that channelpool's socket event may never be
        //   triggered even though data is available in the socket's internal
        //   buffer.  This is what happens with OpenSSL sockets.
        //
        // Plan:
        //   Replicate cases 9 and 11 with a custom-made buffered socket for
        //   testing, and the internal buffer socket being quite large.
        //
        // Testing:
        //   Concern: OpenSSL-like sockets
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING OpenSSL-LIKE SOCKETS"
                 << "\n============================" << endl;

        using namespace TEST_CASE_SSL_SOCKETS;
        typedef bteso_SslLikeStreamSocketFactory<btlso::IPv4Address> Factory;
        const int SSL_LIKE_SOCKET_BUFFER_SIZE = 4 * 65536;

        bslma::TestAllocator ta(veryVeryVerbose);

        // The following is a copy-paste of test case 9 but using
        // bteso_SslLikeStreamSocket instead of bteso_InetStreamSocket.
        {
            using namespace TEST_CASE_WRITE;

            Factory factory(SSL_LIKE_SOCKET_BUFFER_SIZE, &ta);
            runTestCase(ARGV[0], &factory, ta);
                // Note: ta passed by reference
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

        // The following is a copy-paste of test case 11, but using
        // bteso_SslLikeStreamSocket instead of bteso_InetStreamSocket.
        {
            using namespace TEST_CASE_ENABLE_DISABLE_CHANNELS;

            Factory factory(SSL_LIKE_SOCKET_BUFFER_SIZE, &ta);
            runTestCaseEnableDisable(&factory, ta); // ta passed by reference
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase19()
{
        // --------------------------------------------------------------------
        // TESTING import works under high load
        //
        // Concern: Channel pool fails to import connections under high load.
        //
        // Plan:
        //   Create an instance under test.  Create local connections and
        //   import them into the channel pool.  Wait until the import succeeds
        //   (by means of a barrier) and from the channel state callback,
        //   retrieve the channel ID, and write a number of bytes.  The amount
        //   of bytes written will control the load: the higher, the busier the
        //   channel pool.  Then wait sufficiently long enough to write all the
        //   data, close the channel pool and check on the distribution of the
        //   load among the various event managers / threads.
        //
        // Testing:
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM_OS_UNIX
        if (verbose) cout << "TESTING import works under high load" << endl
                          << "====================================" << endl;

        using namespace TEST_CASE_IMPORT;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            bsl::vector<my_ChannelEvent> channelEvents;
            bslmt::Mutex                 channelEventsMutex;

            enum {
                NUM_SOCKETS   = 100,
                MAX_THREADS   = 10,
                SERVER_ID     = 1013410001,
                BACKLOG       = 100,
                SEND_SIZE     = 16384, // 1024,
                SEND_BUFFERS  = 16
            };

            int managersStatistics[MAX_THREADS];
            bslmt::Barrier channelBarrier(2);

            // Initialize per-manager counters.
            bsl::fill_n(managersStatistics, (int)MAX_THREADS, 0);

            // Initialize IO Vecs that will be written by the object under
            // testing.
            const char *TEXT[SEND_BUFFERS][SEND_SIZE];  // contents don't
                                                        // matter
            btls::Iovec messageVecs[SEND_BUFFERS];
            for (int i = 0; i < SEND_BUFFERS; ++i) {
                messageVecs[i].setBuffer(TEXT, SEND_SIZE);
            }

            // Set up channel pool config.
            btlmt::ChannelPoolConfiguration config;
            config.setMaxConnections(NUM_SOCKETS);
            config.setMaxThreads(MAX_THREADS);
            config.setMetricsInterval(100.0);

            // Set up channel pool.
            typedef btlso::InetStreamSocketFactory<btlso::IPv4Address> Factory;
            typedef btlso::StreamSocket<btlso::IPv4Address>            Socket;

            const btlso::IPv4Address ADDRESS("127.0.0.1", 0);

            btlmt::ChannelPool *poolAddr;
            int                poolEvent = -1;
            int               *eventAddr = &poolEvent;
            int                channelId;

            Factory factory(&ta);

            // This pool will import.
            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseImportChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &eventAddr
                                        , &channelBarrier
                                        , &channelId));

            btlmt::ChannelPool::PoolStateChangeCallback poolCb;
            btlmt::ChannelPool::BlobBasedReadCallback   dataCb;

            makeNull(&poolCb);
            makeNull(&dataCb);

            Obj mX(channelCb, dataCb, poolCb, config, &ta);
            const Obj& X = mX;
            poolAddr = &mX;

            ASSERT(0 == mX.start());
            ASSERT(0 == X.numChannels());

            // And this pool will simply accept connections and consume the
            // data at a certain rate.
            btlmt::ChannelPoolConfiguration config2;
            config.setMaxConnections(NUM_SOCKETS);
            config.setMaxThreads(1);

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb2;
            btlmt::ChannelPool::BlobBasedReadCallback      dataCb2(
                bsl::allocator_arg_t(),
                bsl::allocator<btlmt::ChannelPool::BlobBasedReadCallback>(&ta),
                &caseImportDataCallback);

            btlmt::ChannelPool::PoolStateChangeCallback    poolCb2;

            makeNull(&channelCb2);
            makeNull(&poolCb2);

            Obj mY(channelCb2, dataCb2, poolCb2, config, &ta);
            const Obj& Y = mY;

            ASSERT(0 == mY.start());
            ASSERT(0 == mY.listen(ADDRESS, BACKLOG, SERVER_ID));
            ASSERT(0 == Y.numChannels());
            btlso::IPv4Address PEER = getServerLocalAddress(&mY, SERVER_ID);

            // Import lots of sockets and send some data.
            if (verbose) cout << "Importing sockets.\n";
            btlso::StreamSocket<btlso::IPv4Address> *
                                                    clientSockets[NUM_SOCKETS];
            for (int i = 0; i < NUM_SOCKETS; ++i) {
                Socket  *socket = factory.allocate();
                clientSockets[i] = socket;

                int ret = socket->connect(PEER);
                LOOP_ASSERT(i, 0 == ret);

                if (0 == ret) {

                    typedef btlso::StreamSocketFactoryDeleter Deleter;

                    bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                        managedSocket(
                                   socket,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

                    ret = mX.import(&managedSocket, i);
                    LOOP_ASSERT(i, 0 == ret);
                    if (0 == ret) {
                        // Other barrier is in caseImportChannelStateCallback,
                        // called by import.
                        channelBarrier.wait();
                    }
                }
                if (0 != ret) {
                    factory.deallocate(clientSockets[i]);
                    clientSockets[i] = 0;
                }
                else {
                    // Unfortunately, the callback has no idea what thread it
                    // was assigned to...
                    mX.write(channelId,
                             (btls::Iovec*)&messageVecs,
                             SEND_BUFFERS);
                }
            }

//             bslmt::ThreadUtil::microSleep(0, 3); // 3 sec should be enough

            if (verbose)
                cout << "Shutting down" << endl;

            mX.stop();
            mY.stop();

            if (verbose) {
                int totalNumEvents = 0;
                cout << "Distribution per manager:" << endl;
                for (int i = 0; i < MAX_THREADS; ++i) {
                    // T_(); P_(i); T_(); P(managersStatistics[i]);
                    T_(); P_(i); T_(); P(X.numEvents(i));
                    totalNumEvents += X.numEvents(i);
                }
                P(totalNumEvents);
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }
#endif

}

void TestDriver::testCase18()
{
        // --------------------------------------------------------------------
        // TESTING accept fails if FD limit reached
        //
        // TBD: this test case is intermittently taking a very long time, as
        //      much as 13 minutes.  Most of the time it runs quickly.
        //
        // Concern: Channel pool must fail to accept connections if FD limit is
        //   reached (works as designed).
        //
        // Plan: Create an instance under test.  Make sure the FD limit is set
        //   properly to the number of sockets that will try to connect to the
        //   channel pool instance.  Note that each process has four time the
        //   number of FDs open (two per each socket in the main process, plus
        //   two for a channel).  In addition, it has 2 open FDs for cin and
        //   cout.  Then connect one too many times and assert (via the
        //   callback) that the behavior is as designed.  Finally, after
        //   clearing these connections, assert that the specified number of
        //   connections can still occur.
        //
        //   Note that the number of FDs is *not* the same when this test
        //   driver is run on the command line, or when it is run in the
        //   nightly builds (because the output is logged).  So we run it with
        //   MAX_THREADS == 5 instead of 1, just to buffer the difference in
        //   the number of FDs open.
        //
        // Testing:
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM_OS_UNIX
        if (verbose)
             cout << "\nTESTING accept fails if FD limit reached"
                  << "\n========================================" << endl;

        using namespace TEST_CASE_ACCEPT;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                MAX_THREADS = 5,
                SERVER_ID   = 1013410001,
                BACKLOG     = 1
            };

            if (verbose)
               cout << "Configuring channel pool and setting FD limit" << endl;

            bsls::AtomicInt64 acceptErrors(0);

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb;
            makeNull(&channelCb);

            btlmt::ChannelPool::PoolStateChangeCallback poolCb(
                                 bdlf::BindUtil::bind(
                                                  &caseAcceptPoolStateCallback,
                                                  _1, _2, _3,
                                                  &acceptErrors));

            btlmt::ChannelPool::BlobBasedReadCallback dataCb;
            makeNull(&dataCb);

            btlmt::ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_THREADS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(100.0);

            struct rlimit rlim;
            ASSERT(0 == getrlimit(RLIMIT_NOFILE, &rlim));

#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_LINUX)
            rlim.rlim_cur = 4 * MAX_THREADS + 2;
#else
            rlim.rlim_cur = 4 * MAX_THREADS + 5;
#endif
            ASSERT(0 == setrlimit(RLIMIT_NOFILE, &rlim));
            if (veryVerbose) { P(rlim.rlim_cur); }

            Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;

            const btlso::IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == mX.start());
            ASSERT(0 == mX.listen(ADDRESS, BACKLOG, SERVER_ID));
            ASSERT(0 == X.numChannels());
            btlso::IPv4Address PEER = getServerLocalAddress(&mX, SERVER_ID);

            typedef btlso::InetStreamSocketFactory<btlso::IPv4Address> Factory;
            typedef btlso::StreamSocket<btlso::IPv4Address>            Socket;
            typedef btlsos::TcpChannel                                Channel;

            Factory               factory(&ta);
            bsl::vector<Socket *> sockets(MAX_THREADS);
            int      retCode;

            if (verbose)
                cout << "Establishing connection (should not accept)" << endl;

            for (int i = 0; i < MAX_THREADS; ++i) {
                sockets[i] = factory.allocate();
                if (!sockets[i]) {
                    // On some platforms, we seem to exceed the number of
                    // sockets we can open.  This is not really a failure.
                    // Temporarily disable test case for those platforms.

                    while (0 < i) {
                        factory.deallocate(sockets[--i]);
                    }
                    ASSERT(0 == btlso::SocketImpUtil::cleanup());
                    return;                                           // RETURN
                }
                LOOP_ASSERT(i, sockets[i]);
                retCode = sockets[i]->setBlockingMode(
                                           btlso::Flag::e_NONBLOCKING_MODE);
                LOOP2_ASSERT(i, retCode, 0 == retCode);
            }

            for (int i = 0; i < MAX_THREADS; ++i) {
                retCode = sockets[i]->connect(PEER);
                if (veryVerbose) { P_(retCode); P(errno); }

                // Give time to the channel pool to process accept error
                // callbacks.
                bslmt::ThreadUtil::microSleep(100, 0);
            }

            // Give time to the channel pool to process accept error callbacks.
//             bslmt::ThreadUtil::microSleep(0, 8);

            ASSERT(acceptErrors);

            if (veryVerbose) { P(acceptErrors); }

            for (int i = 0; i < MAX_THREADS; ++i) {
                factory.deallocate(sockets[i]);
            }

            if (verbose)
                cout << "Reset FD limit to max available" << endl;

            ASSERT(0 == getrlimit(RLIMIT_NOFILE, &rlim));
            rlim.rlim_cur = rlim.rlim_max;
            ASSERT(0 == setrlimit(RLIMIT_NOFILE, &rlim));
            if (veryVerbose) { P(rlim.rlim_cur); }

            bslmt::ThreadUtil::microSleep(0, 8);
            acceptErrors = 0;

            if (verbose)
                cout << "Establishing connection (should succeed)" << endl;
#ifndef BSLS_PLATFORM_OS_LINUX
            // The Linux machine used in the nightly builds does not properly
            // reset the FD limit to its max value (more exactly, it does but
            // it still fails to accept beyond the previous FD limit).  Comment
            // out this code for now, until a new test machine is found.

            for (int i = 0; i < MAX_THREADS; ++i) {
                sockets[i] = factory.allocate();
                LOOP_ASSERT(i, sockets[i]);
                retCode = sockets[i]->setBlockingMode(
                                              btlso::Flag::e_BLOCKING_MODE);
                LOOP2_ASSERT(i, retCode, 0 == retCode);
            }

            for (int i = 0; i < MAX_THREADS; ++i) {
                retCode = sockets[i]->connect(PEER);
                // LOOP2_ASSERT(i, retCode, 0 == retCode);
                if (veryVerbose) { P_(retCode); P(errno); }
            }

            // Give time to the channel pool to process accept error callbacks.
            bslmt::ThreadUtil::microSleep(0, 10);

            ASSERT(0 == acceptErrors);
            if (veryVerbose) { P(acceptErrors); }

            for (int i = 0; i < MAX_THREADS; ++i) {
                // LOOP_ASSERT(i, 0 == sockets[i]->connectionStatus());
                if (veryVerbose) { P(sockets[i]->connectionStatus()); }
                factory.deallocate(sockets[i]);
            }
#endif
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }
#endif

}

void TestDriver::testCase17()
{
        // --------------------------------------------------------------------
        // TESTING 'maxConnections' limit is respected
        //
        // Concerns:
        //   * That a channel pool can only create the number of channels
        //     specified by the 'maxConnections' argument of the
        //     'btlmt::ChannelPoolConfiguration'.  In particular, we are
        //     concerned that
        //        - exactly this many channels may be allocated for accepting
        //        - exactly this many channels may be allocated for connecting
        //        - exactly this many channels may be allocated for some
        //          combination of accepting and connecting
        //   * That a channel that is no longer in use is freed for re-use by
        //     the channel pool.
        //
        // Plan:
        //   Instantiate a modifiable 'btlmt::ChannelPool', 'mX', configured
        //   with a fixed number of 'maxConnections'.  Create a non-modifiable
        //   reference to 'mX' named 'X'.  Establish 'maxConnections' number of
        //   connections.  Verify using 'X' that the channel is currently
        //   managing the specified maximum number of connections.  Close the
        //   connections, and verify using 'X' that the number of connections
        //   is 0.  Re-open the connections, and verify again that the
        //   specified maximum number of connections are established.  Attempt
        //   to establish one additional connection, and verify that it is
        //   not accepted.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING 'maxConnections' limit is respected" << endl
                 << "===========================================" << endl;

        using namespace TEST_CASE_MAX_CONNS_LIMIT;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS = 16,
                MAX_THREADS = 2,
                SERVER_ID   = 1010010001,
                BACKLOG     = 1
            };

            bslmt::Barrier  channelBarrier(2);
            bsls::AtomicInt limitReachedFlag;

            btlmt::ChannelPool *poolAddr;
            int                 poolEvent = -1;
            int                *eventAddr = &poolEvent;

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseMaxConnsChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &eventAddr
                                        , &channelBarrier));

            btlmt::ChannelPool::PoolStateChangeCallback    poolCb(
                    bdlf::BindUtil::bind( &caseMaxConnsPoolStateCallback
                                        , _1, _2, _3
                                        , &eventAddr
                                        , &limitReachedFlag));

            btlmt::ChannelPool::BlobBasedReadCallback         dataCb;
            makeNull(&dataCb);

            btlmt::ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setIncomingMessageSizes(1, 32, 256);

            Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;
            poolAddr = &mX;

            const btlso::IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == mX.start());
            int retListen = mX.listen(ADDRESS, BACKLOG, SERVER_ID);
            LOOP_ASSERT(retListen, 0 == retListen);
            ASSERT(0 == X.numChannels());
            btlso::IPv4Address PEER = getServerLocalAddress(&mX, SERVER_ID);

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            typedef btlso::InetStreamSocketFactory<btlso::IPv4Address> Factory;
            typedef btlso::StreamSocket<btlso::IPv4Address>            Socket;
            typedef btlsos::TcpChannel                                 Channel;

            Factory              factory(&ta);
            bsl::vector<Socket*> sockets(&ta);
            sockets.reserve(MAX_CLIENTS);

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\nEstablish MAX_CLIENTS connections."
                     << "\n----------------------------------"
                     << endl;
            }
            {
                for (int i = 0; i < MAX_CLIENTS; ++i) {
                    Socket  *socket = factory.allocate();
                    Channel  channel(socket, &ta);

                    ASSERT(0 == socket->connect(PEER));
                    ASSERT(0 == channel.isInvalid());

                    channelBarrier.wait();
                    LOOP_ASSERT(i, i+1 == X.numChannels());
                    sockets.push_back(socket);
                }
                while (!limitReachedFlag) {
                    bslmt::ThreadUtil::microSleep(100);
                    bslmt::ThreadUtil::yield();
                }
                ASSERT(PoolState::BTEMT_CHANNEL_LIMIT == poolEvent);
                ASSERT(MAX_CLIENTS == X.numChannels());
            }
            if (veryVerbose) { P(ta); }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\nClose and re-open connections."
                     << "\n------------------------------"
                     << endl;
            }
            {
                if (veryVerbose) {
                    cout << "\t(a) close connections" << endl
                         << "\t---------------------" << endl;
                }
                limitReachedFlag = false;
                {
                    for (bsl::vector<Socket*>::iterator it = sockets.begin();
                         it != sockets.end(); ++it)
                    {
                        Socket  *socket = *it;
                        Channel  channel(socket);

                        channel.invalidate();
                        factory.deallocate(socket);
                        channelBarrier.wait();
                    }
                    ASSERT(0  == X.numChannels());
                    LOOP_ASSERT(poolEvent, -1 == poolEvent);
                    sockets.clear();
                }
                if (veryVerbose) { P(ta); }

                if (veryVerbose) {
                    cout << "\t(b) open connections" << endl
                         << "\t-------------------" << endl;
                }
                {
                    for (int i = 0; i < MAX_CLIENTS; ++i) {
                        Socket  *socket = factory.allocate();
                        Channel  channel(socket, &ta);

                        ASSERT(0 == socket->connect(PEER));
                        ASSERT(0 == channel.isInvalid());

                        channelBarrier.wait();
                        LOOP_ASSERT(i, i+1 == X.numChannels());
                        sockets.push_back(socket);
                    }
                    while (!limitReachedFlag) {
                        bslmt::ThreadUtil::microSleep(100);
                        bslmt::ThreadUtil::yield();
                    }
                    ASSERT(PoolState::BTEMT_CHANNEL_LIMIT == poolEvent);
                    ASSERT(MAX_CLIENTS == X.numChannels());
                }
                if (veryVerbose) { P(ta); }
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\nAttempt to open MAX_CLIENTS + 1 connection."
                     << "\n-------------------------------------------"
                     << endl;
            }
            {
                Socket  *socket = factory.allocate();
                Channel  channel(socket);

                ASSERT(0 == socket->connect(PEER));
                ASSERT(0 == channel.isInvalid());

                bslmt::ThreadUtil::yield();
                ASSERT(MAX_CLIENTS == X.numChannels());
                ASSERT(PoolState::BTEMT_CHANNEL_LIMIT == poolEvent);

                channel.invalidate();
                factory.deallocate(socket);
            }
            if (veryVerbose) { P(ta); }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (verbose) {
                cout << "\nClose connections and stop server."
                     << "\n----------------------------------"
                     << endl;
            }
            {
                for (bsl::vector<Socket*>::iterator it = sockets.begin();
                     it != sockets.end(); ++it)
                {
                    Socket  *socket = *it;
                    Channel  channel(socket);

                    channel.invalidate();
                    factory.deallocate(socket);
                    channelBarrier.wait();
                }
                bslmt::ThreadUtil::yield();
                ASSERT(0  == X.numChannels());
                ASSERT(-1 == poolEvent);
            }
            if (veryVerbose) { P(ta); }
            ASSERT(0 == mX.stop());
            ASSERT(0 == X.numChannels());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase16()
{
        // --------------------------------------------------------------------
        // TESTING channel callbacks are called with correct ids
        //
        // Concerns:
        //   That CHANNEL_UP and CHANNEL_DOWN events are delivered with valid
        //   channel and server IDs for all connections supported by a single-
        //   threaded server when the listen channel is shutdown.
        //
        // Plan:
        //   Create a 'btlmt::ChannelPool' object, 'mX', and open a channel in
        //   listen mode.  Open a connection to this "server", and verify the
        //   channel and server IDs in the bound channel state callback
        //   function.  Wait for some period, and then close the client
        //   channel via 'shutdown'.  Verify that a CHANNEL_DOWN event is
        //   processed for the open connection with valid channel and server ID
        //   parameters.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING channel callbacks are called with correct ids"
                 << "\n====================================================="
                 << endl;

        using namespace TEST_CASE_CALLBACKS_TEST;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 1,
                MAX_THREADS  = 1,
                PORT         = 0,
                SERVER_ID    = 0x701d50da,
                BACKLOG      = 1
            };

            bslmt::Barrier barrier(2);
            int            channelId = 0;

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseCallbacksChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &channelId
                                        , &barrier));

            btlmt::ChannelPool::PoolStateChangeCallback poolCb;
            btlmt::ChannelPool::BlobBasedReadCallback   dataCb;

            makeNull(&ta, &poolCb);
            makeNull(&dataCb);

            btlmt::ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setIncomingMessageSizes(1, 32, 256);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, cpc, &ta);
            ASSERT(0 == pool.start());
            ASSERT(0 == pool.listen(PORT, BACKLOG, SERVER_ID));
            btlso::IPv4Address PEER = getServerLocalAddress(&pool, SERVER_ID);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                            factory.allocate();
            btlsos::TcpChannel                      channel(socket);

            ASSERT(0 == socket->connect(PEER));
            ASSERT(0 == channel.isInvalid());

            barrier.wait();
            ASSERT(1 == pool.numChannels());
            ASSERT(0 != channelId);
            ASSERT(0 == pool.shutdown(channelId,
                                      btlmt::ChannelPool::e_IMMEDIATE));
            barrier.wait();

            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == pool.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase15()
{
        // --------------------------------------------------------------------
        // TESTING CONCERN: SHUTDOWN INSIDE CHANNEL STATE CALLBACK
        //
        // Concerns:
        //   That 'shutdown' is correctly processed when called from inside
        //   the channel state callback function.
        //
        // Plan:
        //   Create a 'btlmt::ChannelPool' object, 'mX', and open a channel in
        //   listen mode.  Open a connection to this "server", and verify the
        //   channel and server ID in the bound channel state callback
        //   function.  In the CHANNEL_UP branch, close the client
        //   channel via 'shutdown'.  Verify that a CHANNEL_DOWN event is
        //   processed for the open connection with valid channel and server ID
        //   parameters.
        //
        // Testing:
        //   Concern: Shutdown inside channel state callback
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting Concern: Shutdown inside channel state callback"
                 << "\n======================================================="
                 << endl;

        using namespace TEST_CASE_SHUTDOWN_IN_USER_CALLBACK;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 1,
                MAX_THREADS  = 1,
                SERVER_ID    = 0x701d50da,

                PORT         = 0,
                BACKLOG      = 1
            };

            btlmt::ChannelPool::BlobBasedReadCallback  dataCb;
            btlmt::ChannelPool::PoolStateChangeCallback       poolCb;

            btlmt::ChannelPool *poolAddr = 0;
            bslmt::Barrier      barrier(2);
            int                 channelId = 0;

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseShutdownChannelStateCallback
                                        , _1, _2, _3, _4
                                        , (int)SERVER_ID
                                        , &poolAddr
                                        , &channelId
                                        , &barrier));

            makeNull(&ta, &dataCb);
            makeNull(&ta, &poolCb);

            btlmt::ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setIncomingMessageSizes(1, 32, 256);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, cpc, &ta);
            poolAddr = &pool;
            ASSERT(0 == pool.start());
            ASSERT(0 == pool.listen(PORT, BACKLOG, SERVER_ID));
            btlso::IPv4Address PEER = getServerLocalAddress(&pool, SERVER_ID);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                            factory.allocate();
            btlsos::TcpChannel                      channel(socket);

            ASSERT(0 == socket->connect(PEER));
            ASSERT(0 == channel.isInvalid());

            barrier.wait();
            ASSERT(0 != pool.shutdown(channelId,
                                      btlmt::ChannelPool::e_IMMEDIATE));

            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == pool.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase14()
{
        // --------------------------------------------------------------------
        // Testing getHandleStatistics facility
        //
        // Concerns:
        //   * That 'getHandleStatistics' correctly reports all open socket
        //     handles, whether created by 'listen', 'connect', or 'import'
        //
        // Plan:
        //   Instantiate a channel pool, exercise 'listen', 'connect', and
        //   'import', and call 'getHandleStatistics' at various points to
        //   check the expected result.  Note that the handles are not reported
        //   in any specific order, so we sort them to make sure the result is
        //   canonical.
        //
        // Testing:
        //   void btlmt::ChannelPool::getHandleStatistics*(...);
        // --------------------------------------------------------------------

        if (verbose)
             cout << "\nTesting getHandleStatistics facility"
                  << "\n====================================" << endl;

#ifndef BSLS_PLATFORM_OS_WINDOWS
        using namespace TEST_CASE_GET_HANDLE_STATS;
        bslma::TestAllocator ta(veryVeryVerbose);
        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                NUM_CONNECT_ATTEMPTS = 10,  // for client channel
                TIMEOUT              = 1,   // 1s
                NUM_CLOCKS           = 10,
                NUM_INVOCATIONS      = 10,
                SETUP_TIMEOUT        = 20   // 20s
            };

            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(10);
            config.setReadTimeout(1);

            btlmt::ChannelPool::PoolStateChangeCallback  poolCb;
            btlmt::ChannelPool::BlobBasedReadCallback         dataCb;

            int acceptedChannelId1 = -1;
            int channelId1 = -1;
            int channelId2 = -1;

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseHandleStatsChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &acceptedChannelId1
                                        , &channelId1
                                        , &channelId2
                                        , veryVerbose));

            bsl::vector<btlmt::ChannelPool::HandleInfo> handles;
            btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);

            if (verbose) cout << "\tVerifying that nothing is picked up.\n";
            {
                handles.clear();
                mX.getHandleStatistics(&handles);
                ASSERT(handles.empty());
                if (veryVerbose) { P(handles); }

                ASSERT(0 == mX.start());

                handles.clear();
                mX.getHandleStatistics(&handles);
                ASSERT(handles.empty());
                if (veryVerbose) { P(handles); }
            }

            if (verbose) cout << "\tVerifying that 'listen' is picked up.\n";

            bsls::TimeInterval serverTime = bdlt::CurrentTime::now();
            {
                ASSERT(0 == mX.listen(0, 1, SERVER_ID));

                handles.clear();
                mX.getHandleStatistics(&handles);

                ASSERT(1 == handles.size());
                if (1 <= handles.size()) {
                    ASSERT(btlmt::ChannelType::e_LISTENING_CHANNEL
                                      == handles[0].d_channelType);
                    ASSERT(-1         == handles[0].d_channelId);
                    ASSERT(serverTime <= handles[0].d_creationTime);
                    ASSERT(SERVER_ID  == handles[0].d_userId);
                }
                if (veryVerbose) { P(handles); }

                // Remember server creation time (must not change from now on).

                serverTime = handles[0].d_creationTime;
            }

            btlso::IPv4Address address = getServerLocalAddress(&mX, SERVER_ID);

            if (verbose) cout << "\tVerifying that 'connect' is picked up.\n";

            bsls::TimeInterval connectTime = bdlt::CurrentTime::now();
            bsls::TimeInterval acceptTime  = connectTime;
            {
                ASSERT(0 == mX.connect(address,
                                       NUM_CONNECT_ATTEMPTS,
                                       bsls::TimeInterval(TIMEOUT),
                                       CLIENT_ID1));

                // Wait until channel has been created.

                for (int i = 0; (-1 == acceptedChannelId1 || -1 == channelId1)
                                                   && i < SETUP_TIMEOUT; ++i) {
                    bslmt::ThreadUtil::sleep(bsls::TimeInterval(0.1));
                    bslmt::ThreadUtil::yield();
                }

                handles.clear();
                mX.getHandleStatistics(&handles);

                ASSERT(3 == handles.size());
                bsl::sort(handles.begin(), handles.end(), LessThanByType());
                if (3 <= handles.size()) {
                    LOOP_ASSERT(handles[0].d_channelType,
                                btlmt::ChannelType::e_LISTENING_CHANNEL
                                              == handles[0].d_channelType);
                    ASSERT(-1                 == handles[0].d_channelId);
                    ASSERT(serverTime         == handles[0].d_creationTime);
                    ASSERT(SERVER_ID          == handles[0].d_userId);

                    LOOP_ASSERT(handles[1].d_channelType,
                                btlmt::ChannelType::e_ACCEPTED_CHANNEL
                                              == handles[1].d_channelType);
                    ASSERT(acceptedChannelId1 == handles[1].d_channelId);
                    ASSERT(acceptTime         <= handles[1].d_creationTime);
                    ASSERT(SERVER_ID          == handles[1].d_userId);

                    LOOP_ASSERT(handles[2].d_channelType,
                                btlmt::ChannelType::e_CONNECTED_CHANNEL
                                              == handles[2].d_channelType);
                    ASSERT(channelId1         == handles[2].d_channelId);
                    ASSERT(connectTime        <= handles[2].d_creationTime);
                    ASSERT(CLIENT_ID1         == handles[2].d_userId);
                }
                if (veryVerbose) { P(handles); }

                // Remember connection time (must not change from now on).

                acceptTime  = handles[1].d_creationTime;
                connectTime = handles[2].d_creationTime;
            }

            if (verbose) cout << "\tVerifying that 'import' is picked up.\n";

            bsls::TimeInterval importTime;
            {
                btlso::SocketHandle::Handle socketHandles[2];
                ASSERT(0 ==
                       btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                   socketHandles,
                                   btlso::SocketImpUtil::k_SOCKET_STREAM));

                btlso::StreamSocket<btlso::IPv4Address> *socket =
                                            factory.allocate(socketHandles[0]);

                handles.clear();
                mX.getHandleStatistics(&handles);
                ASSERT(3 == handles.size());  // make sure

                importTime = bdlt::CurrentTime::now();

                typedef btlso::StreamSocketFactoryDeleter Deleter;

                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                     managedSocket(socket,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

                ASSERT(0 == mX.import(&managedSocket, CLIENT_ID2));

                // Wait until channel has been created.

                for (int i = 0; (-1 == channelId2) && i < SETUP_TIMEOUT; ++i) {
                    bslmt::ThreadUtil::sleep(bsls::TimeInterval(0.1));
                    bslmt::ThreadUtil::yield();
                }

                handles.clear();
                mX.getHandleStatistics(&handles);

                ASSERT(4 == handles.size());
                bsl::sort(handles.begin(), handles.end(), LessThanByType());
                if (4 <= handles.size()) {
                    ASSERT(btlmt::ChannelType::e_LISTENING_CHANNEL
                                              == handles[0].d_channelType);
                    ASSERT(-1                 == handles[0].d_channelId);
                    ASSERT(serverTime         == handles[0].d_creationTime);
                    ASSERT(SERVER_ID          == handles[0].d_userId);

                    ASSERT(btlmt::ChannelType::e_ACCEPTED_CHANNEL
                                              == handles[1].d_channelType);
                    ASSERT(acceptedChannelId1 == handles[1].d_channelId);
                    ASSERT(acceptTime         == handles[1].d_creationTime);
                    ASSERT(SERVER_ID          == handles[1].d_userId);

                    ASSERT(btlmt::ChannelType::e_CONNECTED_CHANNEL
                                              == handles[2].d_channelType);
                    ASSERT(channelId1         == handles[2].d_channelId);
                    ASSERT(connectTime        == handles[2].d_creationTime);
                    ASSERT(CLIENT_ID1         == handles[2].d_userId);

                    ASSERT(btlmt::ChannelType::e_IMPORTED_CHANNEL
                                              == handles[3].d_channelType);
                    ASSERT(channelId2         == handles[3].d_channelId);
                    ASSERT(importTime         <= handles[3].d_creationTime);
                    ASSERT(CLIENT_ID2         == handles[3].d_userId);
                }
                if (veryVerbose) { P(handles); }

                importTime  = handles[3].d_creationTime;
            }

            if (verbose) cout << "\tVerifying that 'close' removes server.\n";
            {
                mX.close(SERVER_ID);

                handles.clear();
                mX.getHandleStatistics(&handles);

                ASSERT(3 == handles.size());
                bsl::sort(handles.begin(), handles.end(), LessThanByType());
                if (3 <= handles.size()) {
                    ASSERT(btlmt::ChannelType::e_ACCEPTED_CHANNEL
                                              == handles[0].d_channelType);
                    ASSERT(acceptedChannelId1 == handles[0].d_channelId);
                    ASSERT(acceptTime         == handles[0].d_creationTime);
                    ASSERT(SERVER_ID          == handles[0].d_userId);

                    ASSERT(btlmt::ChannelType::e_CONNECTED_CHANNEL
                                              == handles[1].d_channelType);
                    ASSERT(channelId1         == handles[1].d_channelId);
                    ASSERT(connectTime        == handles[1].d_creationTime);
                    ASSERT(CLIENT_ID1         == handles[1].d_userId);

                    ASSERT(btlmt::ChannelType::e_IMPORTED_CHANNEL
                                              == handles[2].d_channelType);
                    ASSERT(channelId2         == handles[2].d_channelId);
                    ASSERT(importTime         == handles[2].d_creationTime);
                    ASSERT(CLIENT_ID2         == handles[2].d_userId);
                }
                if (veryVerbose) { P(handles); }
            }

            if (verbose) cout << "\tVerifying after 'shutdown'.\n";
            {
                mX.shutdown(channelId1);

                // Wait until channels have been destroyed.

                for (int i = 0; (-1 != acceptedChannelId1 || -1 != channelId1)
                                                   && i < SETUP_TIMEOUT; ++i) {
                    bslmt::ThreadUtil::sleep(bsls::TimeInterval(0.1));
                    bslmt::ThreadUtil::yield();
                }

                handles.clear();
                mX.getHandleStatistics(&handles);

                ASSERT(1 == handles.size());
                ASSERT(btlmt::ChannelType::e_IMPORTED_CHANNEL
                                                  == handles[0].d_channelType);
                ASSERT(channelId2 == handles[0].d_channelId);
                ASSERT(importTime == handles[0].d_creationTime);
                ASSERT(CLIENT_ID2 == handles[0].d_userId);
                if (veryVerbose) { P(handles); }

                mX.shutdown(channelId2);

                // Wait until channels have been destroyed.

                for (int i = 0; (-1 != channelId2) && i < SETUP_TIMEOUT; ++i) {
                    bslmt::ThreadUtil::sleep(bsls::TimeInterval(0.1));
                    bslmt::ThreadUtil::yield();
                }

                handles.clear();
                mX.getHandleStatistics(&handles);
                ASSERT(handles.empty());

                if (veryVerbose) { P(handles); }
            }
        }

#endif
}

void TestDriver::testCase13()
{
        // --------------------------------------------------------------------
        // Testing numBytes*(), totalBytes*() and getChannelStatistics*()
        // facility
        //
        // Concerns:
        //   * That numBytesRead() reports the expected number if supplied
        //     a valid channel id; and otherwise reports 1.
        //   * That numBytesWritten() reports the expected number if supplied
        //     a valid channel id; and otherwise reports 1.
        //   * That numBytesRequestedToBeWritten() reports the expected number
        //     if supplied a valid channel id; and otherwise reports 1.
        //   * That the totalBytes*() facilities report the correct numbers.
        //   * That the *Reset() functions reset the statistics.
        //
        // Plan:
        //   Instantiate a channel pool configured to use only one thread, put
        //   it in 'listen' mode and start.  Exercise the tested facility to
        //   verify that the methods appropriately report an error since at
        //   this point no channels have been create.  Then connect a client
        //   to the pool.  The client will write as much at it can to the
        //   server which feeds back everything it received back to the client.
        //   Since the client will not be reading, write() will eventually
        //   fail.  At this point, we will make the client read small chunks
        //   by small chunks.  Between each read, we will exercise the
        //   facility to verify everything is correct.  At the end of read
        //   process, we will compare the provided numbers with the number
        //   the client effectively sent to the server.  We will reiterate
        //   the whole process to check for consistency.  At the end,
        //   we will check the '*Reset' functions().
        //
        // Testing:
        //   int btlmt::ChannelPool::numBytes*(...);
        //   int btlmt::ChannelPool::totalBytes*(...);
        //   int btlmt::ChannelPool::getChannelStatistics*(...);
        // --------------------------------------------------------------------

        if (verbose)
             cout << "\nTesting {num,total}Bytes*() facility"
                  << "\n====================================" << endl;

#ifndef BSLS_PLATFORM_OS_WINDOWS
        using namespace TEST_CASE_GET_CHANNEL_STATS;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_THREADS = 1,
                SERVER_ID   = 1013410001,
                BACKLOG     = 1
            };

            btlmt::ChannelPool::PoolStateChangeCallback    poolCb;
            makeNull(&poolCb);

            bslmt::Barrier  channelBarrier(2);
            bsls::AtomicInt fail(0);

            btlmt::ChannelPool *poolAddr;
            int                 poolEvent = -1;
            int                *eventAddr = &poolEvent;
            int                 channelId;

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseChannelStatsChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &eventAddr
                                        , &channelBarrier
                                        , &channelId));

            btlmt::ChannelPool::BlobBasedReadCallback         dataCb(
                    bdlf::BindUtil::bind( &caseChannelStatsChannelDataCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &fail));

            btlmt::ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_THREADS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(100.0);
            cpc.setOutgoingMessageSizes(0, 1024, 4096);

            Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;
            poolAddr = &mX;

            const btlso::IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == mX.start());
            ASSERT(0 == mX.listen(ADDRESS, BACKLOG, SERVER_ID));
            ASSERT(0 == X.numChannels());
            btlso::IPv4Address PEER = getServerLocalAddress(&mX, SERVER_ID);

            typedef btlso::InetStreamSocketFactory<btlso::IPv4Address> Factory;
            typedef btlso::StreamSocket<btlso::IPv4Address>            Socket;
            typedef btlsos::TcpChannel                                 Channel;

            Factory              factory(&ta);

            Socket  *socket = factory.allocate();
            Channel  channel(socket, &ta);

            int connectRetCode =  socket->connect(PEER);
            ASSERT(0 == connectRetCode);
            ASSERT(0 == channel.isInvalid());

            if (0 == connectRetCode) {
                channelBarrier.wait();
            }
            else {
                cout << "Failed to connect to channelpool.  Abort.\n";
                return;                                               // RETURN
            }

            ASSERT(0 == mX.setSocketOption(
                                       btlso::SocketOptUtil::k_SENDBUFFER,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       512, channelId));

            char buffer[1024];
            bsls::Types::Int64 clientBytesWritten = 0;
            while(!fail) {
                ASSERT(sizeof(buffer) == channel.write(0, buffer,
                                                       sizeof(buffer),0));
                clientBytesWritten += sizeof(buffer);
                if (veryVerbose) {
                    MTCOUT << "client wrote " << clientBytesWritten
                           << MTENDL;
                }
                #ifdef BSLS_PLATFORM_OS_AIX
                if (clientBytesWritten >= 1024) {
                    break; // AIX is brain-dead.
                }
                #endif
            }
            bslmt::ThreadUtil::microSleep(200 * 1000);
            LOOP_ASSERT(X.numChannels(), 1 == X.numChannels());

            bsls::Types::Int64 clientBytesRead = 0;
            bsls::Types::Int64 bytesWritten;
            bsls::Types::Int64 bytesRequestedToBeWritten;
            bsls::Types::Int64 bytesRead;
            do {
                LOOP_ASSERT(X.numChannels(), 1 == X.numChannels());

                #ifndef BSLS_PLATFORM_OS_AIX
                int r =  channel.read(buffer, sizeof(buffer), 0);
                #else
                // AIX is doing something very very weird.  The following
                // read() blocks even if the client's buffer is full or has
                // more data than we request.  Reading very small chunks works
                // but it is slow.

                int r;
                if ((clientBytesWritten - clientBytesRead) >= 50) {
                    r =  channel.read(buffer, 50, 0);
                }
                else {
                    r =  channel.read(buffer, 1, 0);
                }
                #endif
                ASSERT(r >= 0);
                clientBytesRead += r;
                ASSERT(0 == X.numBytesRead(&bytesRead, channelId));
                ASSERT(0 == X.numBytesWritten(&bytesWritten, channelId));
                ASSERT(0 == X.numBytesRequestedToBeWritten(
                                       &bytesRequestedToBeWritten, channelId));
                if (veryVerbose) {
                    MTCOUT << "client read "      << clientBytesRead    << ", "
                           << "client wrote "     << clientBytesWritten << ", "
                           << "server read "      << bytesRead          << ", "
                           << "server wrote "     << bytesWritten       << ", "
                           << "server requested " << bytesRequestedToBeWritten
                           << MTENDL;
                }
                // Test consistency.  We cannot use '==' since we're racing
                // against the channel thread.

                bsls::Types::Int64 temp;
                X.totalBytesRead(&temp);
                ASSERT(temp >= bytesRead);
                X.totalBytesWritten(&temp);
                ASSERT(temp >= bytesWritten);
                X.totalBytesRequestedToBeWritten(&temp);
                ASSERT(temp >= bytesRequestedToBeWritten);

                ASSERT(clientBytesRead <= clientBytesWritten);

                // We cannot assert anything between clientBytesRead and
                // bytesWritten because we do not know what is going to happen
                // first: the network transfer or the execution of the
                // callback.  While it is more likely that the callback is
                // going to be executed first, it is not always the case.

                ASSERT(bytesRequestedToBeWritten >= clientBytesRead);
                ASSERT(bytesRequestedToBeWritten >= bytesWritten);

            } while (clientBytesWritten != clientBytesRead);

            ASSERT(0 == X.numBytesWritten(&bytesWritten, channelId));
            ASSERT(0 == X.numBytesRequestedToBeWritten(
                                       &bytesRequestedToBeWritten, channelId));
            LOOP2_ASSERT(bytesWritten, bytesRequestedToBeWritten,
                                    bytesWritten == bytesRequestedToBeWritten);
            LOOP2_ASSERT(bytesWritten, clientBytesWritten,
                                           bytesWritten == clientBytesWritten);

            bsls::Types::Int64 temp, temp2, temp3;
            X.totalBytesRead(&temp);
            ASSERT(temp == bytesRead);
            X.totalBytesWritten(&temp);
            ASSERT(temp == bytesWritten);
            X.totalBytesRequestedToBeWritten(&temp);
            ASSERT(temp == bytesRequestedToBeWritten);
            ASSERT(0 == X.getChannelStatistics(&temp, &temp2, &temp3,
                                               channelId));
            ASSERT(temp  == bytesRead);
            ASSERT(temp2 == bytesRequestedToBeWritten);
            ASSERT(temp3 == bytesWritten);

            channel.invalidate();
            factory.deallocate(socket);
            channelBarrier.wait();

            // Second pass to test the total* methods and the methods
            // with a reset semantics.

            socket = factory.allocate();
            Channel  channel2(socket, &ta);

            connectRetCode =  socket->connect(PEER);
            ASSERT(0 == connectRetCode);
            ASSERT(0 == channel2.isInvalid());

            if (0 == connectRetCode) {
                channelBarrier.wait();
            }
            else {
                cout << "Failed to connect to channelpool.  Abort.\n";
                return;                                               // RETURN
            }

            ASSERT(0 == mX.setSocketOption(
                                       btlso::SocketOptUtil::k_SENDBUFFER,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       512, channelId));
            clientBytesWritten = 0;
            fail = 0;
            while(!fail) {
                ASSERT(sizeof(buffer) == channel2.write(0, buffer,
                                                       sizeof(buffer),0));
                clientBytesWritten += sizeof(buffer);
                if (veryVerbose) {
                    MTCOUT << "client wrote " << clientBytesWritten
                           << MTENDL;
                }
                #ifdef BSLS_PLATFORM_OS_AIX
                if (clientBytesWritten >= 1024) {
                    break; // AIX is brain-dead.
                }
                #endif
            }
             bslmt::ThreadUtil::microSleep(200 * 1000);
            ASSERT(1 == X.numChannels());

            bsls::Types::Int64 oldBytesWritten = bytesWritten;
            bsls::Types::Int64 oldBytesRequestedToBeWritten =
                                                    bytesRequestedToBeWritten;
            bsls::Types::Int64 oldBytesRead = bytesRead;
            clientBytesRead = 0;
            bytesWritten = 0;
            bytesRequestedToBeWritten = 0;
            bytesRead = 0;
            do {
                ASSERT(1 == X.numChannels());

                #ifndef BSLS_PLATFORM_OS_AIX
                int r =  channel2.read(buffer, sizeof(buffer), 0);
                #else
                // AIX is doing something very very weird.  The following
                // read() blocks even if the client's buffer is full or has
                // more data than we request.  Reading very small chunks works
                // but it is slow.

                int r;
                if ((clientBytesWritten - clientBytesRead) >= 50) {
                    r =  channel2.read(buffer, 50, 0);
                }
                else {
                    r =  channel2.read(buffer, 1, 0);
                }
                #endif
                ASSERT(r >= 0);
                clientBytesRead += r;
                ASSERT(0 == X.numBytesRead(&bytesRead, channelId));
                ASSERT(0 == X.numBytesWritten(&bytesWritten, channelId));
                ASSERT(0 == X.numBytesRequestedToBeWritten(
                                       &bytesRequestedToBeWritten, channelId));
                if (veryVerbose) {
                    MTCOUT << "client read "      << clientBytesRead    << ", "
                           << "client wrote "     << clientBytesWritten << ", "
                           << "server read "      << bytesRead          << ", "
                           << "server wrote "     << bytesWritten       << ", "
                           << "server requested " << bytesRequestedToBeWritten
                           << MTENDL;
                }
                // Test consistency.  We cannot use '==' since we're racing
                // against the channel thread.

                bsls::Types::Int64 temp;
                X.totalBytesRead(&temp);
                ASSERT(temp >= bytesRead);
                X.totalBytesWritten(&temp);
                ASSERT(temp >= bytesWritten);
                X.totalBytesRequestedToBeWritten(&temp);
                ASSERT(temp >= bytesRequestedToBeWritten);

                ASSERT(clientBytesRead <= clientBytesWritten);

                // We cannot assert anything between clientBytesRead and
                // bytesWritten because we do not know what is going to happen
                // first: the network transfer or the execution of the
                // callback.  While it is more likely that the callback is
                // going to be executed first, it is not always the case.

                ASSERT(bytesRequestedToBeWritten >= clientBytesRead);
                ASSERT(bytesRequestedToBeWritten >= bytesWritten);

            } while (clientBytesWritten != clientBytesRead);

            ASSERT(0 == X.numBytesWritten(&bytesWritten, channelId));
            ASSERT(0 == X.numBytesRequestedToBeWritten(
                                       &bytesRequestedToBeWritten, channelId));
            ASSERT(bytesWritten == bytesRequestedToBeWritten);
            ASSERT(bytesWritten == clientBytesWritten);

            X.totalBytesRead(&temp);
            ASSERT(temp ==  bytesRead + oldBytesRead);
            X.totalBytesWritten(&temp);
            LOOP3_ASSERT(temp, bytesWritten, oldBytesWritten,
                                       temp == bytesWritten + oldBytesWritten);
            X.totalBytesRequestedToBeWritten(&temp);
            ASSERT(temp == bytesRequestedToBeWritten +
                           oldBytesRequestedToBeWritten);

            // Reset functions
            bsls::Types::Int64 old;

            X.totalBytesRead(&old);
            mX.totalBytesReadReset(&temp);
            ASSERT(old == temp);
            X.totalBytesRead(&temp);
            ASSERT(0 == temp);

            X.totalBytesWritten(&old);
            mX.totalBytesWrittenReset(&temp);
            ASSERT(old == temp);
            X.totalBytesWritten(&temp);
            ASSERT(0 == temp);

            X.totalBytesRequestedToBeWritten(&old);
            mX.totalBytesRequestedToBeWrittenReset(&temp);
            ASSERT(old == temp);
            X.totalBytesRequestedToBeWritten(&temp);
            ASSERT(0 == temp);

            channel2.invalidate();
            factory.deallocate(socket);
            channelBarrier.wait();

            // Third pass to test for 'getStatisticsReset'

            socket = factory.allocate();
            Channel  channel3(socket, &ta);

            connectRetCode =  socket->connect(PEER);
            ASSERT(0 == connectRetCode);
            ASSERT(0 == channel3.isInvalid());

            if (0 == connectRetCode) {
                channelBarrier.wait();
            }
            else {
                cout << "Failed to connect to channelpool.  Abort.\n";
                return;                                               // RETURN
            }

            ASSERT(0 == mX.setSocketOption(
                                       btlso::SocketOptUtil::k_SENDBUFFER,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       512, channelId));
            clientBytesWritten = 0;
            fail = 0;
            while(!fail) {
                ASSERT(sizeof(buffer) == channel3.write(0, buffer,
                                                       sizeof(buffer),0));
                clientBytesWritten += sizeof(buffer);
                if (veryVerbose) {
                    MTCOUT << "client wrote " << clientBytesWritten
                           << MTENDL;
                }
                #ifdef BSLS_PLATFORM_OS_AIX
                if (clientBytesWritten >= 1024) {
                    break; // AIX is brain-dead.
                }
                #endif
            }
//             bslmt::ThreadUtil::microSleep(200 * 1000);
            ASSERT(1 == X.numChannels());

            clientBytesRead = 0;
            bytesWritten = 0;
            bytesRequestedToBeWritten = 0;
            bytesRead = 0;
            do {
                ASSERT(1 == X.numChannels());

                #ifndef BSLS_PLATFORM_OS_AIX
                int r =  channel3.read(buffer, sizeof(buffer), 0);
                #else
                // AIX is doing something very very weird.  The following
                // read() blocks even if the client's buffer is full or has
                // more data than we request.  Reading very small chunks works
                // but it is slow.

                int r;
                if ((clientBytesWritten - clientBytesRead) >= 50) {
                    r =  channel3.read(buffer, 50, 0);
                }
                else {
                    r =  channel3.read(buffer, 1, 0);
                }
                #endif
                ASSERT(r >= 0);
                clientBytesRead += r;
                ASSERT(0 == X.numBytesRead(&bytesRead, channelId));
                ASSERT(0 == X.numBytesWritten(&bytesWritten, channelId));
                ASSERT(0 == X.numBytesRequestedToBeWritten(
                                       &bytesRequestedToBeWritten, channelId));
                if (veryVerbose) {
                    MTCOUT << "client read "      << clientBytesRead    << ", "
                           << "client wrote "     << clientBytesWritten << ", "
                           << "server read "      << bytesRead          << ", "
                           << "server wrote "     << bytesWritten       << ", "
                           << "server requested " << bytesRequestedToBeWritten
                           << MTENDL;
                }
                // Test consistency.  We cannot use '==' since we're racing
                // against the channel thread.

                bsls::Types::Int64 temp;
                X.totalBytesRead(&temp);
                ASSERT(temp >= bytesRead);
                X.totalBytesWritten(&temp);
                ASSERT(temp >= bytesWritten);
                X.totalBytesRequestedToBeWritten(&temp);
                ASSERT(temp >= bytesRequestedToBeWritten);

                ASSERT(clientBytesRead <= clientBytesWritten);

                // We cannot assert anything between clientBytesRead and
                // bytesWritten because we do not know what is going to happen
                // first: the network transfer or the execution of the
                // callback.  While it is more likely that the callback is
                // going to be executed first, it is not always the case.

                ASSERT(bytesRequestedToBeWritten >= clientBytesRead);
                ASSERT(bytesRequestedToBeWritten >= bytesWritten);

            } while (clientBytesWritten != clientBytesRead);

            ASSERT(0 == X.numBytesWritten(&bytesWritten, channelId));
            ASSERT(0 == X.numBytesRequestedToBeWritten(
                                       &bytesRequestedToBeWritten, channelId));
            ASSERT(bytesWritten == bytesRequestedToBeWritten);
            ASSERT(bytesWritten == clientBytesWritten);

            X.totalBytesRead(&temp);
            ASSERT(temp ==  bytesRead);
            X.totalBytesWritten(&temp);
            ASSERT(temp == bytesWritten);
            X.totalBytesRequestedToBeWritten(&temp);
            ASSERT(temp == bytesRequestedToBeWritten);

            ASSERT(0 == X.getChannelStatistics(&temp, &temp2, &temp3,
                                               channelId));
            ASSERT(temp  == bytesRead);
            ASSERT(temp2 == bytesRequestedToBeWritten);
            ASSERT(temp3 == bytesWritten);

            channel3.invalidate();
            factory.deallocate(socket);
            channelBarrier.wait();
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }
#endif
}

void TestDriver::testCase12()
{
        // --------------------------------------------------------------------
        // TESTING 'reportWeightedAverageReset()'
        //
        // Concerns:
        //   That reportWeightedAverageReset() works as expected:
        //   o Must report an accurate evaluation of the weighted average.
        //   o The computation of d_sum when a channel goes down is
        //     correct.
        //   o Must reset the information properly.
        //
        // Plan:
        //   Instantiate a btlmt::ChannelPool configured to accept NUM_THREADS
        //   connection using NUM_THREADS threads.  Then create NUM_THREADS
        //   clients and connect them to the pool.  After each successful
        //   connection, call reportWeightedAverageReset() and check the value
        //   to address concerns 1 and 3.  A certain tolerance from the
        //   expected value will be accepted since it is impossible to control
        //   accurately the timing.
        //
        //   After resetting the timer, for each client sleep N seconds and
        //   then disconnect the client.  N should be big enough to "dilute"
        //   the timing inaccuracies into the tolerance value.  At the end
        //   of the process, call reportWeightedAverageReset() and check the
        //   return value to address concern 2 and implicitly concern 3.
        //
        // Testing:
        //   double btlmt::ChannelPool::reportWeightedAverageReset();
        // --------------------------------------------------------------------

        if (verbose)
             cout << "\nTesting reportWeightedAverageReset()"
                  << "\n====================================" << endl;

        using namespace TEST_CASE_REPORT_AVG_RESET;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_THREADS = 8,
                SERVER_ID   = 1013410001,
                BACKLOG     = 1
            };
            const double HARD_TOLERANCE = 0.60;  // 60% tolerance to assert
            const double TOLERANCE      = 0.30;  // 30% tolerance to warn

            btlmt::ChannelPool::BlobBasedReadCallback   dataCb;
            btlmt::ChannelPool::PoolStateChangeCallback poolCb;

            makeNull(&dataCb);
            makeNull(&poolCb);

            bslmt::Barrier channelBarrier(2);

            btlmt::ChannelPool *poolAddr;
            int                 poolEvent = -1;
            int                *eventAddr = &poolEvent;

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseAvgResetChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &eventAddr
                                        , &channelBarrier));

            btlmt::ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_THREADS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(100.0);

            Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;
            poolAddr = &mX;

            const btlso::IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == mX.start());
            ASSERT(0 == mX.listen(ADDRESS, BACKLOG, SERVER_ID));
            ASSERT(0 == X.numChannels());
            btlso::IPv4Address PEER = getServerLocalAddress(&mX, SERVER_ID);

            typedef btlso::InetStreamSocketFactory<btlso::IPv4Address> Factory;
            typedef btlso::StreamSocket<btlso::IPv4Address>            Socket;
            typedef btlsos::TcpChannel                                 Channel;

            Factory              factory(&ta);
            bsl::vector<Socket*> sockets(&ta);
            sockets.reserve(MAX_THREADS);

            ASSERT(0 == mX.reportWeightedAverageReset());
            for (int i = 0; i < MAX_THREADS; ++i) {
                Socket  *socket = factory.allocate();
                Channel  channel(socket, &ta);

                bslmt::ThreadUtil::yield();
                ASSERT(0 == socket->connect(PEER));
                ASSERT(0 == channel.isInvalid());

                channelBarrier.wait();
                double exp = mX.reportWeightedAverageReset();
                if (i && exp != -1) {
                    // If i == 0, it is too inaccurate; if exp == -1, then too
                    // little time elapsed between the resets.

                    double exp_really_lo = exp * (1 - HARD_TOLERANCE);
                    double exp_really_hi = exp * (1 + HARD_TOLERANCE);
                    double exp_lo = exp * (1 - TOLERANCE);
                    double exp_hi = exp * (1 + TOLERANCE);
                    if (!(exp_lo <i && i < exp_hi)) {
                        cout << "*** Warning: " << ARGV[0] << ":" << L_
                             << ":\n*** Warning: anomalous timing results ***"
                             << endl;
                    }
                    LOOP2_ASSERT(i, exp, i > exp_really_lo &&
                                          i < exp_really_hi);
                }
                else if (exp == -1) {
                    cout << "*** Warning: " << ARGV[0] << ":" << L_
                         << ":\n*** Warning: less than 1ms between resets ***"
                         << endl;
                }

                sockets.push_back(socket);
                if (!i) {
                    // Give some time to the pool to setup its stuff.

                    bslmt::ThreadUtil::microSleep(1000);
                }

            }
#ifndef BSLS_PLATFORM_OS_WINDOWS
            ASSERT(MAX_THREADS == mX.reportWeightedAverageReset());
#endif
            // If we get rescheduled, we will have inaccurate results.

            bslmt::ThreadUtil::yield();

            for (bsl::vector<Socket*>::iterator it = sockets.begin();
                    it != sockets.end(); ++it)
            {
                Socket  *socket = *it;
                Channel  channel(socket);

//                 bslmt::ThreadUtil::microSleep(2000 * 1000);
                channel.invalidate();
                factory.deallocate(socket);
                channelBarrier.wait();
            }
            double rr = mX.reportWeightedAverageReset();

            // Let N the number of seconds we slept, the expected value
            // is
            //..
            //  (N + 2N + ... + MAX_THREADS * N) / (MAX_THREADS * N)
            // = sum(k, 1, MAX_THREADS) / MAX_THREADS
            // = (MAX_THREADS + 1) / 2
            //..

            double exp    = (MAX_THREADS + 1.0) / 2.0;
            double exp_really_lo = exp * (1 - HARD_TOLERANCE);
            double exp_really_hi = exp * (1 + HARD_TOLERANCE);
            double exp_lo = exp * (1 - TOLERANCE);
            double exp_hi = exp * (1 + TOLERANCE);
            if (!(exp_lo < rr && rr < exp_hi)) {
                cout << "*** Warning: " << ARGV[0] << ":" << L_ << ":\n"
                     << "*** Warning: anomalous timing results ***" << endl;
            }
#if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_WINDOWS)
            LOOP2_ASSERT(rr, exp, exp_really_lo < rr && rr < exp_really_hi);
#endif
            if (verbose) { P_(exp_lo); P_(exp); P_(exp_hi); P(rr); }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase11()
{
        // --------------------------------------------------------------------
        // TESTING FLOW CONTROL
        //
        // Concerns:
        //   o WRITE_CACHE_HIWAT and WRITE_CACHE_LOWWAT are generated
        //     appropriately
        //
        // Plan:
        //   Create an instance under test.  Create a local socket pair and
        //   import it into the channel pool.  Block on a barrier waiting
        //   until another thread unblocks the main thread.  Then
        //   read the number of written bytes until EWOULDBLOCK is returned,
        //   then close the channels.  Verify that the water mark messages
        //   are generated.
        //
        // Testing:
        //   FLOW CONTROL
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING FLOW CONTROL."
                          << "=====================" << endl;

        using namespace TEST_CASE_FLOW_CONTROL;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            bsl::vector<my_ChannelEvent> channelEvents;
            bslmt::Mutex                 channelEventsMutex;

            enum {
                NUM_SOCKETS   = 1,
                SEND_SIZE     = 8192,
                LOW_WATERMARK = 1024,
                HI_WATERMARK  = 1 << 20 // 1 MB of cache
            };

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&ta);

            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setReadTimeout(100000);
            config.setWriteCacheWatermarks(LOW_WATERMARK,
                                           HI_WATERMARK);
            if (verbose) { P(config); }

            btlmt::ChannelPool::BlobBasedReadCallback    dataCb;
            btlmt::ChannelPool::PoolStateChangeCallback  poolCb;

            makeNull(&dataCb);
            makeNull(&poolCb);

            bslmt::Barrier barrier(2);
            int            numBytesWritten = 0;

            // Do not overflow the stack.

            btlb::PooledBlobBufferFactory outFactory(HI_WATERMARK/64, &ta);

            btlmt::ChannelPool *mX_p;

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseFlowControlChannelCallback
                                        , _1, _2, _3, _4
                                        , &barrier
                                        , &numBytesWritten
                                        , config
                                        , &outFactory
                                        , &mX_p));

            btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
            mX_p = &mX;

            ASSERT(0 == mX.start());

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                    handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            ASSERT(0 == ret);

            // The following socket options are set only if necessary.
            ret = btlso::SocketOptUtil::setOption(handles[1],
                                    btlso::SocketOptUtil::k_TCPLEVEL,
                                    btlso::SocketOptUtil::k_TCPNODELAY, 1);
            ASSERT(0 == ret);

            btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                                                  factory.allocate(handles[0]);
            ASSERT(serverSocket);
            btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                                                  factory.allocate(handles[1]);
            ASSERT(clientSocket);

            typedef btlso::StreamSocketFactoryDeleter Deleter;

            bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
               managedServerSocket(serverSocket,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

            ASSERT(0 == mX.import(&managedServerSocket, 0));

            barrier.wait();
            if (verbose) {
                Q("Barrier Passed");
                P(numBytesWritten);
            }

            int numBytesRead = 0;
            char buffer[SEND_SIZE];

            int rc = 0;
            while (numBytesRead < numBytesWritten &&
                   (rc = clientSocket->read(buffer, SEND_SIZE)) > 0)
            {
                numBytesRead += rc;
                if (veryVerbose) { PT(numBytesRead); }
            }
            if (rc < 0) { PT(rc); }

            LOOP2_ASSERT(numBytesRead, numBytesWritten,
                         numBytesRead == numBytesWritten);

            if (verbose) cout << "\tShutting down\n";

            mX.stop();

            factory.deallocate(clientSocket);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase10()
{
        // --------------------------------------------------------------------
        // TESTING 'disableRead' and 'enableRead' METHODS
        //
        // Concerns:
        //   o disabling a channel generates AUTO_READ_DISABLED message
        //   o disabled channel can be enabled (and AUTO_READ_ENABLED message
        //   o is generated)
        //   o enabled channel can be disabled (and AUTO_READ_DISABLED message
        //     is generated)
        //   o by default, a channel is enabled (and AUTO_READ_ENABLED message
        //     always follows CHANNEL_UP message)
        //
        // Plan:
        //   Create an instance under test.  Create a number of local socket
        //   pairs.  Import them into the channel pool and verify that
        //   both CHANNEL_UP and AUTO_READ_ENABLED is generated (in this order)
        //   for each imported channel.  Disable every channel.
        //   Verify emitted messages.  Enable back, verify messages.
        //
        // Testing:
        //   int btlmt::ChannelPool::disableRead(int);
        //   int btlmt::ChannelPool::enableRead(int);
        // --------------------------------------------------------------------

        if (verbose)
           cout << "\nTESTING 'disableRead' and 'enableRead' METHODS."
                << "\n===============================================" << endl;

        using namespace TEST_CASE_ENABLE_DISABLE_CHANNELS;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&ta);
            runTestCaseEnableDisable(&factory, ta);  // ta passed by reference
        }

        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase9()
{
        // --------------------------------------------------------------------
        // TESTING 'registerClock' AND 'deregisterClock' METHODS
        //
        // Concerns:
        //   The following conditions must be verified on both enabled and
        //   disabled channel pool:
        //   1.  A clock can be registered
        //   2.  A clock can be registered to execute within the manager of a
        //       specified channel.
        //   3.  A registered timer will execute only once.
        //   4.  A registered clock can be deregistered without invocation.
        //   5.  A registered clock is invoked when expected.
        //   6.  A registered clock can be deregistered within the callback.
        //   7.  If a clock is deregistered, a new clock can be registered with
        //       the same id.
        //   8.  Registration with duplicate ID must fail with appropriate
        //       status.
        //   9.  Registration with wrong channel ID must fail with appropriate
        //       status.
        //   10. Verifying that stopping channel pool will NOT invoke
        //       a registered clock, and that a clock can be deregistered.
        //
        // Plan:
        //  For concerns 1 and 4, with a running channel pool, register a
        //  number of timers and then deregister them.  For concern 8, attempt
        //  to re-register the clocks twice in a row, and witness the failure.
        //
        //  For concerns 3 and 5, with a running channel pool, register a
        //  pre-determined number of clocks all starting at the same (future)
        //  time.  Create a different callback for each timer and give it the
        //  address of the state structure for this timer.  Then let each
        //  callback update the state structure to count the number of
        //  invocations, wait long enough, and make sure that the counters are
        //  as expected.
        //
        //  For concerns 6 and 7, repeat previous test but with the clocks
        //  ticking a pre-determined number of times, and let the callback
        //  deregister its clock after that number of invocations.  Wait long
        //  enough, then verify the clocks did not execute more than the
        //  expected number of times.  Finally, re-register the clocks with the
        //  same clock ID and check success.  This both addresses concern 7 and
        //  makes sure that the clocks were indeed deregistered.
        //
        //  For concerns 10, the testing is incomplete and should
        //  verify that callbacks haven't been involved.
        //
        // Testing:
        //   int btlmt::ChannelPool::registerClock(cmd, start, period, int);
        //   void btlmt::ChannelPool::deregisterClock(int);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING 'registerClock' AND 'deregisterClock'\n";

#ifndef BSLS_PLATFORM_OS_WINDOWS
        using namespace TEST_CASE_CLOCK_FUNCTIONS;
        if (verbose) cout << "\nWhen channel pool is running.\n";
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(10);

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb;
            btlmt::ChannelPool::PoolStateChangeCallback    poolCb;
            btlmt::ChannelPool::BlobBasedReadCallback      dataCb;

            makeNull(&channelCb);
            makeNull(&poolCb);
            makeNull(&dataCb);

            if (verbose)
                cout << "\tRegistering clocks, then deregistering.\n";

            bslma::TestAllocator ta(veryVeryVerbose);
            {
                btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                ASSERT(0 == mX.start());
                bsls::TimeInterval startTime = bdlt::CurrentTime::now() +
                                                                         120.0;

                enum {
                    NUM_CLOCKS = 20
                };

                if (veryVerbose)
                    cout << "\t\tWith up to " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    for (int j = 0; j <= i; ++j) {
                        LOOP2_ASSERT(i, j,
                            0 == mX.registerClock(NULL_CB, startTime,
                                                  bsls::TimeInterval(1.0),
                                                  j));
                        LOOP2_ASSERT(i, j,
                            1 == mX.registerClock(NULL_CB, startTime,
                                                  bsls::TimeInterval(1.0),
                                                  j));
                    }
                    for (int j = 0; j <= i; ++j) {
                        mX.deregisterClock(j);
                    }
                }

                if (veryVerbose) cout << "\t\tStopping the pool\n";
                ASSERT(0 == mX.stop());
            }
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBytesInUse());
            if (veryVerbose) { P(ta); }

            if (verbose) cout << "\tVerifying that a clock can be invoked.\n";
            {
                btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                ASSERT(0 == mX.start());
                bsls::TimeInterval startTime = bdlt::CurrentTime::now();
                bsls::TimeInterval noPeriod(0.0);  // 0s, for timers
                bsls::TimeInterval period(0.1);    // .1s, for clocks
                const int mT = 10 * 1000;         // 10ms

                enum {
                    NUM_CLOCKS      = 5,
                    NUM_INVOCATIONS = 5,
                    LAST_CLOCK_IDX  = 2 * ((NUM_CLOCKS - 1)/ 2)
                        // clocks have even idx, timers have odd idx
                };

                my_ClockState clockState[NUM_CLOCKS];

                if (veryVerbose)
                    cout << "\t\tRegistering " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    clockState[i].d_startTime = startTime + period;
                    clockState[i].d_timeout = i % 2 ? noPeriod : period;
                    clockState[i].d_numInvocations = 0;
                    clockState[i].d_maxNumInvocations = 0;  // do not check
                    bsl::function<void()> functor(
                            bdlf::BindUtil::bind( &caseMyClockCallback
                                                , &clockState[i]
                                                , i
                                                , &mX
                                                , 0
                                                , (int)DONT_CHECK_THREAD_ID));

                    LOOP_ASSERT(i, 0 == mX.registerClock(functor,
                                                  clockState[i].d_startTime,
                                                  clockState[i].d_timeout, i));
                }
                if (veryVerbose)
                    cout << "\t\tWaiting for each clock to be invoked "
                         << NUM_INVOCATIONS << " times.\n";

                bslmt::ThreadUtil::sleep(bsls::TimeInterval(
                                                 NUM_INVOCATIONS * 0.1) + 0.5);
                int iter = 10;
                while (--iter && !(NUM_INVOCATIONS <=
                                clockState[LAST_CLOCK_IDX].d_numInvocations)) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  delays in case" << endl;
                    bslmt::ThreadUtil::microSleep(mT);
                    bslmt::ThreadUtil::yield();
                }

                if (veryVerbose)
                   cout << "\t\tDeRegistering " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    if (i % 2) {
                        LOOP2_ASSERT(i, clockState[i].d_numInvocations,
                             1 == clockState[i].d_numInvocations);
                    }
                    else {
                        LOOP2_ASSERT(i, clockState[i].d_numInvocations,
                            NUM_INVOCATIONS <= clockState[i].d_numInvocations);
                        mX.deregisterClock(i);
                    }
                }
                if (veryVerbose)
                    cout << "\t\tStopping the pool.\n";
                ASSERT(0 == mX.stop());
            }
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBytesInUse());
            if (veryVerbose) { P(ta); }

            if (verbose)
                cout << "\tVerifying that a clock can be executed "
                     << "in a specified thread.\n";
            {
                enum {
                    NUM_CONNECT_ATTEMPTS = 10,  // for client channel
                    NUM_CLOCKS           = 5,
                    NUM_INVOCATIONS      = 5,
                    SETUP_TIMEOUT        = 20,   // 20s
                    SERVER_ID            = 0x701d50da
                };

                bsls::TimeInterval period(0.01);   // 0.01s, for clocks
                const int TIMEOUT = 1;             // 1s
                const int mT = 10 * 1000;          // 10ms

                my_ClockState clockState[NUM_CLOCKS];

                // One server -- system specified port number.

                if (veryVerbose)
                    cout << "\t\tRegistering server channel.\n";

                btlmt::ChannelPool serverPool(channelCb, dataCb, poolCb,
                                             config, &ta);
                ASSERT(0 == serverPool.start());
                ASSERT(0 == serverPool.listen(0, 1, SERVER_ID));

                btlso::IPv4Address address = getServerLocalAddress(
                                                      &serverPool, SERVER_ID);
                ASSERT(0 != address.portNumber());
                if (veryVerbose) { P(address); }

                // Connect to that server.

                if (veryVerbose)
                    cout << "\t\tRegistering connected channels.\n";

                int channelId1 = -1, channelId2 = -1;
                ThreadId threadId1 = NULL_THREAD_ID;
                ThreadId threadId2 = NULL_THREAD_ID;

                btlmt::ChannelPool::ChannelStateChangeCallback channel2Cb(
                        bdlf::BindUtil::bind( &caseChannelStateCallback
                                            , _1, _2, _3, _4
                                            , &channelId1
                                            , &threadId1
                                            , &channelId2
                                            , &threadId2
                                            , veryVerbose));

                btlmt::ChannelPool mX(channel2Cb, dataCb, poolCb, config, &ta);
                ASSERT(0 == mX.start());

                ASSERT(0 == mX.connect(address,
                                       NUM_CONNECT_ATTEMPTS,
                                       bsls::TimeInterval(TIMEOUT),
                                       1));

                // Wait until first channel has been created.

                for (int i = 0; -1 == channelId1 && i < (int) SETUP_TIMEOUT;
                                                                         ++i) {
                    bslmt::ThreadUtil::sleep(bsls::TimeInterval(0.1));
                    bslmt::ThreadUtil::yield();
                }

                if (-1 == channelId1) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  delays in case "
                         << "may prevent running test." << endl;
                }

                ASSERT(0 == mX.connect(address,
                                       NUM_CONNECT_ATTEMPTS,
                                       bsls::TimeInterval(TIMEOUT),
                                       2));

                // Wait until both channels have been created.

                for (int i = 0; (-1 == channelId1 || -1 == channelId2) &&
                                                i < (int) SETUP_TIMEOUT; ++i) {
                    bslmt::ThreadUtil::sleep(bsls::TimeInterval(0.1));
                    bslmt::ThreadUtil::yield();
                }

                if (-1 == channelId2 || -1 == channelId2) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  delays in case "
                         << "prevent running test." << endl;
                }

                // We have two channels hopefully running in different
                // managers.

                if (threadId1 == threadId2) {
                    cout << __FILE__ << ": " << __LINE__
                         << " test won't be very useful." << endl;
                }

                ASSERT(NULL_THREAD_ID != threadId1 &&
                                                  NULL_THREAD_ID != threadId2);
                if (veryVerbose) {
                    T_(); T_(); P_(channelId1); P(threadId1);
                    T_(); T_(); P_(channelId2); P(threadId2);
                }

                // Start test proper.

                if (veryVerbose)
                    cout << "\t\tRegistering " << NUM_CLOCKS << " clocks.\n";

                bsls::TimeInterval startTime = bdlt::CurrentTime::now();

                for (int i = 0; i < (int) NUM_CLOCKS; ++i) {
                    clockState[i].d_startTime = startTime + period;
                    clockState[i].d_timeout   = period;
                    clockState[i].d_numInvocations = 0;
                    clockState[i].d_maxNumInvocations = 0;  // do not check
                    bsl::function<void()> functor(
                            bdlf::BindUtil::bind( &caseMyClockCallback
                                                , &clockState[i]
                                                , i
                                                , &mX
                                                , 0
                                                , i % 2 ? threadId1
                                                        : threadId2));
                    const int rc = mX.registerClock(functor,
                                                    clockState[i].d_startTime,
                                                    clockState[i].d_timeout, i,
                                                    i % 2
                                                    ? channelId1
                                                    : channelId2);
                    LOOP_ASSERT(i, 0 == rc);
                }
                if (veryVerbose)
                    cout << "\t\tWaiting for each clock to be invoked "
                         << NUM_INVOCATIONS << " times.\n";

                bslmt::ThreadUtil::sleep(bsls::TimeInterval(
                                         NUM_INVOCATIONS * 0.1) + 0.5);
                int iter = 10;
                while (--iter && !((int) NUM_INVOCATIONS <=
                                clockState[NUM_CLOCKS - 1].d_numInvocations)) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  delays in case" << endl;
                    bslmt::ThreadUtil::microSleep(mT);
                    bslmt::ThreadUtil::yield();
                }

                if (veryVerbose)
                   cout << "\t\tDeRegistering " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < (int) NUM_CLOCKS; ++i) {
                    LOOP3_ASSERT(i, clockState[i].d_numInvocations,
                                 NUM_INVOCATIONS,
                      (int) NUM_INVOCATIONS <= clockState[i].d_numInvocations);
                    mX.deregisterClock(i);
                }
                if (veryVerbose)
                    cout << "\t\tStopping the pools.\n";
                ASSERT(0 == serverPool.stop());
                ASSERT(0 == mX.stop());
            }
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBytesInUse());
            if (veryVerbose) { P(ta); }

            if (verbose)
                cout << "\tVerify clock can be deregistered in a callback.\n";
            {
                btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                ASSERT(0 == mX.start());
                bsls::TimeInterval startTime = bdlt::CurrentTime::now();
                bsls::TimeInterval period(0.1);  // 1s
                const int mT = 10 * 1000;       // 10ms
                enum {
                    NUM_CLOCKS = 5,
                    NUM_INVOCATIONS = 5
                };

                my_ClockState clockState[NUM_CLOCKS];

                if (veryVerbose)
                   cout << "\t\tRegistering " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    clockState[i].d_startTime = startTime + period;
                    clockState[i].d_timeout = period;
                    clockState[i].d_numInvocations = 0;
                    clockState[i].d_maxNumInvocations = NUM_INVOCATIONS;
                    bsl::function<void()> functor(
                            bdlf::BindUtil::bind( &caseMyClockCallback
                                                , &clockState[i]
                                                , i
                                                , &mX
                                                , 1
                                                , (int)DONT_CHECK_THREAD_ID));

                    LOOP_ASSERT(i, 0 == mX.registerClock(functor,
                       clockState[i].d_startTime, clockState[i].d_timeout, i));
                }
                if (veryVerbose)
                    cout << "\t\tWaiting for each clock to be invoked "
                         << NUM_INVOCATIONS << " times.\n";

                bslmt::ThreadUtil::sleep(bsls::TimeInterval(
                                                 NUM_INVOCATIONS * 0.1) + 0.5);
                int iter = 10;
                while (--iter && !(NUM_INVOCATIONS <=
                                clockState[NUM_CLOCKS - 1].d_numInvocations)) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  delays in case" << endl;
                    bslmt::ThreadUtil::microSleep(mT);
                    bslmt::ThreadUtil::yield();
                }

                if (veryVerbose)
                    cout << "\t\tReRegistering " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    LOOP2_ASSERT(i, clockState[i].d_numInvocations,
                            NUM_INVOCATIONS == clockState[i].d_numInvocations);
                    LOOP_ASSERT(i, 0 == mX.registerClock(NULL_CB,
                          bdlt::CurrentTime::now() + 1.0,
                          bsls::TimeInterval(1.0), i));
                    mX.deregisterClock(i);
                }

                if (veryVerbose) cout << "\t\tStopping the pool\n";
                ASSERT(0 == mX.stop());
            }
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBytesInUse());
            if (veryVerbose) { P(ta); }
        }

        if (verbose) cout << "\nWhen channel pool is NOT running.\n";
        {
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(0);

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb;
            btlmt::ChannelPool::PoolStateChangeCallback    poolCb;
            btlmt::ChannelPool::BlobBasedReadCallback      dataCb;

            makeNull(&channelCb);
            makeNull(&poolCb);
            makeNull(&dataCb);

            if (verbose) cout << "\tRegistering clocks, then deregistering\n";

            bslma::TestAllocator ta(veryVeryVerbose);
            {
                btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                bsls::TimeInterval startTime = bdlt::CurrentTime::now() +
                                                                         120.0;
                ASSERT(0 == mX.stop());

                enum {
                    NUM_CLOCKS = 5
                };

                if (veryVerbose)
                   cout << "\t\tWith up to " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    for (int j = 0; j <= i; ++j) {
                        LOOP2_ASSERT(i, j,
                                0 == mX.registerClock(NULL_CB, startTime,
                                    bsls::TimeInterval(1.0),
                                    j));
                        LOOP2_ASSERT(i, j,
                                1 == mX.registerClock(NULL_CB, startTime,
                                    bsls::TimeInterval(1.0),
                                    j));
                    }
                    for (int j = 0; j <= i; ++j) {
                        mX.deregisterClock(j);
                    }

                }
            }
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBytesInUse());
            if (veryVerbose) { P(ta); }

            if (verbose)
                cout << "\tVerifying behavior of clocks when channel"
                        "pool is stopped.\n";
            {
                btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                bsls::TimeInterval startTime = bdlt::CurrentTime::now() + 10.0;

                enum {
                    NUM_CLOCKS = 2
                };

                if (veryVerbose)
                   cout << "\t\tWith up to " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    ASSERT(0 == mX.start());

                    for (int j = 0; j <= i; ++j) {
                        LOOP2_ASSERT(i, j, 0 == mX.registerClock(
                                                       NULL_CB,
                                                       startTime,
                                                       bsls::TimeInterval(1.0),
                                                       j));
                        LOOP2_ASSERT(i, j, 1 == mX.registerClock(
                                                       NULL_CB,
                                                       startTime,
                                                       bsls::TimeInterval(1.0),
                                                       j));
                    }

                    if (veryVerbose)
                        cout << "\t\tStopping channel pool" << endl;

                    ASSERT(0 == mX.stop());

                    if (veryVerbose)
                        cout << "\t\tSleeping..." << endl;

                    if (veryVerbose)
                        cout << "\t\tDeregistering clocks" << endl;

                    // The clock callback shouldn't be invoked.

                    for (int j = 0; j <= i; ++j) {
                        mX.deregisterClock(j);
                    }
                }
            }
            ASSERT(0 == ta.numMismatches());
            ASSERT(0 == ta.numBytesInUse());
            if (veryVerbose) { P(ta); }
        }
#endif
}

void TestDriver::testCase8()
{
        // --------------------------------------------------------------------
        // Testing 'writeMessage'
        //
        // Concerns:
        //   - That we can handle calls from different threads
        //   - That the function properly enqueues and writes to the channel
        //   - That both versions of write() can coexist properly.
        //
        // Plan.
        //   First we instantiate a channel pool with one maximum
        //   connection and thread.  We will connect a client.  Then,
        //   WRITING_THREADS threads will be created and synchronized to write
        //   on a channel with a BIG_BUFFER_ALLOC that it is big enough to
        //   ensure we have to enqueue all the requests.  Once they all
        //   returned, we will read from the client and ensure that we are
        //   able to read the correct amount of bytes and that the ordering
        //   of the packets is correct.  We will try again with a smaller
        //   number of bytes which will test the case where the call to
        //   'writev()' can atomically succeed.  Then we will exercise
        //   both flavors of the write() function by multiple threads at
        //   the same time to ensure it is properly handled and ordered by the
        //   pool, following the same methodology of big buffers followed by
        //   small buffers.  We attempt to force all the depths of callbacks in
        //   the channel pool by exceeding twice the BTEMT_MAX_IOVEC_SIZE
        //   (usually 16), which forces first the write(Vec)Message to schedule
        //   a callback, and that callback to schedule another one inside.
        //
        // Testing:
        //   int btlmt::ChannelPool::write(btlb::Blob, ...);
        //   int btlmt::ChannelPool::write(btes::Iovecs, ...);
        //   int btlmt::ChannelPool::write(btes::Ovecs, ...);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'write'"
                          << "\n===============" << endl;

        using namespace TEST_CASE_WRITE;
        bslma::TestAllocator ta(veryVeryVerbose);

        {
            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            runTestCase(ARGV[0], &factory, ta);
                // Note: ta passed by reference
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase7()
{
        // --------------------------------------------------------------------
        // TESTING 'shutdown'
        //
        // Concerns:
        // * That a channel is shut down when its peer socket is closed.
        // * That a partial shutdown is a full shutdown when channel does not
        //   support half-open connections.
        // * That a channel created in half-open mode can be partially shut
        //   down and still continue to function in the other half.
        // * That a CHANNEL_DOWN_READ or CHANNEL_DOWN_WRITE callback is not
        //   invoked once that half of the channel is closed.
        // * That a channel partially shut down is closed when the second half
        //   is closed.
        //
        // Plan:
        //
        // Testing:
        //   int btlmt::ChannelPool::shutdown();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'shutdown'"
                          << "\n==================" << endl;

        using namespace TEST_CASE_SHUTDOWN;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 1,
                MAX_THREADS  = 1,

                USER_ID      = 0x12345678,
                BACKLOG      = 1,
                SERVER_ID    = 0x701d50da,

                TIMEOUT      = 1   // 1s
            };

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            btlmt::ChannelPool::ChannelStateChangeCallback     serverChannelCb;
            btlmt::ChannelPool::PoolStateChangeCallback        poolCb;
            btlmt::ChannelPool::BlobBasedReadCallback          dataCb;

            bslmt::Barrier barrier(2);

            caseCallbackInfo info;
            info.d_barrier_p = &barrier;
            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;

            makeNull(&poolCb);
            makeNull(&serverChannelCb);
            makeNull(&dataCb);

            btlmt::ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setReadTimeout(1);    // in seconds
            cpc.setIncomingMessageSizes(1, 32, 256);

            btlmt::ChannelPool serverPool(serverChannelCb, dataCb, poolCb,
                                         cpc, &ta);
            ASSERT(0 == serverPool.start());
            ASSERT(0 == serverPool.listen(0, BACKLOG, SERVER_ID));
            btlso::IPv4Address peer = getServerLocalAddress(&serverPool, SERVER_ID);
            if (verbose) {
                T_(); P(peer);
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &info));

            btlmt::ChannelPool mX(channelCb, dataCb, poolCb, cpc, &ta);
            ASSERT(0 == mX.start());

            if (verbose)
                cout << "\tSimple manual shutdown." << endl;
            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.connect(peer, 1,
                                    bsls::TimeInterval(TIMEOUT),
                                    USER_ID,
                                    false));
                barrier.wait(); // for CHANNEL_UP
                ASSERT(-1 != info.d_channelId);

                ASSERT(0 == mX.shutdown(info.d_channelId));
                barrier.wait(); // for CHANNEL_DOWN

                ASSERT(0 == info.d_channelDownReadFlag);
                ASSERT(0 == info.d_channelDownWriteFlag);

                ASSERT(0 != mX.shutdown(info.d_channelId));
            }

            if (verbose)
                cout << "\tPartial shutdown is same as full shutdown." << endl;
            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.connect(peer, 1,
                                    bsls::TimeInterval(TIMEOUT),
                                    USER_ID,
                                    false));
                barrier.wait(); // for CHANNEL_UP
                ASSERT(-1 != info.d_channelId);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        btlso::Flag::e_SHUTDOWN_RECEIVE));
                barrier.wait(); // for CHANNEL_DOWN

                MTASSERT(0 == info.d_channelDownReadFlag);
                MTASSERT(0 == info.d_channelDownWriteFlag);
            }

            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.connect(peer, 1,
                                    bsls::TimeInterval(TIMEOUT),
                                    USER_ID,
                                    false));
                barrier.wait(); // for CHANNEL_UP
                ASSERT(-1 != info.d_channelId);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        btlso::Flag::e_SHUTDOWN_SEND));
                barrier.wait(); // for CHANNEL_DOWN

                MTASSERT(0 == info.d_channelDownReadFlag);
                MTASSERT(0 == info.d_channelDownWriteFlag);
            }

            if (verbose)
                cout << "\tHalf-open shutdown (close read)." << endl;
            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.connect(peer, 1,
                                    bsls::TimeInterval(TIMEOUT),
                                    USER_ID,
                                    false,
                                    btlmt::ChannelPool::e_KEEP_HALF_OPEN));
                barrier.wait(); // for CHANNEL_UP
                MTASSERT(-1 != info.d_channelId);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        btlso::Flag::e_SHUTDOWN_RECEIVE));
                barrier.wait(); // for CHANNEL_DOWN_READ
                MTASSERT(1 == info.d_channelDownReadFlag);

                ASSERT(0 != mX.shutdown(info.d_channelId,
                                        btlso::Flag::e_SHUTDOWN_RECEIVE));
                MTASSERT(1 == info.d_channelDownReadFlag);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        btlso::Flag::e_SHUTDOWN_SEND));
                barrier.wait(); // for CHANNEL_DOWN_WRITE
                MTASSERT(1 == info.d_channelDownWriteFlag);

                // The SHUTDOWN_SEND will also close the channel.
                barrier.wait(); // for CHANNEL_DOWN
                MTASSERT(1 == info.d_channelDownReadFlag);
                MTASSERT(1 == info.d_channelDownWriteFlag);
            }

            if (verbose)
                cout << "\tHalf-open shutdown (close write)." << endl;
            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.connect(peer, 1,
                                    bsls::TimeInterval(TIMEOUT),
                                    USER_ID,
                                    false,
                                    btlmt::ChannelPool::e_KEEP_HALF_OPEN));
                barrier.wait(); // for CHANNEL_UP
                MTASSERT(-1 != info.d_channelId);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        btlso::Flag::e_SHUTDOWN_SEND));
                barrier.wait(); // for CHANNEL_DOWN_WRITE
                MTASSERT(1 == info.d_channelDownWriteFlag);

                ASSERT(0 != mX.shutdown(info.d_channelId,
                                        btlso::Flag::e_SHUTDOWN_SEND));
                MTASSERT(1 == info.d_channelDownWriteFlag);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        btlso::Flag::e_SHUTDOWN_RECEIVE));
                barrier.wait(); // for CHANNEL_DOWN_READ
                MTASSERT(1 == info.d_channelDownReadFlag);

                // The SHUTDOWN_RECEIVE will also close the channel.
                barrier.wait(); // for CHANNEL_DOWN
                MTASSERT(1 == info.d_channelDownReadFlag);
                MTASSERT(1 == info.d_channelDownWriteFlag);
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase6()
{
        // --------------------------------------------------------------------
        // TESTING CONCERN: IMPORT HALF-CLOSED SOCKET PAIR
        //
        // Concerns:
        //   That import a half-closed socket pair can be imported and that the
        //   channel is properly recognized as half-closed.
        //
        // Plan:
        //   Create a 'btlmt::ChannelPool' object, 'mX', and open a channel in
        //   listen mode.  Create a socket pair, and close one half.  Import
        //   the other half into 'mX'.  Verify that both CHANNEL_UP and
        //   CHANNEL_DOWN events are processed for the open connection with
        //   valid channel and server ID parameters.
        //
        // Testing:
        //   Concern: Import half-closed socket pair
        // --------------------------------------------------------------------

        if (verbose)
           cout << "\nTesting Concern: Import half-closed socket pair"
                << "\n===============================================" << endl;

        using namespace TEST_CASE_IMPORT_HALF_CLOSED_CONN;
        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::TestAllocator ta2(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 1,
                MAX_THREADS  = 1,
                SERVER_ID    = 0x701d50da,
                BACKLOG      = 1
            };

            btlso::InetStreamSocketFactory<btlso::IPv4Address>  factory(&ta2);
            btlso::StreamSocket<btlso::IPv4Address>            *socketA;
            btlso::StreamSocket<btlso::IPv4Address>            *socketB;

            btlmt::ChannelPool::PoolStateChangeCallback poolCb;
            btlmt::ChannelPool::BlobBasedReadCallback   dataCb;

            bslmt::Barrier  barrier(2);

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &barrier));

            makeNull(&poolCb);
            makeNull(&dataCb);

            btlmt::ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setIncomingMessageSizes(1, 32, 256);

            btlmt::ChannelPool pool(channelCb, dataCb, poolCb, cpc, &ta);
            ASSERT(0 == pool.start());
            ASSERT(0 == pool.listen(0, BACKLOG, SERVER_ID));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            enum { NUM_ITERATIONS = 20 };
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                btlso::SocketHandle::Handle handles[2];
                LOOP_ASSERT(i, 0 ==
                    btlso::SocketImpUtil::socketPair<btlso::IPv4Address>
                         (handles, btlso::SocketImpUtil::k_SOCKET_STREAM));

                socketA = factory.allocate(handles[0]);
                socketB = factory.allocate(handles[1]);
                LOOP_ASSERT(i,
                      0 == socketB->shutdown(btlso::Flag::e_SHUTDOWN_BOTH));
                LOOP_ASSERT(i, 0 == pool.numChannels());

                typedef btlso::StreamSocketFactoryDeleter Deleter;

                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                     managedSocket(socketA,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

                ASSERT(0 == pool.import(&managedSocket, SERVER_ID));
                barrier.wait();

                factory.deallocate(socketB);
            }
            ASSERT(0 == pool.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase5()
{
        // --------------------------------------------------------------------
        // TESTING 'import' METHOD
        //
        // Concerns:
        //   * That we can import as many sockets as specified in the
        //     configuration.
        //   * That we cannot import more sockets than allowed.
        //
        // Plan:
        //   Create an instance under test.  Create the maximum number of local
        //   (non-blocking) connections and import them into the channel pool.
        //   Then import one more connection and check that the import failed.
        //   Then read the number of written bytes returned, then close the
        //   channels.  Verify that the water mark messages are generated and
        //   check on the distribution of the load among the various event
        //   managers.
        //
        // Testing:
        //   int btlmt::ChannelPool::import(...);
        // --------------------------------------------------------------------

        if (verbose)
             cout << "\nTESTING 'import' METHOD."
                  << "\n========================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            bsl::vector<my_ChannelEvent> channelEvents;
            bslmt::Mutex                 channelEventsMutex;
            bsl::vector<my_PoolEvent>    poolEvents;
            bslmt::Mutex                 poolEventsMutex;

            const int NUM_SOCKETS = 20;
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setReadTimeout(1000);

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &recordChannelState
                                        , _1, _2, _3, _4
                                        , &channelEvents
                                        , &channelEventsMutex));

            btlmt::ChannelPool::PoolStateChangeCallback    poolCb(
                    bdlf::BindUtil::bind( &recordPoolState
                                        , _1, _2, _3
                                        , &poolEvents
                                        , &poolEventsMutex));

            btlmt::ChannelPool::BlobBasedReadCallback         dataCb;

            btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
            btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
            const btlmt::ChannelPool& X = mX;
            if (verbose) {
                P(config);
            }

            ASSERT(0 == mX.start());

            bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>  socketVecA;
            bsl::vector<btlso::StreamSocket<btlso::IPv4Address> *>  socketVecB;
            btlso::StreamSocket<btlso::IPv4Address>                *socketA;
            btlso::StreamSocket<btlso::IPv4Address>                *socketB;

            for (int i = 0; i < NUM_SOCKETS; ++i) {
                // Create sockets in pairs, otherwise Linux might fail its
                // autoread attempts and close the imported channel right-away.

                btlso::SocketHandle::Handle handles[2];
                LOOP_ASSERT(i, 0 ==
                    btlso::SocketImpUtil::socketPair<btlso::IPv4Address>
                         (handles, btlso::SocketImpUtil::k_SOCKET_STREAM));

                socketA = factory.allocate(handles[0]);
                socketB = factory.allocate(handles[1]);

                typedef btlso::StreamSocketFactoryDeleter Deleter;

                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                     managedSocket(socketA,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

                int retCode = mX.import(&managedSocket, i);
                LOOP2_ASSERT(i, retCode, 0 == retCode);
                socketVecA.push_back(socketA);
                socketVecB.push_back(socketB);
            }

            while (1) {
                bslmt::LockGuard<bslmt::Mutex> lock(&channelEventsMutex);
                const int NUM_EVENTS = (int) channelEvents.size();
                if (NUM_EVENTS / 2 >= NUM_SOCKETS) {
                    break;
                }
            }

            int numChannels = mX.numChannels();
            LOOP_ASSERT(numChannels, NUM_SOCKETS == numChannels);
            int numEvents = channelEvents.size();
            LOOP_ASSERT(numEvents, 2 * NUM_SOCKETS == numEvents);
            if (veryVerbose) {
                P(numEvents);
                PV(channelEvents);
            }

            {
                // Make sure we can't import more sockets than allowed.

                btlso::StreamSocket<btlso::IPv4Address> *socket
                                                          = factory.allocate();

                typedef btlso::StreamSocketFactoryDeleter Deleter;

                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                     managedSocket(socket,
                                   &factory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

                ASSERT(-1 == mX.import(&managedSocket, NUM_SOCKETS));
            }

            int importedFlag[NUM_SOCKETS];
            bsl::fill_n(importedFlag, NUM_SOCKETS, 0);

            for (int i = 0; i < numEvents; ++i) {
                LOOP_ASSERT(i, ChannelState::BTEMT_CHANNEL_UP ==
                                             channelEvents[i].d_event ||
                               ChannelState::BTEMT_AUTO_READ_ENABLED ==
                                              channelEvents[i].d_event);
                const int sourceId = channelEvents[i].d_allocatorId;
                LOOP_ASSERT(i, 0 <= sourceId);
                LOOP_ASSERT(i, sourceId < NUM_SOCKETS);
                if (ChannelState::BTEMT_CHANNEL_UP ==
                                             channelEvents[i].d_event) {
                    const int channelId = channelEvents[i].d_channelId;
                    LOOP_ASSERT(i, socketVecA[sourceId] ==
                                              X.streamSocket(channelId).get());
                }
                if (veryVerbose) { P(channelEvents[i].d_allocatorId); }

                if (channelEvents[i].d_event ==
                                     ChannelState::BTEMT_AUTO_READ_ENABLED)
                {
                    LOOP_ASSERT(i, 1 ==
                          importedFlag[channelEvents[i].d_allocatorId]);
                    importedFlag[channelEvents[i].d_allocatorId] = 3;
                }
                else {
                    LOOP_ASSERT(i, 0 ==
                          importedFlag[channelEvents[i].d_allocatorId]);
                    importedFlag[channelEvents[i].d_allocatorId] = 1;
                }
            }

            for (int i = 0; i < NUM_SOCKETS; ++i) {
                LOOP2_ASSERT(i, importedFlag[i], 3 == importedFlag[i]);
            }

            bsl::fill_n(importedFlag, NUM_SOCKETS, 0);
            bsl::vector<my_ChannelEvent> backup;

            for (int i = 0; i < (int)channelEvents.size(); ++i) {
                if (ChannelState::BTEMT_AUTO_READ_ENABLED !=
                                               channelEvents[i].d_event)
                {
                    backup.push_back(channelEvents[i]);
                }
            }

            channelEvents.clear();

            for (int i = 0; i < (int)backup.size(); ++i) {
                if (veryVerbose) {
                    P(backup[i].d_channelId);
                }
                mX.shutdown(backup[i].d_channelId,
                            btlmt::ChannelPool::e_IMMEDIATE);
            }

            while (1) {
                bslmt::LockGuard<bslmt::Mutex> lock(&channelEventsMutex);
                const int NUM_EVENTS = (int) channelEvents.size();
                if (NUM_EVENTS >= NUM_SOCKETS) {
                    break;
                }
            }

            numEvents = channelEvents.size();
            if (veryVerbose) { P(numEvents); PV(channelEvents); }

            bsl::fill_n(importedFlag, NUM_SOCKETS, 0);
            for (int i = 0; i < numEvents; ++i) {
                LOOP_ASSERT(i, ChannelState::BTEMT_CHANNEL_DOWN ==
                                              channelEvents[i].d_event);
                LOOP_ASSERT(i, NUM_SOCKETS >
                                        channelEvents[i].d_allocatorId);
                LOOP_ASSERT(i, 0 <= channelEvents[i].d_allocatorId);
                importedFlag[channelEvents[i].d_allocatorId] = 1;
            }

            for (int i = 0; i < NUM_SOCKETS; ++i) {
                LOOP_ASSERT(i, 1 == importedFlag[i]);
            }

            for (int i = 0; i < NUM_SOCKETS; ++i) {
                factory.deallocate(socketVecB[i]);
            }
            mX.stop();
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase4()
{
        // --------------------------------------------------------------------
        // TESTING ESTABLISHING LISTENING SOCKET - BASIC TEST
        //
        // Concerns:
        //   The channel pool should always be able establish a listening
        //   socket on port 0, and the query for the server address should
        //   return a valid address with a non-zero port.  Establishing
        //   listening socket on a well-known port must fail.  No memory leaks
        //   can happen.  Accept timeouts should be issued with the specified
        //   frequency.
        //
        // Plan:
        //   Create a channel pool object and call 'listen' on port 0.  Query
        //   for the server address and ASSERT that it is not 0.  Try to
        //   establish another server on the same port.  Verify that
        //   appropriate (failure) status is returned.  Verify that creating
        //   listening sockets with the same and different user-specified IDs
        //   is working correctly.
        //
        // Testing:
        //   int listen(int port, int backlog, int id);
        //   int listen(const IPv4Address& address, int backlog, int id);
        //   const btlso::IPv4Address *serverAddress(int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'listen' METHOD" << endl
                          << "=======================" << endl;

        btlmt::ChannelPoolConfiguration config;
        config.setMaxThreads(1);
        config.setMetricsInterval(10.0);
        config.setMaxConnections(0);

        btlmt::ChannelPool::ChannelStateChangeCallback channelCb;
        btlmt::ChannelPool::PoolStateChangeCallback    poolCb;
        btlmt::ChannelPool::BlobBasedReadCallback      dataCb;

        enum {
            // For a future version of channel pool, with no accessors.

            REGISTER_FAILED        = -7,
            SET_NONBLOCKING_FAILED = -6,
            LOCAL_ADDRESS_FAILED   = -5,
            BIND_FAILED            = -4,
            SET_OPTION_FAILED      = -3,
            ALLOCATE_FAILED        = -2,
            CAPACITY_REACHED       = -1,
            SUCCESS                =  0,
            DUPLICATE_ID           =  1
        };

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            // One server -- system specified port numbers.

            btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
            ASSERT(0 == mX.stop());
            int retCode = mX.listen(0, 1, 0);
            LOOP_ASSERT(retCode, 0 == retCode);

            btlso::IPv4Address address = getServerLocalAddress(&mX, 0);
            ASSERT(0 != address.portNumber());

            retCode = mX.listen(address.portNumber(), 1, 1);

#ifndef BSLS_PLATFORM_OS_WINDOWS
            LOOP_ASSERT(retCode, BIND_FAILED == retCode);
#endif
            retCode = mX.listen(address.portNumber(), 1, 0);
            LOOP_ASSERT(retCode, DUPLICATE_ID == retCode);
            retCode = mX.close(0);
            LOOP_ASSERT(retCode, 0 == retCode);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

        {
            // Two servers -- system specified port numbers.

            btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
            ASSERT(0 == mX.stop());
            int retCode = mX.listen(0, 1, 0);
            LOOP_ASSERT(retCode, 0 == retCode);
            btlso::IPv4Address address = getServerLocalAddress(&mX, 0);
            ASSERT(0 != address.portNumber());

            retCode = mX.listen(0, 1, 1);
            LOOP_ASSERT(retCode, 0 == retCode);
            btlso::IPv4Address address1 = getServerLocalAddress(&mX, 1);
            ASSERT(address.portNumber() != address1.portNumber());
            if (veryVerbose) { P(address); P(address1); }

            retCode = mX.close(1);
            LOOP_ASSERT(retCode, 0 == retCode);
            retCode = mX.close(0);
            LOOP_ASSERT(retCode, 0 == retCode);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

        {
            // Two servers - duplicate IDs.

            btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config);
            ASSERT(0 == mX.stop());
            int retCode = mX.listen(0, 1, 0);
            LOOP_ASSERT(retCode, 0 == retCode);
            btlso::IPv4Address address = getServerLocalAddress(&mX, 0);

            ASSERT(0 != address.portNumber());
            if (veryVerbose) {
                P(address);
            }
            retCode = mX.listen(address.portNumber(), 1, 1);
#ifndef BSLS_PLATFORM_OS_WINDOWS
            LOOP_ASSERT(retCode, BIND_FAILED == retCode);
#endif
            retCode = mX.listen(address.portNumber(), 1, 0);
            LOOP_ASSERT(retCode, DUPLICATE_ID == retCode);
            retCode = mX.listen(0, 1, 0);  // Valid address - duplicate ID.
            LOOP_ASSERT(retCode, 1 == retCode);
            retCode = mX.close(0);
            LOOP_ASSERT(retCode, 0 == retCode);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase3()
{
        // --------------------------------------------------------------------
        // TESTING 'connect' METHOD
        //
        // Concerns:
        //   * That connect performs the correct number of attempts, with the
        //   correct timeout in between.
        ///  * That it does not connect if the port is closed, and that it
        //   does connect if the port is open.
        //   * That the correct 'id' is passed to the callback.
        //   * That when connection to an invalid host/port combination,
        //   channel pool reports ERROR_CONNECTING as expected.
        //   * That after a failed connection, connection request can be
        //   re-queued with the same ID.
        //   * No memory leaks can happen.
        //
        // Plan:
        //   We use 'btlsos_tcptimedacceptor' in a separate thread as test
        //   harness.  The separate thread is launched by the pool state call
        //   back on the 'numAttempts - 1'-th attempt and opens the specified
        //   port, so that the first 'numAttempts - 1' connect attempts will
        //   fail and the last one does connect.  We verify, up to the maximum
        //   specified number of attempts, that a single connection can be
        //   established (on every attempt) and that the correct number of
        //   attempts have been performed.  Note that we only verify that the
        //   channel is created with auto-read enabled, since with auto-read
        //   disabled, we would have to close the channel manually using the
        //   as-yet untested 'shutdown' method.
        //
        //   Next, we try to establish connections to non-existent hosts, both
        //   when the channel pool is running, and when it is not.  We verify
        //   that connection attempts are successfully registered if the pool
        //   is running, but ultimately fail after the specified number of
        //   attempts, and that they are not even registered if the pool is not
        //   running.
        //
        // Testing:
        //   btlmt::ChannelPool::connect(...);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'connect' METHOD"
                          << "\n========================" << endl;

        using namespace TEST_CASE_CONNECT;
        bslma::TestAllocator ta(veryVeryVerbose);

        if (verbose) cout << "TESTING 'connect' TO NON-EXISTENT HOST\n";
        {
            struct {
                const char   *d_peerAddress;
                int           d_peerPort;
                int           d_sourceId;

            } DATA[] = {
                { "127.0.0.1",       8  , 292     },
                { "127.0.0.1",       24 , 117     },
                { "172.17.1.27",     26 , 280     },  // ibm1
                { "172.17.1.44",     26 , 349     },  // sundev3
                { "172.17.1.45",     26 , 316     },  // sundev15
                { "172.17.1.45",     64 , 527     },  // sundev15

                // Behind a firewall, this address will cause system 'connect'
                // to time out.
                { "128.238.3.21",    8  , 589     },  // utopia.poly.edu
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int MAX_NUM_ATTEMPTS = 3;

            const bsls::TimeInterval TIMEOUT(0.01);
            const int EXPECTED_SEVERITY = 1;

            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(1);

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb;
            btlmt::ChannelPool::BlobBasedReadCallback      dataCb;

            if (verbose)
                cout << "\tWhen channel pool is running: test 1\n";
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    for (int j = 0; j < MAX_NUM_ATTEMPTS; ++j) {
                        bsls::AtomicInt isInvoked(0);

                        btlmt::ChannelPool::PoolStateChangeCallback    poolCb(
                                bdlf::BindUtil::bind( &caseErrorPoolStateCb
                                                    , _1, _2, _3
                                                    , DATA[i].d_sourceId
                                                    , EXPECTED_SEVERITY
                                                    , &isInvoked));

                        btlmt::ChannelPool mX(channelCb, dataCb, poolCb,
                                             config, &ta);

                        LOOP2_ASSERT(i, j, 0 == mX.start());
                        btlso::IPv4Address peer(DATA[i].d_peerAddress,
                                                DATA[i].d_peerPort);
                        if (veryVerbose) {
                            MTCOUT << "\nRegistering at "
                                   << bdlt::CurrentTime::now() << MTENDL;
                            P(peer);
                            P(j + 1);
                            P(TIMEOUT);
                        }
                        int ret = mX.connect(peer, j + 1, TIMEOUT,
                                                           DATA[i].d_sourceId);
                        LOOP3_ASSERT(i, j, ret, 0 == ret);
                        while (j + 1 != isInvoked) {
                        }
                        if (veryVerbose) {
                            MTCOUT << "Succeded at "
                                << bdlt::CurrentTime::now() << MTENDL;
                        }
                        LOOP2_ASSERT(i, j, j + 1 == isInvoked);
                        if (veryVerbose) { P(isInvoked); }
                        ASSERT(0 == mX.stop());
                    }
                }
            }

            if (verbose)
                cout << "\tWhen channel pool is running: test 2\n";
            {
                bsl::vector<my_ChannelEvent> channelEvents;
                bslmt::Mutex                 channelEventsMutex;
                bsl::vector<my_PoolEvent>    poolEvents;
                bslmt::Mutex                 poolEventsMutex;

                btlmt::ChannelPool::PoolStateChangeCallback    poolCb(
                        bdlf::BindUtil::bind( &recordPoolState
                                           , _1, _2, _3
                                           , &poolEvents
                                           , &poolEventsMutex));

                btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &recordChannelState
                                       , _1, _2, _3, _4
                                       , &channelEvents
                                       , &channelEventsMutex));

                btlmt::ChannelPool::BlobBasedReadCallback      dataCb;

                config.setMaxThreads(4);
                btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                ASSERT(0 == mX.start());

                for (int j = 0; j < NUM_DATA; ++j) {
                    btlso::IPv4Address peer(DATA[j].d_peerAddress,
                                            DATA[j].d_peerPort);
                    if (veryVerbose) {
                        P_(bdlt::CurrentTime::now()); P_(peer);
                        P(TIMEOUT);
                    }
                    int ret = mX.connect(peer, 1, TIMEOUT, DATA[j].d_sourceId);
                    LOOP2_ASSERT(j, ret, 0 == ret);
                }

                ASSERT(0 == mX.stop());

                if (veryVerbose) {
                    PV(channelEvents);
                    PV(poolEvents);
                }
            }
            if (verbose) cout << "\tWhen channel pool is NOT running.\n";
            for (int i = 0; i < NUM_DATA; ++i) {
                for (int j = 0; j < MAX_NUM_ATTEMPTS; ++j) {
                    bsls::AtomicInt isInvoked(0);

                    btlmt::ChannelPool::PoolStateChangeCallback    poolCb(
                            bdlf::BindUtil::bind( &caseErrorPoolStateCb
                                                , _1, _2, _3
                                                , DATA[i].d_sourceId
                                                , EXPECTED_SEVERITY
                                                , &isInvoked));

                    btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config,
                                         &ta);
                    btlso::IPv4Address peer(DATA[i].d_peerAddress,
                            DATA[i].d_peerPort);
                    if (veryVerbose) {
                        P(peer);
                        P(j + 1);
                    }
                    LOOP2_ASSERT(i, j, -1 == mX.connect(peer, j + 1, TIMEOUT,
                                                        DATA[i].d_sourceId));
                    LOOP2_ASSERT(i, j, 0 == isInvoked);
                }
            }
        }
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

}

void TestDriver::testCase2()
{
        // --------------------------------------------------------------------
        // TESTING 'start' AND 'stop' METHODS
        //
        // Concerns: TODO
        // Plan: Start a channel pool, assert that there is one thread only,
        //   wait, and stop the channel pool.
        //
        // Testing
        //   btlmt::ChannelPool::start();
        //   btlmt::ChannelPool::stop();
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'start' AND 'stop'"
                 << "\n==========================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                BASE_PORT = 1234,
                BACKLOG   = 10
            };

            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMetricsInterval(10.0);

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb;
            btlmt::ChannelPool::PoolStateChangeCallback    poolCb;
            btlmt::ChannelPool::BlobBasedReadCallback      dataCb;

            btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);

            ASSERT(0 == mX.start());
            ASSERT(1 == mX.numThreads());

            ASSERT(0 == mX.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());

}

void TestDriver::testCase1()
{
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developer's sandbox.
        //
        // Plan:
        //   Create an instance of the entity under test and exercise some
        //   functionality.  This test tests nothing.
        //   Create a server and a client; establish a connection and send some
        //   data to the server, wait for response, then shut down connection
        //   gracefully on the server side.  Observe event on the client side.
        //   Then shut down the channel pool.
        //
        // Testing
        //   This test exercises basic functionality and tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            btlmt::ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMetricsInterval(10.0);

            btlmt::ChannelPool::ChannelStateChangeCallback channelCb;
            btlmt::ChannelPool::PoolStateChangeCallback    poolCb;
            btlmt::ChannelPool::BlobBasedReadCallback      dataCb;

            btlmt::ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);

            enum {
                BACKLOG = 10
            };

            for (int i = 0; i < 10; ++i) {
                btlso::IPv4Address address(getLocalAddress());
                int s = mX.listen(address, BACKLOG, i);
                if (veryVerbose) {
                    P(s);
                }
                LOOP_ASSERT(i, 0 == s);
                btlso::IPv4Address result = getServerLocalAddress(&mX, i);
            }

            for (int i = 0; i < 10; ++i) {
                LOOP_ASSERT(i, 0 == mX.start());
                LOOP_ASSERT(i, 1 == mX.numThreads());
                LOOP_ASSERT(i, 0 == mX.stop());
                LOOP_ASSERT(i, 0 == mX.numThreads());
            }

            for (int i = 0; i < 10; ++i) {
                LOOP_ASSERT(i, 0 == mX.close(i));
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
}

static void negativeCase1()
{
        // --------------------------------------------------------------------
        // TESTING a QueueClient
        //
        // Plan:
        //
        // Start my_QueueProcessor server first, then run this case in a
        // separate process
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING QUEUE PROCESSOR" << endl
                          << "=======================" << endl;

        if (verbose) {
            cout << "This test case (to be run manually) complements "
                 << "the stress test case.\n\n";

            cout << "Usage: Arguments provided to this test driver are"
                 << " (besides the case number, -1)\n\t<verbosity>"
                 << " <numConnections> <portNumber> <host> <numMessages>\n"
                 << "\tDefaults:  1  10  2564  127.0.0.1  10000\n"
                 << "\t\t(verbosity: 0 = silent, 1 = verbose, 2 = veryVerbose,"
                 << " 3 = veryVeryVerbose)\n";
        }

        using namespace QUEUE_CLIENT_NAMESPACE;

        enum {
            DEFAULT_PORT_NUMBER = 2564,
            DEFAULT_NUM_CONNECTIONS =  10,
            DEFAULT_NUM_MESSAGES = 10000
        };

        caseMinusOneInfo info;
        info.d_numConnections = (ARGC > 3) ? atoi(ARGV[3])
                                           : DEFAULT_NUM_CONNECTIONS;
        info.d_portNumber     = (ARGC > 4) ? atoi(ARGV[4])
                                           : DEFAULT_PORT_NUMBER;
        info.d_hostname       = (ARGC > 5) ? ARGV[5]
                                           : const_cast<char*>("127.0.0.1");
        info.d_numMessages    = (ARGC > 6) ? atoi(ARGV[6])
                                           : DEFAULT_NUM_MESSAGES;

        if (ARGC > 2) {
            verbose         = atoi(ARGV[2]) > 0;
            veryVerbose     = atoi(ARGV[2]) > 1;
            veryVeryVerbose = atoi(ARGV[2]) > 2;
        }
        else {
            verbose = 1;
            veryVerbose = veryVeryVerbose = 0;
        }

        usageExampleMinusOne(&info);

}

static void negativeCase2()
{
        // --------------------------------------------------------------------
        // TESTING CONNECTION MEM USAGE:
        //
        // Plan:
        //
        // Create a few connections, print the memory usage
        // --------------------------------------------------------------------

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Test Initialization

        enum {
            MAX_THREADS        = 1
         ,  SERVER_ID          = 1013410001
         ,  BACKLOG            = 1
        };

        using namespace TEST_CASE_N2_NAMESPACE;

        bslma::TestAllocator ta;
        btlmt::ChannelPool::BlobBasedReadCallback    dataCb;
        btlmt::ChannelPool::PoolStateChangeCallback  poolCb;

        bslmt::Barrier channelBarrier(2);

        btlmt::ChannelPool  *poolAddr;
        int                 poolEvent = -1;
        int                *eventAddr = &poolEvent;
        int                 channelId = -1;

        btlmt::ChannelPool::ChannelStateChangeCallback channelCb(
                    bdlf::BindUtil::bind( &caseN2ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &eventAddr
                                        , &channelBarrier
                                        , &channelId));

        btlmt::ChannelPoolConfiguration cpc;
        cpc.setMaxConnections(1024);
        cpc.setMaxThreads(MAX_THREADS);
        cpc.setMetricsInterval(100.0);

        const int maxOutgoingMessageSize = 2;
        cpc.setOutgoingMessageSizes(0,
                                    (maxOutgoingMessageSize+1)/2,
                                    maxOutgoingMessageSize);

        Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;
        poolAddr = &mX;

        const btlso::IPv4Address ADDRESS("127.0.0.1", 0);

        int retCode = mX.start();
        LOOP_ASSERT(retCode, 0 == retCode);
        retCode = mX.listen(ADDRESS, BACKLOG, SERVER_ID);
        LOOP_ASSERT(retCode, 0 == retCode);
        ASSERT(0 == X.numChannels());
        btlso::IPv4Address PEER = getServerLocalAddress(&mX, SERVER_ID);

        typedef btlso::InetStreamSocketFactory<btlso::IPv4Address> Factory;
        typedef btlso::StreamSocket<btlso::IPv4Address>            Socket;
        typedef btlsos::TcpChannel                                Channel;

        Factory              factory;

        bsl::vector<bsl::pair<Socket*, bsl::shared_ptr<Channel> > > channels;
        channels.resize(16);

        int oldNumBytes = ta.numBytesInUse();
        for (int i = 0; i < (int) channels.size(); ++i) {
            channels[i].first = factory.allocate();
            channels[i].second.createInplace(0, channels[i].first);

            Socket *socket = channels[i].first;
            Channel *channel = channels[i].second.get();
            int connectRetCode =  socket->connect(PEER);
            ASSERT(0 == connectRetCode);
            ASSERT(0 == channel->isInvalid());

            if (0 == connectRetCode) {
                channelBarrier.wait();
            }
            else {
                cout << "Failed to connect to channelpool.  Abort.\n";
                return;                                               // RETURN
            }
            const int newNumBytes = ta.numBytesInUse();
            bsl::cout << "Channel " << i << ": "
                      << newNumBytes - oldNumBytes
                      << bsl::endl;
            oldNumBytes = newNumBytes;
        }
        for (int i = 0; i < (int) channels.size(); ++i) {
            channels[i].second->invalidate();
            channels[i].second.reset();
            factory.deallocate(channels[i].first);
        }
}

static void negativeCase3()
{
        // --------------------------------------------------------------------
        // TESTING CONCERN: Stress test
        //
        // Concern:
        //   We want to exercise the component under stress.
        //
        // Plan:
        //   We will create two groups of 5 threads each.  Each groups will
        //   have a leader.  The leader will be responsible to open and close a
        //   channel.  Each thread will send request through the channelId open
        //   by its group leader.  Every 10 iterations, the group leader
        //   will close the channel and open another one.  Since the threads
        //   are not synchronized, their status is unknown so only a very
        //   superficial error testing will be performed, but the objective of
        //   this test is mainly to verify that no crashes or memory corruption
        //   occurs.  We also use a channel pool for the server (reading the
        //   other end of the channels) and use various parameters for that
        //   channel pool (incoming message size, number of bytes needed before
        //   the next data callback).
        //
        // Testing:
        //   Concern: Stress test
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING CONCERN: Stress test."
                 << "\n=============================" << endl;

        using namespace TEST_CASE_STRESS_TEST;

        typedef btlso::InetStreamSocketFactory<btlso::IPv4Address> Factory;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            if (verbose) cout << "\tSmall message test." << endl
                              << "\t-------------------" << endl;

            // Small individual messages on the client size, with NUM_THREADS
            // client, and one server accepting large messages.

            enum {
#if !defined(BSLS_PLATFORM_CPU_X86) && !defined(BSLS_PLATFORM_CPU_X86_64) \
 && !defined(BSLS_PLATFORM_OS_AIX)
                NUM_THREADS        = 10,
                NUM_ITERS          = 1005,   // never a multiple of 10
                LARGE_NUM_ITERS    = 10005,  // never a multiple of 10
#else
                // This check is due to the fact that we're still trying to
                // assert the return value of connect and shutdown.  If we
                // overload the machine too much, they will fail because of the
                // backlog.  Since it is still nice to check their return
                // values, we customize the test (our x86 boxes do not have
                // many CPUs).  Also, AIX is brain-dead and we scale the test
                // down.

                NUM_THREADS        = 4,
                NUM_ITERS          = 105,   // never a multiple of 10
                LARGE_NUM_ITERS    = 1005,  // never a multiple of 10

#endif
                BUFFER_SIZE        = 1024,
                SMALL_MSG_SIZE     = 327,
                MSG_SIZE           = 1024 * 2 + 512,
                SERVER_SIZE        = 1024 * 32,
                SERVER_NEEDED      = 1024 * 2 + 128
            };

            Factory factory(&ta);

            if (verbose) cout << "\t\tAlways invoke data callback." << endl;

            runTestCaseStressTest(ARGV[0],
                                  NUM_THREADS, NUM_ITERS,
                                  BUFFER_SIZE, MSG_SIZE,
                                  SERVER_SIZE, 1,
                                  &factory, &ta);

            if (verbose) cout << "\n\t\tInvoke data callback after "
                              << SERVER_NEEDED << " bytes only." << endl;

            runTestCaseStressTest(ARGV[0],
                                  NUM_THREADS, NUM_ITERS,
                                  BUFFER_SIZE, MSG_SIZE,
                                  SERVER_SIZE, SERVER_NEEDED,
                                  &factory, &ta);

            if (verbose) cout << "\n\t\tInvoke data callback every "
                              << SMALL_MSG_SIZE << " bytes (msg boundaries)."
                              << endl;

            runTestCaseStressTest(ARGV[0],
                                  2, LARGE_NUM_ITERS,
                                  BUFFER_SIZE, -SMALL_MSG_SIZE,
                                  SERVER_SIZE, SERVER_NEEDED,
                                  &factory, &ta);

            if (verbose) cout << "\n\t\tInvoke data callback every "
                              << MSG_SIZE << " bytes (msg boundaries)."
                              << endl;

            runTestCaseStressTest(ARGV[0],
                                  2, LARGE_NUM_ITERS,
                                  BUFFER_SIZE, -MSG_SIZE,
                                  SERVER_SIZE, SERVER_NEEDED,
                                  &factory, &ta);
        }
        ASSERT(0 <=  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

#if !defined(BSLS_PLATFORM_OS_AIX)
        // Bad AIX seems to have problems read and writing from the same
        // process, takes forever.  Similar cases commented out in case 9.
        {
            enum {
                NUM_THREADS        = 4,
                NUM_ITERS          = 101,
                BUFFER_SIZE        = 4096,
                MSG_SIZE           = 32768 + 50,
                SERVER_SIZE        = 32768,
                SERVER_NEEDED      = 32768 + 48
            };

            if (verbose) cout << endl
                              << "\tBig buffer test." << endl
                              << "\t----------------" << endl;

            // Large individual messages on the client size, with NUM_THREADS
            // client, and one server accepting large messages (of about the
            // same size).

            Factory factory(&ta);

            if (verbose) cout << "\t\tAlways invoke data callback." << endl;

            runTestCaseStressTest(ARGV[0],
                                  NUM_THREADS, NUM_ITERS,
                                  BUFFER_SIZE, MSG_SIZE,
                                  SERVER_SIZE, 1,
                                  &factory, &ta);

            if (verbose) cout << "\n\t\tInvoke data callback after "
                              << SERVER_NEEDED << " bytes only." << endl;

            runTestCaseStressTest(ARGV[0],
                                  NUM_THREADS, NUM_ITERS,
                                  BUFFER_SIZE, MSG_SIZE,
                                  SERVER_SIZE, SERVER_NEEDED,
                                  &factory, &ta);
        }
        ASSERT(0 <=  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

        {
            enum {
                NUM_THREADS        = 4,
                NUM_ITERS          = 101,
                BUFFER_SIZE        = 1024,
                MSG_SIZE           = 1024 * 16 + 50,
                SERVER_SIZE        = 1024 * 2,
                SERVER_NEEDED      = 1024 * 16 + 48
            };

            if (verbose) cout << endl
                              << "\tBig message test." << endl
                              << "\t-----------------" << endl;

            // Large individual messages on the client size, with NUM_THREADS
            // client, and one server accepting large messages (of about the
            // same size).

            Factory factory(&ta);

            // Always invoke data callback if data is incoming.

            runTestCaseStressTest(ARGV[0],
                                  NUM_THREADS, NUM_ITERS,
                                  BUFFER_SIZE, MSG_SIZE,
                                  SERVER_SIZE, 1,
                                  &factory, &ta);

            // Only invoke data callback when 'SERVER_NEEDED' bytes are
            // available.

            runTestCaseStressTest(ARGV[0],
                                  NUM_THREADS, NUM_ITERS,
                                  BUFFER_SIZE, MSG_SIZE,
                                  SERVER_SIZE, SERVER_NEEDED,
                                  &factory, &ta);
        }
        ASSERT(0 <=  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }
#endif

}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    ARGC = argc;
    ARGV = argv;

    cout << "TEST " << __FILE__ << " CASE " << test
         << " STARTED " << bdlt::CurrentTime::utc() << endl;

    ASSERT(0 == btlso::SocketImpUtil::startup());

#ifdef BSLS_PLATFORM_OS_UNIX
    // Ignore SIGPIPE - test driver-wide.  This signal is raised when writing
    // into a socket whose peer is down.  It creates havoc in some test cases.
    // but there is no reason it should be raised in any of the other test
    // cases either.

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = SIG_IGN;
    if (0 != sigaction(SIGPIPE, &sa, NULL)) {
        cout << "\n\t\t***WARNING****     Could not mask SIGPIPE."
                "      ***WARNING***\n" << endl;
        ASSERT(SIGPIPE);
    }
#endif

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER) case NUMBER: TestDriver::testCase##NUMBER(); break
      CASE(37);
      CASE(36);
      CASE(35);
      CASE(34);
      CASE(33);
      CASE(32);
      CASE(31);
      CASE(30);
      CASE(29);
      CASE(28);
      CASE(27);
      CASE(26);
      CASE(25);
      CASE(24);
      CASE(23);
      CASE(22);
      CASE(21);
      CASE(20);
      CASE(19);
      CASE(18);
      CASE(17);
      CASE(16);
      CASE(15);
      CASE(14);
      CASE(13);
      CASE(12);
      CASE(11);
      CASE(10);
      CASE(9);
      CASE(8);
      CASE(7);
      CASE(6);
      CASE(5);
      CASE(4);
      CASE(3);
      CASE(2);
      CASE(1);
      case -1: {
        negativeCase1();
      } break;
      case -2: {
        negativeCase2();
      } break;
      case -3: {
        negativeCase3();
      } break;
#undef CASE
      default: {
        cerr << "WARNING: CASE " << test << " NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == btlso::SocketImpUtil::cleanup());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    cout << "TEST CASE " << test << " ENDED "
         << bdlt::CurrentTime::utc() << endl;

//#endif // !BSLS_PLATFORM_OS_WINDOWS

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
