// btemt_sessionpool.t.cpp         -*-C++-*-
#include <btemt_sessionpool.h>

#include <btemt_asyncchannel.h>
#include <btemt_channelpool.h>
#include <btemt_message.h>
#include <btemt_session.h>

#include <bcema_blobutil.h>
#include <bcema_testallocator.h>
#include <bcemt_thread.h>

#include <bdef_function.h>
#include <bdef_memfn.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bteso_ipv4address.h>
#include <bteso_inetstreamsocketfactory.h>
#include <bteso_streamsocket.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <cstdlib>     // atoi()

using namespace BloombergLP;


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
//                  STANDARD BDEX EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEX_EXCEPTION_TEST                                           \
{                                                                           \
    {                                                                       \
        static int firstTime = 1;                                           \
        if (veryVerbose && firstTime)  cout <<                              \
            "### BDEX EXCEPTION TEST -- (ENABLED) --" << endl;              \
        firstTime = 0;                                                      \
    }                                                                       \
    if (veryVeryVerbose) cout <<                                            \
        "### Begin bdex exception test." << endl;                           \
    int bdexExceptionCounter = 0;                                           \
    static int bdexExceptionLimit = 100;                                    \
    testInStream.setInputLimit(bdexExceptionCounter);                       \
    do {                                                                    \
        try {

#define END_BDEX_EXCEPTION_TEST                                             \
        } catch (bdex_TestInStreamException& e) {                           \
            if (veryVerbose && bdexExceptionLimit || veryVeryVerbose)       \
            {                                                               \
                --bdexExceptionLimit;                                       \
                cout << "(" <<                                              \
                bdexExceptionCounter << ')';                                \
                if (veryVeryVerbose) { cout << " BDEX_EXCEPTION: "          \
                    << "input limit = " << bdexExceptionCounter << ", "     \
                    << "last data type = " << e.dataType();                 \
                }                                                           \
                else if (0 == bdexExceptionLimit) {                         \
                    cout << " [ Note: 'bdexExceptionLimit' reached. ]";     \
                }                                                           \
                cout << endl;                                               \
            }                                                               \
            testInStream.setInputLimit(++bdexExceptionCounter);             \
            continue;                                                       \
        }                                                                   \
        testInStream.setInputLimit(-1);                                     \
        break;                                                              \
    } while (1);                                                            \
    if (veryVeryVerbose) cout <<                                            \
        "### End bdex exception test." << endl;                             \
}
#else
#define BEGIN_BDEX_EXCEPTION_TEST                                           \
{                                                                           \
    static int firstTime = 1;                                               \
    if (verbose && firstTime) { cout <<                                     \
        "### BDEX EXCEPTION TEST -- (NOT ENABLED) --" << endl;              \
        firstTime = 0;                                                      \
    }                                                                       \
}
#define END_BDEX_EXCEPTION_TEST
#endif

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

                        // ===========================
                        // class my_BlobBasedEchoSessionFactory
                        // ===========================

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
        ASSERT(0 == std::strcmp(readBuffer, STRING));

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
        ASSERT(0 == std::strcmp(readBuffer, STRING));

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
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    bcema_TestAllocator ta("ta", veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
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
            bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();

            const char STRING[] = "Hello World!";

            const bteso_IPv4Address ADDRESS("127.0.0.1",
                                            echoServer.portNumber());
            ASSERT(0 == socket->connect(ADDRESS));
            ASSERT(sizeof(STRING) == socket->write(STRING, sizeof(STRING)));

            char readBuffer[sizeof(STRING)];
            ASSERT(sizeof(STRING) == socket->read(readBuffer, sizeof(STRING)));
            ASSERT(0 == std::strcmp(readBuffer, STRING));

            factory.deallocate(socket);
        }
        ASSERT(0 != ta.numBytesInUse());
        LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
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
