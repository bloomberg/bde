// btemt_channelpool.h                                                -*-C++-*-
#ifndef INCLUDED_BTEMT_CHANNELPOOL
#define INCLUDED_BTEMT_CHANNELPOOL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide thread-enabled stream-based IPv4 communication.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@SEE_ALSO:
//   btemt_channelpoolconfiguration, btemt_channelqueuepool
//
//@CLASSES:
//   btemt_ChannelPool: channel manager
//
//@DESCRIPTION:  This component provides a thread-enabled manager of the
// IPv4-based byte-stream communication channels.  The channels are created
// automatically when the appropriate events occur and destroyed based on user
// requests.  A new channel is allocated automatically when an incoming
// connection is accepted, or when the user explicitly requests a connection to
// a server.  Channel pool provides both client (aka connector) and server (aka
// acceptor) functionality.  The channel pool manages efficient delivery of
// messages to/from the user based on configuration information supplied at
// construction.  The states of individual messages are *not* reported;
// rather, channel pool notifies the user when a channel's state changes.  It
// also notifies the user when the pool's state is affected and provides the
// classification of errors.  The notification is done via asynchronous
// callback that may be invoked from *any* (managed) thread.
//
///Message management and delivery
///-------------------------------
// The channel pool provides an efficient mechanism for the full-duplex
// delivery of messages trying to achieve fully parallel communication on a
// socket whenever possible.  If a particular socket's system buffers are full,
// the messages are cached up to a certain (user-defined) limit, at which point
// an alert is generated.
//
// The channel pool tries to achieve optimal performance by enabling zero-copy
// semantics whenever appropriate.  On the read side, a message is read
// into a buffer that is subsequently passed to the user (via the
// 'DataReadCallback') for accessing and eventual deallocation.  On the write
// side, the channel pool adopts ownership of the buffers passed to 'write()'.
// Note that page-like allocation is used for the data buffers created by a
// channel pool (see 'bcema_pooledbufferchain').
//
///Channel identification
///----------------------
// Each channel is identified by an integer ID that is (a) assigned by the
// channel pool and (b) is guaranteed to be unique for the lifetime of the
// channel pool.  The user can rely on this uniqueness to identify channels.
//
///Asynchronous connect
///--------------------
// The channel pool supports extended asynchronous connect mechanism, by which
// the pool will try to establish a connection with the server making up to
// (user-provided) number of attempts with a (user-provided) interval between
// attempts.  If a connection attempt cannot succeed within an interval, it is
// aborted and a new connection request is issued.  If a connection is
// successfully established, the channel state callback (see configuration) is
// invoked.  In that case, if this connection is dropped at a later time,
// channel pool *will* *not* *try* to reconnect automatically.  If *all*
// attempts to establish a connection fail, then a pool state callback
// (see configuration) is invoked.  Once initiated, a connect request can
// lead to only two outcomes -- success or failure.  In particular, it can't
// be cancelled.
//
///Half-open connections
///---------------------
// It is already possible to import a half-duplex connection into a channel
// pool, but should any half of any channel be closed, the channel pool would
// always close the channel (both halves).  A new feature, introduced in BCE
// 1.5.0, allows to keep half-open channels, i.e., single-duplex connections.
// In particular, it is possible at the creation of the channel to specify what
// the channel pool should do if only half of the channel is closed.  Passing
// in the following optional parameter (of enumeration type
// 'btemt_ChannelPool::KeepHalfOpenMode') to 'close', 'import', or 'listen' has
// the following behavior.
//..
//  BTEMT_KEEP_HALF_OPEN    If a peer closes its write part of the channel,
//                          but keeps its receive part open, then the channel
//                          pool will keep sending messages to the peer, but
//                          will disable reading on the channel.
//                          If a peer closes its receive part of the channel,
//                          but keeps its write part open, then the channel
//                          pool will keep reading enable on this channel, but
//                          further calls to writeMessage on this channel will
//                          fail.
//
//  BTEMT_CLOSE_BOTH        If a peer closes either its write or receive part
//                          of the channel, the channel pool will simply shut
//                          down the channel.  This is the default behavior.
//..
//
///Resource limits
///---------------
// The channel pool limits the resource usage based on the configuration.  The
// user must specify the maximum number of connections that an instance can
// manage, the maximum number of threads that should be used, and the size (in
// bytes) of a channel's outgoing buffer.  Once the maximum number of
// connections has been reached, an alert is generated and further channels
// cannot be created.  On the client side, all requests for a connection will
// be denied.  On the server side, no connections will be accepted, though the
// listening port is *not* closed.
//
// If the peer on a particular channel is unable to keep up with the traffic,
// the system buffers will become full and the channel will cache the outgoing
// messages up to the specified limit.  Once this limit is reached, an alert is
// generated and all further requests for sending data are denied until there
// is space available in the channel's buffer.  The same limit applies for
// every channel.
//
///Behavior on 'fork'
///------------------
// On Unix systems, the channel's underlying sockets (file descriptors) have
// the close-on-exec flag set by default at construction time, which is passed
// on to all the child processes created by 'fork()' during the lifetime of the
// channel pool.  This ensures that the channels, which are owned by the
// channel pool, are not passed on to other applications created by 'fork()'
// followed by an 'exec()'; instead, all sockets associated to channels are
// closed in the child process following the 'exec()' command.
//
// This default setting is done for user convenience.  The user should realize
// that the behavior of 'fork()' is actually undefined for multi-threaded
// processes.  Also, beware that this does not guard against 'fork()' *not*
// followed by 'exec()': the file descriptors will remain opened in the child
// processes, which may potentially outlive the lifetime of the channel pool,
// preventing the channel's socket files from being closed properly.
//
///Metrics and capacity
///--------------------
// By default, the channel pool monitors the workload of managed event
// managers and reports, upon request, an average value of this workload.  The
// workload of each individual event manager is calculated as the ratio of CPU
// bound processing time to the total processing time of the event manager
// (i.e. CPU bound time +  I/O bound time) over a user-configured interval.
// The time line for each event manager is broken into two classifications, I/O
// bound and CPU bound intervals; an event manager is performing an I/O
// bound operation when it is blocked on a system call (e.g., 'select');
// otherwise, it is performing a CPU bound operation.  Note that non-blocking
// system calls are NOT considered as I/O bound.  An average is then calculated
// over the set of *all* *possible* event managers, and the load is taken as 0
// if an event manager is not started.  The length of interval that metrics
// are periodically collected over is configured by the
// 'btemt_ChannelPoolConfiguration' supplied at construction.
//
// For calculating the percentage of CPU time used by the channel pool given
// that channel pool manages 'n' event pollers at a moment (with the maximum
// of the number of threads, 'T'), the sum of workloads is taken over the
// set of 'T' event managers, and the workload is 0 for non-existent
// managers.  Then the sum is divided by the number of event managers, T.
// The result reflects the workload of the channel pool as "percent busy".
// Strictly speaking, the following is true:
//..
//  ] T - maximum number of event managers
//  ] n - current number of event managers.
//  Then, the total workload is
//
//       T
//      __
//      \ '                                         -
//      /  workload(ev )   , where workload(ev ) = | CPU time/(CPU + I/0 time)
//      --            i                       i    | or
//      i=1                                        | 0 iff i > n
//  W = ----------------                            -
//            T
//..
//
///Thread Safety
///-------------
// The channel pool is *thread-enabled* meaning that any operation on the same
// instance can be invoked from any thread.  The configuration is thread-safe,
// but not thread-enabled and requires explicit synchronization in the user
// space.  A user-defined callback can be invoked from *any* (managed) thread
// and the user must account for that.
//
///Usage
///-----
// In this section, we show two usage examples.  The first illustrates the
// basics of establishing a connection.  The second illustrates how the channel
// pool can be used for a simple echo server.  Since echoing is not
// data-dependent, the data callback simply echoes back as soon as it gets a
// (portion of a) message.
//
///Usage Example 1
///- - - - - - - -
// The following snippets of code illustrate how to establish connection
// to a remote host.  First of all, we need to create a callback to be invoked
// once the channel status change (i.e., a new connection is established, in
// this case):
//..
//  struct my_LocalCallback {
//      int d_sourceId;
//      void channelStateCb(int                 channelId,
//                          int                 sourceId,
//                          int                 status,
//                          void               *arg,
//                          btemt_ChannelPool **poolAddr)
//      {
//          assert(sourceId == d_sourceId);
//          if (btemt_ChannelPool::BTEMT_CHANNEL_DOWN == status) {
//              // Client disconnected from the server.
//              assert(poolAddr && *poolAddr);
//              (*poolAddr)->shutdown(channelId,
//                                    btemt_ChannelPool::BTEMT_IMMEDIATE);
//          } else
//          if (btemt_ChannelPool::BTEMT_CHANNEL_UP == status) {
//              // Connected to the server.
//              // ...
//          }
//          else {
//              // Handle various failure modes
//              // ...
//          }
//      }
//  };
//..
// Secondly, we need to create a configuration for the channel pool:
//..
//  int main(int argc, char *argv[]) {
//      my_LocalCallback localCallback;
//      btemt_ChannelPoolConfiguration config;
//      config.setMaxThreads(4);
//      config.setMetricsInterval(10.0);
//      config.setMaxConnections(16);
//      config.setIncomingMessageSizes(1, 128, 256);
//
//      bdef_Function<void (*)(int, int, int, void*)>
//                                          ccb;    // channel state callback
//      bdef_Function<void (*)(int, int*, int*, void*)>
//                                          dcb;    // data callback
//      bdef_Function<void (*)(int, int, int)>
//                                          pcb;    // pool state callback
//
//      bcemt_ChannelPool *poolAddr;
//      ccb = bdef_BindUtil::bind( &my_LocalCallback::channelStateCb
//                               , &local)
//                               , _1, _2, _3, _4
//                               , &poolAddr);
//      makeNull(&dcb);  // not interested in data
//      makeNull(&pcb);  // not interested in pool state
//
//      localCallback.d_sourceId = 5;    // just for a simple verification
//
//      btemt_ChannelPool pool(ccb, dcb, pcb, config);
//      poolAddr = &pool;
//..
// Now, start the channel pool, issue the connect request, and wait for
// completion.  Note that main thread is never blocked, so we have to put it
// to sleep explicitly:
//..
//      assert(0 == pool.start());
//      bteso_IPv4Address peer("127.0.0.1", 7); // echo server
//      assert(0 == pool.connect(peer, 1, bdet_TimeInterval(10.0), 5));
//      bcemt_ThreadUtil::sleep(15000000); // Give enough time to connect.
//      return 0;
//  }
//..
//
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
//  class my_EchoServer {
//      // This class implements a multi-user multi-threaded echo server.
//
//      enum {
//          SERVER_ID = 0xAB   // An (arbitrary) constant (passed to 'listen')
//                             // that identifies the channel pool operation
//                             // associated with a pool state or channel
//                             // state callback.
//      };
//
//      // DATA
//      btemt_ChannelPoolConfiguration d_config;        // pool's configuration
//      btemt_ChannelPool             *d_channelPool_p; // managed pool
//      bslma_Allocator               *d_allocator_p;   // memory manager
//      bcemt_Mutex                   *d_coutLock_p;    // synchronize 'cout'
//
//    private:
//      // Callback functions:
//      void poolStateCb(int state, int source, int severity);
//          // Output a message to 'stdout' indicating the specified 'state'
//          // associated with the specified 'source' has occurred, with the
//          // specified 'severity'.  Note that 'state' is one of the
//          // 'btemt_PoolMsg' constants (see 'btemt_message'), 'source'
//          // identifies the channel pool operation associated with this state
//          // (in this case, the 'SERVER_ID' passed to 'listen()' or 0 for
//          // pool states with no associated source), and 'severity' is one
//          // of the 'btemt_ChannelPool::Severity' values.
//
//      void channelStateCb(int channelId, int sourceId, int state, void *ctx);
//          // Output a message to 'stdout' indicating the specified 'state',
//          // associated with the specified 'channelId' and 'sourceId', has
//          // occurred.  If 'state' is 'btemt_ChannelPool::BTEMT_CHANNEL_DOWN'
//          // then shutdown the channel.  Note that the 'channelId' is a
//          // unique identifier chosen by the channel pool for each connection
//          // channel, 'sourceId' identifies the channel pool operation
//          // responsible for creating the channel (in this case, the
//          // 'SERVER_ID' passed to 'listen()'), 'state' is a
//          // 'btemt_ChannelPool::ChannelState' enumeration value, and 'ctx'
//          // is the address of a context object provided for the channel
//          // (using 'setChannelContext()'), in this example we do not
//          // specify a context, so the value will be 0.
//
//      void dataCb(int           *numConsumed,
//                  int           *numNeeded,
//                  btemt_DataMsg  msg,
//                  void          *context);
//          // Echo the specified 'msg' to the client on the channel
//          // identified by 'msg.channelId()' channel, load into the
//          // specified 'numConsumed' the number of bytes processed from
//          // 'msg', load into 'numNeeeded' the minimum length of
//          // additional data that is needed to complete a message, and close
//          // the communication channel.  Because this echo server is not
//          // interested in a discrete messages in a particular message
//          /// format, 'numConsumed' will always be set to the length of
//          // 'msg', and 'numNeeded' will be set to 1 (indicating this
//          // callback should be invoked again as soon as any new data is
//          // read).
//
//      // NOT IMPLEMENTED
//      my_EchoServer(const my_EchoServer&);
//      my_EchoServer& operator=(const my_EchoServer&);
//
//    public:
//      my_EchoServer(bcemt_Mutex     *coutLock,
//                    int              portNumber,
//                    int              numConnections,
//                    bslma_Allocator *basicAllocator = 0);
//          // Create an echo server that listens for incoming connections on
//          // the specified 'portNumber' managing up to the specified
//          // 'numConnections' simultaneous connections.  The echo server
//          // will use the specified 'coutLock' to synchronize access to the
//          // standard output.  Optionally specify a 'basicAllocator' used to
//          // supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.  The behavior is undefined
//          // unless 'coutLock' is a valid address for a mutex object.
//
//      ~my_EchoServer();
//          // Destroy this server.
//
//      // MANIPULATORS
//      const btemt_ChannelPool& pool() { return *d_channelPool_p; }
//  };
//..
// In the constructor of 'my_EchoServer', the configuration is initialized,
// the channel pool is created, configured, and started.  The listening port
// is established:
//..
//  my_EchoServer::my_EchoServer(bcemt_Mutex     *coutLock,
//                               int              portNumber,
//                               int              numConnections,
//                               bslma_Allocator *basicAllocator)
//  : d_allocator_p(bslma_Default::allocator(basicAllocator))
//  , d_coutLock_p(coutLock) {
//      d_config.setMaxThreads(4);
//      d_config.setMaxConnections(numConnections);
//      d_config.setReadTimeout(5.0);       // in seconds
//      d_config.setMetricsInterval(10.0);  // seconds
//      d_config.setMaxWriteCache(1<<10);   // 1MB
//      d_config.setIncomingMessageSizes(1, 100, 1024);
//
//      btemt_ChannelPool::ChannelStateChangeCallback channelStateFunctor(
//              &my_EchoServer::channelStateCb
//            , this)
//            , basicAllocator);
//
//      btemt_ChannelPool::PoolStateChangeCallback poolStateFunctor(
//              &my_EchoServer::poolStateCb
//            , this)
//            , basicAllocator));
//
//      btemt_ChannelPool::DataReadCallback dataFunctor(
//              &my_EchoServer::dataCb
//            , this)
//            , basicAllocator));
//
//      d_channelPool_p = new (*d_allocator_p)
//          btemt_ChannelPool(channelStateFunctor,
//                            dataFunctor,
//                            poolStateFunctor,
//                            d_config,
//                            basicAllocator);
//
//      assert(0 == d_channelPool_p->start());
//      assert(0 == d_channelPool_p->listen(portNumber,
//                                          numConnections,
//                                          SERVER_ID));
//  }
//..
// Destructor just stops the pool and destroys it:
//..
//  my_EchoServer::~my_EchoServer() {
//      d_channelPool_p->stop();
//      d_allocator_p->deleteObject(d_channelPool_p);
//  }
//..
// The pool state callback will just print the new state.  The channel state
// callback will report a new state and the address of the peer.  The data
// state callback will immediately write data back to the channel pool.  These
// methods are documented in the example header, and the implementation for
// these methods is shown below:
//..
//  void my_EchoServer::poolStateCb(int state, int source, int severity) {
//      d_coutLock_p->lock();
//      cout << "Pool state changed: ("
//           << source << ", "
//           << severity << ", "
//           << state << ") " << endl;
//      d_coutLock_p->unlock();
//  }
//
//  void my_EchoServer::channelStateCb(int   channelId,
//                                     int   sourceId,
//                                     int   state,
//                                     void *context)
//  {
//      assert(SERVER_ID == sourceId);
//
//      switch(state) {
//        case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
//            bteso_IPv4Address peer;
//            d_channelPool_p->getPeerAddress(&peer, channelId);
//            d_coutLock_p->lock();
//            cout << "Client from " << peer << " has disconnected." << endl;
//            d_coutLock_p->unlock();
//            d_channelPool_p->shutdown(channelId,
//                                      btemt_ChannelPool::BTEMT_IMMEDIATE);
//        } break;
//        case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
//            bteso_IPv4Address peer;
//            d_channelPool_p->getPeerAddress(&peer, channelId);
//            d_coutLock_p->lock();
//            cout << "Client connected from " << peer << endl;
//            d_coutLock_p->unlock();
//        } break;
//      }
//  }
//
//  void my_EchoServer::dataCb(int           *numConsumed,
//                             int           *numNeeded,
//                             btemt_DataMsg  msg,
//                             void          *context)
//  {
//      assert(numConsumed);
//      assert(msg.data());
//      assert(0 < msg.data()->length());
//
//      assert(0 == d_channelPool_p->write(msg.channelId(), msg));
//
//      *numConsumed = msg.data()->length();
//      *numNeeded   = 1;
//
//      d_channelPool_p->shutdown(msg.channelId(),
//                                btemt_ChannelPool::BTEMT_IMMEDIATE);
//  }
//..
// The implementation of an echo server is now complete.  Let's create
// a small program that uses it.  We will create a server object, then
// the main thread will monitor the channel pool and periodically print
// its busy metrics.  For simplicity, we will use the following function
// for monitoring:
//..
//  static inline void  monitorPool(bcemt_Mutex              *coutLock,
//                                  const btemt_ChannelPool&  pool,
//                                  int                       numTimes)
//      // Every 10 seconds, output the percent busy of the specified channel
//      // 'pool' to the standard output, using the specified 'coutLock' to
//      // synchronizing access to the standard output stream; return to the
//      // caller after 'numTimes' output operations (i.e. numTimes * 10
//      // seconds).
//  {
//      while(--numTimes > 0) {
//          coutLock->lock();
//          cout << "The pool is " << pool.busyMetrics() << "% busy ("
//              << pool.numThreads() << " threads)." << endl;
//          coutLock->unlock();
//          bcemt_ThreadUtil::sleep(bdet_TimeInterval(10*1E6));  // 10 seconds
//      }
//  }
//..
// The main function is shown below:
//..
//   int main() {
//       enum {
//           PORT_NUMBER     = 1423
//         , MAX_CONNECTIONS = 1000
//         , NUM_MONITOR     = 50
//       };
//       bcemt_Mutex coutLock;
//       my_EchoServer echoServer(&coutLock, PORT_NUMBER, MAX_CONNECTIONS);
//       monitorPool(&coutLock, echoServer.pool(), NUM_MONITOR);
//       return 0;
//
//   }
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTEMT_CHANNELPOOLCONFIGURATION
#include <btemt_channelpoolconfiguration.h>
#endif

#ifndef INCLUDED_BTEMT_CHANNELTYPE
#include <btemt_channeltype.h>
#endif

#ifndef INCLUDED_BTEMT_MESSAGE
#include <btemt_message.h>
#endif

#ifndef INCLUDED_BTESO_INETSTREAMSOCKETFACTORY
#include <bteso_inetstreamsocketfactory.h>
#endif

#ifndef INCLUDED_BTEMT_TCPTIMEREVENTMANAGER
#include <btemt_tcptimereventmanager.h>
#endif

#ifndef INCLUDED_BTES_IOVECUTIL
#include <btes_iovecutil.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETHANDLE
#include <bteso_sockethandle.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
// This must be included before <bcec_objectcatalog.h> in order to avoid
// triggering a Sun CC 5.2 compiler bug.
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEC_OBJECTCATALOG
#include <bcec_objectcatalog.h>
#endif

#ifndef INCLUDED_BCEF_VFUNC3
#include <bcef_vfunc3.h>        // @DEPRECATED
#endif

#ifndef INCLUDED_BCEF_VFUNC4
#include <bcef_vfunc4.h>        // @DEPRECATED
#endif

#ifndef INCLUDED_BCEMA_BLOB
#include <bcema_blob.h>
#endif

#ifndef INCLUDED_BCEMA_POOLALLOCATOR
#include <bcema_poolallocator.h>
#endif

#ifndef INCLUDED_BCEMA_POOLEDBLOBBUFFERFACTORY
#include <bcema_pooledblobbufferfactory.h>
#endif

#ifndef INCLUDED_BCEMA_POOLEDBUFFERCHAIN
#include <bcema_pooledbufferchain.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BCES_ATOMICUTIL
#include <bces_atomicutil.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifdef BSLS_PLATFORM__OS_UNIX
#ifndef INCLUDED_BSL_C_LIMITS
#include <bsl_c_limits.h>      // for IOV_MAX
#endif
#endif

namespace BloombergLP {

// Used in name only
class bteso_IPv4Address;
class bteso_SocketOptions;
class btesc_TimedCbChannel;
class btesc_TimedCbChannelAllocator;

// Local classes
class btemt_Channel;
class btemt_Connector;
class btemt_ServerState;

                       //========================
                       // struct btemt_TimerState
                       //========================

struct btemt_TimerState {
    // Provide a description of a scheduled timer event.  Note that a
    // 'btemt_ChannelPool' associates a 'btemt_TimerState' object with each
    // timer callback it registers with an underlying
    // 'btemt_TcpTimerEventManager'.
    //
    // This class is an implementation detail of 'btemt_ChannelPool', and is
    // *not* intended to be used in client code.

    void                       *d_eventManagerId; // identifies the timer in
                                                  // 'd_eventManager_p'

    btemt_TcpTimerEventManager *d_eventManager_p; // event manager the timer
                                                  // is registered with

    bdet_TimeInterval           d_absoluteTime;   // next scheduled occurrence
                                                  // (as an offset from the
                                                  // epoch time)

    bdet_TimeInterval           d_period;         // if a positive value, the
                                                  // periodic interval for
                                                  // the timer; otherwise it
                                                  // is a non-recurring timer

    bdef_Function<void (*)()>   d_callback;       // callback function to
                                                  // invoke
};

                       //=========================
                       // struct btemt_ChannelPool
                       //=========================

class btemt_ChannelPool {
    // This class provides a channel pool, i.e., a mechanism by which
    // connections can be established and managed.  This channel pool allows
    // the establishment of both server channels (see section "Server part" of
    // the manipulators section) via the 'listen' and 'close' methods, and
    // client channels (see section "Client part") via the 'connect' method,
    // and provide the 'disableRead', 'enableRead', and 'shutdown' methods for
    // managing the channels once they have been created (see the section
    // "Channel management").  In addition, it allows the communication of
    // messages through any of the channels (see the section "Outgoing
    // Messages") via the 'write' methods; the processing of incoming data to
    // any of the channels is done asynchronously through a data callback
    // passed at construction of this pool.  An existing socket can be
    // imported and this will create a channel enabled both for read and for
    // write.  This channel pool can dispatch events to be executed at
    // different times at recurring intervals (see the section "Clock
    // management").    All this processing will be performed in a number of
    // threads.  The channel pool can be started or stopped (see the section
    // "Threads management").  Once started and until stopped, the channel pool
    // dispatches incoming and outgoing connections, messages, and other
    // channel functions to the processing threads.  Once stopped, the channel
    // pool can be started again and the channels will resume their operations.
    // This channel pool keeps a set of metrics (see the "Metrics" section).
    // It can be configured at construction by passing a
    // 'btemt_ChannelPoolConfiguration' object.

  public:
    // TYPES
    typedef bcef_Vfunc4<int, int, int, void*> ChannelStateCallback;
        // @DEPRECATED - use 'ChannelStateChangeCallback'

    typedef bdef_Function<void (*)(int, int, int, void*)>
                                                    ChannelStateChangeCallback;
        // The callback of this type is invoked whenever a channel's state
        // changes.  The first argument is the (unique) channel ID, chosen by
        // the channel pool, that identifies the connection channel that
        // changed state.  The second argument is the source ID, i.e., the
        // client-provided identifier passed to the channel pool, identifying
        // the operation responsible for creating the channel (e.g., the
        // 'serverId' parameter of 'listen()' or the 'sourceId' parameter of
        // 'connect()').  The third argument is the new state of this channel
        // (can be any one of the 'ChannelEvents' enumerations).  The fourth
        // and last parameter is passed the user-specified (via
        // 'setChannelContext') channel context or '(void*)0' if no context
        // was specified.  Users MUST handle the 'BTEMT_CHANNEL_DOWN' event,
        // minimally by calling 'shutdown' on the channel ID.  The prototype
        // for a channel state callback might look like:
        //..
        //  void channelStateCallback(int   channelId,
        //                            int   sourceId,
        //                            int   state,
        //                            void *context);
        //..

    typedef bcef_Vfunc4<int *, int *, btemt_DataMsg, void*> DataCallback;
        // @DEPRECATED - use 'DataReadCallback'

    typedef bdef_Function<void (*)(int *, int *, const btemt_DataMsg&, void*)>
                                                              DataReadCallback;
        // The callback of this type is invoked every time data is read from a
        // channel.  The third argument to this callback is passed the data
        // read from the channel (including the channel ID, found using
        // 'btemt_DataMsg::channelId()').  The fourth and last parameter is
        // passed the user-specified channel context (set using
        // 'setChannelContext()') or '(void*)0' if no context was specified.
        // The callback, when invoked, must indicate the length of consumed
        // data by storing it into the first argument of the callback.  It
        // should store into the second argument the minimum length of
        // additional data that is needed to complete a message.  The
        // channel will cache the remaining data (that was not consumed) and
        // deliver it on the next invocation of this callback.  If there is
        // not enough data for a single message for a particular protocol, 0
        // must be stored into the first argument.  If at least one byte is
        // consumed, the user takes the ownership of the message; the channel
        // will use message's buffer only for the copying out the remaining
        // (un-consumed) bytes.  The prototype for a data callback might look
        // like:
        //..
        //  void dataCallback(int           *numConsumed,
        //                    int           *numNeeded,
        //                    btemt_DataMsg  message,
        //                    void          *context);
        //..

    typedef bdef_Function<void (*)(int *, bcema_Blob *, int, void *)>
                                                         BlobBasedReadCallback;
        // The callback of this type is invoked every time there is a
        // sufficiently large amount of data read from a channel.  The second
        // argument to this callback is passed the data read from the channel
        // in the form of a modifiable 'bcema_Blob'.  The channel pool expects
        // that clients take ownership of some of the data in the the passed
        // 'bcema_Blob' and readjust the the 'bcema_Blob' accordingly.  The
        // third argument specifies the channel ID.  The fourth and last
        // parameter is passed the user-specified channel context (set using
        // 'setChannelContext()') or '(void *)0' if no context was specified.
        // The callback, when invoked, must store into the first argument the
        // minimum length of additional data that is needed to complete a
        // message.  If there is not enough data for a single message of a
        // particular protocol, 0 must be stored into the first argument.  The
        // prototype for a data callback might look like:
        //..
        //  void blobDataCallback(int        *numNeeded,
        //                        bcema_Blob *message,
        //                        int         channelId,
        //                        void       *context);
        //..

    typedef bcef_Vfunc3<int, int, int> PoolStateCallback;
        // @DEPRECATED - use 'PoolStateChangeCallback'

    typedef bdef_Function<void (*)(int, int, int)> PoolStateChangeCallback;
        // The callback of this type is invoked whenever a change affecting the
        // pool occurs.  The first parameter indicates the type of event which
        // triggered the callback (i.e., one of the 'btemt_PoolMsg::PoolState'
        // enumerations).  The second parameter indicates the source of the
        // event (e.g., the 'serverId' passed to 'listen()' or the 'sourceId'
        // passed to 'connect()'), or is 0 if there is no associated source
        // (e.g. a 'CHANNEL_LIMIT' alert).  The third parameter indicates the
        // severity of the event (must be one of the 'Severity' enumerations).
        // The prototype of a pool state callback might look like:
        //..
        //  void poolStateCallback(int eventType, int sourceId, int severity);
        //..

    enum ChannelEvents {
        // This enumeration provides names for the different values passed into
        // the third argument to 'ChannelCallback' to discriminate between
        // various changes in the state of a channel.

        BTEMT_CHANNEL_DOWN         = 0,
        BTEMT_CHANNEL_UP           = 1,
        BTEMT_READ_TIMEOUT         = 2,
        BTEMT_WRITE_BUFFER_FULL    = 3,
        BTEMT_MESSAGE_DISCARDED    = 4,
        BTEMT_AUTO_READ_ENABLED    = 5,
        BTEMT_AUTO_READ_DISABLED   = 6,
        BTEMT_WRITE_CACHE_LOWWAT   = 7,            // write cache low watermark
        BTEMT_WRITE_CACHE_HIWAT    = BTEMT_WRITE_BUFFER_FULL,
                                                   // write cache hi watermark
        BTEMT_CHANNEL_DOWN_READ    = 8,
        BTEMT_CHANNEL_DOWN_WRITE   = 9
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , CHANNEL_DOWN       = BTEMT_CHANNEL_DOWN
      , CHANNEL_UP         = BTEMT_CHANNEL_UP
      , READ_TIMEOUT       = BTEMT_READ_TIMEOUT
      , WRITE_BUFFER_FULL  = BTEMT_WRITE_BUFFER_FULL
      , MESSAGE_DISCARDED  = BTEMT_MESSAGE_DISCARDED
      , AUTO_READ_ENABLED  = BTEMT_AUTO_READ_ENABLED
      , AUTO_READ_DISABLED = BTEMT_AUTO_READ_DISABLED
      , WRITE_CACHE_LOWWAT = BTEMT_WRITE_CACHE_LOWWAT
      , WRITE_CACHE_HIWAT  = BTEMT_WRITE_CACHE_HIWAT
      , CHANNEL_DOWN_READ  = BTEMT_CHANNEL_DOWN_READ
      , CHANNEL_DOWN_WRITE = BTEMT_CHANNEL_DOWN_WRITE
#endif
    };

    enum ConnectResolutionMode {
        // Mode indicating whether to perform name resolution at each connect
        // attempt in 'connect'.

        BTEMT_RESOLVE_ONCE            = 0,  // perform resolution once prior
                                            // to the first connect attempt

        BTEMT_RESOLVE_AT_EACH_ATTEMPT = 1   // perform resolution prior to each
                                            // connect attempt
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , RESOLVE_ONCE            = BTEMT_RESOLVE_ONCE
      , RESOLVE_AT_EACH_ATTEMPT = BTEMT_RESOLVE_AT_EACH_ATTEMPT
#endif
    };

    enum KeepHalfOpenMode {
        // Mode affecting how half-open connections are handled by a server or
        // a client channel, passed to 'connect', 'import' or 'listen'.

        BTEMT_CLOSE_BOTH         = 0,  // close whole channel if half-open
                                       // connection

        BTEMT_KEEP_HALF_OPEN     = 1   // keep either part alive, if the other
                                       // half senses a closed connection by
                                       // the peer
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , CLOSE_BOTH     = BTEMT_CLOSE_BOTH
      , KEEP_HALF_OPEN = BTEMT_KEEP_HALF_OPEN
#endif
    };

    enum ShutdownMode {
        // Mode affecting how channel is terminated, passed to 'shutdown'.

        BTEMT_IMMEDIATE = 0  // The channel is terminated immediately, all
                             // pending messages are discarded.
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , IMMEDIATE = BTEMT_IMMEDIATE
#endif
    };

    enum Severity {
        // This enumeration provides names for different levels of severity.

        BTEMT_CRITICAL  = 0, // A critical condition occurred and the channel
                             // pool is unable to operate normally.

        BTEMT_ALERT     = 1  // An alerting condition occurred and the channel
                             // pool can operate normally.
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , CRITICAL = BTEMT_CRITICAL
      , ALERT    = BTEMT_ALERT
#endif
    };

    struct HandleInfo {
        // This 'struct' contains information about an open file descriptor,
        // namely the channel ID of the channel that uses this file descriptor,
        // its manager ID (and thread ID of that manager's dispatcher thread),
        // and the server ID (for a socket created by a call to 'listen', or a
        // channel accepted by that socket) or source ID (for a channel created
        // by a call to 'connect' or 'import').  Some channel types correspond
        // to a channel in process of creation (during 'connect') and may not
        // yet have a channel ID, in which case -1 is used for 'd_channelId'
        // and for 'd_userId' instead.

        bteso_SocketHandle::Handle  d_handle;       // socket handle (file
                                                    // descriptor)

        btemt_ChannelType::Value    d_channelType;  // indicates how the
                                                    // channel was created

        int                         d_channelId;    // channel using this
                                                    // file descriptor

        bdet_TimeInterval           d_creationTime; // when was this channel
                                                    // created

        bcemt_ThreadUtil::Handle    d_threadHandle; // manager's dispatcher
                                                    // thread

        int                         d_userId;       // 'serverId' or 'sourceId'
    };

  private:
    // PRIVATE TYPES
    typedef bcema_SharedPtr<btemt_Channel>     ChannelHandle;
    typedef bcema_SharedPtr<btemt_ServerState> ServerHandle;

    // INSTANCE DATA
                                        // *** Transport-related state ***
    bcec_ObjectCatalog<ChannelHandle>   d_channels;
    bsl::vector<btemt_TcpTimerEventManager *>
                                        d_managers;
    mutable bcemt_Mutex                 d_managersLock;

    bsl::map<int, btemt_Connector>      d_connectors;
    mutable bcemt_Mutex                 d_connectorsLock;

    bsl::map<int, ServerHandle>         d_acceptors;
    mutable bcemt_Mutex                 d_acceptorsLock;

    bcema_PoolAllocator                 d_sharedPtrRepAllocator;
    bcema_PooledBufferChainFactory      d_messageFactory;
    bcema_PooledBufferChainFactory      d_vecMessageFactory;
    bcema_PooledBlobBufferFactory       d_writeBlobFactory;
    bcema_PooledBlobBufferFactory       d_readBlobFactory;

    bcemt_Mutex                         d_timersLock;
    bsl::map<int, btemt_TimerState>     d_timers;

                                        // *** Parameters ***
    btemt_ChannelPoolConfiguration      d_config;
    bces_AtomicUtil::Int                d_capacity;
    int                                 d_startFlag;
    bool                                d_collectTimeMetrics;
                                               // whether to collect time
                                               // metrics

                                        // *** Capacity monitoring ***
    void                               *d_metricsTimerId;
    bdef_Function<void (*)()>           d_metricsFunctor;
    ChannelStateChangeCallback          d_channelStateCb;
    PoolStateChangeCallback             d_poolStateCb;
    DataReadCallback                    d_pooledBufferChainBasedReadCb;
    BlobBasedReadCallback               d_blobBasedReadCb;
    bool                                d_useBlobForDataReads;

                                        // *** Metrics ***
    bces_AtomicInt                      d_totalConnectionsLifetime;
    bdet_TimeInterval                   d_lastResetTime;
    volatile bsls_PlatformUtil::Int64   d_totalBytesReadAdjustment;
                                               // adjustment to
                                               // the sum of individual
                                               // channel numBytesRead(),
                                               // accounting for closed
                                               // channels and calls to
                                               // 'resetTotalBytesRead'

    volatile bsls_PlatformUtil::Int64   d_totalBytesWrittenAdjustment;
                                               // adjustment to
                                               // the sum of individual
                                               // channel numBytesWritten(),
                                               // accounting for closed
                                               // channels and calls to
                                               // 'resetTotalBytesWritten'

    volatile bsls_PlatformUtil::Int64   d_totalBytesRequestedWrittenAdjustment;
                                               // adjustment to
                                               // the sum of individual
                                               // channel values,
                                               // accounting for closed
                                               // channels and calls to
                                               // reset

    mutable bcemt_Mutex                 d_metricAdjustmentMutex;
                                               // synchronize operations on
                                               // two metric adjustment values

                                        // *** Memory allocation ***
    bteso_InetStreamSocketFactory<bteso_IPv4Address>
                                        d_factory;

    bcema_Pool                          d_pool;        // for btemt_Channel
                                                       // (owned)

    bslma_Allocator                    *d_allocator_p; // (held, not owned)

  private:
    // FRIENDS
    friend class btemt_Channel;

    // PRIVATE MANIPULATORS
    btemt_TcpTimerEventManager *allocateEventManager();
        // From the set of current event managers, find the most idle one
        // (i.e., having the minimal percent CPU busy) and return its address.
        // Return the address of event manager on success, and 0 otherwise, in
        // which case 'status' will be loaded with a non-zero value.
        //
        // LOCKING: This function doesn't lock any synchronization primitives.

    void init();
        // Initialize this channel pool.

                                  // *** Server part ***
    void acceptCb(int serverId, ServerHandle server);
        // Add a newly allocated channel to the set of channels managed by this
        // channel pool and invoke the channel pool callback.  Note that this
        // method is executed whenever a connection is accepted on the
        // listening socket corresponding to the server whose ID is
        // 'it->first'.  All other information (e.g., listening socket and
        // event manager) is held in the server state 'it->second'.

    void acceptRetryCb(int serverID, ServerHandle server);
        // Re-register listening socket for the server whose ID is 'serverId'
        // to match 'ACCEPT' events (and invoke callback 'acceptCb' on such
        // events).  This callback is called after no resources were available
        // to accept a connection, and the socket was subsequently
        // deregistered.  This callback is scheduled in an exponential backoff
        // sequence fashion.  The exponential series is reset once a call to
        // 'accept' stops returning
        // 'bteso_SocketHandle::BTESO_ERROR_NORESOURCES'.

    void acceptTimeoutCb(int serverId, ServerHandle server);
        // Issue a pool callback with 'ACCEPT_TIMEOUT' and re-schedule this
        // timeout callback for the server whose ID is 'it->first', if the
        // listening socket held in the server state 'it->second' did not
        // receive a connection attempt in the timeout period specified in the
        // server state since the last server connection or last timeout
        // callback.

    int listen(const bteso_IPv4Address&   endpoint,
               int                        backlog,
               int                        serverId,
               int                        reuseAddress,
               bool                       readEnabledFlag,
               KeepHalfOpenMode           mode,
               bool                       isTimedFlag,
               const bdet_TimeInterval&   timeout = bdet_TimeInterval(),
               const bteso_SocketOptions *socketOptions = 0);
        // Establish a listening socket having the specified 'backlog' maximum
        // number of pending connections on the specified 'endpoint' and the
        // specified 'reuseAddress' used in setting 'REUSEADDRESS' socket
        // option, and associate this newly established socket with the
        // specified 'serverId'.  If the specified 'readEnabledFlag' is
        // non-zero, any channel created by 'acceptCb' will be enabled for read
        // upon creation, and otherwise it will not.  If the specified
        // 'isTimedFlag' is non-zero, register a timer which will execute
        // 'acceptTimeoutCb' in the dispatcher thread of the event manager for
        // this server, if no connection attempt is received for the optionally
        // specified 'timeout' period since the last connection or the last
        // timeout.  Optionally specify 'socketOptions' that will be used to
        // specify what options should be set on the listening socket.  Return
        // 0 on success, a positive value if there is a listening socket
        // associated with 'serverId' (i.e., 'serverId' is not unique) and a
        // negative value if an error occurred.  The behavior is undefined
        // unless 0 < 'backlog'.

                                  // *** Client part ***
    void connectCb(bsl::map<int,btemt_Connector>::iterator it);
        // Add a newly allocated channel to the set of channels managed by this
        // channel pool and invoke the channel state callback.  Note that this
        // method is executed whenever a valid connection is established
        // corresponding to the client whose ID is 'it->first'.  All other
        // information (e.g., connection socket and event manager) is held in
        // the connector state 'it->second'.

    void connectEventCb(bsl::map<int,btemt_Connector>::iterator it);
        // Check the connection status of the connection socket upon call back
        // from the socket event 'bteso_EventType::BTESO_CONNECT'.  If the
        // connection is valid then call 'connectCb', otherwise this socket
        // timed out and we must close it, reopen another socket, and
        // re-attempt a connection.  All other information (e.g., connection
        // socket and event manager) is held in the connector state
        // 'it->second'.

    void connectInitiateCb(bsl::map<int,btemt_Connector>::iterator it);
        // Initiate a connection for the client whose clientId is 'it->first',
        // and upon success proceed to 'connectCb'.  Otherwise, register
        // 'connectEventCb' for when this connection is established or times
        // out, or upon failure, invoke a pool state callback with
        // 'ERROR_CONNECTING', clientId given by the specified 'it->first', and
        // severity 'BTEMT_ALERT'.  All other information (e.g., connection
        // socket and event manager) is held in the connector state
        // 'it->second'.

    void connectTimeoutCb(bsl::map<int,btemt_Connector>::iterator it);
        // Decrease the number of attempts held in the connector state.  Once
        // the number of attempts reaches zero, the connection event is
        // deregistered and the connector removed from 'd_connectors'.  Note
        // that this callback is invoked if the connecting socket held in the
        // connector state 'it->second' did not establish a connection in the
        // timeout period specified in the server state, either through
        // 'connectInitiateCb' or through a 'connectEventCb' after the last
        // 'connectInitiateCb'.

                                  // *** Channel management part ***
    void importCb(
                bteso_StreamSocket<bteso_IPv4Address>        *socket,
                bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                btemt_TcpTimerEventManager                   *manager,
                btemt_TcpTimerEventManager                   *srcManager,
                int                                           sourceId,
                bool                                          readEnabledFlag,
                bool                                          mode,
                bool                                          imported);
        // Add a newly allocated channel to the set of channels managed by this
        // channel pool and invoke the channel pool callback in the specified
        // 'manager'.  Upon destruction, 'socket' will be destroyed via the
        // specified 'factory'.  Note that this method is executed whenever a
        // connection is imported on the 'socket' corresponding to
        // 'sourceId'.  In addition, it is invoked by 'connectCb' to create a
        // newly allocated channel once the socket connection is established.
        // This function should be executed in the dispatcher thread of the
        // specified 'srcManager'.

                                  // *** Clock management ***

    void timerCb(int timerId);
        // Execute the callback associated with the timer or clock whose ID is
        // the specified 'timerId' whenever the current timeout of this timer
        // or clock expires, and if the 'timerId' corresponds to a clock then
        // re-register the clock for the next period.

                                  // *** Metrics ***
    void metricsCb();
        // Update metrics for each event manager.

    // PRIVATE ACCESSORS
    int findChannelHandle(ChannelHandle *handle, int channelId) const;
        // Load into 'handle' a shared-pointer to the channel associated with
        // the specified 'channelId'.  Return 0 on success, or a non-zero
        // value if there is no valid channel channel is associated with
        // 'channelId'.  Note that a channel handle in 'd_channels' may be
        // null, if the channel has been added but not yet initialized.

  private:
    // NOT IMPLEMENTED
    btemt_ChannelPool(const btemt_ChannelPool& original);
    btemt_ChannelPool& operator=(const btemt_ChannelPool& rhs);

  public:
    // CREATORS
    btemt_ChannelPool(
           ChannelStateCallback                   channelStateCb,
           DataCallback                           pooledBufferChainBasedReadCb,
           PoolStateCallback                      poolStateCb,
           const btemt_ChannelPoolConfiguration&  parameters,
           bslma_Allocator                       *basicAllocator = 0);
        // Create a channel pool with the specified 'channelStateCb',
        // 'pooledBufferChainBasedReadCb' and 'poolStateCb' callbacks to be
        // invoked, correspondingly, when a channel state changes, data
        // arrives, or pool state changes.  The channel pool is configured
        // using the specified configuration 'parameters'.  Optionally specify
        // a 'basicAllocator' used to supply memory (except for messages).  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
        //
        // The exact description of the argument to the callbacks
        // are given in the type definitions of the
        // 'ChannelStateChangeCallback', 'DataReadCallback', and
        // 'PoolStateChangeCallback'.
        //
        // @DEPRECATED - use the constructor that uses bdef_Function parameters

    btemt_ChannelPool(
           ChannelStateChangeCallback             channelStateCb,
           DataReadCallback                       pooledBufferChainBasedReadCb,
           PoolStateChangeCallback                poolStateCb,
           const btemt_ChannelPoolConfiguration&  parameters,
           bslma_Allocator                       *basicAllocator = 0);
        // Create a channel pool with the specified 'channelStateCb',
        // 'pooledBufferChainBasedReadCb' and 'poolStateCb' callbacks to be
        // invoked, correspondingly, when a channel state changes, data
        // arrives, or pool state changes.  The channel pool is configured
        // using the specified configuration 'parameters'.  Optionally specify
        // a 'basicAllocator' used to supply memory (except for messages).  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
        //
        // The exact description of the argument to the callbacks
        // are given in the type definitions of the
        // 'ChannelStateChangeCallback', 'DataReadCallback', and
        // 'PoolStateChangeCallback'.

    btemt_ChannelPool(
            ChannelStateCallback                   channelStateCb,
            BlobBasedReadCallback                  blobBasedReadCb,
            PoolStateCallback                      poolStateCb,
            const btemt_ChannelPoolConfiguration&  parameters,
            bslma_Allocator                       *basicAllocator = 0);
        // Create a channel pool with the specified 'channelStateCb',
        // 'blobBasedReadCb' and 'poolStateCb' callbacks to be invoked,
        // correspondingly, when a channel state changes, data arrives, or
        // pool state changes.  The channel pool is configured using the
        // specified configuration 'parameters'.  Optionally specify a
        // 'basicAllocator' used to supply memory (except for messages).  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
        //
        // The exact description of the argument to the callbacks
        // are given in the type definitions of the
        // 'ChannelStateChangeCallback', 'BlobBasedReadCallback', and
        // 'PoolStateChangeCallback'.
        //
        // @DEPRECATED - use the constructor that uses bdef_Function parameters

    btemt_ChannelPool(
            ChannelStateChangeCallback             channelStateCb,
            BlobBasedReadCallback                  blobBasedReadCb,
            PoolStateChangeCallback                poolStateCb,
            const btemt_ChannelPoolConfiguration&  parameters,
            bslma_Allocator                       *basicAllocator = 0);
        // Create a channel pool with the specified 'channelStateCb',
        // 'blobBasedReadCb' and 'poolStateCb' callbacks to be invoked,
        // correspondingly, when a channel state changes, data arrives, or
        // pool state changes.  The channel pool is configured using the
        // specified configuration 'parameters'.  Optionally specify a
        // 'basicAllocator' used to supply memory (except for messages).  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
        //
        // The exact description of the argument to the callbacks
        // are given in the type definitions of the
        // 'ChannelStateChangeCallback', 'BlobBasedReadCallback', and
        // 'PoolStateChangeCallback'.

    ~btemt_ChannelPool();
        // Destroy this channel pool.  The behavior is undefined if the
        // channel pool was not shut down properly.

    // MANIPULATORS
                                  // *** Server part ***

    int close(int serverId);
        // Close the listening socket corresponding to the specified
        // 'serverId'.  Return 0 on success, and a non-zero value otherwise.
        // Note that closing a listening socket has no effect on any channels
        // managed by this pool.

    int listen(int                        portNumber,
               int                        backlog,
               int                        serverId,
               int                        reuseAddress = 1,
               bool                       readEnabledFlag = true,
               const bteso_SocketOptions *socketOptions = 0);
    int listen(int                        portNumber,
               int                        backlog,
               int                        serverId,
               const bdet_TimeInterval&   timeout,
               int                        reuseAddress = 1,
               bool                       readEnabledFlag = true,
               const bteso_SocketOptions *socketOptions = 0);
    int listen(const bteso_IPv4Address&   endpoint,
               int                        backlog,
               int                        serverId,
               int                        reuseAddress = 1,
               bool                       readEnabledFlag = true,
               const bteso_SocketOptions *socketOptions = 0);
    int listen(const bteso_IPv4Address&   endpoint,
               int                        backlog,
               int                        serverId,
               const bdet_TimeInterval&   timeout,
               int                        reuseAddress = 1,
               bool                       readEnabledFlag = true,
               KeepHalfOpenMode           mode = BTEMT_CLOSE_BOTH,
               const bteso_SocketOptions *socketOptions = 0);
        // Establish a listening socket having the specified 'backlog' maximum
        // number of pending connections on the specified 'portNumber' on all
        // local interfaces or the specified 'endpoint', depending on which
        // overload of 'listen' is used, and associate this newly established
        // socket with the specified 'serverId'.  Optionally, specify a
        // 'timeout' *duration* for accepting a connection.  If no connection
        // attempt is received for a period of 'timeout' since the last
        // connection or the last timeout, a pool state callback is invoked
        // with event equal to 'ACCEPT_TIMEOUT'.  Optionally specify a
        // 'reuseAddress' value to be used in setting 'REUSEADDRESS' socket
        // option; if 'reuseAddress' is not specified, 1 is used (i.e.,
        // 'REUSEADDRESS' is enabled).  Optionally specify via a
        // 'readEnabledFlag' whether automatic reading should be enabled on
        // this channel immediately after creation; if 'readEnabledFlag' is not
        // specified, then 'true' is used (i.e., reading on new channels is
        // automatically enabled).  If 'endpoint', 'timeout', 'reuseAddress'
        // and 'readEnabledFlag' are all specified, also optionally specify a
        // 'mode' to keep channel half-open in case a channel established by
        // the server with this 'serverId' is half-closed; if 'mode' is not
        // specified, then 'BTEMT_CLOSE_BOTH' is used (i.e., half-open
        // connections lead to closing the channel completely).  Optionally
        // specify 'socketOptions' that will be used to indicate what options
        // should be set on the listening socket.  Return 0 on success, a
        // positive value if there is a listening socket associated with
        // 'serverId' (i.e., 'serverId' is not unique) and a negative value if
        // an error occurred.  Every time a connection is accepted by this pool
        // on this (newly established) listening socket, 'serverId' is passed
        // to the callback provided in the configuration at construction.  The
        // behavior is undefined unless '0 < backlog'.

                                  // *** Client part ***

    int connect(const char                *hostname,
                int                        portNumber,
                int                        numAttempts,
                const bdet_TimeInterval&   interval,
                int                        sourceId,
                ConnectResolutionMode      resolutionMode = BTEMT_RESOLVE_ONCE,
                bool                       readEnabledFlag = true,
                KeepHalfOpenMode           halfCloseMode = BTEMT_CLOSE_BOTH,
                const bteso_SocketOptions *socketOptions = 0,
                const bteso_IPv4Address   *localAddress = 0);
        // Asynchronously issue up to the specified 'numAttempts' connection
        // requests to a server at the address resolved from the specified
        // 'hostname' on the specified 'portNumber', with at least the
        // specified (relative) time 'interval' after each attempt before
        // either a new connection is retried (if 'numAttempts' is not reached)
        // or the connection attempts are abandoned (if 'numAttempts' is
        // reached).  When the connection is established, an internal channel
        // is created and a channel state callback, with the event
        // 'BTEMT_CHANNEL_UP', the newly created channel ID, and the specified
        // 'sourceId' is invoked in an internal thread.  If the 'interval' is
        // reached, or in case other events occur (e.g., 'ERROR_CONNECTING',
        // 'CHANNEL_LIMIT', or 'CAPACITY_LIMIT'), a pool state callback is
        // invoked with the event type, 'sourceId' and a severity.  Optionally
        // specify a 'resolutionMode' to indicate whether the name resolution
        // is performed once (if 'resolutionMode' is 'BTEMT_RESOLVE_ONCE'), or
        // performed anew prior to each attempt (if 'resolutionMode' is
        // 'BTEMT_RESOLVE_AT_EACH_ATTEMPT'); if 'resolutionMode' is not
        // specified, 'BTEMT_RESOLVE_ONCE' is used.  Optionally specify via a
        // 'readEnabledFlag' whether automatic reading should be enabled on
        // this channel immediately after creation; if 'readEnabledFlag' is not
        // specified, then 'true' is used (i.e., reading on new channels is
        // automatically enabled).  Optionally specify a 'halfCloseMode' in
        // case the channel created for this connection is half-closed; if
        // 'mode' is not specified, then 'BTEMT_CLOSE_BOTH' is used (i.e.,
        // so-called half-open connections, that is, anything less than full
        // duplex, lead to close the channel).  Optionally specify
        // 'socketOptions' that will be used to specify what options should be
        // set on the connecting socket.  Optionally specify the
        // 'localAddress' that should be used as the source address.  Return 0
        // on successful initiation, a positive value if there is an active
        // connection attempt with the same 'sourceId' (in which case this
        // connection attempt may be retried after that other connection either
        // succeeds, fails, or times out), or a negative value if an error
        // occurred, with the value of -1 indicating that the channel pool is
        // not running.  The behavior is undefined unless '0 < numAttempts',
        // and either '0 < interval' or '1 == numAttempts' or both.  Note that
        // if the connection cannot be established, up to 'numAttempts' pool
        // state callbacks with 'ERROR_CONNECTING' may be generated, one for
        // each 'interval'.  Also note that this function will fail if this
        // channel pool is not running, and that no callbacks will be invoked
        // if the return value is non-zero.  Also note that the same 'sourceId'
        // can be used in several calls to 'connect' or 'import' as long as two
        // calls to connect with the same 'sourceId' do not overlap.  Finally,
        // note that the lifetime of the 'hostname' need not extend past the
        // return of this function call, that is, 'hostname' need not remain
        // valid until the last connection attempt but can be deleted upon
        // return.

    int connect(const bteso_IPv4Address&   serverAddress,
                int                        numAttempts,
                const bdet_TimeInterval&   interval,
                int                        sourceId,
                bool                       readEnabledFlag = true,
                KeepHalfOpenMode           mode = BTEMT_CLOSE_BOTH,
                const bteso_SocketOptions *socketOptions = 0,
                const bteso_IPv4Address   *localAddress = 0);
        // Asynchronously issue up to the specified 'numAttempts' connection
        // requests to a server at the specified 'serverAddress', with at least
        // the specified (relative) time 'interval' after each attempt before
        // either a new connection is retried (if 'numAttempts' is not reached)
        // or the connection attempts are abandoned (if 'numAttempts' is
        // reached).  When the connection is established, an internal channel
        // is created and a channel state callback, with the event
        // 'BTEMT_CHANNEL_UP', the newly created channel ID, and the specified
        // 'sourceId' is invoked in an internal thread.  If the 'interval' is
        // reached, or in case other events occur (e.g., 'ERROR_CONNECTING',
        // 'CHANNEL_LIMIT', or 'CAPACITY_LIMIT'), a pool state callback is
        // invoked with the event type, 'sourceId' and a severity.  Optionally
        // specify via a 'readEnabledFlag' whether automatic reading should be
        // enabled on this channel immediately after creation; if
        // 'readEnabledFlag' is not specified, then 'true' is used (i.e.,
        // reading on new channels is automatically enabled).  Optionally
        // specify a half-close 'mode' in case the channel created for this
        // connection is half-closed; if 'mode' is not specified, then
        // 'BTEMT_CLOSE_BOTH' is used (i.e., half-open connections lead to
        // close the channel).  Optionally specify 'socketOptions' that will be
        // used to specify what options should be set on the connecting socket.
        // Optionally specify the 'localAddress' that should be used as the
        // source address.  Return 0 on successful initiation, a positive value
        // if there is an active connection attempt with the same 'sourceId'
        // (in which case this connection attempt may be retried after that
        // other connection either succeeds, fails, or times out), or a
        // negative value if an error occurred, with the value of -1 indicating
        // that the channel pool is not running.  The behavior is undefined
        // unless '0 < numAttempts', and either '0 < interval' or
        // '1 == numAttempts' or both.  Note that if the connection cannot be
        // established, up to 'numAttempts' pool state callbacks with
        // 'ERROR_CONNECTING' may be generated, one for each 'interval'.  Also
        // note that this function will fail if this channel pool is not
        // running, and that no callbacks will be invoked if the return value
        // is non-zero.  Also note that the same 'sourceId' can be used in
        // several calls to 'connect' or 'import' as long as two calls to
        // connect with the same 'sourceId' do not overlap.

                                  // *** Channel management ***

    int disableRead(int channelId);
        // Enqueue a request to disable automatic reading on the channel
        // having the specified 'channelId'.  Return 0 on success and
        // a non-zero value otherwise.  Once automatic reading is disabled
        // a channel state callback for this channel is invoked with
        // 'BTEMT_AUTO_READ_DISABLED' state.
        //
        // This method offers the following specific guarantees:
        // - When shutting down a channel, 'BTEMT_AUTO_READ_DISABLED' message
        //   is *not* generated.
        // - A data callback will always happen in between
        //   'BTEMT_AUTO_READ_ENABLED' and 'BTEMT_AUTO_READ_DISABLED'
        //   callbacks.
        // - The data currently enqueued in the channel pool for this channel
        //   is *not* discarded.

    int enableRead(int channelId);
        // Enqueue a request to enable automatic reading on the channel
        // having the specified 'channelId'.  Return 0 on success and
        // a non-zero value otherwise.  Once automatic reading is enabled
        // a channel state callback for this channel is invoked with
        // 'BTEMT_AUTO_READ_ENABLED' state.
        //
        // This method offers the following specific guarantees:
        // - By default, a newly created channel is in
        //   'BTEMT_AUTO_READ_ENABLED' state (except imported channels with
        //   'readEnabledFlag' not set), unless the 'readEnabledFlag was set to
        //   'false'.
        // - When a new channel is created and read is enabled, both
        //   'BTEMT_CHANNEL_UP' and BTEMT_AUTO_READ_ENABLED messages are
        //   generated, in this order.  However, 'BTEMT_CHANNEL_UP' and
        //   'BTEMT_AUTO_READ_ENABLED' may be generated from different threads.
        // - A data callback will always happen in between
        //   'BTEMT_AUTO_READ_ENABLED' and 'BTEMT_AUTO_READ_DISABLED'
        //   callbacks.

    int import(
         bteso_StreamSocket<bteso_IPv4Address>        *streamSocket,
         bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
         int                                           sourceId,
         bool                                          readEnabledFlag = true,
         KeepHalfOpenMode                              mode =BTEMT_CLOSE_BOTH);
        // Import the specified 'streamSocket' into this channel pool.  A
        // corresponding channel is internally created and assigned to internal
        // threads.  The channel ID is assigned and a channel state callback,
        // with 'BTEMT_CHANNEL_UP', channel ID, and the specified 'sourceId' is
        // invoked in an internal thread.  Upon destruction, 'streamSocket' is
        // destroyed via the specified 'factory'.  Optionally specify via a
        // 'readEnabledFlag' whether automatic reading should be enabled on
        // this channel immediately after creation; if 'readEnabledFlag' is not
        // specified, then 'true' is used (i.e., reading on new channels is
        // automatically enabled).  Optionally specify a half-close 'mode' in
        // case the channel created for this connection is half-closed; if
        // 'mode' is not specified, then 'BTEMT_CLOSE_BOTH' is used (i.e.,
        // half-open connections lead to close the channel).  Return 0 on
        // success and a non-zero value, with no effect on the channel pool,
        // otherwise.  Note that the same 'sourceId' can be used in several
        // calls to 'connect' or 'import' as long as two calls to connect with
        // the same 'sourceId' do not overlap.  Also note that a half-closed
        // 'streamSocket' can be imported into this channel pool, irrespective
        // of 'mode'.

    void setChannelContext(int channelId, void *context);
        // Associate the specified (opaque) 'context' with the channel having
        // the specified 'channelId'.  The channel context will be reported on
        // any invocation of a callback related to this channel.

    int shutdown(int                      channelId,
                 ShutdownMode             mode = BTEMT_IMMEDIATE);
    int shutdown(int                      channelId,
                 bteso_Flag::ShutdownType type,
                 ShutdownMode             mode = BTEMT_IMMEDIATE);
        // Shut down the communication channel having the specified 'channelId'
        // in the optionally specified 'mode' and return 0 on success, and a
        // non-zero value otherwise.  Optionally specify a shutdown 'type' to
        // close only the reading or the writing part; by default,
        // 'BTEMT_CLOSE_BOTH' is used (i.e., both halves of the channel are
        // closed).  Note that shutting down a channel will deallocate all
        // system resources associated with 'channel' and subsequent references
        // to channel will result in undefined behavior.  Also note that, if
        // the channel does not support half-open connections (i.e., the 'mode'
        // passed to 'connect', 'listen', or 'import' was set to
        // 'BTEMT_CLOSE_BOTH'), then shutting down the channel leads to a
        // complete shutdown, irrespective of the shutdown 'type'.  If the
        // channel does support half-open connections, but is already
        // half-closed, and the 'type' (set to 'SHUTDOWN_BOTH',
        // 'SHUTDOWN_RECEIVE' or 'SHUTDOWN_SEND') closes the other half, then
        // the channel is shut down completely; otherwise, only one half of the
        // channel is closed but the channel itself is not, and subsequent
        // calls to write (if 'type' is 'SHUTDOWN_SEND'), or to 'enableRead'
        // (if 'type' is 'SHUTDOWN_RECEIVE'), will fail.

    int setWriteCacheHiWatermark(int channelId, int numBytes);
        // Set the write-cache high-watermark for the specified 'channelId' to
        // the specified 'numBytes'; return 0 on success, or a non-zero value
        // if either 'channelId' does not exist or 'numBytes' is less than the
        // low watermark for the write cache.  This channel pool maintains an
        // internal cache of outgoing data for each channel, and data written
        // to a channel is added to this cache until the associated socket can
        // be written-to without blocking.  Once the write-cache
        // high-watermark is reached, this channel pool will no longer accept
        // messages for the channel until additional space becomes available
        // (by writing the cached data to the underlying socket) and a
        // 'BTEMT_WRITE_CACHE_HIWAT' alert is provided to the client via the
        // channel state callback.  The behavior is undefined unless
        // '0 <= numBytes'.  Note that this method overrides the default value
        // configured (for all channels) by the
        // 'btemt_ChannelPoolConfiguration' supplied at construction.

    int setWriteCacheLowWatermark(int channelId, int numBytes);
        // Set the write-cache low-watermark for the specified 'channelId' to
        // the specified 'numBytes'; return 0 on success, or a non-zero value
        // if either 'channelId' does not exist or 'numBytes' is less than the
        // low watermark for the write cache.  The behavior is undefined unless
        // '0 <= numBytes'.  Note that this method overrides the default value
        // configured (for all channels) by the
        // 'btemt_ChannelPoolConfiguration' supplied at construction.  This
        // channel pool maintains an internal cache of outgoing data for each
        // channel, and data written to a channel is added to this cache until
        // the associated socket can be written-to without blocking.  Once the
        // write-cache high-watermark is reached, this channel pool will no
        // longer accept messages for the channel until additional space
        // becomes available.  After the data is written to the socket and the
        // cache size falls below the low-watermark then a
        // 'BTEMT_WRITE_CACHE_LOWWAT' alert is provided to the client via the
        // channel state callback to suggest that further writing can resume.

    int resetRecordedMaxWriteCacheSize(int channelId);
        // Reset the recorded max write cache size for the specified
        // 'channelId' to the current write cache size.  Return 0 on success,
        // or a non-zero value if 'channelId' does not exist.  Note that this
        // function resets the recorded max write cache size and does not
        // change the write cache high watermark for 'channelId'.

    int setNotifyLowWatermark(int channelId);
        // Notify the user of the 'BTEMT_WRITE_CACHE_LOWWAT' alert via the
        // channel state callback when the internal write cache for the
        // specified 'channelId' drops below the low-watermark specified at
        // construction.  Return 0 on success, or a non-zero value if
        // 'channelId' does not exist.  Note that the
        // 'BTEMT_WRITE_CACHE_LOWWAT' alert is only invoked after a write
        // failure because the write cache size exceeds the high-watermark and
        // not the optional enqueue watermark argument of write.  So this
        // function can be used to trigger the 'BTEMT_WRITE_CACHE_LOWWAT' alert
        // if write failed because the enqueue watermark was exceeded.  Also
        // note that this method should normally *NOT* be called; it is
        // provided as a work around and may be removed in a subsequent
        // release.

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

                                  // *** Incoming messages ***

    bcema_PooledBufferChainFactory *incomingBufferFactory();
        // Return the address of the pooled buffer chain factory used by this
        // channel pool to produce buffer chains for incoming messages.

    bcema_PooledBlobBufferFactory *incomingBlobBufferFactory();
        // Return the address of the pooled blob buffer factory used by this
        // channel pool to produce blobs for incoming messages.

                                  // *** Outgoing messages ***

    bcema_PooledBufferChainFactory *outboundBufferFactory();
        // Return the address of the pooled buffer chain factory used by this
        // channel pool to produce buffer chains for outbound messages.
        //
        // Note that this version of channel pool now uses blob internally.
        // Although this method returns a valid (i.e., properly initialized)
        // pooled buffer chain factory which can be used to create data
        // messages, it is more efficient to create blob messages, using the
        // 'outboundBlobBufferFactory()'.

    bcema_PooledBlobBufferFactory *outboundBlobBufferFactory();
        // Return the address of the blob buffer factory used by this
        // channel pool to produce buffer chains for outbound messages.
        //
        // Note that this version of channel pool now uses blob internally.
        // It is more efficient to create blob messages, using the
        // 'outboundBlobBufferFactory()', than to create data messages using
        // 'outboundBufferFactory()'.

    int write(int                  channelId,
              const bcema_Blob&    message);
    int write(int                  channelId,
              const bcema_Blob&    message,
              int                  enqueueWatermark);
    int write(int                  channelId,
              const btemt_BlobMsg& message);
    int write(int                  channelId,
              const btemt_BlobMsg& message,
              int                  enqueueWatermark);
        // Enqueue a request to write the specified 'message' into the channel
        // having the specified 'channelId'.  Optionally specify an
        // 'enqueueWaterMark' to limit the size of the enqueued portion of the
        // message.  Return 0 on success (i.e., the operation was successfully
        // enqueued) and a non-zero value otherwise.

    int write(int                   channelId,
              const btemt_DataMsg&  message);
    int write(int                   channelId,
              const btemt_DataMsg&  message,
              int                   enqueueWatermark);
        // Enqueue a request to write the specified 'message' into the channel
        // having the specified 'channelId'.  Optionally specify an
        // 'enqueueWaterMark' to limit the size of the enqueued portion of the
        // message.  Return 0 on success (i.e., the operation was successfully
        // enqueued) and a non-zero value otherwise.

    int write(int                   channelId,
              const btemt_DataMsg&  message,
              btemt_BlobMsg        *blobMsg);
    int write(int                   channelId,
              const btemt_DataMsg&  message,
              int                   enqueueWatermark,
              btemt_BlobMsg        *blobMsg);
        // Deprecated.  Note that the 'blobMsg' parameter is ignored.

    int write(int               channelId,
              const btes_Iovec  vecs[],
              int               numVecs);
    int write(int               channelId,
              const btes_Ovec   vecs[],
              int               numVecs);
        // Enqueue a request to write the specified 'vecs' into the channel
        // having the specified 'channelId'.  Return 0 on success (i.e., the
        // operation was successfully enqueued) and a non-zero value otherwise.
        // Note that you should prefer this method over the other 'write()'
        // method *only* *if* you expect that this object will be able to write
        // most of the data contained in the specified 'vecs' atomically.  If
        // the 'vecs' must be enqueued, an inefficient data copy will occur to
        // allow to control the lifetime of the data.

                                  // *** Clock management ***

    int registerClock(const bdef_Function<void (*)()>& command,
                      const bdet_TimeInterval&         startTime,
                      const bdet_TimeInterval&         period,
                      int                              clockId);
    int registerClock(const bdef_Function<void (*)()>& command,
                      const bdet_TimeInterval&         startTime,
                      const bdet_TimeInterval&         period,
                      int                              clockId,
                      int                              channelId);
        // Register the specified 'command' to be invoked after the specified
        // 'startTime' absolute time and associate this registration with the
        // specified 'clockId'.  If the specified 'period' (relative time) is
        // positive, repeat invoking 'command' at 'period' intervals until the
        // clock is deregistered, otherwise a single 'command' will be invoked.
        // Optionally specify 'channelId' to indicate that the clock must be
        // executed in the same event manager as the 'channelId' callbacks.
        // Return 0 on success and a non-zero value otherwise; the return value
        // of 1 is indicates that a clock with the specified 'clockId' is
        // already registered.  Note that if 'channelId' is provided and does
        // not correspond to an active channel, a non-zero value not equal to 1
        // is returned, even if a clock with the specified 'clockId' is already
        // registered.

    void deregisterClock(int clockId);
        // Deregister the clock having the specified 'clockId'.

                                  // *** Socket Options ***

    int getLingerOption(bteso_SocketOptUtil::LingerData *result,
                        int                              channelId) const;
        // Load into the specified 'result', the value of the linger option for
        // the channel having the specified 'channelId'.  Return 0 on success
        // and a non-zero value otherwise.  The behavior is undefined if
        // 'result' is 0.

    int getServerSocketOption(int *result,
                              int  option,
                              int  level,
                              int  serverId) const;
        // Load into the specified 'result' the value of the specified 'option'
        // of the specified 'level' socket option on the server socket having
        // the specified 'serverId'.  Return 0 on success and a non-zero value
        // otherwise.  (See 'bteso_SocketOptUtil' for the set of commonly-used
        // options.)

    int getSocketOption(int *result,
                        int  option,
                        int  level,
                        int  channelId) const;
        // Load into the specified 'result' the value of the specified 'option'
        // of the specified 'level' socket option on the channel having the
        // specified 'channelId'.  Return 0 on success and a non-zero value
        // otherwise.  (See 'bteso_SocketOptUtil' for the set of commonly-used
        // options.)

    int setLingerOption(const bteso_SocketOptUtil::LingerData& value,
                        int                                    channelId);
        // Set the linger option on the channel with the specified 'channelId'
        // to the specified 'value'.  Return 0 on success and a non-zero
        // value otherwise.

    int setServerSocketOption(int option, int level, int value, int serverId);
        // Set the specified 'option' (of the specified 'level') socket option
        // on the listening socket with the specified 'serverId' to the
        // specified 'value'.  Return 0 on success and a non-zero value
        // otherwise.  (See 'bteso_socketoptutil' for the list of commonly
        // supported options.)

    int setSocketOption(int option, int level, int value, int channelId);
        // Set the specified 'option' (of the specified 'level') socket option
        // on the channel with the specified 'channelId' to the specified
        // 'value'.  Return 0 on success and a non-zero value otherwise.
        // (See 'bteso_socketoptutil' for the list of commonly supported
        // options.)

                                  // *** Metrics ***

    double reportWeightedAverageReset();
        // Return the weighted average of the connections lifetime since the
        // previous call to this method or, for the first call, since this
        // object construction, if at least one millisecond passed since then,
        // otherwise return -1.
        // If all the connections at the time of the call have been up during
        // the considered period of time and were already up at the time of the
        // previous reset, this method will return the same number as
        // 'numChannels()'.  0 means that they were all down.

    void totalBytesReadReset(bsls_PlatformUtil::Int64 *result);
        // Load, into the specified 'result', and atomically reset the total
        // number of bytes read by the pool.

    void totalBytesWrittenReset(bsls_PlatformUtil::Int64 *result);
        // Load, into the specified 'result', and atomically reset the total
        // number of bytes written by the pool.

    void totalBytesRequestedToBeWrittenReset(bsls_PlatformUtil::Int64 *result);
        // Load, into the specified 'result', and atomically reset the total
        // number of bytes requested to be written by the pool.

    // ACCESSORS
    int busyMetrics() const;
        // Return the (percent) value in the range [0..100] (inclusive) that
        // reflects the workload of this channel pool (e.g., how busy it is for
        // the last period).  If the 'collectTimeMetrics' property of the
        // configuration supplied at construction is 'false' (i.e., the
        // collection of time metrics has been disable), then the returned
        // value is unspecified.  The value 0 indicates that the pool
        // is idle and 100 indicates that pool operates at the configured
        // capacity.

    void *channelContext(int channelId) const;
        // Return a user-defined channel context associated with the specified
        // 'channelId', and '(void*)0' if no such channel exists OR the user
        // context for this channel was explicitly set to '(void*)0'.

    int getChannelStatistics(bsls_PlatformUtil::Int64 *numRead,
                             bsls_PlatformUtil::Int64 *numRequestedToBeWritten,
                             bsls_PlatformUtil::Int64 *numWritten,
                             int                       channelId) const;
        // Load into the specified 'numRead', 'numRequestedToBeWritten' and
        // 'numWritten' respectively the number of bytes read, requested to
        // be written and written by the channel identified by the specified
        // 'channelId' and return 0 if the specified 'channelId' is a valid
        // channel id.  Otherwise, return a non-zero value.  Note that for
        // performance reasons this *sequence* is not captured atomically: by
        // the time one of the values is captured, another may already have
        // changed.

    int getChannelWriteCacheStatistics(int *recordedMaxWriteCacheSize,
                                       int *currentWriteCacheSize,
                                       int  channelId) const;
        // Load into the specified 'recordedMaxWriteCacheSize' and
        // 'currentWriteCacheSize' the maximum and current size respectively of
        // the write cache of the channel identified by the specified
        // 'channelId' and return 0 if the specified 'channelId' is a valid
        // channel id.  Otherwise, return a non-zero value.  Note that for
        // performance reasons this *sequence* is not captured atomically: by
        // the time one of the values is captured, another may already have
        // changed.

    void getHandleStatistics(bsl::vector<HandleInfo> *handleInfo) const;
        // Append to the specified 'handleInfo' array a snapshot of the
        // information per socket handle currently in use by this channel pool.
        // Note that a socket handle 'fd' is in use for one of five possible
        // reasons:
        //..
        //  Reason                               d_channelType      d_userId
        //  ------                               -------------      --------
        //  1. This channel pool is listening    LISTENING_CHANNEL  serverId
        //     on 'fd', with a given 'serverId'
        //
        //  2. A channel was created as the      ACCEPTED_CHANNEL   serverId
        //     result of a connection to the
        //     server with the given 'serverId'
        //
        //  3. This channel pool is connecting   CONNECTING_CHANNEL sourceId
        //     using a connection socket with
        //     the socket handle 'fd', with a
        //     a given 'sourceId'
        //
        //  4. A channel was created as the      CONNECTED_CHANNEL  sourceId
        //     result of a call to 'connect'
        //     with the given 'sourceId'
        //
        //  5. A channel was created as the      IMPORTED_CHANNEL   sourceId
        //     result of a call to 'import'
        //     with the given 'sourceId'
        //..
        // Also note that there is no specified order in which 'handleInfo' is
        // updated.  Finally note that entries are appended to 'handleInfo';
        // clear this vector prior to calling this function if desired.

    int getServerAddress(bteso_IPv4Address *result, int serverId) const;
        // Load into the specified 'result' the complete IP address associated
        // with the server with the specified 'serverId' that is managed by
        // this channel pool if the server is established.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise.

    int getLocalAddress(bteso_IPv4Address *result, int channelId) const;
        // Load into the specified 'result' the complete IP address
        // associated with the local (i.e., this process) end-point of the
        // communication channel having the specified 'channelId'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise.

    bool useBlobForDataReads() const;
        // Return 'true' if the read callback registered with this channel
        // pool is blob based, and 'false' otherwise.

    int getPeerAddress(bteso_IPv4Address *result, int channelId) const;
        // Load into the specified 'result' the complete IP address
        // associated with the remote (i.e., peer process) end-point of the
        // communication channel having the specified 'channelId'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise.

    int numBytesRead(bsls_PlatformUtil::Int64 *result, int channelId) const;
        // Load, into the specified 'result', the number of bytes read by the
        // channel identified by the specified 'channelId' and return 0 if the
        // specified 'channelId' is a valid channel id.  Otherwise, return a
        // non-zero value.

    int numBytesWritten(bsls_PlatformUtil::Int64 *result, int channelId) const;
        // Load, into the specified 'result', the number of bytes written by
        // the channel identified by the specified 'channelId' and return 0 if
        // the specified 'channelId' is a valid channel id.  Otherwise, return
        // a non-zero value.

    int numBytesRequestedToBeWritten(bsls_PlatformUtil::Int64 *result,
                                     int                      channelId) const;
        // Load, into the specified 'result', the number of bytes requested to
        // be written by the channel identified by the specified 'channelId'
        // and return 0 if the specified 'channelId' is a valid channel id.
        // Otherwise, return a non-zero value.

    int numChannels() const;
        // Return the number of channels currently managed by this channel
        // pool.

    int numEvents(int index) const;
        // Return the number of events currently registered with an event
        // manager corresponding to the specified 'index'.  The behavior is
        // undefined unless '0 <= index < numThreads'.

    int numThreads() const;
        // Return the number of threads currently managed by this channel
        // pool.  Note that each thread corresponds to a single event manager,
        // and, therefore, the number of threads is the number of active
        // event managers.

    const bteso_IPv4Address *serverAddress(int serverId) const;
        // Return the address of the server IPv4 address for the server with
        // the specified 'serverId' that is managed by this channel pool if the
        // server is established, and 0 otherwise.  Note that the address is
        // only valid as long as the server is up.
        //
        // @DEPRECATED - use 'getServerAddress'

    bcema_SharedPtr<const bteso_StreamSocket<bteso_IPv4Address> >
                                             streamSocket(int channelId) const;
        // Return a shared pointer to the non-modifiable stream socket
        // associated with the specified 'channelId', and an empty shared
        // pointer if a corresponding channel does not exist.  The returned
        // shared pointer is aliased to the underlying channel and the channel
        // will not be closed until this shared pointer is destroyed.
        // Therefore, it is important that clients carefully manage the
        // lifetime of the returned shared pointer.  The behavior of this
        // channel pool is undefined if the underlying socket is manipulated
        // while still under mangement by this channel pool.

    void totalBytesRead(bsls_PlatformUtil::Int64 *result) const;
        // Load, into the specified 'result', the total number of bytes read by
        // the pool.

    void totalBytesRequestedToBeWritten(
                                       bsls_PlatformUtil::Int64 *result) const;
        // Load, into the specified 'result', the total number of bytes
        // requested to be written by the pool.

    void totalBytesWritten(bsls_PlatformUtil::Int64 *result) const;
        // Load, into the specified 'result', the total number of bytes written
        // by the pool.
};

                 // ==================================
                 // class btemt_ChannelPool_IovecArray
                 // ==================================

template <typename IOVEC>
class btemt_ChannelPool_IovecArray {
    // This is an implementation type of 'btemt_ChannelPool' and should not
    // be used by clients of this component.  An 'IovecArray' is an in-core
    // value-semantic type describing a array of iovec objects of templatized
    // type 'IOVEC'.  The parameterized 'IOVEC' type must be either
    // 'bteso_Iovec' or 'btes_Ovec'.  Note that the each 'IOVEC' object in
    // the 'iovecs()' array  refers to an array of data, so an 'IovecArray' is
    // an array of arrays, and the the total data length of an 'IovecArray' is
    // the sum of the lengths of the 'IOVEC' objects in 'iovecs()'.

    // DATA
    const IOVEC              *d_iovecs;       // array of iovecs
    int                       d_numIovecs;    // number of iovecs
    bsls_PlatformUtil::Int64  d_totalLength;  // total data length

    public:

    // CREATORS
    btemt_ChannelPool_IovecArray(const IOVEC *iovecs, int numIovecs);
            // Create an 'IovecArray' object for the specified array of
            // 'iovecs' of length 'numIovecs'.

    // ~btemt_ChannelPool_IovecArray();
        // Destroy this array of iovec objects.  Note that this operation
        // is supplied by the compiler.

    btemt_ChannelPool_IovecArray(const btemt_ChannelPool_IovecArray& original);
        // Create an iovec array with the same value as the specified
        // original.

    // MANIPULATORS
    btemt_ChannelPool_IovecArray& operator=(
                                      const btemt_ChannelPool_IovecArray& rhs);
        // Assign this iovec array the value of the specified 'rhs', and
        // return a reference to this modifiable iovec array.

    // ACCESSORS
    const IOVEC *iovecs() const;
        // Return the array of 'IOVEC' objects.  Note that each 'IOVEC'
        // object in the returned array refers to an array of data.

    int numIovecs() const;
        // Return the length of the array of 'IOVEC' objects returned by
        // 'iovecs()'.

    bsls_PlatformUtil::Int64 length() const;
        // Return the total length, in bytes, of the data referred to by
        // 'IOVEC' objects in the 'iovecs()' array of 'IOVEC' objects.
};

// FREE OPERATORS
template <typename IOVEC>
inline
bool operator==(const btemt_ChannelPool_IovecArray<IOVEC> &lhs,
                const btemt_ChannelPool_IovecArray<IOVEC> &rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iovec arrays have the
    // same value, and 'false' otherwise.  Two iovec arrays have the same
    // value if their respective array addresses, array lengths, and total
    // data lengths are the same.

template <typename IOVEC>
inline
bool operator!=(const btemt_ChannelPool_IovecArray<IOVEC> &lhs,
                const btemt_ChannelPool_IovecArray<IOVEC> &rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iovec arrays do not have
    // the same value, and 'false' otherwise.  Two iovec arrays do not have
    // the same value if their respective array addresses, array lengths, or
    // total data lengths are not the name.

                 // =====================================
                 // class btemt_ChannelPool_MessageUtil
                 // =====================================

struct btemt_ChannelPool_MessageUtil {
    // This is an implementation type of 'btemt_ChannelPool' and should not
    // be used by clients of this component.  The
    // 'btemt_ChannelPool_MessageUtil' struct provides a namespace for a
    // set of common functions that can be applied to the different message
    // container types supported by a 'ChannelPool'.

    // PUBLIC CONSTANTS
    enum {
        // This enumeration defines the constant 'BTEMT_MAX_IOVEC_SIZE', which
        // is used to indicate the maximum length of an array of iovecs
        // that can be directly read-from or written-to a socket.  Note that
        // 'IOV_MAX' is defined (on POSIX unix platforms) in "limits.h", and
        // indicates the  maximum number of iovecs that can be supplied to
        // 'writev'.

#ifdef BSLS_PLATFORM__OS_UNIX
#ifdef IOV_MAX
#if IOV_MAX > 32
// If too big, this would make 'btemt_Channel' really big.
        BTEMT_MAX_IOVEC_SIZE = 32
#else
        BTEMT_MAX_IOVEC_SIZE = IOV_MAX
#endif
#else
        BTEMT_MAX_IOVEC_SIZE = 16
#endif
#else // Windows
        BTEMT_MAX_IOVEC_SIZE = 16
#endif
     };

    // CLASS METHODS
    template <typename IOVEC>
    static bsls_PlatformUtil::Int64 length(
                               const btemt_ChannelPool_IovecArray<IOVEC>& msg);
    static bsls_PlatformUtil::Int64 length(const btemt_DataMsg& msg);
    static bsls_PlatformUtil::Int64 length(const bcema_Blob& msg);
        // Return the length of the specified 'msg'.

    template <typename IOVEC>
    static int write(bteso_StreamSocket<bteso_IPv4Address>      *socket,
                     btes_Iovec                                 *temp,
                     const btemt_ChannelPool_IovecArray<IOVEC>&  msg);
    static int write(bteso_StreamSocket<bteso_IPv4Address> *socket,
                     btes_Iovec                            *temp,
                     const bcema_Blob&                      msg);
    static int write(bteso_StreamSocket<bteso_IPv4Address> *socket,
                     btes_Iovec                            *temp,
                     const btemt_DataMsg&                   msg);
        // Write, to the specified 'socket', the buffers in the specified
        // 'msg', up to 'BTEMT_MAX_IOVEC_SIZE' buffers, using the
        // specified 'temp' array of iovec objects as a (temporary)
        // intermediary for 'StreamSocket::writev' (if required).  Return the
        // return value from 'StreamSocket::writev'.

    static int loadIovec(btes_Iovec *dest, const bcema_Blob& msg);
    static int loadIovec(btes_Iovec *dest, const btemt_DataMsg& msg);
        // Load into the specified 'dest' iovec the data buffers from the
        // specified 'msg', up to 'BTEMT_MAX_IOVEC_SIZE' buffers.  Return the
        // number of buffers loaded into 'dest'.

    template <typename IOVEC>
    static int loadBlob(bcema_Blob                                 *dest,
                        const btemt_ChannelPool_IovecArray<IOVEC>&  msg,
                        int                                         msgOffset);
    static int loadBlob(bcema_Blob           *dest,
                        const btemt_DataMsg&  msg,
                        int                   msgOffset);
    static int loadBlob(bcema_Blob        *dest,
                        const bcema_Blob&  msg,
                        int                msgOffset);
        // Load into the specified 'dest' the data in the specified 'msg'
        // starting at the specified 'msgOffset'.  For performance reasons the
        // first blob buffer added to 'dest' may start *before* the
        // 'msgOffset' byte in 'msg'; return the offset into the first blob
        // buffer in 'dest' of the corresponding 'msgOffset' byte in
        // 'msg'.  The behavior is undefined unless 'dest' is empty.

    template <typename IOVEC>
    static void appendToBlob(bcema_Blob                                 *dest,
                             const btemt_ChannelPool_IovecArray<IOVEC>&  msg);
    static void appendToBlob(bcema_Blob           *dest,
                             const btemt_DataMsg&  msg);
    static void appendToBlob(bcema_Blob        *dest,
                             const bcema_Blob&  msg);
        // Append, to the specified 'dest' blob, the data buffers in the
        // specified 'msg'.  The behavior is undefined unless the last buffer
        // in 'dest' is trimmed.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                       //-------------------------
                       // struct btemt_ChannelPool
                       //-------------------------

// PRIVATE ACCESSORS
inline
int btemt_ChannelPool::findChannelHandle(
                                    ChannelHandle *handle, int channelId) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                       0 == d_channels.find(channelId, handle))
          && BSLS_PERFORMANCEHINT_PREDICT_LIKELY(*handle)) {
        return 0;
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    return 1;
}

// MANIPULATORS
inline
int btemt_ChannelPool::write(int channelId, const bcema_Blob& message)
{
    return write(channelId, message, 0x7FFFFFFF);
}

inline
int btemt_ChannelPool::write(int channelId, const btemt_BlobMsg& message)
{
    return write(channelId, message, 0x7FFFFFFF);
}

inline
int btemt_ChannelPool::write(int                   channelId,
                             const btemt_DataMsg&  message)
{
    return write(channelId, message,  0x7FFFFFFF);
}

inline
int btemt_ChannelPool::write(int                   channelId,
                             const btemt_DataMsg&  message,
                             int                   enqueueWatermark,
                             btemt_BlobMsg        *)
{
    return write(channelId, message, enqueueWatermark);
}

inline
int btemt_ChannelPool::write(int                   channelId,
                             const btemt_DataMsg&  message,
                             btemt_BlobMsg        *)
{
    return write(channelId, message, 0x7FFFFFFF);
}

inline
bcema_PooledBufferChainFactory *btemt_ChannelPool::incomingBufferFactory()
{
    return &d_messageFactory;
}

inline
bcema_PooledBlobBufferFactory *btemt_ChannelPool::incomingBlobBufferFactory()
{
    return &d_readBlobFactory;
}

inline
bcema_PooledBufferChainFactory *btemt_ChannelPool::outboundBufferFactory()
{
    return &d_vecMessageFactory;
}

inline
bcema_PooledBlobBufferFactory *btemt_ChannelPool::outboundBlobBufferFactory()
{
    return &d_writeBlobFactory;
}

// ACCESSORS
inline
int btemt_ChannelPool::busyMetrics() const
{
    return bces_AtomicUtil::getInt(d_capacity);
}

inline
bool btemt_ChannelPool::useBlobForDataReads() const
{
    return d_useBlobForDataReads;
}

inline
int btemt_ChannelPool::numChannels() const
{
    return d_channels.length();
}

inline
int btemt_ChannelPool::numThreads() const
{
    return d_startFlag ? static_cast<int>(d_managers.size()) : 0;
}

                 // ----------------------------------
                 // class btemt_ChannelPool_IovecArray
                 // ----------------------------------

// CREATORS
template <typename IOVEC>
inline
btemt_ChannelPool_IovecArray<IOVEC>::btemt_ChannelPool_IovecArray(
                                                        const IOVEC *iovecs,
                                                        int          numIovecs)
: d_iovecs(iovecs)
, d_numIovecs(numIovecs)
, d_totalLength(btes_IovecUtil::length(iovecs, numIovecs))
{
}

template <typename IOVEC>
inline
btemt_ChannelPool_IovecArray<IOVEC>::btemt_ChannelPool_IovecArray(
                                  const btemt_ChannelPool_IovecArray& original)
: d_iovecs(original.d_iovecs)
, d_numIovecs(original.d_numIovecs)
, d_totalLength(original.d_totalLength)
{
}

// MANIPULATORS
template <typename IOVEC>
inline
btemt_ChannelPool_IovecArray<IOVEC>&
btemt_ChannelPool_IovecArray<IOVEC>::operator=(
                                       const btemt_ChannelPool_IovecArray& rhs)
{
    d_iovecs      = rhs.d_iovecs;
    d_numIovecs   = rhs.d_numIovecs;
    d_totalLength = rhs.d_totalLength;
    return *this;
}

// ACCESSORS
template <typename IOVEC>
inline
bsls_PlatformUtil::Int64
btemt_ChannelPool_IovecArray<IOVEC>::length() const
{
    return d_totalLength;
}

template <typename IOVEC>
inline
const IOVEC *
btemt_ChannelPool_IovecArray<IOVEC>::iovecs() const
{
    return d_iovecs;
}

template <typename IOVEC>
inline
int btemt_ChannelPool_IovecArray<IOVEC>::numIovecs() const
{
    return d_numIovecs;
}

                 // -----------------------------------
                 // class btemt_ChannelPool_MessageUtil
                 // -----------------------------------

// CLASS METHODS
inline
bsls_PlatformUtil::Int64
btemt_ChannelPool_MessageUtil::length(const btemt_DataMsg& msg)
{
    return msg.data()->length();
}

inline
bsls_PlatformUtil::Int64
btemt_ChannelPool_MessageUtil::length(const bcema_Blob& msg)
{
    return msg.length();
}

template <typename IOVEC>
inline
bsls_PlatformUtil::Int64
btemt_ChannelPool_MessageUtil::length(
                                const btemt_ChannelPool_IovecArray<IOVEC>& msg)
{
    return msg.length();
}

template <typename IOVEC>
inline
int btemt_ChannelPool_MessageUtil::write(
                           bteso_StreamSocket<bteso_IPv4Address>      *socket,
                           btes_Iovec                                 *,
                           const btemt_ChannelPool_IovecArray<IOVEC>&  msg)
{
    int minNumVecs = msg.numIovecs();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                          minNumVecs > BTEMT_MAX_IOVEC_SIZE)) {
        minNumVecs = BTEMT_MAX_IOVEC_SIZE;
    }

    return socket->writev(msg.iovecs(), minNumVecs);
}

inline
int btemt_ChannelPool_MessageUtil::write(
                                 bteso_StreamSocket<bteso_IPv4Address> *socket,
                                 btes_Iovec                            *temp,
                                 const bcema_Blob&                      msg)
{
    int numVecs = loadIovec(temp, msg);
    BSLS_ASSERT(0 < numVecs);
    return socket->writev(temp, numVecs);
}

inline
int btemt_ChannelPool_MessageUtil::write(
                               bteso_StreamSocket<bteso_IPv4Address>  *socket,
                               btes_Iovec                             *temp,
                               const btemt_DataMsg&                    msg)
{
    int numVecs = loadIovec(temp, msg);
    BSLS_ASSERT(0 < numVecs);
    return socket->writev(temp, numVecs);
}

template <typename IOVEC>
inline
int btemt_ChannelPool_MessageUtil::loadBlob(
                         bcema_Blob                                 *dest,
                         const btemt_ChannelPool_IovecArray<IOVEC>&  msg,
                         int                                         msgOffset)
{
    btes_IovecUtil::appendToBlob(
                              dest, msg.iovecs(), msg.numIovecs(), msgOffset);
    BSLS_ASSERT(dest->length() == msg.length() - msgOffset);
    return 0;
}

template <typename IOVEC>
inline
void btemt_ChannelPool_MessageUtil::appendToBlob(
                             bcema_Blob                                 *dest,
                             const btemt_ChannelPool_IovecArray<IOVEC>&  msg)
{
    btes_IovecUtil::appendToBlob(dest, msg.iovecs(), msg.numIovecs());
}

// FREE OPERATORS
template <typename IOVEC>
inline
bool operator==(const btemt_ChannelPool_IovecArray<IOVEC> &lhs,
                const btemt_ChannelPool_IovecArray<IOVEC> &rhs)
{
    return lhs.iovecs()    == rhs.iovecs()
        && lhs.numIovecs() == rhs.numIovecs()
        && lhs.length()    == rhs.length();
}

template <typename IOVEC>
inline
bool operator!=(const btemt_ChannelPool_IovecArray<IOVEC> &lhs,
                const btemt_ChannelPool_IovecArray<IOVEC> &rhs)
{
    return !(lhs == rhs);
}

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
