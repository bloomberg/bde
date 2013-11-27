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
#include <bteso_socketoptions.h>
#include <bteso_streamsocket.h>

#include <btemt_channelpoolchannel.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;
using namespace bdef_PlaceHolders;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver does nothing, except verify that the usage example can
// compile and run correctly as documented.
//-----------------------------------------------------------------------------
// [13] int setWriteCacheWatermarks(int, int, int);
// [12] int stopAndRemoveAllSessions();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
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

typedef btemt_SessionPool                         Obj;
typedef bteso_SocketOptions                       SocketOptions;
typedef btemt_AsyncChannel::BlobBasedReadCallback BlobReadCallback;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

static bcemt_Mutex coutMutex;

//=============================================================================
//                  SUPPORT CLASSES AND FUNCTIONS USED FOR TESTING
//-----------------------------------------------------------------------------
static
bteso_IPv4Address getLocalAddress() {
    // On Cygwin, binding to bteso_IPv4Address() doesn't seem to work.
    // Wants to bind to localhost/127.0.0.1.  
#ifdef BSLS_PLATFORM_OS_CYGWIN
    return bteso_IPv4Address("127.0.0.1", 0);
#else
    return bteso_IPv4Address();
#endif
}

namespace BTEMT_SESSION_POOL_GENERIC_METHODS {

void poolStateCallback(int reason, int source, void *userData)
{
    if (veryVerbose) {
        MTCOUT << "Pool state changed: (" << reason << ", " << source
               << ") " << MTENDL;
    }
}

void poolStateCallbackWithBarrier(int            state,
                                  int            source,
                                  void          *userData,
                                  int           *poolState,
                                  bcemt_Barrier *barrier)
{
    if (veryVerbose) {
        MTCOUT << "Pool state callback called with"
               << " State: " << state
               << " Source: "  << source << MTENDL;
    }
    *poolState = state;
    barrier->wait();
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

void sessionStateCallbackWithBarrier(int            state,
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

                            // =================
                            // class TestSession
                            // =================


class TestSession : public btemt_Session {
    // This class is a concrete implementation of the 'btemt_Session' protocol
    // to use along with 'Tester' objects.

    // DATA
    bool                d_useBlobReadCb;
    btemt_AsyncChannel *d_channel_p;
    BlobReadCallback   *d_callback_p;

  private:
    // NOT IMPLEMENTED
    TestSession(const TestSession&);
    TestSession& operator=(const TestSession&);

    void blobReadCb(int         result,
                    int        *numNeeded,
                    bcema_Blob *blob,
                    int         channelId);
        // Read callback for session pool.

    void dataMsgReadCb(int                   state,
                       int                  *numConsumed,
                       int                  *numNeeded,
                       const btemt_DataMsg&  msg);

  public:
    // CREATORS
    TestSession(bool                useBlobReadCb,
                btemt_AsyncChannel *channel,
                BlobReadCallback   *callback);
        // Create a new 'TestSession' object for the specified 'channel'.

    ~TestSession();
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

                            // -----------------
                            // class TestSession
                            // -----------------

// PRIVATE MANIPULATORS
void TestSession::blobReadCb(int         result,
                             int        *numNeeded,
                             bcema_Blob *blob,
                             int         channelId)
{
    if (result) {
        // Session is going down.

        d_channel_p->close();
        return;
    }

    ASSERT(0 == d_channel_p->write(*blob));

    blob->removeAll();

    *numNeeded = 1;
}

void TestSession::dataMsgReadCb(int                   state,
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
}

// CREATORS
TestSession::TestSession(bool                useBlobReadCb,
                         btemt_AsyncChannel *channel,
                         BlobReadCallback   *callback)
: d_useBlobReadCb(useBlobReadCb)
, d_channel_p(channel)
, d_callback_p(callback)
{
}

TestSession::~TestSession()
{
}

// MANIPULATORS
int TestSession::start()
{
    if (veryVerbose) {
        MTCOUT << "Session started" << MTENDL;
    }

    if (d_useBlobReadCb) {
        btemt_AsyncChannel::BlobBasedReadCallback callback =
                            d_callback_p
                            ? *d_callback_p
                            : bdef_MemFnUtil::memFn(&TestSession::blobReadCb,
                                                    this);

        d_channel_p->read(1, callback);
    }
    else {
        btemt_AsyncChannel::ReadCallback callback =
                      bdef_MemFnUtil::memFn(&TestSession::dataMsgReadCb, this);

        d_channel_p->read(1, callback);
    }

    return 0;
}

int TestSession::stop()
{
    if (veryVerbose) {
        MTCOUT << "Session stopped" << MTENDL;
    }

    d_channel_p->close();
    return 0;
}

// ACCESSORS
btemt_AsyncChannel *TestSession::channel() const
{
    return d_channel_p;
}

                    // =================
                    // class TestFactory
                    // =================

class TestFactory : public btemt_SessionFactory {
    // This class is a concrete implementation of the 'btemt_SessionFactory'
    // that simply allocates 'TestSession' objects.  No specific allocation
    // strategy (such as pooling) is implemented.

    // DATA
    bool              d_useBlobReadCb; // use blobs
    BlobReadCallback *d_callback_p;    // read callback (held, not owned)
    bslma::Allocator *d_allocator_p;   // memory allocator (held, not owned)

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TestFactory,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    TestFactory(bool              useBlobReadCb = true,
                BlobReadCallback *callback = 0,
                bslma::Allocator *basicAllocator = 0);
        // Create a new 'TestFactory' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    virtual ~TestFactory();
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

                        // -----------------
                        // class TestFactory
                        // -----------------

// CREATORS
TestFactory::TestFactory(bool              useBlobReadCb,
                         BlobReadCallback *callback,
                         bslma::Allocator *basicAllocator)
: d_useBlobReadCb(useBlobReadCb)
, d_callback_p(callback)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TestFactory::~TestFactory()
{
}

// MANIPULATORS
void TestFactory::allocate(btemt_AsyncChannel                    *channel,
                           const btemt_SessionFactory::Callback&  callback)
{
    if (veryVerbose) {
        MTCOUT << "Allocate factory called: " << MTENDL;
    }

    TestSession *session = new (*d_allocator_p) TestSession(d_useBlobReadCb,
                                                            channel,
                                                            d_callback_p);

    callback(0, session);
}

void TestFactory::deallocate(btemt_Session *session)
{
    if (veryVerbose) {
        MTCOUT << "Deallocate factory called: " << MTENDL;
    }

    d_allocator_p->deleteObjectRaw(session);
}

}  // close namespace BTEMT_SESSION_POOL_GENERIC_METHODS

namespace BTEMT_SESSION_POOL_SETTING_SOCKETOPTIONS {

using namespace BTEMT_SESSION_POOL_GENERIC_METHODS;

int createConnection(
                Obj                                          *sessionPool,
                btemt_SessionPool::SessionStateCallback      *sessionStateCb, 
                btemt_SessionFactory                         *sessionFactory,
                bteso_StreamSocket<bteso_IPv4Address>        *serverSocket,
                SocketOptions                                *socketOptions,
                bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                const bteso_IPv4Address                      *ipAddress)
{
    ASSERT(0 == serverSocket->bind(getLocalAddress()));
    ASSERT(0 == serverSocket->listen(1));

    bteso_IPv4Address serverAddr;
    ASSERT(0 == serverSocket->localAddress(&serverAddr));

    int handleBuffer;
    if (socketOptions) {
        return sessionPool->connect(&handleBuffer,
                                    *sessionStateCb,
                                    serverAddr,
                                    1,
                                    bdet_TimeInterval(1),
                                    sessionFactory,
                                    0,
                                    socketOptions,
                                    ipAddress);
    } else {
        BSLS_ASSERT_OPT(socketFactory); // test invariant

        typedef bteso_StreamSocketFactoryDeleter Deleter;

        bdema_ManagedPtr<bteso_StreamSocket<bteso_IPv4Address> >
            clientSocket(socketFactory->allocate(),
                         socketFactory,
                         &Deleter::deleteObject<bteso_IPv4Address>);

        const int rc = clientSocket->bind(*ipAddress);
        if (rc) {
            return rc;                                                // RETURN
        }

        return sessionPool->connect(&handleBuffer,
                                    *sessionStateCb,
                                    serverAddr,
                                    1,
                                    bdet_TimeInterval(1),
                                    &clientSocket,
                                    sessionFactory,
                                    0);
    }
}

}  // close namespace BTEMT_SESSION_POOL_SETTING_SOCKETOPTIONS


namespace BTEMT_SESSION_POOL_STOPANDREMOVEALLCHANNELS {

using namespace BTEMT_SESSION_POOL_GENERIC_METHODS;

bcemt_Mutex        mapMutex;
bsl::map<int, btemt_AsyncChannel *> sourceIdToChannelMap;
typedef bsl::map<int, btemt_AsyncChannel *>::iterator MapIter;

const int NT = 10;

bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *> clientSockets(NT);
bteso_InetStreamSocketFactory<bteso_IPv4Address>     factory;

struct ConnectData {
    int               d_index;
    int               d_numBytes;
    bteso_IPv4Address d_serverAddress;
};

void *connectFunction(void *args)
{
    ConnectData             data      = *(const ConnectData *) args;
    const int               INDEX     = data.d_index;
    const int               NUM_BYTES = data.d_numBytes;
    const bteso_IPv4Address ADDRESS   = data.d_serverAddress;

    clientSockets[INDEX] = factory.allocate();

    ASSERT(0 == clientSockets[INDEX]->connect(ADDRESS));

    bsl::vector<char> buffer(NUM_BYTES);

    int numRemaining = NUM_BYTES;
    do {
        int rc = clientSockets[INDEX]->read(buffer.data(), numRemaining);
        if (rc != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK) {
            numRemaining -= rc;
        }

        rc = clientSockets[INDEX]->write(buffer.data(), numRemaining);

        bcemt_ThreadUtil::microSleep(1000 , 0);
    } while (numRemaining > 0);
    return 0;
}

bsl::vector<bteso_StreamSocket<bteso_IPv4Address> *> serverSockets(NT);

struct ListenData {
    int d_index;
    int d_numBytes;
};

void *listenFunction(void *args)
{
    ListenData data      = *(const ListenData *) args;
    const int  INDEX     = data.d_index;
    const int  NUM_BYTES = data.d_numBytes;

    serverSockets[INDEX] = factory.allocate();

    ASSERT(0 == serverSockets[INDEX]->bind(getLocalAddress()));
    ASSERT(0 == serverSockets[INDEX]->listen(1));

    bteso_StreamSocket<bteso_IPv4Address> *client;
    ASSERT(!serverSockets[INDEX]->accept(&client));
    ASSERT(0 == client->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE));

    bsl::vector<char> buffer(NUM_BYTES);

    int numRemaining = NUM_BYTES;
    do {
        int rc = client->read(buffer.data(), numRemaining);
        if (rc != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK) {
            numRemaining -= rc;
        }

        rc = client->write(buffer.data(), numRemaining);

        bcemt_ThreadUtil::microSleep(1000 , 0);
    } while (numRemaining > 0);
    return 0;
}

}  // end namespace BTEMT_SESSION_POOL_TESTCASE_12


namespace BTEMT_SESSION_POOL_DRQS_28731692 {

using namespace BTEMT_SESSION_POOL_GENERIC_METHODS;

void readCbWithBlob(int         result,
                    int        *numNeeded,
                    bcema_Blob *data,
                    int         channelId,
                    bcema_Blob *blob)
{
    if (result) {
        // Session is going down.

        return;
    }

    ASSERT(numNeeded);
    ASSERT(data);
    ASSERT(0 < data->length());

    blob->moveAndAppendDataBuffers(data);

    *numNeeded = 1;
}

}

namespace BTEMT_SESSION_POOL_DRQS_29067989 {

using namespace BTEMT_SESSION_POOL_GENERIC_METHODS;

static bslma::TestAllocator testAllocator;
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

void readCbWithMetrics(int         result,
                       int        *numNeeded,
                       bcema_Blob *blob,
                       int         channelId)
{
    if (result) {
        // Session is going down.

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

}

namespace BTEMT_SESSION_POOL_DRQS_20535695 {

using namespace BTEMT_SESSION_POOL_GENERIC_METHODS;

                    // ========================
                    // class TestSessionFactory
                    // ========================

class TestSessionFactory : public btemt_SessionFactory {
    // This class is a concrete implementation of the 'btemt_SessionFactory'
    // that simply allocates 'TestSession' objects.  No specific allocation
    // strategy (such as pooling) is implemented.

    // DATA
    bcemt_Barrier *d_barrier_p;

  public:
    // CREATORS
    TestSessionFactory(bcemt_Barrier *barrier);
        // Create a new 'TestSessionFactory' object using the specified
        // 'barrier'.

    virtual ~TestSessionFactory();
        // Destroy this factory.

    // MANIPULATORS
    virtual void allocate(btemt_AsyncChannel                   *channel,
                          const btemt_SessionFactory::Callback& callback);
        // Asynchronously allocate a 'btemt_Session' object for the
        // specified 'channel', and invoke the specified 'callback' with
        // this session.

    virtual void deallocate(btemt_Session *session);
        // Deallocate the specified 'session'.

    btemt_AsyncChannel *channel() const;
        // Return the channel managed by this factory.
};

                        // ------------------------
                        // class TestSessionFactory
                        // ------------------------

// CREATORS
TestSessionFactory::TestSessionFactory(bcemt_Barrier *barrier)
: d_barrier_p(barrier)
{
}

TestSessionFactory::~TestSessionFactory()
{
}

// MANIPULATORS
void TestSessionFactory::allocate(
                               btemt_AsyncChannel                    *channel,
                               const btemt_SessionFactory::Callback&  callback)
{
    if (veryVerbose) {
        MTCOUT << "Allocate factory called: " << MTENDL;
    }

    d_barrier_p->wait();
}

void TestSessionFactory::deallocate(btemt_Session *session)
{
    if (veryVerbose) {
        MTCOUT << "Deallocate factory called: " << MTENDL;
    }
}

btemt_AsyncChannel *TestSessionFactory::channel() const
{
    return 0;
}

}

namespace BTEMT_SESSION_POOL_DRQS_22373213 {

using namespace BTEMT_SESSION_POOL_GENERIC_METHODS;

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

                    // ========================
                    // class TestSessionFactory
                    // ========================

class TestSessionFactory : public btemt_SessionFactory {
    // This class is a concrete implementation of the 'btemt_SessionFactory'
    // that simply allocates 'TestSession' objects.  No specific allocation
    // strategy (such as pooling) is implemented.

    // DATA
    btemt_SessionFactory::Callback  d_callback;
    btemt_Session                  *d_session_p;
    bcemt_Barrier                  *d_barrier_p;
    bslma::Allocator               *d_allocator_p;  // memory allocator (held,
                                                    // not owned)

    void readCb(int         state,
                int        *numNeeded,
                bcema_Blob *msg,
                int         channelId);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TestSessionFactory,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    TestSessionFactory(bcemt_Barrier    *barrier,
                       bslma::Allocator *basicAllocator = 0);
        // Create a new 'TestSessionFactory' object using the specified
        // 'barrier'.  Optionally specify 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    virtual ~TestSessionFactory();
        // Destroy this factory.

    // MANIPULATORS
    virtual void allocate(btemt_AsyncChannel                   *channel,
                          const btemt_SessionFactory::Callback& callback);
        // Asynchronously allocate a 'btemt_Session' object for the
        // specified 'channel', and invoke the specified 'callback' with
        // this session.

    virtual void deallocate(btemt_Session *session);
        // Deallocate the specified 'session'.

    btemt_AsyncChannel *channel() const;
        // Return the channel managed by this factory.
};

                        // ------------------------
                        // class TestSessionFactory
                        // ------------------------

// CREATORS
TestSessionFactory::TestSessionFactory(bcemt_Barrier    *barrier,
                                       bslma::Allocator *basicAllocator)
: d_session_p(0)
, d_barrier_p(barrier)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TestSessionFactory::~TestSessionFactory()
{
}

// MANIPULATORS
void TestSessionFactory::allocate(
                             btemt_AsyncChannel                    *channel,
                             const btemt_SessionFactory::Callback&  callback)
{
    if (veryVerbose) {
        MTCOUT << "Allocate factory called: " << MTENDL;
    }

    d_session_p = new (*d_allocator_p) TestSession(true, channel, 0);

    d_barrier_p->wait();

    callback(0, d_session_p);
}

void TestSessionFactory::deallocate(btemt_Session *session)
{
    if (veryVerbose) {
        MTCOUT << "Deallocate factory called: " << MTENDL;
    }

    d_allocator_p->deleteObjectRaw(session);
}

void TestSessionFactory::readCb(int         state,
                                int        *numNeeded,
                                bcema_Blob *msg,
                                int         channelId)
{
    if (veryVerbose) {
        MTCOUT << "Read callback called with: " << state << MTENDL;
    }

    d_callback(0, d_session_p);
}

btemt_AsyncChannel *TestSessionFactory::channel() const
{
    return d_session_p->channel();
}

}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

        bslma::Allocator *d_allocator_p; // memory allocator (held, not owned)

      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(my_EchoSessionFactory,
                                     bslalg::TypeTraitUsesBslmaAllocator);

        // CREATORS
        my_EchoSessionFactory(bslma::Allocator *basicAllocator = 0);
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
                                              bslma::Allocator *basicAllocator)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
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

        bslma::Allocator              *d_allocator_p;     // memory allocator
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
                                     bslalg::TypeTraitUsesBslmaAllocator);

        // CREATORS
        my_EchoServer(bcemt_Mutex      *coutLock,
                      int               portNumber,
                      int               numConnections,
                      bool              reuseAddressFlag,
                      bslma::Allocator *basicAllocator = 0);
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
    my_EchoServer::my_EchoServer(bcemt_Mutex      *lock,
                                 int               portNumber,
                                 int               numConnections,
                                 bool              reuseAddressFlag,
                                 bslma::Allocator *basicAllocator)
    : d_sessionFactory(basicAllocator)
    , d_coutLock_p(lock)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
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
    int usageExample(bslma::Allocator *allocator) {

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
      case 13: {
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
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'connect' with socket options
        //   Ensure that the 'connect' that takes a socket option object
        //   returns a session up callback on success and user callback on
        //   error.
        //
        // Concerns:
        //   1. The session state callback is called with success if setting of
        //      the socket options succeeds.
        //
        //   2. A user callback is invoked on encountering an asynchronous
        //      failure while setting the socket options.
        //
        // Plan:
        //   1. For concern 1, invoke 'connect' with a socket option expected
        //      to succeed.  Verify that a session state callback is invoked
        //      informing the user of connect success.
        //
        //   1. For concern 2, invoke 'connect' with a socket option expected
        //      to fail.  Verify that a pool state callback with is invoked
        //      informing the user of the connect error.
        //
        // Testing:
        //   int connect(host, ...., *socketOptions);
        //   int connect(serverAddr, ...., *socketOptions);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TESTING 'connect' with socket options"
                               << bsl::endl
                               << "====================================="
                               << bsl::endl;

        using namespace BTEMT_SESSION_POOL_SETTING_SOCKETOPTIONS;

        if (verbose) cout << "connect(IPv4Address...) with socket options"
                          << endl;
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);

            typedef btemt_SessionPool::SessionPoolStateCallback PoolCb;

            bcemt_Barrier   channelCbBarrier(2);
            btemt_SessionPool::SessionStateCallback sessionStateCb(
                          bdef_BindUtil::bind(&sessionStateCallbackWithBarrier,
                                              _1, _2, _3, _4,
                                              &channelCbBarrier));

            int             poolState;
            bcemt_Barrier   poolCbBarrier(2);
            btemt_SessionPool::SessionPoolStateCallback
                 poolStateCb(bdef_BindUtil::bind(&poolStateCallbackWithBarrier,
                                                 _1, _2, _3,
                                                 &poolState,
                                                 &poolCbBarrier));

            TestFactory sessionFactory;

            btemt_SessionPool pool(config, poolStateCb, false);

            ASSERT(0 == pool.start());

            bteso_InetStreamSocketFactory<bteso_IPv4Address> socketFactory;

            {

                typedef bteso_StreamSocketFactoryDeleter Deleter;

                bdema_ManagedPtr<bteso_StreamSocket<bteso_IPv4Address> >
                    socket(socketFactory.allocate(), 
                           &socketFactory,
                           &Deleter::deleteObject<bteso_IPv4Address>);

                SocketOptions opt;  opt.setKeepAlive(true); // always succeeds 
                const int rc = createConnection(&pool,
                                                &sessionStateCb,
                                                &sessionFactory,
                                                socket.ptr(),
                                                &opt, 0, 0);
                ASSERT(!rc);

                channelCbBarrier.wait();
            }

            {

                typedef bteso_StreamSocketFactoryDeleter Deleter;

                bdema_ManagedPtr<bteso_StreamSocket<bteso_IPv4Address> >
                    socket(socketFactory.allocate(), 
                           &socketFactory,
                           &Deleter::deleteObject<bteso_IPv4Address>);
                ASSERT(socket);

                SocketOptions opt;  opt.setSendTimeout(1); // fails on all
                                                           // platforms 
                const int rc = createConnection(&pool,
                                                &sessionStateCb,
                                                &sessionFactory,
                                                socket.ptr(),
                                                &opt, 0, 0);
                ASSERT(!rc);

                poolCbBarrier.wait();
                ASSERT(btemt_SessionPool::CONNECT_FAILED == poolState);
            }

            ASSERT(0 == pool.stopAndRemoveAllSessions());
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'connect' with a user-specified local address
        //   Ensure that the 'connect' that takes a client address returns a
        //   session up callback on success and user callback on error.
        //
        // Concerns:
        //   1. The session state callback is called with success if binding
        //      to the local address succeeds.
        //
        //   2. A user callback is invoked on encountering an asynchronous
        //      failure while binding to a provided local address.
        //
        // Plan:
        //   1. For concern 1, invoke 'connect' with a good IP address.  Verify
        //      that a session state callback is invoked informing the user of
        //      connect success.
        //
        //   2. For concern 2, invoke 'connect' with a bad IP address.  Verify
        //      that a pool state callback is invoked informing the user of
        //      the connect error.
        //
        // Testing:
        //   int connect(host, ...., *socketOptions, *clientAddr);
        //   int connect(serverAddr, ...., *socketOptions, *clientAddr);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TESTING 'connect' with a local address"
                               << bsl::endl
                               << "======================================"
                               << bsl::endl;

        using namespace BTEMT_SESSION_POOL_SETTING_SOCKETOPTIONS;

        if (verbose) cout << "connect(IPv4Address...) with client address"
                          << endl;
        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(1);

            typedef btemt_SessionPool::SessionPoolStateCallback PoolCb;

            bcemt_Barrier   channelCbBarrier(2);
            btemt_SessionPool::SessionStateCallback sessionStateCb(
                          bdef_BindUtil::bind(&sessionStateCallbackWithBarrier,
                                              _1, _2, _3, _4,
                                              &channelCbBarrier));

            int             poolState;
            bcemt_Barrier   poolCbBarrier(2);
            btemt_SessionPool::SessionPoolStateCallback
                 poolStateCb(bdef_BindUtil::bind(&poolStateCallbackWithBarrier,
                                                 _1, _2, _3,
                                                 &poolState,
                                                 &poolCbBarrier));

            TestFactory sessionFactory;

            btemt_SessionPool pool(config, poolStateCb, false);

            ASSERT(0 == pool.start());

            bteso_InetStreamSocketFactory<bteso_IPv4Address> socketFactory;

            {

                typedef bteso_StreamSocketFactoryDeleter Deleter;

                bdema_ManagedPtr<bteso_StreamSocket<bteso_IPv4Address> >
                    socket(socketFactory.allocate(), 
                           &socketFactory,
                           &Deleter::deleteObject<bteso_IPv4Address>);

                bteso_IPv4Address address("127.0.0.1", 45000); // good address
                const int rc = createConnection(&pool,
                                                &sessionStateCb,
                                                &sessionFactory,
                                                socket.ptr(),
                                                0, &socketFactory,
                                                &address);
                ASSERT(!rc);

                if (veryVerbose) {
                    MTCOUT << "Waiting on channel barrier..." << MTENDL;
                }
                channelCbBarrier.wait();
            }

            {

                typedef bteso_StreamSocketFactoryDeleter Deleter;

                bdema_ManagedPtr<bteso_StreamSocket<bteso_IPv4Address> >
                    socket(socketFactory.allocate(), 
                           &socketFactory,
                           &Deleter::deleteObject<bteso_IPv4Address>);

                bteso_IPv4Address address("1.1.1.1", 45000);  // bad address
                const int rc = createConnection(&pool,
                                                &sessionStateCb,
                                                &sessionFactory,
                                                socket.ptr(),
                                                0, &socketFactory,
                                                &address);
                if (!rc) {
                    if (veryVerbose) {
                        MTCOUT << "Waiting on pool barrier..." << MTENDL;
                    }
                    poolCbBarrier.wait();
                    ASSERT(btemt_SessionPool::CONNECT_FAILED == poolState);
                }
            }

            ASSERT(0 == pool.stopAndRemoveAllSessions());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'setWriteCacheWatermarks'
        //   The 'setWriteCacheWatermarks' method has the expected effect.
        //
        // Concerns:
        //   1. That 'setWriteCacheWatermarks' fails when passed an invalid
        //      session id.
        //
        //   2. That 'setWriteCacheWatermarks' correctly passes its arguments
        //      to btemt_ChannelPool::setWriteCacheWatermarks'.
        //
        // Plan:
        //   1. For concern 1, invoke 'setWriteCacheWatermarks' with an invalid
        //      session id and verify that the method fails.
        //
        //   2. For concern 2, create a session, capturing the session id of a
        //      client connection.  Invoke the method using that session id and
        //      representative (valid) values for the low- and high-water
        //      marks.  Assert that the method succeeds in each case.
        //
        // Testing:
        //   int setWriteCacheWatermarks(int, int, int);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TESTING 'setWriteCacheWatermarks'"
                               << bsl::endl
                               << "================================="
                               << bsl::endl;

        using namespace BTEMT_SESSION_POOL_GENERIC_METHODS;

        enum {
            LOW_WATERMARK =  512,
            HI_WATERMARK  = 4096
        };

        typedef btemt_SessionPool::SessionStateCallback     SessionCb;
        typedef btemt_SessionPool::SessionPoolStateCallback PoolCb;

        btemt_ChannelPoolConfiguration config;
        config.setMaxThreads(4);
        config.setWriteCacheWatermarks(LOW_WATERMARK, HI_WATERMARK);

        PoolCb    poolStateCb    = &poolStateCallback;
        SessionCb sessionStateCb = &sessionStateCallback;

        btemt_SessionPool sessionPool(config, poolStateCb, false);

        ASSERT(0 == sessionPool.start());

        TestFactory factory;

        int handle = 0;
        ASSERT(0 == sessionPool.listen(&handle,
                                       sessionStateCb,
                                       0,
                                       5,
                                       1,
                                       &factory));
        const int PORTNUM = sessionPool.portNumber(handle);

        bteso_IPv4Address ADDRESS("127.0.0.1", PORTNUM);

        ASSERT(0 == sessionPool.connect(&handle,
                                        sessionStateCb,
                                        ADDRESS,
                                        5,
                                        bdet_TimeInterval(1),
                                        &factory));

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(2));

        ASSERT(0 != sessionPool.setWriteCacheWatermarks(handle + 666,
                                                        LOW_WATERMARK + 1,
                                                        HI_WATERMARK - 1));

        ASSERT(0 == sessionPool.setWriteCacheWatermarks(handle,
                                                     LOW_WATERMARK,
                                                     HI_WATERMARK));
        ASSERT(0 == sessionPool.setWriteCacheWatermarks(handle,
                                                     LOW_WATERMARK,
                                                     LOW_WATERMARK));

        sessionPool.setWriteCacheWatermarks(handle,
                                            LOW_WATERMARK, HI_WATERMARK);
        ASSERT(0 == sessionPool.setWriteCacheWatermarks(handle,
                                                        HI_WATERMARK,
                                                        HI_WATERMARK));

        sessionPool.setWriteCacheWatermarks(handle,
                                            LOW_WATERMARK, HI_WATERMARK);
        ASSERT(0 == sessionPool.setWriteCacheWatermarks(handle,
                                                        LOW_WATERMARK - 2,
                                                        LOW_WATERMARK));

        sessionPool.setWriteCacheWatermarks(handle,
                                            LOW_WATERMARK, HI_WATERMARK);
        ASSERT(0 == sessionPool.setWriteCacheWatermarks(handle,
                                                        LOW_WATERMARK - 2,
                                                        LOW_WATERMARK - 1));

        sessionPool.setWriteCacheWatermarks(handle,
                                            LOW_WATERMARK, HI_WATERMARK);
        ASSERT(0 == sessionPool.setWriteCacheWatermarks(handle,
                                                        HI_WATERMARK,
                                                        HI_WATERMARK + 2));

        sessionPool.setWriteCacheWatermarks(handle,
                                            LOW_WATERMARK, HI_WATERMARK);
        ASSERT(0 == sessionPool.setWriteCacheWatermarks(handle,
                                                        HI_WATERMARK + 1,
                                                        HI_WATERMARK + 2));

        ASSERT(0 == sessionPool.stop());

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'stopAndRemoveAllSessions'
        //
        // Concerns
        //: 1 Session pool stops all threads and removes all sessions when
        //:   this function is called.
        //:
        //: 2 Any resources associated with any session is released.
        //
        // Plan:
        //: 1 Create a session pool object, mX.
        //:
        //: 2 Open a pre-defined number of listening sockets in mX.
        //:
        //: 3 Create a number of threads that each connect to one of the
        //:   listening sockets in mX.
        //:
        //: 4 Create a number of threads that each listen on a socket.
        //:
        //: 5 Open a session in mX by connecting a listening sockets created
        //:   in step 4.
        //:
        //: 6 Write large amount of data through mX across all the open
        //:   channels.
        //:
        //: 7 Invoke 'stopAndRemoveAllSessions' and confirm that no sessions
        //:   are outstanding.
        //
        // Testing:
        //   int stopAndRemoveAllSessions();
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TESTING 'stopAndRemoveAllChannels'"
                               << bsl::endl
                               << "=================================="
                               << bsl::endl;

        using namespace BTEMT_SESSION_POOL_STOPANDREMOVEALLCHANNELS;

        typedef btemt_SessionPool::SessionStateCallback SessionCb;
        typedef btemt_SessionPool::SessionPoolStateCallback PoolCb;

        const int NUM_BYTES = 1024 * 1024 * 10;
        btemt_ChannelPoolConfiguration config;
        config.setMaxThreads(NT);
        config.setWriteCacheWatermarks(0, NUM_BYTES * 10);  // 1Mb

        PoolCb poolStateCb(&poolStateCallback);
        SessionCb callback(&sessionStateCallback);

        btemt_SessionPool mX(config, poolStateCb, true);
        const btemt_SessionPool& X = mX;

        ASSERT(0 == mX.start());

        bsl::vector<int> serverHandles(NT);
        TestFactory      sessionFactory;
        for (int i = 0; i < NT; ++i) {
            ASSERT(0 == mX.listen(&serverHandles[i],
                                  callback,
                                  0,
                                  5,
                                  &sessionFactory));
        }

        bcemt_ThreadUtil::microSleep(0, 2);

        bcemt_ThreadUtil::Handle connectThreads[NT];
        ConnectData              connectData[NT];
        const int                SIZE = 1024 * 1024; // 1 MB

        for (int i = 0; i < NT; ++i) {
            connectData[i].d_index    = i;
            connectData[i].d_numBytes = SIZE;
            const int PORTNUM = X.portNumber(serverHandles[i]);
            connectData[i].d_serverAddress = bteso_IPv4Address("127.0.0.1",
                                                               PORTNUM);

            ASSERT(0 == bcemt_ThreadUtil::create(&connectThreads[i],
                                                 &connectFunction,
                                                 (void *) &connectData[i]));
        }

        bcemt_ThreadUtil::microSleep(0, 5);

        bcemt_ThreadUtil::Handle listenThreads[NT];
        ListenData               listenData[NT];
        for (int i = 0; i < NT; ++i) {
            listenData[i].d_index    = i;
            listenData[i].d_numBytes = SIZE;

            ASSERT(0 == bcemt_ThreadUtil::create(&listenThreads[i],
                                                 &listenFunction,
                                                 (void *) &listenData[i]));
        }

        bcemt_ThreadUtil::microSleep(0, 5);

        bsl::vector<int> clientHandles(NT);
        for (int i = 0; i < NT; ++i) {
            bteso_IPv4Address serverAddr;
            ASSERT(0 == serverSockets[i]->localAddress(&serverAddr));

            ASSERT(0 == mX.connect(&clientHandles[i],
                                   callback,
                                   serverAddr,
                                   10,
                                   bdet_TimeInterval(1),
                                   &sessionFactory));
        }

        bcemt_ThreadUtil::microSleep(0, 2);

        bcema_PooledBlobBufferFactory f(SIZE);
        bcema_Blob                    b(&f);
        b.setLength(NUM_BYTES);

        mapMutex.lock();
        for (int i = 0; i < NT; ++i) {
            MapIter iter = sourceIdToChannelMap.find(serverHandles[i]);
            if (iter != sourceIdToChannelMap.end()) {
                ASSERT(0 == iter->second->write(b));
            }

            iter = sourceIdToChannelMap.find(clientHandles[i]);
            if (iter != sourceIdToChannelMap.end()) {
                ASSERT(0 == iter->second->write(b));
            }
        }
        mapMutex.unlock();

        ASSERT(0 != mX.numSessions());

        ASSERT(!mX.stopAndRemoveAllSessions());

        ASSERT(0 == mX.numSessions());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // REPRODUCING DRQS 28731692
        //  Ensure that when session pool owns factories used for allocating
        //  read buffers.
        //
        // Concerns
        //: 1 Session pool owns the factories that are used to allocate the
        //:   buffers provided to clients in the data callback.
        //
        // Plan:
        //: 1 Create a bcema_Blob that stores the data returned in the data
        //:   callback.
        //:
        //: 2 Create a session pool object, mX, designed to use pooled buffer
        //:   chains and listen on a port on the local machine.
        //:
        //: 3 Create a socket and 'connect' to the port number on which the
        //:   session pool is listening.
        //:
        //: 4 Write data on the socket.
        //:
        //: 5 Destroy the session pool.
        //:
        //: 6 Destroy the blob.
        //
        // Testing:
        //  DRQS 28731692
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "DRQS 28731692" << bsl::endl
                               << "=============" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_DRQS_28731692;

        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(4);

            typedef btemt_SessionPool::SessionStateCallback     SessionCb;
            typedef btemt_SessionPool::SessionPoolStateCallback PoolCb;

            PoolCb    poolCb    = &poolStateCallback;
            SessionCb sessionCb = &sessionStateCallback;

            bslma::TestAllocator ta1, ta2;
            btemt_SessionPool sessionPool(config, poolCb, false, &ta1);

            ASSERT(0 == sessionPool.start());

            bcema_Blob blob;
            BlobReadCallback callback(bdef_BindUtil::bind(&readCbWithBlob,
                                                          _1,
                                                          _2,
                                                          _3,
                                                          _4,
                                                          &blob));
            TestFactory factory(true, &callback, &ta2);

            int handle = 0;
            ASSERT(0 == sessionPool.listen(&handle,
                                           sessionCb,
                                           0,
                                           5,
                                           &factory));
            const int PORTNUM = sessionPool.portNumber(handle);
            {
                bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
                bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                            factory.allocate();

                const bteso_IPv4Address ADDRESS("127.0.0.1", PORTNUM);
                int rc = socket->connect(ADDRESS);
                ASSERT(!rc);

                const int PAYLOAD_SIZE = 320;
                char payload[PAYLOAD_SIZE];
                bsl::memset(payload, 'Z', PAYLOAD_SIZE);
                const int NT = 1;

                for (int i = 0; i < NT; ++i) {
                    socket->write(payload, PAYLOAD_SIZE);
                }

                factory.deallocate(socket);
            }

            bcemt_ThreadUtil::microSleep(0, 3);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // REPRODUCING DRQS 29067989
        //  Ensure that session pool does not allocate and hold on to
        //  additional memory buffers when clients read all the data.
        //
        // Concerns
        //: 1 Session pool does not allocate and hold on to memory buffers
        //:   indefinitely even if the client is reading all the data.
        //
        // Plan:
        //: 1 Create a session pool object, mX, and listen on a port on the
        //:   local machine.
        //:
        //: 2 Create a socket and 'connect' to the port number on which the
        //:   session pool is listening.
        //:
        //: 3 Write data on the socket in multiple attempts and monitor the
        //:   size and length of the blob returned by session pool in the data
        //:   callback.
        //:
        //: 4 Verify that the blob provided in the data callback does not
        //:   unnecessarily hoard memory.
        //
        // Testing:
        //  DRQS 29067989
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "DRQS 29067989" << bsl::endl
                               << "=============" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_DRQS_29067989;

        {
            btemt_ChannelPoolConfiguration config;
            config.setMaxThreads(4);

            bslma::TestAllocator ta;
            btemt_SessionPool sessionPool(config,
                                          &poolStateCallback,
                                          true,
                                          &ta);

            ASSERT(0 == sessionPool.start());

            BlobReadCallback callback(&readCbWithMetrics);

            TestFactory sessionFactory(true, &callback, &ta);

            int handle = 0;
            ASSERT(0 == sessionPool.listen(&handle,
                                           &sessionStateCallback,
                                           0,
                                           5,
                                           &sessionFactory));

            const int PORTNUM = sessionPool.portNumber(handle);

            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

            const bteso_IPv4Address ADDRESS("127.0.0.1", PORTNUM);
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
                MTCOUT << "TA In Use: " << testAllocator.numBytesInUse()
                       << MTENDL;
                MTCOUT << "TA In Use Blocks: "
                       << testAllocator.numBlocksInUse()
                       << MTENDL;
                MTCOUT << "TA In Max: " << testAllocator.numBytesMax()
                       << MTENDL;
                MTCOUT << "maxLength: " << maxLength << MTENDL;
                MTCOUT << "maxSize: " << maxSize << MTENDL;
                MTCOUT << "maxExtra: " << maxExtra << MTENDL;
                MTCOUT << "maxNumBuffers: " << maxNumBuffers << MTENDL;
                MTCOUT << "maxNumDataBuffers: " << maxNumDataBuffers << MTENDL;
            }

            // TBD:
            bcemt_ThreadUtil::microSleep(0, 1);
        }
      } break;
      case 6: {
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

        using namespace BTEMT_SESSION_POOL_GENERIC_METHODS;

        typedef btemt_SessionPool::SessionStateCallback     SessionCb;
        typedef btemt_SessionPool::SessionPoolStateCallback PoolCb;

        const int NUM_SESSIONS = 2;
        btemt_ChannelPoolConfiguration config;
        config.setMaxThreads(NUM_SESSIONS);
        config.setMaxConnections(NUM_SESSIONS);

        PoolCb poolStateCb(&poolStateCallback);
        bcemt_Barrier barrier(NUM_SESSIONS + 1);
        SessionCb callback(bdef_BindUtil::bind(
                                              &sessionStateCallbackWithBarrier,
                                              _1, _2, _3, _4,
                                              &barrier));

        btemt_SessionPool mX(config, poolStateCb, false);
        const btemt_SessionPool& X = mX;

        ASSERT(0 == mX.start());

        int           handle;
        TestFactory sessionFactory;
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
      case 5: {
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
        TestSessionFactory sessionFactory(&barrier);
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
      case 4: {
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

        btemt_ChannelPoolConfiguration config;
        config.setMaxThreads(4);

        int           poolState;
        bcemt_Barrier barrier(2);

        TestSessionFactory factory(&barrier);

        typedef btemt_SessionPool::SessionPoolStateCallback SessionPoolStateCb;
        SessionPoolStateCb poolCb(bdef_BindUtil::bind(
                                                 &poolStateCallbackWithBarrier,
                                                 _1,
                                                 _2,
                                                 _3,
                                                 &poolState,
                                                 &barrier));

        btemt_SessionPool sessionPool(config, poolCb, false);

        int rc = sessionPool.start();
        ASSERT(0 == rc);

        int handle;
        rc = sessionPool.listen(&handle,
                                &sessionStateCallback,
                                0,
                                5,
                                1,
                                &factory);
        ASSERT(0 == rc);

        const int PORTNUM = sessionPool.portNumber(handle);

        const char STRING[] = "Hello World!";

        bteso_IPv4Address ADDRESS("127.0.0.1", PORTNUM);

        bteso_InetStreamSocketFactory<bteso_IPv4Address> socketFactory;
        bteso_StreamSocket<bteso_IPv4Address> *socket =
                                                      socketFactory.allocate();

        rc = socket->connect(ADDRESS);
        ASSERT(0 == rc);

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

        rc = socket->write(STRING, sizeof(STRING));
        ASSERT(sizeof(STRING) == rc);

        barrier.wait();

        socketFactory.deallocate(socket);

        LOOP_ASSERT(cbClass.cbCount(), 1 == cbClass.cbCount());
      } break;
      case 3: {
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

        using namespace BTEMT_SESSION_POOL_GENERIC_METHODS;

        bcema_TestAllocator da("defaultguard", veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

        TestFactory factory(true, 0, &ta);

        btemt_ChannelPoolConfiguration config;

        typedef btemt_SessionPool::SessionPoolStateCallback SessionPoolStateCb;
        typedef btemt_SessionPool::SessionStateCallback     SessionStateCb;

        SessionPoolStateCb poolStateCb    = &poolStateCallback;
        SessionStateCb     sessionStateCb = &sessionStateCallback;

        btemt_SessionPool sessionPool(config, poolStateCb, true, &ta);
        int rc = sessionPool.start();
        ASSERT(0 == rc);

        int handle;
        rc = sessionPool.listen(&handle,
                                sessionStateCb,
                                0,
                                5,
                                false,
                                &factory);
        ASSERT(0 == rc);

        {
            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);
            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

            const char STRING[] = "Hello World!";

            const bteso_IPv4Address ADDRESS("127.0.0.1",
                                            sessionPool.portNumber(handle));
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
        // TEST ALLOCATOR PROPAGATION
        //
        // Plan: Install a default allocator, then execute the substance of
        // the usage example, keeping the SessionPool object in-scope after
        // the test is complete.  Then verify that no memory is outstanding
        // through the default allocator.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "ALLOCATOR PROPAGATION" << bsl::endl
                               << "=====================" << bsl::endl;

        using namespace BTEMT_SESSION_POOL_GENERIC_METHODS;

        bcema_TestAllocator da("defaultguard", veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

        TestFactory factory(false, 0, &ta);

        btemt_ChannelPoolConfiguration config;

        typedef btemt_SessionPool::SessionPoolStateCallback SessionPoolStateCb;
        typedef btemt_SessionPool::SessionStateCallback     SessionStateCb;

        SessionPoolStateCb poolStateCb    = &poolStateCallback;
        SessionStateCb     sessionStateCb = &sessionStateCallback;

        btemt_SessionPool sessionPool(config, poolStateCb, false, &ta);
        int rc = sessionPool.start();
        ASSERT(0 == rc);

        int handle;
        rc = sessionPool.listen(&handle,
                                sessionStateCb,
                                0,
                                5,
                                false,
                                &factory);
        ASSERT(0 == rc);

        {
            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);
            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

            const char STRING[] = "Hello World!";

            const bteso_IPv4Address ADDRESS("127.0.0.1",
                                            sessionPool.portNumber(handle));
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

          Obj X(config, btemt_SessionPool::SessionPoolStateCallback());
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
