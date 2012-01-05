// btemt_sessionpool.t.cpp                                            -*-C++-*-
#include <btemt_sessionpool.h>

#include <btemt_asyncchannel.h>
#include <btemt_channelpool.h>
#include <btemt_message.h>
#include <btemt_session.h>

#include <bcema_blobutil.h>
#include <bcema_testallocator.h>
#include <bcemt_thread.h>
#include <bcemt_barrier.h>

#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdef_placeholder.h>
#include <bdef_memfn.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bteso_ipv4address.h>
#include <bteso_inetstreamsocketfactory.h>
#include <bteso_streamsocket.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bdef_PlaceHolders;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver does nothing, except verify that the usage example can
// compile and run correctly as documented.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [1 ] BREATHING TEST
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\n";aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"\
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << \
       "\t" << #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"\
       << #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print identifier
                                                        // and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote identifier
                                                        // literally.
#define NL() bsl::cout << bsl::endl;                    // End of line
#define P_(X) bsl::cout << #X " = " << (X) << ", "<< bsl::flush; // P(X)
                                                                 // without
                                                                 // '\n'
#define T_()  bsl::cout << '\t' << bsl::flush;        // Print tab w/o newline
#define L_ __LINE__                           // current Line number

// The following macros facilitate thread-safe streaming to standard output.

#define MTCOUT   coutMutex.lock(); { bsl::cout \
                                           << bcemt_ThreadUtil::selfIdAsInt() \
                                           << ": "
#define MTENDL   bsl::endl;  } coutMutex.unlock()

#define DUMMYBRACE {  //keep context-aware editors happy about the "}" below

#define MTFLUSH  bsl::flush; } coutMutex.unlock()

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef btemt_SessionPool Obj;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

static bcemt_Mutex coutMutex;

//=============================================================================
//                  SUPPORT CLASSES AND FUNCTIONS USED FOR TESTING
//-----------------------------------------------------------------------------

namespace BTEMT_SESSION_POOL_DRQS_29067989 {

static bslma_TestAllocator testAllocator;
static int callbackCount = 0;

static int maxLength = 0;
static int maxSize = 0;
static int maxExtra = 0;
static int maxNumBuffers = 0;
static int maxNumDataBuffers = 0;

enum {
    PAYLOAD_SIZE = 320,
    HALF_PAYLOAD_SIZE = 160
};

void poolStateCallback(int reason, int source, void *userData)
{
    if (veryVerbose) {
        MTCOUT << "Pool state changed: (" << reason << ", " << source
               << ") " << MTENDL;
    }
}

void sessionStateCallback(int            state,
                          int            handle,
                          btemt_Session *session,
                          void          *userData,
                          bcemt_Barrier *barrier)
{
    switch(state) {
      case btemt_SessionPool::SESSION_DOWN: {
          if (veryVerbose) {
              MTCOUT << "Client from "
                     << session->channel()->peerAddress()
                     << " has disconnected."
                     << MTENDL;
          }
      } break;
      case btemt_SessionPool::SESSION_UP: {
          if (veryVerbose) {
              MTCOUT << "Client connected from "
                     << session->channel()->peerAddress()
                     << MTENDL;
          }
          barrier->wait();
      } break;
    }
}

                            // ===================
                            // class TesterSession
                            // ===================

class TesterSession : public btemt_Session {
    // This class is a concrete implementation of the 'btemt_Session' protocol
    // to use along with 'Tester' objects.

    // DATA
    btemt_AsyncChannel *d_channel_p; // underlying channel (held, not owned)

  private:
    // NOT IMPLEMENTED
    TesterSession(const TesterSession&);
    TesterSession& operator=(const TesterSession&);

    // PRIVATE MANIPULATORS
    void readCb(int         result,
                int        *numNeeded,
                bcema_Blob *blob,
                int         channelId);
        // Read callback for session pool.

  public:
    // CREATORS
    TesterSession(btemt_AsyncChannel *channel);
        // Create a new 'TesterSession' object for the specified 'channel'.

    ~TesterSession();
        // Destroy this object.

        // MANIPULATORS
    virtual int start();
        // Begin the asynchronous operation of this session.

    virtual int stop();
        // Stop the operation of this session.

    // ACCESSORS
    virtual btemt_AsyncChannel *channel() const;
        // Return the channel associate with this session.
};

                      // ========================
                      // class TestSessionFactory
                      // ========================

class TestSessionFactory : public btemt_SessionFactory {
    // This class is a concrete implementation of the
    // 'btemt_SessionFactory' that simply allocates 'TestSession'
    // objects.  No specific allocation strategy (such as pooling) is
    // implemented.

    bslma_Allocator *d_allocator_p; // memory allocator (held, not owned)

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TestSessionFactory,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    TestSessionFactory(bslma_Allocator *basicAllocator = 0);
        // Create a new 'TestSessionFactory' object.  Optionally specify
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    virtual ~TestSessionFactory();
        // Destroy this factory.

    // MANIPULATORS
    virtual void allocate(btemt_AsyncChannel                    *channel,
                          const btemt_SessionFactory::Callback&  callback);
       // Asynchronously allocate a 'btemt_Session' object for the
       // specified 'channel', and invoke the specified 'callback' with
       // this session.

    virtual void deallocate(btemt_Session *session);
        // Deallocate the specified 'session'.
};

                      // ================
                      // class TestServer
                      // ================

class TestServer {
    // This class implements a multi-user multi-threaded echo server.

    // DATA
    btemt_ChannelPoolConfiguration d_config;          // pool
                                                      // configuration

    btemt_SessionPool             *d_sessionPool_p;   // managed pool
                                                      // (owned)

    TestSessionFactory             d_sessionFactory;  // TestSession
                                                      // factory

    int                            d_portNumber;      // port on which this
                                                      // server is
                                                      // listening

    bslma_Allocator               *d_allocator_p;     // memory allocator
                                                      // (held)

    bcemt_Mutex                   *d_coutMutex;       // Mutex for cout
                                                      // operations

    // PRIVATE MANIPULATORS
    void poolStateCb(int reason, int source, void *userData);
        // Indicates the status of the whole pool.

    void sessionStateCb(int            state,
                        int            handle,
                        btemt_Session *session,
                        void          *userData);
        // Per-session state.

    private:
    // NOT IMPLEMENTED
    TestServer(const TestServer& original);
    TestServer& operator=(const TestServer& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TestServer,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    TestServer(bcemt_Mutex     *coutMutex,
               int              portNumber,
               int              numConnections);
        // Create an echo server that listens for incoming connections on
        // the specified 'portNumber' managing up to the specified
        // 'numConnections' simultaneous connections.  Pass the specified
        // 'reuseAddressFlag' to the set the 'REUSE_ADDRESS' socket option
        // to the listening socket.  The echo server will use the
        // specified 'coutLock' to synchronize access to the standard
        // output.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined if
        // 'coutLock' is 0.

    ~TestServer();
        // Destroy this server.

    // MANIPULATORS
    const btemt_SessionPool& pool() const;
        // Return a non-modifiable reference to the session pool used by
        // this echo server.

    int portNumber() const;
        // Return the port number on which this server is listening.
};

                            // -------------------
                            // class TesterSession
                            // -------------------

// CREATORS
TesterSession::TesterSession(btemt_AsyncChannel *channel)
: d_channel_p(channel)
{
}

TesterSession::~TesterSession()
{
}

// MANIPULATORS
int TesterSession::start()
{
    if (veryVerbose) {
        MTCOUT << "Session started" << MTENDL;
    }

    btemt_AsyncChannel::BlobBasedReadCallback callback =
                           bdef_MemFnUtil::memFn(&TesterSession::readCb, this);
    return d_channel_p->read(PAYLOAD_SIZE, callback);
}

int TesterSession::stop()
{
    if (veryVerbose) {
        MTCOUT << "Session stopped" << MTENDL;
    }

    d_channel_p->close();
    return 0;
}

void TesterSession::readCb(int         result,
                           int        *numNeeded,
                           bcema_Blob *blob,
                           int         channelId)
{
    if (result) {
        // Session is going down.

        d_channel_p->close();
        return;
    }

    ++callbackCount;

    ASSERT(numNeeded);
    ASSERT(blob);
    ASSERT(0 < blob->length());

    int consume = blob->length();
    if (0 == callbackCount % 2) {
        // Every second time leave a bit of data in the input buffer.
        consume -= sizeof(int);
    }

    if (maxLength < blob->length()) {
        maxLength = blob->length();
    }

    if (maxSize < blob->totalSize()) {
        maxSize = blob->totalSize();
    }

    if (maxExtra < (blob->totalSize() - blob->length())) {
        maxExtra = (blob->totalSize() - blob->length());
    }

    if (maxNumDataBuffers < blob->numDataBuffers()) {
        maxNumDataBuffers = blob->numDataBuffers();
    }

    if (maxNumBuffers < blob->numBuffers()) {
        maxNumBuffers = blob->numBuffers();
    }

    bcema_BlobUtil::erase(blob, 0, consume);

    *numNeeded = PAYLOAD_SIZE;
}

// ACCESSORS
btemt_AsyncChannel *TesterSession::channel() const
{
    return d_channel_p;
}

                      // ------------------------
                      // class TestSessionFactory
                      // ------------------------

// CREATORS
TestSessionFactory::TestSessionFactory(bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

TestSessionFactory::~TestSessionFactory()
{
}

// MANIPULATORS
void
TestSessionFactory::allocate(btemt_AsyncChannel                    *channel,
                             const btemt_SessionFactory::Callback&  callback)
{
    TesterSession *session = new (*d_allocator_p) TesterSession(channel);
    callback(0, session);
}

void
TestSessionFactory::deallocate(btemt_Session *session)
{
    d_allocator_p->deleteObject(session);
}

                          // -------------------
                          // class TestServer
                          // -------------------

// PRIVATE MANIPULATORS
void TestServer::poolStateCb(int reason, int source, void *userData)
{
    if (veryVerbose) {
        d_coutMutex->lock();
        bsl::cout << "Pool state changed: (" << reason << ", " << source
                  << ") " << bsl::endl;
        d_coutMutex->unlock();
    }
}

void TestServer::sessionStateCb(int            state,
                                int            handle,
                                btemt_Session *session,
                                void          *userData)
{
    switch(state) {
      case btemt_SessionPool::SESSION_DOWN: {
        if (veryVerbose) {
            d_coutMutex->lock();
            bsl::cout << "Client from "
                      << session->channel()->peerAddress()
                      << " has disconnected."
                      << bsl::endl;
            d_coutMutex->unlock();
        }
      } break;
      case btemt_SessionPool::SESSION_UP: {
        if (veryVerbose) {
            d_coutMutex->lock();
            bsl::cout << "Client connected from "
                      << session->channel()->peerAddress()
                      << bsl::endl;
            d_coutMutex->unlock();
        }
      } break;
    }
}

// CREATORS
TestServer::TestServer(bcemt_Mutex     *coutMutex,
                       int              portNumber,
                       int              numConnections)
: d_sessionFactory(bslma_Default::defaultAllocator())
, d_allocator_p(bslma_Default::defaultAllocator())
, d_coutMutex(coutMutex)
{
    d_config.setMaxThreads(4);                  // 4 I/O threads
    d_config.setMaxConnections(numConnections);
    d_config.setMetricsInterval(10.0);          // seconds
    d_config.setMaxWriteCache(1<<10);           // 1Mb
    d_config.setIncomingMessageSizes(1, 100, 1024);

    typedef btemt_SessionPool::SessionPoolStateCallback SessionPoolStateCb;
    SessionPoolStateCb poolStateCb = bdef_MemFnUtil::memFn(
                                        &TestServer::poolStateCb, this);

    d_sessionPool_p = new (*d_allocator_p)
                           btemt_SessionPool(d_config,
                                             poolStateCb,
                                             true,
                                             &testAllocator);

    btemt_SessionPool::SessionStateCallback sessionStateCb =
                     bdef_MemFnUtil::memFn(&TestServer::sessionStateCb,
                                           this);

    int rc = d_sessionPool_p->start();
    ASSERT(!rc);
    int handle;
    rc = d_sessionPool_p->listen(&handle,
                                 sessionStateCb,
                                 portNumber,
                                 numConnections,
                                 &d_sessionFactory);
    ASSERT(!rc);

    d_portNumber = d_sessionPool_p->portNumber(handle);
}

TestServer::~TestServer()
{
    d_sessionPool_p->stop();
    d_allocator_p->deleteObject(d_sessionPool_p);
}

// ACCESSORS
const btemt_SessionPool& TestServer::pool() const
{
    return *d_sessionPool_p;
}

int TestServer::portNumber() const
{
    return d_portNumber;
}

}

namespace BTEMT_SESSION_POOL_DRQS_24968477 {

void poolStateCallback(int reason, int source, void *userData)
{
    if (veryVerbose) {
        MTCOUT << "Pool state changed: (" << reason << ", " << source
               << ") " << MTENDL;
    }
}

void sessionStateCallback(int            state,
                          int            handle,
                          btemt_Session *session,
                          void          *userData,
                          bcemt_Barrier *barrier)
{
    switch(state) {
      case btemt_SessionPool::SESSION_DOWN: {
          if (veryVerbose) {
              MTCOUT << "Client from "
                     << session->channel()->peerAddress()
                     << " has disconnected."
                     << MTENDL;
          }
      } break;
      case btemt_SessionPool::SESSION_UP: {
          if (veryVerbose) {
              MTCOUT << "Client connected from "
                     << session->channel()->peerAddress()
                     << MTENDL;
          }
          barrier->wait();
      } break;
    }
}

                            // ===================
                            // class TesterSession
                            // ===================

class TesterSession : public btemt_Session {
    // This class is a concrete implementation of the 'btemt_Session' protocol
    // to use along with 'Tester' objects.

    // DATA
    btemt_AsyncChannel *d_channel_p; // underlying channel (held, not owned)

  private:
    // NOT IMPLEMENTED
    TesterSession(const TesterSession&);
    TesterSession& operator=(const TesterSession&);

  public:
    // CREATORS
    TesterSession(btemt_AsyncChannel *channel);
        // Create a new 'TesterSession' object for the specified 'channel'.

    ~TesterSession();
        // Destroy this object.

    // MANIPULATORS
    virtual int start();
        // Begin the asynchronous operation of this session.

    virtual int stop();
        // Stop the operation of this session.

    // ACCESSORS
    virtual btemt_AsyncChannel *channel() const;
        // Return the channel associate with this session.
};

                    // ===================
                    // class TesterFactory
                    // ===================

class TesterFactory : public btemt_SessionFactory {
    // This class is a concrete implementation of the 'btemt_SessionFactory'
    // that simply allocates 'TesterSession' objects.  No specific allocation
    // strategy (such as pooling) is implemented.

    // DATA
    btemt_SessionFactory::Callback  d_callback;
    btemt_Session                  *d_session_p;
    bslma_Allocator                *d_allocator_p;  // memory allocator (held,
                                                    // not owned)

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TesterFactory,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    TesterFactory(bslma_Allocator *basicAllocator = 0);
        // Create a new 'TesterFactory' object using the specified 'barrier'.
        // Optionally specify 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    virtual ~TesterFactory();
        // Destroy this factory.

    // MANIPULATORS
    virtual void allocate(btemt_AsyncChannel                   *channel,
                          const btemt_SessionFactory::Callback& callback);
        // Asynchronously allocate a 'btemt_Session' object for the
        // specified 'channel', and invoke the specified 'callback' with
        // this session.

    virtual void deallocate(btemt_Session *session);
        // Deallocate the specified 'session'.

    void readCb(int         state,
                int        *numNeeded,
                bcema_Blob *msg,
                int         channelId);
        // Blob based read callback for session pool.

    void writeCb(int         state,
                 int        *numNeeded,
                 bcema_Blob *msg,
                 int         channelId);
        // Blob based read callback for session pool.

    btemt_AsyncChannel *channel() const;
        // Return the channel managed by this factory.
};

                            // -------------------
                            // class TesterSession
                            // -------------------

// CREATORS
TesterSession::TesterSession(btemt_AsyncChannel *channel)
: d_channel_p(channel)
{
}

TesterSession::~TesterSession()
{
}

// MANIPULATORS
int TesterSession::start()
{
    if (veryVerbose) {
        MTCOUT << "Session started" << MTENDL;
    }

    return 0;
}

int TesterSession::stop()
{
    if (veryVerbose) {
        MTCOUT << "Session stopped" << MTENDL;
    }

    d_channel_p->close();
    return 0;
}

// ACCESSORS
btemt_AsyncChannel *TesterSession::channel() const
{
    return d_channel_p;
}

                        // -------------------
                        // class TesterFactory
                        // -------------------

// CREATORS
TesterFactory::TesterFactory(bslma_Allocator *basicAllocator)
: d_session_p(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

TesterFactory::~TesterFactory()
{
}

// MANIPULATORS
void TesterFactory::allocate(btemt_AsyncChannel                    *channel,
                             const btemt_SessionFactory::Callback&  callback)
{
    if (veryVerbose) {
        MTCOUT << "Allocate factory called: " << MTENDL;
    }

    d_session_p = new (*d_allocator_p) TesterSession(channel);

    callback(0, d_session_p);
}

void TesterFactory::deallocate(btemt_Session *session)
{
    if (veryVerbose) {
        MTCOUT << "Deallocate factory called: " << MTENDL;
    }

    d_allocator_p->deleteObjectRaw(session);
}

void TesterFactory::readCb(int         state,
                           int        *numNeeded,
                           bcema_Blob *msg,
                           int         channelId)
{
    if (veryVerbose) {
        MTCOUT << "Read callback called with: " << state << MTENDL;
    }

    d_callback(0, d_session_p);
}

btemt_AsyncChannel *TesterFactory::channel() const
{
    return d_session_p->channel();
}

}

namespace BTEMT_SESSION_POOL_DRQS_20535695 {

void poolStateCallback(int reason, int source, void *userData)
{
    if (veryVerbose) {
        MTCOUT << "Pool state changed: (" << reason << ", " << source
               << ") " << MTENDL;
    }
}

void sessionStateCallback(int            state,
                          int            handle,
                          btemt_Session *session,
                          void          *userData)
{
    switch(state) {
      case btemt_SessionPool::SESSION_DOWN: {
          if (veryVerbose) {
              MTCOUT << "Client from "
                     << session->channel()->peerAddress()
                     << " has disconnected."
                     << MTENDL;
          }
      } break;
      case btemt_SessionPool::SESSION_UP: {
          if (veryVerbose) {
              MTCOUT << "Client connected from "
                     << session->channel()->peerAddress()
                     << MTENDL;
          }
      } break;
    }
}

                    // ===================
                    // class TesterFactory
                    // ===================

class TesterFactory : public btemt_SessionFactory {
    // This class is a concrete implementation of the 'btemt_SessionFactory'
    // that simply allocates 'TesterSession' objects.  No specific allocation
    // strategy (such as pooling) is implemented.

    // DATA
    bcemt_Barrier *d_barrier_p;

  public:
    // CREATORS
    TesterFactory(bcemt_Barrier *barrier);
        // Create a new 'TesterFactory' object using the specified 'barrier'.

    virtual ~TesterFactory();
        // Destroy this factory.

    // MANIPULATORS
    virtual void allocate(btemt_AsyncChannel                   *channel,
                          const btemt_SessionFactory::Callback& callback);
        // Asynchronously allocate a 'btemt_Session' object for the
        // specified 'channel', and invoke the specified 'callback' with
        // this session.

    virtual void deallocate(btemt_Session *session);
        // Deallocate the specified 'session'.

    void readCb(int         state,
                int        *numNeeded,
                bcema_Blob *msg,
                int         channelId);
        // Blob based read callback for session pool.

    void writeCb(int         state,
                 int        *numNeeded,
                 bcema_Blob *msg,
                 int         channelId);
        // Blob based read callback for session pool.

    btemt_AsyncChannel *channel() const;
        // Return the channel managed by this factory.
};

                        // -------------------
                        // class TesterFactory
                        // -------------------

// CREATORS
TesterFactory::TesterFactory(bcemt_Barrier *barrier)
: d_barrier_p(barrier)
{
}

TesterFactory::~TesterFactory()
{
}

// MANIPULATORS
void TesterFactory::allocate(btemt_AsyncChannel                    *channel,
                             const btemt_SessionFactory::Callback&  callback)
{
    if (veryVerbose) {
        MTCOUT << "Allocate factory called: " << MTENDL;
    }

    d_barrier_p->wait();
}

void TesterFactory::deallocate(btemt_Session *session)
{
    if (veryVerbose) {
        MTCOUT << "Deallocate factory called: " << MTENDL;
    }
}

void TesterFactory::readCb(int         state,
                           int        *numNeeded,
                           bcema_Blob *msg,
                           int         channelId)
{
    if (veryVerbose) {
        MTCOUT << "Read callback called with: " << state << MTENDL;
    }
}

btemt_AsyncChannel *TesterFactory::channel() const
{
    return 0;
}

}

namespace BTEMT_SESSION_POOL_DRQS_22373213 {

void poolStateCallback(int reason, int source, void *userData)
{
    if (veryVerbose) {
        MTCOUT << "Pool state changed: (" << reason << ", " << source
               << ") " << MTENDL;
    }
}

void sessionStateCallback(int            state,
                          int            handle,
                          btemt_Session *session,
                          void          *userData)
{
    switch(state) {
      case btemt_SessionPool::SESSION_DOWN: {
          if (veryVerbose) {
              MTCOUT << "Client from "
                     << session->channel()->peerAddress()
                     << " has disconnected."
                     << MTENDL;
          }
      } break;
      case btemt_SessionPool::SESSION_UP: {
          if (veryVerbose) {
              MTCOUT << "Client connected from "
                     << session->channel()->peerAddress()
                     << MTENDL;
          }
      } break;
    }
}

class CallbackClass {
    // This class provides a callback function that can be invoked to read
    // data from a channel.  In addition this class also stores a counter than
    // checks the number of times the callback function was invoked.

    int            d_cbCount;   // number of times the callback function was
                                // invoked.
    bcemt_Barrier *d_barrier_p;

  public:
    // CREATORS
    CallbackClass(bcemt_Barrier *barrier)
    : d_cbCount(0)
    , d_barrier_p(barrier)
    {
    }

    ~CallbackClass()
    {
    }

    // MANIPULATORS
    void readCb(int         state,
                int        *numNeeded,
                bcema_Blob *msg,
                int         channelId)
    {
        if (veryVerbose) {
            MTCOUT << "Read callback called with: " << state << MTENDL;
            MTCOUT << msg->length() << MTENDL;
        }
        bcema_Blob tmpBlob;
        tmpBlob.moveDataBuffers(msg);
        *numNeeded = 1;
        ++d_cbCount;
        d_barrier_p->wait();
    }

    // ACCESSORS
    int cbCount() const { return d_cbCount; }
};
                            // ===================
                            // class TesterSession
                            // ===================

class TesterSession : public btemt_Session {
    // This class is a concrete implementation of the 'btemt_Session' protocol
    // to use along with 'Tester' objects.

    // DATA
    btemt_AsyncChannel *d_channel_p; // underlying channel (held, not owned)

  private:
    // NOT IMPLEMENTED
    TesterSession(const TesterSession&);
    TesterSession& operator=(const TesterSession&);

  public:
    // CREATORS
    TesterSession(btemt_AsyncChannel *channel);
        // Create a new 'TesterSession' object for the specified 'channel'.

    ~TesterSession();
        // Destroy this object.

    // MANIPULATORS
    virtual int start();
        // Begin the asynchronous operation of this session.

    virtual int stop();
        // Stop the operation of this session.

    // ACCESSORS
    virtual btemt_AsyncChannel *channel() const;
        // Return the channel associate with this session.
};

                    // ===================
                    // class TesterFactory
                    // ===================

class TesterFactory : public btemt_SessionFactory {
    // This class is a concrete implementation of the 'btemt_SessionFactory'
    // that simply allocates 'TesterSession' objects.  No specific allocation
    // strategy (such as pooling) is implemented.

    // DATA
    btemt_SessionFactory::Callback  d_callback;
    btemt_Session                  *d_session_p;
    bcemt_Barrier                  *d_barrier_p;
    bslma_Allocator                *d_allocator_p;  // memory allocator (held,
                                                    // not owned)

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TesterFactory,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    TesterFactory(bcemt_Barrier *barrier, bslma_Allocator *basicAllocator = 0);
        // Create a new 'TesterFactory' object using the specified 'barrier'.
        // Optionally specify 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    virtual ~TesterFactory();
        // Destroy this factory.

    // MANIPULATORS
    virtual void allocate(btemt_AsyncChannel                   *channel,
                          const btemt_SessionFactory::Callback& callback);
        // Asynchronously allocate a 'btemt_Session' object for the
        // specified 'channel', and invoke the specified 'callback' with
        // this session.

    virtual void deallocate(btemt_Session *session);
        // Deallocate the specified 'session'.

    void readCb(int         state,
                int        *numNeeded,
                bcema_Blob *msg,
                int         channelId);
        // Blob based read callback for session pool.

    void writeCb(int         state,
                int        *numNeeded,
                bcema_Blob *msg,
                int         channelId);
        // Blob based read callback for session pool.

    btemt_AsyncChannel *channel() const;
        // Return the channel managed by this factory.
};

                            // -------------------
                            // class TesterSession
                            // -------------------

// CREATORS
TesterSession::TesterSession(btemt_AsyncChannel *channel)
: d_channel_p(channel)
{
}

TesterSession::~TesterSession()
{
}

// MANIPULATORS
int TesterSession::start()
{
    if (veryVerbose) {
        MTCOUT << "Session started" << MTENDL;
    }

    return 0;
}

int TesterSession::stop()
{
    if (veryVerbose) {
        MTCOUT << "Session stopped" << MTENDL;
    }

    d_channel_p->close();
    return 0;
}

// ACCESSORS
btemt_AsyncChannel *TesterSession::channel() const
{
    return d_channel_p;
}

                        // -------------------
                        // class TesterFactory
                        // -------------------

// CREATORS
TesterFactory::TesterFactory(bcemt_Barrier   *barrier,
                             bslma_Allocator *basicAllocator)
: d_session_p(0)
, d_barrier_p(barrier)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

TesterFactory::~TesterFactory()
{
}

// MANIPULATORS
void TesterFactory::allocate(btemt_AsyncChannel                    *channel,
                             const btemt_SessionFactory::Callback&  callback)
{
    if (veryVerbose) {
        MTCOUT << "Allocate factory called: " << MTENDL;
    }

    d_session_p = new (*d_allocator_p) TesterSession(channel);

    callback(0, d_session_p);

    d_barrier_p->wait();
}

void TesterFactory::deallocate(btemt_Session *session)
{
    if (veryVerbose) {
        MTCOUT << "Deallocate factory called: " << MTENDL;
    }

    d_allocator_p->deleteObjectRaw(session);
}

void TesterFactory::readCb(int         state,
                           int        *numNeeded,
                           bcema_Blob *msg,
                           int         channelId)
{
    if (veryVerbose) {
        MTCOUT << "Read callback called with: " << state << MTENDL;
    }

    d_callback(0, d_session_p);
}

btemt_AsyncChannel *TesterFactory::channel() const
{
    return d_session_p->channel();
}

}

namespace BTEMT_SESSION_POOL_DRQS {

static bcemt_Mutex globalLock;
static bool        closeChannel = false;


                            // ===================
                            // class TesterSession
                            // ===================

class TesterSession : public btemt_Session {
    // This class is a concrete implementation of the 'btemt_Session' protocol
    // to use along with 'Tester' objects.

    // DATA
    btemt_AsyncChannel *d_channel_p; // underlying channel (held, not owned)

  private:
    // NOT IMPLEMENTED
    TesterSession(const TesterSession&);
    TesterSession& operator=(const TesterSession&);

  public:
    // CREATORS
    TesterSession(btemt_AsyncChannel *channel);
        // Create a new 'TesterSession' object for the specified 'channel'.

    ~TesterSession();
        // Destroy this object.

    // MANIPULATORS
    virtual int start();
        // Begin the asynchronous operation of this session.

    virtual int stop();
        // Stop the operation of this session.

    // ACCESSORS
    virtual btemt_AsyncChannel *channel() const;
        // Return the channel associate with this session.
};

                    // ===================
                    // class TesterFactory
                    // ===================

class TesterFactory : public btemt_SessionFactory {
    // This class is a concrete implementation of the 'btemt_SessionFactory'
    // that simply allocates 'TesterSession' objects.  No specific allocation
    // strategy (such as pooling) is implemented.

    // DATA
    int                              d_mode;
    btemt_SessionFactory::Callback   d_callback;
    btemt_Session                   *d_session_p;
    bslma_Allocator                 *d_allocator_p; // memory allocator (held,
                                                    // not owned)

  public:

    enum { LISTENER = 0, CONNECTOR };

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TesterFactory,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    TesterFactory(int mode, bslma_Allocator *basicAllocator = 0);
        // Create a new 'TesterFactory' object of the specified 'mode'.
        // Optionally specify 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    virtual ~TesterFactory();
        // Destroy this factory.

    // MANIPULATORS
    virtual void allocate(btemt_AsyncChannel                   *channel,
                          const btemt_SessionFactory::Callback& callback);
        // Asynchronously allocate a 'btemt_Session' object for the
        // specified 'channel', and invoke the specified 'callback' with
        // this session.

    virtual void deallocate(btemt_Session *session);
        // Deallocate the specified 'session'.

    void readCb(int         state,
                int        *numNeeded,
                bcema_Blob *msg,
                int         channelId);
        // Blob based read callback for session pool.


    void writeCb(int         state,
                int        *numNeeded,
                bcema_Blob *msg,
                int         channelId);
        // Blob based read callback for session pool.

    btemt_Session *session() const;
        // Return the session managed by this factory.
};

                            // -------------------
                            // class TesterSession
                            // -------------------

// CREATORS
TesterSession::TesterSession(btemt_AsyncChannel *channel)
: d_channel_p(channel)
{
}

TesterSession::~TesterSession()
{
}

// MANIPULATORS
int TesterSession::start()
{
    if (veryVerbose) {
        MTCOUT << "Session started" << MTENDL;
    }

    return 0;
}

int TesterSession::stop()
{
    if (veryVerbose) {
        MTCOUT << "Session stopped" << MTENDL;
    }

    d_channel_p->close();
    return 0;
}

// ACCESSORS
btemt_AsyncChannel *TesterSession::channel() const
{
    return d_channel_p;
}

                        // -------------------
                        // class TesterFactory
                        // -------------------

// CREATORS
TesterFactory::TesterFactory(int mode, bslma_Allocator *basicAllocator)
: d_mode(mode)
, d_session_p(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

TesterFactory::~TesterFactory()
{
}

// MANIPULATORS
void TesterFactory::allocate(btemt_AsyncChannel                    *channel,
                             const btemt_SessionFactory::Callback&  callback)
{
    if (veryVerbose) {
        MTCOUT << "Allocate factory called: " << MTENDL;
        if (LISTENER == d_mode) {
            MTCOUT << "LISTENER" << MTENDL;
        }
        else {
            MTCOUT << "CONNECTOR" << MTENDL;
        }
    }

    d_session_p = new (*d_allocator_p) TesterSession(channel);

    callback(0, d_session_p);
}

void TesterFactory::deallocate(btemt_Session *session)
{
    d_allocator_p->deleteObjectRaw(session);
}

void TesterFactory::readCb(int         state,
                           int        *numNeeded,
                           bcema_Blob *msg,
                           int         channelId)
{
    if (veryVerbose) {
        MTCOUT << "Read callback called with: " << state << MTENDL;
    }

    d_callback(0, d_session_p);
}

btemt_Session *TesterFactory::session() const
{
    return d_session_p;
}

                        // -------------------
                        // class TesterFactory
                        // -------------------

class Tester {
    btemt_ChannelPoolConfiguration  d_config;          // pool configuration

    btemt_SessionPool              *d_sessionPool_p;   // managed pool (owned)

    TesterFactory                   d_sessionFactory;  // factory

    int                             d_portNumber;      // port on which this
                                                       // echo server is
                                                       // listening

    bslma_Allocator                *d_allocator_p;     // memory allocator
                                                       // (held)

  public:

    enum { LISTENER = 0, CONNECTOR };

    // CREATORS
    Tester(int                       mode,
           const bteso_IPv4Address&  endPointAddr,
           bool                      useBlobBasedReads = false,
           bslma_Allocator          *basicAllocator = 0);
    ~Tester();

    void poolStateCb(int reason, int source, void *userData);
        // Indicates the status of the whole pool.

    void sessionStateCb(int            state,
                        int            handle,
                        btemt_Session *session,
                        void          *userData);
        // Per-session state.

    int portNumber() const;

    btemt_Session *session() const { return d_sessionFactory.session(); }
};

// CREATORS
Tester::Tester(int                       mode,
               const bteso_IPv4Address&  endPointAddr,
               bool                      useBlobBasedReads,
               bslma_Allocator          *basicAllocator)
: d_sessionFactory(mode, basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_config.setMaxThreads(4);                  // 4 I/O threads
    d_config.setMaxConnections(5);
    d_config.setReadTimeout(5.0);               // in seconds
    d_config.setMetricsInterval(10.0);          // seconds
    d_config.setMaxWriteCache(1<<10);           // 1Mb
    d_config.setIncomingMessageSizes(1, 100, 1024);

    typedef btemt_SessionPool::SessionPoolStateCallback SessionPoolStateCb;

    SessionPoolStateCb poolStateCb = bdef_MemFnUtil::memFn(
                                          &Tester::poolStateCb,
                                          this);

    d_sessionPool_p = new (*d_allocator_p)
                             btemt_SessionPool(d_config,
                                               poolStateCb,
                                               useBlobBasedReads,
                                               basicAllocator);

    btemt_SessionPool::SessionStateCallback sessionStateCb =
                 bdef_MemFnUtil::memFn(&Tester::sessionStateCb,
                                       this);

    ASSERT(0 == d_sessionPool_p->start());
    int handle;
    if (LISTENER == mode) {
        ASSERT(0 == d_sessionPool_p->listen(&handle,
                                            sessionStateCb,
                                            d_portNumber,
                                            5,
                                            1,
                                            &d_sessionFactory));

        d_portNumber = d_sessionPool_p->portNumber(handle);
    }
    else {
        ASSERT(0 == d_sessionPool_p->connect(&handle,
                                             sessionStateCb,
                                             endPointAddr,
                                             5,
                                             bdet_TimeInterval(1),
                                             &d_sessionFactory));
        d_portNumber = endPointAddr.portNumber();
    }

}

Tester::~Tester()
{
    d_sessionPool_p->stop();
    d_allocator_p->deleteObject(d_sessionPool_p);
}

void Tester::poolStateCb(int reason, int source, void *userData)
{
    if (veryVerbose) {
        MTCOUT << "Pool state changed: (" << reason << ", " << source << ") "
               << MTENDL;
    }
}

void Tester::sessionStateCb(int            state,
                            int            handle,
                            btemt_Session *session,
                            void          *userData)
{
    switch (state) {
      case btemt_SessionPool::SESSION_DOWN: {
          if (veryVerbose) {
              MTCOUT << "Client from "
                     << session->channel()->peerAddress()
                     << " has disconnected."
                     << MTENDL;
          }
      } break;
      case btemt_SessionPool::SESSION_UP: {
          if (veryVerbose) {
              MTCOUT << "Client connected from "
                     << session->channel()->peerAddress()
                     << MTENDL;
          }
      } break;
    }
}

int Tester::portNumber() const
{
    return d_portNumber;
}

}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace BTEMT_SESSION_BLOB_BASED_EXAMPLE {

                            // =============================
                            // class my_BlobBasedEchoSession
                            // =============================

   class my_BlobBasedEchoSession : public btemt_Session {
       // This class is a concrete implementation of the 'btemt_Session'
       // protocol to use along with 'my_BlobBasedEchoServer' objects.

       // DATA
       btemt_AsyncChannel *d_channel_p; // underlying channel (held, not owned)

       // PRIVATE MANIPULATORS
       void blobBasedReadCb(int         state,
                            int        *numNeeded,
                            bcema_Blob *msg,
                            int         channelId);
           // Blob based read callback for session pool.

     private:
       // NOT IMPLEMENTED
       my_BlobBasedEchoSession(const my_BlobBasedEchoSession&);
       my_BlobBasedEchoSession& operator=(const my_BlobBasedEchoSession&);

     public:
       // CREATORS
       my_BlobBasedEchoSession(btemt_AsyncChannel *channel);
           // Create a new 'my_BlobBasedEchoSession' object for the specified
           // 'channel'.

       ~my_BlobBasedEchoSession();
           // Destroy this object.

       // MANIPULATORS
       virtual int start();
           // Begin the asynchronous operation of this session.

       virtual int stop();
           // Stop the operation of this session.

       // ACCESSORS
       virtual btemt_AsyncChannel *channel() const;
           // Return the channel associate with this session.
    };

                        // ====================================
                        // class my_BlobBasedEchoSessionFactory
                        // ====================================

    class my_BlobBasedEchoSessionFactory : public btemt_SessionFactory {
        // This class is a concrete implementation of the
        // 'btemt_SessionFactory' that simply allocates
        // 'my_BlobBasedEchoSession' objects.  No specific allocation strategy
        // (such as pooling) is implemented.

        bslma_Allocator *d_allocator_p; // memory allocator (held, not owned)

      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(my_BlobBasedEchoSessionFactory,
                                     bslalg_TypeTraitUsesBslmaAllocator);

        // CREATORS
        my_BlobBasedEchoSessionFactory(bslma_Allocator *basicAllocator = 0);
            // Create a new 'my_BlobBasedEchoSessionFactory' object.
            // Optionally specify 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default
            // allocator is used.

        virtual ~my_BlobBasedEchoSessionFactory();
           // Destroy this factory.

        // MANIPULATORS
        virtual void allocate(btemt_AsyncChannel                   *channel,
                              const btemt_SessionFactory::Callback& callback);
           // Asynchronously allocate a 'btemt_Session' object for the
           // specified 'channel', and invoke the specified 'callback' with
           // this session.

        virtual void deallocate(btemt_Session *session);
           // Deallocate the specified 'session'.
    };

                            // -----------------------------
                            // class my_BlobBasedEchoSession
                            // -----------------------------

    // PRIVATE MANIPULATORS
    void my_BlobBasedEchoSession::blobBasedReadCb(int         state,
                                                  int        *numNeeded,
                                                  bcema_Blob *msg,
                                                  int         channelId)
    {
        if (state) {
            // Session is going down.

            d_channel_p->close();
            return;
        }

        ASSERT(0 < msg->length());

        ASSERT(0 == d_channel_p->write(*msg));

        bcema_BlobUtil::erase(msg, 0, msg->length());

        *numNeeded = 1;

        d_channel_p->close(); // close connection.
    }

    // CREATORS
    my_BlobBasedEchoSession::my_BlobBasedEchoSession(
                                                   btemt_AsyncChannel *channel)
    : d_channel_p(channel)
    {
    }

    my_BlobBasedEchoSession::~my_BlobBasedEchoSession()
    {
    }

    // MANIPULATORS
    int my_BlobBasedEchoSession::start()
    {
        btemt_AsyncChannel::BlobBasedReadCallback callback(
               bdef_MemFnUtil::memFn(&my_BlobBasedEchoSession::blobBasedReadCb,
                                     this));
        return d_channel_p->read(1, callback);
    }

    int my_BlobBasedEchoSession::stop()
    {
        d_channel_p->close();
        return 0;
    }

    // ACCESSORS
    btemt_AsyncChannel *my_BlobBasedEchoSession::channel() const
    {
        return d_channel_p;
    }

                        // ------------------------------------
                        // class my_BlobBasedEchoSessionFactory
                        // ------------------------------------

    // CREATORS
    my_BlobBasedEchoSessionFactory::my_BlobBasedEchoSessionFactory(
                                               bslma_Allocator *basicAllocator)
    : d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
    }

    my_BlobBasedEchoSessionFactory::~my_BlobBasedEchoSessionFactory()
    {
    }

    // MANIPULATORS
    void
    my_BlobBasedEchoSessionFactory::allocate(
                                btemt_AsyncChannel                   *channel,
                                const btemt_SessionFactory::Callback& callback)
    {
        my_BlobBasedEchoSession *session = new (*d_allocator_p)
                                              my_BlobBasedEchoSession(channel);
        callback(0, session);
    }

    void
    my_BlobBasedEchoSessionFactory::deallocate(btemt_Session *session)
    {
        d_allocator_p->deleteObjectRaw(session);
    }

                        // ============================
                        // class my_BlobBasedEchoServer
                        // ============================

    class my_BlobBasedEchoServer {
        // This class implements a multi-user multi-threaded echo server.

        // DATA
        btemt_ChannelPoolConfiguration d_config;          // pool
                                                          // configuration

        btemt_SessionPool             *d_sessionPool_p;   // managed pool
                                                          // (owned)

        my_BlobBasedEchoSessionFactory d_sessionFactory;  // my_EchoSession
                                                          // factory

        int                            d_portNumber;      // port on which this
                                                          // echo server is
                                                          // listening

        bcemt_Mutex                   *d_coutLock_p;      // mutex protecting
                                                          // bsl::cout

        bslma_Allocator               *d_allocator_p;     // memory allocator
                                                          // (held)

        // PRIVATE MANIPULATORS
        void poolStateCb(int reason, int source, void *userData);
            // Indicates the status of the whole pool.

        void sessionStateCb(int            state,
                            int            handle,
                            btemt_Session *session,
                            void          *userData);
            // Per-session state.

      private:
        // NOT IMPLEMENTED
        my_BlobBasedEchoServer(const my_BlobBasedEchoServer& original);
        my_BlobBasedEchoServer& operator=(const my_BlobBasedEchoServer& rhs);

      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(my_BlobBasedEchoServer,
                                     bslalg_TypeTraitUsesBslmaAllocator);

        // CREATORS
        my_BlobBasedEchoServer(bcemt_Mutex     *coutLock,
                               int              portNumber,
                               int              numConnections,
                               bool             reuseAddressFlag,
                               bslma_Allocator *basicAllocator = 0);
            // Create an echo server that listens for incoming connections on
            // the specified 'portNumber' managing up to the specified
            // 'numConnections' simultaneous connections.  The echo server
            // will use the specified 'coutLock' to synchronize access to the
            // standard output.  Optionally specify a 'basicAllocator' used to
            // supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.  The behavior is undefined
            // unless coutLock is not 0.

        ~my_BlobBasedEchoServer();
            // Destroy this server.

        // MANIPULATORS
        const btemt_SessionPool& pool() const;
            // Return a non-modifiable reference to the session pool used by
            // this echo server.

        int portNumber() const;
            // Return the actual port number on which this server is listening.
    };

    // my_echoserver.h (continued)

                            // -------------------
                            // class my_BlobBasedEchoServer
                            // -------------------

    // PRIVATE MANIPULATORS
    void my_BlobBasedEchoServer::poolStateCb(int reason,
                                             int source,
                                             void *userData)
    {
        if (veryVerbose) {
            d_coutLock_p->lock();
            bsl::cout << "Pool state changed: (" << reason << ", " << source
                      << ") " << bsl::endl;
            d_coutLock_p->unlock();
        }
    }

    void my_BlobBasedEchoServer::sessionStateCb(int            state,
                                                int            handle,
                                                btemt_Session *session,
                                                void          *userData)
    {
        switch(state) {
          case btemt_SessionPool::SESSION_DOWN: {
              if (veryVerbose) {
                  d_coutLock_p->lock();
                  bsl::cout << "Client from "
                            << session->channel()->peerAddress()
                            << " has disconnected."
                            << bsl::endl;
                  d_coutLock_p->unlock();
              }
          } break;
          case btemt_SessionPool::SESSION_UP: {
              if (veryVerbose) {
                  d_coutLock_p->lock();
                  bsl::cout << "Client connected from "
                            << session->channel()->peerAddress()
                            << bsl::endl;
                  d_coutLock_p->unlock();
              }
          } break;
        }
    }

    // CREATORS
    my_BlobBasedEchoServer::my_BlobBasedEchoServer(
                                             bcemt_Mutex     *lock,
                                             int              portNumber,
                                             int              numConnections,
                                             bool             reuseAddressFlag,
                                             bslma_Allocator *basicAllocator)
    : d_sessionFactory(basicAllocator)
    , d_coutLock_p(lock)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
        d_config.setMaxThreads(4);                  // 4 I/O threads
        d_config.setMaxConnections(numConnections);
        d_config.setReadTimeout(5.0);               // in seconds
        d_config.setMetricsInterval(10.0);          // seconds
        d_config.setMaxWriteCache(1<<10);           // 1Mb
        d_config.setIncomingMessageSizes(1, 100, 1024);

        typedef btemt_SessionPool::SessionPoolStateCallback SessionPoolStateCb;

        SessionPoolStateCb poolStateCb = bdef_MemFnUtil::memFn(
                                          &my_BlobBasedEchoServer::poolStateCb,
                                          this);

        d_sessionPool_p = new (*d_allocator_p)
                             btemt_SessionPool(d_config,
                                               poolStateCb,
                                               true,
                                               basicAllocator);

        btemt_SessionPool::SessionStateCallback sessionStateCb =
                 bdef_MemFnUtil::memFn(&my_BlobBasedEchoServer::sessionStateCb,
                                       this);

        ASSERT(0 == d_sessionPool_p->start());
        int handle;
        ASSERT(0 == d_sessionPool_p->listen(&handle, sessionStateCb,
                                            portNumber,
                                            numConnections,
                                            reuseAddressFlag,
                                            &d_sessionFactory));

        d_portNumber = d_sessionPool_p->portNumber(handle);
    }

    my_BlobBasedEchoServer::~my_BlobBasedEchoServer()
    {
        d_sessionPool_p->stop();
        d_allocator_p->deleteObjectRaw(d_sessionPool_p);
    }

    // ACCESSORS
    const btemt_SessionPool& my_BlobBasedEchoServer::pool() const
    {
        return *d_sessionPool_p;
    }

    int my_BlobBasedEchoServer::portNumber() const
    {
        return d_portNumber;
    }

    int blobBasedUsageExample(bslma_Allocator *allocator) {

        enum {
            BACKLOG = 5,
            REUSE   = 1
        };

        my_BlobBasedEchoServer echoServer(&coutMutex, 0, BACKLOG, REUSE);

        bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
        bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

        const char STRING[] = "Hello World!";

        const bteso_IPv4Address ADDRESS("127.0.0.1", echoServer.portNumber());
        ASSERT(0 == socket->connect(ADDRESS));
        ASSERT(sizeof(STRING) == socket->write(STRING, sizeof(STRING)));

        char readBuffer[sizeof(STRING)];
        ASSERT(sizeof(STRING) == socket->read(readBuffer, sizeof(STRING)));
        ASSERT(0 == bsl::strcmp(readBuffer, STRING));

        factory.deallocate(socket);
        return 0;
    }


}  // close namespace BTEMT_SESSION_POOL_USAGE_EXAMPLE

namespace BTEMT_SESSION_POOL_USAGE_EXAMPLE {

///Usage example
///-------------
// The following example implements a simple echo server.  This server accepts
// connections, reads what it receives right away from the network stream,
// sends it back and closes the connection.
//
// To set up this server, users must create a concrete session class derived
// from 'btemt_Session' protocol, and a factory for creating instances of this
// concrete session type.  'my_EchoSession' objects are created by a factory
// that must be derived from 'btemt_SessionFactory'.  A 'my_EchoSessionFactory'
// just allocates and deallocates sessions (with no pooling or allocation
// strategy).  This is the simplest form of factory.
//..
    // my_echoserver.h

                            // ====================
                            // class my_EchoSession
                            // ====================

   class my_EchoSession : public btemt_Session {
       // This class is a concrete implementation of the 'btemt_Session'
       // protocol to use along with 'my_EchoServer' objects.

       // DATA
       btemt_AsyncChannel *d_channel_p; // underlying channel (held, not owned)

       // PRIVATE MANIPULATORS
       void readCb(int                   state,
                   int                  *numConsumed,
                   int                  *numNeeded,
                   const btemt_DataMsg&  msg);
           // Read callback for session pool.

     private:
       // NOT IMPLEMENTED
       my_EchoSession(const my_EchoSession&);
       my_EchoSession& operator=(const my_EchoSession&);

     public:
       // CREATORS
       my_EchoSession(btemt_AsyncChannel *channel);
           // Create a new 'my_EchoSession' object for the specified 'channel'.

       ~my_EchoSession();
           // Destroy this object.

       // MANIPULATORS
       virtual int start();
           // Begin the asynchronous operation of this session.

       virtual int stop();
           // Stop the operation of this session.

       // ACCESSORS
       virtual btemt_AsyncChannel *channel() const;
           // Return the channel associate with this session.
    };

                        // ===========================
                        // class my_EchoSessionFactory
                        // ===========================

    class my_EchoSessionFactory : public btemt_SessionFactory {
        // This class is a concrete implementation of the
        // 'btemt_SessionFactory' that simply allocates 'my_EchoSession'
        // objects.  No specific allocation strategy (such as pooling) is
        // implemented.

        bslma_Allocator *d_allocator_p; // memory allocator (held, not owned)

      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(my_EchoSessionFactory,
                                     bslalg_TypeTraitUsesBslmaAllocator);

        // CREATORS
        my_EchoSessionFactory(bslma_Allocator *basicAllocator = 0);
            // Create a new 'my_EchoSessionFactory' object.  Optionally specify
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        virtual ~my_EchoSessionFactory();
           // Destroy this factory.

        // MANIPULATORS
        virtual void allocate(btemt_AsyncChannel                   *channel,
                              const btemt_SessionFactory::Callback& callback);
           // Asynchronously allocate a 'btemt_Session' object for the
           // specified 'channel', and invoke the specified 'callback' with
           // this session.

        virtual void deallocate(btemt_Session *session);
           // Deallocate the specified 'session'.
    };
//..
// The implementations of those session and factory types are rather
// straightforward.  'readCb' will be called when the first byte is received.
// It is in this method that the echo logic is implemented.
//..
    // my_echoserver.cpp

                            // --------------------
                            // class my_EchoSession
                            // --------------------

    // PRIVATE MANIPULATORS
    void my_EchoSession::readCb(int                   state,
                                int                  *numConsumed,
                                int                  *numNeeded,
                                const btemt_DataMsg&  msg)
    {
        if (state) {
            // Session is going down.

            d_channel_p->close();
            return;
        }

        ASSERT(numConsumed);
        ASSERT(msg.data());
        ASSERT(0 < msg.data()->length());

        ASSERT(0 == d_channel_p->write(msg));

        *numConsumed = msg.data()->length();
        *numNeeded   = 1;

        d_channel_p->close(); // close connection.
    }

    // CREATORS
    my_EchoSession::my_EchoSession(btemt_AsyncChannel *channel)
    : d_channel_p(channel)
    {
    }

    my_EchoSession::~my_EchoSession()
    {
    }

    // MANIPULATORS
    int my_EchoSession::start()
    {
        btemt_AsyncChannel::ReadCallback
                        callback(bdef_MemFnUtil::memFn(&my_EchoSession::readCb,
                                                       this));
        return d_channel_p->read(1, callback);
    }

    int my_EchoSession::stop()
    {
        d_channel_p->close();
        return 0;
    }

    // ACCESSORS
    btemt_AsyncChannel *my_EchoSession::channel() const
    {
        return d_channel_p;
    }

                        // ---------------------------
                        // class my_EchoSessionFactory
                        // ---------------------------

    // CREATORS
    my_EchoSessionFactory::my_EchoSessionFactory(
                                               bslma_Allocator *basicAllocator)
    : d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
    }

    my_EchoSessionFactory::~my_EchoSessionFactory()
    {
    }

    // MANIPULATORS
    void
    my_EchoSessionFactory::allocate(
                                btemt_AsyncChannel                   *channel,
                                const btemt_SessionFactory::Callback& callback)
    {
        my_EchoSession *session = new (*d_allocator_p) my_EchoSession(channel);
        callback(0, session);
    }

    void
    my_EchoSessionFactory::deallocate(btemt_Session *session)
    {
        d_allocator_p->deleteObjectRaw(session);
    }
//..
//  We now have all the pieces needed to design and implement our echo server.
//  The server itself owns an instance of the above-defined factory.
//..
    // my_echoserver.h (continued)

                        // ===================
                        // class my_EchoServer
                        // ===================

    class my_EchoServer {
        // This class implements a multi-user multi-threaded echo server.

        // DATA
        btemt_ChannelPoolConfiguration d_config;          // pool
                                                          // configuration

        btemt_SessionPool             *d_sessionPool_p;   // managed pool
                                                          // (owned)

        my_EchoSessionFactory          d_sessionFactory;  // my_EchoSession
                                                          // factory

        int                            d_portNumber;      // port on which this
                                                          // echo server is
                                                          // listening

        bcemt_Mutex                   *d_coutLock_p;      // mutex protecting
                                                          // bsl::cout

        bslma_Allocator               *d_allocator_p;     // memory allocator
                                                          // (held)

        // PRIVATE MANIPULATORS
        void poolStateCb(int reason, int source, void *userData);
            // Indicates the status of the whole pool.

        void sessionStateCb(int            state,
                            int            handle,
                            btemt_Session *session,
                            void          *userData);
            // Per-session state.

      private:
        // NOT IMPLEMENTED
        my_EchoServer(const my_EchoServer& original);
        my_EchoServer& operator=(const my_EchoServer& rhs);

      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(my_EchoServer,
                                     bslalg_TypeTraitUsesBslmaAllocator);

        // CREATORS
        my_EchoServer(bcemt_Mutex     *coutLock,
                      int              portNumber,
                      int              numConnections,
                      bool             reuseAddressFlag,
                      bslma_Allocator *basicAllocator = 0);
            // Create an echo server that listens for incoming connections on
            // the specified 'portNumber' managing up to the specified
            // 'numConnections' simultaneous connections.  The echo server
            // will use the specified 'coutLock' to synchronize access to the
            // standard output.  Optionally specify a 'basicAllocator' used to
            // supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.  The behavior is undefined
            // unless coutLock is not 0.

        ~my_EchoServer();
            // Destroy this server.

        // MANIPULATORS
        const btemt_SessionPool& pool() const;
            // Return a non-modifiable reference to the session pool used by
            // this echo server.

        int portNumber() const;
            // Return the actual port number on which this server is listening.
    };
//..
// Note that this example server prints information depending on
// implicitly-defined static variables and therefore must use a mutex to
// synchronize access to 'bsl::cout'.  A production application should use a
// proper logging mechanism instead such as the 'bael' logger.
//..
    // my_echoserver.h (continued)

                            // -------------------
                            // class my_EchoServer
                            // -------------------

    // PRIVATE MANIPULATORS
    void my_EchoServer::poolStateCb(int reason, int source, void *userData)
    {
        if (veryVerbose) {
            d_coutLock_p->lock();
            bsl::cout << "Pool state changed: (" << reason << ", " << source
                      << ") " << bsl::endl;
            d_coutLock_p->unlock();
        }
    }

    void my_EchoServer::sessionStateCb(int            state,
                                       int            handle,
                                       btemt_Session *session,
                                       void          *userData) {

        switch(state) {
          case btemt_SessionPool::SESSION_DOWN: {
              if (veryVerbose) {
                  d_coutLock_p->lock();
                  bsl::cout << "Client from "
                            << session->channel()->peerAddress()
                            << " has disconnected."
                            << bsl::endl;
                  d_coutLock_p->unlock();
              }
          } break;
          case btemt_SessionPool::SESSION_UP: {
              if (veryVerbose) {
                  d_coutLock_p->lock();
                  bsl::cout << "Client connected from "
                            << session->channel()->peerAddress()
                            << bsl::endl;
                  d_coutLock_p->unlock();
              }
          } break;
        }
    }

    // CREATORS
    my_EchoServer::my_EchoServer(bcemt_Mutex     *lock,
                                 int              portNumber,
                                 int              numConnections,
                                 bool             reuseAddressFlag,
                                 bslma_Allocator *basicAllocator)
    : d_sessionFactory(basicAllocator)
    , d_coutLock_p(lock)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
        d_config.setMaxThreads(4);                  // 4 I/O threads
        d_config.setMaxConnections(numConnections);
        d_config.setReadTimeout(5.0);               // in seconds
        d_config.setMetricsInterval(10.0);          // seconds
        d_config.setMaxWriteCache(1<<10);           // 1Mb
        d_config.setIncomingMessageSizes(1, 100, 1024);

        typedef btemt_SessionPool::SessionPoolStateCallback SessionPoolStateCb;

        SessionPoolStateCb poolStateCb = bdef_MemFnUtil::memFn(
                                            &my_EchoServer::poolStateCb, this);

        d_sessionPool_p = new (*d_allocator_p)
                             btemt_SessionPool(d_config,
                                               poolStateCb,
                                               basicAllocator);

        btemt_SessionPool::SessionStateCallback sessionStateCb =
                       bdef_MemFnUtil::memFn(&my_EchoServer::sessionStateCb,
                                             this);

        ASSERT(0 == d_sessionPool_p->start());
        int handle;
        ASSERT(0 == d_sessionPool_p->listen(&handle, sessionStateCb,
                                            portNumber,
                                            numConnections,
                                            reuseAddressFlag,
                                            &d_sessionFactory));

        d_portNumber = d_sessionPool_p->portNumber(handle);
    }

    my_EchoServer::~my_EchoServer()
    {
        d_sessionPool_p->stop();
        d_allocator_p->deleteObjectRaw(d_sessionPool_p);
    }

    // ACCESSORS
    const btemt_SessionPool& my_EchoServer::pool() const
    {
        return *d_sessionPool_p;
    }

    int my_EchoServer::portNumber() const
    {
        return d_portNumber;
    }
//..
// We can implement a simple "Hello World!" example to exercise our echo
// server.
//..
    int usageExample(bslma_Allocator *allocator) {

        enum {
            BACKLOG = 5,
            REUSE   = 1
        };

        my_EchoServer echoServer(&coutMutex, 0, BACKLOG, REUSE);

        bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
        bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

        const char STRING[] = "Hello World!";

        const bteso_IPv4Address ADDRESS("127.0.0.1", echoServer.portNumber());
        ASSERT(0 == socket->connect(ADDRESS));
        ASSERT(sizeof(STRING) == socket->write(STRING, sizeof(STRING)));

        char readBuffer[sizeof(STRING)];
        ASSERT(sizeof(STRING) == socket->read(readBuffer, sizeof(STRING)));
        ASSERT(0 == bsl::strcmp(readBuffer, STRING));

        factory.deallocate(socket);
        return 0;
    }
//..

}  // close namespace BTEMT_SESSION_POOL_USAGE_EXAMPLE
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    bcema_TestAllocator ta("ta", veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // REPRODUCING DRQS 29067989
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //  DRQS 29067989
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "DRQS 29067989" << bsl::endl
                               << "=============" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_DRQS_29067989;

        TestServer testServer(&coutMutex, 0, 5);

        bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
        bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

        const bteso_IPv4Address ADDRESS("127.0.0.1", testServer.portNumber());
        int rc = socket->connect(ADDRESS);
        ASSERT(!rc);

        char payload[PAYLOAD_SIZE];
        bsl::memset(payload, 'X', PAYLOAD_SIZE);
        const int NT = 10000;

        for (int i = 0; i < NT; ++i) {
            socket->write(payload, PAYLOAD_SIZE);
        }

        bcemt_ThreadUtil::microSleep(0, 3);

        factory.deallocate(socket);

        if (veryVerbose) {
            MTCOUT << "TA In Use: " << testAllocator.numBytesInUse() << MTENDL;
            MTCOUT << "TA In Use Blocks: " << testAllocator.numBlocksInUse()
                   << MTENDL;
            MTCOUT << "TA In Max: " << testAllocator.numBytesMax() << MTENDL;

            MTCOUT << "maxLength: " << maxLength << MTENDL;
            MTCOUT << "maxSize: " << maxSize << MTENDL;
            MTCOUT << "maxExtra: " << maxExtra << MTENDL;
            MTCOUT << "maxNumBuffers: " << maxNumBuffers << MTENDL;
            MTCOUT << "maxNumDataBuffers: " << maxNumDataBuffers << MTENDL;
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // REPRODUCING DRQS 24968477
        //  Ensure that the bug where d_numSessions is decremented in stop and
        //  then again when the session handle is destroyed has been fixed.
        //
        // Concerns:
        //: 1 d_numSessions is not decremented twice after a call to 'stop'.
        //
        // Plan:
        //: 1 Create a session pool object, mX, and listen on a port on the
        //:   local machine. 
        //:
        //: 2 Define NUM_SESSIONS with a value of 2 as the number of sessions
        //:   to be created.
        //:
        //: 3 Open NUM_SESSIONS sockets and 'connect' to the port number on
        //:   which the session pool is listening.
        //:
        //: 4 Confirm that numSessions on mX returns NUM_SESSIONS.
        //:
        //: 5 Call 'stop' on the session pool and confirm that numSessions
        //:   returns 0.
        //
        // Testing:
        //  DRQS 24968477
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "DRQS 24968477" << bsl::endl
                               << "=============" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_DRQS_24968477;

        typedef btemt_SessionPool::SessionStateCallback SessionCb;
        typedef btemt_SessionPool::SessionPoolStateCallback PoolCb;

        const int NUM_SESSIONS = 2;
        btemt_ChannelPoolConfiguration config;
        config.setMaxThreads(NUM_SESSIONS);
        config.setMaxConnections(NUM_SESSIONS);

        PoolCb poolStateCb(&poolStateCallback);
        bcemt_Barrier barrier(NUM_SESSIONS + 1);
        SessionCb callback(bdef_BindUtil::bind(&sessionStateCallback,
                                               _1, _2, _3, _4,
                                               &barrier));

        btemt_SessionPool mX(config, poolStateCb, false);
        const btemt_SessionPool& X = mX;

        ASSERT(0 == mX.start());

        int           handle;
        TesterFactory sessionFactory;
        int rc = mX.listen(&handle, callback, 0, 5, &sessionFactory);
        ASSERT(!rc);

        const int PORTNUM = X.portNumber(handle);

        bteso_IPv4Address ADDRESS("127.0.0.1", PORTNUM);

        bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *> sockets;
        bteso_InetStreamSocketFactory<bteso_IPv4Address> socketFactory;
        for (int i = 0; i < NUM_SESSIONS; ++i) {
            bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                      socketFactory.allocate();

            ASSERT(0 == socket->connect(ADDRESS));
            sockets.push_back(socket);
            bcemt_ThreadUtil::sleep(bdet_TimeInterval(1));
        }

        barrier.wait();

        int ns = X.numSessions();
        LOOP_ASSERT(ns, NUM_SESSIONS == ns);

        ASSERT(0 == mX.stop());

        ns = X.numSessions();
        LOOP_ASSERT(ns, 0 == ns);

        for (int i = 0; i < NUM_SESSIONS; ++i) {
          bteso_StreamSocket<bteso_IPv4Address> *socket = sockets[i];
          socketFactory.deallocate(socket);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // REPRODUCING DRQS 20535695
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "DRQS 20535695" << bsl::endl
                               << "=============" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_DRQS_20535695;

        typedef btemt_SessionPool::SessionStateCallback SessionCb;
        typedef btemt_SessionPool::SessionPoolStateCallback PoolCb;

        btemt_ChannelPoolConfiguration config;
        config.setMaxThreads(1);

        PoolCb    poolStateCb    = &poolStateCallback;
        SessionCb sessionStateCb = &sessionStateCallback;

        btemt_SessionPool sessionPool(config, poolStateCb, false);

        ASSERT(0 == sessionPool.start());

        bcemt_Barrier barrier(2);
        int           handle;
        TesterFactory sessionFactory(&barrier);
        sessionPool.listen(&handle, sessionStateCb, 0, 1, &sessionFactory);

        bteso_InetStreamSocketFactory<bteso_IPv4Address> socketFactory;
        bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                      socketFactory.allocate();
        const int PORTNUM = sessionPool.portNumber(handle);

        bteso_IPv4Address ADDRESS("127.0.0.1", PORTNUM);

        ASSERT(0 == socket->connect(ADDRESS));

        socket->shutdown(bteso_Flag::BTESO_SHUTDOWN_BOTH);

        socketFactory.deallocate(socket);

        barrier.wait();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // REPRODUCING DRQS 22373213
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "DRQS 22373213" << bsl::endl
                               << "=============" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_DRQS_22373213;

        typedef btemt_SessionPool::SessionStateCallback     SessionCb;
        typedef btemt_SessionPool::SessionPoolStateCallback PoolCb;

        btemt_ChannelPoolConfiguration config;
        config.setMaxThreads(4);

        PoolCb    poolStateCb    = &poolStateCallback;
        SessionCb sessionStateCb = &sessionStateCallback;

        btemt_SessionPool sessionPool(config, poolStateCb, false);

        ASSERT(0 == sessionPool.start());

        bcemt_Barrier barrier(2);
        TesterFactory factory(&barrier);

        int handle;
        ASSERT(0 == sessionPool.listen(&handle,
                                       sessionStateCb,
                                       0,
                                       5,
                                       1,
                                       &factory));
        const int PORTNUM = sessionPool.portNumber(handle);

        const char STRING[] = "Hello World!";

        bteso_IPv4Address ADDRESS("127.0.0.1", PORTNUM);

        bteso_InetStreamSocketFactory<bteso_IPv4Address> socketFactory;
        bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                      socketFactory.allocate();

        ASSERT(0 == socket->connect(ADDRESS));

        barrier.wait();

        CallbackClass cbClass(&barrier);
        btemt_AsyncChannel::BlobBasedReadCallback readFunctor =
                                                   bdef_MemFnUtil::memFn(
                                                        &CallbackClass::readCb,
                                                        &cbClass);

        btemt_AsyncChannel *channel = factory.channel();
        channel->read(1, readFunctor);
        channel->read(2, readFunctor);
        channel->read(3, readFunctor);

        ASSERT(sizeof(STRING) == socket->write(STRING, sizeof(STRING)));

        barrier.wait();

        socketFactory.deallocate(socket);

        LOOP_ASSERT(cbClass.cbCount(), 1 == cbClass.cbCount());
        bcemt_ThreadUtil::sleep(bdet_TimeInterval(5));
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // BLOB BASED USAGE EXAMPLE
        //   The usage example from the header has been changed to use
        //   'bcema_Blob's for reading.
        //
        // Plan:
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "BLOB BASED USAGE EXAMPLE" << bsl::endl
                               << "========================" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_DRQS;

        bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
        bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

        const char STRING[] = "Hello World!";

        bteso_IPv4Address address("127.0.0.1",
                                  bteso_IPv4Address::BTESO_ANY_PORT);

        ASSERT(0 == socket->bind(address));

        ASSERT(0 == socket->localAddress(&address));

        Tester tester(Tester::CONNECTOR, address);

        ASSERT(0 == socket->listen(2));

        if (veryVerbose) {
            MTCOUT << "Bringing down the channel" << MTENDL;
        }

        socket->shutdown(bteso_Flag::BTESO_SHUTDOWN_BOTH);

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(5));

        factory.deallocate(socket);

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(10));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // BLOB BASED USAGE EXAMPLE
        //   The usage example from the header has been changed to use
        //   'bcema_Blob's for reading.
        //
        // Plan:
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "BLOB BASED USAGE EXAMPLE" << bsl::endl
                               << "========================" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_DRQS;

        Tester tester(Tester::LISTENER, bteso_IPv4Address());

        bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
        bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

        const char STRING[] = "Hello World!";

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(1));

        const bteso_IPv4Address ADDRESS("127.0.0.1", tester.portNumber());
        ASSERT(0 == socket->connect(ADDRESS));

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(5));

        if (veryVerbose) {
            MTCOUT << "Bringing down the channel" << MTENDL;
        }

        socket->shutdown(bteso_Flag::BTESO_SHUTDOWN_BOTH);

        factory.deallocate(socket);

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(10));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BLOB BASED USAGE EXAMPLE
        //   The usage example from the header has been changed to use
        //   'bcema_Blob's for reading.
        //
        // Plan:
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "BLOB BASED USAGE EXAMPLE" << bsl::endl
                               << "========================" << bsl::endl;

        using namespace BTEMT_SESSION_BLOB_BASED_EXAMPLE;

        blobBasedUsageExample(&ta);
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST ALLOCATOR PROPAGATION
        //
        // Plan: Install a default allocator, then execute the substance of
        // the usage example, keeping the SessionPool object in-scope after
        // the test is complete.  Then verify that no memory is outstanding
        // through the default allocator.
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "ALLOCATOR PROPAGATION" << bsl::endl
                               << "=====================" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_USAGE_EXAMPLE;
        enum {
            BACKLOG = 5,
            REUSE   = 1
        };

        bcema_TestAllocator da("defaultguard", veryVeryVerbose);
        bslma_DefaultAllocatorGuard defaultAllocGuard(&da);

        my_EchoServer echoServer(&coutMutex, 0, BACKLOG, REUSE, &ta);
        {
            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);
            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

            const char STRING[] = "Hello World!";

            const bteso_IPv4Address ADDRESS("127.0.0.1",
                                            echoServer.portNumber());
            ASSERT(0 == socket->connect(ADDRESS));
            ASSERT(sizeof(STRING) == socket->write(STRING, sizeof(STRING)));

            char readBuffer[sizeof(STRING)];
            ASSERT(sizeof(STRING) == socket->read(readBuffer, sizeof(STRING)));
            ASSERT(0 == bsl::strcmp(readBuffer, STRING));

            factory.deallocate(socket);
        }
        ASSERT(0 != ta.numBytesInUse());
        const int NUM_BYTES = da.numBytesInUse();
        LOOP_ASSERT(NUM_BYTES, 0 == NUM_BYTES);
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //   The usage example from the header has been incorporated into this
        //   test driver.  All references to 'assert' have been replaced with
        //   'ASSERT'.  Call the test example function and assert that it works
        //   as expected.
        //
        // Plan:
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "USAGE EXAMPLE" << bsl::endl
                               << "=============" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_USAGE_EXAMPLE;

        usageExample(&ta);
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Plan:
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

          btemt_ChannelPoolConfiguration config;

          Obj X(config,
                btemt_SessionPool::SessionPoolStateCallback());

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
