// btemt_channelpool.t.cpp         -*-C++-*-

#include <btemt_channelpool.h>

#include <btemt_channelpoolconfiguration.h>
#include <btemt_message.h>
#include <btemt_asyncchannel.h>
#include <bteso_flag.h>
#include <bteso_inetstreamsocketfactory.h>
#include <bteso_ipv4address.h>
#include <bteso_resolveutil.h>
#include <bteso_streamsocket.h>
#include <bteso_socketoptions.h>

#include <btesos_tcptimedacceptor.h>
#include <btesos_tcpchannel.h>
#include <bcemt_lockguard.h>
#include <bcemt_thread.h>
#include <btes_iovecutil.h>

#include <bcec_queue.h>

#include <bcema_testallocator.h>
#include <bcema_blobutil.h>
#include <bcema_pool.h>
#include <bcema_pooledbufferchain.h>
#include <bcemt_barrier.h>
#include <bcep_fixedthreadpool.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_stopwatch.h>
#include <bsls_platform.h>

#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdef_placeholder.h>
#include <bdef_memfn.h>
#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_byteoutstream.h>
#include <bdex_byteinstream.h>
#include <bdex_bytestreamimputil.h>
#include <bdeu_hashutil.h>
#include <bdeu_print.h>
#include <bdeut_strtokeniter.h>

#include <bsl_algorithm.h>

#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_memory.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsls_assert.h>

#ifdef BSLS_PLATFORM__OS_UNIX
#include <bsl_c_signal.h>
#include <sys/resource.h>
#endif

using namespace bsl;  // automatically added by script

using namespace BloombergLP;
using namespace bdef_PlaceHolders;

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
// section, concerns about the interplay of various functions, and concerns
// raised by various DRQS.
//-----------------------------------------------------------------------------
// CLASS 'btemt_ChannelPool'
// [ 2]  btemt_ChannelPool(...);
// [ 2]  ~btemt_ChannelPool();
// [ 3]  int btemt_ChannelPool::start(...);
// [ 3]  int btemt_ChannelPool::stop(...);
// [ 4]  int btemt_ChannelPool::connect(...);
// [ 5]  int btemt_ChannelPool::listen(int port, ...);
// [ 4]  int btemt_ChannelPool::listen(const bteso_IPv4Address&  address, ...);
// [ 5]  int btemt_ChannelPool::close(int serverID);
// [ 6]  int btemt_ChannelPool::import(...);
// [ 8]  int btemt_ChannelPool::shutdown();
// [ 9]  int btemt_ChannelPool::write(btemt_DataMsg, ...);
// [ 9]  int btemt_ChannelPool::write(btemt_BlobMsg, ...);
// [ 9]  int btemt_ChannelPool::write(btes_Iovecs, ...);
// [ 9]  int btemt_ChannelPool::write(btes_Ovecs, ...);
// [10]  int btemt_ChannelPool::registerClock(...);
// [10]  int btemt_ChannelPool::deregisterClock(...);
// [11]  int btemt_ChannelPool::enableRead(int channelId);
// [11]  int btemt_ChannelPool::disableRead(int channelId);
// [25]  int btemt_ChannelPool::setWriteCacheHiWatermark(int,int)
// [  ]  void *btemt_ChannelPool::channelContext(int channelId);
// [  ]  void btemt_ChannelPool::setChannelContext(int channelId, ...);
// [  ]  int btemt_ChannelPool::outboundBufferFactory();
// [  ]  int btemt_ChannelPool::outboundBlobBufferFactory();
// [  ]  int btemt_ChannelPool::set*Options(...);
// [  ]  int btemt_ChannelPool::get*Options(...) const;
// [  ]  int btemt_ChannelPool::numChannels() const;
// [  ]  int btemt_ChannelPool::numEvents() const;
// [  ]  int btemt_ChannelPool::numThreads() const;
// [13]  double btemt_ChannelPool::reportWeightedAverageReset();
// [28]  int btemt_ChannelPool::busyMetrics() const;
// [14]  int btemt_ChannelPool::getChannelStatistics*(...);
// [14]  int btemt_ChannelPool::numBytes*(...);
// [14]  int btemt_ChannelPool::totalBytes*(...);
// [  ]  const bteso_IPv4Address *btemt_ChannelPool::serverAddress(...) const;
//
// CLASS 'btemt_ChannelPool_MessageUtil'
// [26] btemt_ChannelPool_IovecArray<>
// [26] bsls_PlatformUtil::Int64 IovecArray<>::length() const;
// [26] static bsls_PlatformUtil::Int64 length(...);
// [26] static int write(bteso_StreamSocket<bteso_IPv4Address> *, ... )
// [26] static int loadIovec(btes_Iovec *, ... );
// [26] static int loadBlob(bcema_Blob *, ... );
// [26] static void appendToBlob(bcema_Blob *, ... );
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] CONCERN: Import half-closed socket pair
// [12] CONCERN: Flow control
// [15] CONCERN: Shutdown inside channel state callback
// [11] CONCERN: Number of channels is as specified in configuration
// [17] CONCERN: DRQS 4340683 (Channelpool.shutdown() results in callback
//                             nonsense)
// [18] CONCERN: DRQS 4430835 (Channel pool's max #connections allows max
//                             #connections + 1)
// [19] CONCERN: DRQS 5425522 (must fail to accept connections if FD limit is
//                             reached - works as designed)
//      CONCERN: DRQS 9214891 (running out of file descriptors)
// [20] CONCERN: DRQS 8397003 (fails to import under high load)
// [21] CONCERN: Ssl-like sockets
// [22] CONCERN: Stress test
// [23] CONCERN: Half-open connections
// [27] CONCERN: Read timeout
// [28] TESTING: 'busyMetrics' and time metrics collection.
// [28] CONCERN: Event Manager Allocation
// [30] USAGE EXAMPLE 2
// [31] (OLD) USAGE EXAMPLE my_QueueProcessor
// [32] (OLD) USAGE EXAMPLE VLM Echo Server
// [33] USAGE EXAMPLE 1
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

static bcemt_Mutex coutMutex;

#define MTCOUT   { coutMutex.lock(); cout \
                                           << bcemt_ThreadUtil::selfIdAsInt() \
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef btemt_ChannelPool         Obj;
typedef bteso_SocketOptUtil       SockOpt;
typedef bteso_SocketOptions       SocketOptions;
typedef bteso_LingerOptions       LingerOptions;
typedef bsls_PlatformUtil::Uint64 ThreadId;

const ThreadId NULL_THREAD_ID = (ThreadId) (long long) -1;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//                       TESTING FUNCTIONS/CLASSES HELPER
//-----------------------------------------------------------------------------

template <typename A1, typename A2, typename A3>
struct NullFn3 {
    void operator()(A1, A2, A3) {}
};

template <typename A1, typename A2, typename A3, typename A4>
struct NullFn4 {
    void operator()(A1, A2, A3, A4) {}
};

template <typename A1, typename A2, typename A3>
void makeNull(bdef_Function<void (*)(A1, A2, A3)> * f) {
    *f = NullFn3<A1, A2, A3>();
}

template <typename A1, typename A2, typename A3>
void makeNull(bslma_Allocator * a, bdef_Function<void (*)(A1, A2, A3)> * f) {
    *f = bdef_Function<void (*)(A1, A2, A3)>(NullFn3<A1, A2, A3>(), a);
}

template <typename A1, typename A2, typename A3, typename A4>
void makeNull(bdef_Function<void (*)(A1, A2, A3, A4)> * f) {
    *f = NullFn4<A1, A2, A3, A4>();
}

template <typename A1, typename A2, typename A3, typename A4>
void makeNull(bslma_Allocator * a, bdef_Function<void (*)(A1, A2, A3, A4)> * f)
{
    *f = bdef_Function<void (*)(A1, A2, A3, A4)>(NullFn4<A1, A2, A3, A4>(), a);
}

struct my_ChannelEvent {
    bdet_TimeInterval          d_when;
    bcemt_ThreadUtil::Handle   d_thread;
    bsls_PlatformUtil::Uint64  d_threadId;
    btemt_ChannelMsg           d_data;
    void                      *d_context;
};

bsl::ostream& operator<<(bsl::ostream& s, const my_ChannelEvent& event) {
    s << "{ "
      << event.d_when     << ", "
      << event.d_threadId << ", "
      << event.d_data     << ", "
      << event.d_context
      << " }";
    return s;
}

struct my_PoolEvent {
    bdet_TimeInterval         d_when;
    bcemt_ThreadUtil::Handle  d_thread;
    bsls_PlatformUtil::Uint64 d_threadId;
    btemt_PoolMsg             d_data;
};

bsl::ostream& operator<<(bsl::ostream& s, const my_PoolEvent& event) {
    s << "{ "
      << event.d_when     << ", "
      << event.d_threadId << ", "
      << event.d_data
      << "}";
    return s;
}

static
void recordChannelState(int                           channelId,
                        int                           sourceId,
                        int                           state,
                        void                         *context,
                        bsl::vector<my_ChannelEvent> *results,
                        bcemt_Mutex                  *resultsLock)
    // Record the occurrence of this event into the specified 'results'
    // array of events using the specified 'resultsLock' for synchronization.
{
    ASSERT(results);
    ASSERT(resultsLock);

    my_ChannelEvent event;
    event.d_when = bdetu_SystemTime::now();
    event.d_thread = bcemt_ThreadUtil::self();
    event.d_threadId = bcemt_ThreadUtil::selfIdAsUint64();
    event.d_data.setChannelId(channelId);
    event.d_data.setAllocatorId(sourceId);
    event.d_data.setChannelState((btemt_ChannelMsg::ChannelState)state);
    event.d_context = context;
    bcemt_LockGuard<bcemt_Mutex> lock(resultsLock);
    results->push_back(event);
}

static
void recordPoolState(int                        state,
                     int                        sourceId,
                     int                        severity,
                     bsl::vector<my_PoolEvent> *results,
                     bcemt_Mutex               *resultsLock)
    // Record the occurrence of this event into the specified 'results'
    // array of events using the specified 'resultsLock' for synchronization.
{
    ASSERT(results);
    ASSERT(resultsLock);

    my_PoolEvent event;
    event.d_when = bdetu_SystemTime::now();
    event.d_thread = bcemt_ThreadUtil::self();
    event.d_threadId = bcemt_ThreadUtil::selfIdAsUint64();
    event.d_data.setSourceId(sourceId);
    event.d_data.setState((btemt_PoolMsg::PoolState)state);

    bcemt_LockGuard<bcemt_Mutex> lock(resultsLock);
    results->push_back(event);
}

static inline void assertCb() {
    ASSERT("This function should NOT be called" && 0);
}

class ChannelPoolStateCbTester {
    // This class is a wrapper around a 'btemt_ChannelPool' that provides a
    // 'waitForState' method for testing whether a particular channel pool
    // state event was delivered by the channel pool via the channel state
    // callback.  On construction, a 'ChannelPoolStateCbTester' initializes a
    // channel pool using the supplied channel pool configuration.  Clients
    // can then access and modify the created channel pool, and call
    // 'waitForState' to wait for a particular channel state callback.

  public:
    // PUBLIC TYPES
    struct ChannelState {
        // A channel state object represents the parameters passed to the
        // 'channelStateCb'.

        int                 d_channelId;   // channel id for the state callback
        int                 d_sourceId;    // source id for the state callback
        int                 d_state;       // state for the state callback
        bsls_PlatformUtil::Uint64
                            d_threadId;    // thread id for the state callback

        ChannelState(int                       channelId,
                     int                       sourceId,
                     int                       state,
                     bsls_PlatformUtil::Uint64 threadId)
        : d_channelId(channelId)
        , d_sourceId(sourceId)
        , d_state(state)
        , d_threadId(threadId)
        {
        }
    };

  private:
    // DATA
    bdema_ManagedPtr<btemt_ChannelPool>
                        d_channelPool_p;  // wrapper pool

    int                 d_lastChannelId;  // id of the most recently open
                                          // channel

    bsl::vector<ChannelState>
                        d_channelStates;  // sequence of states since last
                                          // 'waitForState'

    bcemt_Mutex         d_mutex;          // synchronize access to data

    bcemt_Condition     d_condition;      // wait for a state callback

    void channelStateCb(int    channelId,
                        int    sourceId,
                        int    state,
                        void  *arg)
        // Append the specified 'state' to the queue of channel states, and if
        // 'waitForState' is currently blocked waiting for a state, wake of
        // the waiting thread to return the update state information.
    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        d_channelStates.push_back(
                         ChannelState(channelId,
                                      sourceId,
                                      state,
                                      bcemt_ThreadUtil::selfIdAsInt()));
        switch(state){
          case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
              if (veryVerbose) {
                  MTCOUT << "Connection terminated:"
                         << " serverId="  << bsl::hex << sourceId << bsl::dec
                         << " channelId=" << channelId
                         << MTENDL;
              }
              d_channelPool_p->shutdown(channelId,
                                        btemt_ChannelPool::BTEMT_IMMEDIATE);

          } break;

          case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
              if (veryVerbose) {
                  MTCOUT << "Connection established:"
                         << " serverId="  << bsl::hex << sourceId << bsl::dec
                         << " channelId=" << channelId
                         << MTENDL;
              }
              d_lastChannelId = channelId;

          } break;
          case btemt_ChannelPool::BTEMT_READ_TIMEOUT: {
              if (veryVerbose) {
                  MTCOUT << "READ TIMEOUT " << MTENDL;
              }
          } break;
          case btemt_ChannelPool::BTEMT_WRITE_CACHE_HIWAT: {
              if (veryVerbose) {
                  MTCOUT << "HIWAT " << MTENDL;
              }
          } break;
          case btemt_ChannelPool::BTEMT_WRITE_CACHE_LOWWAT: {
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
                     const btemt_ChannelPoolConfiguration&  config,
                     bslma_Allocator                       *basicAllocator = 0)
        // Initialize a channel pool state callback tester using the specified
        // 'config'.  This object will manage an underlying
        // 'btemt_ChannelPool', configured with 'config', and will report any
        // state changes to that channel pool using the 'waitForState' method.
        // The underlying channel pool will be initialized with a null data
        // and pool-state callback.
    : d_channelPool_p(0)
    , d_lastChannelId(0)
    , d_mutex()
    , d_condition()
    {
        bslma_Allocator *ma = bslma_Default::allocator(basicAllocator);
        btemt_ChannelPool::ChannelStateChangeCallback channelCb(
               bdef_MemFnUtil::memFn( &ChannelPoolStateCbTester::channelStateCb
                                    , this)
             , ma);

        btemt_ChannelPool::DataReadCallback         dataCb;
        btemt_ChannelPool::PoolStateChangeCallback  poolCb;

        makeNull(&poolCb);
        makeNull(&dataCb);

        d_channelPool_p.load(
            new (*ma) btemt_ChannelPool(channelCb, dataCb, poolCb, config, ma),
            ma);
    }

    ChannelPoolStateCbTester(
               const btemt_ChannelPoolConfiguration&       config,
               btemt_ChannelPool::DataReadCallback         dataCb,
               btemt_ChannelPool::PoolStateChangeCallback  poolCb,
               bslma_Allocator                            *basicAllocator = 0)
        // Initialize a channel pool state callback tester using the specified
        // 'config', 'dataCb', and 'poolCb'.  This object will manage an
        // underlying 'btemt_ChannelPool', configured with 'config', and will
        // report any state changes to that channel pool using the
        // 'waitForState' method.  The underlying channel pool will be
        // initialized with the provided data and pool-state callbacks.
    : d_channelPool_p(0)
    , d_lastChannelId(0)
    , d_mutex()
    , d_condition()
    {
        bslma_Allocator *ma = bslma_Default::allocator(basicAllocator);
        btemt_ChannelPool::ChannelStateChangeCallback channelCb(
               bdef_MemFnUtil::memFn( &ChannelPoolStateCbTester::channelStateCb
                                    , this)
             , ma);

        d_channelPool_p.load(
            new (*ma) btemt_ChannelPool(channelCb, dataCb, poolCb, config, ma),
            ma);
    }

    // MANIPULATORS
    btemt_ChannelPool& pool()  { return *d_channelPool_p; }

    // ACCESSORS
    int waitForState(bsl::vector<ChannelState> *stateChanges,
                     int                        state,
                     const bdet_TimeInterval&   elapsedTime)
        // Wait for up to the specified 'elapsedTime' for a client to receive
        // a channel state callback indicating the specified 'state'; return 0
        // and append to the specified 'stateChanges' the list of channel
        // states since the previous successful call to 'waitForState' on
        // success; return a non-zero value if the 'elapsedTime' passes
        // without seeing the indicated 'state'.
    {
        bdet_TimeInterval now     = bdetu_SystemTime::now();
        bdet_TimeInterval timeout = now + elapsedTime;
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        int  idx   = 0;
        bool found = false;
        while (!found && now < timeout) {
            for (; idx < (int) d_channelStates.size(); ++idx) {
                if (d_channelStates[idx].d_state == state) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                d_condition.timedWait(&d_mutex, timeout);
            }
            now = bdetu_SystemTime::now();
        }
        if (found) {
            *stateChanges = d_channelStates;
            d_channelStates.clear();
            return 0;
        }
        return -1;
    }

    int lastOpenedChannelId()
        // Return the identifier for the most recent channel to receive the
        // 'CHANNEL_UP' state.
    { return d_lastChannelId; }

};

void populateMessage(bcema_Blob      *msg,
                     int              length,
                     bslma_Allocator *allocator)
    // Populate the specified 'msg' with a buffer of the specified 'length'
    // containing arbitrary data (starting at -1 (0xFF) and decreasing by 1 per
    // element) using the specified 'allocator' to supply memory;
{
    ASSERT(0 < length);

    bcema_SharedPtr<char> buffer =
              bcema_SharedPtrUtil::createInplaceUninitializedBuffer(length,
                                                                    allocator);
    char value = -1; // 0xff
    char  *c   = buffer.ptr();
    char  *end = c + length;
    for (; c < end; ++c) {
        *c = value--;
    }
    bcema_BlobBuffer blobBuffer(buffer, length);
    msg->appendDataBuffer(blobBuffer);
}

//-----------------------------------------------------------------------------
// CASE 36
//-----------------------------------------------------------------------------

namespace CASE36 {

Obj       *channelPool;
int        channelId;
const int  CACHE_HI_WAT = 6000000;

bcema_PooledBlobBufferFactory blobFactory(256);

int delayBetweenWrites = 1000000;
int maxWritesPerThread = 10000;
int maxConsecutiveFailures = 1;
int minMsgSize = 100;
int maxMsgSize = 32000;

bcemt_Barrier *barrier;

void poolStateCb(int            state,
                 int            source,
                 int            severity)
{
    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(&coutMutex);
        bsl::cout << "Pool state callback called with"
                  << " State: " << state
                  << " Source: "  << source
                  << " Severity: " << severity << bsl::endl;
    }
}

void channelStateCb(int              channelId,
                    int              serverId,
                    int              state,
                    void            *arg,
                    int             *id,
                    bcemt_Barrier   *barrier)
{
    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(&coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }
    if (btemt_ChannelPool::BTEMT_CHANNEL_UP == state) {
        *id = channelId;
        barrier->wait();
    }
}

void blobBasedReadCb(int             *needed,
                     bcema_Blob      *msg,
                     int              channelId,
                     void            *arg)
{
    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(&coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }
    *needed = 1;
    msg->removeAll();
}

int write(int nBytes)
{
    bcema_Blob blob(&blobFactory);
    blob.setLength(nBytes);
    return channelPool->write(channelId, blob);
}

enum  {
    ERROR_IMPOSSIBLE_WRITE_SUCCEEDED = 1,
    ERROR_MAX_POSSIBLE_WRITE_FAILED = 2
};

int check()
{
    int rc;

    // try to write too much

    rc = write(CACHE_HI_WAT + 1);
    MTLOOP_ASSERT(rc, rc);
    if (!rc) return ERROR_IMPOSSIBLE_WRITE_SUCCEEDED;

    // try to write max possible

    rc = write(CACHE_HI_WAT);
    MTLOOP_ASSERT(rc, !rc);
    if (rc) return ERROR_MAX_POSSIBLE_WRITE_FAILED;

    return 0;
}

void delay(int delay)
{
    int i;
    for (i = 0; i < delay; ++i);
}

// signal allow writer threads to start writing from the same time (to
// increase concurrency), barrier could be another way but might be too heavy.

static bces_AtomicInt sig = 0;
void signalerThread()
{
    while(1) {
        ++sig;

        delay(delayBetweenWrites);
    }
}

void writerThread(unsigned threadIndex)
{
    int failures = 0;
    int consecutiveFailures = 0;
    int iter;
    int oldSignal = 0;

    bcema_Blob blob(&blobFactory);
    for (iter = 0; iter < maxWritesPerThread &&
                   consecutiveFailures < maxConsecutiveFailures; ++iter) {

        int randVal = rand_r(&threadIndex);
        int nBytes  = minMsgSize + randVal % (maxMsgSize - minMsgSize + 1);
        blob.setLength(nBytes);

        while(sig <= oldSignal);
        oldSignal = sig;

        int rc = channelPool->write(channelId, blob);
        MTLOOP_ASSERT(rc, !rc);

        if (rc == 0) {
            consecutiveFailures = 0;
        }
        else {
            ++consecutiveFailures;
            ++failures;
        }
    }

    barrier->wait();
}

}

//-----------------------------------------------------------------------------
// CASE 35
//-----------------------------------------------------------------------------

namespace CASE35 {

int                    s_channelId = -1;
static bcemt_Mutex     case35Mutex;
static bcemt_Condition case35Condition;
bool                   poolCbCalled = false;

void poolStateCb(int state, int source, int severity)
{
    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(&coutMutex);
        bsl::cout << "Pool state callback called with"
                  << " State: " << state
                  << " Source: "  << source
                  << " Severity: " << severity << bsl::endl;
    }
    poolCbCalled = true;
    case35Condition.signal();
}

void channelStateCb(int channelId, int serverId, int state, void *arg)
{
    if (btemt_ChannelPool::BTEMT_CHANNEL_UP == state) {
        s_channelId = channelId;
        case35Condition.signal();
    }
    else if (btemt_ChannelPool::BTEMT_CHANNEL_DOWN == state) {
        s_channelId = -1;
        case35Condition.signal();
    }

    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(&coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }
}

void blobBasedReadCb(int             *needed,
                     bcema_Blob      *msg,
                     int              channelId,
                     void            *arg)
{
    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(&coutMutex);
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
// right to configure a 'bteso_SocketOptions' according to a custom language.
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
        linger.setUseLingering('Y' == value ? true : false);
        const char nextValue = *(specString + 2);
        LOOP_ASSERT(nextValue, '0' == nextValue
                    || '1' == nextValue || '2' == nextValue);
        linger.setTimeout(DATA[nextValue - '0']);
        options->setLinger(linger);
        ++numRead;
      } break;
      default: {
        LOOP2_ASSERT(spec, value, 0);
        return 0;
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
            return numRead;
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

int verify(const Obj&                 pool,
           int                        channelId,
           const bteso_SocketOptions& options)
    // Verify that the socket options on the specified 'handle' match the
    // specified 'options'.  Return 0 on success and a non-zero value
    // otherwise.
{
    if (!options.debugFlag().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                        &result,
                                        bteso_SocketOptUtil::BTESO_DEBUGINFO,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                                        bteso_SocketOptUtil::BTESO_BROADCAST,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                                      bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                                      bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                      channelId);
        if (rc) {
            return rc;                                                // RETURN
        }

        LOOP2_ASSERT((bool) result, options.reuseAddress().value(),
                     (bool) result == options.reuseAddress().value());
    }

    if (!options.keepAlive().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                        &result,
                                        bteso_SocketOptUtil::BTESO_KEEPALIVE,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                                        bteso_SocketOptUtil::BTESO_DONTROUTE,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.bypassNormalRouting().value(),
                     (bool) result == options.bypassNormalRouting().value());
    }

    if (!options.linger().isNull()) {
        bteso_SocketOptUtil::LingerData lingerData;
        const int rc = pool.getLingerOption(&lingerData, channelId);
        if (rc) {
            return rc;                                                // RETURN
        }

        LOOP2_ASSERT((bool) lingerData.l_onoff,
                     options.linger().value().useLingering(),
         (bool) lingerData.l_onoff == options.linger().value().useLingering());
        LOOP2_ASSERT(lingerData.l_linger, options.linger().value().timeout(),
                    lingerData.l_linger == options.linger().value().timeout());
    }

    if (!options.leaveOutOfBandDataInline().isNull()) {
        int result;
        const int rc = pool.getSocketOption(
                                        &result,
                                        bteso_SocketOptUtil::BTESO_OOBINLINE,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                                        bteso_SocketOptUtil::BTESO_SENDBUFFER,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                                     bteso_SocketOptUtil::BTESO_RECEIVEBUFFER,
                                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                                       bteso_SocketOptUtil::BTESO_SENDLOWATER,
                                       bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                                    bteso_SocketOptUtil::BTESO_RECEIVELOWATER,
                                    bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                                       bteso_SocketOptUtil::BTESO_SENDTIMEOUT,
                                       bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                                    bteso_SocketOptUtil::BTESO_RECEIVETIMEOUT,
                                    bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                    channelId);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.receiveTimeout().value(),
                     result == options.receiveTimeout().value());
    }

    return 0;
}

}

//-----------------------------------------------------------------------------
// CASE 31 DRQS 22256519
//-----------------------------------------------------------------------------
namespace TEST_CASE_31_NAMESPACE {

class ReadServer
{
    // A read server.

    enum { SERVER_ID = 1066 };

    int                d_port;         // well-known port for service requests
    bsl::string        d_data;
    int                d_expDataSize;
    bslma_Allocator   *d_allocator_p;
    btemt_ChannelPool *d_cp_p;
    bcemt_Mutex       *d_coutMutex;

  private:
    // ChannelPool Callback Functions
    void poolCB(int state, int source, int severity);

    void chanCB(int channelId, int serverId, int state, void *arg);

    void blobBasedReadCb(int        *numNeeded,
                         bcema_Blob *msg,
                         int         channelId,
                         void       *context);
        // Blob based read callback.

    // Not Implemented
    ReadServer(const ReadServer&);
    ReadServer& operator=(const ReadServer&);

  public:
    // CREATORS
    ReadServer(bcemt_Mutex                           *coutMutex,
               int                                    expDataSize,
               const btemt_ChannelPoolConfiguration&  channelPoolConfig,
               bslma_Allocator                       *allocator = 0);
        // Create a server object which accepts connections
        // on localhost at the specified by 'port' and uses the specified
        // 'useBlobForDataReads' to decide if 'bcema_Blob' or
        // 'bcema_PooledBufferChain' should be used for data reads.  The server
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

    bsl::string& data();
        // Return the data stored by this server.

    // ACCESSORS
    int portNumber() const;
        // Return the port number on the local host on which this server
        // listens to connections.

    const bsl::string& data() const;
        // Return the data stored by this server.
};

// CREATORS
ReadServer::ReadServer(
        bcemt_Mutex                           *coutMutex,
        int                                    expDataSize,
        const btemt_ChannelPoolConfiguration&  channelPoolConfig,
        bslma_Allocator                       *allocator)
: d_port(0)
, d_data(allocator)
, d_expDataSize(expDataSize)
, d_allocator_p(bslma_Default::allocator(allocator))
, d_coutMutex(coutMutex)
{
    btemt_ChannelPool::PoolStateChangeCallback poolCb(
                                     bdef_MemFnUtil::memFn(&ReadServer::poolCB,
                                                           this),
                                     d_allocator_p);

    btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                                     bdef_MemFnUtil::memFn(&ReadServer::chanCB,
                                                           this),
                                     d_allocator_p);

    btemt_ChannelPool::BlobBasedReadCallback dataFunctor =
                            bdef_MemFnUtil::memFn(&ReadServer::blobBasedReadCb,
                                                  this);

    d_cp_p = new (*d_allocator_p) btemt_ChannelPool(channelCb,
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
    d_port = d_cp_p->serverAddress(SERVER_ID)->portNumber();
    return src || lrc;
}

int ReadServer::stop()
{
    return d_cp_p->stop();
}

void ReadServer::poolCB(int state, int source, int severity)
{
}

void ReadServer::chanCB(int channelId, int serverId, int state, void *arg)
{
    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(d_coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        d_cp_p->shutdown(channelId, btemt_ChannelPool::BTEMT_IMMEDIATE);
      } break;
      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
      } break;
    }
}

void ReadServer::blobBasedReadCb(int             *needed,
                                 bcema_Blob      *msg,
                                 int              channelId,
                                 void            *arg)
{
    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(d_coutMutex);
        bsl::cout << "Blob Based Read Cb called with"
                  << " Channel Id: " << channelId
                  << " of length: "  << msg->length() << bsl::endl;
    }

    ASSERT(needed);
    ASSERT(msg);

    const int numBuffers = msg->numDataBuffers();
    const int length     = msg->length();

    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(d_coutMutex);
        bcema_BlobUtil::hexDump(bsl::cout, *msg, 0, length);
    }

    if (d_expDataSize != length) {
        *needed = 1;
        return;
    }

    for (int i = 0; i < numBuffers - 1; ++i) {
        const bcema_BlobBuffer& buffer = msg->buffer(i);
        d_data.append(buffer.data(), buffer.size());
    }

    const bcema_BlobBuffer& buffer = msg->buffer(numBuffers - 1);
    d_data.append(buffer.data(), msg->lastDataBufferLength());

    bcema_BlobUtil::erase(msg, 0, length);

    *needed = 1;
}

// MANIPULATORS
std::string& ReadServer::data()
{
    return d_data;
}


// ACCESSORS
int ReadServer::portNumber() const
{
    return d_cp_p->serverAddress(SERVER_ID)->portNumber();
}

const std::string& ReadServer::data() const
{
    return d_data;
}

}  // namespace TEST_CASE_31_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 30 read testing
//-----------------------------------------------------------------------------
namespace TEST_CASE_30_NAMESPACE {

                        // ================
                        // class DataReader
                        // ================

class DataReader {
    // This class implements a data reader whose callback functions are
    // invoked to read the data from a btemt_ChannelPoolChannel object.

    int                 d_msgId;           // message id
    int                 d_msgLength;       // message length
    bsl::string         d_data;            // actual data
    mutable bcemt_Mutex d_mutex;           // mutex for data

  public:
    // CREATORS
    DataReader();
        // Construct a default reader.

    ~DataReader();
        // Destroy this reader.

    // MANIPULATORS
    void blobBasedReadCb(int        *numNeeded,
                         bcema_Blob *msg,
                         int         channelId,
                         void       *context);
        // Blob based read callback.

    void pbcBasedReadCb(int                  *numConsumed,
                        int                  *needed,
                        const btemt_DataMsg&  msg,
                        void                 *context);
        // Pooled Buffer Chain based read callback.

    // ACCESSORS
    const bsl::string& data() const;
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
    const bool d_useBlobForDataReads;

    bslma_Allocator         *d_allocator_p;
    btemt_ChannelPool       *d_cp_p;
    ChannelMap               d_channelMap;
    mutable bcemt_Mutex      d_channelMapMutex;
    DataMap                  d_dataMap;
    mutable bcemt_Mutex      d_dataMapMutex;
    mutable bcemt_Mutex      d_generalMutex;
    bcemt_Mutex             *d_coutMutex;

  private:
    // ChannelPool Callback Functions
    void poolCB(int state, int source, int severity);

    void chanCB(int channelId, int serverId, int state, void *arg);

    void blobBasedReadCb(int        *numNeeded,
                         bcema_Blob *msg,
                         int         channelId,
                         void       *context);
        // Blob based read callback.

    void pbcBasedReadCb(int                  *numConsumed,
                        int                  *needed,
                        const btemt_DataMsg&  msg,
                        void                 *context);
        // Pooled Buffer Chain based read callback.

    // Not Implemented
    ReadServer(const ReadServer&);
    ReadServer& operator=(const ReadServer&);

  public:
    // CREATORS
    ReadServer(bcemt_Mutex       *coutMutex,
               int                port,
               bool               useBlobForDataReads,
               bslma_Allocator   *allocator = 0);
        // Create a server object which accepts connections
        // on localhost at the specified by 'port' and uses the specified
        // 'useBlobForDataReads' to decide if 'bcema_Blob' or
        // 'bcema_PooledBufferChain' should be used for data reads.  The server
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
        bcemt_Mutex       *coutMutex,
        int                port,
        bool               useBlobForDataReads,
        bslma_Allocator   *allocator)
: d_port(port)
, d_useBlobForDataReads(useBlobForDataReads)
, d_allocator_p(bslma_Default::allocator(allocator))
, d_coutMutex(coutMutex)
{
    btemt_ChannelPoolConfiguration cpc;
    cpc.setMaxConnections(20);
    cpc.setMaxThreads(20);
    cpc.setMetricsInterval(10.0);
    cpc.setIncomingMessageSizes(1, 5, 10);

    btemt_ChannelPool::PoolStateChangeCallback poolCb(
                bdef_MemFnUtil::memFn( &ReadServer::poolCB
                                     , this)
              , d_allocator_p);

    btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                bdef_MemFnUtil::memFn( &ReadServer::chanCB
                                     , this)
              , d_allocator_p);

    if (d_useBlobForDataReads) {
        btemt_ChannelPool::BlobBasedReadCallback dataFunctor =
                            bdef_MemFnUtil::memFn(&ReadServer::blobBasedReadCb,
                                                  this);

        d_cp_p = new (*d_allocator_p) btemt_ChannelPool(channelCb,
                                                        dataFunctor,
                                                        poolCb,
                                                        cpc,
                                                        allocator);
    }
    else {
        btemt_ChannelPool::DataReadCallback dataCb(
                    bdef_MemFnUtil::memFn( &ReadServer::pbcBasedReadCb
                                         , this)
                  , d_allocator_p);

        d_cp_p = new (*d_allocator_p) btemt_ChannelPool(channelCb,
                                                        dataCb,
                                                        poolCb,
                                                        cpc,
                                                        allocator);
    }
}

ReadServer::~ReadServer()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_dataMapMutex);
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
    d_port = d_cp_p->serverAddress(SERVER_ID)->portNumber();
    return src || lrc;
}

int ReadServer::stop()
{
    return d_cp_p->stop();
}

void ReadServer::poolCB(int state, int source, int severity)
{
}

void ReadServer::chanCB(int channelId, int serverId, int state, void *arg)
{
    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(d_coutMutex);
        bsl::cout << "Channel state callback called with"
                  << " Channel Id: " << channelId
                  << " Server Id: "  << serverId
                  << " State: " << state << bsl::endl;
    }

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        d_cp_p->shutdown(channelId, btemt_ChannelPool::BTEMT_IMMEDIATE);
      } break;
      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        DataReader *r = reader(channelId, true);
        ASSERT(r);
      } break;
    }
}

void ReadServer::blobBasedReadCb(int             *needed,
                                 bcema_Blob      *msg,
                                 int              channelId,
                                 void            *arg)
{
    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(d_coutMutex);
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

void ReadServer::pbcBasedReadCb(int                  *numConsumed,
                                int                  *needed,
                                const btemt_DataMsg&  msg,
                                void                 *arg)
{
    if (veryVerbose) {
        bcemt_LockGuard<bcemt_Mutex> guard(d_coutMutex);
        bsl::cout << "PBC Based Read Cb called with"
                  << " Channel Id: " << msg.channelId()
                  << " of length: "  << msg.data()->length() << bsl::endl;
    }

    ASSERT(numConsumed);
    ASSERT(needed);

    DataReader *r = reader(msg.channelId());
    ASSERT(r);
    r->pbcBasedReadCb(numConsumed, needed, msg, arg);
}

// MANIPULATORS
DataReader *ReadServer::reader(int channelId, bool createNew)
{
    int msgId;
    bcemt_LockGuard<bcemt_Mutex> guard1(&d_channelMapMutex);
    ChannelMap::const_iterator citer = d_channelMap.find(channelId);
    if (citer == d_channelMap.end()) {
        if (!createNew) {
            bcemt_LockGuard<bcemt_Mutex> guard(d_coutMutex);
            bsl::cout << "Channel up callback called with"
                      << " Channel Id: " << channelId
                      << " which does not exist" << bsl::endl;
            return 0;
        }
        else {
            msgId = d_channelMap.size();
            d_channelMap[channelId] = msgId;
        }
    }
    else {
        if (createNew) {
            bcemt_LockGuard<bcemt_Mutex> guard(d_coutMutex);
            bsl::cout << "Channel up callback called with"
                      << " Channel Id: " << channelId
                      << " which is already up" << bsl::endl;
            return 0;
        }
        else {
            msgId = citer->second;
        }
    }
    guard1.release()->unlock();

    DataReader *reader = 0;
    bcemt_LockGuard<bcemt_Mutex> guard2(&d_dataMapMutex);
    DataMap::iterator iter = d_dataMap.find(msgId);
    if (iter == d_dataMap.end()) {
        if (!createNew) {
            bcemt_LockGuard<bcemt_Mutex> guard(d_coutMutex);
            bsl::cout << "Channel up callback called with"
                      << " Channel Id: " << channelId
                      << " which does not exist" << bsl::endl;
            return 0;
        }
        else {
            reader = new (*d_allocator_p) DataReader();
            d_dataMap[msgId] = reader;
        }
    }
    else {
        if (createNew) {
            bcemt_LockGuard<bcemt_Mutex> guard(d_coutMutex);
            bsl::cout << "Channel up callback called with"
                      << " Channel Id: " << channelId
                      << " which is already up" << bsl::endl;
            return 0;
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
    bcemt_LockGuard<bcemt_Mutex> guard(&d_dataMapMutex);
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
    return d_cp_p->serverAddress(SERVER_ID)->portNumber();
}

DataReader *ReadServer::dataReader(int msgIdx)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_dataMapMutex);
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
                                 bcema_Blob *msg,
                                 int         channelId,
                                 void       *context)
{
    if (veryVerbose) {
        MTCOUT << "Blob based callback called with "
               << "for Channel Id: " << channelId
               << " with data of length: " << msg->length()
               << MTENDL;
    }

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (-1 == d_msgId) {
        if (msg->length() < (int) sizeof(int)) {
            *numNeeded = sizeof(int);
            return;                                                   // RETURN
        }

        bdex_ByteStreamImpUtil::getInt32(&d_msgId, msg->buffer(0).data());
        ASSERT(0 <= d_msgId);
        bcema_BlobUtil::erase(msg, 0, sizeof(int));

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

        bdex_ByteStreamImpUtil::getInt32(&d_msgLength,
                                         msg->buffer(0).data());
        ASSERT(0 <= d_msgLength);
        bcema_BlobUtil::erase(msg, 0, sizeof(int));

        if (0 == msg->length()) {
            *numNeeded = d_msgLength;
            return;                                               // RETURN
        }
    }

    bsl::string msgData;

    const int numDataBufs = msg->numDataBuffers();
    for (int i = 0; i < numDataBufs - 1; ++i) {
        msgData.append(msg->buffer(i).data(), msg->buffer(i).size());
    }
    msgData.append(msg->buffer(numDataBufs - 1).data(),
                   msg->lastDataBufferLength());

    bcema_BlobUtil::erase(msg, 0, msg->length());
    d_data.append(msgData);
    const int numRemaining = d_msgLength - d_data.size();
    *numNeeded = 1;
}

void DataReader::pbcBasedReadCb(int                  *numConsumed,
                                int                  *numNeeded,
                                const btemt_DataMsg&  msg,
                                void                 *context)
{
    if (veryVerbose) {
        MTCOUT << "PBC based callback called with "
               << "for Channel Id: " << msg.channelId()
               << " with data of length: " << msg.data()->length()
               << MTENDL;
    }

    bcema_PooledBufferChain *chain = msg.data();
    ASSERT(chain);

    const int bufSize    = chain->bufferSize();
    const int numBuffers = (chain->length() / bufSize) + 1;
    int       bufOffset  = 0;
    int       bufIdx     = 0;
    *numConsumed         = 0;

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (-1 == d_msgId) {
        if (chain->length() < (int) sizeof(int)) {
            *numNeeded = sizeof(int);
            return;                                                   // RETURN
        }
        bdex_ByteStreamImpUtil::getInt32(&d_msgId, chain->buffer(bufIdx));
        ASSERT(0 <= d_msgId);

        if (veryVerbose) {
            MTCOUT << "Msg Id: " << d_msgId << MTENDL;
        }

        bufOffset += sizeof(int);
        *numConsumed += sizeof(int);
        if (bufOffset >= bufSize) {
            ++bufIdx;
            bufOffset = 0;
        }

        if (*numConsumed == chain->length()) {
            *numNeeded = sizeof(int);
            return;                                               // RETURN
        }
    }

    if (-1 == d_msgLength) {
        if (chain->length() < *numConsumed + (int) sizeof(int)) {
            *numNeeded = sizeof(int) - (chain->length() - *numConsumed);
            return;                                                   // RETURN
        }
        ASSERT(bufSize >= bufOffset + (int) sizeof(int));

        bdex_ByteStreamImpUtil::getInt32(&d_msgLength,
                                        chain->buffer(bufIdx) + bufOffset);
        ASSERT(0 <= d_msgLength);
        if (veryVerbose) {
            MTCOUT << "Msg Length: " << d_msgLength << MTENDL;
        }

        bufOffset += sizeof(int);

        *numConsumed += sizeof(int);
        if (bufOffset >= bufSize) {
            ++bufIdx;
            bufOffset = 0;
        }

        if (*numConsumed == chain->length()) {
            *numNeeded = d_msgLength;
            return;                                               // RETURN
        }
    }

    bsl::string msgData;
    if (bufOffset >= bufSize) {
        ++bufIdx;
        bufOffset = 0;
    }

    int remaining = chain->length() - *numConsumed;
    if (d_msgLength < remaining) {
        remaining = d_msgLength;
    }

    while (remaining) {
        int currSize = remaining > bufSize - bufOffset
                     ? bufSize - bufOffset : remaining;
        msgData.append(chain->buffer(bufIdx) + bufOffset, currSize);
        remaining -= currSize;
        bufOffset += currSize;
        *numConsumed += currSize;
        if (bufOffset >= bufSize) {
            ++bufIdx;
            bufOffset = 0;
        }
    }

    d_data.append(msgData);

    if (veryVerbose) {
        MTCOUT << d_data << MTENDL;
    }

    const int numRemaining = d_msgLength - d_data.size();
    *numNeeded = 1;
}

// ACCESSORS
const bsl::string& DataReader::data() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_data;
}

int DataReader::msgId() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_msgId;
}

int DataReader::msgLength() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_msgLength;
}

bool DataReader::done() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return (int) d_data.size() == d_msgLength;
}

                    // ==============
                    // class TestData
                    // ==============

struct TestData {
    // This 'struct' represents the data passed the testing thread.

    int                                     d_threadIntId; // integer thread id
    bsl::string                             d_expData;  // expected data
    bool                                    d_useBlobs; // use blobs
    bteso_IPv4Address                       d_address;  // ip address
    bteso_StreamSocket<bteso_IPv4Address>  *d_socket_p; // socket to write to
    bcemt_Barrier                          *d_barrier_p;// barrier

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
    bdex_ByteStreamImpUtil::putInt32(data, d_threadIntId);
    offset += sizeof(int);

    bdex_ByteStreamImpUtil::putInt32(data + offset, DATA_SIZE);
    offset += sizeof(int);

    const int TOTAL_SIZE = DATA_SIZE + offset;

    bsl::memcpy(data + offset, d_expData.data(), DATA_SIZE);

    int incr = 1, written = 0;
    offset = 0;
    while (written < TOTAL_SIZE) {
        LOOP2_ASSERT(d_threadIntId, incr,
                     incr == d_socket_p->write(data + offset,
                                               incr));
        written += incr;
        offset  += incr;
        if (written + incr + 1 > TOTAL_SIZE) {
            incr = TOTAL_SIZE - written;
        }
        else {
            ++incr;
        }
        bcemt_ThreadUtil::microSleep(100);
    }
}

}  // namespace TEST_CASE_30_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 28: 'btemt_ChannelPool::busyMetrics()'
//-----------------------------------------------------------------------------

namespace TEST_CASE_BUSY_METRICS {

void dummyDataCallbackWithDelay(int                  *numConsumed,
                                int                  *numNeeded,
                                const btemt_DataMsg&  message,
                                void                 *context,
                                double                delayS)
{
    bcemt_ThreadUtil::sleep(bdet_TimeInterval(delayS));
    *numNeeded   = 1;
    *numConsumed = message.data()->length();
}

}  // namespace TEST_CASE_BUSY_METRICS
//-----------------------------------------------------------------------------
// CASE 26: 'btemt_ChannelPool_MessageUtil'
//-----------------------------------------------------------------------------
namespace TEST_CASE_MESSAGEHELPER_NAMESPACE {

int createData(bsl::vector<bsl::vector<char> > *data,
               const  char                     *description)
    // Populate the specified 'data' with a series of characters whose lengths
    // are described by 'description', each array containing arbitrary data;
    // return the total number of bytes of data in 'data'.  The 'description'
    // must be a null-terminated string with a space separated list of lengths
    // (e.g., "1 2 3 4").  The arbitrary data will begin with the byte 0xFF
    // and decrease by 1 per element.
{
    int length = 0;
    bsl::vector<int> lengths;
    for (bdeut_StrTokenIter it(description, " "); it; ++it) {
        lengths.push_back(atoi(it()));
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

template <typename IOVEC>
void loadIovecs(bsl::vector<IOVEC>                *iovecs,
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

void loadBlob(bcema_Blob                       *blob,
              bsl::vector<bsl::vector<char> >&  data)
    // Load into the specified 'blob' the specified 'data' buffers.
{
    bsl::vector<bsl::vector<char> >::iterator srcIt  = data.begin();
    for (; srcIt != data.end(); ++srcIt) {
        bcema_SharedPtr<char> sharedData(&srcIt->front(),
                                         bcema_SharedPtrNilDeleter(),
                                         0);
        bcema_BlobBuffer buffer(sharedData, srcIt->size());
        blob->appendDataBuffer(buffer);
    }
}

void loadBufferChain(bcema_PooledBufferChain          *chain,
                     bsl::vector<bsl::vector<char> >&  data)
    // Load into the specified 'chain' the specified 'data' buffers.
{
    bsl::vector<bsl::vector<char> >::iterator srcIt  = data.begin();
    for (; srcIt != data.end(); ++srcIt) {
        chain->append(&srcIt->front(), srcIt->size());
    }
}

bool isEqualByteSequence(
                const bsl::vector<bsl::vector<char> >& expectedSequence,
                int                                    expectedSequenceOffset,
                const bcema_Blob&                      blob,
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
        const bcema_BlobBuffer&   buffer     = blob.buffer(blobIndex);
        const char               *bufferData = buffer.buffer().ptr();
        const bsl::vector<char>&  array      = expectedSequence[arrayIndex];

        while (blobOffset < (int) buffer.size() &&
                                            arrayOffset < (int) array.size()) {
            if (bufferData[blobOffset] != array[arrayOffset]) {
                bsl::cout << "Unequal bytes found at: ";
                P_(blobPosition); P_(blobIndex);
                P_(blobOffset);   P(blob.length());
                P_(arrayIndex);   P_(arrayOffset);
                P_(array.size())  P(expectedSequence.size());
                return false;
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
        return true;
    }
    bsl::cout << "Unqueal end of buffer: ";
    P_(blobPosition); P_(blobIndex);
    P_(blobOffset);   P(blob.length());
    P_(arrayIndex);   P_(arrayOffset);
    P(expectedSequence.size());
    return false;
}

template <typename IOVEC>
int loadTestVector(bsl::vector<IOVEC>  *vector,
                   bsl::vector<IOVEC>&  srcData,
                   const char          *description)
    // Load into the specified 'vector' elements from the specified 'srcData'
    // as indicated by the specified 'description'.  The 'description'
    // must be a null-terminated string with a space separated list of indices
    // into the 'srcData' vector (e.g., "1 2 3 4").  The behavior is undefined
    // unless each numeric element in 'description' is in the range
    // [0, 'srcData.size()').
{
    int length = 0;
    for (bdeut_StrTokenIter it(description, " "); it; ++it) {
        int index = atoi(it());
        vector->push_back(srcData[index]);
        length += srcData[index].length();
    }
    return length;

}

template <typename IOVEC>
void testIovecArray()
    // Perform the standard value-semantic tests on a
    // 'btemt_ChannelPool_IovecArray<IOVEC>'.
{
    typedef btemt_ChannelPool_MessageUtil     Helper;
    typedef btemt_ChannelPool_IovecArray<IOVEC> IovecArray;

    bslma_TestAllocator ta;

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

} // close namespace TEST_CASE_25_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 25 'setWriteCacheHighWatermark()'
//-----------------------------------------------------------------------------
namespace TEST_CASE_25_NAMESPACE {

int drainSocket(bteso_StreamSocket<bteso_IPv4Address> *clientSocket,
                int                                    numBytesExpected)
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
    LOOP2_ASSERT(numBytesRead, numBytesExpected,
                                             numBytesRead == numBytesExpected);
    return numBytesRead;
}

enum {
    CASE_25_NUM_THREADS = 5
};

class TestCase25ConcurrencyTest {

     typedef bteso_StreamSocket<bteso_IPv4Address> Socket;
     btemt_ChannelPool * const d_pool_p;
     const int                 d_channelId;
     Socket * const            d_clientSocket_p;
     const int                 d_numThreads;
     bces_AtomicInt            d_numBytesWritten;
     bces_AtomicInt            d_done;
     bcemt_Barrier             d_barrier;
     bcep_FixedThreadPool      d_threadPool;

     void executeTest();
        // Perform the concurrency test: Write data to 'd_pool_p' and modify
        // the write-cache of 'd_pool_p', increment 'd_numBytesWritten' with
        // the number of bytes written (as they are written), and increment
        // 'd_done' to indicate the operation has completed.

  public:

    TestCase25ConcurrencyTest(
                         btemt_ChannelPool                     *pool,
                         int                                    channelId,
                         bteso_StreamSocket<bteso_IPv4Address> *clientSocket,
                         int                                    numThreads,
                         bslma_Allocator                       *allocator = 0);

    void run();
        // Run 'executeTest' on multiple threads.

};

TestCase25ConcurrencyTest::TestCase25ConcurrencyTest(
                          btemt_ChannelPool                      *pool,
                          int                                    channelId,
                          bteso_StreamSocket<bteso_IPv4Address> *clientSocket,
                          int                                    numThreads,
                          bslma_Allocator                       *allocator)
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

void TestCase25ConcurrencyTest::executeTest()
{
    enum { HI_WATERMARK = 1096,
           NUM_BYTES    = HI_WATERMARK * 25 };

    int rc = 0, totalBytesWritten = 0;
    bcema_Blob oneByteMsg;
    populateMessage(&oneByteMsg, 1, bslma_Default::allocator(0));

    d_barrier.wait();

    d_pool_p->setWriteCacheHiWatermark(d_channelId, HI_WATERMARK);
    while (totalBytesWritten < NUM_BYTES) {
        int currentBytesWritten = 0;
        while (currentBytesWritten < (HI_WATERMARK / 4) &&
               0 == (rc = d_pool_p->write(d_channelId, oneByteMsg))) {
            currentBytesWritten += oneByteMsg.length();
            totalBytesWritten   += oneByteMsg.length();
            d_numBytesWritten   += oneByteMsg.length();
        }

        d_pool_p->setWriteCacheHiWatermark(d_channelId, 2 * HI_WATERMARK);

        currentBytesWritten = 0;
        while (currentBytesWritten < (HI_WATERMARK / 4) &&
               0 == (rc = d_pool_p->write(d_channelId, oneByteMsg))) {
            currentBytesWritten += oneByteMsg.length();
            totalBytesWritten   += oneByteMsg.length();
            d_numBytesWritten   += oneByteMsg.length();
        }

        d_pool_p->setWriteCacheHiWatermark(d_channelId, HI_WATERMARK);
    }
    ++d_done;
}

void TestCase25ConcurrencyTest::run()
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
    bdef_Function<void(*)()> job = bdef_BindUtil::bind(
                                    &TestCase25ConcurrencyTest::executeTest,
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
            bcemt_ThreadUtil::microSleep(10);
        }
    }
    d_threadPool.drain();
    bcemt_ThreadUtil::microSleep(100);
    ASSERT(numBytesRead == (int)d_numBytesWritten);
    if (veryVerbose) {
        PT2((int)d_numBytesWritten, numBytesRead);
    }
}

void testCase25ConcurrencyTest(btemt_ChannelPool *pool,
                               int                channelId,
                               bcemt_Barrier     *barrier,
                               bces_AtomicInt    *numWritten,
                               bces_AtomicInt    *done)
{
    enum { HI_WATERMARK = 1024,
           NUM_BYTES    = HI_WATERMARK * 10 };

    int rc = 0, totalBytesWritten = 0;
    bcema_Blob oneByteMsg;
    populateMessage(&oneByteMsg, 1, bslma_Default::allocator(0));

    barrier->wait();

    pool->setWriteCacheHiWatermark(channelId, HI_WATERMARK);
    while (totalBytesWritten < NUM_BYTES) {
        int currentBytesWritten = 0;
        while (currentBytesWritten < (HI_WATERMARK / 4) &&
               0 == (rc = pool->write(channelId, oneByteMsg))) {
            currentBytesWritten += oneByteMsg.length();
            totalBytesWritten   += oneByteMsg.length();
            (*numWritten)       += oneByteMsg.length();
        }

        pool->setWriteCacheHiWatermark(channelId, 2 * HI_WATERMARK);

        currentBytesWritten = 0;
        while (currentBytesWritten < (HI_WATERMARK / 4) &&
               0 == (rc = pool->write(channelId, oneByteMsg))) {
            currentBytesWritten += oneByteMsg.length();
            totalBytesWritten   += oneByteMsg.length();
            (*numWritten)       += oneByteMsg.length();
        }

        pool->setWriteCacheHiWatermark(channelId, HI_WATERMARK);
    }
    (*done)++;
}

} // closing namespace TEST_CASE_12_NAMESPACE

//-----------------------------------------------------------------------------
// CASE -2 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_N2_NAMESPACE {
static
void caseN2ChannelStateCallback(int                 channelId,
                               int                 serverId,
                               int                 state,
                               void               *arg,
                               btemt_ChannelPool **poolAddr,
                               int               **eventAddr,
                               bcemt_Barrier      *barrier,
                               int                *channelId_p)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btemt_ChannelPool *pool = *poolAddr;

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
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
}

//-----------------------------------------------------------------------------
// CASE 23 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_23_NAMESPACE {

const char MESSAGE[] = "MESSAGE\n";

struct case23CallbackInfo {
    bcemt_Barrier     *d_clientServerBarrier_p;
    bcemt_Barrier     *d_stateBarrier_p;
    bcemt_Barrier     *d_dataBarrier_p;
    bces_AtomicInt     d_channelId;
    bces_AtomicInt     d_channelDownReadFlag;
    bces_AtomicInt     d_channelDownWriteFlag;
    btemt_ChannelPool *d_channelPool_p;
    int                d_msgLength;
    bool               d_echoMode; // differentiates server thread
};

#if 0
static
void case23ChannelStateCallback(int                 channelId,
                                int                 serverId,
                                int                 state,
                                void               *arg,
                                case23CallbackInfo *info)
{
    ASSERT(info);

    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN_READ: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated (read only):"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        ++info->d_channelDownReadFlag;
        info->d_stateBarrier_p->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN_WRITE: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated (write only):"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        ++info->d_channelDownWriteFlag;
        info->d_stateBarrier_p->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        info->d_stateBarrier_p->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        info->d_channelId = channelId;
        info->d_stateBarrier_p->wait();
      } break;
    }
}

static
void case23DataCallback(int                *numConsumed,
                        int                *numNeeded,
                        btemt_DataMsg       msg,
                        void               *context,
                        case23CallbackInfo *info)
{
    // This is an echo server, the clients merely receive the data but don't
    // echo it.  They do only minimal checking.  We need to adjust the
    // channelId just in case the channel has been shut down, in which case
    // channelId is 0 but oldChannelId still has the good data.

    MTASSERT(numConsumed);
    MTASSERT(msg.data());
    MTLOOP_ASSERT(msg.data()->length(), 0 < msg.data()->length());

    info->d_msgLength -= msg.data()->length();

    *numConsumed = msg.data()->length();
    *numNeeded   = info->d_msgLength;

    if (info->d_echoMode && 0 == info->d_msgLength) {
        // Delay the answer with the use of the barrier.
        info->d_dataBarrier_p->wait();

        int retCode = info->d_channelPool_p->write(msg.channelId(), msg);
        MTLOOP_ASSERT(retCode, 0 == retCode ||
                              -2 == retCode ||  // reached high-watermark
                              -3 == retCode);   // channel shut down
        if (0 == retCode) {
            if (veryVerbose) {
                MTCOUT << "\t\tServer echoing back " << *numConsumed
                       << " bytes on channel " << msg.channelId() << MTENDL;
            }
        }
        else {
            if (veryVerbose) {
                MTCOUT << "\t\tServer dropped " << *numConsumed
                       << " bytes on channel " << msg.channelId() << MTENDL;
            }
        }
    }
}
#endif

extern "C"
void *case23ClientThread3(void *arg)
{
    case23CallbackInfo *info = (case23CallbackInfo*)arg;

    info->d_stateBarrier_p->wait(); // for CHANNEL_UP
    MTASSERT(-1 != info->d_channelId);

    btes_Ovec msg[1];
    *msg = btes_Ovec(MESSAGE, sizeof(MESSAGE));
    ASSERT(0 == info->d_channelPool_p->write(info->d_channelId, msg, 1));

    info->d_clientServerBarrier_p->wait(); // for server to send response

    ASSERT(0 == info->d_channelPool_p->shutdown(
                                             info->d_channelId,
                                             bteso_Flag::BTESO_SHUTDOWN_SEND));
    info->d_stateBarrier_p->wait(); // for CHANNEL_DOWN_WRITE
    MTASSERT(1 == info->d_channelDownWriteFlag);

    info->d_clientServerBarrier_p->wait(); // for server to start closing

    info->d_stateBarrier_p->wait(); // for CHANNEL_DOWN_READ
    MTASSERT(1 == info->d_channelDownReadFlag);

    info->d_stateBarrier_p->wait(); // for CHANNEL_DOWN
    MTASSERT(1 == info->d_channelDownReadFlag);
    MTASSERT(1 == info->d_channelDownWriteFlag);

    return 0;
}

extern "C"
void *case23ServerThread3(void *arg)
{
    case23CallbackInfo *info = (case23CallbackInfo*)arg;

    info->d_stateBarrier_p->wait(); // for CHANNEL_UP
    MTASSERT(-1 != info->d_channelId);

    info->d_dataBarrier_p->wait(); // for server dataCb's reply

    info->d_clientServerBarrier_p->wait(); // enable client to close channel

    // The data callback will close the channel, detected b/c
    // server connection is read-enabled.
    info->d_stateBarrier_p->wait(); // for CHANNEL_DOWN_READ
    MTASSERT(1 == info->d_channelDownReadFlag);

    info->d_clientServerBarrier_p->wait(); // for server to send response

    MTASSERT(0 == info->d_channelPool_p->shutdown(
                                             info->d_channelId,
                                             bteso_Flag::BTESO_SHUTDOWN_SEND));

    info->d_stateBarrier_p->wait(); // for CHANNEL_DOWN_WRITE
    MTASSERT(1 == info->d_channelDownReadFlag);

    info->d_stateBarrier_p->wait(); // for CHANNEL_DOWN
    MTASSERT(1 == info->d_channelDownReadFlag);

    return 0;
}

} // closing namespace TEST_CASE_23_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 22 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_22_NAMESPACE {

const int MAGIC          = 0x0ABCDEF0;

struct case22ThreadInfo {
    bcemt_Barrier                  *d_barrier_p;
    Obj                            *d_channelPool_p;
    bteso_IPv4Address               d_serverAddress;
    bool                            d_isLeaderFlag;
    int                             d_groupId;
    bces_AtomicInt                 *d_channelId;
    bces_AtomicInt                 *d_oldChannelId;
    int                             d_bufferAlloc;
    int                             d_totalAlloc;
    int                             d_numIters;
    char                            d_write;
    bcema_PooledBufferChainFactory *d_factory_p;
    int                             d_messageLength;
    bslma_Allocator                *d_allocator_p;
};

extern "C" {
void * case22Thread(void * arg)
{
    case22ThreadInfo *info = (case22ThreadInfo *) arg;
    const int BUF_SIZE       = info->d_bufferAlloc;
    const int MSG_LENGTH     = info->d_messageLength;
    const int TOTAL_SIZE     = MSG_LENGTH ? MSG_LENGTH : info->d_totalAlloc;
    const int NUM_ITERATIONS = info->d_numIters;
    const char PAYLOAD_CHAR  = info->d_write;

    int retCode;
    btemt_ChannelPool *mXp = info->d_channelPool_p;
    const bteso_IPv4Address ADDRESS = info->d_serverAddress;

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

    // Segment buffer into DataMsg of individual buffers of BUF_SIZE.
    bcema_PooledBufferChain *chain = info->d_factory_p->allocate(0);
    for (int i = 0 ;i < numVecs; ++i) {
        chain->append(buffer + i * BUF_SIZE,
                      (i < numVecs-1) ? BUF_SIZE : LAST_VEC_SIZE);
    }

    // If keeping track of message length, put it into the first eight bytes.
    btemt_DataMsg msg;
    msg.setData(chain, info->d_factory_p, info->d_allocator_p);
    info->d_allocator_p->deallocate(buffer);

    if (info->d_isLeaderFlag) {
        if (verbose) {
            MTCOUT << "\tInitial connection: channelId=" << *info->d_channelId
                   << ", groupId=" << info->d_groupId << MTENDL;
        }
        retCode = mXp->connect(ADDRESS,
                               100, bdet_TimeInterval(1.0), // 1s
                               info->d_groupId);
        MTLOOP_ASSERT(retCode, 0 == retCode);
        bcemt_ThreadUtil::yield();
        bcemt_ThreadUtil::microSleep(0, 1); // 1s
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
                                        btemt_ChannelPool::BTEMT_IMMEDIATE);
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
                        bcemt_ThreadUtil::yield();
                        bcemt_ThreadUtil::microSleep(10000); // 10ms
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

        msg.setChannelId(channelId);
        retCode = mXp->write(channelId, msg);
        MTLOOP_ASSERT(retCode,
                       0 == retCode ||  // o.k.
                      -2 == retCode ||  // reached high-watermark
                      -3 == retCode);   // channel down or unknown

        if (veryVerbose) {
            if (0 == retCode) {
                MTCOUT << "\t\tWrote " << msg.data()->length()
                       << " bytes to channel " << channelId << MTENDL;
            }
            else {
                MTCOUT << "\t\tDroppped " << msg.data()->length()
                       << " bytes to channel " << channelId << MTENDL;
            }
        }
    }
    return 0;
}
} // closing extern "C"

static
void case22PoolStateCallback(
    int             state,
    int             sourceId,
    int             severity)
{
    switch (state) {
      case btemt_PoolMsg::BTEMT_ACCEPT_TIMEOUT: {
        if (verbose) {
            MTCOUT << "\tAccept timed out:"
                   << " sourceId=" << sourceId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case btemt_PoolMsg::BTEMT_ERROR_CONNECTING: {
        if (verbose) {
            MTCOUT << "\tError Connecting:"
                   << " sourceId=" << sourceId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case btemt_PoolMsg::BTEMT_ERROR_ACCEPTING: {
        if (verbose) {
            MTCOUT << "\tError Accepting:"
                   << " sourceId=" << sourceId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case btemt_PoolMsg::BTEMT_CHANNEL_LIMIT: {
        if (verbose) {
            MTCOUT << "\tChannel Limit Reached:"
                   << " sourceId=" << sourceId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
    }
}

struct case22CallbackInfo {
    Obj               **d_channelPool_p;
    bteso_IPv4Address   d_serverAddress;
    bces_AtomicInt     *d_channelId1;
    bces_AtomicInt     *d_channelId2;
    bces_AtomicInt     *d_oldChannelId1;
    bces_AtomicInt     *d_oldChannelId2;
    char                d_payloadChar1;
    char                d_payloadChar2;
    int                 d_numNeeded;
    int                 d_msgSize;
};

static
void case22ChannelStateCallback(int                 channelId,
                                int                 sourceId,
                                int                 state,
                                void               *arg,
                                case22CallbackInfo *info)
{
    MTASSERT(info);
    btemt_ChannelPool        *mXp = *info->d_channelPool_p;
    const bteso_IPv4Address&  ADDRESS = info->d_serverAddress;
    const int                 oldChannelId1 = *info->d_oldChannelId1;
    const int                 oldChannelId2 = *info->d_oldChannelId2;

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (oldChannelId1 == channelId) {
            int retCode = mXp->connect(ADDRESS,
                                       10, bdet_TimeInterval(0.010), // 10ms
                                       0); // clientId first group
            MTLOOP_ASSERT(retCode, 0 == retCode);
        }
        else if (oldChannelId2 == channelId) {
            int retCode = mXp->connect(ADDRESS,
                                       10, bdet_TimeInterval(0.010), // 10ms
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
      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        MTASSERT(0 == mXp->setSocketOption(
                                        bteso_SocketOptUtil::BTESO_SENDBUFFER,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        32768, channelId));
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

void case22DataCallback(int                *numConsumed,
                        int                *numNeeded,
                        btemt_DataMsg       msg,
                        void               *context,
                        case22CallbackInfo *info)
{
    // This is an echo server, the clients merely receive the data but don't
    // echo it.  They do only minimal checking.  We need to adjust the
    // channelId just in case the channel has been shut down, in which case
    // channelId is 0 but oldChannelId still has the good data.

    btemt_ChannelPool  *mXp = *(info->d_channelPool_p);
    const int           channelId1 = *info->d_channelId1
                                   ? *info->d_channelId1
                                   : *info->d_oldChannelId1;
    const int           channelId2 = *info->d_channelId2
                                   ? *info->d_channelId2
                                   : *info->d_oldChannelId2;
    const char          PAYLOAD_CHAR1 = info->d_payloadChar1;
    const char          PAYLOAD_CHAR2 = info->d_payloadChar2;

    MTASSERT(numConsumed);
    MTASSERT(msg.data());
    MTLOOP_ASSERT(msg.data()->length(), 0 < msg.data()->length());

    if (msg.channelId() == channelId1 || msg.channelId() == channelId2) {
        if (0 < info->d_msgSize) {
            // Keep track of message length read by each thread.  The first
            // four bytes of a message indicate its length and must be equal to
            // MSG_LEN.  The remaining bytes must be equal to the PAYLOAD_CHAR.

            int length = msg.data()->length();
            const int MSG_LENGTH = info->d_msgSize;

            *numConsumed = 0;
            *numNeeded   = MSG_LENGTH;

            if (veryVerbose) {
                MTCOUT << "\t\tClient receiving back " << length
                       << " bytes on channel " << msg.channelId() << MTENDL;
            }

            while (length >= MSG_LENGTH) {
                union {
                    int d_msg;
                    char msg_buffer[sizeof(int)];
                } x;
                x.msg_buffer[0] = (*msg.data())[*numConsumed];
                x.msg_buffer[1] = (*msg.data())[*numConsumed + 1];
                x.msg_buffer[2] = (*msg.data())[*numConsumed + 2];
                x.msg_buffer[3] = (*msg.data())[*numConsumed + 3];
                MTLOOP2_ASSERT(msg.channelId(), x.d_msg,
                               MAGIC == x.d_msg);

                x.msg_buffer[0] = (*msg.data())[*numConsumed + 4];
                x.msg_buffer[1] = (*msg.data())[*numConsumed + 5];
                x.msg_buffer[2] = (*msg.data())[*numConsumed + 6];
                x.msg_buffer[3] = (*msg.data())[*numConsumed + 7];
                MTLOOP2_ASSERT(msg.channelId(), x.d_msg,
                               MSG_LENGTH == x.d_msg);

                const char PAYLOAD_CHAR = (msg.channelId() == channelId1)
                                        ? PAYLOAD_CHAR1 : PAYLOAD_CHAR2;

                for (int i = 8; i < MSG_LENGTH; ++i) {
                    MTLOOP2_ASSERT(msg.channelId(), i,
                              PAYLOAD_CHAR == (*msg.data())[*numConsumed + i]);

                    if (PAYLOAD_CHAR != (*msg.data())[i]) {
                        x.msg_buffer[0] = (*msg.data())[*numConsumed + i];
                        x.msg_buffer[1] = (*msg.data())[*numConsumed + i + 1];
                        x.msg_buffer[2] = (*msg.data())[*numConsumed + i + 2];
                        x.msg_buffer[3] = (*msg.data())[*numConsumed + i + 3];
                        if (MAGIC == x.d_msg) {
                            MTLOOP2_ASSERT(msg.channelId(), i,
                                           0 == "Client receiving MAGIC word");
                        }
                        if (verbose) {
                            PT3(__LINE__, (int)PAYLOAD_CHAR,
                                          (int)(*msg.data())[i]);
                        }
                        *numConsumed += i;
                        return;
                    }
                }

                *numConsumed += MSG_LENGTH;  // but not if we return early
                length -= MSG_LENGTH;
            }
            *numNeeded = MSG_LENGTH - length;
        }
        else {
            // In this mode, numNeeded is driven by the test case, always,
            // regardless of how many messages we have read, and of what
            // length.  Also, we always expect the char read to be the
            // PAYLOAD_CHAR.

            *numConsumed = msg.data()->length();
            *numNeeded = info->d_numNeeded;

            if (veryVerbose) {
                MTCOUT << "\t\tClient receiving back " << *numConsumed
                       << " bytes on channel " << msg.channelId() << MTENDL;
            }
            const char PAYLOAD_CHAR = (msg.channelId() == channelId1)
                                    ? PAYLOAD_CHAR1 : PAYLOAD_CHAR2;
            for (int i = 0; i < *numConsumed; ++i) {
                MTLOOP2_ASSERT(msg.channelId(), i,
                                             PAYLOAD_CHAR == (*msg.data())[i]);
                if (PAYLOAD_CHAR != (*msg.data())[i]) {
                    if (verbose) {
                        PT3(__LINE__, (int)PAYLOAD_CHAR,
                                      (int)(*msg.data())[i]);
                    }
                    return;
                }
            }
        }
    }
    else {
        *numConsumed = msg.data()->length();
        *numNeeded = info->d_numNeeded;

        int retCode = mXp->write(msg.channelId(), msg);
        MTLOOP_ASSERT(retCode, 0 == retCode ||
                              -2 == retCode ||  // reached high-watermark
                              -3 == retCode);   // channel shut down
        if (0 == retCode) {
            if (veryVerbose) {
                MTCOUT << "\t\tServer echoing back " << *numConsumed
                       << " bytes on channel " << msg.channelId() << MTENDL;
            }
        }
        else {
            int retCode = mXp->shutdown(msg.channelId(),
                                        btemt_ChannelPool::BTEMT_IMMEDIATE);
            MTLOOP_ASSERT(retCode, 0 == retCode || -1 == retCode);
            if (verbose) {
                MTCOUT << "\tServer connection closed:"
                       << " channelId=" << msg.channelId()
                       << " numChannels=" << mXp->numChannels()
                       << " shutdown=" << retCode
                       << MTENDL;
            }
            if (veryVerbose) {
                MTCOUT << "\t\tServer dropped " << *numConsumed
                       << " bytes on channel " << msg.channelId() << MTENDL;
            }
        }

    }
}

static
void runTestCase22(char                                         *progname,
                   int                                           numThreads,
                   int                                           numIters,
                   int                                           bufferSize,
                   int                                           msgSize,
                   int                                           serverSize,
                   int                                           serverNeeded,
                   bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                   bcema_TestAllocator                          *allocator)
{
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Test Initialization

    enum {
        SERVER_ID           = 1013410001,
        BACKLOG             = 100,
        REUSE_ADDRESS       = 1,
        PAYLOAD_CHAR1       = '\xb2',  // -78
        PAYLOAD_CHAR2       = '\x5c'   //  92
    };
    const int NUM_THREADS   = numThreads;
    const int NUM_ITERS     = numIters;
    const int BUFFER_ALLOC  = bufferSize;
    const int RECEIVE_ALLOC = serverSize;
    const int SEND_ALLOC    = msgSize > 0 ? msgSize : - msgSize;

    // Channel pool configuration.

    const bteso_IPv4Address ADDRESS("127.0.0.1", 0);

    btemt_ChannelPool *poolAddr;
    bces_AtomicInt channelId1 = 0;
    bces_AtomicInt channelId2 = 0;
    bces_AtomicInt oldChannelId1 = 0;
    bces_AtomicInt oldChannelId2 = 0;

    case22CallbackInfo info;
    info.d_channelPool_p = &poolAddr;
    info.d_serverAddress = ADDRESS;
    info.d_channelId1    = &channelId1;
    info.d_channelId2    = &channelId2;
    info.d_oldChannelId1 = &oldChannelId1;
    info.d_oldChannelId2 = &oldChannelId2;
    info.d_payloadChar1  = (char)PAYLOAD_CHAR1;
    info.d_payloadChar2  = (char)PAYLOAD_CHAR2;
    info.d_numNeeded     = serverNeeded;
    info.d_msgSize       = msgSize < 0 ? -msgSize : 0;

    btemt_ChannelPool::ChannelStateChangeCallback channelCb(
            bdef_BindUtil::bindA( allocator
                                , &case22ChannelStateCallback
                                , _1, _2, _3, _4
                                , &info));

    btemt_ChannelPool::DataReadCallback  dataCb(
            bdef_BindUtil::bindA( allocator
                                , &case22DataCallback
                                , _1, _2, _3, _4
                                , &info));

    btemt_ChannelPool::PoolStateChangeCallback poolCb( &case22PoolStateCallback
                                                     , allocator);

    btemt_ChannelPoolConfiguration cpc;
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

    bcema_PooledBufferChainFactory bufferFactory(BUFFER_ALLOC, allocator);

    // Starting server.
    {
        Obj mX(channelCb, dataCb, poolCb, cpc, allocator);  const Obj& X = mX;
        poolAddr = &mX;

        ASSERT(0 == mX.start());
        ASSERT(0 == mX.listen(ADDRESS, BACKLOG, SERVER_ID, REUSE_ADDRESS));
        ASSERT(0 == X.numChannels());
        const bteso_IPv4Address PEER = *mX.serverAddress(SERVER_ID);
        info.d_serverAddress = PEER;

        ASSERT(0 == mX.setServerSocketOption(
                            bteso_SocketOptUtil::BTESO_SENDBUFFER,
                            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                            RECEIVE_ALLOC, SERVER_ID));
        ASSERT(0 == mX.setServerSocketOption(
                            bteso_SocketOptUtil::BTESO_RECEIVEBUFFER,
                            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                            RECEIVE_ALLOC, SERVER_ID));

        bcemt_ThreadUtil::Handle *threads = (bcemt_ThreadUtil::Handle*)
           allocator->allocate(sizeof(bcemt_ThreadUtil::Handle) * NUM_THREADS);
        case22ThreadInfo *info = (case22ThreadInfo*)
                   allocator->allocate(sizeof(case22ThreadInfo) * NUM_THREADS);
        bcemt_Barrier barrier(NUM_THREADS + 1);
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

            ASSERT(0  == bcemt_ThreadUtil::create(&threads[i],
                                                  &case22Thread,
                                                  &info[i]));
        }

        barrier.wait();

        // Give some time for channel pool to read all the data, then stop.

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
        }

        if (verbose) {
            MTCOUT << "\tShutting down channel pool:" << MTFLUSH;

            bsls_PlatformUtil::Int64 numBytesRead;
            bsls_PlatformUtil::Int64 numBytesWritten;
            bsls_PlatformUtil::Int64 numBytesRequestedToBeWritten;

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

} // closing namespace TEST_CASE_22_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 21 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_21_NAMESPACE {

template <class ADDRESS>
class bteso_SslLikeStreamSocketFactory;

                       // =========================
                       // class SslLikeStreamSocket
                       // =========================

template <class ADDRESS>
class bteso_SslLikeStreamSocket : public bteso_StreamSocket<ADDRESS> {
    // This class implements the 'bteso_StreamSocket<ADDRESS>' protocol to
    // provide stream-based socket communications.  It holds an underlying
    // socket to which it forwards all function calls.  The only addition to
    // the 'bteso_StreamSocket' is that writing and reading go through an
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
    bteso_StreamSocket<ADDRESS> *d_socket_p;   // held, not owned

    int              d_bufferSize;            // internal buffers size

    char            *d_readBuffer;            // internal read buffer (owned)

    int              d_readBytesAvailable;    // number of data bytes in
                                              // internal read buffer

    int              d_maxReadBytesAvailable; // number of data bytes that
                                              // should be read until the
                                              // read limit is reached (or
                                              // -1 if there is no read limit)

    char            *d_writeBuffer;           // internal write buffer (owned)

    int              d_writeOffset;           // offset for next write

    int              d_maxWriteBytes;         // maximum number of bytes this
                                              // socket will accept for write
                                              // until write limit is reached
                                              // (or -1 if there is no write
                                              // limit)

    bslma_Allocator *d_allocator_p;           // held, not owned

    friend class bteso_SslLikeStreamSocketFactory<ADDRESS>;

    // CREATORS
    bteso_SslLikeStreamSocket(int                          internalBufferSize,
                              bteso_StreamSocket<ADDRESS> *socket,
                              bslma_Allocator             *allocator);
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
    virtual int accept(bteso_StreamSocket<ADDRESS> **result);
    virtual int accept(bteso_StreamSocket<ADDRESS> **socket,
                       ADDRESS                      *peerAddress );
        // Return the result of calling 'accept' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int bind(const ADDRESS& address);
        // Return the result of calling 'bind' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int connect(const ADDRESS& address);
        // Return the result of calling 'connect' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int listen(int backlog);
        // Return the result of calling 'listen' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int read(char *buffer, int length);
        // Copy the specified 'length' bytes from the internal read buffer to
        // the specified 'buffer', or the number of available bytes in the
        // internal read buffer if this number is smaller.  Return the number
        // of bytes copied.  If the internal read buffer is exhausted, first
        // attempt to refill it by calling 'read' on the underlying
        // 'bteso_StreamSocket' object, and return the error code if that call
        // returns an error code.  In particular, if there is no data in the
        // internal buffer, either block (in blocking mode) or return
        // 'ERROR_WOULDBLOCK' if reading on the underlying socket would block.

    virtual int readv(const btes_Iovec *buffers, int numBuffers);
        // Fill in the up to 'numBuffers' specified 'buffers' with data from
        // the internal read buffer, up to the number of bytes available in the
        // internal read buffer if this number is smaller that the total size
        // of the 'buffers'.  Return the number of bytes copied.  If the
        // internal read buffer is exhausted, first attempt to refill it by
        // calling 'read' on the underlying 'bteso_StreamSocket' object, and
        // return the error code if that call returns an error code.  In
        // particular, if there is no data in the internal buffer, either block
        // (in blocking mode) or return 'ERROR_WOULDBLOCK' if reading on the
        // underlying socket would block.

    virtual int write(const char *buffer, int length);
        // Copy the specified 'length' bytes from the specified 'buffer' to the
        // internal write buffer, or the number of free bytes in the internal
        // write buffer if this number is smaller.  If the internal write
        // buffer gets filled up, subsequently attempt to flush it by calling
        // 'flush' on the underlying 'bteso_StreamSocket' object, and return
        // the error code if that call returns an error code.  Otherwise,
        // return the number of bytes copied.  In particular, if there is no
        // data in the internal buffer, either block (in blocking mode) or
        // return 'ERROR_WOULDBLOCK' if reading on the underlying socket would
        // block.  Note that a successful call to this function does not
        // guarantee that the data has been transmitted successfully, but
        // simply that the data was successfully written to the underlying
        // socket's transmit buffers.

    virtual int writev(const btes_Iovec *buffers, int numBuffers);
    virtual int writev(const btes_Ovec *buffers, int numBuffers);
        // Copy up to the specified 'numBuffers' of the specified 'buffers'
        // from the internal write buffer, or the number of free bytes in the
        // internal write buffer if this number is smaller than the total size
        // of the 'buffers'.  If the internal write buffer gets filled up,
        // subsequently attempt to flush it by calling 'write' on the
        // underlying 'bteso_StreamSocket' object, and return the error code if
        // that call returns an error code.  Otherwise, return the number of
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

    virtual int setBlockingMode(bteso_Flag::BlockingMode mode);
        // Return the result of calling 'setBlockingMode' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int shutdown(bteso_Flag::ShutdownType streamOption);
        // Return the result of calling 'shutdown' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int waitForConnect(const bdet_TimeInterval& timeout);
        // Return the result of calling 'waitForConnect' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int waitForAccept(const bdet_TimeInterval& timeout);
        // Return the result of calling 'waitForAccept' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int waitForIO(bteso_Flag::IOWaitType   type,
                          const bdet_TimeInterval& timeout);
    virtual int waitForIO(bteso_Flag::IOWaitType type);
        // Return the result of calling 'waitForIO' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int setLingerOption(
                           const bteso_SocketOptUtil::LingerData& options);
        // Return the result of calling 'setLingerOption' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int setOption(int level, int option, int value);
        // Return the result of calling 'setOption' on the underlying
        // 'bteso_StreamSocket' object.

    void setReadLimit(int numBytes);
        // Set this socket to accept at most the specified 'numBytes' number of
        // bytes in subsequent write calls, and to shut down the connection if
        // this limit is reached.

    void setWriteLimit(int numBytes);
        // Set this socket to read at most the specified 'numBytes' number of
        // bytes in subsequent read calls, and to shut down the connection if
        // this limit is reached.

    // ACCESSORS
    virtual int blockingMode(bteso_Flag::BlockingMode *result) const;
        // Return the result of calling 'blockingMode' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int connectionStatus() const;
        // Return the result of calling 'connectionStatus' on the underlying
        // 'bteso_StreamSocket' object.

    virtual bteso_SocketHandle::Handle handle() const;
        // Return the result of calling 'handle' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int localAddress(ADDRESS *result) const;
        // Return the result of calling 'localAddress' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int peerAddress(ADDRESS *result) const;
        // Return the result of calling 'peerAddress' on the underlying
        // 'bteso_StreamSocket' object.

    virtual int lingerOption(bteso_SocketOptUtil::LingerData *result) const;
        // Return the result of calling 'lingerOption' on the underlying
        // 'bteso_StreamSocket' object.
    virtual int socketOption(int *result, int level, int option) const;
        // Return the result of calling 'socketOption' on the underlying
        // 'bteso_StreamSocket' object.
};

                    // ================================
                    // class SslLikeStreamSocketFactory
                    // ================================

template <class ADDRESS>
class bteso_SslLikeStreamSocketFactory
: public bteso_InetStreamSocketFactory<ADDRESS>
{
    // Provide an implementation for a factory to allocate and deallocate
    // TCP-based OpenSsl-like stream socket objects of type
    // 'bteso_SslLikeStreamSocket<ADDRESS>'.

    int              d_bufferSize;  // used to allocate ssl-like sockets
    bslma_Allocator *d_allocator_p; // holds (but doesn't own) object

    public:
    // CREATORS
    bteso_SslLikeStreamSocketFactory(int              internalBufferSize,
                                     bslma_Allocator *basicAllocator = 0);
        // Create a stream socket factory for ssl-like sockets with an internal
        // buffer of the specified 'internalBufferSize'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    virtual ~bteso_SslLikeStreamSocketFactory();
        // Destroy this factory.  The stream sockets created by this factory
        // will not be destroyed when this factory is destroyed.

    // MANIPULATORS
    virtual bteso_StreamSocket<ADDRESS> *allocate();
        // Create a stream-based socket of type
        // 'bteso_SslLikeStreamSocket<ADDRESS>'.  Return the address of the
        // newly-created socket on success, and 0 otherwise.

    virtual bteso_StreamSocket<ADDRESS> *allocate(
                                            bteso_SocketHandle::Handle handle);
        // Create a stream-based socket of type
        // 'bteso_SslLikeStreamSocket<ADDRESS>' attached to the specified
        // socket 'handle'.  Return the address of the newly-created socket on
        // success, and 0 otherwise.  The behavior is undefined unless 'handle'
        // refers to a valid TCP-based stream socket with the address class
        // 'ADDRESS'.

    virtual void deallocate(bteso_StreamSocket<ADDRESS> *socket);
    virtual void deallocate(bteso_StreamSocket<ADDRESS> *socket, bool);
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

    bteso_StreamSocket<ADDRESS>    *d_socket_p;  // socket under proctorship
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
                            bteso_StreamSocket<ADDRESS>               *socket,
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
        bteso_StreamSocket<ADDRESS> *socket,
        bslma_Allocator             *basicAllocator)
: d_socket_p(socket)
, d_bufferSize(internalBufferSize)
, d_readBytesAvailable(0)
, d_writeOffset(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
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
        bteso_StreamSocket<ADDRESS> **result)
{
    return d_socket_p->accept(result);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::accept(
        bteso_StreamSocket<ADDRESS> **socket,
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
            return retCode;
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
        return length;
    }

    return bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::readv(const btes_Iovec *buffers,
                                              int               numBuffers)
{
    BSLS_ASSERT(0 < numBuffers);

    if (0 == d_readBytesAvailable) {
        int retCode = d_socket_p->read(d_readBuffer, d_bufferSize);
        if (retCode < 0) {
            return retCode;
        }
        if (veryVerbose) {
            MTCOUT << "\tSslLikeStreamSocket::readv: " << retCode << "bytes."
                   << MTENDL;
        }
        d_readBytesAvailable = retCode;
    }

    if (0 < d_readBytesAvailable) {
        int length = btes_IovecUtil::scatter(buffers, numBuffers,
                                             d_readBuffer,
                                             d_readBytesAvailable);
        BSLS_ASSERT(length < d_readBytesAvailable);
        d_readBytesAvailable -= length;
        bsl::memmove(d_readBuffer, d_readBuffer + length,
                     d_readBytesAvailable);
        return length;
    }

    return bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::write(const char *buffer, int length)
{
    BSLS_ASSERT(0 < length);

    if (d_writeOffset == d_bufferSize) {
        int retCode = d_socket_p->write(d_writeBuffer, d_bufferSize);
        if (retCode < 0) {
            return retCode;
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
        return length;
    }

    return bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::writev(const btes_Iovec *buffers,
                                               int               numBuffers)
{
    BSLS_ASSERT(0 < numBuffers);

    if (d_writeOffset == d_bufferSize) {
        int retCode = d_socket_p->write(d_writeBuffer, d_bufferSize);
        if (retCode < 0) {
            return retCode;
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
        int length = btes_IovecUtil::gather(d_writeBuffer + d_writeOffset,
                                            d_bufferSize - d_writeOffset,
                                            buffers, numBuffers);
        d_writeOffset += length;
        return length;
    }

    return bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::writev(const btes_Ovec *buffers,
                                               int              numBuffers)
{
    BSLS_ASSERT(0 < numBuffers);

    if (d_writeOffset == d_bufferSize) {
        int retCode = d_socket_p->write(d_writeBuffer, d_bufferSize);
        if (retCode < 0) {
            return retCode;
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
        int length = btes_IovecUtil::gather(d_writeBuffer + d_writeOffset,
                                            d_bufferSize - d_writeOffset,
                                            buffers, numBuffers);
        d_writeOffset += length;
        return length;
    }

    return bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::writeFlush()
{
    if (0 == d_writeOffset) {
        return 0;
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
        return retCode;
    }

    BSLS_ASSERT(retCode < 0);
    return retCode;
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::setBlockingMode(
        bteso_Flag::BlockingMode mode)
{
    return d_socket_p->setBlockingMode(mode);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::shutdown(
        bteso_Flag::ShutdownType streamOption)
{
    return d_socket_p->shutdown(streamOption);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::waitForConnect(
        const bdet_TimeInterval& timeout)
{
    return d_socket_p->waitForConnect(timeout);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::waitForAccept(
        const bdet_TimeInterval& timeout)
{
    return d_socket_p->waitForAccept(timeout);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::waitForIO(
        bteso_Flag::IOWaitType   type,
        const bdet_TimeInterval& timeout)
{
    return d_socket_p->waitForIO(type, timeout);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::waitForIO(
        bteso_Flag::IOWaitType type)
{
    return d_socket_p->waitForIO(type);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::setLingerOption(
        const bteso_SocketOptUtil::LingerData& options)
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
                                        bteso_Flag::BlockingMode *result) const
{
    return d_socket_p->blockingMode(result);
}

template <class ADDRESS>
int bteso_SslLikeStreamSocket<ADDRESS>::connectionStatus() const
{
    return d_socket_p->connectionStatus();
}

template <class ADDRESS>
bteso_SocketHandle::Handle bteso_SslLikeStreamSocket<ADDRESS>::handle() const
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
                                 bteso_SocketOptUtil::LingerData *result) const
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
        int              internalBufferSize,
        bslma_Allocator *basicAllocator)
: bteso_InetStreamSocketFactory<ADDRESS>(basicAllocator)
, d_bufferSize(internalBufferSize)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <class ADDRESS>
bteso_SslLikeStreamSocketFactory<ADDRESS>::~bteso_SslLikeStreamSocketFactory()
{
}

    // MANIPULATORS
template <class ADDRESS>
bteso_StreamSocket<ADDRESS> *
bteso_SslLikeStreamSocketFactory<ADDRESS>::allocate()
{
    bteso_StreamSocket<ADDRESS> *socket =
                            bteso_InetStreamSocketFactory<ADDRESS>::allocate();

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
bteso_StreamSocket<ADDRESS> *
bteso_SslLikeStreamSocketFactory<ADDRESS>::allocate(
        bteso_SocketHandle::Handle handle)
{
    bteso_StreamSocket<ADDRESS> *socket =
                      bteso_InetStreamSocketFactory<ADDRESS>::allocate(handle);

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
        bteso_StreamSocket<ADDRESS> *socket)
{
    bteso_SslLikeStreamSocket<ADDRESS> *sSocket =
                reinterpret_cast<bteso_SslLikeStreamSocket<ADDRESS> *>(socket);

    bteso_InetStreamSocketFactory<ADDRESS>::deallocate(sSocket->d_socket_p);
    sSocket->~bteso_SslLikeStreamSocket<ADDRESS>();
    d_allocator_p->deallocate(socket);
}

template <class ADDRESS>
void bteso_SslLikeStreamSocketFactory<ADDRESS>::deallocate(
        bteso_StreamSocket<ADDRESS> *socket, bool closeHandleFlag)
{
    bteso_SslLikeStreamSocket<ADDRESS> *sSocket =
                reinterpret_cast<bteso_SslLikeStreamSocket<ADDRESS> *>(socket);

    if (closeHandleFlag) {
       bteso_InetStreamSocketFactory<ADDRESS>::deallocate(sSocket->d_socket_p);
    }
    sSocket->~bteso_SslLikeStreamSocket<ADDRESS>();
    d_allocator_p->deallocate(socket);
}

} // closing namespace TEST_CASE_21_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 20 function helpers
//-----------------------------------------------------------------------------
namespace TEST_CASE_20_NAMESPACE {

static
void case20DataCallback(int           *numConsumed,
                        int           *numNeeded,
                        btemt_DataMsg  msg,
                        void          *context)
{
    *numConsumed = msg.data()->length();
    *numNeeded   = 1;

    // bcemt_ThreadUtil::microSleep(1000); // 1ms
}

static
void case20ChannelStateCallback(int                 channelId,
                                int                 serverId,
                                int                 state,
                                void               *arg,
                                btemt_ChannelPool **poolAddr,
                                int               **eventAddr,
                                bcemt_Barrier      *barrier,
                                int                *channelId_p)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btemt_ChannelPool *pool = *poolAddr;

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
      } break;
      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
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

} // closing namespace TEST_CASE_20_NAMESPACE

//-----------------------------------------------------------------------------
// CASE XX supporting classes and methods
// Apparently unused
//-----------------------------------------------------------------------------

#if 0
namespace TEST_CASE_XX_NAMESPACE {

struct CaseXXImporterInfo {
    btemt_ChannelPool                                *pool;
    bteso_InetStreamSocketFactory<bteso_IPv4Address> *factory;
    bteso_IPv4Address                                 address;
    bcemt_Barrier                                    *barrier;
};

//extern "C"
void *caseXXImporterThread(void *arg)
{
    const CaseXXImporterInfo& info = *(struct CaseXXImporterInfo*) arg;
    typedef bteso_StreamSocket<bteso_IPv4Address> Socket;

    info.barrier->wait();
    for (int i = 0; i < 10; ++i) {
        Socket  *socket = info.factory->allocate();

        ASSERT(0 == socket->connect(info.address));
        ASSERT(0 == info.pool->import(socket, info.factory, 0));
    }
    return 0;
}

struct CaseXXCloserInfo {
    btemt_ChannelPool *pool;
    bcec_ObjectCatalog<int> *handles;
    bcemt_Barrier                                    *barrier;
};

//extern "C"
void *caseXXCloserThread(void *arg) {
    const CaseXXCloserInfo& info = *(struct CaseXXCloserInfo *) arg;

    info.barrier->wait();
    for (int i = 0; i < 10; ++i) {
        int channelHandle;

        while (1) {
            bool handleFound = false;
            int handle;
            {
                bcec_ObjectCatalogIter<int> it(*info.handles);
                if (static_cast<const void *>(it)) {
                    handle = it().first;
                    handleFound = true;
                }
            }
            if (handleFound &&
                0 == info.handles->remove(handle, &channelHandle)) {
                break;
            }
            bcemt_ThreadUtil::yield();
            bcemt_ThreadUtil::microSleep(50); // 50 microseconds
        }
        ASSERT(0 == info.pool->shutdown(channelHandle,
                                        btemt_ChannelPool::BTEMT_IMMEDIATE));
    }
    return 0;
}

static
void caseXXClientDataReadCallback(
        int             *consumed,
        int             *needed,
        btemt_DataMsg    msg,
        void            *arg)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(msg.data());

    bcema_PooledBufferChain *chain = msg.data();
    int numAvailable = chain->length();

    *consumed = numAvailable / 2;
    *needed = numAvailable - *consumed + 1;
    //*consumed = 0;
    //*needed = numAvailable + 1;
}

static
void caseXXClientChannelStateCallback(
        int                 channelId,
        int                 serverId,
        int                 state,
        void               *arg,
        bcec_ObjectCatalog<int> *catalog)
{
    ASSERT(catalog);

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        //bcemt_ThreadUtil::microSleep(50);  // 50 microsecs.
        catalog->add(channelId);
      } break;
    }
}

static
void caseXXServerChannelStateCallback(
        int                 channelId,
        int                 serverId,
        int                 state,
        void               *arg,
        btemt_ChannelPool **poolAddr)
{
    ASSERT(poolAddr && *poolAddr)
    btemt_ChannelPool *pool = *poolAddr;

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        int i;
        for (i = 0; i < 100; ++i) {
            bcema_PooledBufferChain *chain =
                                    pool->outboundBufferFactory()->allocate(0);
            chain->setLength(chain->bufferSize());
            bsl::memset(chain->buffer(0), 0xab, chain->bufferSize());
            btemt_DataMsg msg(chain, pool->outboundBufferFactory(), channelId);
            if (0 != pool->write(channelId, msg)){
                break;
            }
        }
        //MTCOUT << "stopped at " << i << MTENDL;
      } break;
    }
}

} // closing namespace TEST_CASE_XX_NAMESPACE
#endif

//-----------------------------------------------------------------------------
// CASE 19 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_19_NAMESPACE {

static
void case19PoolStateCallback(
    int              state,
    int              serverId,
    int              severity,
    bces_AtomicInt64 *acceptErrors)
{
    ASSERT(acceptErrors);

    switch (state) {
      case btemt_PoolMsg::BTEMT_ERROR_ACCEPTING: {
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

} // closing namespace TEST_CASE_19_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 18 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_18_NAMESPACE {

static
void case18ChannelStateCallback(
        int                 channelId,
        int                 serverId,
        int                 state,
        void               *arg,
        btemt_ChannelPool **poolAddr,
        int               **eventAddr,
        bcemt_Barrier      *barrier)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btemt_ChannelPool *pool = *poolAddr;

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        pool->shutdown(channelId, btemt_ChannelPool::BTEMT_IMMEDIATE);
        **eventAddr = -1;
        barrier->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
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
void case18PoolStateCallback(
    int              state,
    int              serverId,
    int              severity,
    int            **eventAddr,
    bces_AtomicInt  *limitReachedFlag)
{
    ASSERT(eventAddr && *eventAddr);
    ASSERT(limitReachedFlag);

    **eventAddr = state;

    switch (state) {
      case btemt_PoolMsg::BTEMT_ACCEPT_TIMEOUT: {
        if (veryVerbose) {
            MTCOUT << "Accept timed out:"
                   << " serverId=" << serverId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case btemt_PoolMsg::BTEMT_ERROR_CONNECTING: {
        if (veryVerbose) {
            MTCOUT << "Error Connecting:"
                   << " serverId=" << serverId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case btemt_PoolMsg::BTEMT_ERROR_ACCEPTING: {
        if (veryVerbose) {
            MTCOUT << "Error Accepting:"
                   << " serverId=" << serverId
                   << " severity=" << (severity ? "ALERT" : "CRITICAL")
                   << MTENDL;
        }
      } break;
      case btemt_PoolMsg::BTEMT_CHANNEL_LIMIT: {
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

} // closing namespace TEST_CASE_18_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 17 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_17_NAMESPACE {

static
void case17ChannelStateCallback(int            channelId,
                                int            serverId,
                                int            state,
                                void          *arg,
                                int           *value,
                                bcemt_Barrier *barrier)
{
    ASSERT(value);
    ASSERT(barrier);

    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
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

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
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

} // closing namespace TEST_CASE_17_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 16 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_16_NAMESPACE {

static
void case16ChannelStateCallback(
        int                 channelId,
        int                 serverId,
        int                 state,
        void               *arg,
        int                 expServerId,
        btemt_ChannelPool **poolAddr,
        int                *value,
        bcemt_Barrier      *barrier)
{
    ASSERT(poolAddr && *poolAddr);
    ASSERT(barrier);

    ASSERT(expServerId == serverId);
    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
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

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        ASSERT(0 == *value);
        ASSERT(0x701d50da == (unsigned)serverId);
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        *value = channelId;
        (*poolAddr)->shutdown(channelId, btemt_ChannelPool::BTEMT_IMMEDIATE);
      } break;
    }
}

} // closing namespace TEST_CASE_16_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 15 supporting classes and methods
//-----------------------------------------------------------------------------

bsl::ostream&
operator<<(bsl::ostream& s,
           const bsl::vector<btemt_ChannelPool::HandleInfo>& handles)
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

namespace TEST_CASE_15_NAMESPACE {

enum {
    SERVER_ID            = 0x701d50da,
    CLIENT_ID1           = 0x12345678,
    CLIENT_ID2           = 0x23456789
};

struct LessThanByType {
    typedef btemt_ChannelPool::HandleInfo first_argument_type;
    typedef btemt_ChannelPool::HandleInfo second_argument_type;
    typedef bool result_type;

    result_type operator()(const first_argument_type&  lhs,
                           const second_argument_type& rhs) const {
        return lhs.d_channelType < rhs.d_channelType;
    }
};

static
void case15ChannelStateCallback(int   channelId,
                                int   sourceId,
                                int   state,
                                void *arg,
                                // additional parameters below:
                                int  *acceptedChannelId1,
                                int  *channelId1,
                                int  *channelId2,
                                int   veryVerbose)
{
    ASSERT(acceptedChannelId1);
    ASSERT(channelId1);
    ASSERT(channelId2);

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
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

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
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

} // closing namespace TEST_CASE_15_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 14 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_14_NAMESPACE {

static
void case14ChannelDataCallback(int *numConsumed, int *numNeeded,
                           btemt_DataMsg msg, void *context,
                           Obj **channelPool_p, bces_AtomicInt *fail)
{
    ASSERT(numConsumed);
    ASSERT(msg.data());
    ASSERT(0 < msg.data()->length());

    if (0 == (*channelPool_p)->write(msg.channelId(), msg)) {
        *fail = 1;
    }

    *numConsumed = msg.data()->length();
    *numNeeded   = 1;
}

static
void case14ChannelStateCallback(
        int                 channelId,
        int                 serverId,
        int                 state,
        void               *arg,
        btemt_ChannelPool **poolAddr,
        int               **eventAddr,
        bcemt_Barrier      *barrier,
        int                *channelId_p)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btemt_ChannelPool *pool = *poolAddr;

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        barrier->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
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

} // closing namespace TEST_CASE_14_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 13 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_13_NAMESPACE {

static
void case13ChannelStateCallback(int                 channelId,
                                int                 serverId,
                                int                 state,
                                void               *arg,
                                btemt_ChannelPool **poolAddr,
                                int               **eventAddr,
                                bcemt_Barrier      *barrier)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btemt_ChannelPool *pool = *poolAddr;

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        barrier->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
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

} // closing namespace TEST_CASE_13_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 12 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_12_NAMESPACE {

static
void case12FlowControlChannelCallback(
                       int                                    channelId,
                       int                                    sourceId,
                       int                                    state,
                       void                                   *arg,
                       bcemt_Barrier                          *barrier,
                       int                                    *numBytesWritten,
                       const btemt_ChannelPoolConfiguration&   config,
                       bcema_PooledBufferChainFactory         *factory,
                       btemt_ChannelPool                     **instance)
{
    ASSERT(barrier);
    ASSERT(numBytesWritten);
    ASSERT(factory);
    ASSERT(instance);

    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << sourceId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        (*instance)->shutdown(channelId, btemt_ChannelPool::BTEMT_IMMEDIATE);
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << sourceId << bsl::dec
                   << " channelId=" << channelId
                << MTENDL;
        }
        bcema_PooledBufferChain *chain =  factory->allocate(0);
        chain->setLength(chain->bufferSize());
        bsl::memset(chain->buffer(0), 0xab, chain->bufferSize());
        btemt_DataMsg msg(chain, factory, channelId);
        while ((*instance)->write(channelId, msg) == 0) {
            *numBytesWritten += chain->length();
        }
        if (verbose) {
            P(*numBytesWritten);
        }
        barrier->wait();
      } break;
      case btemt_ChannelPool::BTEMT_WRITE_CACHE_HIWAT: {
        if (verbose) {
            MTCOUT << "HIWAT " << MTENDL;
        }
      } break;
      case btemt_ChannelPool::BTEMT_WRITE_CACHE_LOWWAT: {
        if (verbose) {
            MTCOUT << "LOWWAT " << MTENDL;
        }
      } break;
    }
}

} // closing namespace TEST_CASE_12_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 11 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_11_NAMESPACE {

static
void runTestCase11(bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                   bcema_TestAllocator&                          ta)
{
    enum {
        SERVER_ID   = 1013410001,
        BACKLOG     = 1
    };
    const int NUM_SOCKETS = 20;  // an enum would confuse bsl::fill_n below

    bsl::vector<my_ChannelEvent> channelEvents;
    bcemt_Mutex                  channelEventsMutex;
    bsl::vector<my_PoolEvent>    poolEvents;
    bcemt_Mutex                  poolEventsMutex;

    btemt_ChannelPoolConfiguration config;
    config.setMaxThreads(4);
    config.setMetricsInterval(10.0);
    config.setMaxConnections(NUM_SOCKETS);
    config.setIncomingMessageSizes(1, 1, 1);
    config.setReadTimeout(1000);

    btemt_ChannelPool::DataReadCallback         dataCb;

    btemt_ChannelPool::ChannelStateChangeCallback channelCb(
            bdef_BindUtil::bindA( &ta
                                , recordChannelState
                                , _1, _2, _3, _4
                                , &channelEvents
                                , &channelEventsMutex));

    btemt_ChannelPool::PoolStateChangeCallback    poolCb(
            bdef_BindUtil::bindA( &ta
                                , recordPoolState
                                , _1, _2, _3
                                , &poolEvents
                                , &poolEventsMutex));

    makeNull(&dataCb);

    btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
    if (verbose) {
        P(config);
    }
    ASSERT(0 == mX.start());

    // Establish server
    makeNull(&channelCb);
    makeNull(&poolCb);
    btemt_ChannelPool mY(channelCb, dataCb, poolCb, config, &ta);
    mY.start();

    const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
    ASSERT(0 == mY.listen(ADDRESS, BACKLOG, SERVER_ID));
    const bteso_IPv4Address PEER = *mY.serverAddress(SERVER_ID);

    // Establish clients and connect them to server
    for (int i = 0; i < NUM_SOCKETS; ++i) {
        bteso_StreamSocket<bteso_IPv4Address> *socket = factory->allocate();
#if 0
        ASSERT(0 == socket->setOption(bteso_SocketOptUtil::BTESO_TCPNODELAY,
                                      bteso_SocketOptUtil::BTESO_TCPLEVEL,
                                      1));
#endif
        ASSERT(0 == socket->connect(PEER));

        mX.import(socket, factory, i);
    }
    bcemt_ThreadUtil::yield();
    bcemt_ThreadUtil::microSleep(0, 15); // 5 sec should be good enough

    ASSERT(NUM_SOCKETS     == mX.numChannels());
    ASSERT(2 * NUM_SOCKETS == channelEvents.size());

    int numEvents = channelEvents.size();
    if (veryVerbose) { P(numEvents); PV(channelEvents); }

    int importedFlag[NUM_SOCKETS];
    bsl::fill_n((int*)importedFlag, NUM_SOCKETS, 0);

    for (int i = 0; i < numEvents; ++i) {
        LOOP_ASSERT(i, 0 <= channelEvents[i].d_data.allocatorId());
        if (veryVerbose) { P(channelEvents[i].d_data.allocatorId()); }

        LOOP_ASSERT(i, btemt_ChannelMsg::BTEMT_CHANNEL_UP ==
                                             channelEvents[i].d_data.event() ||
                       btemt_ChannelMsg::BTEMT_AUTO_READ_ENABLED ==
                                             channelEvents[i].d_data.event());
        LOOP_ASSERT(i, NUM_SOCKETS > channelEvents[i].d_data.allocatorId());

        if (channelEvents[i].d_data.event() ==
                                     btemt_ChannelMsg::BTEMT_AUTO_READ_ENABLED)
        {
            LOOP_ASSERT(i, 1 ==
                          importedFlag[channelEvents[i].d_data.allocatorId()]);
            importedFlag[channelEvents[i].d_data.allocatorId()] = 3;
        }
        else {
            LOOP_ASSERT(i, 0 ==
                          importedFlag[channelEvents[i].d_data.allocatorId()]);
            importedFlag[channelEvents[i].d_data.allocatorId()] = 1;
        }
    }

    for (int i = 0; i < NUM_SOCKETS; ++i) {
        LOOP2_ASSERT(i, importedFlag[i], 3 == importedFlag[i]);
    }

    bsl::fill_n(importedFlag, NUM_SOCKETS, 0);
    bsl::vector<my_ChannelEvent> backup;

    for (int i = 0; i < (int)channelEvents.size(); ++i) {
        if (btemt_ChannelMsg::BTEMT_AUTO_READ_ENABLED !=
                                               channelEvents[i].d_data.event())
        {
            backup.push_back(channelEvents[i]);
        }
    }

    if (verbose)
        cout << "\tDisabling channels." << endl;
    {
        channelEvents.clear();
        for (int i = 0; i < (int)backup.size(); ++i) {
            if (veryVerbose) { P(backup[i].d_data.channelId()); }
            LOOP_ASSERT(i, 0 == mX.disableRead(backup[i].d_data.channelId()));
        }
        bcemt_ThreadUtil::microSleep(0, 2);

        numEvents = channelEvents.size();
        if (veryVerbose) {
            P(numEvents);
            PV(channelEvents);
        }

        bsl::fill_n(importedFlag, NUM_SOCKETS, 0);
        for (int i = 0; i < numEvents; ++i) {
            LOOP_ASSERT(i, 0 <= channelEvents[i].d_data.allocatorId());
            if (veryVerbose) {
                P(channelEvents[i].d_data.allocatorId());
            }
            LOOP_ASSERT(i, btemt_ChannelMsg::BTEMT_AUTO_READ_DISABLED ==
                                              channelEvents[i].d_data.event());
            LOOP_ASSERT(i, NUM_SOCKETS >
                                        channelEvents[i].d_data.allocatorId());
            importedFlag[channelEvents[i].d_data.allocatorId()] = 1;
        }

        for (int i = 0; i < NUM_SOCKETS; ++i) {
            LOOP_ASSERT(i, 1 == importedFlag[i]);
        }
        bcemt_ThreadUtil::microSleep(0, 2);
    }

    if (verbose)
        cout << "\tEnabling channels." << endl;
    {
        channelEvents.clear();
        for (int i = 0; i < (int)backup.size(); ++i) {
            if (veryVerbose) {
                P(backup[i].d_data.channelId());
            }
            LOOP_ASSERT(i, 0 == mX.enableRead(backup[i].d_data.channelId()));
        }
        bcemt_ThreadUtil::microSleep(0, 2);

        numEvents = channelEvents.size();
        if (veryVerbose) {
            P(numEvents);
            PV(channelEvents);
        }

        bsl::fill_n(importedFlag, NUM_SOCKETS, 0);
        for (int i = 0; i < numEvents; ++i) {
            LOOP_ASSERT(i, 0 <= channelEvents[i].d_data.allocatorId());
            LOOP_ASSERT(i, btemt_ChannelMsg::BTEMT_AUTO_READ_ENABLED ==
                                              channelEvents[i].d_data.event());
            LOOP_ASSERT(i, NUM_SOCKETS >
                                        channelEvents[i].d_data.allocatorId());
            importedFlag[channelEvents[i].d_data.allocatorId()] = 1;
        }

        for (int i = 0; i < NUM_SOCKETS; ++i) {
            LOOP_ASSERT(i, 1 == importedFlag[i]);
        }
        bcemt_ThreadUtil::microSleep(0, 2);
    }

    for (int i = 0; i < (int)backup.size(); ++i) {
        mX.shutdown(backup[i].d_data.channelId(),
                    btemt_ChannelPool::BTEMT_IMMEDIATE);
    }
    mX.stop();

    mY.close(SERVER_ID);
    mY.stop();
}

} // closing namespace TEST_CASE_11_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 10 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_10_NAMESPACE {

enum { DONT_CHECK_THREAD_ID = -1 };

struct my_ClockState {
    bcemt_ThreadUtil::Handle  d_threadHandle;
    bdet_TimeInterval         d_startTime;
    bdet_TimeInterval         d_timeout;
    int                       d_numInvocations;
    int                       d_maxNumInvocations;
};

static
void case10MyClockCallback(my_ClockState     *state,
                           int                clockId,
                           btemt_ChannelPool *pool,
                           int                deregister,
                           ThreadId           checkThreadId)
{
    bdet_TimeInterval now = bdetu_SystemTime::now();
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

    if (NULL_THREAD_ID != checkThreadId) {
        ASSERT(checkThreadId == bcemt_ThreadUtil::selfIdAsUint64());
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
void case10ChannelStateCallback(int                         channelId,
                                int                         sourceId,
                                int                         state,
                                void                       *arg,
                                // additional parameters below:
                                int                        *channelId1,
                                bsls_PlatformUtil::Uint64  *threadId1,
                                int                        *channelId2,
                                bsls_PlatformUtil::Uint64  *threadId2,
                                int                         veryVerbose)
{
    ASSERT(channelId1 && threadId1);
    ASSERT(channelId2 && threadId1);

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << sourceId
                   << " channelId=" << channelId
                   << MTENDL;
        }
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="    << sourceId
                   << " channelId="   << channelId
                   << MTENDL;
        }
        if (1 == sourceId) {
            *channelId1 = channelId;
            *threadId1  = bcemt_ThreadUtil::selfIdAsInt();
        }
        else {
            *channelId2 = channelId;
            *threadId2  = bcemt_ThreadUtil::selfIdAsInt();
        }
      } break;
    }
}

} // closing namespace TEST_CASE_10_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 9 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_9_NAMESPACE {

struct case9ThreadInfo {
    bcemt_Barrier   *d_barrier;
    bces_AtomicInt  *d_threadCount;
    Obj             *d_channelPool_p;
    int              d_channelId;
    int              d_write;
    int              d_bufferAlloc;
    int              d_totalAlloc;
    bslma_Allocator *d_allocator_p;
};

struct case9ThreadInfo2 {
    bcemt_Barrier                  *d_barrier;
    bces_AtomicInt                 *d_threadCount;
    Obj                            *d_channelPool_p;
    int                             d_channelId;
    int                             d_write;
    int                             d_bufferAlloc;
    int                             d_totalAlloc;
    bslma_Allocator                *d_allocator_p;
    bcema_PooledBufferChainFactory *d_factory_p;
};

extern "C" {
void *case9Thread0(void *arg)
{
    case9ThreadInfo2 *info = (case9ThreadInfo2 *) arg;
    const int BUF_SIZE = info->d_bufferAlloc;
    const int TOTAL_SIZE = info->d_totalAlloc;

    // Set up buffer to write to channel pool, of total size
    // TOTAL_SIZE, filled with the requested value.
    char *buffer = (char *) info->d_allocator_p->allocate(TOTAL_SIZE);
    bsl::memset(buffer, info->d_write, TOTAL_SIZE);
    bcema_SharedPtr<char> sharedBuffer(buffer,
                                       info->d_allocator_p);

    // Be careful to round numVecs upward and compute lastVec size correctly.
    const int numVecs = (TOTAL_SIZE + BUF_SIZE - 1) / BUF_SIZE;
    const int LAST_VEC_SIZE = (0 < TOTAL_SIZE % BUF_SIZE)
                            ? TOTAL_SIZE % BUF_SIZE
                            : BUF_SIZE;  // size of the last buffer

    // Segment buffer into bcema_Blob of individual buffers of BUF_SIZE.
    bcema_Blob blob(info->d_allocator_p);
    for (int i = 0 ;i < numVecs; ++i) {
        bcema_SharedPtr<char> buf(sharedBuffer, buffer + i * BUF_SIZE);
        blob.appendBuffer(bcema_BlobBuffer(buf,
                                           (i < numVecs-1) ? BUF_SIZE
                                                           : LAST_VEC_SIZE));
    }
    blob.setLength(TOTAL_SIZE);

    // Wait until all threads have initialized their buffers.
    info->d_barrier->wait();

    // Now write in a short burst, until write succeeds.
    while (0 != info->d_channelPool_p->write(info->d_channelId, blob)) {
        bcemt_ThreadUtil::yield();
    }

    // Signal effective termination to write.
    ++(*(info->d_threadCount));
    info->d_barrier->wait();

    return NULL;
}
}
extern "C" {
void *case9Thread1(void *arg)
{
    case9ThreadInfo2 *info = (case9ThreadInfo2 *) arg;
    const int BUF_SIZE = info->d_bufferAlloc;
    const int TOTAL_SIZE = info->d_totalAlloc;

    // Set up buffer to write to channel pool, of total size
    // TOTAL_SIZE, filled with the requested value.
    char *buffer = (char *) info->d_allocator_p->allocate(TOTAL_SIZE);
    bsl::memset(buffer, info->d_write, TOTAL_SIZE);

    // Be careful to round numVecs upward and compute lastVec size correctly.
    const int numVecs = (TOTAL_SIZE + BUF_SIZE - 1) / BUF_SIZE;
    const int LAST_VEC_SIZE = (0 < TOTAL_SIZE % BUF_SIZE)
                            ? TOTAL_SIZE % BUF_SIZE
                            : BUF_SIZE;  // size of the last buffer

    // Segment buffer into DataMsg of individual buffers of BUF_SIZE.
    bcema_PooledBufferChain *chain = info->d_factory_p->allocate(0);
    for (int i = 0 ;i < numVecs; ++i) {
        chain->append(buffer + i * BUF_SIZE,
                      (i < numVecs-1) ? BUF_SIZE : LAST_VEC_SIZE);
    }
    btemt_DataMsg msg;
    msg.setData(chain, info->d_factory_p, info->d_allocator_p);
    msg.setChannelId(info->d_channelId);

    // Wait until all threads have initialized their buffers.
    info->d_barrier->wait();

    // Now write in a short burst, until write succeeds.
    while (0 != info->d_channelPool_p->write(info->d_channelId, msg)) {
        bcemt_ThreadUtil::yield();
    }

    // Signal effective termination to write.
    ++(*(info->d_threadCount));
    info->d_barrier->wait();

    info->d_allocator_p->deallocate(buffer);
    return NULL;
}
}
extern "C" {
void *case9Thread2(void *arg)
{
    case9ThreadInfo *info = (case9ThreadInfo *) arg;
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
    btes_Iovec *vecs = (btes_Iovec *) info->d_allocator_p->allocate(numVecs *
                                                            sizeof(btes_Ovec));
    for (int i = 0; i < numVecs; ++i) {
        vecs[i].setBuffer(buffer + i * BUF_SIZE,
                          (i < numVecs-1) ? BUF_SIZE : LAST_VEC_SIZE);
    }

    // Wait until all threads have initialized their buffers.
    info->d_barrier->wait();

    // Now write in a short burst, until write succeeds.
    while (0 != info->d_channelPool_p->write(info->d_channelId,
                                             vecs, numVecs)) {
        bcemt_ThreadUtil::yield();
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
void *case9Thread3(void *arg)
{
    case9ThreadInfo2 *info = (case9ThreadInfo2 *) arg;
    const int BUF_SIZE = info->d_bufferAlloc;
    const int TOTAL_SIZE = info->d_totalAlloc;

    // Set up buffer to write to channel pool, of total size
    // info->d_bufferAlloc, filled with the requested value.
    char *buffer = (char *) info->d_allocator_p->allocate(TOTAL_SIZE);
    bsl::memset(buffer, info->d_write, TOTAL_SIZE);
    bcema_SharedPtr<char> sharedBuffer;
    if (0 == info->d_write % 3) {
        sharedBuffer.load(buffer, info->d_allocator_p);
    }

    // Be careful to round numVecs upward and compute lastVec size correctly.
    const int numVecs = (TOTAL_SIZE + BUF_SIZE - 1) / BUF_SIZE;
    const int LAST_VEC_SIZE = (0 < TOTAL_SIZE % BUF_SIZE)
                            ? TOTAL_SIZE % BUF_SIZE
                            : BUF_SIZE;  // size of the last buffer

    // Segment buffer into Iovecs, or DataMsg, of individual buffers of
    // BUF_SIZE.
    bcema_Blob blob(info->d_allocator_p);
    bcema_PooledBufferChain *chain = info->d_factory_p->allocate(0);
    btemt_DataMsg msg;
    btes_Iovec *vecs = (btes_Iovec *) info->d_allocator_p->allocate(numVecs *
                                                           sizeof(btes_Iovec));

    for (int i = 0 ;i < numVecs; ++i) {
        switch (info->d_write % 3) {
          case 0: {
            bcema_SharedPtr<char> buf(sharedBuffer, buffer + i * BUF_SIZE);
            blob.appendBuffer(bcema_BlobBuffer(buf,
                                               (i < numVecs-1) ? BUF_SIZE
                                                             : LAST_VEC_SIZE));
          } break;
          case 1: {
            chain->append(buffer + i * BUF_SIZE,
                          (i < numVecs-1) ? BUF_SIZE : LAST_VEC_SIZE);
          } break;
          case 2: {
            vecs[i].setBuffer(buffer + i * BUF_SIZE,
                              (i < numVecs-1) ? BUF_SIZE : LAST_VEC_SIZE);
          } break;
          default: ASSERT(0 && "Unreachable");
        }
    }
    switch (info->d_write % 3) {
      case 0: {
        blob.setLength(TOTAL_SIZE);
      } break;
      case 1: {
        msg.setData(chain, info->d_factory_p, info->d_allocator_p);
        msg.setChannelId(info->d_channelId);
      } break;
      case 2: {
      } break;
      default: ASSERT(0 && "Unreachable");
    }

    // Wait until all threads have initialized their buffers.
    info->d_barrier->wait();

    // Now write in a short burst, until write succeeds.
    switch (info->d_write % 3) {
      case 0: {
        while (0 != info->d_channelPool_p->write(info->d_channelId, blob)) {
            bcemt_ThreadUtil::yield();
        }
      } break;
      case 1: {
        while (0 != info->d_channelPool_p->write(info->d_channelId, msg)) {
            bcemt_ThreadUtil::yield();
        }
      } break;
      case 2: {
        while (0 != info->d_channelPool_p->write(info->d_channelId,
                                                 vecs, numVecs)) {
            bcemt_ThreadUtil::yield();
        }
      } break;
      default: ASSERT(0 && "Unreachable");
    }

    // Signal effective termination to write.
    ++(*(info->d_threadCount));
    info->d_barrier->wait();

    info->d_allocator_p->deallocate(vecs);
    if (0 != info->d_write % 3) {
        info->d_allocator_p->deallocate(buffer);
    }
    return NULL;
}
}

struct case9ReadInfo {
    btesos_TcpChannel  *d_channel_p;
    int                 d_concurrentReadFlag;
    int                 d_numThreads;
    bces_AtomicInt     *d_threadCount;
    int                 d_bufferAlloc;
    int                 d_totalAlloc;
    bslma_Allocator    *d_allocator_p;
};

extern "C" {
void *case9Read(void *arg)
{
    case9ReadInfo *info = (case9ReadInfo*) arg;
    const int NUM_THREADS = info->d_numThreads;
#ifdef BSLS_PLATFORM__OS_AIX
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
            bcemt_ThreadUtil::yield();
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
                        bdeu_Print::hexDump(bsl::cout, buffer, read);
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
                        bdeu_Print::hexDump(bsl::cout, buffer, read);
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
void case9ChannelStateCallback(int                 channelId,
                               int                 serverId,
                               int                 state,
                               void               *arg,
                               btemt_ChannelPool **poolAddr,
                               int               **eventAddr,
                               bcemt_Barrier      *barrier,
                               int                *channelId_p)
{
    ASSERT(poolAddr  && *poolAddr);
    ASSERT(eventAddr && *eventAddr);
    ASSERT(barrier);

    btemt_ChannelPool *pool = *poolAddr;

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << serverId
                   << " channelId=" << channelId
                   << " numChannels=" << pool->numChannels()
                   << MTENDL;
        }
        barrier->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
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
void runTestCase9(char                                         *progname,
                  bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                  bcema_TestAllocator&                          ta)
{
    using namespace TEST_CASE_21_NAMESPACE; // for dynamic_cast(SslLikeSocket)
    typedef bteso_SslLikeStreamSocketFactory<bteso_IPv4Address>
                                                          SslLikeSocketFactory;
    const int IS_CASE_21 = (dynamic_cast<SslLikeSocketFactory*>(factory) != 0);

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
#ifndef BSLS_PLATFORM__OS_AIX
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
        // exercise the callback inside btemt_Channel::writeMessage or
        // btemt_Channel::writeVecMessage.
#ifndef BSLS_PLATFORM__OS_AIX
        { L_,   0,    1024,     1024 * 2,          1024           },
        { L_,   0,    1024,     1024 * 4,          1024           },
        { L_,   0,    1024,     1024 * 8,          1024           },
        { L_,   0,    1024,     1024 * 15,         1024           },
#endif
        { L_,   0,    1024,     1024 * 17,         1024           },
#ifndef BSLS_PLATFORM__OS_AIX
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
#ifndef BSLS_PLATFORM__OS_AIX
        { L_,   0,    1024,     1024 * 17 - 128,   1024           },
        { L_,   0,    1024,     1024 * 17,         1024 - 128     },
        { L_,   0,    1024,     1024 * 17 + 128,   1024           },
        { L_,   0,    1024,     1024 * 17,         1024 + 128     },
#endif
        { L_,   0,    1024,     1024 * 17 + 128,   1024 + 128     },

#ifndef BSLS_PLATFORM__OS_AIX
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
#ifndef BSLS_PLATFORM__OS_AIX
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
        const int CONCURRENT_READ    = IS_CASE_21
                                     ? 1 // always concurrent read in case 21
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
            WRITING_THREADS    = 4
        };

        bcemt_Barrier channelBarrier(2);

        btemt_ChannelPool  *poolAddr;
        int                 poolEvent = -1;
        int                *eventAddr = &poolEvent;
        int                 channelId = -1;

        btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                bdef_BindUtil::bindA( &ta
                                    , &case9ChannelStateCallback
                                    , _1, _2, _3, _4
                                    , &poolAddr
                                    , &eventAddr
                                    , &channelBarrier
                                    , &channelId));

        btemt_ChannelPool::PoolStateChangeCallback    poolCb;
        makeNull(&poolCb);

        btemt_ChannelPool::DataReadCallback         dataCb;
        makeNull(&dataCb);

        btemt_ChannelPoolConfiguration cpc;
        cpc.setMaxConnections(MAX_THREADS);
        cpc.setMaxThreads(MAX_THREADS);
        cpc.setMetricsInterval(100.0);

        int maxOutgoingMessageSize = BUFFER_ALLOC;
        cpc.setOutgoingMessageSizes(0,
                                    (maxOutgoingMessageSize+1)/2,
                                    maxOutgoingMessageSize);

        Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;
        poolAddr = &mX;

        const bteso_IPv4Address ADDRESS("127.0.0.1", 0);

        int retCode = mX.start();
        LOOP_ASSERT(retCode, 0 == retCode);
        retCode = mX.listen(ADDRESS, BACKLOG, SERVER_ID);
        LOOP_ASSERT(retCode, 0 == retCode);
        ASSERT(0 == X.numChannels());
        const bteso_IPv4Address PEER = *mX.serverAddress(SERVER_ID);

        typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;
        typedef bteso_SslLikeStreamSocket<bteso_IPv4Address>     SslLikeSocket;
        typedef btesos_TcpChannel                                Channel;

        Socket  *socket = factory->allocate();
        Channel  channel(socket, &ta);

#if 0
        ASSERT(0 == channel.setOption(bteso_SocketOptUtil::BTESO_RECEIVEBUFFER,
                                      bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                      1024));

        ASSERT(0 == channel.setOption(bteso_SocketOptUtil::BTESO_TCPNODELAY,
                                      bteso_SocketOptUtil::BTESO_TCPLEVEL,
                                      1));
#endif

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
            return;
        }

        ASSERT(0 == mX.setServerSocketOption(
                                       bteso_SocketOptUtil::BTESO_SENDBUFFER,
                                       bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                       1024, SERVER_ID));

        ASSERT(0 == mX.setServerSocketOption(
                                       bteso_SocketOptUtil::BTESO_TCPNODELAY,
                                       bteso_SocketOptUtil::BTESO_TCPLEVEL,
                                       1, SERVER_ID));

        ASSERT(-3 == mX.write(31312313, (btes_Iovec*)NULL, 0));
        bcema_PooledBufferChainFactory bufferFactory(BUFFER_ALLOC, &ta);

        if (veryVerbose)
            cout << "Writing using blobs\n"
                 << "-------------------" << endl;
        {
            if (veryVerbose)
                cout << "\tWriting BIG_BUFFER_ALLOC" << endl;

            bces_AtomicInt count = 0;
            bcemt_ThreadUtil::Handle threadsHandle[WRITING_THREADS + 1];
            bcemt_Barrier            threadBarrier(WRITING_THREADS);
            bcemt_Attribute          attr;
            case9ThreadInfo2        info[WRITING_THREADS];
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
                ASSERT(0 == bcemt_ThreadUtil::create(&threadsHandle[i],
                                                     attr, &case9Thread0,
                                                     &info[i]));
            }

            if (veryVerbose) {
                cout << "\tReading BIG_BUFFER_ALLOC" << endl;
            }
            case9ReadInfo infoRead;
            infoRead.d_channel_p          = &channel;
            infoRead.d_threadCount        = &count;
            infoRead.d_concurrentReadFlag = CONCURRENT_READ;
            infoRead.d_numThreads         = WRITING_THREADS;
            infoRead.d_bufferAlloc        = BUFFER_ALLOC;
            infoRead.d_totalAlloc         = BIG_BUFFER_ALLOC;
            infoRead.d_allocator_p        = &ta;
            ASSERT(0 ==
                   bcemt_ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                            attr, &case9Read, &infoRead));

            // For reuse in case 21
            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threadsHandle[i], 0));
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
                ASSERT(0 == bcemt_ThreadUtil::create(&threadsHandle[i],
                                                     attr, &case9Thread0,
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
                   bcemt_ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                            attr, &case9Read, &infoRead));

            // For reuse in case 21
            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threadsHandle[i], 0));
            }

        }

        if (veryVerbose)
            cout << "Writing using datamsg\n"
                 << "---------------------" << endl;
        {
            if (veryVerbose)
                cout << "\tWriting BIG_BUFFER_ALLOC" << endl;

            bces_AtomicInt count = 0;
            bcemt_ThreadUtil::Handle threadsHandle[WRITING_THREADS+1];
            bcemt_Barrier            threadBarrier(WRITING_THREADS);
            bcemt_Attribute          attr;
            case9ThreadInfo2        info[WRITING_THREADS];
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
                ASSERT(0 == bcemt_ThreadUtil::create(&threadsHandle[i],
                                                     attr, &case9Thread1,
                                                     &info[i]));
            }

            if (veryVerbose) {
                cout << "\tReading BIG_BUFFER_ALLOC" << endl;
            }
            case9ReadInfo infoRead;
            infoRead.d_channel_p          = &channel;
            infoRead.d_threadCount        = &count;
            infoRead.d_concurrentReadFlag = CONCURRENT_READ;
            infoRead.d_numThreads         = WRITING_THREADS;
            infoRead.d_bufferAlloc        = BUFFER_ALLOC;
            infoRead.d_totalAlloc         = BIG_BUFFER_ALLOC;
            infoRead.d_allocator_p        = &ta;
            ASSERT(0 ==
                   bcemt_ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                            attr, &case9Read, &infoRead));

            // For reuse in case 21
            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threadsHandle[i], 0));
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
                ASSERT(0 == bcemt_ThreadUtil::create(&threadsHandle[i],
                                                     attr, &case9Thread1,
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
                   bcemt_ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                            attr, &case9Read, &infoRead));

            // For reuse in case 21
            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threadsHandle[i], 0));
            }

        }

        if (veryVerbose)
            cout << "Writing using iovecs\n"
                 << "--------------------" << endl;
        {
            if (veryVerbose)
                cout << "\tWriting BIG_BUFFER_ALLOC" << endl;

            bces_AtomicInt count = 0;
            bcemt_ThreadUtil::Handle threadsHandle[WRITING_THREADS+1];
            bcemt_Barrier            threadBarrier(WRITING_THREADS);
            bcemt_Attribute          attr;
            case9ThreadInfo         info[WRITING_THREADS];
            for (int i = 0; i < WRITING_THREADS; ++i) {
                info[i].d_barrier       = &threadBarrier;
                info[i].d_threadCount   = &count;
                info[i].d_channelPool_p = &mX;
                info[i].d_channelId     = channelId;
                info[i].d_write         = i;
                info[i].d_bufferAlloc   = BUFFER_ALLOC;
                info[i].d_totalAlloc    = BIG_BUFFER_ALLOC;
                info[i].d_allocator_p   = &ta;
                ASSERT(0 == bcemt_ThreadUtil::create(&threadsHandle[i],
                                                     attr, &case9Thread2,
                                                     &info[i]));
            }

            if (veryVerbose) {
                cout << "\tReading BIG_BUFFER_ALLOC" << endl;
            }
            case9ReadInfo infoRead;
            infoRead.d_channel_p          = &channel;
            infoRead.d_threadCount        = &count;
            infoRead.d_concurrentReadFlag = CONCURRENT_READ;
            infoRead.d_numThreads         = WRITING_THREADS;
            infoRead.d_bufferAlloc        = BUFFER_ALLOC;
            infoRead.d_totalAlloc         = BIG_BUFFER_ALLOC;
            infoRead.d_allocator_p        = &ta;
            ASSERT(0 ==
                   bcemt_ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                            attr, &case9Read, &infoRead));

            // For reuse in case 21
            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threadsHandle[i], 0));
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
                ASSERT(0 == bcemt_ThreadUtil::create(&threadsHandle[i],
                                                     attr, &case9Thread2,
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
                   bcemt_ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                            attr, &case9Read, &infoRead));

            // For reuse in case 21
            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threadsHandle[i], 0));
            }
        }

        if (veryVerbose)
            cout << "Writing using all three flavors of write()\n"
                 << "------------------------------------------" << endl;

        {
            if (veryVerbose) {
                cout << "\tWriting BIG_BUFFER_ALLOC" << endl;
            }

            bces_AtomicInt count = 0;
            bcemt_ThreadUtil::Handle threadsHandle[WRITING_THREADS+1];
            bcemt_Barrier            threadBarrier(WRITING_THREADS);
            bcemt_Attribute          attr;
            case9ThreadInfo2        info[WRITING_THREADS];
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
                ASSERT(0 == bcemt_ThreadUtil::create(&threadsHandle[i],
                                                     attr, &case9Thread3,
                                                     &info[i]));
            }

            if (veryVerbose) {
                cout << "\tReading BIG_BUFFER_ALLOC" << endl;
            }
            case9ReadInfo infoRead;
            infoRead.d_channel_p          = &channel;
            infoRead.d_threadCount        = &count;
            infoRead.d_concurrentReadFlag = CONCURRENT_READ;
            infoRead.d_numThreads         = WRITING_THREADS;
            infoRead.d_bufferAlloc        = BUFFER_ALLOC;
            infoRead.d_totalAlloc         = BIG_BUFFER_ALLOC;
            infoRead.d_allocator_p        = &ta;
            ASSERT(0 ==
                   bcemt_ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                            attr, &case9Read, &infoRead));

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threadsHandle[i], 0));
            }

            // For reuse in case 21
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
                ASSERT(0 == bcemt_ThreadUtil::create(&threadsHandle[i],
                                                     attr, &case9Thread3,
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
                   bcemt_ThreadUtil::create(&threadsHandle[WRITING_THREADS],
                                            attr, &case9Read, &infoRead));

            // For reuse in case 21
            if (sSocket) {
                sSocket->writeFlush();
            }

            for (int i = 0; i <= WRITING_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threadsHandle[i], 0));
            }
        }

        channel.invalidate();
        factory->deallocate(socket);
        channelBarrier.wait();
    }
}

} // closing namespace TEST_CASE_9_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 8 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_8_NAMESPACE {

struct case8CallbackInfo {
    bcemt_Barrier  *d_barrier_p;
    bces_AtomicInt  d_channelId;
    bces_AtomicInt  d_channelDownReadFlag;
    bces_AtomicInt  d_channelDownWriteFlag;
};

static
void case8ChannelStateCallback(int                channelId,
                               int                serverId,
                               int                state,
                               void              *arg,
                               case8CallbackInfo *info)
{
    ASSERT(info);

    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN_READ: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated (read only):"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        ++info->d_channelDownReadFlag;
        info->d_barrier_p->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN_WRITE: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated (write only):"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        ++info->d_channelDownWriteFlag;
        info->d_barrier_p->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        info->d_barrier_p->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
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

} // closing namespace TEST_CASE_8_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 7 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_7_NAMESPACE {

static
void case7ChannelStateCallback(
        int            channelId,
        int            serverId,
        int            state,
        void          *arg,
        bcemt_Barrier *barrier)
{
    ASSERT(barrier);

    switch(state){
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (veryVerbose) {
            MTCOUT << "Connection terminated:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
        barrier->wait();
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        if (veryVerbose) {
            MTCOUT << "Connection established:"
                   << " serverId="  << bsl::hex << serverId << bsl::dec
                   << " channelId=" << channelId
                   << MTENDL;
        }
      } break;
    }
}

} // closing namespace TEST_CASE_7_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 6 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_6_NAMESPACE {

} // closing namespace TEST_CASE_6_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 5 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_5_NAMESPACE {

} // closing namespace TEST_CASE_5_NAMESPACE

//-----------------------------------------------------------------------------
// CASE 4 supporting classes and methods
//-----------------------------------------------------------------------------
namespace TEST_CASE_4_NAMESPACE {

struct case4WorkerInfo {
    int                       d_i;
    int                       d_expUserId;
    int                       d_expNumFailures;
    int                       d_queueSize;
    int                       d_timeOut;
    bces_AtomicInt            d_portNumber;
    bcemt_ThreadUtil::Handle *d_worker_p;
};

extern "C"
void *case4OpenConnectThread(void *arg) {
    case4WorkerInfo *info = (case4WorkerInfo*) arg;

    const int i            = info->d_i;
    const int QUEUE_SIZE   = info->d_queueSize;
    const int TIMEOUT      = info->d_timeOut;
    const int PORT_NUMBER  = info->d_portNumber;
    ASSERT(0 != PORT_NUMBER);

    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
    bteso_IPv4Address serverAddress;
    serverAddress.setPortNumber(PORT_NUMBER);

    btesos_TcpTimedAcceptor acceptor(&factory);

    if (veryVerbose) {
        MTCOUT << bdetu_SystemTime::now() << ": OPENING SERVER@ "
               << PORT_NUMBER << endl << MTENDL;
    }
    if (0 != acceptor.open(serverAddress, QUEUE_SIZE)) {
        MTCOUT << "bdetu_SystemTime::now() = " << bdetu_SystemTime::now()
               << ": ERROR OPENING SERVER.\n"
               << MTENDL;
        // Leave port number to non 0, to signal opening error in main thread.
        return 0;
    }

    int status;
    bdet_TimeInterval timeout = bdetu_SystemTime::now() + TIMEOUT * 3;
    btesc_Channel *channel = acceptor.timedAllocate(&status, timeout);

    if (channel) {
        if (veryVerbose) {
            MTCOUT << "bdetu_SystemTime::now() = " << bdetu_SystemTime::now()
                   << ": CLIENT CONNECTED.\n"
                   << MTENDL;
        }
        if (channel) {
            acceptor.deallocate(channel);
        }
        if (veryVerbose) {
            MTCOUT << bdetu_SystemTime::now() << ": CLOSING SERVER@ "
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

static void case4PoolStateCb(int              poolState,
                             int              sourceId,
                             int              severity,
                             // additional arguments follow
                             bces_AtomicInt  *numFailures,
                             case4WorkerInfo *info)
{
    if (veryVerbose) {
        MTCOUT << "PoolStateCb: bdetu_SystemTime::now() = "
               << bdetu_SystemTime::now() << endl;
        switch(poolState) {
#define CASE(X) case X: cout << "\tstate = " << #X << endl; break;
            CASE(btemt_PoolMsg::BTEMT_ACCEPT_TIMEOUT);
            CASE(btemt_PoolMsg::BTEMT_ERROR_ACCEPTING);
            CASE(btemt_PoolMsg::BTEMT_ERROR_CONNECTING);
            CASE(btemt_PoolMsg::BTEMT_CHANNEL_LIMIT);
            CASE(btemt_PoolMsg::BTEMT_CAPACITY_LIMIT);
#undef CASE
            default: cout << "Unknown pool state: "; P(poolState); break;
        }
        T_(); P_(sourceId); P_(info->d_expUserId); P(severity);
        T_(); P_(*numFailures); P(info->d_expNumFailures);
        cout << MTENDL;
    }

    if (btemt_PoolMsg::BTEMT_CHANNEL_LIMIT == poolState) {
        ASSERT(btemt_ChannelPool::BTEMT_ALERT == severity);  // ALERT
        return;
    }

    ASSERT(btemt_PoolMsg::BTEMT_ERROR_CONNECTING == poolState);
    ASSERT(0 <= *numFailures);
    ++(*numFailures);
    LOOP3_ASSERT(*numFailures, info->d_expNumFailures, info->d_portNumber,
            *numFailures <= info->d_expNumFailures || info->d_portNumber);
    ASSERT(info->d_expUserId == sourceId);

    if (*numFailures == info->d_expNumFailures) {
        // Now is the time to let the last connect attempt succeed.

        bcemt_ThreadUtil::create(info->d_worker_p, &case4OpenConnectThread,
                                 (void *)info);
    }
}

static void case4ChannelCb(int                channelId,
                           int                sourceId,
                           int                state,
                           void              *context,
                           // additional arguments follow
                           int                *channelIdPtr,
                           bces_AtomicInt     *isInvokedFlag,
                           bces_AtomicInt     *numFailures,
                           case4WorkerInfo    *info,
                           btemt_ChannelPool **pool)
{
    if (veryVerbose) {
        MTCOUT << "ChannelCb: bdetu_SystemTime::now() = "
               << bdetu_SystemTime::now() << endl;
        switch(state) {
#define CASE(X) case X: cout << "\tstate = " << #X << endl; break;
            CASE(btemt_ChannelPool::BTEMT_CHANNEL_UP);
            CASE(btemt_ChannelPool::BTEMT_CHANNEL_DOWN);
            CASE(btemt_ChannelPool::BTEMT_READ_TIMEOUT);
            CASE(btemt_ChannelPool::BTEMT_WRITE_BUFFER_FULL);
            CASE(btemt_ChannelPool::BTEMT_AUTO_READ_ENABLED);
            CASE(btemt_ChannelPool::BTEMT_AUTO_READ_DISABLED);
            CASE(btemt_ChannelPool::BTEMT_WRITE_CACHE_LOWWAT);
            CASE(btemt_ChannelPool::BTEMT_CHANNEL_DOWN_READ);
            CASE(btemt_ChannelPool::BTEMT_CHANNEL_DOWN_WRITE);
#undef CASE
            default: cout << "Unknown channel state: "; P(state); break;
        }
        T_(); P_(channelId); P(sourceId);
        T_(); P(info->d_expUserId);
        T_(); P_(*numFailures); P(info->d_expNumFailures);
        cout << MTENDL;
    }

    ASSERT(info->d_expUserId == sourceId);

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_UP:
        ASSERT(*isInvokedFlag == 0);
        ASSERT(*numFailures <= info->d_expNumFailures);

        // NOTE: if port was open by another test driver running concurrently,
        // we may connect earlier than we should: do not assert.

        *channelIdPtr = channelId;
        *isInvokedFlag += 1;
        break;

      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN:
        (*pool)->close(channelId);
        break;

      case btemt_ChannelPool::BTEMT_AUTO_READ_ENABLED:
        *isInvokedFlag += 2;
        break;

      default:
        ASSERT(!"Unexpected channel state callback in case4ChannelCallback");
        P(state);
    }
}

static void case4ErrorPoolStateCb(int             poolState,
                                  int             sourceId,
                                  int             severity,
                                  int             expectedSourceId,
                                  int             expectedSeverity,
                                  bces_AtomicInt *isInvokedFlag)
{
    ASSERT(expectedSourceId == sourceId);
    ASSERT(expectedSeverity == severity);
    ASSERT(btemt_PoolMsg::BTEMT_ERROR_CONNECTING == poolState);
    if (veryVerbose) {
        PT(bdetu_SystemTime::now());
        PT(btemt_PoolMsg::PoolState(poolState));
        PT(sourceId);
        PT(severity);
    }
    ++(*isInvokedFlag);
}

} // closing namespace TEST_CASE_XX_NAMESPACE

//=============================================================================
//                  USAGE EXAMPLES
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// monitorPool, from usage example 2, is reused.
//-----------------------------------------------------------------------------

static inline void  monitorPool(bcemt_Mutex              *coutLock,
                                const btemt_ChannelPool&  pool,
                                int                       numTimes,
                                bool                      verbose = true)
    // With the period of 2 seconds up to the specified 'numTimes' times,
    // output the percent busy of the specified channel 'pool' to
    // the standard output using the specified 'coutLock' for synchronizing
    // access to a standard output stream.
{
    while (0 <= --numTimes) {
        bcemt_ThreadUtil::yield();
        bcemt_ThreadUtil::microSleep(0, 2);  // 2 seconds
        if (verbose) {
            coutLock->lock();
            bsl::cout << bdetu_SystemTime::nowAsDatetimeGMT() << "\n"
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

//-----------------------------------------------------------------------------
/// Usage Example 1:
// Appears unused
//-----------------------------------------------------------------------------

struct my_LocalCallback {
    int d_status;
    int d_id;
    int d_channelExpected;
    void connectCb(int channelId, int id, int status, void *context) {
        MTCOUT << "Callback invoked" << MTENDL;
        ASSERT(id == d_id);
        ASSERT(d_status == status);
    }
};

int usageExample1(bslma_Allocator *allocator) {
    my_LocalCallback testCb;
    btemt_ChannelPoolConfiguration config;
    config.setMaxThreads(2);
    config.setMetricsInterval(10.0);

    btemt_ChannelPool::ChannelStateChangeCallback channelCb(
            bdef_MemFnUtil::memFn(&my_LocalCallback::connectCb, &testCb)
          , allocator);

    btemt_ChannelPool::PoolStateChangeCallback    poolCb;
    makeNull(&poolCb);

    btemt_ChannelPool::DataReadCallback         dataCb;
    makeNull(&dataCb);

    testCb.d_id = 5;
    testCb.d_status = btemt_ChannelPool::BTEMT_CHANNEL_UP;
    testCb.d_channelExpected = 1;
    btemt_ChannelPool pool(channelCb, dataCb, poolCb, config, allocator);
    bteso_IPv4Address peer("127.0.0.1", 7); // echo server
    if (0 != pool.connect(peer, 1, bdet_TimeInterval(10.0), 5)) {
        // Some machines do not run an echo server
        if (veryVerbose) {
            cout << "The local host does not appear to be running "
                 << "an echo server." << endl;
        }
        return 1;
    }
    ASSERT(0 == pool.start());
    enum { NUM_MONITOR = 5 };
    MTCOUT << "monitor pool: count=" << NUM_MONITOR << MTENDL;
    monitorPool(&coutMutex, pool, NUM_MONITOR);
    MTCOUT << "Stopping the event manager" << MTENDL;
    ASSERT(0 == pool.stop());
    MTCOUT << "Exiting" << MTENDL;
    return 0;
}

class my_Clock {
    int   d_numInvocations;
    int   d_totalNumber;
    int   d_regId;
    btemt_ChannelPool *d_pool;
  private:
    void clockCb() {
        if (++d_numInvocations == d_totalNumber) {
            d_pool->deregisterClock(d_regId);
        }
    }
  public:
      my_Clock(int numCbs, btemt_ChannelPool *pool, int Id)
      : d_numInvocations(0)
      , d_totalNumber(numCbs)
      , d_pool(pool) {
            bdef_Function<void (*)()> functor(
                    bdef_MemFnUtil::memFn(&my_Clock::clockCb, this));

            d_regId = d_pool->registerClock(functor, bdetu_SystemTime::now(),
                                            bdet_TimeInterval(0, 50000),
                                            Id);
            ASSERT(0 <= d_regId);
        }
      ~my_Clock();
};

my_Clock::~my_Clock() {
    if (veryVerbose) {
        P(d_numInvocations);
        P(d_totalNumber);
    }
    ASSERT(d_numInvocations == d_totalNumber);
}

//-----------------------------------------------------------------------------
// USAGE EXAMPLE 3: A variable-length-message echo server.
//-----------------------------------------------------------------------------
namespace USAGE_EXAMPLE_3_NAMESPACE {

class vlm_EchoServer
{
    // A variable-length-message echo server.  Messages are formatted
    // as a four-byte length value (in network byte order) followed
    // by a null-terminated string.

    enum { SERVER_ID = 1066 };

    int    d_port;           // well-known port for service requests

    bslma_Allocator                  *d_allocator_p;
    bcema_PooledBufferChainFactory   *d_pbcFactory_p;
    btemt_ChannelPool                *d_cp_p;

private:
    // ChannelPool Callback Functions
    void poolCB(int state, int source, int severity);
    void chanCB(int channelId, int serverId, int state, void *arg);
    void dataCB(int *consumed, int *needed, btemt_DataMsg msg, void *arg);

    // Not Implemented
    vlm_EchoServer(const vlm_EchoServer&);
    vlm_EchoServer& operator=(const vlm_EchoServer&);

public:
    // CREATORS
    vlm_EchoServer(int                port,
                   bslma_Allocator   *allocator = 0);
        // Create a server object which accepts connections
        // on localhost at the specified by 'port'.  The server
        // uses the specified 'allocator' for internal memory
        // management.

   ~vlm_EchoServer();
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
    int portNumber();
        // Return the port number on the local host on which this server
        // listens to connections.
};

vlm_EchoServer::vlm_EchoServer(
        int                port,
        bslma_Allocator   *allocator)
: d_port(port)
, d_allocator_p(bslma_Default::allocator(allocator))
{
    enum { BUFSIZE = 1 << 10 };    // 1K buffers
    d_pbcFactory_p = new(*d_allocator_p)
                         bcema_PooledBufferChainFactory(BUFSIZE);

    btemt_ChannelPool::PoolStateChangeCallback    poolCb(
            bdef_MemFnUtil::memFn(&vlm_EchoServer::poolCB, this)
          , d_allocator_p);

    btemt_ChannelPool::ChannelStateChangeCallback channelCb(
            bdef_MemFnUtil::memFn(&vlm_EchoServer::chanCB, this)
          , d_allocator_p);

    btemt_ChannelPool::DataReadCallback         dataCb(
            bdef_MemFnUtil::memFn(&vlm_EchoServer::dataCB, this)
          , d_allocator_p);

    btemt_ChannelPoolConfiguration    cpc;
    cpc.setMaxConnections(5);
    cpc.setMaxThreads(1);
    cpc.setMetricsInterval(10.0);
    cpc.setIncomingMessageSizes(sizeof(int), 1<<8, 1024*2);

    d_cp_p = new(*d_allocator_p)
                 btemt_ChannelPool(channelCb, dataCb, poolCb, cpc, allocator);
}

vlm_EchoServer::~vlm_EchoServer()
{
    d_cp_p->stop();
    d_allocator_p->deleteObjectRaw(d_cp_p);
    d_allocator_p->deleteObjectRaw(d_pbcFactory_p);
}

int vlm_EchoServer::start()
{
    int    src = d_cp_p->start();
    int    lrc = d_cp_p->listen(d_port, 5, SERVER_ID);
    d_port = d_cp_p->serverAddress(SERVER_ID)->portNumber();
    return src || lrc;
}

int vlm_EchoServer::stop()
{
    return d_cp_p->stop();
}

int vlm_EchoServer::portNumber()
{
    return d_cp_p->serverAddress(SERVER_ID)->portNumber();
}

void vlm_EchoServer::poolCB(int state, int source, int severity)
{
}

void vlm_EchoServer::chanCB(int channelId, int serverId, int state, void *arg)
{
    if (btemt_ChannelPool::BTEMT_CHANNEL_DOWN == state) {
        d_cp_p->shutdown(channelId, btemt_ChannelPool::BTEMT_IMMEDIATE);
    }
}

void vlm_EchoServer::dataCB(
        int             *consumed,
        int             *needed,
        btemt_DataMsg    msg,
        void            *arg)
{
    ASSERT(consumed);
    ASSERT(needed);
    ASSERT(msg.data());

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

    // Process Request
    ASSERT(1 == msg.data()->numBuffers());
    ASSERT(length == (int)(msg.data()->length() - sizeof length));

    d_cp_p->write(msg.channelId(), msg);
}

} // closing namespace USAGE_EXAMPLE_3_NAMESPACE

//-----------------------------------------------------------------------------
/// USAGE EXAMPLE 2: queue-based message processor
//-----------------------------------------------------------------------------
namespace USAGE_EXAMPLE_2_NAMESPACE {

static
int parseMessages(int *numNeeded, bcema_PooledBufferChain *chain) {
    ASSERT(numNeeded);
    char *data = chain->buffer(0);
    int   length = chain->length();
    ASSERT(data);
    int originalLength  = length;
    int processedMessages = 0;
    while (length > (int) sizeof(int)) {
        int msgLength;
        memcpy(&msgLength, data, sizeof(int));
        msgLength = ntohl(msgLength);
        ASSERT(0 <= msgLength);
        // As a guard against malicious user, the msgLength should be limited
        // in the protocol.
        if (length < msgLength) {
            *numNeeded = msgLength - length;
            return originalLength - length;
        }
        length -= msgLength;
        data += msgLength;
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

    btemt_ChannelPoolConfiguration d_config;          // pool's configuration
    btemt_ChannelPool             *d_channelPool_p;   // managed pool
    bslma_Allocator               *d_allocator_p;     // memory manager
    bcemt_Mutex                   *d_coutLock_p;
    bcec_Queue<btemt_DataMsg>     *d_incomingQueue_p;
    bcec_Queue<btemt_DataMsg>     *d_outgoingQueue_p;
    bcemt_ThreadUtil::Handle       d_processorHandle;
    bces_AtomicUtil::Int           d_runningFlag;
  private:
    // Callback functions:
    void poolCb(int state, int source, int severity);
    void channelStateCb(int channelId, int sourceId, int state, void *context);
    void dataCb(int *numConsumed, int *numNeeded, btemt_DataMsg msg,
                void *context);

    // Not implemented:
    my_QueueProcessor(const my_QueueProcessor& original);        // Not impl.
    my_QueueProcessor& operator=(const my_QueueProcessor& rhs);  // Not impl.
  public:
    // CREATORS
    my_QueueProcessor(bcec_Queue<btemt_DataMsg> *incomingQueue,
                      bcec_Queue<btemt_DataMsg> *outgoingQueue,
                      bcemt_Mutex *coutLock,
                      int portNumber, int numConnections,
                      bslma_Allocator *basicAllocator = 0);

    ~my_QueueProcessor();
        // Destroy this server.

    // MANIPULATORS
    int processOutgoingQueue();

    int startProcessor();

    int stopProcessor();

    const btemt_ChannelPool& pool() { return *d_channelPool_p; }

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

my_QueueProcessor::my_QueueProcessor(bcec_Queue<btemt_DataMsg> *incomingQueue,
                                     bcec_Queue<btemt_DataMsg> *outgoingQueue,
                                     bcemt_Mutex               *coutLock ,
                                     int                        portNumber,
                                     int                        numConnections,
                                     bslma_Allocator           *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_coutLock_p(coutLock)
, d_incomingQueue_p(incomingQueue)
, d_outgoingQueue_p(outgoingQueue)
{
    bces_AtomicUtil::initInt(&d_runningFlag, 0);
    d_config.setMaxThreads(4);
    d_config.setMaxConnections(numConnections);
    d_config.setReadTimeout(5.0);      // in seconds
    d_config.setMetricsInterval(10.0); // seconds
    d_config.setWriteCacheWatermarks(0, 1<<20);  // 1Mb
    d_config.setIncomingMessageSizes(4, 100, 1024);

    if (verbose) {
        PT(d_config);
    }

    btemt_ChannelPool::ChannelStateChangeCallback channelCb(
            bdef_MemFnUtil::memFn(&my_QueueProcessor::channelStateCb, this)
          , basicAllocator);

    btemt_ChannelPool::DataReadCallback         dataCb(
            bdef_MemFnUtil::memFn(&my_QueueProcessor::dataCb, this)
          , basicAllocator);

    btemt_ChannelPool::PoolStateChangeCallback    poolCb(
            bdef_MemFnUtil::memFn(&my_QueueProcessor::poolCb, this)
          , basicAllocator);

    d_channelPool_p = new (*d_allocator_p) btemt_ChannelPool(channelCb,
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
    while (bces_AtomicUtil::getInt(d_runningFlag)) {
        bdet_TimeInterval TIMEOUT(bdetu_SystemTime::now());
        TIMEOUT += 5.0; // 5s
        btemt_DataMsg msg;
        if (0 == d_outgoingQueue_p->timedPopBack(&msg, TIMEOUT)) {
            if (0 != d_channelPool_p->write(msg.channelId(), msg)) {
                if (veryVerbose) {
                    d_coutLock_p->lock();
                    bsl::cout << "Channel " << msg.channelId() << " is down."
                              << bsl::endl;
                    d_coutLock_p->unlock();
                }
                d_channelPool_p->shutdown(msg.channelId(),
                                          btemt_ChannelPool::BTEMT_IMMEDIATE);
            }
        }
    }
    return 0;
}

int my_QueueProcessor::startProcessor() {
    bcemt_Attribute attributes;
    attributes.setDetachedState(bcemt_Attribute::BCEMT_CREATE_JOINABLE);
    bces_AtomicUtil::setInt(&d_runningFlag, 1);
    return bcemt_ThreadUtil::create(&d_processorHandle, attributes,
                                    &queueProc, (void*)this);
}

int my_QueueProcessor::stopProcessor() {
    bces_AtomicUtil::setInt(&d_runningFlag, 0);
    return bcemt_ThreadUtil::join(d_processorHandle);
}

int my_QueueProcessor::portNumber() {
    return d_channelPool_p->serverAddress(SERVER_ID)->portNumber();
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
                                       void *context)
{
    ASSERT(SERVER_ID == sourceId);

    switch(state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
          bteso_IPv4Address peer;
          d_channelPool_p->getPeerAddress(&peer, channelId);
          if (veryVerbose) {
              MTCOUT << "Client " << channelId << " from " << peer
                     << " has disconnected on server " << sourceId
                     << "." << MTENDL;
          }
          d_channelPool_p->shutdown(channelId,
                                    btemt_ChannelPool::BTEMT_IMMEDIATE);
      } break;
      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
          bteso_IPv4Address peer;
          d_channelPool_p->getPeerAddress(&peer, channelId);
          if (veryVerbose) {
              MTCOUT << "Client " << channelId << " connected from " << peer
                     << " on server " << sourceId << "." << MTENDL;
          }
      } break;
    }
}

void my_QueueProcessor::dataCb(int *numConsumed, int *numNeeded,
                               btemt_DataMsg msg, void *context) {
    ASSERT(numConsumed);
    ASSERT(msg.data());
    int length = msg.data()->length();
    ASSERT(0 < length);

    *numConsumed = parseMessages(numNeeded, msg.data());

    if (veryVeryVerbose) {
        MTCOUT << "Received " << msg.data()->length() << " bytes from "
               << msg.channelId() << " " << *numConsumed << " processed; "
               << *numNeeded << " needed to complete a message." << MTENDL;
    }

    if (*numConsumed) {
        d_incomingQueue_p->pushBack(msg);
    }
}

} // closing namespace USAGE_EXAMPLE_2_NAMESPACE

//-----------------------------------------------------------------------------
/// USAGE EXAMPLE 2: echo server
//-----------------------------------------------------------------------------
namespace USAGE_EXAMPLE_2_NAMESPACE {

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
        btemt_ChannelPoolConfiguration d_config;        // pool's configuration
        btemt_ChannelPool             *d_channelPool_p; // managed pool
        bslma_Allocator               *d_allocator_p;   // memory manager
        bcemt_Mutex                   *d_coutLock_p;    // synchronize 'cout'

      private:
        // Callback functions:
        void poolStateCb(int state, int source, int severity);
            // Output a message to 'stdout' indicating the specified 'state'
            // associated with the specified 'source' has occurred, with the
            // specified 'severity'.  Note that 'state' is one of the
            // 'btemt_PoolMsg' constants (see 'btemt_message'), 'source'
            // identifies the channel pool operation associated with this state
            // (in this case, this must be the 'SERVER_ID' passed to
            // 'listen()'), and 'severity' is one of the
            // 'btemt_ChannelPool::Severity' values.

        void channelStateCb(int channelId, int sourceId, int state, void *ctx);
            // Output a message to 'stdout' indicating the specified 'state',
            // associated with the specified 'channelId' and 'sourceId', has
            // occured.  If 'state' is 'btemt_ChannelPool::BTEMT_CHANNEL_DOWN'
            // then shutdown the channel.  Note that the 'channelId' is a
            // unique identifier chosen by the channel pool for each connection
            // channel, 'sourceId' identifies the channel pool operation
            // responsible for creating the channel (in this case, this must
            // be the 'SERVER_ID' passed to 'listen()'), state is a
            // 'btemt_ChannelPool::ChannelState' enumeration value, and 'ctx'
            // is the address of a context object provided for the channel
            // (using 'setChannelContext()'), in this example we do not
            // specify a context, so the value will be 0.

        void dataCb(int           *numConsumed,
                    int           *numNeeded,
                    btemt_DataMsg  msg,
                    void          *context);
            // Echo the specified 'msg' to the client on the channel
            // identified by 'msg.channelId()' channel, load into the
            // specified 'numConsumed' the number of bytes processed from
            // 'msg', load into 'numNeeeded' the minimum length of
            // additional data that is needed to complete a message, and close
            // the communication channel.  Because this echo server is not
            // interested in a discrete messages in a particular message
            /// format, 'numConsumed' will always be set to the length of
            // 'msg', and 'numNeeded' will be set to 1 (indicating this
            // callback should be invoked again as soon as any new data is
            // read).

        // NOT IMPLEMENTED
        my_EchoServer(const my_EchoServer&);
        my_EchoServer& operator=(const my_EchoServer&);

      public:
        my_EchoServer(bcemt_Mutex     *coutLock,
                      int              portNumber,
                      int              numConnections,
                      bslma_Allocator *basicAllocator = 0);
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
        const btemt_ChannelPool& pool() { return *d_channelPool_p; }
    };
//..
// In the constructor of 'my_EchoServer', the configuration is initialized,
// the channel pool is created, configured, and started.  The listening port
// is established:
//..
    my_EchoServer::my_EchoServer(bcemt_Mutex     *coutLock,
                                 int              portNumber,
                                 int              numConnections,
                                 bslma_Allocator *basicAllocator)
    : d_allocator_p(bslma_Default::allocator(basicAllocator))
    , d_coutLock_p(coutLock) {
        d_config.setMaxThreads(4);
        d_config.setMaxConnections(numConnections);
        d_config.setReadTimeout(5.0);       // in seconds
        d_config.setMetricsInterval(10.0);  // seconds
        d_config.setMaxWriteCache(1<<10);   // 1MB
        d_config.setIncomingMessageSizes(1, 100, 1024);

        btemt_ChannelPool::ChannelStateChangeCallback channelStateFunctor(
                bdef_MemFnUtil::memFn(&my_EchoServer::channelStateCb, this)
              , basicAllocator);

        btemt_ChannelPool::PoolStateChangeCallback poolStateFunctor(
                bdef_MemFnUtil::memFn(&my_EchoServer::poolStateCb, this)
              , basicAllocator);

        btemt_ChannelPool::DataReadCallback dataFunctor(
                bdef_MemFnUtil::memFn(&my_EchoServer::dataCb, this)
              , basicAllocator);

        d_channelPool_p = new (*d_allocator_p)
            btemt_ChannelPool(channelStateFunctor,
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
                                       void *context)
    {
        ASSERT(SERVER_ID == sourceId);

        switch(state) {
          case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
              bteso_IPv4Address peer;
              d_channelPool_p->getPeerAddress(&peer, channelId);
              d_coutLock_p->lock();
              MTCOUT << "Client from " << peer << " has disconnected."
                     << MTENDL;
              d_coutLock_p->unlock();
              d_channelPool_p->shutdown(channelId,
                                        btemt_ChannelPool::BTEMT_IMMEDIATE);
          } break;
          case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
              bteso_IPv4Address peer;
              d_channelPool_p->getPeerAddress(&peer, channelId);
              d_coutLock_p->lock();
              MTCOUT << "Client connected from " << peer << MTENDL;
              d_coutLock_p->unlock();
          } break;
        }
    }

    void my_EchoServer::dataCb(int           *numConsumed,
                               int           *numNeeded,
                               btemt_DataMsg  msg,
                               void          *context)
    {
        ASSERT(numConsumed);
        ASSERT(msg.data());
        ASSERT(0 < msg.data()->length());

        ASSERT(0 == d_channelPool_p->write(msg.channelId(), msg));
        *numConsumed = msg.data()->length();
        *numNeeded   = 1;
        MTCOUT << "Shutdown " << msg.channelId() << MTENDL;
        d_channelPool_p->shutdown(msg.channelId(),
                                  btemt_ChannelPool::BTEMT_IMMEDIATE);
    }
//..
// The implementation of an echo server is now complete.  Let's create
// a small program that uses it.  We will create a server object, then
// the main thread will monitor the channel pool and periodically print
// its busy metrics.  For simplicity, we will use the following function
// for monitoring:
//..
    static inline void  monitorPool(bcemt_Mutex              *coutLock,
                                    const btemt_ChannelPool&  pool,
                                    int                       numTimes)
        // Every 10 seconds, output the percent busy of the specified channel
        // 'pool' to the standard output, using the specified 'coutLock' to
        // synchronizing access to the standard output stream; return to the
        // caller after 'numTimes' output operations (i.e. numTimes * 10
        // seconds).
    {
        while(--numTimes > 0) {
            coutLock->lock();
            MTCOUT << "The pool is " << pool.busyMetrics() << "% busy ("
                   << pool.numThreads() << " threads)." << MTENDL;
            coutLock->unlock();
            bcemt_ThreadUtil::sleep(bdet_TimeInterval(10*1E6));  // 10 seconds
        }
    }
//..

} // closing namespace USAGE_EXAMPLE_2_NAMESPACE

//-----------------------------------------------------------------------------
/// ADDITIONAL USAGE EXAMPLE: queue-based client (used in case -1)
//-----------------------------------------------------------------------------

namespace USAGE_EXAMPLE_M1_NAMESPACE {

static void
generateMessage(btemt_DataMsg& msg, bslma_Allocator *basicAllocator = 0) {
    bcema_PooledBufferChain *chain = msg.data();
    bdet_TimeInterval now = bdetu_SystemTime::now();
    bdex_ByteOutStream stream(basicAllocator);
    bdex_OutStreamFunctions::streamOut(stream, now, 1);
    int streamedLength = stream.length();
    chain->setLength(sizeof(int) + streamedLength);
    chain->replace(sizeof(int), stream.data(), streamedLength);
    char *buffer = chain->buffer(0);
    int tmp = htonl(streamedLength + sizeof(int));
    memcpy(buffer, &tmp, sizeof(int));
}

class my_QueueClient {
    // This class implements a queue-based message TCP/IP server.

    enum {
        CLIENT_BASE_ID = 0xAB      // Actual clientId is between this and
                                   // CLIENT_BASE_ID + d_numConnections.
    };
    btemt_ChannelPoolConfiguration d_config;          // pool's configuration
    btemt_ChannelPool             *d_channelPool_p;   // managed pool
    bslma_Allocator               *d_allocator_p;     // memory manager
    bcemt_Mutex                   *d_coutLock_p;
    bcec_Queue<btemt_DataMsg>     *d_incomingQueue_p;
    bcec_Queue<btemt_DataMsg>     *d_outgoingQueue_p;
    bcemt_ThreadUtil::Handle       d_processorHandle;
    bces_AtomicUtil::Int           d_runningFlag;
    btemt_DataMsg                  d_initialMessage;
    bteso_IPv4Address              d_peer;
    int                            d_numConnections;
  private:
    // Callback functions:
    void poolStateCb(int state, int source, int severity);
    void channelStateCb(int channelId, int sourceId, int state, void *context);
    void dataCb(int *numConsumed, int *numNeeded, btemt_DataMsg msg,
                void *context);

    // Not implemented:
    my_QueueClient(const my_QueueClient& original);        // Not impl.
    my_QueueClient& operator=(const my_QueueClient& rhs);  // Not impl.
  public:
    my_QueueClient(bcec_Queue<btemt_DataMsg>    *incomingQueue,
                      bcec_Queue<btemt_DataMsg> *outgoingQueue,
                      bcemt_Mutex               *coutLock,
                      const char                *hostname,
                      int                        portNumber,
                      int                        numConnections,
                      const btemt_DataMsg&       initialMessage,
                      bslma_Allocator           *basicAllocator = 0);

    ~my_QueueClient();
        // Destroy this server.

    int processOutgoingQueue();

    int shutdown(int channelId) {
        return d_channelPool_p->shutdown(channelId,
                                         btemt_ChannelPool::BTEMT_IMMEDIATE);
    }

    int startProcessor();

    int stopProcessor();

    // MANIPULATORS
    const btemt_ChannelPool& pool() { return *d_channelPool_p; }
};

extern "C" {
    static void *queueClientProc(void *arg) {
        my_QueueClient *proc = (my_QueueClient*)arg;
        ASSERT(proc);
        return (void*)proc->processOutgoingQueue();
    }
}

my_QueueClient::my_QueueClient(bcec_Queue<btemt_DataMsg> *incomingQueue,
                               bcec_Queue<btemt_DataMsg> *outgoingQueue,
                               bcemt_Mutex               *coutLock ,
                               const char                *hostname,
                               int                        portNumber,
                               int                        numConnections,
                               const btemt_DataMsg&       initialMessage,
                               bslma_Allocator           *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_coutLock_p(coutLock)
, d_incomingQueue_p(incomingQueue)
, d_outgoingQueue_p(outgoingQueue)
, d_initialMessage(initialMessage)
, d_peer(hostname, portNumber)
, d_numConnections(numConnections)
{
    bces_AtomicUtil::initInt(&d_runningFlag, 0);
    d_config.setMaxThreads(4);
    d_config.setMaxConnections(numConnections);
    d_config.setReadTimeout(5.0);      // in seconds
    d_config.setMetricsInterval(10.0); // seconds
    d_config.setMaxWriteCache(1<<10);  // 1Mb
    d_config.setIncomingMessageSizes(4, 100, 1024);

    if (verbose) {
        P(d_config);
    }

    btemt_ChannelPool::ChannelStateChangeCallback channelCb(
            bdef_MemFnUtil::memFn(&my_QueueClient::channelStateCb, this)
          , basicAllocator);

    btemt_ChannelPool::PoolStateChangeCallback    poolCb(
            bdef_MemFnUtil::memFn(&my_QueueClient::poolStateCb, this)
          , basicAllocator);

    btemt_ChannelPool::DataReadCallback         dataCb(
            bdef_MemFnUtil::memFn(&my_QueueClient::dataCb, this)
          , basicAllocator);

    d_channelPool_p = new (*d_allocator_p) btemt_ChannelPool(channelCb,
                                                             dataCb,
                                                             poolCb,
                                                             d_config,
                                                             basicAllocator);
    ASSERT(0 == d_channelPool_p->start());
    for (int i = 0; i < numConnections; ++i) {
        MTCOUT << "Connecting to " << hostname << ":" << portNumber << MTENDL;
        ASSERT(0 == d_channelPool_p->connect(d_peer,
                                             10,
                                             bdet_TimeInterval(5.0),
                                             CLIENT_BASE_ID + i));
    }
}

my_QueueClient::~my_QueueClient() {
    d_channelPool_p->stop();
    d_allocator_p->deleteObjectRaw(d_channelPool_p);
}

int my_QueueClient::processOutgoingQueue() {
    while (bces_AtomicUtil::getInt(d_runningFlag)) {
        bdet_TimeInterval TIMEOUT(bdetu_SystemTime::now());
        TIMEOUT += 5.0; // 5s
        btemt_DataMsg msg;
        if (0 == d_outgoingQueue_p->timedPopBack(&msg, TIMEOUT)) {
            ASSERT(0 == d_channelPool_p->write(msg.channelId(), msg));
        }
    }
    return 0;
}

int my_QueueClient::startProcessor() {
    bcemt_Attribute attributes;
    attributes.setDetachedState(bcemt_Attribute::BCEMT_CREATE_JOINABLE);
    bces_AtomicUtil::setInt(&d_runningFlag, 1);
    return bcemt_ThreadUtil::create(&d_processorHandle, attributes,
                                    &queueClientProc, (void*)this);
}

int my_QueueClient::stopProcessor() {
    bces_AtomicUtil::setInt(&d_runningFlag, 0);
    return bcemt_ThreadUtil::join(d_processorHandle);
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
                                    void *context)
{
    ASSERT(CLIENT_BASE_ID <= sourceId &&
           sourceId < CLIENT_BASE_ID + d_numConnections);

    switch(state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
          // bteso_IPv4Address peer;
          // d_channelPool_p->getPeerAddress(&peer, channelId);
          if (veryVerbose) {
              MTCOUT << "Client disconnect from " << d_peer
                     << " on channel " << channelId << "." << MTENDL;
          }
          ASSERT(0 == d_channelPool_p->connect(d_peer,
                                               10,
                                               bdet_TimeInterval(5.0),
                                               sourceId));
          if (veryVerbose) {
              MTCOUT << "Re-establishing connection." << MTENDL;
          }
      } break;
      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
          bteso_IPv4Address peer;
          d_channelPool_p->getPeerAddress(&peer, channelId);
          if (veryVerbose) {
              MTCOUT << "Client connected to " << peer
                     << " on channel " << channelId << "." << MTENDL;
          }
          if (0 != d_channelPool_p->write(channelId, d_initialMessage)) {
              d_channelPool_p->shutdown(channelId,
                                        btemt_ChannelPool::BTEMT_IMMEDIATE);
          }
      } break;
    }
}

void my_QueueClient::dataCb(int           *numConsumed,
                            int           *numNeeded,
                            btemt_DataMsg  msg,
                            void          *context) {
    ASSERT(numConsumed);
    ASSERT(msg.data());
    ASSERT(0 < msg.data()->length());

    *numConsumed = USAGE_EXAMPLE_2_NAMESPACE::parseMessages(numNeeded,
                                                            msg.data());
    if (*numConsumed) {
        d_incomingQueue_p->pushBack(msg);
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

    bcema_TestAllocator ta(veryVeryVerbose);
    if (verbose) {
        P(NUM_CONNECTIONS);
        P(HOSTNAME);
        P(PORT_NUMBER);
        P(NUM_MESSAGES);
    }
    bcec_Queue<btemt_DataMsg> incoming, outgoing;

    bcema_PooledBufferChainFactory chainFactory(100, &ta);
    bcema_PooledBufferChain *chain = chainFactory.allocate(0);

    btemt_DataMsg initialMessage(chain, &chainFactory, -1, &ta);

    generateMessage(initialMessage, &ta);

    my_QueueClient qp(&incoming, &outgoing, &coutMutex,
                      HOSTNAME, PORT_NUMBER, NUM_CONNECTIONS,
                      initialMessage);

    ASSERT(0 == qp.startProcessor());

    typedef bsl::vector<bdet_TimeInterval> my_ChannelStats;
    bsl::map<int, my_ChannelStats> channelStats(bsl::less<int>(),
                                                &ta);

    int numConnections = NUM_CONNECTIONS * NUM_ITERS;
    while (numConnections) {
        btemt_DataMsg msg = incoming.popFront();

        // Process message here
        bdet_TimeInterval now = bdetu_SystemTime::now();
        char *msgData = msg.data()->buffer(0);
        msgData += sizeof(int);
        bdex_ByteInStream stream(msgData, msg.data()->length()
                - sizeof(int));
        bdet_TimeInterval initialTime;
        bdex_InStreamFunctions::streamIn(stream, initialTime, 1);
        now -= initialTime;
        int channelId = msg.channelId();
        if (veryVerbose) {
            MTCOUT << "Processed message from " << msg.channelId() << "."
                   << " Delay = " << now << MTENDL;
        }
        if (veryVerbose) {
            T_(); PT(initialTime);
        }
        channelStats[channelId - 1].push_back(now);
        if ((int) channelStats[channelId - 1].size() < NUM_MESSAGES) {
            generateMessage(msg, &ta);
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
                MTCOUT << "Processed message from " << msg.channelId() << "."
                    << " Average delay = " << delay << MTENDL;
            }
            channelStats[channelId - 1].clear();
            qp.shutdown(msg.channelId());
            --numConnections;
        }
    }
    ASSERT(0 == qp.stopProcessor());
    return NULL;
}

} // closing namespace USAGE_EXAMPLE_M1_NAMESPACE

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    // TBD: these tests frequently timeout on Windows, disabling until fixed
#ifdef BSLS_PLATFORM__OS_WINDOWS
    testStatus = -1;
#else

    cout << "TEST " << __FILE__ << " CASE " << test
         << " STARTED " << bdetu_SystemTime::nowAsDatetimeGMT() << endl;

    ASSERT(0 == bteso_SocketImpUtil::startup());
    bdef_Function<void (*)()> NULL_CB(&assertCb);

#ifdef BSLS_PLATFORM__OS_UNIX
    // Ignore SIGPIPE - test driver-wide.  This signal is raised when writing
    // into a socket whose peer is down.  It creates havoc in test case 22 esp.
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
      case 36: {
        // --------------------------------------------------------------------
        // REPRODUCING DRQS 25245489
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDRQS 25245489"
                          << "\n============="
                          << endl;

        using namespace CASE36;

        btemt_ChannelPoolConfiguration config;
        config.setMaxThreads(1);
        config.setMaxConnections(100);
        config.setReadTimeout(5.0);       // in seconds
        config.setMetricsInterval(10.0);  // seconds
        config.setWriteCacheWatermarks(0, CACHE_HI_WAT);
        if (verbose) {
          P(config);
        }

        bcemt_Barrier channelCbBarrier(2);
        btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                                       bdef_BindUtil::bind(&channelStateCb,
                                                           _1, _2, _3, _4,
                                                           &channelId,
                                                           &channelCbBarrier));

        btemt_ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

        btemt_ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

        const int SIZE = 1024;
        bcema_PooledBlobBufferFactory f(SIZE);
        bcema_Blob b(&f);
        b.setLength(CACHE_HI_WAT);

        btemt_ChannelPool mX(channelCb, dataCb, poolCb, config);
        btemt_ChannelPool mY(channelCb, dataCb, poolCb, config);

        channelPool = &mY;

        ASSERT(0 == mX.start());
        ASSERT(0 == mY.start());

        bteso_IPv4Address serverAddr;
        const int SOURCE_ID = 1700;
        const int SERVER_ID = 1701;

        ASSERT(0 == mX.listen(serverAddr, 5, SERVER_ID));

        ASSERT(0 == mX.getServerAddress(&serverAddr, SERVER_ID));

        int rc = mY.connect(serverAddr,
                            5,
                            bdet_TimeInterval(1),
                            SOURCE_ID);
        ASSERT(!rc);

        channelCbBarrier.wait();
        channelCbBarrier.wait();

        const int NT = 5;
        bcemt_Barrier currBarrier(NT + 1);
        barrier = &currBarrier;

        rc = check();
        ASSERT(!rc);

        // signalerThread
        bcemt_ThreadUtil::Handle handle;
        rc = bcemt_ThreadUtil::create(&handle, &signalerThread);
        ASSERT(!rc);

        // fork threads
        for (int i = 0; i < NT; ++i) {
          bcemt_ThreadUtil::Handle handle;
          rc = bcemt_ThreadUtil::create(&handle,
                                        bdef_BindUtil::bind(&writerThread, i));
          ASSERT(!rc);
        }

        barrier->wait();

        // quick check
        bcemt_ThreadUtil::sleep(bdet_TimeInterval(1)); // let flush complete
                                                       // if any
        rc = check();
        ASSERT(!rc);

        cout << "First check" << endl;

        // more thorough check
        bcemt_ThreadUtil::sleep(bdet_TimeInterval(10));  // in case flush did
                                                         // not complete

        rc = check();
        ASSERT(!rc);

        cout << "Second check" << endl;

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(10));   // in case flush did
                                                          // not complete

        rc = check();
        ASSERT(!rc);

        cout << "Third check" << endl;

        // Following veriy that the bug is due to d_writeCacheSize mess-up
        bcemt_ThreadUtil::sleep(bdet_TimeInterval(1));   // in case flush did
                                                         // not complete
        if (ERROR_MAX_POSSIBLE_WRITE_FAILED) {
          rc = channelPool->setWriteCacheHiWatermark(channelId,
                                                     CACHE_HI_WAT * 2);
          ASSERT(!rc);

          rc = write(CACHE_HI_WAT);
          ASSERT(!rc);
        }
        else {
          rc = channelPool->setWriteCacheHiWatermark(channelId, 1);
          ASSERT(!rc);

          rc = write(CACHE_HI_WAT + 1);
          ASSERT(rc);
        }
      } break;
      case 35: {
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

        using namespace CASE35;

        const struct {
            int         d_line;
            const char *d_spec_p;
            int         d_exp;
        } DATA[] = {
            // Line   Spec  Exp
            // ----   ----  ---
            {   L_,   "GN",         0 },

#ifdef BSLS_PLATFORM__OS_LINUX
            {   L_,   "GY",        -1 },
#else
            {   L_,   "GY",         0 },
#endif

            {   L_,   "HN",         0 },

#ifndef BSLS_PLATFORM__OS_AIX
            {   L_,   "HY",         0 },
#endif

            {   L_,   "IN",         0 },
            {   L_,   "IY",         0 },

#ifndef BSLS_PLATFORM__OS_AIX
// TBD on AIX setting this option succeeds for BTESO_SOCKET_DATAGRAM
//                   {   L_,   "JN",        -1 },
//                   {   L_,   "JY",        -1 },
// #else
            {   L_,   "JN",         0 },
            {   L_,   "JY",         0 },
#endif

            {   L_,   "KN",         0 },
            {   L_,   "KY",         0 },

#ifndef BSLS_PLATFORM__OS_HPUX
// TBD on HPUX setting this option succeeds for BTESO_SOCKET_DATAGRAM
//                   {   L_,   "LN",        -1 },
//                   {   L_,   "LY",        -1 },
// #else
            {   L_,   "LN",         0 },
            {   L_,   "LY",         0 },
#endif

            {   L_,   "GNHN",       0 },
            {   L_,   "GNHYIN",     0 },
            {   L_,   "GNHYIYKY",   0 },

            {   L_,   "A0",         0 },
            {   L_,   "A1",         0 },
            {   L_,   "A2",         0 },

#ifndef BSLS_PLATFORM__OS_AIX
            {   L_,   "B0",         0 },
            {   L_,   "B1",         0 },
            {   L_,   "B2",         0 },
#endif

// #if !defined(BSLS_PLATFORM__OS_SOLARIS)          \
//  && !defined(BSLS_PLATFORM__OS_LINUX)            \
//  && !defined(BSLS_PLATFORM__OS_HPUX)
//               // Cannot be changed on Linux and not specified on Sun

//             {   L_,   "C0",         0 },
//             {   L_,   "C1",         0 },
//             {   L_,   "C2",         0 },
// #elif !defined(BSLS_PLATFORM__OS_HPUX)
//             {   L_,   "C0",        -1 },
//             {   L_,   "C1",        -1 },
//             {   L_,   "C2",        -1 },
// #endif

#ifdef BSLS_PLATFORM__OS_SOLARIS
            {   L_,   "D0",        -1 },
            {   L_,   "D1",        -1 },
            {   L_,   "D2",        -1 },
#elif !defined(BSLS_PLATFORM__OS_HPUX) && !defined(BSLS_PLATFORM__OS_AIX)
            {   L_,   "D0",         0 },
            {   L_,   "D1",         0 },
            {   L_,   "D2",         0 },
#endif

            // Fails on all platforms TBD Uncomment

#ifndef BSLS_PLATFORM__OS_HPUX
// TBD on HPUX setting this option succeeds but the timeout value is not what
// was specified.
//               {   L_,   "E0",         0 },
//               {   L_,   "E1",         0 },
//               {   L_,   "E2",         0 },

//               {   L_,   "F0",         0 },
//               {   L_,   "F1",         0 },
//               {   L_,   "F2",         0 },
// #else
            {   L_,   "E0",        -1 },
            {   L_,   "E1",        -1 },
            {   L_,   "E2",        -1 },

            {   L_,   "F0",        -1 },
            {   L_,   "F1",        -1 },
            {   L_,   "F2",        -1 },
#endif

#if defined(BSLS_PLATFORM__OS_AIX)
            // Works only on IBM.  On other platforms although the return
            // code is 0, the timeout is not set correctly.

            {   L_,   "MN1",       0 },
#endif

            {   L_,   "MY2",       0 },

#ifndef BSLS_PLATFORM__OS_AIX
            {   L_,   "A1B2MY2",   0 },
#endif
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // connect(IPv4Address .... ) with socket options provided
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMaxConnections(100);
            config.setReadTimeout(5.0);      // in seconds
            config.setMetricsInterval(10.0); // seconds
            config.setMaxWriteCache(1<<10);  // 1Mb
            config.setIncomingMessageSizes(4, 100, 1024);
            if (verbose) {
                P(config);
            }

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                &channelStateCb);

            btemt_ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

            btemt_ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            vector<bteso_StreamSocket<bteso_IPv4Address> *> sockets;
            int numExpChannels = 0;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec_p;
                const int   EXP  = DATA[i].d_exp;

                SocketOptions opt = g(SPEC); const SocketOptions& OPT = opt;

                if (veryVerbose) { P_(LINE) P(OPT) }

                const int SERVER_ID = 100;
                const int SOURCE_ID = 200;

                bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                            factory.allocate();
                sockets.push_back(socket);

                ASSERT(0 == socket->bind(bteso_IPv4Address()));
                ASSERT(0 == socket->listen(5));

                bteso_IPv4Address serverAddr;
                ASSERT(0 == socket->localAddress(&serverAddr));

                int rc = pool.connect(serverAddr,
                                      1,
                                      bdet_TimeInterval(1),
                                      SOURCE_ID + i,
                                      true,
                                      Obj::BTEMT_CLOSE_BOTH,
                                      &OPT);

                ASSERT(!rc);

                case35Mutex.lock();
                case35Condition.wait(&case35Mutex);
                case35Mutex.unlock();

                if (EXP) {
                    LOOP_ASSERT(LINE, poolCbCalled);
                    poolCbCalled = false;
                }
                else {
                    ++numExpChannels;
                    LOOP_ASSERT(LINE, !verify(pool, s_channelId, OPT));
                }
                LOOP2_ASSERT(LINE, numExpChannels,
                             numExpChannels == pool.numChannels());
            }

            for (int i = 0; i < sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }

        // connect(hostname ... ) with socket options provided
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMaxConnections(100);
            config.setReadTimeout(5.0);      // in seconds
            config.setMetricsInterval(10.0); // seconds
            config.setMaxWriteCache(1<<10);  // 1Mb
            config.setIncomingMessageSizes(4, 100, 1024);
            if (verbose) {
                P(config);
            }

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                &channelStateCb);

            btemt_ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

            btemt_ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            vector<bteso_StreamSocket<bteso_IPv4Address> *> sockets;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec_p;
                const int   EXP  = DATA[i].d_exp;

                SocketOptions opt = g(SPEC); const SocketOptions& OPT = opt;

                if (veryVerbose) { P_(LINE) P(OPT) }

                const int SERVER_ID = 100;
                const int SOURCE_ID = 200;

                bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                            factory.allocate();
                sockets.push_back(socket);

                ASSERT(0 == socket->bind(bteso_IPv4Address()));
                ASSERT(0 == socket->listen(5));

                bteso_IPv4Address serverAddr;
                ASSERT(0 == socket->localAddress(&serverAddr));

                const char *host = "127.0.0.1";
                int rc = pool.connect(host,
                                      serverAddr.portNumber(),
                                      1,
                                      bdet_TimeInterval(1),
                                      SOURCE_ID + i,
                                      Obj::BTEMT_RESOLVE_ONCE,
                                      true,
                                      Obj::BTEMT_CLOSE_BOTH,
                                      &OPT);

                ASSERT(!rc);

                case35Mutex.lock();
                case35Condition.wait(&case35Mutex);
                case35Mutex.unlock();

                if (EXP) {
                    LOOP_ASSERT(LINE, poolCbCalled);
                    poolCbCalled = false;
                }
                else {
                    LOOP_ASSERT(LINE, !verify(pool, s_channelId, OPT));
                }
            }

            for (int i = 0; i < sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }

        // connect(IPv4Address .... ) with client address specified
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMaxConnections(100);
            config.setReadTimeout(5.0);      // in seconds
            config.setMetricsInterval(10.0); // seconds
            config.setMaxWriteCache(1<<10);  // 1Mb
            config.setIncomingMessageSizes(4, 100, 1024);
            if (verbose) {
                P(config);
            }

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                &channelStateCb);

            btemt_ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

            btemt_ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            const int SERVER_ID = 100;
            const int SOURCE_ID = 200;

            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

            ASSERT(0 == socket->bind(bteso_IPv4Address()));
            ASSERT(0 == socket->listen(5));

            bteso_IPv4Address serverAddr;
            ASSERT(0 == socket->localAddress(&serverAddr));

            bteso_IPv4Address exp_ca, ca;
            exp_ca.setIpAddress("127.0.0.1");
            exp_ca.setPortNumber(45000);
            int rc = pool.connect(serverAddr,
                                  1,
                                  bdet_TimeInterval(1),
                                  SOURCE_ID,
                                  true,
                                  Obj::BTEMT_CLOSE_BOTH,
                                  0,
                                  &exp_ca);

            ASSERT(!rc);

            case35Mutex.lock();
            case35Condition.wait(&case35Mutex);
            case35Mutex.unlock();

            pool.getLocalAddress(&ca, s_channelId);

            LOOP2_ASSERT(exp_ca, ca, exp_ca == ca);
            factory.deallocate(socket);
        }

        // connect(hostname ... ) with client address specified
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMaxConnections(100);
            config.setReadTimeout(5.0);      // in seconds
            config.setMetricsInterval(10.0); // seconds
            config.setMaxWriteCache(1<<10);  // 1Mb
            config.setIncomingMessageSizes(4, 100, 1024);
            if (verbose) {
                P(config);
            }

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                &channelStateCb);

            btemt_ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

            btemt_ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            const int SERVER_ID = 100;
            const int SOURCE_ID = 200;

            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();
            ASSERT(0 == socket->bind(bteso_IPv4Address()));
            ASSERT(0 == socket->listen(5));

            bteso_IPv4Address serverAddr;
            ASSERT(0 == socket->localAddress(&serverAddr));

            bteso_IPv4Address exp_ca, ca;
            exp_ca.setIpAddress("127.0.0.1");
            exp_ca.setPortNumber(45000);

            const char *host = "127.0.0.1";
            int rc = pool.connect(host,
                                  serverAddr.portNumber(),
                                  1,
                                  bdet_TimeInterval(1),
                                  SOURCE_ID,
                                  Obj::BTEMT_RESOLVE_ONCE,
                                  true,
                                  Obj::BTEMT_CLOSE_BOTH,
                                  0,
                                  &exp_ca);

            ASSERT(!rc);

            case35Mutex.lock();
            case35Condition.wait(&case35Mutex);
            case35Mutex.unlock();


            pool.getLocalAddress(&ca, s_channelId);

            LOOP2_ASSERT(exp_ca, ca, exp_ca == ca);
            factory.deallocate(socket);
        }

        // listen(port, backlog, id, reuseAddr, readEnabled, sockOpts)
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMaxConnections(100);
            config.setReadTimeout(5.0);      // in seconds
            config.setMetricsInterval(10.0); // seconds
            config.setMaxWriteCache(1<<10);  // 1Mb
            config.setIncomingMessageSizes(4, 100, 1024);
            if (verbose) {
                P(config);
            }

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                &channelStateCb);

            btemt_ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

            btemt_ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            vector<bteso_StreamSocket<bteso_IPv4Address> *> sockets;
            int numExpChannels = 0;

            enum {
                AMBIGUOUS_REUSE_ADDRESS  = -11,
                SET_SOCKET_OPTION_FAILED = -10
            };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec_p;
                const int   EXP  = DATA[i].d_exp;

                SocketOptions opt = g(SPEC); const SocketOptions& OPT = opt;

                if (veryVerbose) { P_(LINE) P(OPT) }

                const int SERVER_PORT = 1700;
                const int SERVER_ID   = 100;

                int rc = pool.listen(SERVER_PORT + i,
                                     5,
                                     SERVER_ID + i,
                                     bdet_TimeInterval(1),
                                     true,
                                     true,
                                     &OPT);

                if (!OPT.reuseAddress().isNull()
                 && false == OPT.reuseAddress().value()) {
                    LOOP_ASSERT(LINE, AMBIGUOUS_REUSE_ADDRESS == rc);
                    continue;
                }

                if (EXP) {
                    LOOP_ASSERT(LINE, SET_SOCKET_OPTION_FAILED == rc);
                    continue;
                }

                LOOP_ASSERT(LINE, !rc);
                bteso_IPv4Address serverAddr;
                ASSERT(0 == pool.getServerAddress(&serverAddr, SERVER_ID + i));

                bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                            factory.allocate();
                sockets.push_back(socket);

                ASSERT(0 == socket->connect(serverAddr));

                case35Mutex.lock();
                case35Condition.wait(&case35Mutex);
                case35Mutex.unlock();

                ++numExpChannels;
                LOOP_ASSERT(LINE, !verify(pool, s_channelId, OPT));
                LOOP2_ASSERT(LINE, numExpChannels,
                             numExpChannels == pool.numChannels());
            }

            for (int i = 0; i < sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }

        // listen(port, backlog, id, timeout, reuseAddr, readEnabled, sockOpts)
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMaxConnections(100);
            config.setReadTimeout(5.0);      // in seconds
            config.setMetricsInterval(10.0); // seconds
            config.setMaxWriteCache(1<<10);  // 1Mb
            config.setIncomingMessageSizes(4, 100, 1024);
            if (verbose) {
                P(config);
            }

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                &channelStateCb);

            btemt_ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

            btemt_ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            vector<bteso_StreamSocket<bteso_IPv4Address> *> sockets;
            int numExpChannels = 0;

            enum {
                AMBIGUOUS_REUSE_ADDRESS  = -11,
                SET_SOCKET_OPTION_FAILED = -10
            };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec_p;
                const int   EXP  = DATA[i].d_exp;

                SocketOptions opt = g(SPEC); const SocketOptions& OPT = opt;

                if (veryVerbose) { P_(LINE) P(OPT) }

                const int SERVER_PORT = 1700;
                const int SERVER_ID   = 100;

                int rc = pool.listen(SERVER_PORT + i,
                                     5,
                                     SERVER_ID + i,
                                     bdet_TimeInterval(1),
                                     true,
                                     true,
                                     &OPT);

                if (!OPT.reuseAddress().isNull()
                 && false == OPT.reuseAddress().value()) {
                    LOOP_ASSERT(LINE, AMBIGUOUS_REUSE_ADDRESS == rc);
                    continue;
                }

                if (EXP) {
                    LOOP_ASSERT(LINE, SET_SOCKET_OPTION_FAILED == rc);
                    continue;
                }

                LOOP_ASSERT(LINE, !rc);

                bteso_IPv4Address serverAddr;
                ASSERT(0 == pool.getServerAddress(&serverAddr, SERVER_ID + i));

                bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                            factory.allocate();
                sockets.push_back(socket);

                ASSERT(0 == socket->connect(serverAddr));

                case35Mutex.lock();
                case35Condition.wait(&case35Mutex);
                case35Mutex.unlock();

                ++numExpChannels;
                LOOP_ASSERT(LINE, !verify(pool, s_channelId, OPT));
                LOOP2_ASSERT(LINE, numExpChannels,
                             numExpChannels == pool.numChannels());
            }

            for (int i = 0; i < sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }

        // listen(addr, backlog, id, reuseAddr, readEnabled, sockOpts)
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMaxConnections(100);
            config.setReadTimeout(5.0);      // in seconds
            config.setMetricsInterval(10.0); // seconds
            config.setMaxWriteCache(1<<10);  // 1Mb
            config.setIncomingMessageSizes(4, 100, 1024);
            if (verbose) {
                P(config);
            }

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                &channelStateCb);

            btemt_ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

            btemt_ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            vector<bteso_StreamSocket<bteso_IPv4Address> *> sockets;
            int numExpChannels = 0;

            enum {
                AMBIGUOUS_REUSE_ADDRESS  = -11,
                SET_SOCKET_OPTION_FAILED = -10
            };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec_p;
                const int   EXP  = DATA[i].d_exp;

                SocketOptions opt = g(SPEC); const SocketOptions& OPT = opt;

                if (veryVerbose) { P_(LINE) P(OPT) }

                const int SERVER_ID = 100;

                bteso_IPv4Address serverAddr;
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

                if (EXP) {
                    LOOP_ASSERT(LINE, SET_SOCKET_OPTION_FAILED == rc);
                    continue;
                }

                LOOP_ASSERT(LINE, !rc);
                ASSERT(0 == pool.getServerAddress(&serverAddr, SERVER_ID + i));

                bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                            factory.allocate();
                sockets.push_back(socket);

                ASSERT(0 == socket->connect(serverAddr));

                case35Mutex.lock();
                case35Condition.wait(&case35Mutex);
                case35Mutex.unlock();

                ++numExpChannels;
                LOOP_ASSERT(LINE, !verify(pool, s_channelId, OPT));
                LOOP2_ASSERT(LINE, numExpChannels,
                             numExpChannels == pool.numChannels());
            }

            for (int i = 0; i < sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }

        // listen(addr, backlog, id, time, reuse, read, mode, sockOpts)
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMaxConnections(100);
            config.setReadTimeout(5.0);      // in seconds
            config.setMetricsInterval(10.0); // seconds
            config.setMaxWriteCache(1<<10);  // 1Mb
            config.setIncomingMessageSizes(4, 100, 1024);
            if (verbose) {
                P(config);
            }

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                &channelStateCb);

            btemt_ChannelPool::PoolStateChangeCallback poolCb(&poolStateCb);

            btemt_ChannelPool::BlobBasedReadCallback dataCb(&blobBasedReadCb);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, config);

            ASSERT(0 == pool.start());

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            vector<bteso_StreamSocket<bteso_IPv4Address> *> sockets;
            int numExpChannels = 0;

            enum {
                AMBIGUOUS_REUSE_ADDRESS  = -11,
                SET_SOCKET_OPTION_FAILED = -10
            };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec_p;
                const int   EXP  = DATA[i].d_exp;

                SocketOptions opt = g(SPEC); const SocketOptions& OPT = opt;

                if (veryVerbose) { P_(LINE) P(OPT) }

                const int SERVER_ID = 100;

                bteso_IPv4Address serverAddr;
                int rc = pool.listen(serverAddr,
                                     5,
                                     SERVER_ID + i,
                                     bdet_TimeInterval(1),
                                     true,
                                     true,
                                     Obj::BTEMT_CLOSE_BOTH,
                                     &OPT);

                if (!OPT.reuseAddress().isNull()
                 && false == OPT.reuseAddress().value()) {
                    LOOP_ASSERT(LINE, AMBIGUOUS_REUSE_ADDRESS == rc);
                    continue;
                }

                if (EXP) {
                    LOOP_ASSERT(LINE, SET_SOCKET_OPTION_FAILED == rc);
                    continue;
                }

                LOOP_ASSERT(LINE, !rc);
                ASSERT(0 == pool.getServerAddress(&serverAddr, SERVER_ID + i));

                bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                            factory.allocate();
                sockets.push_back(socket);

                ASSERT(0 == socket->connect(serverAddr));

                case35Mutex.lock();
                case35Condition.wait(&case35Mutex);
                case35Mutex.unlock();

                ++numExpChannels;
                LOOP_ASSERT(LINE, !verify(pool, s_channelId, OPT));
                LOOP2_ASSERT(LINE, numExpChannels,
                             numExpChannels == pool.numChannels());
            }

            for (int i = 0; i < sockets.size(); ++i) {
                factory.deallocate(sockets[i]);
            }
        }
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 3
        //   Send and receive various messages conforming to the vlm_EchoServer
        //   message schema.  This is a common message schema for passing
        //   variable-length messages.
        //
        // Plan:
        //
        // Testing:
        //   USAGE EXAMPLE 3
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING USAGE EXAMPLE - VLM ECHO SERVER."
                 << "\n========================================" << endl;

        using namespace USAGE_EXAMPLE_3_NAMESPACE;

        const struct {
            int           d_line;
            const char   *d_text;
        } DATA[] = {
            // Line  Text
            {  L_,   "" },
            {  L_,   "BDE" },
            {  L_,   "hello world"},
            {  L_,   "This is a short test."},
            {  L_,   "This is a somewhat longer test which we hope "
                     "will not pose much of a problem for the "
                     "vlm_EchoServer." },
        };
        const int DATA_SIZE = sizeof DATA / sizeof *DATA;

        typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> IPv4Factory;
        typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            vlm_EchoServer server(0, &ta);
            ASSERT(0 == server.start());
            const int PORT = server.portNumber();

            IPv4Factory          factory(&ta);
            Socket              *socket = factory.allocate();
            btesos_TcpChannel    channel(socket);
            bteso_IPv4Address    address;

            address.setPortNumber(PORT);
            ASSERT(0 == socket->connect(address));
            ASSERT(0 == channel.isInvalid());

            for(int i=0; i < DATA_SIZE; ++i){
                // Send Request
                const int             LINE = DATA[i].d_line;
                const char           *TEXT = DATA[i].d_text;
                int                   length;
                char                  rawLength[sizeof length];

                length = strlen(TEXT) + 1;
                bdex_ByteStreamImpUtil::putInt32(rawLength, length);
                channel.write(rawLength, sizeof length);
                channel.write(TEXT, length);

                // Read Response
                channel.read(rawLength, sizeof length);
                bdex_ByteStreamImpUtil::getInt32(&length, rawLength);
                LOOP_ASSERT(i, 0 < length);
                char   *text = new char[length];
                channel.read(text, length);

                if(veryVerbose){
                    P_(i); P(TEXT);
                    P_(i); P(text);
                }
                LOOP2_ASSERT(i, LINE, 0 == strcmp(TEXT, text));
                delete[] text;
            }

            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == server.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 33: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST: my_QueueProcessor
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.  Additionally, replace all calls to 'assert' in the usage
        //   example with calls to 'ASSERT'.  In order for this usage example
        //   to actually do something useful, run the test case -1 in another
        //   thread.
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------
        if (verbose)
            cout << "\nTESTING USAGE EXAMPLE - QUEUE PROCESSOR"
                 << "\n=======================================" << endl;

        using namespace USAGE_EXAMPLE_2_NAMESPACE;
        using namespace USAGE_EXAMPLE_M1_NAMESPACE;

        if (verbose) {
            cout << "In another window, run this test driver's case -1, e.g.:";
            cout << "\n\t" << argv[0] << " -1  10  2564  127.0.0.1  10000\n";
            cout << "For bigger jobs (i.e, stress test), try:";
            cout << "\n\t" << argv[0] << " -1  10  2564  127.0.0.1  10000\n";
            cout << "For non-null testing in verbose mode, try:";
            cout << "\n\t" << argv[0] << " 24 -1\n";
        }

        enum {
            NUM_CONNECTIONS = 10,
            MAX_CONNECTIONS = 100,
            NUM_MESSAGES    = 10,  // per connection
            NUM_ITERS       = 10
        };

        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_Pool pool(100, &ta);

        bcec_Queue<btemt_DataMsg> incoming, outgoing;
        my_QueueProcessor qp(&incoming, &outgoing, &coutMutex,
                             0, MAX_CONNECTIONS, &ta);
        ASSERT(0 == qp.startProcessor());
        const int PORT_NUMBER = qp.portNumber();

        bcemt_ThreadUtil::Handle usageMinusOneHandle;
        if (verbose < 0) {
            caseMinusOneInfo info;
            info.d_numConnections = NUM_CONNECTIONS;
            info.d_portNumber     = PORT_NUMBER;
            info.d_hostname       = const_cast<char *>("127.0.0.1");
            info.d_numMessages    = NUM_MESSAGES;
            info.d_numIters       = NUM_ITERS;
            bcemt_ThreadUtil::create(&usageMinusOneHandle,
                                     &usageExampleMinusOne,
                                     (void *)&info);
        }

        bcemt_ThreadUtil::microSleep(0, 1); // 1s, to let my_QueueClients
                                            // enqueue messages into incoming
                                            // queue.

        while (0 < incoming.queue().length()) {
            btemt_DataMsg msg = incoming.popFront(); // get msg from client
            if (veryVerbose) {
                PT(msg.data()->length());
            }
            ASSERT(msg.data()->length());
            if (veryVeryVerbose) {
                MTCOUT << "Processing message from "
                       << msg.channelId() << MTENDL;
            }
            bcemt_ThreadUtil::microSleep(10000);
            outgoing.pushBack(msg); // will send back to corresponding client
        }

        if (verbose < 0) {
            bcemt_ThreadUtil::join(usageMinusOneHandle);
        }
        ASSERT(0 == qp.stopProcessor());

      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
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
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING USAGE EXAMPLE - AN ECHO SERVER"
                 << "\n======================================" << endl;

        using namespace USAGE_EXAMPLE_2_NAMESPACE;

        enum {
            MAX_CONNECTIONS = 1000,
            NUM_MONITOR     = 10
        };

        my_EchoServer echoServer(&coutMutex, 0, MAX_CONNECTIONS);

        if (verbose) {
            MTCOUT << "monitor pool: count=" << NUM_MONITOR << MTENDL;
        }
        monitorPool(&coutMutex, echoServer.pool(), NUM_MONITOR, verbose);

      } break;

      case 31: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: DRQS 22256519
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
        //   DRQS 22256519
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING FOR DRQS 22256519"
                 << "\n=========================" << endl;

        using namespace TEST_CASE_31_NAMESPACE;

        typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> IPv4Factory;
        typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;

        const char *TEXT = "AAAAAAAAAABBBBBCCCCCDDDDDDDDDD";// 30 characters
        const int   LEN  = bsl::strlen(TEXT);

        btemt_ChannelPoolConfiguration config;
        config.setMaxConnections(1);
        config.setMaxThreads(1);
        config.setIncomingMessageSizes(1, 3, 5);

        bcemt_Mutex         coutMutex;
        bcema_TestAllocator ta(veryVeryVerbose);
        ReadServer          server(&coutMutex, LEN, config, &ta);

        ASSERT(0 == server.start());

        const int         PORT = server.portNumber();
        bteso_IPv4Address address;
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
            bcemt_ThreadUtil::sleep(bdet_TimeInterval(1));
        }

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(5));

        const bsl::string& DATA = server.data();

        LOOP_ASSERT(DATA, DATA == bsl::string(TEXT));
      } break;
      case 30: {
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

        using namespace TEST_CASE_30_NAMESPACE;

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

            // A long passage from the component level doc
            { L_,
"//@DESCRIPTION:  This component provides a thread-enabled manager of the"
"// IPv4-based byte-stream communication channels.  The channels are created"
"// automatically when the appropriate events occur an destroyed based on user"
"// requests.  A new channel is allocated automatically when an incoming"
"// connection is accepted, or the user explicitly requests a connection to"
"// a server.  Pool provides both client (aka connector) and server (aka"
"// acceptor) functionality.  The channel pool manages efficient delivery of"
"// messages to/from the user based on configuration information supplied at"
"// construction.  The states of individual messages are *not* reported;"
"// rather, channel pool notifies the user when a channel's state changes.  It"
"// also notifies the user when the pool's state is affected and provides the"
"// classification of errors.  The notification is done via asynchronous"
"// callback that may be invoked from *any* (managed) thread."
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> IPv4Factory;
        typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;

        {
            for (int type = 0; type < 2; ++type) {
                bcemt_Mutex coutMutex;
                bcema_TestAllocator ta(veryVeryVerbose);
                ReadServer server(&coutMutex, 0, (bool) type, &ta);
                ASSERT(0 == server.start());
                const int PORT = server.portNumber();
                bteso_IPv4Address address;
                address.setPortNumber(PORT);

                IPv4Factory              factory;
                bcemt_ThreadUtil::Handle handles[NUM_DATA];
                bsl::vector<TestData>    tests(NUM_DATA);
                bcemt_Barrier            barrier(NUM_DATA);

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int          LINE = DATA[i].d_line;
                    const bsl::string& TEXT = DATA[i].d_text;

                    TestData& testData      = tests[i];
                    testData.d_threadIntId  = i;
                    testData.d_expData      = TEXT;
                    testData.d_address      = address;
                    testData.d_socket_p     = factory.allocate();
                    testData.d_barrier_p    = &barrier;
                    testData.d_useBlobs     = type;

                    bcemt_ThreadUtil::create(&handles[i],
                                             threadFunction,
                                             &testData);
                }

                for (int i = 0; i < NUM_DATA; ++i) {
                    ASSERT(0 == bcemt_ThreadUtil::join(handles[i]));
                }

                while (NUM_DATA != server.numCompletedMsgs()) {
                    bcemt_ThreadUtil::yield();
                }

                for (int i = 0; i < NUM_DATA; ++i) {
                    DataReader        *READER = server.dataReader(i);
                    const int          MSG_ID = READER->msgId();
                    const bsl::string  MSG    = READER->data();

                    const int          LINE      = DATA[MSG_ID].d_line;
                    const bsl::string  EXP_MSG   = DATA[MSG_ID].d_text;

                    LOOP3_ASSERT(LINE, MSG, EXP_MSG, MSG == EXP_MSG);
                }

                ASSERT(0 == server.stop());
            }
        }
      } break;
      case 29: {
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

        bcema_TestAllocator ta;

        int       NUM_THREADS[] = { 2,  4,  8 };
        const int NUM_NUM_THREADS = sizeof NUM_THREADS / sizeof *NUM_THREADS;
        bool      COLLECT_METRICS[] = { false, true };
        const int NUM_COLLECT_METRICS = sizeof COLLECT_METRICS /
                                        sizeof * COLLECT_METRICS;

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

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(MAX_THREADS);
            config.setCollectTimeMetrics(false);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setWriteCacheWatermarks(LOW_WATERMARK, HI_WATERMARK);

            if (verbose)         { P(MAX_THREADS); }
            if (veryVeryVerbose) { P(config); }

            ChannelPoolStateCbTester mX(config, &ta);
            Obj& pool = mX.pool(); const Obj& POOL = mX.pool();
            bcema_PooledBufferChainFactory outFactory(HI_WATERMARK / 64,
                                                      &ta);
            ASSERT(0 == pool.start());

            // Keep track of the client sockets (so we can deallocate them),
            // and the set of thread ids, and the number of connections
            // assigned to each thread id.
            bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *> cltSockets;
            bsl::map<ThreadId, int>                              threadIdCount;
            for (int j = 0; j < 2 * MAX_THREADS; ++j) {
                if (veryVeryVerbose) {
                    P(j);
                }

                int ret  = 0;
                bteso_SocketHandle::Handle handles[2];

                ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                            handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);

                ASSERT(0 == ret);

                // The following socket options are set only if necessary.
                ret = bteso_SocketOptUtil::setOption(handles[1],
                                                     SockOpt::BTESO_TCPLEVEL,
                                                     SockOpt::BTESO_TCPNODELAY,
                                                     1);
                ASSERT(0 == ret);
                bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
                ASSERT(serverSocket);
                bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
                ASSERT(clientSocket);
                cltSockets.push_back(clientSocket);
                ASSERT(0 == pool.import(serverSocket, &factory, 0));

                // Wait for the channel to come up.
                bsl::vector<ChannelPoolStateCbTester::ChannelState>
                                                                   states(&ta);
                ASSERT(0 == mX.waitForState(
                                           &states,
                                           btemt_ChannelPool::BTEMT_CHANNEL_UP,
                                           bdet_TimeInterval(1.0)));

                // For any CHANNEL_UP message, record the thread to which the
                // new channel was assinged.
                for (int k = 0; k < (int) states.size(); ++k) {
                    if (veryVerbose) {
                        P_(k); P_(states[k].d_channelId);
                        P_(states[k].d_state); P(states[k].d_threadId);
                    }
                    if (states[k].d_state ==
                                         btemt_ChannelPool::BTEMT_CHANNEL_UP) {
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
                bcemt_ThreadUtil::sleep(bdet_TimeInterval(.05));
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

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);
            btemt_ChannelPoolConfiguration config;
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
            btemt_ChannelPool::DataReadCallback         dataCb(
                    bdef_BindUtil::bindA( &ta
                                        , &dummyDataCallbackWithDelay
                                        , _1, _2, _3, _4
                                        , 0.5));

            btemt_ChannelPool::PoolStateChangeCallback    poolCb;
            makeNull(&poolCb);

            ChannelPoolStateCbTester mX(config, dataCb, poolCb, &ta);

            Obj& pool = mX.pool(); const Obj& POOL = mX.pool();
            bcema_PooledBufferChainFactory outFactory(HI_WATERMARK / 64,
                                                      &ta);
            ASSERT(0 == pool.start());

            ThreadId lastClientSocketThreadId;
            bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *> cltSockets;
            bsl::map<ThreadId, int>                              threadIdCount;
            for (int j = 0; j < MAX_THREADS; ++j) {
                int ret  = 0;
                bteso_SocketHandle::Handle handles[2];
                ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                            handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);

                ASSERT(0 == ret);

                // The following socket options are set only if necessary.
                ret = bteso_SocketOptUtil::setOption(handles[1],
                                                     SockOpt::BTESO_TCPLEVEL,
                                                     SockOpt::BTESO_TCPNODELAY,
                                                     1);
                ASSERT(0 == ret);
                bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
                ASSERT(serverSocket);
                bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
                ASSERT(clientSocket);
                cltSockets.push_back(clientSocket);
                ASSERT(0 == pool.import(serverSocket, &factory, 0));

                // Wait for the channel to come up.
                bsl::vector<ChannelPoolStateCbTester::ChannelState>
                                                                   states(&ta);
                ASSERT(0 == mX.waitForState(
                                           &states,
                                           btemt_ChannelPool::BTEMT_CHANNEL_UP,
                                           bdet_TimeInterval(1.0)));

                // For any CHANNEL_UP message, record the thread to which the
                // new channel was assinged.
                for (int k = 0; k < (int) states.size(); ++k) {
                    if (veryVerbose) {
                        P_(k); P_(states[k].d_channelId);
                        P_(states[k].d_state); P(states[k].d_threadId);
                    }
                    if (states[k].d_state ==
                                         btemt_ChannelPool::BTEMT_CHANNEL_UP) {
                        threadIdCount[states[k].d_threadId]++;
                        if (j == MAX_THREADS - 1) {
                            lastClientSocketThreadId = states[k].d_threadId;
                        }
                    }
                }
                LOOP_ASSERT(threadIdCount.size(),
                            j + 1 == (int) threadIdCount.size());
                bcemt_ThreadUtil::sleep(bdet_TimeInterval(.05));
            }

            // Write data to all but the last channel, to simulate processing
            // on all but the last channel.
            char ONE_BYTE_MSG[] = { (char) 0xff };
            for (int j = 0; j < (int) cltSockets.size() - 1; ++j) {
                ASSERT(1 == cltSockets[j]->write(ONE_BYTE_MSG, 1));
            }
            bcemt_ThreadUtil::sleep(bdet_TimeInterval(.25));

            if (veryVerbose) {
                P(pool.busyMetrics());
            }

            // Create a new socket to import.
            int ret  = 0;
            bteso_SocketHandle::Handle handles[2];
            ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                            handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);

            ASSERT(0 == ret);

            // The following socket options are set only if necessary.
            ret = bteso_SocketOptUtil::setOption(handles[1],
                                                 SockOpt::BTESO_TCPLEVEL,
                                                 SockOpt::BTESO_TCPNODELAY, 1);
            ASSERT(0 == ret);
            bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
            ASSERT(serverSocket);
            bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
            ASSERT(clientSocket);
            ASSERT(0 == pool.import(serverSocket, &factory, 0));

            // Wait for the channel to come up.
            bsl::vector<ChannelPoolStateCbTester::ChannelState> states(&ta);
            ASSERT(0 == mX.waitForState(&states,
                                        btemt_ChannelPool::BTEMT_CHANNEL_UP,
                                        bdet_TimeInterval(1.0)));

            // Verify that the newly imported socket was assigned to the
            // thread (i.e., the event manager) of the channel that was not
            // simulating processing (i.e., the last channel).
            for (int k = 0; k < (int) states.size(); ++k) {
                if (states[k].d_state == btemt_ChannelPool::BTEMT_CHANNEL_UP) {
                    LOOP2_ASSERT(lastClientSocketThreadId,
                                 states[k].d_threadId,
                                 lastClientSocketThreadId ==
                                                         states[k].d_threadId);
                }
            }

            pool.stop();
            factory.deallocate(clientSocket);
            for (int j = 0; j < (int) cltSockets.size(); ++j) {
                factory.deallocate(cltSockets[j]);
            }

        }
      } break;
      case 28: {
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

        bcema_TestAllocator ta(veryVeryVerbose);

        int       NUM_THREADS[] = { 1,  2,  4,  8 };
        const int NUM_NUM_THREADS = sizeof NUM_THREADS / sizeof *NUM_THREADS;
        bool      COLLECT_METRICS[] = { false, true };
        const int NUM_COLLECT_METRICS = sizeof COLLECT_METRICS /
                                        sizeof * COLLECT_METRICS;

        for (int i = 0; i < NUM_NUM_THREADS; ++i) {
            for (int j = 0; j < NUM_COLLECT_METRICS; ++j) {
                const int MAX_THREADS           = NUM_THREADS[i];
                const bool COLLECT_TIME_METRICS = COLLECT_METRICS[j];

                enum {
                    NUM_SOCKETS   = 1,
                    LOW_WATERMARK = 512,
                    HI_WATERMARK  = 4096
                };

                bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);
                btemt_ChannelPoolConfiguration config;
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

                btemt_ChannelPool::DataReadCallback         dataCb(
                        bdef_BindUtil::bindA( &ta
                                            , &dummyDataCallbackWithDelay
                                            , _1, _2, _3, _4
                                            , 0.5));

                btemt_ChannelPool::PoolStateChangeCallback    poolCb;
                makeNull(&poolCb);

                ChannelPoolStateCbTester mX(config, dataCb, poolCb, &ta);
                Obj& pool = mX.pool(); const Obj& POOL = mX.pool();
                bcema_PooledBufferChainFactory outFactory(HI_WATERMARK / 64,
                                                          &ta);

                ASSERT(0 == pool.start());

                int ret  = 0;
                int code = 0;
                bteso_SocketHandle::Handle handles[2];

                ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                    handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);

                ASSERT(0 == ret);
                ASSERT(0 == code);

                // The following socket options are set only if necessary.
                ret = bteso_SocketOptUtil::setOption(handles[1],
                                                     SockOpt::BTESO_TCPLEVEL,
                                                     SockOpt::BTESO_TCPNODELAY,
                                                     1);

                ASSERT(0 == ret);
                bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
                ASSERT(serverSocket);
                bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
                ASSERT(clientSocket);

                ASSERT(0 == pool.import(serverSocket, &factory, 0));

                char ONE_BYTE_MSG[] = { (char) 0xff };

                // Wait for the channel to come up and obtain the channel id.
                bsl::vector<ChannelPoolStateCbTester::ChannelState>
                                                                 states(&ta);
                ASSERT(0 == mX.waitForState(
                                           &states,
                                           btemt_ChannelPool::BTEMT_CHANNEL_UP,
                                           bdet_TimeInterval(1.0)));

                int channelId = mX.lastOpenedChannelId();
                ASSERT(0 != channelId);

                ASSERT(1 == clientSocket->write(ONE_BYTE_MSG, 1));

                bcemt_ThreadUtil::sleep(bdet_TimeInterval(.3));

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
      } break;
      case 27: {
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

        bcema_TestAllocator ta(veryVeryVerbose);

        using namespace TEST_CASE_25_NAMESPACE;

        double    TEST_VALUES[]   = { 0.0, 0.01, .5, 1.0 };
        const int NUM_TEST_VALUES = sizeof TEST_VALUES / sizeof *TEST_VALUES;
        for (int i = 0; i < NUM_TEST_VALUES; ++i) {

            double READ_TIMEOUT = TEST_VALUES[i];
            enum {
                NUM_SOCKETS   = 1,
                LOW_WATERMARK = 512,
                HI_WATERMARK  = 4096
            };

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);

            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setReadTimeout(READ_TIMEOUT);
            config.setWriteCacheWatermarks(LOW_WATERMARK,
                                           HI_WATERMARK);
            if (verbose) { P(config); }

            ChannelPoolStateCbTester mX(config, &ta);
            btemt_ChannelPool& pool = mX.pool();
            bcema_PooledBufferChainFactory outFactory(HI_WATERMARK / 64, &ta);

            ASSERT(0 == pool.start());

            int ret  = 0;
            int code = 0;
            bteso_SocketHandle::Handle handles[2];

            ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                            handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);

            ASSERT(0 == ret);
            ASSERT(0 == code);

            // The following socket options are set only if necessary.
            ret = bteso_SocketOptUtil::setOption(handles[1],
                                                 SockOpt::BTESO_TCPLEVEL,
                                                 SockOpt::BTESO_TCPNODELAY, 1);

            ASSERT(0 == ret);
            bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
            ASSERT(serverSocket);
            bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
            ASSERT(clientSocket);

            ASSERT(0 == pool.import(serverSocket, &factory, 0));

            bcema_Blob oneByteMsg(&ta);
            populateMessage(&oneByteMsg, 1, &ta);
            ASSERT(1 == oneByteMsg.length());

            // Wait for the channel to come up and obtain the channel id.
            bsl::vector<ChannelPoolStateCbTester::ChannelState> states(&ta);

            ASSERT(0 == mX.waitForState(&states,
                                        btemt_ChannelPool::BTEMT_CHANNEL_UP,
                                        bdet_TimeInterval(1.0)));

            int channelId = mX.lastOpenedChannelId();
            ASSERT(0 != channelId);

            bool timeoutShouldOccur = 0.0 != READ_TIMEOUT;

            bsls_Stopwatch timer;
            timer.start();

            // Test whether the timeout callback is registed for initial read.

            // Note the maximum tested read timeout value is 3s, so 5s should
            // be more than enough.
            LOOP_ASSERT(i,
                        timeoutShouldOccur ==
                        (0 == mX.waitForState(
                                         &states,
                                         btemt_ChannelPool::BTEMT_READ_TIMEOUT,
                                         bdet_TimeInterval(2.0))));

            timer.stop();
            if (timeoutShouldOccur) {
                MTLOOP2_ASSERT(READ_TIMEOUT, timer.elapsedTime(),
                               READ_TIMEOUT - 1.0 < timer.elapsedTime() &&
                               READ_TIMEOUT + 1.0 > timer.elapsedTime());
            }
            else {
                ASSERT(1 < timer.elapsedTime() && 3 > timer.elapsedTime());
            }

            // Test whether the timeout callback is registed when the read
            // timeout is invoked.
            timer.reset();
            timer.start();
            pool.write(channelId, oneByteMsg);

            MTLOOP_ASSERT(i,
                          timeoutShouldOccur ==
                          (0 == mX.waitForState(&states,
                                         btemt_ChannelPool::BTEMT_READ_TIMEOUT,
                                         bdet_TimeInterval(2.0))));

            timer.stop();
            if (timeoutShouldOccur) {
                ASSERT(READ_TIMEOUT - 1.0 < timer.elapsedTime() &&
                       READ_TIMEOUT + 1.0 > timer.elapsedTime());
            }
            else {
                ASSERT(1 < timer.elapsedTime() && 3 > timer.elapsedTime());
            }

            pool.stop();
            factory.deallocate(clientSocket);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING: struct 'btemt_ChannelPool_MessageUtil' and
        //          'btemt_ChannelPool_IovecArray'.
        //
        // Concerns:
        //     o That the nested class 'IovecArray' is a correctly defined
        //       in-core value-semantic type.
        //
        //     o That each of the static operations defined by
        //       'btemt_ChannelPool_MessageUtil' behaves correctly for each
        //       data type.
        //
        // Plan:
        //     o Create a standard value-semantic container test for
        //      'IovecArray<>' in a templatized external test method.  Invoke
        //       this method for both 'btes_Ovec' and 'btes_Iovec'.
        //
        //     o Create a set of test data, composed of a series of buffer
        //        sizes that will be filled with arbitrary data.  For each
        //        test data point, create a series of buffers matching the
        //        test data point description, and use it to populate a
        //        'IovecArray', a 'bcema_Blob', and a 'btemt_DataMsg' (i.e. a
        //        'bcema_PooledBufferChain').  Then invoke the functions
        //        defined in 'btemt_ChannelPool_MessageUtil' on each of the
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
        //  btemt_ChannelPool_IovecArray
        //  ----------------------------
        //   btemt_ChannelPool_IovecArray<IOVEC>(const IOVEC *, int );
        //  btemt_ChannelPool_IovecArray(const btemt_ChannelPool_IovecArray& );
        //   btemt_ChannelPool_IovecArray& operator=(
        //                               const btemt_ChannelPool_IovecArray& );
        //   const IOVEC *iovecs() const;
        //   int numIovecs() const;
        //   bsls_PlatformUtil::Int64 length() const;
        //
        //  btemt_ChannelPool_MessageUtil
        //  -----------------------------
        //   static bsls_PlatformUtil::Int64 length(const IovecArray<IOVEC>& );
        //   static bsls_PlatformUtil::Int64 length(const btemt_DataMsg& );
        //   static bsls_PlatformUtil::Int64 length(const bcema_Blob& );
        //   static int write(bteso_StreamSocket<bteso_IPv4Address> *,
        //                   btes_Iovec                            *,
        //                   const IovecArray<IOVEC>&               );
        //   static int write(bteso_StreamSocket<bteso_IPv4Address> *,
        //                   btes_Iovec                            *,
        //                  const bcema_Blob&                      );
        //   static int write(bteso_StreamSocket<bteso_IPv4Address> *,
        //                   btes_Iovec                            *,
        //                   const btemt_DataMsg&                   );
        //   static int loadIovec(btes_Iovec *, const bcema_Blob& );
        //   static int loadIovec(btes_Iovec *, const btemt_DataMsg& );
        //   static int loadBlob(bcema_Blob *, const IovecArray<IOVEC>&, int);
        //   static int loadBlob(bcema_Blob *, const btemt_DataMsg&, int);
        //   static int loadBlob(bcema_Blob *, const bcema_Blob&, int);
        //   static void appendToBlob(bcema_Blob *, const IovecArray<IOVEC>&);
        //   static void appendToBlob(bcema_Blob *, const btemt_DataMsg&);
        //   static void appendToBlob(bcema_Blob *, const bcema_Blob&);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: btemt_ChannelPool_MessageUtil\n"
                          << "========================================\n";

        using namespace TEST_CASE_MESSAGEHELPER_NAMESPACE;

        bcema_TestAllocator ta;
        typedef btemt_ChannelPool_MessageUtil Helper;
        typedef btemt_ChannelPool_IovecArray<btes_Ovec>   OvecArray;
        typedef btemt_ChannelPool_IovecArray<btes_Iovec>  IovecArray;

        {
            if (veryVerbose)
                cout << "Testing IovecArray constants" << bsl::endl;

#if defined(BSLS_PLATFORM__OS_UNIX) && defined(IOV_MAX) && IOV_MAX > 32
            ASSERT(Helper::BTEMT_MAX_IOVEC_SIZE == 32);
#elif defined(BSLS_PLATFORM__OS_UNIX) && defined(IOV_MAX)
            ASSERT(Helper::BTEMT_MAX_IOVEC_SIZE == IOV_MAX);
#else
            ASSERT(Helper::BTEMT_MAX_IOVEC_SIZE == 16);
#endif
        }

        if (veryVerbose) cout << "Testing IovecArray<btes_Ovec>" << bsl::endl;
        testIovecArray<btes_Ovec>();

        if (veryVerbose) cout << "Testing IovecArray<btes_Iovec>" << bsl::endl;
        testIovecArray<btes_Iovec>();

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

        bcema_PooledBlobBufferFactory blobFactory(BLOCK_SIZE, &ta);
        bcema_PooledBufferChainFactory chainFactory(BLOCK_SIZE, &ta);

        for (int i = 0; i < NUM_VALUES; ++i) {
            // ------------------- Create Test Data ---------------------------
            bsl::vector<bsl::vector<char> > data(&ta);
            const int LINE   = VALUES[i].d_line;
            const int LENGTH = createData(&data, VALUES[i].d_description);

            if (veryVerbose) {
                P_(VALUES[i].d_description); P(LENGTH);
            }

            // Create an 'IovecArray', 'MX'.
            bsl::vector<btes_Iovec> iovecs(&ta);
            loadIovecs(&iovecs, data);
            IovecArray mX(&iovecs.front(),iovecs.size());
            const IovecArray& MX = mX;

            // Create a 'bcema_Blob', 'MY'.
            bcema_Blob mY(&blobFactory, &ta); const bcema_Blob& MY = mY;
            loadBlob(&mY, data);

            // Create a 'btemt_DataMsg', 'MZ'.
            bcema_PooledBufferChain *chain = chainFactory.allocate(0);
            loadBufferChain(chain, data);
            btemt_DataMsg mZ(chain, &chainFactory, 0, &ta);
            const btemt_DataMsg& MZ = mZ;

            // ------------- Test MessageHelper::length() ---------------------
            ASSERT(LENGTH == Helper::length(MX));
            ASSERT(LENGTH == Helper::length(MY));
            ASSERT(LENGTH == Helper::length(MZ));

            // ------------- Test MessageHelper::loadBlob() -------------------

            for (int offset = 0; offset < LENGTH; ++offset) {
                bcema_Blob cX(&blobFactory, &ta); const bcema_Blob& CX = cX;
                bcema_Blob cY(&blobFactory, &ta); const bcema_Blob& CY = cY;
                bcema_Blob cZ(&blobFactory, &ta); const bcema_Blob& CZ = cZ;

                int rcX = Helper::loadBlob(&cX, MX, offset);
                int rcY = Helper::loadBlob(&cY, MY, offset);
                int rcZ = Helper::loadBlob(&cZ, MZ, offset);

                // We need to precompute the expected prefix of the returned
                // by 'loadBlob'.
                int expectedPrefixZ = offset % BLOCK_SIZE;
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
                ASSERT(expectedPrefixZ == rcZ);

                ASSERT(LENGTH - offset                   == CX.length());
                ASSERT(LENGTH - offset + expectedPrefixY == CY.length());
                ASSERT(LENGTH - offset + expectedPrefixZ == CZ.length());

                ASSERT(rcX < CX.buffer(0).size());
                ASSERT(rcY < CY.buffer(0).size());
                ASSERT(rcZ < CZ.buffer(0).size());

                ASSERT(rcX < CX.length());
                ASSERT(rcY < CY.length());
                ASSERT(rcZ < CZ.length());

                LOOP_ASSERT(LINE,
                       isEqualByteSequence(data, offset, CX, 0));
                LOOP_ASSERT(LINE,
                       isEqualByteSequence(data, offset, CY, expectedPrefixY));
                LOOP_ASSERT(LINE,
                       isEqualByteSequence(data, offset, CZ, expectedPrefixZ));
            }

            // ------------- Test MessageHelper::appendToBlob() --------------
            for (int j = 0; j < NUM_VALUES; ++j) {
                bsl::vector<bsl::vector<char> > prefixData(&ta);
                const int PREFIX_LEN = createData(&prefixData,
                                                  VALUES[j].d_description);

                bcema_Blob dX(&blobFactory, &ta); const bcema_Blob& DX = dX;
                bcema_Blob dY(&blobFactory, &ta); const bcema_Blob& DY = dY;
                bcema_Blob dZ(&blobFactory, &ta); const bcema_Blob& DZ = dZ;

                loadBlob(&dX, prefixData);
                loadBlob(&dY, prefixData);
                loadBlob(&dZ, prefixData);

                ASSERT(isEqualByteSequence(prefixData, 0, DX, 0));
                ASSERT(isEqualByteSequence(prefixData, 0, DY, 0));
                ASSERT(isEqualByteSequence(prefixData, 0, DZ, 0));

                Helper::appendToBlob(&dX, MX);
                Helper::appendToBlob(&dY, MY);
                Helper::appendToBlob(&dZ, MZ);

                ASSERT(LENGTH + PREFIX_LEN == DX.length());
                ASSERT(LENGTH + PREFIX_LEN == DY.length());
                ASSERT(LENGTH + PREFIX_LEN == DZ.length());

                // Sanity check that the initial data wasn't changed
                ASSERT(isEqualByteSequence(prefixData, 0, DX, 0, PREFIX_LEN));
                ASSERT(isEqualByteSequence(prefixData, 0, DY, 0, PREFIX_LEN));
                ASSERT(isEqualByteSequence(prefixData, 0, DZ, 0, PREFIX_LEN));

                ASSERT(isEqualByteSequence(data, 0, DX, PREFIX_LEN));
                ASSERT(isEqualByteSequence(data, 0, DY, PREFIX_LEN));
                ASSERT(isEqualByteSequence(data, 0, DZ, PREFIX_LEN));
            }

            // ------------- Test MessageHelper::loadIovec() --------------
            {
                const int NUM_IOVECS = 2 * Helper::BTEMT_MAX_IOVEC_SIZE;
                btes_Iovec dY[NUM_IOVECS];
                btes_Iovec dZ[NUM_IOVECS];

                int rY = Helper::loadIovec(dY, MY);
                int rZ = Helper::loadIovec(dZ, MZ);

                ASSERT(bsl::min(MY.numBuffers(),
                                (int)Helper::BTEMT_MAX_IOVEC_SIZE) == rY);
                ASSERT(bsl::min(MZ.data()->numBuffers(),
                                (int)Helper::BTEMT_MAX_IOVEC_SIZE) == rZ);

                // To utilize the existing comparison operation, load the data
                // into a blob
                bcema_Blob bY(&blobFactory, &ta); const bcema_Blob& BY = bY;
                bcema_Blob bZ(&blobFactory, &ta); const bcema_Blob& BZ = bZ;

                btes_IovecUtil::appendToBlob(&bY, dY, rY, 0);
                btes_IovecUtil::appendToBlob(&bZ, dZ, rZ, 0);

                ASSERT(isEqualByteSequence(data, 0, BY, 0));
                ASSERT(isEqualByteSequence(data, 0, BZ, 0));
            }

            // ------------- Test MessageHelper::write() ----------------------
            {
                const int NUM_IOVECS = 2 * Helper::BTEMT_MAX_IOVEC_SIZE;
                btes_Iovec temp[NUM_IOVECS];

                // Create two sockets, one to receive the message and the
                // other to send it.
                bteso_SocketHandle::Handle handles[2];
                bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);
                int ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                            handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);

                ASSERT(0 == ret);

                // The following socket options are set only if necessary.
                ret = bteso_SocketOptUtil::setOption(
                                                 handles[1],
                                                 SockOpt::BTESO_TCPLEVEL,
                                                 SockOpt::BTESO_TCPNODELAY, 1);

                ASSERT(0 == ret);
                bteso_StreamSocket<bteso_IPv4Address> *sndSocket =
                                                 factory.allocate(handles[0]);
                ASSERT(sndSocket);
                bteso_StreamSocket<bteso_IPv4Address> *rcvSocket =
                                                 factory.allocate(handles[1]);
                ASSERT(rcvSocket);

                // The actual length by 'Helper::write'  may be less that the
                // original data if the number of buffers is greater than
                // 'BTEMT_MAX_IOVEC_SIZE'.
                int LENGTHX, LENGTHY, LENGTHZ;
                {
                    btes_Iovec vY[NUM_IOVECS];
                    btes_Iovec vZ[NUM_IOVECS];

                    int numBufX = bsl::min(MX.numIovecs(),
                                           (int)Helper::BTEMT_MAX_IOVEC_SIZE);
                    int numBufY = Helper::loadIovec(vY, MY);
                    int numBufZ = Helper::loadIovec(vZ, MZ);

                    LENGTHX = btes_IovecUtil::length(MX.iovecs(), numBufX);
                    LENGTHY = btes_IovecUtil::length(vY, numBufY);
                    LENGTHZ = btes_IovecUtil::length(vZ, numBufZ);
                }

                sndSocket->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
                rcvSocket->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);

                bsl::vector<char> rcvBufferX(2 * LENGTH, 0, &ta);
                bsl::vector<char> rcvBufferY(2 * LENGTH, 0, &ta);
                bsl::vector<char> rcvBufferZ(2 * LENGTH, 0, &ta);

                // Send the iovec message.
                int bytesX = 0;
                ASSERT(LENGTHX == Helper::write(sndSocket, temp, MX));
                while (bytesX < LENGTHX) {
                    int rc = rcvSocket->read(&rcvBufferX[bytesX], 2 * LENGTH);
                    ASSERT(0 <= rc);
                    bytesX += rc;
                }

                // Send the 'bcema_Blob' message.
                int bytesY = 0;
                ASSERT(LENGTHY == Helper::write(sndSocket, temp, MY));
                while (bytesY < LENGTHY) {
                    int rc = rcvSocket->read(&rcvBufferY[bytesY], 2 * LENGTH);
                    ASSERT(0 <= rc);
                    bytesY += rc;
                }

                // Send the 'btemt_DataMsg' message.
                int bytesZ = 0;
                ASSERT(LENGTHZ == Helper::write(sndSocket, temp, MZ));
                while (bytesZ < LENGTHZ) {
                    int rc = rcvSocket->read(&rcvBufferZ[bytesZ], 2 * LENGTH);
                    ASSERT(0 <= rc);
                    bytesZ += rc;
                }

                // Verify the correct data was received (load it into a
                // 'bcema_Blob' to use the existing memory comparison
                // function).
                ASSERT(bytesX == LENGTHX);
                ASSERT(bytesY == LENGTHY);
                ASSERT(bytesZ == LENGTHZ);

                bcema_SharedPtr<char> spX(&rcvBufferX.front(),
                                          bcema_SharedPtrNilDeleter(), 0);
                bcema_SharedPtr<char> spY(&rcvBufferY.front(),
                                          bcema_SharedPtrNilDeleter(), 0);
                bcema_SharedPtr<char> spZ(&rcvBufferZ.front(),
                                          bcema_SharedPtrNilDeleter(), 0);

                bcema_Blob bX(&blobFactory, &ta); const bcema_Blob& BX = bX;
                bcema_Blob bY(&blobFactory, &ta); const bcema_Blob& BY = bY;
                bcema_Blob bZ(&blobFactory, &ta); const bcema_Blob& BZ = bZ;

                bX.appendDataBuffer(bcema_BlobBuffer(spX, bytesX));
                bY.appendDataBuffer(bcema_BlobBuffer(spY, bytesY));
                bZ.appendDataBuffer(bcema_BlobBuffer(spZ, bytesZ));

                ASSERT(isEqualByteSequence(data, 0, BX, 0, LENGTHX));
                ASSERT(isEqualByteSequence(data, 0, BY, 0, LENGTHY));
                ASSERT(isEqualByteSequence(data, 0, BZ, 0, LENGTHZ));

                factory.deallocate(sndSocket);
                factory.deallocate(rcvSocket);
            }
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING: 'setWriteCacheHighWatermark()'
        //
        // Concerns:
        //   o That 'setWriteCacheHighWatermark()' modifies the high watermark
        //     of the specified channel (and only the specified channel)
        //   o That the 'HIWAT' alert is delivered properly if the specified
        //     cache size is smaller than the cache.
        //   o That the 'HIWAT" alert is delivered, if the cache size limit
        //     has been reached, then it is expanded, and it is reached again.
        //   o That 'setWriteCacheHighWatermark()' is thread-safe.
        //
        // Plan:
        //   Create an instance under test.  Create a local socket pair and
        //   import it into the channel pool.  Then:
        //
        //   1. Verify that setWriteCacheHiWatermark() fails for succeeds or
        //     fails appropriately based on channelId and numBytes.
        //   2. Fill the write cache, verify the 'HIWAT' alert is delivered
        //      and no more data can be written.
        //   3. Double the write-cache size, verify that the 'HIWAT' alert
        //      is not delivered.  Then refill the cache and verify that the
        //      cache increased by the expected amount and that 'HIWAT' is
        //      delivered.
        //   4. Double the write-cache size again, add one byte (to verify
        //      it is not full), then reduce the write-cache size to the
        //      original size and verify the 'HIWAT' alert is generated
        //   5. Increase the write cache size back to 2 * HI_WATERMARK (one
        //      byte less than is currently in the cache) verify that the
        //      'HIWAT' alert is generated an no data can be written to the
        //      cache.
        //   6. Empty the write-cache and perform a concurrency test.
        // Testing:
        //   int setWriteCacheHighWatermark(int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: setWriteCacheHighWatermark()" << endl
                          << "=====================================" << endl;

        using namespace TEST_CASE_25_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        {
            enum {
                NUM_SOCKETS   = 1,
                LOW_WATERMARK = 512,
                HI_WATERMARK  = 4096
            };

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);

            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setReadTimeout(100000);
            config.setWriteCacheWatermarks(LOW_WATERMARK,
                                           HI_WATERMARK);
            if (verbose) { P(config); }

            ChannelPoolStateCbTester mX(config, &ta);
            btemt_ChannelPool& pool = mX.pool();
            bcema_PooledBufferChainFactory outFactory(HI_WATERMARK / 64, &ta);

            ASSERT(0 == pool.start());

            int ret  = 0;
            int code = 0;
            bteso_SocketHandle::Handle handles[2];

            ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                            handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);

            ASSERT(0 == ret);
            ASSERT(0 == code);

            // The following socket options are set only if necessary.
            ret = bteso_SocketOptUtil::setOption(handles[1],
                                                 SockOpt::BTESO_TCPLEVEL,
                                                 SockOpt::BTESO_TCPNODELAY, 1);

            ASSERT(0 == ret);
            bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                 factory.allocate(handles[0]);
            ASSERT(serverSocket);
            bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                                                 factory.allocate(handles[1]);
            ASSERT(clientSocket);

            ASSERT(0 == pool.import(serverSocket, &factory, 0));

            bcema_Blob oneByteMsg(&ta);
            populateMessage(&oneByteMsg, 1, &ta);
            ASSERT(1 == oneByteMsg.length());

            // Wait for the channel to come up and obtain the channel id.
            bsl::vector<ChannelPoolStateCbTester::ChannelState> states(&ta);
            ASSERT(0 == mX.waitForState(&states,
                                        btemt_ChannelPool::BTEMT_CHANNEL_UP,
                                        bdet_TimeInterval(1.0)));

            int channelId = mX.lastOpenedChannelId();
            ASSERT(0 != channelId);

            // ------------------ We are now ready to perform tests  --------

            // 1. Verify that setWriteCacheHiWatermark() fails for succeeds or
            //    fails appropriately based on channelId and numBytes.
            if (verbose) {
                bsl::cout << "\tVerify invalid arguments are rejected"
                          << bsl::endl;
            }
            ASSERT(0 != pool.setWriteCacheHiWatermark(channelId + 1,
                                                      LOW_WATERMARK + 1));
            ASSERT(0 == pool.setWriteCacheHiWatermark(channelId,
                                                      LOW_WATERMARK + 1));
            ASSERT(0 == pool.setWriteCacheHiWatermark(channelId,
                                                      LOW_WATERMARK))
            ASSERT(0 != pool.setWriteCacheHiWatermark(channelId,
                                                      LOW_WATERMARK - 1));
            ASSERT(0 == pool.setWriteCacheHiWatermark(channelId,
                                                      HI_WATERMARK));

            // 2. Fill the write cache, verify the 'HIWAT' alert is delivered
            //    and no more data can be written.

            int rc = 0, numBytesWritten  = 0, totalBytesWritten = 0;

            // We attempt to fill the write-cache over a period of 1 second.
            // The delay is required,  otherwise the cache may temporary fill,
            // but will empty as data is transmitted to the client (and not
            // read).  Subsequent tests require that the cache starts full.
            for (int i = 0; i < 4; ++i) {

                while (0 == (rc = pool.write(channelId, oneByteMsg))) {
                    numBytesWritten   += oneByteMsg.length();
                    totalBytesWritten += oneByteMsg.length();
                }
                bcemt_ThreadUtil::sleep(bdet_TimeInterval(.25));
            }
            if (veryVerbose) {
                PT2(numBytesWritten, totalBytesWritten);
            }
            int sts = pool.write(channelId, oneByteMsg);
            LOOP_ASSERT(sts, 0 != sts);
            ASSERT(0 ==  mX.waitForState(
                                    &states,
                                    btemt_ChannelPool::BTEMT_WRITE_CACHE_HIWAT,
                                    bdet_TimeInterval(0.25)));

            // 3. Double the write-cache size, verify that the 'HIWAT' alert
            //    is not delivered.  Then refill the cache and verify that the
            //    cache increased by the expected amount and that 'HIWAT' is
            //    delivered.
            if (verbose) {
                bsl::cout << "\tVerify increasing the cache size" << bsl::endl;
            }
            ASSERT(0 ==
                   pool.setWriteCacheHiWatermark(channelId, 2 * HI_WATERMARK));
            ASSERT(0 !=  mX.waitForState(
                                    &states,
                                    btemt_ChannelPool::BTEMT_WRITE_CACHE_HIWAT,
                                    bdet_TimeInterval(0.25)));

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
            ASSERT(0 ==  mX.waitForState(
                                    &states,
                                    btemt_ChannelPool::BTEMT_WRITE_CACHE_HIWAT,
                                    bdet_TimeInterval(0.25)));

            // 4. Double the write-cache size again, add one byte (to verify
            //    it is not full), then reduce the write-cache size to the
            //    original size and verify the 'HIWAT' alert is generated
            if (verbose) {
                bsl::cout << "\tVerify decreasing the cache size & generating "
                          << "a 'HIWAT' alert" << bsl::endl;
            }
            ASSERT(0 ==
                   pool.setWriteCacheHiWatermark(channelId, 4 * HI_WATERMARK));

            ASSERT(0 == pool.write(channelId, oneByteMsg));
            totalBytesWritten += oneByteMsg.length();
            ASSERT(0 !=  mX.waitForState(
                                    &states,
                                    btemt_ChannelPool::BTEMT_WRITE_CACHE_HIWAT,
                                    bdet_TimeInterval(0.25)));

            ASSERT(0 ==
                   pool.setWriteCacheHiWatermark(channelId, HI_WATERMARK));
            ASSERT(0 ==  mX.waitForState(
                                    &states,
                                    btemt_ChannelPool::BTEMT_WRITE_CACHE_HIWAT,
                                    bdet_TimeInterval(0.25)));
            ASSERT(0 != pool.write(channelId, oneByteMsg));

            // 5. Increase the write cache size back to 2 * HI_WATERMARK (one
            //    byte less than is currently in the cache) verify that no
            //    'HIWAT' alert is generated an no data can be written to the
            //    cache.
            if (verbose) {
                bsl::cout << "\tVerify a 'HIWAT' alert is not generated when "
                          << "the cache size grows to a value smaller than "
                          << "the cache" << bsl::endl;
            }
            ASSERT(0 ==
                   pool.setWriteCacheHiWatermark(channelId, 2 * HI_WATERMARK));
            ASSERT(0 !=  mX.waitForState(
                                    &states,
                                    btemt_ChannelPool::BTEMT_WRITE_CACHE_HIWAT,
                                    bdet_TimeInterval(0.25)));
            ASSERT(0 != pool.write(channelId, oneByteMsg));

            // 6. Empty the write-cache and perform a concurrency test.
            if (verbose) {
                bsl::cout << "\tConcurrency Test" << bsl::endl;
            }
            int numBytesRead = drainSocket(clientSocket, totalBytesWritten);
            LOOP2_ASSERT(numBytesRead, totalBytesWritten,
                                            numBytesRead == totalBytesWritten);
            TestCase25ConcurrencyTest concurrencyTest(&pool,
                                                      channelId,
                                                      clientSocket,
                                                      CASE_25_NUM_THREADS);
            concurrencyTest.run();
            pool.stop();
            factory.deallocate(clientSocket);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING 'enableRead' FIX
        //
        // Concerns:
        // * That a channel which is disabled for read but has data read off
        //   the socket and enqueued in the internal buffers, is processing the
        //   data as soon as the channel is re-enabled for read, even though no
        //   new data was written to the socket in the interval disabled for
        //   read.
        //
        // Plan:
        //   Establish a client-server pair, using two distinct channel pools
        //   for the server ('mX') and for the client ('mY').  With the server
        //   enabled for read, write a message from the client, but let the
        //   data callback refuse to process the data (leaving the data in the
        //   internal 'd_currentMsg' buffer); then disable both server-side and
        //   client-side automatic reading.  In the channel state callback for
        //   the server, upon receiving the AUTO_READ_DISABLED signal, trigger
        //   a global variable to let the server data callback know that it
        //   should now start accepting data and function as an echo server,
        //   and subsequently re-enable the server-side channel for automatic
        //   reading (in the channel state callback is fine, or in the main
        //   thread, serialized by a thread barrier).  Assert that data is then
        //   actually read and echoed by the server, even though no new data
        //   was written to the server in the interval disabled for read.
        //   Finally, enable the client-side for reading, and assert that data
        //   is actually received by the client callback, even though no data
        //   was present in the internal buffer prior to the 'enableRead' but
        //   data was written to the client socket by the echo server in the
        //   interval disabled for read by the client.
        //
        // Testing:
        //   int btemt_ChannelPool::disableRead();
        //   int btemt_ChannelPool::enableRead();
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting 'enableRead' fix."
                 << "\n=========================" << endl;

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING HALF-OPEN CONNECTIONS
        //
        // Concerns:
        // * That a channel created in half-open mode can be partially shut
        //   down by peer and still continue to function in the other half.
        // * That a CHANNEL_DOWN_READ or CHANNEL_DOWN_WRITE callback is
        //   invoked only once when half of the channel is closed.
        // * That a channel partially shut down is closed when the second half
        //   is closed.
        //
        // Plan:
        //   Establish a client-server pair, using two distinct channel pools
        //   for the server ('mX') and for the client ('mY').  There are three
        //   cases of interest, depending on who is responsible for the
        //   shutdown and on which part of the channel (read or write):
        //   - Server shuts down read: after receiving a client request, the
        //     server shuts down the read part, but later sends down a
        //     response.  Verify that the client indeed shuts down its write
        //     part of the channel, and so cannot enqueue another request, but
        //     does receive the server response.  Upon server shutting down the
        //     write part as well, verify that the server channel is fully shut
        //     down, and that the client (enabled for automatic reading) is now
        //     shut down for read, and hence fully shut down as well.
        //   - Client shuts down read: after sending the server a request but
        //     prior to getting the response, the client shuts down its read
        //     part of the channel.  Verify that the server consequently shuts
        //     down its write part of the channel, as a result of attempting to
        //     send a response.  Upon client shutting down its  write part as
        //     well, verify that the server channel (always enabled for
        //     automatic reading) is now shut down for read, and hence fully
        //     shut down as well.
        //   - Client shuts down write: after sending the server a request but
        //     prior to getting the response, the client shuts down its write
        //     part of the channel.  Verify that the server consequently shuts
        //     down its read part of the channel, as a result of attempting to
        //     read on the channel, but still manages to send a response
        //     (delayed to make sure the response isn't sent while the server
        //     still think the full duplex channel is alive).  Verify that the
        //     client does receive the response.  Upon client shutting down its
        //     read part as well, verify that the client channel is now fully
        //     shut down as well.  Verify also that either shutting down the
        //     write part of the server channel, or the server attempting to
        //     write another response onto that channel, will result in the
        //     complete shutdown of the channel server-side.
        //
        // Testing:
        //   int btemt_ChannelPool::connect();
        //   int btemt_ChannelPool::listen();
        //   int btemt_ChannelPool::shutdown();
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting half-open connections."
                 << "\n==============================" << endl;

#if 0
        using namespace TEST_CASE_22_NAMESPACE;  // reusing
        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 1,
                MAX_THREADS  = 1,

                SERVER_ID    = 0x701d50da,
                USER_ID      = 0x12345678,
                BACKLOG      = 1,

                TIMEOUT      = 1   // 1s
            };

            bteso_InetStreamSocketFactory<bteso_IPv4Address>  factory;

            btemt_ChannelPool::ChannelStateChangeCallback channelCb;
            btemt_ChannelPool::PoolStateChangeCallback    poolCb;
            btemt_ChannelPool::DataReadCallback           dataCb;

            bcemt_Barrier clientServerBarrier(2); // client and server threads

            bcemt_Barrier serverStateBarrier(2); // server control thread and
                                                 // channelStateCallback of mY

            bcemt_Barrier serverDataBarrier(2);  // server control thread and
                                                 // dataCallback of mY

            case23CallbackInfo serverInfo;
            serverInfo.d_clientServerBarrier_p = &clientServerBarrier;
            serverInfo.d_stateBarrier_p = &serverStateBarrier;
            serverInfo.d_dataBarrier_p = &serverDataBarrier;
            serverInfo.d_channelId = -1;
            serverInfo.d_channelDownReadFlag = 0;
            serverInfo.d_channelDownWriteFlag = 0;
            serverInfo.d_msgLength = sizeof(MESSAGE);
            serverInfo.d_echoMode = true;

            channelCb = bdef_BindUtilA(&ta,
                                       case23ChannelStateCallback,
                                       _1, _2, _3, _4,
                                       &serverInfo);

            makeNull(&poolCb);

            dataCb = bdef_BindUtilA(&ta,
                                    case23DataCallback,
                                    _1, _2, _3, _4,
                                    &serverInfo);

            btemt_ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setReadTimeout(1);    // in seconds
            cpc.setIncomingMessageSizes(1, 32, 256);

            btemt_ChannelPool mX(channelCb, dataCb, poolCb, cpc, &ta);
            serverInfo.d_channelPool_p = &mX;

            bcemt_Barrier clientStateBarrier(2); // client control thread and
                                                 // channelStateCallback of mY

            bcemt_Barrier clientDataBarrier(2);  // client control thread and
                                                 // dataCallback of mY

            case23CallbackInfo clientInfo;
            clientInfo.d_clientServerBarrier_p = &clientServerBarrier;
            clientInfo.d_stateBarrier_p = &clientStateBarrier;
            clientInfo.d_dataBarrier_p = &clientDataBarrier;
            clientInfo.d_channelId = -1;
            clientInfo.d_channelDownReadFlag = 0;
            clientInfo.d_channelDownWriteFlag = 0;
            clientInfo.d_msgLength = sizeof(MESSAGE);
            clientInfo.d_echoMode = false;

            channelCb = bdef_BindUtil::bindA(&ta,
                                             case23ChannelStateCallback,
                                             _1, _2, _3, _4,
                                             &clientInfo);

            dataCb = bdef_BindUtil::bindA(&ta,
                                          case23DataCallback,
                                          _1, _2, _3, _4,
                                          &clientInfo);

            btemt_ChannelPool mY(channelCb, dataCb, poolCb, cpc, &ta);
            clientInfo.d_channelPool_p = &mY;

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            if (verbose)
                cout << "\tServer shuts down read." << endl;
            serverInfo.d_channelId = -1;
            serverInfo.d_channelDownReadFlag = 0;
            serverInfo.d_channelDownWriteFlag = 0;
            clientInfo.d_channelId = -1;
            clientInfo.d_channelDownReadFlag = 0;
            clientInfo.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.start());
                ASSERT(0 == mY.start());

                ASSERT(0 == mX.listen(0, BACKLOG, SERVER_ID));
                bteso_IPv4Address peer = *mX.serverAddress(SERVER_ID);

                ASSERT(0 == mX.stop());
                ASSERT(0 == mY.stop());
            }
            bcemt_ThreadUtil::microSleep(0, 2);

            if (verbose)
                cout << "\tClient shuts down read." << endl;
            serverInfo.d_channelId = -1;
            serverInfo.d_channelDownReadFlag = 0;
            serverInfo.d_channelDownWriteFlag = 0;
            clientInfo.d_channelId = -1;
            clientInfo.d_channelDownReadFlag = 0;
            clientInfo.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.start());
                ASSERT(0 == mY.start());

                ASSERT(0 == mX.listen(0, BACKLOG, SERVER_ID));
                bteso_IPv4Address peer = *mX.serverAddress(SERVER_ID);

                ASSERT(0 == mX.stop());
                ASSERT(0 == mY.stop());
            }

            if (verbose)
                cout << "\tClient shuts down write." << endl;
            serverInfo.d_channelId = -1;
            serverInfo.d_channelDownReadFlag = 0;
            serverInfo.d_channelDownWriteFlag = 0;
            clientInfo.d_channelId = -1;
            clientInfo.d_channelDownReadFlag = 0;
            clientInfo.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.start());
                ASSERT(0 == mY.start());

                ASSERT(0 == mX.listen(0, BACKLOG, SERVER_ID));
                bteso_IPv4Address peer = *mX.serverAddress(SERVER_ID);

                ASSERT(0 == mX.connect(peer, 1,
                                    bdet_TimeInterval(TIMEOUT),
                                    USER_ID,
                                    false,
                                    btemt_ChannelPool::BTEMT_KEEP_HALF_OPEN));

                bcemt_ThreadUtil::Handle serverHandle;
                ASSERT(0 == bcemt_ThreadUtil::create(&serverHandle,
                                                     &case23ServerThread3,
                                                     &serverInfo));

                bcemt_ThreadUtil::Handle clientHandle;
                ASSERT(0 == bcemt_ThreadUtil::create(&clientHandle,
                                                     &case23ClientThread3,
                                                     &clientInfo));

                bcemt_ThreadUtil::join(serverHandle);
                bcemt_ThreadUtil::join(clientHandle);
            }
            bcemt_ThreadUtil::microSleep(0, 1);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }
#endif

      } break;
      case 22: {
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

        using namespace TEST_CASE_22_NAMESPACE;
        typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> Factory;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            if (verbose) cout << "\tSmall message test." << endl
                              << "\t-------------------" << endl;

            // Small individual messages on the client size, with NUM_THREADS
            // client, and one server accepting large messages.

            enum {
#if !defined(BSLS_PLATFORM__CPU_X86) && !defined(BSLS_PLATFORM__CPU_X86_64) \
 && !defined(BSLS_PLATFORM__OS_AIX)
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

            runTestCase22(argv[0],
                          NUM_THREADS, NUM_ITERS,
                          BUFFER_SIZE, MSG_SIZE,
                          SERVER_SIZE, 1,
                          &factory, &ta);

            if (verbose) cout << "\n\t\tInvoke data callback after "
                              << SERVER_NEEDED << " bytes only." << endl;

            runTestCase22(argv[0],
                          NUM_THREADS, NUM_ITERS,
                          BUFFER_SIZE, MSG_SIZE,
                          SERVER_SIZE, SERVER_NEEDED,
                          &factory, &ta);

            if (verbose) cout << "\n\t\tInvoke data callback every "
                              << SMALL_MSG_SIZE << " bytes (msg boundaries)."
                              << endl;

            runTestCase22(argv[0],
                          2, LARGE_NUM_ITERS,
                          BUFFER_SIZE, -SMALL_MSG_SIZE,
                          SERVER_SIZE, SERVER_NEEDED,
                          &factory, &ta);

            if (verbose) cout << "\n\t\tInvoke data callback every "
                              << MSG_SIZE << " bytes (msg boundaries)."
                              << endl;

            runTestCase22(argv[0],
                          2, LARGE_NUM_ITERS,
                          BUFFER_SIZE, -MSG_SIZE,
                          SERVER_SIZE, SERVER_NEEDED,
                          &factory, &ta);
        }
        ASSERT(0 <=  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

#if !defined(BSLS_PLATFORM__OS_AIX)
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

            runTestCase22(argv[0],
                          NUM_THREADS, NUM_ITERS,
                          BUFFER_SIZE, MSG_SIZE,
                          SERVER_SIZE, 1,
                          &factory, &ta);

            if (verbose) cout << "\n\t\tInvoke data callback after "
                              << SERVER_NEEDED << " bytes only." << endl;

            runTestCase22(argv[0],
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

            runTestCase22(argv[0],
                          NUM_THREADS, NUM_ITERS,
                          BUFFER_SIZE, MSG_SIZE,
                          SERVER_SIZE, 1,
                          &factory, &ta);

            // Only invoke data callback when 'SERVER_NEEDED' bytes are
            // available.

            runTestCase22(argv[0],
                          NUM_THREADS, NUM_ITERS,
                          BUFFER_SIZE, MSG_SIZE,
                          SERVER_SIZE, SERVER_NEEDED,
                          &factory, &ta);
        }
        ASSERT(0 <=  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }
#endif

      } break;
      case 21: {
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

        using namespace TEST_CASE_21_NAMESPACE;
        typedef bteso_SslLikeStreamSocketFactory<bteso_IPv4Address> Factory;
        const int SSL_LIKE_SOCKET_BUFFER_SIZE = 4 * 65536;

        bcema_TestAllocator ta(veryVeryVerbose);

        // The following is a copy-paste of test case 9 but using
        // bteso_SslLikeStreamSocket instead of bteso_InetStreamSocket.
        {
            using namespace TEST_CASE_9_NAMESPACE;

            Factory factory(SSL_LIKE_SOCKET_BUFFER_SIZE, &ta);
            runTestCase9(argv[0], &factory, ta);
                // Note: ta passed by reference
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

        // The following is a copy-paste of test case 11, but using
        // bteso_SslLikeStreamSocket instead of bteso_InetStreamSocket.
        {
            using namespace TEST_CASE_11_NAMESPACE;

            Factory factory(SSL_LIKE_SOCKET_BUFFER_SIZE, &ta);
            runTestCase11(&factory, ta); // Note: ta passed by reference
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: DRQS 8397003
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
        //   Concern: DRQS 8397003
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_UNIX
        if (verbose) cout << "Testing Concern: DRQS 8397003" << endl
                          << "=============================" << endl;

        using namespace TEST_CASE_20_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        {
            bsl::vector<my_ChannelEvent> channelEvents;
            bcemt_Mutex                  channelEventsMutex;

            enum {
                NUM_SOCKETS   = 100,
                MAX_THREADS   = 10,
                SERVER_ID     = 1013410001,
                BACKLOG       = 100,
                SEND_SIZE     = 16384, // 1024,
                SEND_BUFFERS  = 16
            };

            int managersStatistics[MAX_THREADS];
            bcemt_Barrier channelBarrier(2);

            // Initialize per-manager counters.
            bsl::fill_n(managersStatistics, (int)MAX_THREADS, 0);

            // Initialize IO Vecs that will be written by the object under
            // testing.
            const char *TEXT[SEND_BUFFERS][SEND_SIZE];  // contents don't
                                                        // matter
            btes_Iovec messageVecs[SEND_BUFFERS];
            for (int i = 0; i < SEND_BUFFERS; ++i) {
                messageVecs[i].setBuffer(TEXT, SEND_SIZE);
            }

            // Set up channel pool config.
            btemt_ChannelPoolConfiguration config;
            config.setMaxConnections(NUM_SOCKETS);
            config.setMaxThreads(MAX_THREADS);
            config.setMetricsInterval(100.0);

            // Set up channel pool.
            typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> Factory;
            typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);

            btemt_ChannelPool *poolAddr;
            int                poolEvent = -1;
            int               *eventAddr = &poolEvent;
            int                channelId;

            Factory factory(&ta);

            // This pool will import.
            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case20ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &eventAddr
                                        , &channelBarrier
                                        , &channelId));

            btemt_ChannelPool::PoolStateChangeCallback    poolCb;
            makeNull(&poolCb);

            btemt_ChannelPool::DataReadCallback         dataCb;
            makeNull(&dataCb);

            Obj mX(channelCb, dataCb, poolCb, config, &ta);
            const Obj& X = mX;
            poolAddr = &mX;

            ASSERT(0 == mX.start());
            ASSERT(0 == X.numChannels());

            // And this pool will simply accept connections and consume the
            // data at a certain rate.
            btemt_ChannelPoolConfiguration config2;
            config.setMaxConnections(NUM_SOCKETS);
            config.setMaxThreads(1);

            btemt_ChannelPool::ChannelStateChangeCallback channelCb2;
            makeNull(&channelCb2);

            btemt_ChannelPool::DataReadCallback dataCb2(&case20DataCallback,
                                                        &ta);

            btemt_ChannelPool::PoolStateChangeCallback    poolCb2;
            makeNull(&poolCb2);

            Obj mY(channelCb2, dataCb2, poolCb2, config, &ta);
            const Obj& Y = mY;

            ASSERT(0 == mY.start());
            ASSERT(0 == mY.listen(ADDRESS, BACKLOG, SERVER_ID));
            ASSERT(0 == Y.numChannels());
            const bteso_IPv4Address PEER = *mY.serverAddress(SERVER_ID);

            // Import lots of sockets and send some data.
            if (verbose) cout << "Importing sockets.\n";
            bteso_StreamSocket<bteso_IPv4Address> *clientSockets[NUM_SOCKETS];
            for (int i = 0; i < NUM_SOCKETS; ++i) {
                Socket  *socket = factory.allocate();
                clientSockets[i] = socket;

                int ret = socket->connect(PEER);
                LOOP_ASSERT(i, 0 == ret);

                if (0 == ret) {
                    ret = mX.import(socket, &factory, i);
                    LOOP_ASSERT(i, 0 == ret);
                    if (0 == ret) {
                        // Other barrier is in case20ChannelStateCallback,
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
                             (btes_Iovec*)&messageVecs,
                             SEND_BUFFERS);
                }
            }

            bcemt_ThreadUtil::microSleep(0, 3); // 3 sec should be good enough

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

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: DRQS 5425522
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
        //   Concern: DRQS 5425522
        // --------------------------------------------------------------------

#ifdef BSLS_PLATFORM__OS_UNIX
        if (verbose)
             cout << "\nTesting Concern: DRQS 5425522"
                  << "\n=============================" << endl;

        using namespace TEST_CASE_19_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_THREADS = 5,
                SERVER_ID   = 1013410001,
                BACKLOG     = 1
            };

            if (verbose)
               cout << "Configuring channel pool and setting FD limit" << endl;

            bces_AtomicInt64 acceptErrors = 0;

            btemt_ChannelPool::ChannelStateChangeCallback channelCb;
            makeNull(&channelCb);

            btemt_ChannelPool::PoolStateChangeCallback    poolCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case19PoolStateCallback
                                        , _1, _2, _3
                                        , &acceptErrors));

            btemt_ChannelPool::DataReadCallback         dataCb;
            makeNull(&dataCb);

            btemt_ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_THREADS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(100.0);

            struct rlimit rlim;
            ASSERT(0 == getrlimit(RLIMIT_NOFILE, &rlim));
#if defined(BSLS_PLATFORM__OS_AIX) || defined(BSLS_PLATFORM__OS_LINUX)
            rlim.rlim_cur = 4 * MAX_THREADS + 2;
#else
            rlim.rlim_cur = 4 * MAX_THREADS + 5;
#endif
            ASSERT(0 == setrlimit(RLIMIT_NOFILE, &rlim));
            if (veryVerbose) { P(rlim.rlim_cur); }

            Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == mX.start());
            ASSERT(0 == mX.listen(ADDRESS, BACKLOG, SERVER_ID));
            ASSERT(0 == X.numChannels());
            const bteso_IPv4Address PEER = *mX.serverAddress(SERVER_ID);

            typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> Factory;
            typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;
            typedef btesos_TcpChannel                                Channel;

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
                    ASSERT(0 == bteso_SocketImpUtil::cleanup());
                    return testStatus;
                }
                LOOP_ASSERT(i, sockets[i]);
                retCode = sockets[i]->setBlockingMode(
                                              bteso_Flag::BTESO_BLOCKING_MODE);
                LOOP2_ASSERT(i, retCode, 0 == retCode);
            }

            for (int i = 0; i < MAX_THREADS; ++i) {
                retCode = sockets[i]->connect(PEER);
                if (veryVerbose) { P_(retCode); P(errno); }
            }

            // Give time to the channel pool to process accept error callbacks.
            bcemt_ThreadUtil::microSleep(0, 8);

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

            bcemt_ThreadUtil::microSleep(0, 8);
            acceptErrors = 0;

            if (verbose)
                cout << "Establishing connection (should succeed)" << endl;
#ifndef BSLS_PLATFORM__OS_LINUX
            // The Linux machine used in the nightly builds does not properly
            // reset the FD limit to its max value (more exactly, it does but
            // it still fails to accept beyond the previous FD limit).  Comment
            // out this code for now, until a new test machine is found.

            for (int i = 0; i < MAX_THREADS; ++i) {
                sockets[i] = factory.allocate();
                LOOP_ASSERT(i, sockets[i]);
                retCode = sockets[i]->setBlockingMode(
                                              bteso_Flag::BTESO_BLOCKING_MODE);
                LOOP2_ASSERT(i, retCode, 0 == retCode);
            }

            for (int i = 0; i < MAX_THREADS; ++i) {
                retCode = sockets[i]->connect(PEER);
                // LOOP2_ASSERT(i, retCode, 0 == retCode);
                if (veryVerbose) { P_(retCode); P(errno); }
            }

            // Give time to the channel pool to process accept error callbacks.
            bcemt_ThreadUtil::microSleep(0, 10);

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

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: DRQS 4430835
        //
        // Concerns:
        //   * That a channel pool can only create the number of channels
        //     specified by the 'maxConnections' argument of the
        //     'btemt_ChannelPoolConfiguration'.  In particular, we are
        //     concerned that
        //        - exactly this many channels may be allocated for accepting
        //        - exactly this many channels may be allocated for connecting
        //        - exactly this many channels may be allocated for some
        //          combination of accepting and connecting
        //   * That a channel that is no longer in use is freed for re-use by
        //     the channel pool.
        //
        // Plan:
        //   Instantiate a modifiable 'btemt_ChannelPool', 'mX', configured
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
        //   Concern: DRQS 4430835
        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Concern: DRQS 4430835" << endl
                 << "=============================" << endl;

        using namespace TEST_CASE_18_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS = 16,
                MAX_THREADS = 2,
                SERVER_ID   = 1010010001,
                BACKLOG     = 1
            };

            bcemt_Barrier  channelBarrier(2);
            bces_AtomicInt limitReachedFlag;

            btemt_ChannelPool *poolAddr;
            int                poolEvent = -1;
            int               *eventAddr = &poolEvent;

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case18ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &eventAddr
                                        , &channelBarrier));

            btemt_ChannelPool::PoolStateChangeCallback    poolCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case18PoolStateCallback
                                        , _1, _2, _3
                                        , &eventAddr
                                        , &limitReachedFlag));

            btemt_ChannelPool::DataReadCallback         dataCb;
            makeNull(&dataCb);

            btemt_ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setIncomingMessageSizes(1, 32, 256);

            Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;
            poolAddr = &mX;

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == mX.start());
            int retListen = mX.listen(ADDRESS, BACKLOG, SERVER_ID);
            LOOP_ASSERT(retListen, 0 == retListen);
            ASSERT(0 == X.numChannels());
            const bteso_IPv4Address PEER = *mX.serverAddress(SERVER_ID);

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> Factory;
            typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;
            typedef btesos_TcpChannel                                Channel;

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
                    bcemt_ThreadUtil::microSleep(200000);
                }
                while (!limitReachedFlag) {
                    bcemt_ThreadUtil::microSleep(200000);
                    bcemt_ThreadUtil::yield();
                }
                ASSERT(btemt_PoolMsg::BTEMT_CHANNEL_LIMIT == poolEvent);
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
                    bcemt_ThreadUtil::microSleep(250000);
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
                        bcemt_ThreadUtil::microSleep(200000);
                        bcemt_ThreadUtil::yield();
                    }
                    ASSERT(btemt_PoolMsg::BTEMT_CHANNEL_LIMIT == poolEvent);
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

                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(400000);
                ASSERT(MAX_CLIENTS == X.numChannels());
                ASSERT(btemt_PoolMsg::BTEMT_CHANNEL_LIMIT == poolEvent);

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
                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(400000);
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

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: DRQS 4340683
        //
        // Concerns:
        //   That CHANNEL_UP and CHANNEL_DOWN events are delivered with valid
        //   channel and server IDs for all connections supported by a single-
        //   threaded server when the listen channel is shutdown.
        //
        // Plan:
        //   Create a 'btemt_ChannelPool' object, 'mX', and open a channel in
        //   listen mode.  Open a connection to this "server", and verify the
        //   channel and server IDs in the bound channel state callback
        //   function.  Wait for some period, and then close the client
        //   channel via 'shutdown'.  Verify that a CHANNEL_DOWN event is
        //   processed for the open connection with valid channel and server ID
        //   parameters.
        //
        // Testing:
        //   Concern: DRQS 4340683
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting Concern: DRQS 4340683"
                 << "\n=============================" << endl;

        using namespace TEST_CASE_17_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
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

            bcemt_Barrier barrier(2);
            int           channelId = 0;

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case17ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &channelId
                                        , &barrier));

            btemt_ChannelPool::PoolStateChangeCallback    poolCb;
            makeNull(&ta, &poolCb);

            btemt_ChannelPool::DataReadCallback         dataCb;
            makeNull(&dataCb);

            btemt_ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setIncomingMessageSizes(1, 32, 256);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, cpc, &ta);
            ASSERT(0 == pool.start());
            ASSERT(0 == pool.listen(PORT, BACKLOG, SERVER_ID));
            const bteso_IPv4Address PEER = *pool.serverAddress(SERVER_ID);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();
            btesos_TcpChannel                      channel(socket);

            ASSERT(0 == socket->connect(PEER));
            ASSERT(0 == channel.isInvalid());

            barrier.wait();
            ASSERT(1 == pool.numChannels());
            ASSERT(0 != channelId);
            ASSERT(0 == pool.shutdown(channelId,
                                      btemt_ChannelPool::BTEMT_IMMEDIATE));
            barrier.wait();

            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == pool.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: SHUTDOWN INSIDE CHANNEL STATE CALLBACK
        //
        // Concerns:
        //   That 'shutdown' is correctly processed when called from inside
        //   the channel state callback function.
        //
        // Plan:
        //   Create a 'btemt_ChannelPool' object, 'mX', and open a channel in
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

        using namespace TEST_CASE_16_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
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

            btemt_ChannelPool::DataReadCallback  dataCb;
            btemt_ChannelPool::PoolStateChangeCallback       poolCb;

            btemt_ChannelPool *poolAddr = 0;
            bcemt_Barrier      barrier(2);
            int                channelId = 0;

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case16ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , (int)SERVER_ID
                                        , &poolAddr
                                        , &channelId
                                        , &barrier));

            makeNull(&ta, &dataCb);
            makeNull(&ta, &poolCb);

            btemt_ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setIncomingMessageSizes(1, 32, 256);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, cpc, &ta);
            poolAddr = &pool;
            ASSERT(0 == pool.start());
            ASSERT(0 == pool.listen(PORT, BACKLOG, SERVER_ID));
            const bteso_IPv4Address PEER = *pool.serverAddress(SERVER_ID);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();
            btesos_TcpChannel                      channel(socket);

            ASSERT(0 == socket->connect(PEER));
            ASSERT(0 == channel.isInvalid());

            barrier.wait();
            ASSERT(0 != pool.shutdown(channelId,
                                      btemt_ChannelPool::BTEMT_IMMEDIATE));

            channel.invalidate();
            factory.deallocate(socket);
            ASSERT(0 == pool.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 15: {
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
        //   void btemt_ChannelPool::getHandleStatistics*(...);
        // --------------------------------------------------------------------

        if (verbose)
             cout << "\nTesting getHandleStatistics facility"
                  << "\n====================================" << endl;

        using namespace TEST_CASE_15_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
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

            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(10);
            config.setReadTimeout(1);

            btemt_ChannelPool::PoolStateChangeCallback  poolCb;
            btemt_ChannelPool::DataReadCallback         dataCb;

            int acceptedChannelId1 = -1;
            int channelId1 = -1;
            int channelId2 = -1;

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case15ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &acceptedChannelId1
                                        , &channelId1
                                        , &channelId2
                                        , veryVerbose));

            makeNull(&poolCb);
            makeNull(&dataCb);

            bsl::vector<btemt_ChannelPool::HandleInfo> handles;
            btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);

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

            bdet_TimeInterval serverTime = bdetu_SystemTime::now();
            {
                ASSERT(0 == mX.listen(0, 1, SERVER_ID));

                handles.clear();
                mX.getHandleStatistics(&handles);

                ASSERT(1 == handles.size());
                if (1 <= handles.size()) {
                    ASSERT(btemt_ChannelType::BTEMT_LISTENING_CHANNEL
                                      == handles[0].d_channelType);
                    ASSERT(-1         == handles[0].d_channelId);
                    ASSERT(serverTime <= handles[0].d_creationTime);
                    ASSERT(SERVER_ID  == handles[0].d_userId);
                }
                if (veryVerbose) { P(handles); }

                // Remember server creation time (must not change from now on).

                serverTime = handles[0].d_creationTime;
            }

            bteso_IPv4Address address = *mX.serverAddress(SERVER_ID);

            if (verbose) cout << "\tVerifying that 'connect' is picked up.\n";

            bdet_TimeInterval connectTime = bdetu_SystemTime::now();
            bdet_TimeInterval acceptTime  = connectTime;
            {
                ASSERT(0 == mX.connect(address,
                                       NUM_CONNECT_ATTEMPTS,
                                       bdet_TimeInterval(TIMEOUT),
                                       CLIENT_ID1));

                // Wait until channel has been created.

                for (int i = 0; (-1 == acceptedChannelId1 || -1 == channelId1)
                                                   && i < SETUP_TIMEOUT; ++i) {
                    bcemt_ThreadUtil::sleep(bdet_TimeInterval(TIMEOUT));  // 1s
                    bcemt_ThreadUtil::yield();
                }

                handles.clear();
                mX.getHandleStatistics(&handles);

                ASSERT(3 == handles.size());
                bsl::sort(handles.begin(), handles.end(), LessThanByType());
                if (3 <= handles.size()) {
                    LOOP_ASSERT(handles[0].d_channelType,
                                btemt_ChannelType::BTEMT_LISTENING_CHANNEL
                                              == handles[0].d_channelType);
                    ASSERT(-1                 == handles[0].d_channelId);
                    ASSERT(serverTime         == handles[0].d_creationTime);
                    ASSERT(SERVER_ID          == handles[0].d_userId);

                    LOOP_ASSERT(handles[1].d_channelType,
                                btemt_ChannelType::BTEMT_ACCEPTED_CHANNEL
                                              == handles[1].d_channelType);
                    ASSERT(acceptedChannelId1 == handles[1].d_channelId);
                    ASSERT(acceptTime         <= handles[1].d_creationTime);
                    ASSERT(SERVER_ID          == handles[1].d_userId);

                    LOOP_ASSERT(handles[2].d_channelType,
                                btemt_ChannelType::BTEMT_CONNECTED_CHANNEL
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

            bdet_TimeInterval importTime;
            {
                bteso_SocketHandle::Handle socketHandles[2];
                ASSERT(0 == bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                                    socketHandles,
                                    bteso_SocketImpUtil::BTESO_SOCKET_STREAM));

                bteso_StreamSocket<bteso_IPv4Address> *socket =
                                            factory.allocate(socketHandles[0]);

                handles.clear();
                mX.getHandleStatistics(&handles);
                ASSERT(3 == handles.size());  // make sure

                importTime = bdetu_SystemTime::now();

                ASSERT(0 == mX.import(socket, &factory, CLIENT_ID2));

                // Wait until channel has been created.

                for (int i = 0; (-1 == channelId2) && i < SETUP_TIMEOUT; ++i) {
                    bcemt_ThreadUtil::sleep(bdet_TimeInterval(TIMEOUT));  // 1s
                    bcemt_ThreadUtil::yield();
                }

                handles.clear();
                mX.getHandleStatistics(&handles);

                ASSERT(4 == handles.size());
                bsl::sort(handles.begin(), handles.end(), LessThanByType());
                if (4 <= handles.size()) {
                    ASSERT(btemt_ChannelType::BTEMT_LISTENING_CHANNEL
                                              == handles[0].d_channelType);
                    ASSERT(-1                 == handles[0].d_channelId);
                    ASSERT(serverTime         == handles[0].d_creationTime);
                    ASSERT(SERVER_ID          == handles[0].d_userId);

                    ASSERT(btemt_ChannelType::BTEMT_ACCEPTED_CHANNEL
                                              == handles[1].d_channelType);
                    ASSERT(acceptedChannelId1 == handles[1].d_channelId);
                    ASSERT(acceptTime         == handles[1].d_creationTime);
                    ASSERT(SERVER_ID          == handles[1].d_userId);

                    ASSERT(btemt_ChannelType::BTEMT_CONNECTED_CHANNEL
                                              == handles[2].d_channelType);
                    ASSERT(channelId1         == handles[2].d_channelId);
                    ASSERT(connectTime        == handles[2].d_creationTime);
                    ASSERT(CLIENT_ID1         == handles[2].d_userId);

                    ASSERT(btemt_ChannelType::BTEMT_IMPORTED_CHANNEL
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
                    ASSERT(btemt_ChannelType::BTEMT_ACCEPTED_CHANNEL
                                              == handles[0].d_channelType);
                    ASSERT(acceptedChannelId1 == handles[0].d_channelId);
                    ASSERT(acceptTime         == handles[0].d_creationTime);
                    ASSERT(SERVER_ID          == handles[0].d_userId);

                    ASSERT(btemt_ChannelType::BTEMT_CONNECTED_CHANNEL
                                              == handles[1].d_channelType);
                    ASSERT(channelId1         == handles[1].d_channelId);
                    ASSERT(connectTime        == handles[1].d_creationTime);
                    ASSERT(CLIENT_ID1         == handles[1].d_userId);

                    ASSERT(btemt_ChannelType::BTEMT_IMPORTED_CHANNEL
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
                    bcemt_ThreadUtil::sleep(bdet_TimeInterval(TIMEOUT));  // 1s
                    bcemt_ThreadUtil::yield();
                }

                handles.clear();
                mX.getHandleStatistics(&handles);

                ASSERT(1 == handles.size());
                ASSERT(btemt_ChannelType::BTEMT_IMPORTED_CHANNEL
                                                  == handles[0].d_channelType);
                ASSERT(channelId2 == handles[0].d_channelId);
                ASSERT(importTime == handles[0].d_creationTime);
                ASSERT(CLIENT_ID2 == handles[0].d_userId);
                if (veryVerbose) { P(handles); }

                mX.shutdown(channelId2);

                // Wait until channels have been destroyed.

                for (int i = 0; (-1 != channelId2) && i < SETUP_TIMEOUT; ++i) {
                    bcemt_ThreadUtil::sleep(bdet_TimeInterval(TIMEOUT));  // 1s
                    bcemt_ThreadUtil::yield();
                }

                handles.clear();
                mX.getHandleStatistics(&handles);
                ASSERT(handles.empty());

                if (veryVerbose) { P(handles); }
            }
        }

      } break;
      case 14: {
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
        //   int btemt_ChannelPool::numBytes*(...);
        //   int btemt_ChannelPool::totalBytes*(...);
        //   int btemt_ChannelPool::getChannelStatistics*(...);
        // --------------------------------------------------------------------

        if (verbose)
             cout << "\nTesting {num,total}Bytes*() facility"
                  << "\n====================================" << endl;

        using namespace TEST_CASE_14_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_THREADS = 1,
                SERVER_ID   = 1013410001,
                BACKLOG     = 1
            };

            btemt_ChannelPool::PoolStateChangeCallback    poolCb;

            bcemt_Barrier channelBarrier(2);
            bces_AtomicInt fail = 0;

            btemt_ChannelPool  *poolAddr;
            int                 poolEvent = -1;
            int                *eventAddr = &poolEvent;
            int                 channelId;

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case14ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &eventAddr
                                        , &channelBarrier
                                        , &channelId));

            makeNull(&ta, &poolCb);

            btemt_ChannelPool::DataReadCallback         dataCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case14ChannelDataCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &fail));

            btemt_ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_THREADS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(100.0);
            cpc.setOutgoingMessageSizes(0, 1024, 4096);

            Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;
            poolAddr = &mX;

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == mX.start());
            ASSERT(0 == mX.listen(ADDRESS, BACKLOG, SERVER_ID));
            ASSERT(0 == X.numChannels());
            const bteso_IPv4Address PEER = *mX.serverAddress(SERVER_ID);

            typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> Factory;
            typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;
            typedef btesos_TcpChannel                                Channel;

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
                return -1;
            }

            ASSERT(0 == mX.setSocketOption(
                                        bteso_SocketOptUtil::BTESO_SENDBUFFER,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        512, channelId));

            char buffer[1024];
            bsls_PlatformUtil::Int64 clientBytesWritten = 0;
            while(!fail) {
                ASSERT(sizeof(buffer) == channel.write(0, buffer,
                                                       sizeof(buffer),0));
                clientBytesWritten += sizeof(buffer);
                if (veryVerbose) {
                    MTCOUT << "client wrote " << clientBytesWritten
                           << MTENDL;
                }
                #ifdef BSLS_PLATFORM__OS_AIX
                if (clientBytesWritten >= 1024) {
                    break; // AIX is brain-dead.
                }
                #endif
            }
            bcemt_ThreadUtil::microSleep(200 * 1000);
            LOOP_ASSERT(X.numChannels(), 1 == X.numChannels());

            bsls_PlatformUtil::Int64 clientBytesRead = 0;
            bsls_PlatformUtil::Int64 bytesWritten;
            bsls_PlatformUtil::Int64 bytesRequestedToBeWritten;
            bsls_PlatformUtil::Int64 bytesRead;
            do {
                LOOP_ASSERT(X.numChannels(), 1 == X.numChannels());

                #ifndef BSLS_PLATFORM__OS_AIX
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

                bsls_PlatformUtil::Int64 temp;
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

            bsls_PlatformUtil::Int64 temp, temp2, temp3;
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
                return -1;
            }

            ASSERT(0 == mX.setSocketOption(
                                        bteso_SocketOptUtil::BTESO_SENDBUFFER,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                #ifdef BSLS_PLATFORM__OS_AIX
                if (clientBytesWritten >= 1024) {
                    break; // AIX is brain-dead.
                }
                #endif
            }
            bcemt_ThreadUtil::microSleep(200 * 1000);
            ASSERT(1 == X.numChannels());

            bsls_PlatformUtil::Int64 oldBytesWritten = bytesWritten;
            bsls_PlatformUtil::Int64 oldBytesRequestedToBeWritten =
                                                    bytesRequestedToBeWritten;
            bsls_PlatformUtil::Int64 oldBytesRead = bytesRead;
            clientBytesRead = 0;
            bytesWritten = 0;
            bytesRequestedToBeWritten = 0;
            bytesRead = 0;
            do {
                ASSERT(1 == X.numChannels());

                #ifndef BSLS_PLATFORM__OS_AIX
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

                bsls_PlatformUtil::Int64 temp;
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
            bsls_PlatformUtil::Int64 old;

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
                return -1;
            }

            ASSERT(0 == mX.setSocketOption(
                                        bteso_SocketOptUtil::BTESO_SENDBUFFER,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
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
                #ifdef BSLS_PLATFORM__OS_AIX
                if (clientBytesWritten >= 1024) {
                    break; // AIX is brain-dead.
                }
                #endif
            }
            bcemt_ThreadUtil::microSleep(200 * 1000);
            ASSERT(1 == X.numChannels());

            clientBytesRead = 0;
            bytesWritten = 0;
            bytesRequestedToBeWritten = 0;
            bytesRead = 0;
            do {
                ASSERT(1 == X.numChannels());

                #ifndef BSLS_PLATFORM__OS_AIX
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

                bsls_PlatformUtil::Int64 temp;
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

      } break;
      case 13: {
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
        //   Instantiate a btemt_ChannelPool configured to accept NUM_THREADS
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
        //   double btemt_ChannelPool::reportWeightedAverageReset();
        // --------------------------------------------------------------------

        if (verbose)
             cout << "\nTesting reportWeightedAverageReset()"
                  << "\n====================================" << endl;

        using namespace TEST_CASE_13_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
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

            btemt_ChannelPool::DataReadCallback         dataCb;
            btemt_ChannelPool::PoolStateChangeCallback  poolCb;

            bcemt_Barrier channelBarrier(2);

            btemt_ChannelPool *poolAddr;
            int                poolEvent = -1;
            int               *eventAddr = &poolEvent;

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case13ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &eventAddr
                                        , &channelBarrier));

            makeNull(&poolCb);
            makeNull(&dataCb);

            btemt_ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_THREADS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(100.0);

            Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;
            poolAddr = &mX;

            const bteso_IPv4Address ADDRESS("127.0.0.1", 0);
            ASSERT(0 == mX.start());
            ASSERT(0 == mX.listen(ADDRESS, BACKLOG, SERVER_ID));
            ASSERT(0 == X.numChannels());
            const bteso_IPv4Address PEER = *mX.serverAddress(SERVER_ID);

            typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> Factory;
            typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;
            typedef btesos_TcpChannel                                Channel;

            Factory              factory(&ta);
            bsl::vector<Socket*> sockets(&ta);
            sockets.reserve(MAX_THREADS);

            ASSERT(0 == mX.reportWeightedAverageReset());
            for (int i = 0; i < MAX_THREADS; ++i) {
                Socket  *socket = factory.allocate();
                Channel  channel(socket, &ta);

                bcemt_ThreadUtil::yield();
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
                        cout << "*** Warning: " << argv[0] << ":" << L_
                             << ":\n*** Warning: anomalous timing results ***"
                             << endl;
                    }
                    LOOP2_ASSERT(i, exp, i > exp_really_lo &&
                                          i < exp_really_hi);
                }
                else if (exp == -1) {
                    cout << "*** Warning: " << argv[0] << ":" << L_
                         << ":\n*** Warning: less than 1ms between resets ***"
                         << endl;
                }

                sockets.push_back(socket);
                if (!i) {
                    // Give some time to the pool to setup its stuff.

                    bcemt_ThreadUtil::microSleep(200 * 1000);
                }

            }
            ASSERT(MAX_THREADS == mX.reportWeightedAverageReset());

            // If we get rescheduled, we will have inaccurate results.

            bcemt_ThreadUtil::yield();

            for (bsl::vector<Socket*>::iterator it = sockets.begin();
                    it != sockets.end(); ++it)
            {
                Socket  *socket = *it;
                Channel  channel(socket);

                bcemt_ThreadUtil::microSleep(2000 * 1000);
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
                cout << "*** Warning: " << argv[0] << ":" << L_ << ":\n"
                     << "*** Warning: anomalous timing results ***" << endl;
            }
#ifndef BSLS_PLATFORM__OS_AIX
            LOOP2_ASSERT(rr, exp, exp_really_lo < rr && rr < exp_really_hi);
#endif
            if (verbose) { P_(exp_lo); P_(exp); P_(exp_hi); P(rr); }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING FLOW CONTROL
        //
        // Concerns:
        //   o WRITE_CACHE_HIWAT and WRITE_CACHE_LOWAT are generated
        //     appropriately
        //
        // Plan:
        //   Create an instance under test.  Create a local socket pair and
        //   import it into the channel pool.  Block on a barrier waiting
        //   until another thread unblocks the main thread.  Then
        //   read the number of written bytes until EWOULDBLOCK is returned,
        //   then close the channels.  Verify that the watermark messages
        //   are generated.
        //
        // Testing:
        //   FLOW CONTROL
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING FLOW CONTROL."
                          << "=====================" << endl;

        using namespace TEST_CASE_12_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        {
            bsl::vector<my_ChannelEvent> channelEvents;
            bcemt_Mutex                  channelEventsMutex;

            enum {
                NUM_SOCKETS   = 1,
                SEND_SIZE     = 8192,
                LOW_WATERMARK = 1024,
                HI_WATERMARK  = 1 << 20 // 1 MB of cache
            };

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);

            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setReadTimeout(100000);
            config.setWriteCacheWatermarks(LOW_WATERMARK,
                                           HI_WATERMARK);
            if (verbose) { P(config); }

            btemt_ChannelPool::DataReadCallback         dataCb;
            btemt_ChannelPool::PoolStateChangeCallback  poolCb;

            bcemt_Barrier barrier(2);
            int           numBytesWritten = 0;

            // Do not overflow the stack.

            bcema_PooledBufferChainFactory outFactory(HI_WATERMARK / 64, &ta);

            btemt_ChannelPool *mX_p;

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case12FlowControlChannelCallback
                                        , _1, _2, _3, _4
                                        , &barrier
                                        , &numBytesWritten
                                        , config
                                        , &outFactory
                                        , &mX_p));

            makeNull(&poolCb);
            makeNull(&dataCb);

            btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
            mX_p = &mX;

            ASSERT(0 == mX.start());

            bteso_SocketHandle::Handle handles[2];
            int ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                                     handles,
                                     bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
            ASSERT(0 == ret);

            // The following socket options are set only if necessary.
            ret = bteso_SocketOptUtil::setOption(handles[1],
                                     bteso_SocketOptUtil::BTESO_TCPLEVEL,
                                     bteso_SocketOptUtil::BTESO_TCPNODELAY, 1);
            ASSERT(0 == ret);

            bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                                                  factory.allocate(handles[0]);
            ASSERT(serverSocket);
            bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                                                  factory.allocate(handles[1]);
            ASSERT(clientSocket);

            ASSERT(0 == mX.import(serverSocket, &factory, 0));

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
            bcemt_ThreadUtil::microSleep(0, 2);

            mX.stop();

            factory.deallocate(clientSocket);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 11: {
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
        //   int btemt_ChannelPool::disableRead(int);
        //   int btemt_ChannelPool::enableRead(int);
        // --------------------------------------------------------------------

        if (verbose)
           cout << "\nTESTING 'disableRead' and 'enableRead' METHODS."
                << "\n===============================================" << endl;

        using namespace TEST_CASE_11_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        {
            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);
            runTestCase11(&factory, ta); // Note: ta passed by reference
        }

        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 10: {
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
        //   int btemt_ChannelPool::registerClock(cmd, start, period, int);
        //   void btemt_ChannelPool::deregisterClock(int);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING 'registerClock' AND 'deregisterClock'\n";

        using namespace TEST_CASE_10_NAMESPACE;
        if (verbose) cout << "\nWhen channel pool is running.\n";
        {
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(10);

            btemt_ChannelPool::ChannelStateChangeCallback channelCb;
            btemt_ChannelPool::PoolStateChangeCallback    poolCb;
            btemt_ChannelPool::DataReadCallback           dataCb;

            makeNull(&channelCb);
            makeNull(&poolCb);
            makeNull(&dataCb);

            if (verbose)
                cout << "\tRegistering clocks, then deregistering.\n";

            bcema_TestAllocator ta(veryVeryVerbose);
            {
                btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                ASSERT(0 == mX.start());
                bdet_TimeInterval startTime = bdetu_SystemTime::now() + 120.0;

                enum {
                    NUM_CLOCKS = 20
                };

                if (veryVerbose)
                    cout << "\t\tWith up to " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    for (int j = 0; j <= i; ++j) {
                        LOOP2_ASSERT(i, j,
                            0 == mX.registerClock(NULL_CB, startTime,
                                                  bdet_TimeInterval(1.0),
                                                  j));
                        LOOP2_ASSERT(i, j,
                            1 == mX.registerClock(NULL_CB, startTime,
                                                  bdet_TimeInterval(1.0),
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
                btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                ASSERT(0 == mX.start());
                bdet_TimeInterval startTime = bdetu_SystemTime::now();
                bdet_TimeInterval noPeriod(0.0);  // 0s, for timers
                bdet_TimeInterval period(1.0);    // 1s, for clocks
                const int mT = 10 * 1000;         // 10ms

                enum {
                    NUM_CLOCKS      = 10,
                    NUM_INVOCATIONS = 10,
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
                    bdef_Function<void (*)()> functor(
                            bdef_BindUtil::bindA( &ta
                                                , &case10MyClockCallback
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

                bcemt_ThreadUtil::sleep(bdet_TimeInterval(
                                                 NUM_INVOCATIONS * 1.0) + 0.5);
                int iter = 10;
                while (--iter && !(NUM_INVOCATIONS <=
                                clockState[LAST_CLOCK_IDX].d_numInvocations)) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  delays in case 10 (a)" << endl;
                    bcemt_ThreadUtil::microSleep(mT);
                    bcemt_ThreadUtil::yield();
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
                    TIMEOUT              = 1,   // 1s
                    NUM_CLOCKS           = 10,
                    NUM_INVOCATIONS      = 10,
                    SETUP_TIMEOUT        = 20,   // 20s
                    SERVER_ID            = 0x701d50da
                };

                bdet_TimeInterval period(1.0);    // 1s, for clocks
                const int mT = 10 * 1000;         // 10ms

                my_ClockState clockState[NUM_CLOCKS];

                // One server -- system specified port number.

                if (veryVerbose)
                    cout << "\t\tRegistering server channel.\n";

                btemt_ChannelPool serverPool(channelCb, dataCb, poolCb,
                                             config, &ta);
                ASSERT(0 == serverPool.start());
                ASSERT(0 == serverPool.listen(0, 1, SERVER_ID));

                const bteso_IPv4Address address = *serverPool.serverAddress(
                                                                    SERVER_ID);
                ASSERT(0 != address.portNumber());
                if (veryVerbose) { P(address); }

                // Connect to that server.

                if (veryVerbose)
                    cout << "\t\tRegistering connected channels.\n";

                int channelId1 = -1, channelId2 = -1;
                ThreadId threadId1 = NULL_THREAD_ID;
                ThreadId threadId2 = NULL_THREAD_ID;

                btemt_ChannelPool::ChannelStateChangeCallback channel2Cb(
                        bdef_BindUtil::bindA( &ta
                                            , &case10ChannelStateCallback
                                            , _1, _2, _3, _4
                                            , &channelId1
                                            , &threadId1
                                            , &channelId2
                                            , &threadId2
                                            , veryVerbose));

                btemt_ChannelPool mX(channel2Cb, dataCb, poolCb, config, &ta);
                ASSERT(0 == mX.start());

                ASSERT(0 == mX.connect(address,
                                       NUM_CONNECT_ATTEMPTS,
                                       bdet_TimeInterval(TIMEOUT),
                                       1));

                // Wait until first channel has been created.

                for (int i = 0; -1 == channelId1 && i < (int) SETUP_TIMEOUT;
                                                                         ++i) {
                    bcemt_ThreadUtil::sleep(bdet_TimeInterval(TIMEOUT));  // 1s
                    bcemt_ThreadUtil::yield();
                }

                if (-1 == channelId1) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  delays in case 10 (a) "
                         << "may prevent running test." << endl;
                }

                ASSERT(0 == mX.connect(address,
                                       NUM_CONNECT_ATTEMPTS,
                                       bdet_TimeInterval(TIMEOUT),
                                       2));

                // Wait until both channels have been created.

                for (int i = 0; (-1 == channelId1 || -1 == channelId2) &&
                                                i < (int) SETUP_TIMEOUT; ++i) {
                    bcemt_ThreadUtil::sleep(bdet_TimeInterval(TIMEOUT));  // 1s
                    bcemt_ThreadUtil::yield();
                }

                if (-1 == channelId2 || -1 == channelId2) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  delays in case 10 (a) "
                         << "prevent running test." << endl;
                }

                // We have two channels hopefully running in different
                // managers.

                if (threadId1 == threadId2) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  same managers means that "
                         << "test won't be very useful." << endl;
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

                bdet_TimeInterval startTime = bdetu_SystemTime::now();

                for (int i = 0; i < (int) NUM_CLOCKS; ++i) {
                    clockState[i].d_startTime = startTime + period;
                    clockState[i].d_timeout   = period;
                    clockState[i].d_numInvocations = 0;
                    clockState[i].d_maxNumInvocations = 0;  // do not check
                    bdef_Function<void (*)()> functor(
                            bdef_BindUtil::bindA( &ta
                                                , &case10MyClockCallback
                                                , &clockState[i]
                                                , i
                                                , &mX
                                                , 0
                                                , i % 2 ? threadId1
                                                        : threadId2));

                    LOOP_ASSERT(i, 0 == mX.registerClock(functor,
                                                  clockState[i].d_startTime,
                                                  clockState[i].d_timeout, i,
                                                  i % 2 ? channelId1
                                                        : channelId2));
                }
                if (veryVerbose)
                    cout << "\t\tWaiting for each clock to be invoked "
                         << NUM_INVOCATIONS << " times.\n";

                bcemt_ThreadUtil::sleep(bdet_TimeInterval(
                                                 NUM_INVOCATIONS * 1.0) + 0.5);
                int iter = 10;
                while (--iter && !((int) NUM_INVOCATIONS <=
                                clockState[NUM_CLOCKS - 1].d_numInvocations)) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  delays in case 10 (a)" << endl;
                    bcemt_ThreadUtil::microSleep(mT);
                    bcemt_ThreadUtil::yield();
                }

                if (veryVerbose)
                   cout << "\t\tDeRegistering " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < (int) NUM_CLOCKS; ++i) {
                    LOOP2_ASSERT(i, clockState[i].d_numInvocations,
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
                btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                ASSERT(0 == mX.start());
                bdet_TimeInterval startTime = bdetu_SystemTime::now();
                bdet_TimeInterval period(1.0);  // 1s
                const int mT = 10 * 1000;       // 10ms
                enum {
                    NUM_CLOCKS = 10,
                    NUM_INVOCATIONS = 10
                };

                my_ClockState clockState[NUM_CLOCKS];

                if (veryVerbose)
                   cout << "\t\tRegistering " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    clockState[i].d_startTime = startTime + period;
                    clockState[i].d_timeout = period;
                    clockState[i].d_numInvocations = 0;
                    clockState[i].d_maxNumInvocations = NUM_INVOCATIONS;
                    bdef_Function<void (*)()> functor(
                            bdef_BindUtil::bindA( &ta
                                                , &case10MyClockCallback
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

                bcemt_ThreadUtil::sleep(bdet_TimeInterval(
                                                 NUM_INVOCATIONS * 1.0) + 0.5);
                int iter = 10;
                while (--iter && !(NUM_INVOCATIONS <=
                                clockState[NUM_CLOCKS - 1].d_numInvocations)) {
                    cout << __FILE__ << ": " << __LINE__
                         << ": ***WARNING***  delays in case 10 (a)" << endl;
                    bcemt_ThreadUtil::microSleep(mT);
                    bcemt_ThreadUtil::yield();
                }

                if (veryVerbose)
                    cout << "\t\tReRegistering " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    LOOP2_ASSERT(i, clockState[i].d_numInvocations,
                            NUM_INVOCATIONS == clockState[i].d_numInvocations);
                    LOOP_ASSERT(i, 0 == mX.registerClock(NULL_CB,
                          bdetu_SystemTime::now() + 1.0,
                          bdet_TimeInterval(1.0), i));
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
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(0);

            btemt_ChannelPool::ChannelStateChangeCallback channelCb;
            btemt_ChannelPool::PoolStateChangeCallback    poolCb;
            btemt_ChannelPool::DataReadCallback           dataCb;

            makeNull(&channelCb);
            makeNull(&poolCb);
            makeNull(&dataCb);

            if (verbose) cout << "\tRegistering clocks, then deregistering\n";

            bcema_TestAllocator ta(veryVeryVerbose);
            {
                btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                bdet_TimeInterval startTime = bdetu_SystemTime::now() + 120.0;
                ASSERT(0 == mX.stop());

                enum {
                    NUM_CLOCKS = 20
                };

                if (veryVerbose)
                   cout << "\t\tWith up to " << NUM_CLOCKS << " clocks.\n";

                for (int i = 0; i < NUM_CLOCKS; ++i) {
                    for (int j = 0; j <= i; ++j) {
                        LOOP2_ASSERT(i, j,
                                0 == mX.registerClock(NULL_CB, startTime,
                                    bdet_TimeInterval(1.0),
                                    j));
                        LOOP2_ASSERT(i, j,
                                1 == mX.registerClock(NULL_CB, startTime,
                                    bdet_TimeInterval(1.0),
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
                btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                bdet_TimeInterval startTime = bdetu_SystemTime::now() + 10.0;

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
                                                        bdet_TimeInterval(1.0),
                                                        j));
                        LOOP2_ASSERT(i, j, 1 == mX.registerClock(
                                                        NULL_CB,
                                                        startTime,
                                                        bdet_TimeInterval(1.0),
                                                        j));
                    }

                    if (veryVerbose)
                        cout << "\t\tStopping channel pool" << endl;

                    ASSERT(0 == mX.stop());

                    if (veryVerbose)
                        cout << "\t\tSleeping..." << endl;

                    bcemt_ThreadUtil::microSleep(0, 7);

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

      } break;
      case 9: {
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
        //   int btemt_ChannelPool::write(btemt_DataMsg, ...);
        //   int btemt_ChannelPool::write(btemt_BlobMsg, ...);
        //   int btemt_ChannelPool::write(btes_Iovecs, ...);
        //   int btemt_ChannelPool::write(btes_Ovecs, ...);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'write'"
                          << "\n===============" << endl;

        using namespace TEST_CASE_9_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);

        {
            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            runTestCase9(argv[0], &factory, ta);
                // Note: ta passed by reference
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 8: {
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
        //   int btemt_ChannelPool::shutdown();
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting 'shutdown'"
                 << "\n==================" << endl;

        using namespace TEST_CASE_8_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
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

            bteso_InetStreamSocketFactory<bteso_IPv4Address>  factory;
            btemt_ChannelPool::ChannelStateChangeCallback     serverChannelCb;
            btemt_ChannelPool::PoolStateChangeCallback        poolCb;
            btemt_ChannelPool::DataReadCallback               dataCb;

            bcemt_Barrier barrier(2);

            case8CallbackInfo info;
            info.d_barrier_p = &barrier;
            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;

            makeNull(&poolCb);
            makeNull(&serverChannelCb);
            makeNull(&dataCb);

            btemt_ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setReadTimeout(1);    // in seconds
            cpc.setIncomingMessageSizes(1, 32, 256);

            btemt_ChannelPool serverPool(serverChannelCb, dataCb, poolCb,
                                         cpc, &ta);
            ASSERT(0 == serverPool.start());
            ASSERT(0 == serverPool.listen(0, BACKLOG, SERVER_ID));
            bteso_IPv4Address peer = *serverPool.serverAddress(SERVER_ID);
            if (verbose) {
                T_(); P(peer);
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case8ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &info));

            btemt_ChannelPool mX(channelCb, dataCb, poolCb, cpc, &ta);
            ASSERT(0 == mX.start());

            if (verbose)
                cout << "\tSimple manual shutdown." << endl;
            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.connect(peer, 1,
                                    bdet_TimeInterval(TIMEOUT),
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
            bcemt_ThreadUtil::microSleep(0, 1);

            if (verbose)
                cout << "\tPartial shutdown is same as full shutdown." << endl;
            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.connect(peer, 1,
                                    bdet_TimeInterval(TIMEOUT),
                                    USER_ID,
                                    false));
                barrier.wait(); // for CHANNEL_UP
                ASSERT(-1 != info.d_channelId);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        bteso_Flag::BTESO_SHUTDOWN_RECEIVE));
                barrier.wait(); // for CHANNEL_DOWN

                MTASSERT(0 == info.d_channelDownReadFlag);
                MTASSERT(0 == info.d_channelDownWriteFlag);
            }
            bcemt_ThreadUtil::microSleep(0, 1);

            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.connect(peer, 1,
                                    bdet_TimeInterval(TIMEOUT),
                                    USER_ID,
                                    false));
                barrier.wait(); // for CHANNEL_UP
                ASSERT(-1 != info.d_channelId);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        bteso_Flag::BTESO_SHUTDOWN_SEND));
                barrier.wait(); // for CHANNEL_DOWN

                MTASSERT(0 == info.d_channelDownReadFlag);
                MTASSERT(0 == info.d_channelDownWriteFlag);
            }
            bcemt_ThreadUtil::microSleep(0, 1);

            if (verbose)
                cout << "\tHalf-open shutdown (close read)." << endl;
            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.connect(peer, 1,
                                    bdet_TimeInterval(TIMEOUT),
                                    USER_ID,
                                    false,
                                    btemt_ChannelPool::BTEMT_KEEP_HALF_OPEN));
                barrier.wait(); // for CHANNEL_UP
                MTASSERT(-1 != info.d_channelId);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        bteso_Flag::BTESO_SHUTDOWN_RECEIVE));
                barrier.wait(); // for CHANNEL_DOWN_READ
                MTASSERT(1 == info.d_channelDownReadFlag);

                ASSERT(0 != mX.shutdown(info.d_channelId,
                                        bteso_Flag::BTESO_SHUTDOWN_RECEIVE));
                bcemt_ThreadUtil::sleep(bdet_TimeInterval(1));  // 1s
                MTASSERT(1 == info.d_channelDownReadFlag);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        bteso_Flag::BTESO_SHUTDOWN_SEND));
                barrier.wait(); // for CHANNEL_DOWN_WRITE
                MTASSERT(1 == info.d_channelDownWriteFlag);

                // The SHUTDOWN_SEND will also close the channel.
                barrier.wait(); // for CHANNEL_DOWN
                MTASSERT(1 == info.d_channelDownReadFlag);
                MTASSERT(1 == info.d_channelDownWriteFlag);
            }
            bcemt_ThreadUtil::microSleep(0, 1);

            if (verbose)
                cout << "\tHalf-open shutdown (close write)." << endl;
            info.d_channelId = -1;
            info.d_channelDownReadFlag = 0;
            info.d_channelDownWriteFlag = 0;
            {
                ASSERT(0 == mX.connect(peer, 1,
                                    bdet_TimeInterval(TIMEOUT),
                                    USER_ID,
                                    false,
                                    btemt_ChannelPool::BTEMT_KEEP_HALF_OPEN));
                barrier.wait(); // for CHANNEL_UP
                MTASSERT(-1 != info.d_channelId);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        bteso_Flag::BTESO_SHUTDOWN_SEND));
                barrier.wait(); // for CHANNEL_DOWN_WRITE
                MTASSERT(1 == info.d_channelDownWriteFlag);

                ASSERT(0 != mX.shutdown(info.d_channelId,
                                        bteso_Flag::BTESO_SHUTDOWN_SEND));
                bcemt_ThreadUtil::sleep(bdet_TimeInterval(1));  // 1s
                MTASSERT(1 == info.d_channelDownWriteFlag);

                ASSERT(0 == mX.shutdown(info.d_channelId,
                                        bteso_Flag::BTESO_SHUTDOWN_RECEIVE));
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

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: IMPORT HALF-CLOSED SOCKET PAIR
        //
        // Concerns:
        //   That import a half-closed socket pair can be imported and that the
        //   channel is properly recognized as half-closed.
        //
        // Plan:
        //   Create a 'btemt_ChannelPool' object, 'mX', and open a channel in
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

        using namespace TEST_CASE_7_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        bcema_TestAllocator ta2(veryVeryVerbose);
        {
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Initialization

            enum {
                MAX_CLIENTS  = 1,
                MAX_THREADS  = 1,
                SERVER_ID    = 0x701d50da,
                BACKLOG      = 1
            };

            bteso_InetStreamSocketFactory<bteso_IPv4Address>  factory(&ta2);
            bteso_StreamSocket<bteso_IPv4Address>            *socketA;
            bteso_StreamSocket<bteso_IPv4Address>            *socketB;

            btemt_ChannelPool::PoolStateChangeCallback    poolCb;
            btemt_ChannelPool::DataReadCallback           dataCb;

            bcemt_Barrier barrier(2);

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &case7ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &barrier));

            makeNull(&poolCb);
            makeNull(&dataCb);

            btemt_ChannelPoolConfiguration cpc;
            cpc.setMaxConnections(MAX_CLIENTS);
            cpc.setMaxThreads(MAX_THREADS);
            cpc.setMetricsInterval(10.0);
            cpc.setIncomingMessageSizes(1, 32, 256);

            btemt_ChannelPool pool(channelCb, dataCb, poolCb, cpc, &ta);
            ASSERT(0 == pool.start());
            ASSERT(0 == pool.listen(0, BACKLOG, SERVER_ID));

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Test Execution

            enum { NUM_ITERATIONS = 20 };
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                bteso_SocketHandle::Handle handles[2];
                LOOP_ASSERT(i, 0 ==
                    bteso_SocketImpUtil::socketPair<bteso_IPv4Address>
                          (handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM));

                socketA = factory.allocate(handles[0]);
                socketB = factory.allocate(handles[1]);
                LOOP_ASSERT(i,
                      0 == socketB->shutdown(bteso_Flag::BTESO_SHUTDOWN_BOTH));
                LOOP_ASSERT(i, 0 == pool.numChannels());

                pool.import(socketA, &factory, SERVER_ID);
                barrier.wait();

                factory.deallocate(socketB);
            }
            ASSERT(0 == pool.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 6: {
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
        //   channels.  Verify that the watermark messages are generated and
        //   check on the distribution of the load among the various event
        //   managers.
        //
        // Testing:
        //   int btemt_ChannelPool::import(...);
        // --------------------------------------------------------------------

        if (verbose)
             cout << "\nTESTING 'import' METHOD."
                  << "\n========================" << endl;

        using namespace TEST_CASE_6_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        {
            bsl::vector<my_ChannelEvent> channelEvents;
            bcemt_Mutex                  channelEventsMutex;
            bsl::vector<my_PoolEvent>    poolEvents;
            bcemt_Mutex                  poolEventsMutex;

            const int NUM_SOCKETS = 20;
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(4);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(NUM_SOCKETS);
            config.setIncomingMessageSizes(1, 1, 1);
            config.setReadTimeout(1000);

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( &ta
                                        , &recordChannelState
                                        , _1, _2, _3, _4
                                        , &channelEvents
                                        , &channelEventsMutex));

            btemt_ChannelPool::PoolStateChangeCallback    poolCb(
                    bdef_BindUtil::bindA( &ta
                                        , &recordPoolState
                                        , _1, _2, _3
                                        , &poolEvents
                                        , &poolEventsMutex));

            btemt_ChannelPool::DataReadCallback         dataCb;
            makeNull(&dataCb);

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
            const btemt_ChannelPool& X = mX;
            if (verbose) {
                P(config);
            }

            ASSERT(0 == mX.start());

            bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>  socketVecA;
            bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *>  socketVecB;
            bteso_StreamSocket<bteso_IPv4Address>                *socketA;
            bteso_StreamSocket<bteso_IPv4Address>                *socketB;

            for (int i = 0; i < NUM_SOCKETS; ++i) {
                // Create sockets in pairs, otherwise Linux might fail its
                // autoread attempts and close the imported channel right-away.

                bteso_SocketHandle::Handle handles[2];
                LOOP_ASSERT(i, 0 ==
                    bteso_SocketImpUtil::socketPair<bteso_IPv4Address>
                          (handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM));

                socketA = factory.allocate(handles[0]);
                socketB = factory.allocate(handles[1]);
                int retCode = mX.import(socketA, &factory, i);
                LOOP2_ASSERT(i, retCode, 0 == retCode);
                socketVecA.push_back(socketA);
                socketVecB.push_back(socketB);
            }

            bcemt_ThreadUtil::microSleep(0, 10); // 10 secs should be enough

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

                bteso_StreamSocket<bteso_IPv4Address> *socket
                                                          = factory.allocate();
                ASSERT(-1 == mX.import(socket, &factory, NUM_SOCKETS));
                factory.deallocate(socket);
            }

            int importedFlag[NUM_SOCKETS];
            bsl::fill_n(importedFlag, NUM_SOCKETS, 0);

            for (int i = 0; i < numEvents; ++i) {
                LOOP_ASSERT(i, btemt_ChannelMsg::BTEMT_CHANNEL_UP ==
                                             channelEvents[i].d_data.event() ||
                               btemt_ChannelMsg::BTEMT_AUTO_READ_ENABLED ==
                                              channelEvents[i].d_data.event());
                const int sourceId = channelEvents[i].d_data.allocatorId();
                LOOP_ASSERT(i, 0 <= sourceId);
                LOOP_ASSERT(i, sourceId < NUM_SOCKETS);
                if (btemt_ChannelMsg::BTEMT_CHANNEL_UP ==
                                             channelEvents[i].d_data.event()) {
                    const int channelId = channelEvents[i].d_data.channelId();
                    LOOP_ASSERT(i, socketVecA[sourceId] ==
                                              X.streamSocket(channelId).ptr());
                }
                if (veryVerbose) { P(channelEvents[i].d_data.allocatorId()); }

                if (channelEvents[i].d_data.event() ==
                                     btemt_ChannelMsg::BTEMT_AUTO_READ_ENABLED)
                {
                    LOOP_ASSERT(i, 1 ==
                          importedFlag[channelEvents[i].d_data.allocatorId()]);
                    importedFlag[channelEvents[i].d_data.allocatorId()] = 3;
                }
                else {
                    LOOP_ASSERT(i, 0 ==
                          importedFlag[channelEvents[i].d_data.allocatorId()]);
                    importedFlag[channelEvents[i].d_data.allocatorId()] = 1;
                }
            }

            for (int i = 0; i < NUM_SOCKETS; ++i) {
                LOOP2_ASSERT(i, importedFlag[i], 3 == importedFlag[i]);
            }

            bsl::fill_n(importedFlag, NUM_SOCKETS, 0);
            bsl::vector<my_ChannelEvent> backup;

            for (int i = 0; i < (int)channelEvents.size(); ++i) {
                if (btemt_ChannelMsg::BTEMT_AUTO_READ_ENABLED !=
                                               channelEvents[i].d_data.event())
                {
                    backup.push_back(channelEvents[i]);
                }
            }

            channelEvents.clear();

            for (int i = 0; i < (int)backup.size(); ++i) {
                if (veryVerbose) {
                    P(backup[i].d_data.channelId());
                }
                mX.shutdown(backup[i].d_data.channelId(),
                            btemt_ChannelPool::BTEMT_IMMEDIATE);
            }
            bcemt_ThreadUtil::microSleep(0, 2);

            numEvents = channelEvents.size();
            if (veryVerbose) { P(numEvents); PV(channelEvents); }

            bsl::fill_n(importedFlag, NUM_SOCKETS, 0);
            for (int i = 0; i < numEvents; ++i) {
                LOOP_ASSERT(i, btemt_ChannelMsg::BTEMT_CHANNEL_DOWN ==
                                              channelEvents[i].d_data.event());
                LOOP_ASSERT(i, NUM_SOCKETS >
                                        channelEvents[i].d_data.allocatorId());
                LOOP_ASSERT(i, 0 <= channelEvents[i].d_data.allocatorId());
                importedFlag[channelEvents[i].d_data.allocatorId()] = 1;
            }

            for (int i = 0; i < NUM_SOCKETS; ++i) {
                LOOP_ASSERT(i, 1 == importedFlag[i]);
            }

            for (int i = 0; i < NUM_SOCKETS; ++i) {
                factory.deallocate(socketVecB[i]);
            }
            bcemt_ThreadUtil::microSleep(0, 2);
            mX.stop();
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 5: {
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
        //   int listen(const bteso_IPv4Address& address, int backlog, int id);
        //   const bteso_IPv4Address *serverAddress(int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'listen' METHOD" << endl
                          << "=======================" << endl;

        using namespace TEST_CASE_5_NAMESPACE;
        btemt_ChannelPoolConfiguration config;
        config.setMaxThreads(1);
        config.setMetricsInterval(10.0);
        config.setMaxConnections(0);

        btemt_ChannelPool::ChannelStateChangeCallback channelCb;
        btemt_ChannelPool::PoolStateChangeCallback    poolCb;
        btemt_ChannelPool::DataReadCallback           dataCb;

        makeNull(&channelCb);
        makeNull(&poolCb);
        makeNull(&dataCb);

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

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            // One server -- system specified port numbers.

            btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
            const btemt_ChannelPool& X = mX;
            ASSERT(0 == mX.stop());
            int retCode = mX.listen(0, 1, 0);
            LOOP_ASSERT(retCode, 0 == retCode);

            const bteso_IPv4Address *address = X.serverAddress(0);
            LOOP_ASSERT(address, address);
            ASSERT(0 != address->portNumber());
            if (veryVerbose) { P(*address); }

            retCode = mX.listen(address->portNumber(), 1, 1);
            LOOP_ASSERT(retCode, BIND_FAILED == retCode);
            retCode = mX.listen(address->portNumber(), 1, 0);
            LOOP_ASSERT(retCode, DUPLICATE_ID == retCode);
            retCode = mX.close(0);
            LOOP_ASSERT(retCode, 0 == retCode);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

        {
            // Two servers -- system specified port numbers.

            btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
            const btemt_ChannelPool& X = mX;
            ASSERT(0 == mX.stop());
            int retCode = mX.listen(0, 1, 0);
            LOOP_ASSERT(retCode, 0 == retCode);
            const bteso_IPv4Address *address = X.serverAddress(0);
            LOOP_ASSERT(address, address);
            ASSERT(0 != address->portNumber());
            if (veryVerbose) { P(*address); }

            retCode = mX.listen(0, 1, 1);
            LOOP_ASSERT(retCode, 0 == retCode);
            const bteso_IPv4Address *address1 = X.serverAddress(1);
            ASSERT(address->portNumber() != address1->portNumber());
            if (veryVerbose) { P(*address); P(*address1); }

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

            btemt_ChannelPool mX(channelCb, dataCb, poolCb, config);
            const btemt_ChannelPool& X = mX;
            ASSERT(0 == mX.stop());
            int retCode = mX.listen(0, 1, 0);
            LOOP_ASSERT(retCode, 0 == retCode);
            const bteso_IPv4Address *address = X.serverAddress(0);
            LOOP_ASSERT(address, address);
            ASSERT(0 != address->portNumber());
            if (veryVerbose) {
                P(*address);
            }
            retCode = mX.listen(address->portNumber(), 1, 1);
            LOOP_ASSERT(retCode, BIND_FAILED == retCode);
            retCode = mX.listen(address->portNumber(), 1, 0);
            LOOP_ASSERT(retCode, DUPLICATE_ID == retCode);
            retCode = mX.listen(0, 1, 0);  // Valid address - duplicate ID.
            LOOP_ASSERT(retCode, 1 == retCode);
            retCode = mX.close(0);
            LOOP_ASSERT(retCode, 0 == retCode);
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        if (veryVerbose) { P(ta); }

      } break;
      case 4: {
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
        //   We use 'btesos_tcptimedacceptor' in a separate thread as test
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
        //   btemt_ChannelPool::connect(...);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'connect' METHOD"
                 << "\n========================" << endl;

        using namespace TEST_CASE_4_NAMESPACE;
        bcema_TestAllocator ta(veryVeryVerbose);
        {
            enum {
                NUM_ATTEMPTS = 5,
                PORT_BASE    = 16253,
                PORT_RANGE   = 1000,
                USER_ID      = 0x12345678,
                QUEUE_SIZE   = 5,
                TIMEOUT      = 1,     // in seconds
                DELTA        = 450000 // main thread delay, in microseconds
            };

            if (veryVerbose) {
                MTCOUT << "Creating server thread." << MTENDL;
            }
            const int PORT = PORT_BASE
                           + bdeu_HashUtil::hash0(argv[0], PORT_RANGE);
            if (verbose) { P(PORT); }

            bteso_IPv4Address peer("127.0.0.1", PORT);

            case4WorkerInfo info;
            bcemt_ThreadUtil::Handle worker;
            info.d_i           = 0;
            info.d_portNumber  = PORT;
            info.d_queueSize   = QUEUE_SIZE;
            info.d_timeOut     = TIMEOUT;
            info.d_worker_p    = &worker;
            ASSERT(0 == bcemt_ThreadUtil::create(&worker,
                                                 &case4OpenConnectThread,
                                                 (void *)&info));

            if (veryVerbose) {
                MTCOUT << "Creating channel pool." << MTENDL;
            }
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMetricsInterval(30.0);
            config.setMaxConnections(1);
            config.setReadTimeout(600);    // in seconds

            int channelId = -1;
            bces_AtomicInt numFailures = 0;
            bces_AtomicInt isConnected = 0;

            btemt_ChannelPool *pMX;

            btemt_ChannelPool::PoolStateChangeCallback    poolCb(
                    bdef_BindUtil::bind( &case4PoolStateCb
                                       , _1, _2, _3
                                       , &numFailures
                                       , &info));

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bind( &case4ChannelCb
                                       , _1, _2, _3, _4
                                       , &channelId
                                       , &isConnected
                                       , &numFailures
                                       , &info
                                       , &pMX));

            btemt_ChannelPool::DataReadCallback         dataCb;
            makeNull(&dataCb);

            btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
            pMX = &mX;
            ASSERT(0 == mX.start());

            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                if (verbose) {
                    MTCOUT << "\tStarting test iteration "
                           << i << " (with auto-read)." << MTENDL;
                }

                isConnected           = 0;
                numFailures           = 0;
                info.d_expUserId      = USER_ID + i + 1;
                info.d_expNumFailures = i;
                info.d_i              = i;
                info.d_worker_p       = &worker;
                if (0 != i) {
                    // Initial opening server may have failed, do not overwrite
                    // info in that case.  Otherwise, it may have failed at
                    // previous iteration, so we need to reset port number.
                    // Note that port number will be reset to 0 if the callback
                    // could not open the server in time for the last
                    // connection attempt.

                    info.d_portNumber = PORT;
                }

                bcemt_ThreadUtil::microSleep(0, 1);
                if (veryVerbose) {
                    MTCOUT << "--->ATTEMPT " << i << ' '
                           << bdetu_SystemTime::now()
                           << " ---------------------------------------"
                           << MTENDL;
                }
                LOOP_ASSERT(i, 0 == mX.connect(peer,
                                               info.d_expNumFailures + 1,
                                               bdet_TimeInterval(TIMEOUT),
                                               info.d_expUserId));

                bcemt_ThreadUtil::microSleep(0, (i + 2) * TIMEOUT);
                if (0 == info.d_portNumber)
                {
                    LOOP2_ASSERT(numFailures, info.d_expNumFailures,
                                 numFailures == info.d_expNumFailures);
                    LOOP2_ASSERT(i, isConnected, 3 == isConnected);
                    // autoRead & channelUp
                }
                else {
                    // If server was not opened by this test driver, it might
                    // have been opened already: there is no telling if this
                    // connection should have failed or succeeded.

                    MTCOUT << "WARNING: Did not open server: ";
                    P(info.d_portNumber);
                    P_(isConnected);
                    P_(numFailures); P(info.d_expNumFailures);
                    cout << MTENDL;
                }
                bcemt_ThreadUtil::join(worker);
                bcemt_ThreadUtil::microSleep(0, 2 * TIMEOUT);
                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(0, 2 * TIMEOUT);
            }
            ASSERT(0 == mX.stop());
        }
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        if (verbose)
            cout << "TESTING 'connect' BY NAME\n";
        {
            enum {
                NUM_ATTEMPTS = 5,
                PORT_BASE    = 16253,
                PORT_RANGE   = 1000,
                USER_ID      = 0x12345678,
                QUEUE_SIZE   = 5,
                TIMEOUT      = 1,     // in seconds
                DELTA        = 450000 // main thread delay, in microseconds
            };

            if (veryVerbose) {
                MTCOUT << "Creating server thread." << MTENDL;
            }
            const int PORT = PORT_BASE
                           + bdeu_HashUtil::hash0(argv[0], PORT_RANGE);
            if (verbose) { P(PORT); }

            bteso_IPv4Address peer("127.0.0.1", PORT);

            case4WorkerInfo info;
            bcemt_ThreadUtil::Handle worker;
            info.d_i           = 0;
            info.d_portNumber  = PORT;
            info.d_queueSize   = QUEUE_SIZE;
            info.d_timeOut     = TIMEOUT;
            info.d_worker_p    = &worker;
            ASSERT(0 == bcemt_ThreadUtil::create(&worker,
                                                 &case4OpenConnectThread,
                                                 (void *)&info));

            if (veryVerbose) {
                MTCOUT << "Creating channel pool." << MTENDL;
            }
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMetricsInterval(30.0);
            config.setMaxConnections(1);
            config.setReadTimeout(600);    // in seconds

            int channelId = -1;
            bces_AtomicInt numFailures = 0;
            bces_AtomicInt isConnected = 0;

            btemt_ChannelPool *pMX;

            btemt_ChannelPool::PoolStateChangeCallback    poolCb(
                    bdef_BindUtil::bind( &case4PoolStateCb
                                       , _1, _2, _3
                                       , &numFailures
                                       , &info));

            btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bind( &case4ChannelCb
                                       , _1, _2, _3, _4
                                       , &channelId
                                       , &isConnected
                                       , &numFailures
                                       , &info
                                       , &pMX));

            btemt_ChannelPool::DataReadCallback         dataCb;
            makeNull(&dataCb);

            btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
            pMX = &mX;
            ASSERT(0 == mX.start());

            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                if (verbose) {
                    MTCOUT << "\tStarting test iteration "
                           << i << " (with auto-read)." << MTENDL;
                }

                isConnected           = 0;
                numFailures           = 0;
                info.d_expUserId      = USER_ID + i + 1;
                info.d_expNumFailures = i;
                info.d_i              = i;
                info.d_worker_p       = &worker;
                if (0 != i) {
                    // Initial opening server may have failed, do not overwrite
                    // info in that case.  Otherwise, it may have failed at
                    // previous iteration, so we need to reset port number.
                    // Note that port number will be reset to 0 if the callback
                    // could not open the server in time for the last
                    // connection attempt.

                    info.d_portNumber = PORT;
                }

                bcemt_ThreadUtil::microSleep(0, 1);
                if (veryVerbose) {
                    MTCOUT << "--->ATTEMPT " << i << ' '
                           << bdetu_SystemTime::now()
                           << " ---------------------------------------"
                           << MTENDL;
                }
                LOOP_ASSERT(i,
                            0 == mX.connect(
                          "localhost",
                          PORT,
                          info.d_expNumFailures + 1,
                          bdet_TimeInterval(TIMEOUT),
                          info.d_expUserId,
                          (i % 2)
                          ? btemt_ChannelPool::BTEMT_RESOLVE_ONCE
                          : btemt_ChannelPool::BTEMT_RESOLVE_AT_EACH_ATTEMPT));

                bcemt_ThreadUtil::microSleep(0, (i + 2) * TIMEOUT);
                if (0 == info.d_portNumber)
                {
                    LOOP2_ASSERT(numFailures, info.d_expNumFailures,
                                 numFailures == info.d_expNumFailures);
                    LOOP2_ASSERT(i, isConnected, 3 == isConnected);
                    // autoRead & channelUp
                }
                else {
                    // If server was not opened by this test driver, it might
                    // have been opened already: there is no telling if this
                    // connection should have failed or succeeded.

                    MTCOUT << "WARNING: Did not open server: ";
                    P(info.d_portNumber);
                    P_(isConnected);
                    P_(numFailures); P(info.d_expNumFailures);
                    cout << MTENDL;
                }
                bcemt_ThreadUtil::join(worker);
                bcemt_ThreadUtil::microSleep(0, 2 * TIMEOUT);
                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(0, 2 * TIMEOUT);
            }
            ASSERT(0 == mX.stop());
        }
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

        if (verbose)
            cout << "TESTING 'connect' TO NON-EXISTENT HOST\n";
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

            const bdet_TimeInterval TIMEOUT(0.5);
            const int EXPECTED_SEVERITY = 1;

            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMetricsInterval(10.0);
            config.setMaxConnections(1);

            btemt_ChannelPool::ChannelStateChangeCallback channelCb;
            btemt_ChannelPool::DataReadCallback         dataCb;

            makeNull(&channelCb);
            makeNull(&dataCb);

            if (verbose)
                cout << "\tWhen channel pool is running: test 1\n";
            {
                for (int i = 0; i < NUM_DATA; ++i) {
                    for (int j = 0; j < MAX_NUM_ATTEMPTS; ++j) {
                        bces_AtomicInt isInvoked = 0;

                        btemt_ChannelPool::PoolStateChangeCallback    poolCb(
                                bdef_BindUtil::bindA( &ta
                                                    , &case4ErrorPoolStateCb
                                                    , _1, _2, _3
                                                    , DATA[i].d_sourceId
                                                    , EXPECTED_SEVERITY
                                                    , &isInvoked));

                        btemt_ChannelPool mX(channelCb, dataCb, poolCb,
                                             config, &ta);

                        LOOP2_ASSERT(i, j, 0 == mX.start());
                        bteso_IPv4Address peer(DATA[i].d_peerAddress,
                                               DATA[i].d_peerPort);
                        if (veryVerbose) {
                            MTCOUT << "\nRegistering at "
                                   << bdetu_SystemTime::now() << MTENDL;
                            P(peer);
                            P(j + 1);
                            P(TIMEOUT);
                        }
                        int ret = mX.connect(peer, j + 1, TIMEOUT,
                                                           DATA[i].d_sourceId);
                        LOOP3_ASSERT(i, j, ret, 0 == ret);
                        while (j + 1 != isInvoked) {
                            bcemt_ThreadUtil::sleep(TIMEOUT);
                        }
                        if (veryVerbose) {
                            MTCOUT << "Succeded at "
                                << bdetu_SystemTime::now() << MTENDL;
                        }
                        bcemt_ThreadUtil::sleep(TIMEOUT);
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
                bcemt_Mutex                  channelEventsMutex;
                bsl::vector<my_PoolEvent>    poolEvents;
                bcemt_Mutex                  poolEventsMutex;

                btemt_ChannelPool::PoolStateChangeCallback    poolCb(
                        bdef_BindUtil::bind( &recordPoolState
                                           , _1, _2, _3
                                           , &poolEvents
                                           , &poolEventsMutex));

                btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bind( &recordChannelState
                                       , _1, _2, _3, _4
                                       , &channelEvents
                                       , &channelEventsMutex));

                btemt_ChannelPool::DataReadCallback         dataCb;
                makeNull(&dataCb);

                config.setMaxThreads(4);
                btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);
                ASSERT(0 == mX.start());

                for (int i = 0; i < MAX_NUM_ATTEMPTS; ++i) {
                    for (int j = 0; j < NUM_DATA; ++j) {
                        bteso_IPv4Address peer(DATA[j].d_peerAddress,
                                               DATA[j].d_peerPort);
                        if (veryVerbose) {
                            P_(bdetu_SystemTime::now()); P_(peer);
                            P(TIMEOUT);
                        }
                        int ret = mX.connect(peer, 1, TIMEOUT,
                                                           DATA[j].d_sourceId);
                        LOOP3_ASSERT(i, j, ret, 0 == ret);
                    }
                    bcemt_ThreadUtil::sleep(TIMEOUT + TIMEOUT + TIMEOUT);
                }

                bcemt_ThreadUtil::sleep(TIMEOUT + TIMEOUT);
                ASSERT(0 == mX.stop());

                if (veryVerbose) {
                    PV(channelEvents);
                    PV(poolEvents);
                }
            }
            if (verbose) cout << "\tWhen channel pool is NOT running.\n";
            for (int i = 0; i < NUM_DATA; ++i) {
                for (int j = 0; j < MAX_NUM_ATTEMPTS; ++j) {
                    bces_AtomicInt isInvoked = 0;

                    btemt_ChannelPool::PoolStateChangeCallback    poolCb(
                            bdef_BindUtil::bindA( &ta
                                                , &case4ErrorPoolStateCb
                                                , _1, _2, _3
                                                , DATA[i].d_sourceId
                                                , EXPECTED_SEVERITY
                                                , &isInvoked));

                    btemt_ChannelPool mX(channelCb, dataCb, poolCb, config,
                                         &ta);
                    bteso_IPv4Address peer(DATA[i].d_peerAddress,
                            DATA[i].d_peerPort);
                    if (veryVerbose) {
                        P(peer);
                        P(j + 1);
                    }
                    LOOP2_ASSERT(i, j, -1 == mX.connect(peer, j + 1, TIMEOUT,
                                                        DATA[i].d_sourceId));
                    bcemt_ThreadUtil::sleep(TIMEOUT);
                    LOOP2_ASSERT(i, j, 0 == isInvoked);
                }
            }
        }
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVerbose) { P(ta); }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'start' AND 'stop' METHODS
        //
        // Concerns: TODO
        // Plan: Start a channel pool, assert that there is one thread only,
        //   wait, and stop the channel pool.
        //
        // Testing
        //   btemt_ChannelPool::start();
        //   btemt_ChannelPool::stop();
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'start' AND 'stop'"
                 << "\n==========================" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            enum {
                BASE_PORT = 1234,
                BACKLOG   = 10
            };

            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMetricsInterval(10.0);

            btemt_ChannelPool::ChannelStateChangeCallback channelCb;
            btemt_ChannelPool::PoolStateChangeCallback    poolCb;
            btemt_ChannelPool::DataReadCallback           dataCb;

            btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);

            ASSERT(0 == mX.start());
            ASSERT(1 == mX.numThreads());

            bcemt_ThreadUtil::microSleep(0, 5);

            ASSERT(0 == mX.stop());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //   - What are the concerns exactly?
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

      } break;
      case 1: {
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

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);
            config.setMetricsInterval(10.0);

            btemt_ChannelPool::ChannelStateChangeCallback channelCb;
            btemt_ChannelPool::PoolStateChangeCallback    poolCb;
            btemt_ChannelPool::DataReadCallback           dataCb;

            btemt_ChannelPool mX(channelCb, dataCb, poolCb, config, &ta);

            enum {
                PORT_BASE  = 1235,
                PORT_RANGE = 1000,
                BACKLOG    = 10
            };
            const int PORT = PORT_BASE
                           + bdeu_HashUtil::hash0(argv[0], PORT_RANGE);
            if (verbose) { P(PORT); }

            for (int i = 0; i < 10; ++i) {
                bteso_IPv4Address address;
                address.setPortNumber(PORT + i);
                int s = mX.listen(address, BACKLOG, i);
                if (veryVerbose) {
                    P(s);
                }
                LOOP_ASSERT(i, 0 == s);
                const bteso_IPv4Address *result = mX.serverAddress(i);
                LOOP_ASSERT(i, result);
                LOOP_ASSERT(i, result->portNumber() == PORT + i);
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

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST:  my_QueueClient
        //
        // Plan:
        //
        // Start my_QueueProcessor server first, then run this case in a
        // separate process
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE - QUEUE PROCESSOR" << endl
                          << "=======================================" << endl;

        if (verbose) {
            cout << "This test case (to be run manually) complements "
                 << "case 22.\n\n";

            cout << "Usage: Arguments provided to this test driver are"
                 << " (besides the case number, -1)\n\t<verbosity>"
                 << " <numConnections> <portNumber> <host> <numMessages>\n"
                 << "\tDefaults:  1  10  2564  127.0.0.1  10000\n"
                 << "\t\t(verbosity: 0 = silent, 1 = verbose, 2 = veryVerbose,"
                 << " 3 = veryVeryVerbose)\n";
        }

        using namespace USAGE_EXAMPLE_M1_NAMESPACE;

        enum {
            DEFAULT_PORT_NUMBER = 2564,
            DEFAULT_NUM_CONNECTIONS =  10,
            DEFAULT_NUM_MESSAGES = 10000
        };

        caseMinusOneInfo info;
        info.d_numConnections = (argc > 3) ? atoi(argv[3])
                                           : DEFAULT_NUM_CONNECTIONS;
        info.d_portNumber     = (argc > 4) ? atoi(argv[4])
                                           : DEFAULT_PORT_NUMBER;
        info.d_hostname       = (argc > 5) ? argv[5]
                                           : const_cast<char*>("127.0.0.1");
        info.d_numMessages    = (argc > 6) ? atoi(argv[6])
                                           : DEFAULT_NUM_MESSAGES;

        if (argc > 2) {
            verbose         = atoi(argv[2]) > 0;
            veryVerbose     = atoi(argv[2]) > 1;
            veryVeryVerbose = atoi(argv[2]) > 2;
        }
        else {
            verbose = 1;
            veryVerbose = veryVeryVerbose = 0;
        }

        usageExampleMinusOne(&info);

      } break;
      case -2:
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

        bcema_TestAllocator ta;
        btemt_ChannelPool::DataReadCallback         dataCb;
        btemt_ChannelPool::PoolStateChangeCallback  poolCb;

        bcemt_Barrier channelBarrier(2);

        btemt_ChannelPool  *poolAddr;
        int                 poolEvent = -1;
        int                *eventAddr = &poolEvent;
        int                 channelId = -1;

        btemt_ChannelPool::ChannelStateChangeCallback channelCb(
                    bdef_BindUtil::bindA( 0
                                        , &caseN2ChannelStateCallback
                                        , _1, _2, _3, _4
                                        , &poolAddr
                                        , &eventAddr
                                        , &channelBarrier
                                        , &channelId));

        makeNull(&poolCb);
        makeNull(&dataCb);

        btemt_ChannelPoolConfiguration cpc;
        cpc.setMaxConnections(1024);
        cpc.setMaxThreads(MAX_THREADS);
        cpc.setMetricsInterval(100.0);

        const int maxOutgoingMessageSize = 2;
        cpc.setOutgoingMessageSizes(0,
                                    (maxOutgoingMessageSize+1)/2,
                                    maxOutgoingMessageSize);

        Obj mX(channelCb, dataCb, poolCb, cpc, &ta);  const Obj& X = mX;
        poolAddr = &mX;

        const bteso_IPv4Address ADDRESS("127.0.0.1", 0);

        int retCode = mX.start();
        LOOP_ASSERT(retCode, 0 == retCode);
        retCode = mX.listen(ADDRESS, BACKLOG, SERVER_ID);
        LOOP_ASSERT(retCode, 0 == retCode);
        ASSERT(0 == X.numChannels());
        const bteso_IPv4Address PEER = *mX.serverAddress(SERVER_ID);

        typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> Factory;
        typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;
        typedef btesos_TcpChannel                                Channel;

        Factory              factory;

        bsl::vector<bsl::pair<Socket*, bcema_SharedPtr<Channel> > > channels;
        channels.resize(16);

        int oldNumBytes = ta.numBytesInUse();
        for (int i = 0; i < (int) channels.size(); ++i) {
            channels[i].first = factory.allocate();
            channels[i].second.createInplace(0, channels[i].first);

            Socket *socket = channels[i].first;
            Channel *channel = channels[i].second.ptr();
            int connectRetCode =  socket->connect(PEER);
            ASSERT(0 == connectRetCode);
            ASSERT(0 == channel->isInvalid());

            if (0 == connectRetCode) {
                channelBarrier.wait();
            }
            else {
                cout << "Failed to connect to channelpool.  Abort.\n";
                return -1;
            }
            const int newNumBytes = ta.numBytesInUse();
            bsl::cout << "Channel " << i << ": "
                      << newNumBytes - oldNumBytes
                      << bsl::endl;
            oldNumBytes = newNumBytes;
        }
        for (int i = 0; i < (int) channels.size(); ++i) {
            channels[i].second->invalidate();
            channels[i].second.clear();
            factory.deallocate(channels[i].first);
        }
      } break;
      default: {
        cerr << "WARNING: CASE " << test << " NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == bteso_SocketImpUtil::cleanup());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    cout << "TEST CASE " << test << " ENDED "
         << bdetu_SystemTime::nowAsDatetimeGMT() << endl;

#endif // !BSLS_PLATFORM__OS_WINDOWS

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
