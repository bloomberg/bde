// btlmt_sessionpool.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLMT_SESSIONPOOL
#define INCLUDED_BTLMT_SESSIONPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide thread-enabled session-based IPv4 communication
//
//@CLASSES:
//  btlmt::SessionPool: session manager
//  btlmt::SessionPoolSessionIterator: sessions iterator
//
//@SEE_ALSO: btlmt_session, btlmt_asyncchannel, btlmt_channelpool
//
//@DESCRIPTION: This component provides a thread-enabled asynchronous
// 'btlmt::Session' manager of the IPV4-based byte stream communication
// sessions.  The sessions are allocated automatically when the appropriate
// events occur, and destroyed based on user requests.  A new session is
// allocated automatically when an incoming connection is accepted, by the
// 'btlmt::SessionFactory' specified during the call to 'listen', or when a
// user explicitly requests a connection to a server, by a
// 'btlmt::SessionFactory' specified during the call to 'connect'.  Session
// pool has both client-side (aka connector) and server-side (aka acceptor)
// facilities.  The session pool manages efficient delivery of messages to/from
// the user based on configuration information supplied at construction.  The
// states of individual messages are not reported; rather, session pool
// notifies the user when a session changes its state.  It also notifies the
// user when the state of the pool is affected, and provides the classification
// of errors.  The notification is done via asynchronous callbacks that can be
// invoked from any (managed) thread.
//
///Message Management and Delivery
///-------------------------------
// The session pool provides an efficient mechanism for the full-duplex
// delivery of messages trying to achieve fully parallel communication on a
// socket whenever possible.  If a particular socket's system buffers are full,
// the messages are cached up to a certain (user-defined) limit, at which point
// an alert is generated and the user has the ability to explicitly clear the
// session's outgoing buffer or, by default, continue the transmission.
//
// The session pool tries to achieve optimal performance by enabling zero-copy
// semantics whenever appropriate.  On the read side, the message is either
// read into a local buffer (and subsequently copied by the user) or into a
// buffer that is subsequently handed to the user for accessing and subsequent
// deallocation.  On the write side, if caching is needed, ownership of the
// message buffer is assumed and either the message is copied into the local
// buffer of the session or the address of the buffer is retained.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing an Echo Server
/// - - - - - - - - - - - - - - - - - - -
// The following example implements a simple echo server.  This server accepts
// connections, reads what it receives right away from the network stream,
// sends it back and closes the connection.
//
// To set up this server, users must create a concrete session class derived
// from 'btlmt::Session' protocol, and a factory for creating instances of this
// concrete session type.  'my_EchoSession' objects are created by a factory
// that must be derived from 'btlmt::SessionFactory'.  A
// 'my_EchoSessionFactory' just allocates and deallocates sessions (with no
// pooling or allocation strategy).  This is the simplest form of factory.
//..
//  // my_echoserver.h
//
//                          // ====================
//                          // class my_EchoSession
//                          // ====================
//
// class my_EchoSession : public btlmt::Session {
//     // This class is a concrete implementation of the 'btlmt::Session'
//     // protocol to use along with 'my_EchoServer' objects.
//
//     // DATA
//     btlmt::AsyncChannel *d_channel_p;// underlying channel (held, not owned)
//
//     // PRIVATE MANIPULATORS
//     void readCb(int         result,
//                 int        *numNeeded,
//                 btlb::Blob *blob,
//                 int         channelId);
//         // Read callback for session pool.
//
//   private:
//     // NOT IMPLEMENTED
//     my_EchoSession(const my_EchoSession&);
//     my_EchoSession& operator=(const my_EchoSession&);
//
//   public:
//     // CREATORS
//     my_EchoSession(btlmt::AsyncChannel *channel);
//         // Create a new 'my_EchoSession' object for the specified 'channel'.
//
//     ~my_EchoSession();
//         // Destroy this object.
//
//     // MANIPULATORS
//     virtual int start();
//         // Begin the asynchronous operation of this session.
//
//     virtual int stop();
//         // Stop the operation of this session.
//
//     // ACCESSORS
//     virtual btlmt::AsyncChannel *channel() const;
//         // Return the channel associated with this session.
//  };
//
//                      // ===========================
//                      // class my_EchoSessionFactory
//                      // ===========================
//
//  class my_EchoSessionFactory : public btlmt::SessionFactory {
//      // This class is a concrete implementation of the
//      // 'btlmt::SessionFactory' that simply allocates 'my_EchoSession'
//      // objects.  No specific allocation strategy (such as pooling) is
//      // implemented.
//
//      bslma::Allocator *d_allocator_p; // memory allocator (held, not owned)
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(my_EchoSessionFactory,
//                                   bslalg::TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      my_EchoSessionFactory(bslma::Allocator *basicAllocator = 0);
//          // Create a new 'my_EchoSessionFactory' object.  Optionally specify
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      virtual ~my_EchoSessionFactory();
//         // Destroy this factory.
//
//      // MANIPULATORS
//      virtual void allocate(btlmt::AsyncChannel                    *channel,
//                            const btlmt::SessionFactory::Callback&  cb);
//         // Asynchronously allocate a 'btlmt::Session' object for the
//         // specified 'channel', and invoke the specified 'cb' with
//         // this session.
//
//      virtual void deallocate(btlmt::Session *session);
//         // Deallocate the specified 'session'.
//  };
//..
// The implementations of those session and factory types are rather
// straightforward.  'readCb' will be called when the first byte is received.
// It is in this method that the echo logic is implemented.
//..
//  // my_echoserver.cpp
//
//                          // --------------------
//                          // class my_EchoSession
//                          // --------------------
//
//  // PRIVATE MANIPULATORS
//  void my_EchoSession::readCb(int         result,
//                              int        *numNeeded,
//                              btlb::Blob *blob,
//                              int         channelId)
//  {
//      if (result) {
//          d_channel_p->close();
//          return;
//      }
//
//      ASSERT(numNeeded);
//      ASSERT(0 < blob->length());
//
//      ASSERT(0 == d_channel_p->write(*blob));
//
//      *numNeeded   = 1;
//      btlb::BlobUtil::erase(blob, 0, blob->length());
//
//      d_channel_p->close(); // close connection.
//  }
//
//  // CREATORS
//  my_EchoSession::my_EchoSession(btlmt::AsyncChannel *channel)
//  : d_channel_p(channel)
//  {
//  }
//
//  my_EchoSession::~my_EchoSession()
//  {
//  }
//
//  // MANIPULATORS
//  int my_EchoSession::start()
//  {
//      return d_channel_p->read(
//                              1,
//                              bdlf::MemFnUtil::memFn(&my_EchoSession::readCb,
//                              this));
//  }
//
//  int my_EchoSession::stop()
//  {
//      d_channel_p->close();
//      return 0;
//  }
//
//  // ACCESSORS
//  btlmt::AsyncChannel *my_EchoSession::channel() const
//  {
//      return d_channel_p;
//  }
//
//                      // ---------------------------
//                      // class my_EchoSessionFactory
//                      // ---------------------------
//
//  // CREATORS
//  my_EchoSessionFactory::my_EchoSessionFactory(
//                                            bslma::Allocator *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//  }
//
//  my_EchoSessionFactory::~my_EchoSessionFactory()
//  {
//  }
//
//  // MANIPULATORS
//  void
//  my_EchoSessionFactory::allocate(
//                            btlmt::AsyncChannel                    *channel,
//                            const btlmt::SessionFactory::Callback&  callback)
//  {
//      my_EchoSession *session = new (*d_allocator_p) my_EchoSession(channel);
//      callback(0, session);
//  }
//
//  void
//  my_EchoSessionFactory::deallocate(btlmt::Session *session)
//  {
//      d_allocator_p->deleteObject(session);
//  }
//..
//  We now have all the pieces needed to design and implement our echo server.
//  The server itself owns an instance of the above-defined factory.
//..
//  // my_echoserver.h (continued)
//
//                      // ===================
//                      // class my_EchoServer
//                      // ===================
//
//  class my_EchoServer {
//      // This class implements a multi-user multi-threaded echo server.
//
//      // DATA
//      btlmt::ChannelPoolConfiguration  d_config;         // pool
//                                                         // configuration
//
//      btlmt::SessionPool              *d_sessionPool_p;  // managed pool
//                                                         // (owned)
//
//      my_EchoSessionFactory            d_sessionFactory; // my_EchoSession
//                                                         // factory
//
//      int                              d_portNumber;     // port on which
//                                                         // this server is
//                                                         // listening
//
//      bslmt::Mutex                    *d_coutLock_p;     // mutex protecting
//                                                         // bsl::cout
//
//      bslma::Allocator                *d_allocator_p;    // memory allocator
//                                                         // (held)
//
//      // PRIVATE MANIPULATORS
//      void poolStateCb(int reason, int source, void *userData);
//          // Indicates the status of the whole pool.
//
//      void sessionStateCb(int             state,
//                          int             handle,
//                          btlmt::Session *session,
//                          void           *userData);
//          // Per-session state.
//
//    private:
//      // NOT IMPLEMENTED
//      my_EchoServer(const my_EchoServer& original);
//      my_EchoServer& operator=(const my_EchoServer& rhs);
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(my_EchoServer,
//                                   bslalg::TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      my_EchoServer(bslmt::Mutex     *coutLock,
//                    int               portNumber,
//                    int               numConnections,
//                    bool              reuseAddressFlag,
//                    bslma::Allocator *basicAllocator = 0);
//          // Create an echo server that listens for incoming connections on
//          // the specified 'portNumber' managing up to the specified
//          // 'numConnections' simultaneous connections.  Pass the specified
//          // 'reuseAddressFlag' to the set the 'REUSE_ADDRESS' socket option
//          // to the listening socket.  The echo server will use the
//          // specified 'coutLock' to synchronize access to the standard
//          // output.  Optionally specify a 'basicAllocator' used to supply
//          // memory.  If 'basicAllocator' is 0, the currently installed
//          // default allocator is used.  The behavior is undefined if
//          // 'coutLock' is 0.
//
//      ~my_EchoServer();
//          // Destroy this server.
//
//      // MANIPULATORS
//      const btlmt::SessionPool& pool() const;
//          // Return a non-modifiable reference to the session pool used by
//          // this echo server.
//
//      int portNumber() const;
//          // Return the port number on which this server is listening.
//  };
//..
// Note that this example server prints information depending on
// implicitly-defined static variables and therefore must use a mutex to
// synchronize access to 'bsl::cout'.  A production application should use a
// proper logging mechanism instead such as the 'bael' logger.
//..
//  // my_echoserver.h (continued)
//
//                          // -------------------
//                          // class my_EchoServer
//                          // -------------------
//
//  // PRIVATE MANIPULATORS
//  void my_EchoServer::poolStateCb(int reason, int source, void *userData)
//  {
//      if (veryVerbose) {
//          d_coutLock_p->lock();
//          bsl::cout << "Pool state changed: (" << reason << ", " << source
//                    << ") " << bsl::endl;
//          d_coutLock_p->unlock();
//      }
//  }
//
//  void my_EchoServer::sessionStateCb(int             state,
//                                     int             handle,
//                                     btlmt::Session *session,
//                                     void           *userData) {
//
//      switch(state) {
//        case btlmt::SessionPool::SESSION_DOWN: {
//            if (veryVerbose) {
//                d_coutLock_p->lock();
//                bsl::cout << "Client from "
//                          << session->channel()->peerAddress()
//                          << " has disconnected."
//                          << bsl::endl;
//                d_coutLock_p->unlock();
//            }
//        } break;
//        case btlmt::SessionPool::SESSION_UP: {
//            if (veryVerbose) {
//                d_coutLock_p->lock();
//                bsl::cout << "Client connected from "
//                          << session->channel()->peerAddress()
//                          << bsl::endl;
//                d_coutLock_p->unlock();
//            }
//        } break;
//      }
//  }
//
//  // CREATORS
//  my_EchoServer::my_EchoServer(bslmt::Mutex     *lock,
//                               int               portNumber,
//                               int               numConnections,
//                               bslma::Allocator *basicAllocator)
//  : d_sessionFactory(basicAllocator)
//  , d_coutLock_p(lock)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      d_config.setMaxThreads(4);                  // 4 I/O threads
//      d_config.setMaxConnections(numConnections);
//      d_config.setMetricsInterval(10.0);          // seconds
//      d_config.setWriteCacheWatermarks(0, 1<<10); // 1Mb
//      d_config.setIncomingMessageSizes(1, 100, 1024);
//
//      typedef btlmt::SessionPool::SessionPoolStateCallback SessionStateCb;
//
//      SessionStateCb poolStateCb = bdlf::MemFnUtil::memFn(
//                                                 &my_EchoServer::poolStateCb,
//                                                 this);
//
//      d_sessionPool_p = new (*d_allocator_p) btlmt::SessionPool(
//                                                             d_config,
//                                                             poolStateCb,
//                                                             basicAllocator);
//
//      btlmt::SessionPool::SessionStateCallback sessionStateCb =
//                       bdlf::MemFnUtil::memFn(&my_EchoServer::sessionStateCb,
//                                              this);
//
//      ASSERT(0 == d_sessionPool_p->start());
//      int handle;
//      ASSERT(0 == d_sessionPool_p->listen(&handle,
//                                          sessionStateCb,
//                                          portNumber,
//                                          numConnections,
//                                          &d_sessionFactory));
//
//      d_portNumber = d_sessionPool_p->portNumber(handle);
//  }
//
//  my_EchoServer::~my_EchoServer()
//  {
//      d_sessionPool_p->stop();
//      d_allocator_p->deleteObject(d_sessionPool_p);
//  }
//
//  // ACCESSORS
//  const btlmt::SessionPool& my_EchoServer::pool() const
//  {
//      return *d_sessionPool_p;
//  }
//
//  int my_EchoServer::portNumber() const
//  {
//      return d_portNumber;
//  }
//..
// We can implement a simple "Hello World!" example to exercise our echo
// server.
//..
//  int main() {
//
//      enum {
//          BACKLOG = 5,
//          REUSE   = 1
//      };
//
//      my_EchoServer echoServer(&coutMutex, 0, BACKLOG, REUSE);
//
//      btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
//      btlso::StreamSocket<btlso::IPv4Address> *socket = factory.allocate();
//
//      const char STRING[] = "Hello World!";
//
//      const btlso::IPv4Address ADDRESS("127.0.0.1", echoServer.portNumber());
//      assert(0 == socket->connect(ADDRESS));
//      assert(sizeof(STRING) == socket->write(STRING, sizeof(STRING)));
//
//      char readBuffer[sizeof(STRING)];
//      assert(sizeof(STRING) == socket->read(readBuffer, sizeof(STRING)));
//      assert(0 == bsl::strcmp(readBuffer, STRING));
//
//      factory.deallocate(socket);
//      return 0;
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLMT_CHANNELPOOL
#include <btlmt_channelpool.h>
#endif

#ifndef INCLUDED_BTLMT_CHANNELPOOLCONFIGURATION
#include <btlmt_channelpoolconfiguration.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BTLSO_STREAMSOCKET
#include <btlso_streamsocket.h>
#endif

#ifndef INCLUDED_BTLSO_STREAMSOCKETFACTORY
#include <btlso_streamsocketfactory.h>
#endif

#ifndef INCLUDED_BDLCC_OBJECTCATALOG
#include <bdlcc_objectcatalog.h>
#endif

#ifndef INCLUDED_BDLMA_CONCURRENTPOOLALLOCATOR
#include <bdlma_concurrentpoolallocator.h>
#endif

#ifndef INCLUDED_BTLB_BLOB
#include <btlb_blob.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTR
#include <bslma_managedptr.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {

namespace btlso { class SocketOptions; }

namespace btlmt {

class ChannelPool;
class Session;
class SessionFactory;

class  ChannelPool;
class  Session;
class  SessionFactory;
struct SessionPool_Handle;
class  SessionPool;

                   // ================================
                   // class SessionPoolSessionIterator
                   // ================================

class SessionPoolSessionIterator {
    // Provide read-only sequential access to the allocated sessions in a
    // 'SessionPool' object.

    // PRIVATE TYPES
    typedef bdlcc::ObjectCatalogIter<bsl::shared_ptr<SessionPool_Handle> >
                                                               CatalogIterator;

    // DATA
    CatalogIterator            d_iterator;  // underlying catalog iterator

    bsl::pair<int, Session *>  d_current;   // pair referenced by
                                            // this iterator

    bsl::pair<int, Session *> *d_current_p; // points to 'd_current' if
                                            // the iterator is valid, or
                                            // 0 otherwise.

  private:
    // NOT IMPLEMENTED
    SessionPoolSessionIterator(const SessionPoolSessionIterator&);
    SessionPoolSessionIterator& operator=(const SessionPoolSessionIterator&);

    bool operator==(const SessionPoolSessionIterator&) const;
    bool operator!=(const SessionPoolSessionIterator&) const;

  public:
    explicit SessionPoolSessionIterator(SessionPool *sessionPool);
        // Create an iterator for the specified 'sessionPool' and associate it
        // If the 'catalog' is empty than the iterator is initialized to be
        // invalid.  The 'catalog' is read locked for the duration of
        // iterator's life.

    ~SessionPoolSessionIterator();
        // Destroy this iterator.

    // MANIPULATORS
    void operator++();
        // Advance this iterator to refer to the next object of the associated
        // session pool; if there is no next object in the associated session
        // pool, then this iterator becomes *invalid*.  The behavior is
        // undefined unless this iterator is valid.  Note that the order of the
        // iteration is implementation-defined.

    // ACCESSORS
    operator bool() const;
        // Return true if the iterator is *valid*, and false otherwise.

    const bsl::pair<int, Session *>& operator*() const;
        // Return a pair containing the handle ('first' element of the pair)
        // and a pointer to the session ('second' element of the pair )
        // associated with this iterator.  The behavior is undefined unless the
        // iterator is *valid*.
};

                           // =================
                           // class SessionPool
                           // =================

class SessionPool {
    // This class provides a mechanism for establishing and managing network
    // connections.  A session pool can be configured at construction by
    // passing in a 'ChannelPoolConfiguration' object and allows the
    // establishment of both server sessions via the 'listen' method and client
    // sessions via the 'connect' method.  The connection initiation methods
    // take a 'SessionFactory' object that allows users to return a 'Session'
    // object that the session pool stores as a reference for the connection.
    // During the allocation of the 'Session' object clients are provided an
    // 'AsyncChannel' that they can use for exchanging data with the other
    // endpoint in the connection.  An existing socket can be imported into a
    // session pool and this will create a session enabled both for read and
    // for write.  All of the callbacks invoked by session pool are run in a
    // number of threads created at start up.  Session pool ensures that
    // callback methods of a particular connection are always invoked on the
    // same thread so users dont have to synchroninze data associated with a
    // particular connection.  Session pool can be started or stopped via the
    // 'start' and 'stop' methods respectively.  Once started and until
    // stopped, the session pool dispatches incoming and outgoing connections,
    // messages, and other session functions to the processing threads.  Once
    // stopped, the channel pool can be started again and the channels will
    // resume their operations.

  public:
    // PUBLIC TYPES
    enum SessionState{
        // Result code passed to the session callback.

        e_SESSION_UP             = 1,  // new session has been allocated
        e_SESSION_DOWN           = 2,  // session went down
        e_SESSION_ALLOC_FAILED   = 3,  // session allocation failed
        e_SESSION_STARTUP_FAILED = 4,  // the call to 'start' failed
        e_WRITE_CACHE_LOWWAT     = 5,  // write cache low watermark reached
        e_WRITE_CACHE_HIWAT      = 6,  // write cache high watermark reached
        e_ACCEPT_FAILED          = 7,  // accept failed
        e_CONNECT_ATTEMPT_FAILED = 8,  // a connection attempt failed
        e_CONNECT_FAILED         = 9,  // the connection initiation failed
        e_CONNECT_ABORTED        = 10  // session was shutdown before the
                                       // connection could be established


    };

    enum ConnectResolutionMode {
        // Mode indicating whether to perform name resolution at each connect
        // attempt in 'connect'.

        e_RESOLVE_ONCE            = 0,  // perform resolution once prior to the
                                        // first connect attempt

        e_RESOLVE_AT_EACH_ATTEMPT = 1   // perform resolution prior to each
                                        // connect attempt


    };

    enum PoolState {
        // Result code passed to the pool callback.  Note that
        // 'e_CONNECT_ABORTED', 'e_CONNECT_ATTEMPT_FAILED', 'e_CONNECT_FAILED',
        // 'e_ACCEPT_FAILED' are passed to both the session and the pool
        // callbacks.

        e_SESSION_LIMIT_REACHED = 1   // cannot create more sessions


    };

    typedef bsl::function<void(int      state,
                               int      handle,
                               Session *session,
                               void    *userData)> SessionStateCallback;
        // Session callback.

    typedef bsl::function<void(int   state,
                               int   source,
                               void *userData)> SessionPoolStateCallback;
        // Pool callback.

  private:
    // PRIVATE TYPES
    typedef bdlcc::ObjectCatalog<bsl::shared_ptr<SessionPool_Handle> >
                                                                 HandleCatalog;

    // DATA
    HandleCatalog                   d_handles;            // handles catalog

    ChannelPoolConfiguration        d_config;             // channel config

    ChannelPool                    *d_channelPool_p;      // underlying channel
                                                          // manager (owned)

    SessionPoolStateCallback        d_poolStateCB;        // pool state
                                                          // callback

    bdlma::ConcurrentPoolAllocator  d_spAllocator;        // smart pointers
                                                          // allocators

    bslma::ManagedPtr<btlb::BlobBufferFactory>
                                    d_blobBufferFactory;  // blob buffer
                                                          // factory

    bsls::AtomicInt                 d_numSessions;        // number of
                                                          // allocated
                                                          // sessions

    bslma::Allocator               *d_allocator_p;        // allocator (held)

    // PRIVATE MANIPULATORS
    void channelStateCb(int   channelId,
                        int   sourceId,
                        int   state,
                        void *userData);
        // Wrapper for channel pool's channel callback and the session
        // callback.

    void connectAbortTimerCb(
                           const bsl::shared_ptr<SessionPool_Handle>& handle,
                           int                                        clockId);
        // Deregister the timer callback having the specified 'clockId'
        // registered with the underlying channel pool and destroy the
        // specified 'handle'.

    void blobBasedReadCb(int        *numNeeded,
                         btlb::Blob *msg,
                         int         channelId,
                         void       *userData);
        // Channel pool's blob based read callback.

    void terminateSession(SessionPool_Handle *handle);
        // Issue a session down event and stop the session associated with the
        // specified 'handle'.

    void handleDeleter(SessionPool_Handle *handle);
        // Delete the specified 'handle'.

    void poolStateCb(int state, int source, int severity);
        // Wrapper for the channel pool state callback and session pool's.

    void sessionAllocationCb(int result, Session *session, int handleId);
        // Session allocation callback passed to the factory at allocation
        // time.

    void init();
        // Initialize this session pool.

    int makeConnectHandle(
                         const SessionPool::SessionStateCallback&  cb,
                         int                                       numAttempts,
                         void                                     *userData,
                         SessionFactory                           *factory);
       // Add a handle for a connection session with the specified
       // 'numAttempts', 'userData', and 'cb' parameters to this session pool.
       // The factory will be allocated from the specified 'factory'.  Return
       // the identifier for the new handle.

    // FRIENDS
    friend class SessionPoolSessionIterator;

  private:
    // NOT IMPLEMENTED
    SessionPool(const SessionPool&);
    SessionPool& operator=(const SessionPool&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SessionPool, bslma::UsesBslmaAllocator);

    // CREATORS
    SessionPool(const ChannelPoolConfiguration&  config,
                const SessionPoolStateCallback&  poolStateCallback,
                bslma::Allocator                *basicAllocator = 0);
    SessionPool(btlb::BlobBufferFactory         *blobBufferFactory,
                const ChannelPoolConfiguration&  config,
                const SessionPoolStateCallback&  poolStateCallback,
                bslma::Allocator                *basicAllocator = 0);
        // Create a new session pool according to the specified 'config' that
        // uses 'btlb::Blob's for reading data and invokes the specified
        // 'poolStateCallback' when the pool state changes.  Use the specified
        // 'blobBufferFactory' to supply buffers for the blobs used for reading
        // data.  Optionally specify 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  If 'config.readTimeout()' is the default configured
        // read-timeout value, then this session pool will disable the
        // underlying channel pool's read-timeout event (i.e., this session
        // pool will set the configured read timeout to 0.0s).  Note that read
        // timeout events generated by the underlying channel pool are ignored
        // by this session pool (so there is no benefit to enabling them).

    ~SessionPool();
        // Destroy this session pool.

    // MANIPULATORS
                                  // *** pool management section ***
    int start();
        // Begin the asynchronous operation of the session pool.  Return zero
        // on success, and a non-zero value otherwise.

    int stop();
        // Stop the asynchronous operation of the session pool and gracefully
        // terminate all active connection and listeners; return 0 on success
        // and a non-zero value otherwise.  If all attempts to terminate the
        // sessions "gracefully" fail, a negative value is returned and the
        // sessions are destroyed anyway.

    int stopAndRemoveAllSessions();
        // Stop the asynchronous operation of this session pool, gracefully
        // terminate all active connectors and listeners, and remove all those
        // communication sessions from this session pool.  Return 0 on success,
        // and a non-zero value otherwise.  If all attempts to terminate the
        // sessions "gracefully" fail, a negative value is returned and the
        // sessions are destroyed anyway.  Note that shutting down a session
        // will deallocate all system resources associated with that session.

                                  // *** server-related section ***

    int listen(int                                      *handleBuffer,
               const SessionPool::SessionStateCallback&  callback,
               int                                       portNumber,
               int                                       backlog,
               SessionFactory                           *factory,
               void                                     *userData = 0,
               const btlso::SocketOptions               *socketOptions = 0);
    int listen(int                                      *handleBuffer,
               const SessionPool::SessionStateCallback&  callback,
               int                                       portNumber,
               int                                       backlog,
               int                                       reuseAddress,
               SessionFactory                           *factory,
               void                                     *userData = 0,
               const btlso::SocketOptions               *socketOptions = 0);
    int listen(int                                      *handleBuffer,
               const SessionPool::SessionStateCallback&  callback,
               const btlso::IPv4Address&                 endpoint,
               int                                       backlog,
               SessionFactory                           *factory,
               void                                     *userData = 0,
               const btlso::SocketOptions               *socketOptions = 0);
    int listen(int                                      *handleBuffer,
               const SessionPool::SessionStateCallback&  callback,
               const btlso::IPv4Address&                 endpoint,
               int                                       backlog,
               int                                       reuseAddress,
               SessionFactory                           *factory,
               void                                     *userData = 0,
               const btlso::SocketOptions               *socketOptions = 0);
        // Asynchronously listen for connection requests on the specified
        // 'portNumber' on all local interfaces or the specified 'endpoint',
        // depending on which overload of listen is used, with up to a maximum
        // of 'backlog' concurrent connection requests.  Once a connection is
        // successfully accepted, this session pool will allocate and start a
        // session for the connection using the specified 'factory'.  Load a
        // handle for the listening connection into 'handleBuffer'.  Optionally
        // specify a 'reuseAddress' value to be used in setting 'REUSEADDRESS'
        // socket option.  If 'reuseAddress' is not specified, 1 (i.e.,
        // 'REUSEADDRESS' is enabled) is used.  Optionally specify
        // 'socketOptions' that will be used to specify what options should be
        // set on the listening socket.  Return 0 on success, and a non-zero
        // value otherwise.  Every time a connection is accepted by this pool
        // on this (newly-established) listening socket, the newly allocated
        // session is passed to the specified 'callback' along with the
        // optionally specified 'userData'.  The behavior is undefined unless
        // '0 < backlog'.

                                  // *** client-related section ***
    int closeHandle(int handle);
        // Close the listener or the connection represented by the specified
        // 'handle'.  Return 0 on success, or a non-zero value if the specified
        // 'handle' does not match any currently allocation session handle.

    int connect(int                                      *handleBuffer,
                const SessionPool::SessionStateCallback&  callback,
                const char                               *hostname,
                int                                       port,
                int                                       numAttempts,
                const bsls::TimeInterval&                 interval,
                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                                         *socket,
                SessionFactory                           *factory,
                void                                     *userData = 0,
                ConnectResolutionMode                     resolutionMode
                                                             = e_RESOLVE_ONCE);
    int connect(int                                      *handleBuffer,
                const SessionPool::SessionStateCallback&  callback,
                const char                               *hostname,
                int                                       port,
                int                                       numAttempts,
                const bsls::TimeInterval&                 interval,
                SessionFactory                           *factory,
                void                                     *userData = 0,
                ConnectResolutionMode                     resolutionMode
                                                              = e_RESOLVE_ONCE,
                const btlso::SocketOptions               *socketOptions = 0,
                const btlso::IPv4Address                 *localAddress = 0);
        // Asynchronously attempt to connect to the specified 'hostname' on the
        // specified 'port' up to the specified 'numAttempts' delaying for the
        // specified 'interval' between each attempt.  Optionally specify a
        // 'resolutionMode' to indicate whether the name resolution is
        // performed once (if 'resolutionMode' is 'RESOLVE_ONCE'), or performed
        // anew prior to each attempt (if 'resolutionMode' is
        // 'RESOLVE_AT_EACH_ATTEMPT'); if 'resolutionMode is not specified,
        // 'RESOLVE_ONCE' is used.  Once a connection is successfully
        // established, allocate and start a session using the specified
        // 'factory' and load a handle for the initiated connection into
        // 'handleBuffer'.  Whenever this session state changes (i.e., is
        // established), the specified 'callback' will be invoked along with a
        // pointer to newly created 'Session' and the optionally specified
        // 'userData'.  Optionally specify either 'socketOptions' that will be
        // used to specify what options should be set on the connecting socket
        // and/or the specified 'localAddress' to be used as the source
        // address, or specify 'socket' to use as the connecting socket (with
        // any desired options and/or source address already set).  If 'socket'
        // is specified, this pool will assume its ownership, if this function
        // returns successfully, and will be left unchanged if an error is
        // returned.  Return 0 on successful initiation, and a non-zero value
        // otherwise.  The behavior is undefined unless '0 < numAttempts', and
        // '0 < interval || 1 == numAttempts'.

    int connect(int                                      *handleBuffer,
                const SessionPool::SessionStateCallback&  callback,
                btlso::IPv4Address const&                 endpoint,
                int                                       numAttempts,
                const bsls::TimeInterval&                 interval,
                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                                         *socket,
                SessionFactory                           *factory,
                void                                     *userData = 0);
    int connect(int                                      *handleBuffer,
                const SessionPool::SessionStateCallback&  callback,
                btlso::IPv4Address const&                 endpoint,
                int                                       numAttempts,
                const bsls::TimeInterval&                 interval,
                SessionFactory                           *factory,
                void                                     *userData = 0,
                const btlso::SocketOptions               *socketOptions = 0,
                const btlso::IPv4Address                 *localAddress = 0);
        // Asynchronously attempt to connect to the specified 'endpoint' up to
        // the specified 'numAttempts' delaying for the specified 'interval'
        // between each attempt; once a connection is successfully established,
        // allocate and start a session using the specified 'factory' and load
        // a handle for the initiated connection into 'handleBuffer'.  Whenever
        // this session state changes (i.e., is established), the specified
        // 'callback' will be invoked along with a pointer to newly created
        // 'Session' and the optionally specified 'userData'.  Optionally
        // specify either 'socketOptions' that will be used to specify what
        // options should be set on the connecting socket and/or 'localAddress'
        // to be used as the source address, or 'socket' to use as the
        // connecting socket (with any desired options and/or source address
        // already set).  If 'socket' is specified, ownership will be
        // transferred from it if this function returns successfully, and will
        // be left unchanged if an error is returned.  Return 0 on successful
        // initiation, and a non-zero value otherwise.  The behavior is
        // undefined unless '0 < numAttempts', and '0 < interval' or
        // '1 == numAttempts'.

    int import(int                                            *handleBuffer,
               const SessionPool::SessionStateCallback&        callback,
               btlso::StreamSocket<btlso::IPv4Address>        *streamSocket,
               btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
               SessionFactory                                 *sessionFactory,
               void                                           *userData = 0);
    int import(int                                            *handleBuffer,
               const SessionPool::SessionStateCallback&        callback,
               bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                                              *streamSocket,
               SessionFactory                                 *sessionFactory,
               void                                           *userData = 0);
        // Asynchronously import the specified 'streamSocket' into this session
        // pool.  Load into the specified 'handleBuffer' the handle to the
        // corresponding internally-allocated session through the specified
        // 'sessionFactory'.  If 'streamSocket' is a 'bslma::ManagedPtr', then
        // ownership is transferred from it if this function returns
        // successfully, and will be left unchanged if an error is returned;
        // otherwise, upon destruction of this session pool, 'streamSocket' is
        // destroyed via the specified 'socketFactory'.  When the session state
        // changes (i.e., is established), the specified 'callback' will be
        // invoked along with the allocated 'Session' and the optionally
        // specified 'userData'.  Return 0 on success, and a non-zero value
        // with no effect on the session pool otherwise.

    int setWriteCacheWatermarks(int handleId,
                                int lowWatermark,
                                int hiWatermark);
        // Set the write cache low- and high-water marks for the session
        // associated with the specified 'handleId' to the specified
        // 'lowWatermark' and 'hiWatermark' values, respectively; return 0 on
        // success, and a non-zero value otherwise.  A
        // 'BTEMT_WRITE_CACHE_LOWWAT' alert is provided (via the channel state
        // callback) if 'lowWatermark' is greater than or equal to the current
        // size of the write cache, and a 'BTEMT_WRITE_CACHE_HIWAT' alert is
        // provided if 'hiWatermark' is less than or equal to the current size
        // of the write cache.  (See the component-level documentation of
        // 'btlmt_channelpool' for details on 'BTEMT_WRITE_CACHE_HIWAT' and
        // 'BTEMT_WRITE_CACHE_LOWWAT' alerts.)  The behavior is undefined
        // unless '0 <= lowWatermark' and 'lowWatermark <= hiWatermark'.  Note
        // that this method overrides the values configured (for all channels)
        // by the 'ChannelPoolConfiguration' supplied at construction.

    // ACCESSORS
    const ChannelPoolConfiguration& config() const;
        // Return a non-modifiable reference to the configuration used during
        // the construction of this session pool.

    void getChannelHandleStatistics(
                       bsl::vector<ChannelPool::HandleInfo> *handleInfo) const;
        // Load into the specified 'handleInfo' array a snapshot of the
        // information per socket handle currently in use by this channel pool.

    int numSessions() const;
        // Return a *snapshot* of the current number of sessions managed by
        // this session pool.

    int portNumber(int handle) const;
        // Return the port number on which the session with the specified
        // 'handle' is listening, or a negative value if 'handle' does not
        // refer to an active listening session obtained from a successful call
        // to 'listen' on this session pool.
};

// ============================================================================
//                      INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

                   // --------------------------------
                   // class SessionPoolSessionIterator
                   // --------------------------------

// CREATORS
inline
SessionPoolSessionIterator::~SessionPoolSessionIterator()
{
}

// ACCESSORS
inline
btlmt::SessionPoolSessionIterator::operator bool() const
{
    return d_current_p;
}

inline
const bsl::pair<int, Session *>& SessionPoolSessionIterator::operator*() const
{
    return *d_current_p;
}

                           // -----------------
                           // class SessionPool
                           // -----------------

// ACCESSORS
inline
const ChannelPoolConfiguration& SessionPool::config() const
{
    return d_config;
}

inline
void SessionPool::getChannelHandleStatistics(
                        bsl::vector<ChannelPool::HandleInfo> *handleInfo) const
{
    if (d_channelPool_p) {
        d_channelPool_p->getHandleStatistics(handleInfo);
    }
}

inline
int SessionPool::numSessions() const
{
    return d_numSessions;
}

// MANIPULATORS
inline
int SessionPool::import(
                int                                            *handleBuffer,
                const SessionPool::SessionStateCallback&        callback,
                btlso::StreamSocket<btlso::IPv4Address>        *streamSocket,
                btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
                SessionFactory                                 *sessionFactory,
                void                                           *userData)
{
    typedef btlso::StreamSocketFactoryDeleter Deleter;

    bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> > socket(
                                   streamSocket,
                                   socketFactory,
                                   &Deleter::deleteObject<btlso::IPv4Address>);

    const int rc = import(handleBuffer,
                          callback,
                          &socket,
                          sessionFactory,
                          userData);

    if (rc) {
        socket.release();
    }
    return rc;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
