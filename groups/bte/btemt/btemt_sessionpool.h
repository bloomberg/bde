// btemt_sessionpool.h                                                -*-C++-*-
#ifndef INCLUDED_BTEMT_SESSIONPOOL
#define INCLUDED_BTEMT_SESSIONPOOL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide thread-enabled session-based IPv4 communication
//
//@CLASSES:
//                btemt_SessionPool: session manager
// btemt_SessionPoolSessionIterator: iterator over the sessions managed by a
//                                   session pool
//
//@SEE_ALSO: btemt_session, btemt_asyncchannel, btemt_channelpool
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION:  This component provides a thread-enabled asynchronous
// 'btemt_Session' manager of the IPV4-based byte stream communication
// sessions.  The sessions are allocated automatically when the appropriate
// events occur, and destroyed based on user requests.  A new session is
// allocated automatically when an incoming connection is accepted, by the
// 'btemt_SessionFactory' specified during the call to 'listen', or when a user
// explicitly requests a connection to a server, by a 'btemt_SessionFactory'
// specified during the call to 'connect'.  Session pool has both client-side
// (aka connector) and server-side (aka acceptor) facilities.  The session pool
// manages efficient delivery of messages to/from the user based on
// configuration information supplied at construction.  The states of
// individual messages are not reported; rather, session pool notifies the user
// when a session changes its state.  It also notifies the user when the state
// of the pool is affected, and provides the classification of errors.  The
// notification is done via asynchronous callbacks that can be invoked from any
// (managed) thread.
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
//  // my_echoserver.h
//
//                          // ====================
//                          // class my_EchoSession
//                          // ====================
//
// class my_EchoSession : public btemt_Session {
//     // This class is a concrete implementation of the 'btemt_Session'
//     // protocol to use along with 'my_EchoServer' objects.
//
//     // DATA
//     btemt_AsyncChannel *d_channel_p; // underlying channel (held, not owned)
//
//     // PRIVATE MANIPULATORS
//     void readCb(int                  state,
//                 int                 *numConsumed,
//                 int                 *numNeeded,
//                 const btemt_DataMsg& msg);
//         // Read callback for session pool.
//
//   private:
//     // NOT IMPLEMENTED
//     my_EchoSession(const my_EchoSession&);
//     my_EchoSession& operator=(const my_EchoSession&);
//
//   public:
//     // CREATORS
//     my_EchoSession(btemt_AsyncChannel *channel);
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
//     virtual btemt_AsyncChannel *channel() const;
//         // Return the channel associated with this session.
//  };
//
//                      // ===========================
//                      // class my_EchoSessionFactory
//                      // ===========================
//
//  class my_EchoSessionFactory : public btemt_SessionFactory {
//      // This class is a concrete implementation of the
//      // 'btemt_SessionFactory' that simply allocates 'my_EchoSession'
//      // objects.  No specific allocation strategy (such as pooling) is
//      // implemented.
//
//      bslma_Allocator *d_allocator_p; // memory allocator (held, not owned)
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(my_EchoSessionFactory,
//                                   bslalg_TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      my_EchoSessionFactory(bslma_Allocator *basicAllocator = 0);
//          // Create a new 'my_EchoSessionFactory' object.  Optionally specify
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      virtual ~my_EchoSessionFactory();
//         // Destroy this factory.
//
//      // MANIPULATORS
//      virtual void allocate(btemt_AsyncChannel                    *channel,
//                            const btemt_SessionFactory::Callback&  callback);
//         // Asynchronously allocate a 'btemt_Session' object for the
//         // specified 'channel', and invoke the specified 'callback' with
//         // this session.
//
//      virtual void deallocate(btemt_Session *session);
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
//  void my_EchoSession::readCb(int                   state,
//                              int                  *numConsumed,
//                              int                  *numNeeded,
//                              const btemt_DataMsg&  msg)
//  {
//      if (state) {
//          // Session is going down.
//
//          d_channel_p->close();
//          return;
//      }
//
//      assert(numConsumed);
//      assert(msg.data());
//      assert(0 < msg.data()->length());
//
//      assert(0 == d_channel_p->write(msg));
//
//      *numConsumed = msg.data()->length();
//      *numNeeded   = 1;
//
//      d_channel_p->close(); // close connection.
//  }
//
//  // CREATORS
//  my_EchoSession::my_EchoSession(btemt_AsyncChannel *channel)
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
//      return d_channel_p->read(1,
//                               bdef_MemFnUtil::memFn(&my_EchoSession::readCb,
//                               this));
//  }
//
//  int my_EchoSession::stop()
//  {
//      d_channel_p->close();
//      return 0;
//  }
//
//  // ACCESSORS
//  btemt_AsyncChannel *my_EchoSession::channel() const
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
//                                             bslma_Allocator *basicAllocator)
//  : d_allocator_p(bslma_Default::allocator(basicAllocator))
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
//                             btemt_AsyncChannel                    *channel,
//                             const btemt_SessionFactory::Callback&  callback)
//  {
//      my_EchoSession *session = new (*d_allocator_p) my_EchoSession(channel);
//      callback(0, session);
//  }
//
//  void
//  my_EchoSessionFactory::deallocate(btemt_Session *session)
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
//      btemt_ChannelPoolConfiguration d_config;          // pool
//                                                        // configuration
//
//      btemt_SessionPool             *d_sessionPool_p;   // managed pool
//                                                        // (owned)
//
//      my_EchoSessionFactory          d_sessionFactory;  // my_EchoSession
//                                                        // factory
//
//      int                            d_portNumber;      // port on which this
//                                                        // server is
//                                                        // listening
//
//      bcemt_Mutex                   *d_coutLock_p;      // mutex protecting
//                                                        // bsl::cout
//
//      bslma_Allocator               *d_allocator_p;     // memory allocator
//                                                        // (held)
//
//      // PRIVATE MANIPULATORS
//      void poolStateCb(int reason, int source, void *userData);
//          // Indicates the status of the whole pool.
//
//      void sessionStateCb(int            state,
//                          int            handle,
//                          btemt_Session *session,
//                          void          *userData);
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
//                                   bslalg_TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      my_EchoServer(bcemt_Mutex     *coutLock,
//                    int              portNumber,
//                    int              numConnections,
//                    bool             reuseAddressFlag,
//                    bslma_Allocator *basicAllocator = 0);
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
//      const btemt_SessionPool& pool() const;
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
// proper login mechanism instead such as the 'bael' logger.
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
//  void my_EchoServer::sessionStateCb(int            state,
//                                     int            handle,
//                                     btemt_Session *session,
//                                     void          *userData) {
//
//      switch(state) {
//        case btemt_SessionPool::SESSION_DOWN: {
//            if (veryVerbose) {
//                d_coutLock_p->lock();
//                bsl::cout << "Client from "
//                          << session->channel()->peerAddress()
//                          << " has disconnected."
//                          << bsl::endl;
//                d_coutLock_p->unlock();
//            }
//        } break;
//        case btemt_SessionPool::SESSION_UP: {
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
//  my_EchoServer::my_EchoServer(bcemt_Mutex     *lock,
//                               int              portNumber,
//                               int              numConnections,
//                               bslma_Allocator *basicAllocator)
//  : d_sessionFactory(basicAllocator)
//  , d_coutLock_p(lock)
//  , d_allocator_p(bslma_Default::allocator(basicAllocator))
//  {
//      d_config.setMaxThreads(4);                  // 4 I/O threads
//      d_config.setMaxConnections(numConnections);
//      d_config.setMetricsInterval(10.0);          // seconds
//      d_config.setMaxWriteCache(1<<10);           // 1Mb
//      d_config.setIncomingMessageSizes(1, 100, 1024);
//
//      typedef btemt_SessionPool::SessionPoolStateCallback SessionPoolStateCb;
//
//      SessionPoolStateCb poolStateCb = bdef_MemFnUtil::memFn(
//                                          &my_EchoServer::poolStateCb, this);
//
//      d_sessionPool_p = new (*d_allocator_p)
//                           btemt_SessionPool(d_config, poolStateCb,
//                                             basicAllocator);
//
//      btemt_SessionPool::SessionStateCallback sessionStateCb =
//                     bdef_MemFnUtil::memFn(&my_EchoServer::sessionStateCb,
//                                           this);
//
//      ASSERT(0 == d_sessionPool_p->start());
//      int handle;
//      ASSERT(0 == d_sessionPool_p->listen(&handle, sessionStateCb,
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
//  const btemt_SessionPool& my_EchoServer::pool() const
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
//      bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
//      bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();
//
//      const char STRING[] = "Hello World!";
//
//      const bteso_IPv4Address ADDRESS("127.0.0.1", echoServer.portNumber());
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

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTEMT_CHANNELPOOL
#include <btemt_channelpool.h>
#endif

#ifndef INCLUDED_BTEMT_CHANNELPOOLCONFIGURATION
#include <btemt_channelpoolconfiguration.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BTESO_STREAMSOCKET
#include <bteso_streamsocket.h>
#endif

#ifndef INCLUDED_BTESO_STREAMSOCKETFACTORY
#include <bteso_streamsocketfactory.h>
#endif

#ifndef INCLUDED_BCEC_OBJECTCATALOG
#include <bcec_objectcatalog.h>
#endif

#ifndef INCLUDED_BCEMA_POOLALLOCATOR
#include <bcema_poolallocator.h>
#endif

#ifndef INCLUDED_BCEMA_POOLEDBUFFERCHAIN
#include <bcema_pooledbufferchain.h>
#endif

#ifndef INCLUDED_BCEMA_POOLEDBLOBBUFFERFACTORY
#include <bcema_pooledblobbufferfactory.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BDEALG_SCALARPRIMITIVES
#include <bdealg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#endif

namespace BloombergLP {

class btemt_ChannelPool;
class btemt_DataMsg;
class btemt_Session;
class btemt_SessionFactory;

struct btemt_SessionPool_Handle;
class btemt_SessionPool;

class bteso_SocketOptions;

                   // ======================================
                   // class btemt_SessionPoolSessionIterator
                   // ======================================

class btemt_SessionPoolSessionIterator {
    // Provide read-only sequential access to the allocated sessions in a
    // 'btemt_SessionPool' object.

    // PRIVATE TYPES
    typedef bcec_ObjectCatalogIter<bcema_SharedPtr<btemt_SessionPool_Handle> >
                                                               CatalogIterator;

    // DATA
    CatalogIterator                 d_iterator;  // underlying catalog iterator

    bsl::pair<int, btemt_Session*>  d_current;   // pair referenced by
                                                 // this iterator

    bsl::pair<int, btemt_Session*> *d_current_p; // points to 'd_current' if
                                                 // the iterator is valid, or
                                                 // 0 otherwise.

  private:
    // NOT IMPLEMENTED
    btemt_SessionPoolSessionIterator(const btemt_SessionPoolSessionIterator&);
    btemt_SessionPoolSessionIterator& operator=(
                                      const btemt_SessionPoolSessionIterator&);

    bool operator==(const btemt_SessionPoolSessionIterator&) const;
    bool operator!=(const btemt_SessionPoolSessionIterator&) const;

  public:
    explicit btemt_SessionPoolSessionIterator(btemt_SessionPool *sessionPool);
        // Create an iterator for the specified 'sessionPool' and associate it
        // If the 'catalog' is empty than the iterator is initialized to be
        // invalid.  The 'catalog' is read locked for the duration of
        // iterator's life.

    ~btemt_SessionPoolSessionIterator();
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

    bsl::pair<int, btemt_Session*> const & operator *() const;
        // Return a pair containing the handle ('first' element of the pair)
        // and a pointer to the session ('second' element of the pair )
        // associated with this iterator.  The behavior is undefined unless the
        // iterator is *valid*.
};

                           // =======================
                           // class btemt_SessionPool
                           // =======================

class btemt_SessionPool {
    // This class provides a session manager.

  public:
    // PUBLIC TYPES
    enum SessionState{
        // Result code passed to the session callback.

        SESSION_UP = 1              // new session has been allocated
      , SESSION_DOWN = 2            // session went down
      , SESSION_ALLOC_FAILED = 3    // session allocation failed
      , SESSION_STARTUP_FAILED = 4  // the call to 'start' failed
      , WRITE_CACHE_LOWWAT = 5      // write cache low watermark reached
      , WRITE_CACHE_HIWAT = 6       // write cache high watermark reached
      , ACCEPT_FAILED = 7           // accept failed
      , CONNECT_ATTEMPT_FAILED = 8  // a connection attempt failed
      , CONNECT_FAILED = 9          // the connection initiation failed
      , CONNECT_ABORTED = 10        // session was shutdown before the
                                    // connection could be established
    };

    enum ConnectResolutionMode {
        // Mode indicating whether to perform name resolution at each connect
        // attempt in 'connect'.

        RESOLVE_ONCE            = 0,  // perform resolution once prior to the
                                      // first connect attempt

        RESOLVE_AT_EACH_ATTEMPT = 1   // perform resolution prior to each
                                      // connect attempt
    };

    enum PoolState {
        // Result code passed to the pool callback.  Note that
        // 'CONNECT_ABORTED', 'CONNECT_ATTEMPT_FAILED', 'CONNECT_FAILED',
        // 'ACCEPT_FAILED' are passed to both the session and the pool
        // callbacks.

        SESSION_LIMIT_REACHED = 1   // cannot create more sessions
    };

    typedef bdef_Function<void (*)(int            state,
                                   int            handle,
                                   btemt_Session *session,
                                   void          *userData)>
                                                          SessionStateCallback;
        // Session callback.

    typedef bdef_Function<void (*)(int   state,
                                   int   source,
                                   void *userData)> SessionPoolStateCallback;
        // Pool callback.

  private:
    // PRIVATE TYPES
    typedef bcec_ObjectCatalog<bcema_SharedPtr<btemt_SessionPool_Handle> >
                                                                 HandleCatalog;

    // DATA
    HandleCatalog                  d_handles;            // handles catalog

    btemt_ChannelPoolConfiguration d_config;             // channel
                                                         // configuration

    btemt_ChannelPool             *d_channelPool_p;      // underlying channel
                                                         // manager (owned)

    SessionPoolStateCallback       d_poolStateCB;        // pool state callback

    bcema_PoolAllocator            d_spAllocator;        // smart pointers
                                                         // allocators

    bcema_PooledBufferChainFactory d_bufferChainFactory; // buffer chain
                                                         // factory

    bcema_PooledBlobBufferFactory  d_blobBufferFactory;  // blob buffer
                                                         // factory

    const bool                     d_useBlobForDataReads;// use blob for
                                                         // data reads

    bces_AtomicInt                 d_numSessions;        // number of allocated
                                                         // sessions

    bslma_Allocator               *d_allocator_p;        // allocator (held)

    // PRIVATE MANIPULATORS
    void channelStateCb(int   channelId,
                        int   sourceId,
                        int   state,
                        void *userData);
        // Wrapper for channel pool's channel callback and the session
        // callback.

    void connectAbortTimerCb(
                     const bcema_SharedPtr<btemt_SessionPool_Handle>& handle);
        // Invokes the user callbacks for aborted connects.

    void pooledBufferChainBasedReadCb(int                  *numConsumed,
                                      int                  *numNeeded,
                                      const btemt_DataMsg&  msg,
                                      void                 *userData);
        // Channel pool's pooled buffer chain based read callback.

    void blobBasedReadCb(int        *numNeeded,
                         bcema_Blob *msg,
                         int         channelId,
                         void       *userData);
        // Channel pool's blob based read callback.

    void handleDeleter(btemt_SessionPool_Handle *handle);
        // Delete the specified 'handle'.

    void poolStateCb(int state, int source, int severity);
        // Wrapper for the channel pool state callback and session pool's.

    void sessionAllocationCb(int            result,
                             btemt_Session *session,
                             int            handleId);
        // Session allocation callback passed to the factory at allocation
        // time.

    // FRIENDS
    friend class btemt_SessionPoolSessionIterator;

  private:
    // NOT IMPLEMENTED
    btemt_SessionPool(const btemt_SessionPool&);
    btemt_SessionPool& operator=(const btemt_SessionPool&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(btemt_SessionPool,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    btemt_SessionPool(
                    const btemt_ChannelPoolConfiguration&  config,
                    const SessionPoolStateCallback&        poolStateCallback,
                    bslma_Allocator                       *basicAllocator = 0);
    btemt_SessionPool(
                    const btemt_ChannelPoolConfiguration&  config,
                    const SessionPoolStateCallback&        poolStateCallback,
                    bool                                   useBlobForDataReads,
                    bslma_Allocator                       *basicAllocator = 0);
        // Create a new session pool according to the specified 'config' and
        // with the specified 'poolStateCallback' to be invoked when the pool
        // state changes.  Optionally use 'useBlobForDataReads' to specify
        // if 'bcema_Blob's should be used for reading data.  Optionally
        // specify 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  If 'config.readTimeout()' is the default configured
        // read-timeout value, then this session pool will disable the
        // underlying channel pool's read-timeout event (i.e., this session
        // pool will set the configured read timeout to 0.0s).  Note that read
        // timeout events generated by the underlying channel pool are ignored
        // by this session pool (so there is no benefit to enabling them).

    ~btemt_SessionPool();
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

                                  // *** server-related section ***

    int listen(
            int                                            *handleBuffer,
            const btemt_SessionPool::SessionStateCallback&  callback,
            int                                             portNumber,
            int                                             backlog,
            btemt_SessionFactory                           *factory,
            void                                           *userData = 0,
            const bteso_SocketOptions                      *socketOptions = 0);
    int listen(
            int                                            *handleBuffer,
            const btemt_SessionPool::SessionStateCallback&  callback,
            int                                             portNumber,
            int                                             backlog,
            int                                             reuseAddress,
            btemt_SessionFactory                           *factory,
            void                                           *userData = 0,
            const bteso_SocketOptions                      *socketOptions = 0);
    int listen(
            int                                            *handleBuffer,
            const btemt_SessionPool::SessionStateCallback&  callback,
            const bteso_IPv4Address&                        endpoint,
            int                                             backlog,
            btemt_SessionFactory                           *factory,
            void                                           *userData = 0,
            const bteso_SocketOptions                      *socketOptions = 0);
    int listen(
            int                                            *handleBuffer,
            const btemt_SessionPool::SessionStateCallback&  callback,
            const bteso_IPv4Address&                        endpoint,
            int                                             backlog,
            int                                             reuseAddress,
            btemt_SessionFactory                           *factory,
            void                                           *userData = 0,
            const bteso_SocketOptions                      *socketOptions = 0);
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

    int connect(
            int                                            *handleBuffer,
            const btemt_SessionPool::SessionStateCallback&  callback,
            const char                                     *hostname,
            int                                             port,
            int                                             numAttempts,
            const bdet_TimeInterval&                        interval,
            btemt_SessionFactory                           *factory,
            void                                           *userData = 0,
            ConnectResolutionMode                           resolutionMode
                                                               = RESOLVE_ONCE,
            const bteso_SocketOptions                      *socketOptions = 0,
            const bteso_IPv4Address                        *localAddress = 0);
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
        // pointer to newly created 'btemt_Session' and the optionally
        // specified 'userData'.  Optionally specify 'socketOptions' that will
        // be used to specify what options should be set on the connecting
        // socket.  Optionally specify the 'localAddress' that should be used
        // as the source address.  Return 0 on successful initiation, and a
        // non-zero value otherwise.  The behavior is undefined unless
        // '0 < numAttempts', and '0 < interval' or '1 == numAttempts'.

    int connect(
            int                                            *handleBuffer,
            const btemt_SessionPool::SessionStateCallback&  callback,
            bteso_IPv4Address const&                        endpoint,
            int                                             numAttempts,
            const bdet_TimeInterval&                        interval,
            btemt_SessionFactory                           *factory,
            void                                           *userData = 0,
            const bteso_SocketOptions                      *socketOptions = 0,
            const bteso_IPv4Address                        *localAddress = 0);
        // Asynchronously attempt to connect to the specified 'endpoint' up to
        // the specified 'numAttempts' delaying for the specified 'interval'
        // between each attempt; once a connection is successfully established,
        // allocate and start a session using the specified 'factory' and load
        // a handle for the initiated connection into 'handleBuffer'.  Whenever
        // this session state changes (i.e., is established), the specified
        // 'callback' will be invoked along with a pointer to newly created
        // 'btemt_Session' and the optionally specified 'userData'.  Optionally
        // specify 'socketOptions' that will be used to specify what options
        // should be set on the connecting socket.  Optionally specify the
        // 'localAddress' that should be used as the source address.  Return 0
        // on successful initiation, and a non-zero value otherwise.  The
        // behavior is undefined unless '0 < numAttempts', and '0 < interval'
        // or '1 == numAttempts'.

    int import(int                                            *handleBuffer,
               const btemt_SessionPool::SessionStateCallback&  callback,
               bteso_StreamSocket<bteso_IPv4Address>          *streamSocket,
               bteso_StreamSocketFactory<bteso_IPv4Address>   *socketFactory,
               btemt_SessionFactory                           *sessionFactory,
               void                                           *userData = 0);
        // Asynchronously import the specified 'streamSocket' into this session
        // pool.  Load into the specified 'handleBuffer' the handle to the
        // corresponding internally-allocated session through the specified
        // 'sessionFactory'.  Upon destruction of this session pool,
        // 'streamSocket' is destroyed via the specified 'socketFactory'.  When
        // the session state changes (i.e., is established), the specified
        // 'callback' will be invoked along with the allocated 'btemt_Session'
        // and the optionally specified 'userData'.  Return 0 on success, and a
        // non-zero value with no effect on the session pool otherwise.

    // ACCESSORS
    const btemt_ChannelPoolConfiguration& config() const;
        // Return a non-modifiable reference to the configuration used
        // during the construction of this session pool.

    void getChannelHandleStatistics(bsl::vector<btemt_ChannelPool::HandleInfo>
                                                            *handleInfo) const;
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

                   // --------------------------------------
                   // class btemt_SessionPoolSessionIterator
                   // --------------------------------------

// CREATORS
inline
btemt_SessionPoolSessionIterator::~btemt_SessionPoolSessionIterator()
{
}

// ACCESSORS
inline
btemt_SessionPoolSessionIterator::operator bool() const
{
    return d_current_p;
}

inline
bsl::pair<int, btemt_Session*> const &
btemt_SessionPoolSessionIterator::operator*() const
{
    return *d_current_p;
}

                           // -----------------------
                           // class btemt_SessionPool
                           // -----------------------

// ACCESSORS
inline
const btemt_ChannelPoolConfiguration& btemt_SessionPool::config() const
{
    return d_config;
}

inline
void btemt_SessionPool::getChannelHandleStatistics(
                bsl::vector<btemt_ChannelPool::HandleInfo> *handleInfo) const
{
    if (d_channelPool_p) {
        d_channelPool_p->getHandleStatistics(handleInfo);
    }
}

inline
int btemt_SessionPool::numSessions() const
{
    return d_numSessions;
}

}  // close namespace BloombergLP

#endif
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
