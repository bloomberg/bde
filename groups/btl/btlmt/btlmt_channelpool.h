// btlmt_channelpool.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLMT_CHANNELPOOL
#define INCLUDED_BTLMT_CHANNELPOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide thread-enabled stream-based IPv4 communication.
//
//@CLASSES:
//  btlmt::ChannelPool: channel manager
//
//@SEE_ALSO: btlmt_channelpoolconfiguration, btlmt_sessionpool
//
//@DESCRIPTION: This component provides a thread-enabled manager of IPv4-based
// byte-stream communication channels.  The channels are created automatically
// when the appropriate events occur and destroyed based on user requests.  A
// new channel is allocated automatically when an incoming connection is
// accepted, or when the user explicitly requests a connection to a server.
// Channel pool provides both client (aka connector) and server (aka acceptor)
// functionality.  The channel pool manages efficient delivery of messages
// to/from the user based on configuration information supplied at
// construction.  The states of individual messages are *not* reported; rather,
// channel pool notifies the user when a channel's state changes.  It also
// notifies the user when the pool's state is affected and provides the
// classification of errors.  The notification is done via asynchronous
// callback that may be invoked from *any* (managed) thread.
//
///Message Management and Delivery
///-------------------------------
// The channel pool provides an efficient mechanism for the full-duplex
// delivery of messages trying to achieve fully parallel communication on a
// socket whenever possible.  If a particular socket's system buffers are full,
// the messages are cached up to a certain (user-defined) limit, at which point
// an alert is generated.
//
// The channel pool tries to achieve optimal performance by enabling zero-copy
// semantics whenever appropriate.  On the read side, a message is read into a
// buffer that is subsequently passed to the user (via the
// 'BlobBasedReadCallback') for accessing and eventual deallocation.  On the
// write side, the channel pool adopts ownership of the buffers passed to
// 'write()'.
//
///Channel Identification
///----------------------
// Each channel is identified by an integer ID that is (a) assigned by the
// channel pool and (b) is guaranteed to be unique for the lifetime of the
// channel pool.  The user can rely on this uniqueness to identify channels.
//
///Asynchronous Connect
///--------------------
// The channel pool supports an extended asynchronous connect mechanism, by
// which the pool will try to establish a connection with the server making up
// to a (user-provided) number of attempts with a (user-provided) interval
// between attempts.  If a connection attempt cannot succeed within an
// interval, it is aborted and a new connection request is issued.  If a
// connection is successfully established, the channel state callback (see
// configuration) is invoked.  In that case, if this connection is dropped at a
// later time, channel pool *will* *not* *try* to reconnect automatically.  If
// *all* attempts to establish a connection fail, then a pool state callback
// (see configuration) is invoked.  Once initiated, a connect request can lead
// to only two outcomes -- success or failure.  In particular, it can't be
// canceled.
//
///Half-Open Connections
///---------------------
// It is already possible to import a half-duplex connection into a channel
// pool, but should any half of any channel be closed, the channel pool would
// always close the channel (both halves).  A new feature, introduced in BCE
// 1.5.0, allows to keep half-open channels, i.e., single-duplex connections.
// In particular, it is possible at the creation of the channel to specify what
// the channel pool should do if only half of the channel is closed.  Passing
// in the following optional parameter (of enumeration type
// 'btlmt::ChannelPool::KeepHalfOpenMode') to 'close', 'import', or 'listen'
// has the following behavior.
//..
//  e_KEEP_HALF_OPEN    If a peer closes its write part of the channel,
//                      but keeps its receive part open, then the channel pool
//                      will keep sending messages to the peer, but will
//                      disable reading on the channel.  If a peer closes its
//                      receive part of the channel, but keeps its write part
//                      open, then the channel pool will keep reading enable
//                      on this channel, but further calls to writeMessage on
//                      this channel will fail.
//
//  e_CLOSE_BOTH        If a peer closes either its write or receive part of
//                      the channel, the channel pool will simply down the
//                      channel.  This is the default behavior.
//..
//
///Resource Limits
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
///Metrics and Capacity
///--------------------
// By default, the channel pool monitors the workload of managed event managers
// and reports, upon request, an average value of this workload.  The workload
// of each individual event manager is calculated as the ratio of CPU bound
// processing time to the total processing time of the event manager (i.e.  CPU
// bound time +  I/O bound time) over a user-configured interval.  The time
// line for each event manager is broken into two classifications, I/O bound
// and CPU bound intervals; an event manager is performing an I/O bound
// operation when it is blocked on a system call (e.g., 'select'); otherwise,
// it is performing a CPU bound operation.  Note that non-blocking system calls
// are NOT considered as I/O bound.  An average is then calculated over the set
// of *all* *possible* event managers, and the load is taken as 0 if an event
// manager is not started.  The length of interval that metrics are
// periodically collected over is configured by the
// 'btlmt::ChannelPoolConfiguration' supplied at construction.
//
// For calculating the percentage of CPU time used by the channel pool given
// that channel pool manages 'n' event pollers at a moment (with the maximum of
// the number of threads, 'T'), the sum of workloads is taken over the set of
// 'T' event managers, and the workload is 0 for non-existent managers.  Then
// the sum is divided by the number of event managers, T.  The result reflects
// the workload of the channel pool as "percent busy".  Strictly speaking, the
// following is true:
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
///Invocation of High- and Low-Water Mark Callbacks
///------------------------------------------------
// When constructing a channel pool object, users can specify, via the
// 'setWriteCacheWatermarks' function of 'btlmt::ChannelPoolConfiguration', the
// maximum data size (high-water mark) that can be enqueued for writing on a
// channel.  If the write cache size exceeds this high-water mark value, then
// 'write' calls on that channel will fail.  This information is also
// communicated by providing a 'e_WRITE_CACHE_HIWAT' alert to the client via
// the channel state callback.  Note that 'write' calls can also fail if the
// write cache size exceeds the optionally specified 'enqueueWatermark'
// argument provided to 'write', but a 'e_WRITE_CACHE_HIWAT' alert is not
// provided in this scenario.
//
// In addition to the high-water mark, users can also specify a low-water mark,
// again via the 'setWriteCacheWatermarks' function of
// 'btlmt::ChannelPoolConfiguration'.  After a write fails because the write
// cache size would be exceeded, the channel pool will later provide a
// 'e_WRITE_CACHE_LOWWAT' alert to the client via the channel state callback
// when the write cache size falls to, or below, the low-water mark.
// Typically, clients will suspend writing to a channel if the write cache
// exceeds the high-water mark or the optionally provided 'enqueueWatermark',
// and then resume writing after they receive a low-water mark event.  Note
// that a 'e_WRITE_CACHE_LOWWAT' alert is also provided if the write cache size
// exceeds the optionally specified 'enqueueWatermark' argument provided to
// 'write'.
//
// Note that the high- and low-water marks for a specified channel can be
// modified from the values established at construction by the
// 'setWriteCacheHiWatermark', 'setWriteCacheLowWatermark', and
// 'setWriteCacheWatermarks' methods on 'btlmt::ChannelPool'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Establishing a Connection
/// - - - - - - - - - - - - - - - - - -
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
//                          btlmt::ChannelPool **poolAddr)
//      {
//          assert(sourceId == d_sourceId);
//          if (btlmt::ChannelPool::e_CHANNEL_DOWN == status) {
//              // Client disconnected from the server.
//              assert(poolAddr && *poolAddr);
//              (*poolAddr)->shutdown(channelId,
//                                    btlmt::ChannelPool::e_IMMEDIATE);
//          } else
//          if (btlmt::ChannelPool::e_CHANNEL_UP == status) {
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
//      btlmt::ChannelPoolConfiguration config;
//      config.setMaxThreads(4);
//      config.setMetricsInterval(10.0);
//      config.setMaxConnections(16);
//      config.setIncomingMessageSizes(1, 128, 256);
//
//      bsl::function<void(int, int, int, void*)>
//                                          ccb;    // channel state callback
//      bsl::function<void(int, int*, int*, void*)>
//                                          dcb;    // data callback
//      bsl::function<void(int, int, int)>
//                                          pcb;    // pool state callback
//
//      bslmt::ChannelPool *poolAddr;
//      ccb = bdlf::BindUtil::bind( &my_LocalCallback::channelStateCb
//                               , &local)
//                               , _1, _2, _3, _4
//                               , &poolAddr);
//      makeNull(&dcb);  // not interested in data
//      makeNull(&pcb);  // not interested in pool state
//
//      localCallback.d_sourceId = 5;    // just for a simple verification
//
//      btlmt::ChannelPool pool(ccb, dcb, pcb, config);
//      poolAddr = &pool;
//..
// Now, start the channel pool, issue the connect request, and wait for
// completion.  Note that main thread is never blocked, so we have to put it
// to sleep explicitly:
//..
//      assert(0 == pool.start());
//      btlso::IPv4Address peer("127.0.0.1", 7); // echo server
//      assert(0 == pool.connect(peer, 1, bsls::TimeInterval(10.0), 5));
//      bslmt::ThreadUtil::sleep(15000000); // Give enough time to connect.
//      return 0;
//  }
//..
//
///Example 2: Implementing an Echo Server
/// - - - - - - - - - - - - - - - - - - -
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
//      btlmt::ChannelPoolConfiguration d_config;        // pool's config
//      btlmt::ChannelPool             *d_channelPool_p; // managed pool
//      bslma::Allocator              *d_allocator_p;    // memory manager
//      bslmt::Mutex                   *d_coutLock_p;   // synchronize 'cout'
//
//    private:
//      // Callback functions:
//      void poolStateCb(int state, int source, int severity);
//          // Output a message to 'stdout' indicating the specified 'state'
//          // associated with the specified 'source' has occurred, with the
//          // specified 'severity'.  Note that 'state' is one of the
//          // 'PoolEvents' constants, 'source' identifies the channel pool
//          // operation associated with this state (in this case, the
//          // 'SERVER_ID' passed to 'listen()' or 0 for pool states with no
//          // associated source), and 'severity' is one of the
//          // 'btlmt::ChannelPool::Severity' values.
//
//      void channelStateCb(int channelId, int sourceId, int state, void *ctx);
//          // Output a message to 'stdout' indicating the specified 'state',
//          // associated with the specified 'channelId' and 'sourceId', has
//          // occurred.  If 'state' is 'btlmt::ChannelPool::e_CHANNEL_DOWN'
//          // then shutdown the channel.  Note that the 'channelId' is a
//          // unique identifier chosen by the channel pool for each connection
//          // channel, 'sourceId' identifies the channel pool operation
//          // responsible for creating the channel (in this case, the
//          // 'SERVER_ID' passed to 'listen()'), 'state' is a
//          // 'btlmt::ChannelPool::ChannelState' enumeration value, and 'ctx'
//          // is the address of a context object provided for the channel
//          // (using 'setChannelContext()'), in this example we do not
//          // specify a context, so the value will be 0.
//
//      void dataCb(int           *numNeeded,
//                  btlb::Blob  *msg,
//                  int            channelId
//                  void          *context);
//          // Echo the specified 'msg' to the client on the channel
//          // identified by 'channelId' channel, load into the
//          // specified 'numNeeeded' the minimum length of
//          // additional data that is needed to complete a message, and close
//          // the communication channel.  Because this echo server is not
//          // interested in a discrete messages in a particular message
//          // format, the entire message in 'msg' is read, and 'numNeeded'
//          // will be set to 1 (indicating this callback should be invoked
//          // again as soon as any new data is read).
//
//      // NOT IMPLEMENTED
//      my_EchoServer(const my_EchoServer&);
//      my_EchoServer& operator=(const my_EchoServer&);
//
//    public:
//      my_EchoServer(bslmt::Mutex      *coutLock,
//                    int               portNumber,
//                    int               numConnections,
//                    bslma::Allocator *basicAllocator = 0);
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
//      const btlmt::ChannelPool& pool() { return *d_channelPool_p; }
//  };
//..
// In the constructor of 'my_EchoServer', the configuration is initialized,
// the channel pool is created, configured, and started.  The listening port
// is established:
//..
//  my_EchoServer::my_EchoServer(bslmt::Mutex      *coutLock,
//                               int               portNumber,
//                               int               numConnections,
//                               bslma::Allocator *basicAllocator)
//  : d_allocator_p(bslma::Default::allocator(basicAllocator))
//  , d_coutLock_p(coutLock) {
//      d_config.setMaxThreads(4);
//      d_config.setMaxConnections(numConnections);
//      d_config.setReadTimeout(5.0);       // in seconds
//      d_config.setMetricsInterval(10.0);  // seconds
//      d_config.setMaxWriteCache(1<<10);   // 1MB
//      d_config.setIncomingMessageSizes(1, 100, 1024);
//
//      btlmt::ChannelPool::ChannelStateChangeCallback channelStateFunctor(
//              &my_EchoServer::channelStateCb
//            , this)
//            , basicAllocator);
//
//      btlmt::ChannelPool::PoolStateChangeCallback poolStateFunctor(
//              &my_EchoServer::poolStateCb
//            , this)
//            , basicAllocator));
//
//      btlmt::ChannelPool::DataReadCallback dataFunctor(
//              &my_EchoServer::dataCb
//            , this)
//            , basicAllocator));
//
//      d_channelPool_p = new (*d_allocator_p)
//          btlmt::ChannelPool(channelStateFunctor,
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
//        case btlmt::ChannelPool::e_CHANNEL_DOWN: {
//            btlso::IPv4Address peer;
//            d_channelPool_p->getPeerAddress(&peer, channelId);
//            d_coutLock_p->lock();
//            cout << "Client from " << peer << " has disconnected." << endl;
//            d_coutLock_p->unlock();
//            d_channelPool_p->shutdown(channelId,
//                                      btlmt::ChannelPool::e_IMMEDIATE);
//        } break;
//        case btlmt::ChannelPool::e_CHANNEL_UP: {
//            btlso::IPv4Address peer;
//            d_channelPool_p->getPeerAddress(&peer, channelId);
//            d_coutLock_p->lock();
//            cout << "Client connected from " << peer << endl;
//            d_coutLock_p->unlock();
//        } break;
//      }
//  }
//
//  void my_EchoServer::dataCb(int          *numNeeded,
//                             btlb::Blob *msg,
//                             int           channelId,
//                             void         *context)
//  {
//      assert(msg);
//      assert(0 < msg->length());
//
//      assert(0 == d_channelPool_p->write(channelId, *msg));
//
//      msg->removeAll();
//
//      *numNeeded = 1;
//
//      d_channelPool_p->shutdown(channelId,
//                                btlmt::ChannelPool::e_IMMEDIATE);
//  }
//..
// The implementation of an echo server is now complete.  Let's create
// a small program that uses it.  We will create a server object, then
// the main thread will monitor the channel pool and periodically print
// its busy metrics.  For simplicity, we will use the following function
// for monitoring:
//..
//  static void monitorPool(bslmt::Mutex              *coutLock,
//                          const btlmt::ChannelPool&  pool,
//                          int                        numTimes)
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
//       bslmt::Mutex coutLock;
//       my_EchoServer echoServer(&coutLock, PORT_NUMBER, MAX_CONNECTIONS);
//       monitorPool(&coutLock, echoServer.pool(), NUM_MONITOR);
//       return 0;
//
//   }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLMT_CHANNELPOOLCONFIGURATION
#include <btlmt_channelpoolconfiguration.h>
#endif

#ifndef INCLUDED_BTLMT_CHANNELTYPE
#include <btlmt_channeltype.h>
#endif

#ifndef INCLUDED_BTLMT_TCPTIMEREVENTMANAGER
#include <btlmt_tcptimereventmanager.h>
#endif

#ifndef INCLUDED_BTLS_IOVECUTIL
#include <btls_iovecutil.h>
#endif

#ifndef INCLUDED_BTLSO_INETSTREAMSOCKETFACTORY
#include <btlso_inetstreamsocketfactory.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BDLCC_OBJECTCATALOG
#include <bdlcc_objectcatalog.h>
#endif

#ifndef INCLUDED_BTLB_BLOB
#include <btlb_blob.h>
#endif

#ifndef INCLUDED_BDLMA_CONCURRENTPOOLALLOCATOR
#include <bdlma_concurrentpoolallocator.h>
#endif

#ifndef INCLUDED_BTLB_POOLEDBLOBBUFFERFACTORY
#include <btlb_pooledblobbufferfactory.h>
#endif

#ifndef INCLUDED_bslmt_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_bslmt_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BDLT_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTR
#include <bslma_managedptr.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTRDELETER
#include <bslma_managedptrdeleter.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifdef BSLS_PLATFORM_OS_UNIX
#ifndef INCLUDED_BSL_C_LIMITS
#include <bsl_c_limits.h>      // for IOV_MAX
#endif
#endif

namespace BloombergLP {

namespace btlso {

class IPv4Address;
class SocketOptions;

}

namespace btlmt {

class Channel;
class Connector;
class ServerState;

                       //==================
                       // struct TimerState
                       //==================

struct TimerState {
    // Provide a description of a scheduled timer event.  Note that a
    // 'ChannelPool' associates a 'TimerState' object with each timer callback
    // it registers with an underlying 'TcpTimerEventManager'.
    //
    // This class is an implementation detail of 'ChannelPool', and is *not*
    // intended to be used in client code.

    void                       *d_eventManagerId; // identifies the timer in
                                                  // 'd_eventManager_p'

    TcpTimerEventManager       *d_eventManager_p; // event manager the timer
                                                  // is registered with

    bsls::TimeInterval          d_absoluteTime;   // next scheduled occurrence
                                                  // (as an offset from the
                                                  // epoch time)

    bsls::TimeInterval          d_period;         // if a positive value, the
                                                  // periodic interval for
                                                  // the timer; otherwise it
                                                  // is a non-recurring timer

    bsl::function<void()>       d_callback;       // callback function to
                                                  // invoke
};

                       //==================
                       // class ChannelPool
                       //==================

class ChannelPool {
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
    // passed at construction of this pool.  An existing socket can be imported
    // and this will create a channel enabled both for read and for write.
    // This channel pool can dispatch events to be executed at different times
    // at recurring intervals (see the section "Clock management").  All this
    // processing will be performed in a number of threads.  The channel pool
    // can be started or stopped (see the section "Threads management").  Once
    // started and until stopped, the channel pool dispatches incoming and
    // outgoing connections, messages, and other channel functions to the
    // processing threads.  Once stopped, the channel pool can be started again
    // and the channels will resume their operations.  This channel pool keeps
    // a set of metrics (see the "Metrics" section).  It can be configured at
    // construction by passing a 'ChannelPoolConfiguration' object.

  public:
    // TYPES
    typedef bsl::function<void(int, int, int, void*)>
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
        // 'setChannelContext') channel context or '(void*)0' if no context was
        // specified.  Users MUST handle the 'e_CHANNEL_DOWN' event, minimally
        // by calling 'shutdown' on the channel ID.  The prototype for a
        // channel state callback might look like:
        //..
        //  void channelStateCallback(int   channelId,
        //                            int   sourceId,
        //                            int   state,
        //                            void *context);
        //..

    typedef bsl::function<void(int *, btlb::Blob *, int, void *)>
                                                         BlobBasedReadCallback;
        // The callback of this type is invoked every time there is a
        // sufficiently large amount of data read from a channel.  The second
        // argument to this callback is passed the data read from the channel
        // in the form of a modifiable 'btlb::Blob'.  The channel pool expects
        // that clients take ownership of some of the data in the passed
        // 'btlb::Blob' and readjust the 'btlb::Blob' accordingly.  The third
        // argument specifies the channel ID.  The fourth and last parameter is
        // passed the user-specified channel context (set using
        // 'setChannelContext()') or '(void *)0' if no context was specified.
        // The callback, when invoked, must store into the first argument the
        // minimum length of additional data that is needed to complete a
        // message.  If there is not enough data for a single message of a
        // particular protocol, 0 must be stored into the first argument.  The
        // prototype for a data callback might look like:
        //..
        //  void blobDataCallback(int          *numNeeded,
        //                        btlb::Blob *message,
        //                        int           channelId,
        //                        void         *context);
        //..

    typedef bsl::function<void(int, int, int)> PoolStateChangeCallback;
        // The callback of this type is invoked whenever a change affecting the
        // pool occurs.  The first parameter indicates the type of event which
        // triggered the callback (i.e., one of the 'PoolEvents' enumerations).
        // The second parameter indicates the source of the event (e.g., the
        // 'serverId' passed to 'listen()' or the 'sourceId' passed to
        // 'connect()'), or is 0 if there is no associated source (e.g. a
        // 'e_CHANNEL_LIMIT' alert).  The third parameter indicates the
        // severity of the event (must be one of the 'Severity' enumerations).
        // The prototype of a pool state callback might look like:
        //..
        //  void poolStateCallback(int eventType, int sourceId, int severity);
        //..

    enum ChannelEvents {
        // This enumeration provides names for the different values passed into
        // the third argument to 'ChannelCallback' to discriminate between
        // various changes in the state of a channel.

        e_CHANNEL_DOWN         = 0,
        e_CHANNEL_UP           = 1,
        e_READ_TIMEOUT         = 2,
        e_WRITE_BUFFER_FULL    = 3,
        e_MESSAGE_DISCARDED    = 4,
        e_AUTO_READ_ENABLED    = 5,
        e_AUTO_READ_DISABLED   = 6,
        e_WRITE_CACHE_LOWWAT   = 7,
        e_WRITE_CACHE_HIWAT    = e_WRITE_BUFFER_FULL,
        e_CHANNEL_DOWN_READ    = 8,
        e_CHANNEL_DOWN_WRITE   = 9


    };

    enum PoolEvents {
        e_ACCEPT_TIMEOUT = 0,           // timed out accepting a connection
        e_ERROR_ACCEPTING,              // error accepting a connection
        e_ERROR_CONNECTING,             // error connecting to the peer
        e_CHANNEL_LIMIT,                // channel limit reached
        e_CAPACITY_LIMIT,               // capacity limit reached
        e_ERROR_BINDING_CLIENT_ADDR,    // error binding client address
        e_ERROR_SETTING_OPTIONS,        // error setting socket options
        e_EVENT_MANAGER_LIMIT           // event manager limit reached

    };

    enum ConnectResolutionMode {
        // Mode indicating whether to perform name resolution at each connect
        // attempt in 'connect'.

        e_RESOLVE_ONCE                = 0,  // perform resolution once prior
                                            // to the first connect attempt

        e_RESOLVE_AT_EACH_ATTEMPT     = 1   // perform resolution prior to each
                                            // connect attempt
    };

    enum KeepHalfOpenMode {
        // Mode affecting how half-open connections are handled by a server or
        // a client channel, passed to 'connect', 'import' or 'listen'.

        e_CLOSE_BOTH         = 0,  // close whole channel if half-open
                                   // connection

        e_KEEP_HALF_OPEN     = 1   // keep either part alive, if the other
                                   // half senses a closed connection by
                                   // the peer


    };

    enum ShutdownMode {
        // Mode affecting how channel is terminated, passed to 'shutdown'.

        e_IMMEDIATE     = 0  // The channel is terminated immediately, all
                             // pending messages are discarded.


    };

    enum Severity {
        // This enumeration provides names for different levels of severity.

        e_CRITICAL      = 0, // A critical condition occurred and the channel
                             // pool is unable to operate normally.

        e_ALERT         = 1  // An alerting condition occurred and the channel
                             // pool can operate normally.


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

        btlso::SocketHandle::Handle d_handle;       // socket handle (file
                                                    // descriptor)

        ChannelType::Value          d_channelType;  // indicates how the
                                                    // channel was created

        int                         d_channelId;    // channel using this
                                                    // file descriptor

        bsls::TimeInterval          d_creationTime; // when was this channel
                                                    // created

        bslmt::ThreadUtil::Handle   d_threadHandle; // manager's dispatcher
                                                    // thread

        int                         d_userId;       // 'serverId' or 'sourceId'
    };

  private:
    // PRIVATE TYPES
    typedef bsl::shared_ptr<Channel>     ChannelHandle;
    typedef bsl::shared_ptr<ServerState> ServerHandle;

    // INSTANCE DATA
                                        // *** Transport-related state ***
    bdlcc::ObjectCatalog<ChannelHandle> d_channels;

    bsl::vector<TcpTimerEventManager *> d_managers;

    mutable bslmt::Mutex                d_managersStateChangeLock;
                                                    // mutex to synchronize
                                                    // changing the state of
                                                    // the event managers

    bsl::map<int, Connector>            d_connectors;

    mutable bslmt::Mutex                d_connectorsLock;

    bsl::map<int, ServerHandle>         d_acceptors;

    mutable bslmt::Mutex                d_acceptorsLock;

    bdlma::ConcurrentPoolAllocator      d_sharedPtrRepAllocator;

    bslma::ManagedPtr<btlb::BlobBufferFactory>
                                        d_writeBlobFactory;

    bslma::ManagedPtr<btlb::BlobBufferFactory>
                                        d_readBlobFactory;

    bslmt::Mutex                        d_timersLock;

    bsl::map<int, TimerState>           d_timers;

                                        // *** Parameters ***

    ChannelPoolConfiguration            d_config;

    bsls::AtomicOperations::AtomicTypes::Int
                                        d_capacity;

    int                                 d_startFlag;

    bool                                d_collectTimeMetrics;
                                               // whether to collect time
                                               // metrics

                                        // *** Capacity monitoring ***

    bdlb::NullableValue<void *>         d_metricsTimerId;

    bsl::function<void()>               d_metricsFunctor;

    ChannelStateChangeCallback          d_channelStateCb;

    PoolStateChangeCallback             d_poolStateCb;

    BlobBasedReadCallback               d_blobBasedReadCb;

                                        // *** Metrics ***

    bsls::AtomicInt                     d_totalConnectionsLifetime;

    bsls::TimeInterval                  d_lastResetTime;

    volatile bsls::Types::Int64         d_totalBytesReadAdjustment;
                                               // adjustment to
                                               // the sum of individual
                                               // channel numBytesRead(),
                                               // accounting for closed
                                               // channels and calls to
                                               // 'resetTotalBytesRead'

    volatile bsls::Types::Int64         d_totalBytesWrittenAdjustment;
                                               // adjustment to
                                               // the sum of individual
                                               // channel numBytesWritten(),
                                               // accounting for closed
                                               // channels and calls to
                                               // 'resetTotalBytesWritten'

    volatile bsls::Types::Int64         d_totalBytesRequestedWrittenAdjustment;
                                               // adjustment to
                                               // the sum of individual
                                               // channel values,
                                               // accounting for closed
                                               // channels and calls to
                                               // reset

    mutable bslmt::Mutex                d_metricAdjustmentMutex;
                                               // synchronize operations on
                                               // two metric adjustment values

                                        // *** Memory allocation ***

    btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                        d_factory;

    bdlma::ConcurrentPool               d_pool;        // for Channel
                                                       // (owned)

    bslma::Allocator                   *d_allocator_p; // (held, not owned)

  private:
    // FRIENDS
    friend class Channel;

    // PRIVATE MANIPULATORS
    TcpTimerEventManager *allocateEventManager();
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

    void acceptRetryCb(int serverId, ServerHandle server);
        // Re-register listening socket for the server whose ID is 'serverId'
        // to match 'ACCEPT' events (and invoke callback 'acceptCb' on such
        // events).  This callback is called after no resources were available
        // to accept a connection, and the socket was subsequently
        // deregistered.  This callback is scheduled in an exponential backoff
        // sequence fashion.  The exponential series is reset once a call to
        // 'accept' stops returning 'btlso::SocketHandle::e_ERROR_NORESOURCES'.

    void acceptTimeoutCb(int serverId, ServerHandle server);
        // Issue a pool callback with 'ACCEPT_TIMEOUT' and re-schedule this
        // timeout callback for the server whose ID is 'it->first', if the
        // listening socket held in the server state 'it->second' did not
        // receive a connection attempt in the timeout period specified in the
        // server state since the last server connection or last timeout
        // callback.

    int listen(const btlso::IPv4Address&   endpoint,
               int                         backlog,
               int                         serverId,
               int                         reuseAddress,
               bool                        readEnabledFlag,
               KeepHalfOpenMode            mode,
               bool                        isTimedFlag,
               const bsls::TimeInterval&   timeout = bsls::TimeInterval(),
               const btlso::SocketOptions *socketOptions = 0);
        // Establish a listening socket having the specified 'backlog' maximum
        // number of pending connections on the specified 'endpoint' and the
        // specified 'reuseAddress' used in setting 'e_REUSEADDRESS' socket
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
        // unless '0 < backlog'.

                                  // *** Client part ***

    void connectCb(bsl::map<int,Connector>::iterator it);
        // Add a newly allocated channel to the set of channels managed by this
        // channel pool and invoke the channel state callback.  Note that this
        // method is executed whenever a valid connection is established
        // corresponding to the client whose ID is 'it->first'.  All other
        // information (e.g., connection socket and event manager) is held in
        // the connector state 'it->second'.

    void connectEventCb(bsl::map<int,Connector>::iterator it);
        // Check the connection status of the connection socket upon call back
        // from the socket event 'btlso::EventType::e_CONNECT'.  If the
        // connection is valid then call 'connectCb', otherwise this socket
        // timed out and we must close it, reopen another socket, and
        // re-attempt a connection.  All other information (e.g., connection
        // socket and event manager) is held in the connector state
        // 'it->second'.

    void connectInitiateCb(bsl::map<int,Connector>::iterator it);
        // Initiate a connection for the client whose clientId is 'it->first',
        // and upon success proceed to 'connectCb'.  Otherwise, register
        // 'connectEventCb' for when this connection is established or times
        // out, or upon failure, invoke a pool state callback with
        // 'e_ERROR_CONNECTING', clientId given by the specified 'it->first',
        // and severity 'e_ALERT'.  All other information (e.g., connection
        // socket and event manager) is held in the connector state
        // 'it->second'.

    void connectTimeoutCb(bsl::map<int,Connector>::iterator it);
        // Decrease the number of attempts held in the connector state.  Once
        // the number of attempts reaches zero, the connection event is
        // deregistered and the connector removed from 'd_connectors'.  Note
        // that this callback is invoked if the connecting socket held in the
        // connector state 'it->second' did not establish a connection in the
        // timeout period specified in the server state, either through
        // 'connectInitiateCb' or through a 'connectEventCb' after the last
        // 'connectInitiateCb'.

    int connectImp(const btlso::IPv4Address&   serverAddress,
                   int                         numAttempts,
                   const bsls::TimeInterval&   interval,
                   int                         sourceId,
                   bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                              *socket,
                   bool                        readEnabledFlag,
                   KeepHalfOpenMode            mode,
                   const btlso::SocketOptions *socketOptions,
                   const btlso::IPv4Address   *localAddress);
        // Asynchronously issue up to the specified 'numAttempts' connection
        // requests to a server at the specified 'serverAddress', with at least
        // the specified (relative) time 'interval' after each attempt before
        // either a new connection is retried (if 'numAttempts' is not reached)
        // or the connection attempts are abandoned (if 'numAttempts' is
        // reached).  When the connection is established, an internal channel
        // is created and a channel state callback, with the event
        // 'e_CHANNEL_UP', the newly created channel ID, and the specified
        // 'sourceId' is invoked in an internal thread.  If the 'interval' is
        // reached, or in case other events occur (e.g., 'e_ERROR_CONNECTING',
        // 'e_CHANNEL_LIMIT', or 'e_CAPACITY_LIMIT'), a pool state callback is
        // invoked with the event type, 'sourceId' and a severity.  Use the
        // specified 'readEnabledFlag' to indicate whether automatic reading
        // should be enabled on this channel immediately after creation and the
        // specified half-close 'mode' in case the channel created for this
        // connection is half-closed.  Specify either 'socketOptions' that will
        // be used to provide the options that should be set on the connecting
        // socket and the specified 'localAddress' to be used as the source
        // address, or specify 'socket' to use as the connecting socket (with
        // any desired options and source address already set).  If 'socket' is
        // specified, this pool will assume its ownership if this function
        // returns successfully, and will be left unchanged if an error is
        // returned.  Return 0 on successful initiation, a positive value if
        // there is an active connection attempt with the same 'sourceId' (in
        // which case this connection attempt may be retried after that other
        // connection either succeeds, fails, or times out), or a negative
        // value if an error occurred, with the value of -1 indicating that the
        // channel pool is not running.  The behavior is undefined unless
        // '0 < numAttempts', '0 < interval || 1 == numAttempts', and
        // '0 == socketOptions || (0 == socket && 0 == localAddress)'.

    int connectImp(const char                 *hostname,
                   int                         portNumber,
                   int                         numAttempts,
                   const bsls::TimeInterval&   interval,
                   int                         sourceId,
                   bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                              *socket,
                   ConnectResolutionMode       resolutionMode,
                   bool                        readEnabledFlag,
                   KeepHalfOpenMode            halfCloseMode,
                   const btlso::SocketOptions *socketOptions,
                   const btlso::IPv4Address   *localAddress);
        // Asynchronously issue up to the specified 'numAttempts' connection
        // requests to a server at the address resolved from the specified
        // 'hostname' on the specified 'portNumber', with at least the
        // specified (relative) time 'interval' after each attempt before
        // either a new connection is retried (if 'numAttempts' is not reached)
        // or the connection attempts are abandoned (if 'numAttempts' is
        // reached).  When the connection is established, an internal channel
        // is created and a channel state callback, with the event
        // 'e_CHANNEL_UP', the newly created channel ID, and the specified
        // 'sourceId', is invoked in an internal thread.  If the 'interval' is
        // reached, or in case other events occur (e.g., 'e_ERROR_CONNECTING',
        // 'e_CHANNEL_LIMIT', or 'e_CAPACITY_LIMIT'), a pool state callback is
        // invoked with the event type, 'sourceId' and a severity.  Use the
        // specified 'resolutionMode' to indicate whether the name resolution
        // is performaed once (if 'resolutionMode' is 'e_RESOLVE_ONCE'), or
        // performed anew prior to each attempt (if 'resolutionMode' is
        // 'e_RESOLVE_AT_EACH_ATTEMPT'), the specified 'readEnabledFlag' to
        // indicate whether automatic reading should be enabled on this channel
        // immediately after creationo, and the specified 'halfCloseMode' in
        // case the channel created for this connection is half-closed.
        // Specify either 'socketOptions' that will be used to provide the
        // options that should be set on the connecting socket and the
        // specified 'localAddress' to be used as the source address, or
        // specify 'socket' to use as the connecting socket (with any desired
        // options and source address already set).  If 'socket' is specified,
        // this pool will assume ownership its ownership if this function
        // returns successfully, and will be left unchanged if an error is
        // returned.  Return 0 on successful initiation, a positive value if
        // there is an active connection attempt with the same 'sourceId' (in
        // which case this connection attempt may be retried after that other
        // connection either succeeds, fails, or times out), or a negative
        // value if an error occurred, with the value of -1 indicating that the
        // channel pool is not running.  The behavior is undefined unless
        // '0 < numAttempts', '0 < interval || 1 == numAttempts', and
        // '0 == socketOptions || (0 == socket && 0 == localAddress)'

                                  // *** Channel management part ***
    void importCb(btlso::StreamSocket<btlso::IPv4Address> *socket,
                  const bslma::ManagedPtrDeleter&          deleter,
                  TcpTimerEventManager                    *manager,
                  TcpTimerEventManager                    *srcManager,
                  int                                      sourceId,
                  bool                                     readEnabledFlag,
                  bool                                     mode,
                  bool                                     imported);
        // Add a newly allocated channel to the set of channels managed by this
        // channel pool and invoke the channel pool callback in the specified
        // 'manager'.  Upon destruction, 'socket' will be destroyed via the
        // specified 'factory'.  Note that this method is executed whenever a
        // connection is imported on the 'socket' corresponding to 'sourceId'.
        // In addition, it is invoked by 'connectCb' to create a newly
        // allocated channel once the socket connection is established.  This
        // function should be executed in the dispatcher thread of the
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
        // the specified 'channelId'.  Return 0 on success, or a non-zero value
        // if there is no valid channel channel is associated with 'channelId'.
        // Note that a channel handle in 'd_channels' may be null, if the
        // channel has been added but not yet initialized.

  private:
    // NOT IMPLEMENTED
    ChannelPool(const ChannelPool& original);
    ChannelPool& operator=(const ChannelPool& rhs);

  public:
    // CREATORS
    ChannelPool(ChannelStateChangeCallback       channelStateCb,
                BlobBasedReadCallback            blobBasedReadCb,
                PoolStateChangeCallback          poolStateCb,
                const ChannelPoolConfiguration&  parameters,
                bslma::Allocator                *basicAllocator = 0);
        // Create a channel pool with the specified 'channelStateCb',
        // 'blobBasedReadCb' and 'poolStateCb' callbacks to be invoked,
        // correspondingly, when a channel state changes, data arrives, or pool
        // state changes.  The channel pool is configured using the specified
        // configuration 'parameters'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.
        //
        // The exact description of the argument to the callbacks are given in
        // the type definitions of the 'ChannelStateChangeCallback',
        // 'BlobBasedReadCallback', and 'PoolStateChangeCallback'.

    ChannelPool(btlb::BlobBufferFactory       *blobBufferFactory,
                ChannelStateChangeCallback       channelStateCb,
                BlobBasedReadCallback            blobBasedReadCb,
                PoolStateChangeCallback          poolStateCb,
                const ChannelPoolConfiguration&  parameters,
                bslma::Allocator                *basicAllocator = 0);
        // Create a channel pool having the specified 'channelStateCb',
        // 'blobBasedReadCb' and 'poolStateCb' callbacks to be invoked, when a
        // channel state changes, data arrives, or pool state changes
        // (respectively), and using the specified 'blobBufferFactory' to
        // supply buffers for all internal read and write blobs including those
        // passed to 'blobBasedReadCb'.  The channel pool is configured using
        // the specified configuration 'parameters'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.
        //
        // The exact description of the argument to the callbacks are given in
        // the type definitions of the 'ChannelStateChangeCallback',
        // 'BlobBasedReadCallback', and 'PoolStateChangeCallback'.

    ~ChannelPool();
        // Destroy this channel pool.  The behavior is undefined if the channel
        // pool was not shut down properly.

    // MANIPULATORS
                                  // *** Server part ***

    int close(int serverId);
        // Close the listening socket corresponding to the specified
        // 'serverId'.  Return 0 on success, and a non-zero value otherwise.
        // Note that closing a listening socket has no effect on any channels
        // managed by this pool.

    int listen(int                         portNumber,
               int                         backlog,
               int                         serverId,
               int                         reuseAddress = 1,
               bool                        readEnabledFlag = true,
               const btlso::SocketOptions *socketOptions = 0);
    int listen(int                         portNumber,
               int                         backlog,
               int                         serverId,
               const bsls::TimeInterval&   timeout,
               int                         reuseAddress = 1,
               bool                        readEnabledFlag = true,
               const btlso::SocketOptions *socketOptions = 0);
    int listen(const btlso::IPv4Address&   endpoint,
               int                         backlog,
               int                         serverId,
               int                         reuseAddress = 1,
               bool                        readEnabledFlag = true,
               const btlso::SocketOptions *socketOptions = 0);
    int listen(const btlso::IPv4Address&   endpoint,
               int                         backlog,
               int                         serverId,
               const bsls::TimeInterval&   timeout,
               int                         reuseAddress = 1,
               bool                        readEnabledFlag = true,
               KeepHalfOpenMode            mode = e_CLOSE_BOTH,
               const btlso::SocketOptions *socketOptions = 0);
        // Establish a listening socket having the specified 'backlog' maximum
        // number of pending connections on the specified 'portNumber' on all
        // local interfaces or the specified 'endpoint', depending on which
        // overload of 'listen' is used, and associate this newly established
        // socket with the specified 'serverId'.  Optionally, specify a
        // 'timeout' *duration* for accepting a connection.  If no connection
        // attempt is received for a period of 'timeout' since the last
        // connection or the last timeout, a pool state callback is invoked
        // with event equal to 'e_ACCEPT_TIMEOUT'.  Optionally specify a
        // 'reuseAddress' value to be used in setting 'e_REUSEADDRESS' socket
        // option; if 'reuseAddress' is not specified, 1 is used (i.e.,
        // 'e_REUSEADDRESS' is enabled).  Optionally specify via a
        // 'readEnabledFlag' whether automatic reading should be enabled on
        // this channel immediately after creation; if 'readEnabledFlag' is not
        // specified, then 'true' is used (i.e., reading on new channels is
        // automatically enabled).  If 'endpoint', 'timeout', 'reuseAddress'
        // and 'readEnabledFlag' are all specified, also optionally specify a
        // 'mode' to keep channel half-open in case a channel established by
        // the server with this 'serverId' is half-closed; if 'mode' is not
        // specified, then 'e_CLOSE_BOTH' is used (i.e., half-open connections
        // lead to closing the channel completely).  Optionally specify
        // 'socketOptions' that will be used to indicate what options should be
        // set on the listening socket.  Return 0 on success, a positive value
        // if there is a listening socket associated with 'serverId' (i.e.,
        // 'serverId' is not unique) and a negative value if an error occurred.
        // Every time a connection is accepted by this pool on this (newly
        // established) listening socket, 'serverId' is passed to the callback
        // provided in the configuration at construction.  The behavior is
        // undefined unless '0 < backlog'.

                                  // *** Client part ***

    int connect(const char                 *hostname,
                int                         portNumber,
                int                         numAttempts,
                const bsls::TimeInterval&   interval,
                int                         sourceId,
                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                           *socket,
                ConnectResolutionMode       resolutionMode = e_RESOLVE_ONCE,
                bool                        readEnabledFlag = true,
                KeepHalfOpenMode            halfCloseMode = e_CLOSE_BOTH);
    int connect(const char                 *hostname,
                int                         portNumber,
                int                         numAttempts,
                const bsls::TimeInterval&   interval,
                int                         sourceId,
                ConnectResolutionMode       resolutionMode = e_RESOLVE_ONCE,
                bool                        readEnabledFlag = true,
                KeepHalfOpenMode            halfCloseMode = e_CLOSE_BOTH,
                const btlso::SocketOptions *socketOptions = 0,
                const btlso::IPv4Address   *localAddress = 0);
        // Asynchronously issue up to the specified 'numAttempts' connection
        // requests to a server at the address resolved from the specified
        // 'hostname' on the specified 'portNumber', with at least the
        // specified (relative) time 'interval' after each attempt before
        // either a new connection is retried (if 'numAttempts' is not reached)
        // or the connection attempts are abandoned (if 'numAttempts' is
        // reached).  When the connection is established, an internal channel
        // is created and a channel state callback, with the event
        // 'e_CHANNEL_UP', the newly created channel ID, and the specified
        // 'sourceId' is invoked in an internal thread.  If the 'interval' is
        // reached, or in case other events occur (e.g., 'e_ERROR_CONNECTING',
        // 'e_CHANNEL_LIMIT', or 'e_CAPACITY_LIMIT'), a pool state callback is
        // invoked with the event type, 'sourceId' and a severity.  Optionally
        // specify a 'resolutionMode' to indicate whether the name resolution
        // is performed once (if 'resolutionMode' is 'e_RESOLVE_ONCE'), or
        // performed anew prior to each attempt (if 'resolutionMode' is
        // 'e_RESOLVE_AT_EACH_ATTEMPT'); if 'resolutionMode' is not specified,
        // 'e_RESOLVE_ONCE' is used.  Optionally specify via a
        // 'readEnabledFlag' whether automatic reading should be enabled on
        // this channel immediately after creation; if 'readEnabledFlag' is not
        // specified, then 'true' is used (i.e., reading on new channels is
        // automatically enabled).  Optionally specify a 'halfCloseMode' in
        // case the channel created for this connection is half-closed; if
        // 'mode' is not specified, then 'e_CLOSE_BOTH' is used (i.e.,
        // so-called half-open connections, that is, anything less than full
        // duplex, lead to close the channel).  Optionally specify either
        // 'socketOptions' that will be used to specify what options should be
        // set on the connecting socket and/or the specified 'localAddress' to
        // be used as the source address, or specify 'socket' to use as the
        // connecting socket (with any desired options and/or source address
        // already set).  If 'socket' is specified, this pool will assume its
        // ownership if this function returns successfully, and will be left
        // unchanged if an error is returned.  Return 0 on successful
        // initiation, a positive value if there is an active connection
        // attempt with the same 'sourceId' (in which case this connection
        // attempt may be retried after that other connection either succeeds,
        // fails, or times out), or a negative value if an error occurred, with
        // the value of -1 indicating that the channel pool is not running.
        // The behavior is undefined unless '0 < numAttempts', and either
        // '0 < interval' or '1 == numAttempts' or both.  Note that if the
        // connection cannot be established, up to 'numAttempts' pool state
        // callbacks with 'e_ERROR_CONNECTING' may be generated, one for each
        // 'interval'.  Also note that this function will fail if this channel
        // pool is not running, and that no callbacks will be invoked if the
        // return value is non-zero.  Also note that the same 'sourceId' can be
        // used in several calls to 'connect' or 'import' as long as two calls
        // to connect with the same 'sourceId' do not overlap.  Finally, note
        // that the lifetime of the 'hostname' need not extend past the return
        // of this function call, that is, 'hostname' need not remain valid
        // until the last connection attempt but can be deleted upon return.

    int connect(const btlso::IPv4Address&   serverAddress,
                int                         numAttempts,
                const bsls::TimeInterval&   interval,
                int                         sourceId,
                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                           *socket,
                bool                        readEnabledFlag = true,
                KeepHalfOpenMode            mode = e_CLOSE_BOTH);
    int connect(const btlso::IPv4Address&   serverAddress,
                int                         numAttempts,
                const bsls::TimeInterval&   interval,
                int                         sourceId,
                bool                        readEnabledFlag = true,
                KeepHalfOpenMode            mode = e_CLOSE_BOTH,
                const btlso::SocketOptions *socketOptions = 0,
                const btlso::IPv4Address   *localAddress = 0);
        // Asynchronously issue up to the specified 'numAttempts' connection
        // requests to a server at the specified 'serverAddress', with at least
        // the specified (relative) time 'interval' after each attempt before
        // either a new connection is retried (if 'numAttempts' is not reached)
        // or the connection attempts are abandoned (if 'numAttempts' is
        // reached).  When the connection is established, an internal channel
        // is created and a channel state callback, with the event
        // 'e_CHANNEL_UP', the newly created channel ID, and the specified
        // 'sourceId' is invoked in an internal thread.  If the 'interval' is
        // reached, or in case other events occur (e.g., 'e_ERROR_CONNECTING',
        // 'e_CHANNEL_LIMIT', or 'e_CAPACITY_LIMIT'), a pool state callback is
        // invoked with the event type, 'sourceId' and a severity.  Optionally
        // specify via a 'readEnabledFlag' whether automatic reading should be
        // enabled on this channel immediately after creation; if
        // 'readEnabledFlag' is not specified, then 'true' is used (i.e.,
        // reading on new channels is automatically enabled).  Optionally
        // specify a half-close 'mode' in case the channel created for this
        // connection is half-closed; if 'mode' is not specified, then
        // 'e_CLOSE_BOTH' is used (i.e., half-open connections lead to close
        // the channel).  Optionally specify either 'socketOptions' that will
        // be used to specify what options should be set on the connecting
        // socket and/or the specified 'localAddress' to be used as the source
        // address, or specify 'socket' to use as the connecting socket (with
        // any desired options and/or source address already set).  If 'socket'
        // is specified, this pool will assume its ownership.  Return 0 on
        // successful initiation, a positive value if there is an active
        // connection attempt with the same 'sourceId' (in which case this
        // connection attempt may be retried after that other connection either
        // succeeds, fails, or times out), or a negative value if an error
        // occurred, with the value of -1 indicating that the channel pool is
        // not running.  The behavior is undefined unless '0 < numAttempts',
        // and either '0 < interval' or '1 == numAttempts' or both.  Note that
        // if the connection cannot be established, up to 'numAttempts' pool
        // state callbacks with 'e_ERROR_CONNECTING' may be generated, one for
        // each 'interval'.  Also note that this function will fail if this
        // channel pool is not running, and that no callbacks will be invoked
        // if the return value is non-zero.  Also note that the same 'sourceId'
        // can be used in several calls to 'connect' or 'import' as long as two
        // calls to connect with the same 'sourceId' do not overlap.

                                  // *** Channel management ***

    int disableRead(int channelId);
        // Enqueue a request to disable automatic reading on the channel having
        // the specified 'channelId'.  Return 0 on success and a non-zero value
        // otherwise.  Once automatic reading is disabled a channel state
        // callback for this channel is invoked with 'e_AUTO_READ_DISABLED'
        // state.
        //
        // This method offers the following specific guarantees:
        // - When shutting down a channel, 'e_AUTO_READ_DISABLED' message
        //   is *not* generated.
        // - A data callback will always happen in between
        //   'e_AUTO_READ_ENABLED' and 'e_AUTO_READ_DISABLED'
        //   callbacks.
        // - The data currently enqueued in the channel pool for this channel
        //   is *not* discarded.

    int enableRead(int channelId);
        // Enqueue a request to enable automatic reading on the channel having
        // the specified 'channelId'.  Return 0 on success and a non-zero value
        // otherwise.  Once automatic reading is enabled a channel state
        // callback for this channel is invoked with 'e_AUTO_READ_ENABLED'
        // state.
        //
        // This method offers the following specific guarantees:
        // - By default, a newly created channel is in
        //   'e_AUTO_READ_ENABLED' state (except imported channels with
        //   'readEnabledFlag' not set), unless the 'readEnabledFlag was set to
        //   'false'.
        // - When a new channel is created and read is enabled, both
        //   'e_CHANNEL_UP' and e_AUTO_READ_ENABLED messages are
        //   generated, in this order.  However, 'e_CHANNEL_UP' and
        //   'e_AUTO_READ_ENABLED' may be generated from different threads.
        // - A data callback will always happen in between
        //   'e_AUTO_READ_ENABLED' and 'e_AUTO_READ_DISABLED'
        //   callbacks.

    int import(bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                                      *streamSocket,
               int                                     sourceId,
               bool                                    readEnabledFlag = true,
               KeepHalfOpenMode                        mode = e_CLOSE_BOTH);
        // Add the specified 'streamSocket' to this channel pool.  Assign a
        // channel ID and invoke a channel state callback, passing
        // 'e_CHANNEL_UP' and the specified 'sourceId', in an internal thread.
        // Assume ownership from 'streamSocket', leaving it null, if this
        // function returns successfully, and leave it unchanged if an error is
        // returned.  Optionally specify via 'readEnabledFlag' whether
        // automatic reading should be enabled on this channel immediately
        // after creation; if 'readEnabledFlag' is not specified, then 'true'
        // is used (i.e., reading on new channels is automatically enabled).
        // Optionally specify a half-close 'mode' in case the channel created
        // for this connection is half-closed; if 'mode' is not specified, then
        // 'e_CLOSE_BOTH' is used (i.e., half-open connections lead to close
        // the channel).  Return 0 on success and a non-zero value, with no
        // effect on the channel pool, otherwise.  Note that the same
        // 'sourceId' can be used in several calls to 'connect' or 'import' as
        // long as two calls to connect with the same 'sourceId' do not
        // overlap.  Also note that a half-closed 'streamSocket' can be
        // imported into this channel pool, irrespective of 'mode'.

    void setChannelContext(int channelId, void *context);
        // Associate the specified (opaque) 'context' with the channel having
        // the specified 'channelId'.  The channel context will be reported on
        // any invocation of a callback related to this channel.

    int shutdown(int                       channelId,
                 ShutdownMode              mode = e_IMMEDIATE);
    int shutdown(int                       channelId,
                 btlso::Flag::ShutdownType type,
                 ShutdownMode              mode = e_IMMEDIATE);
        // Shut down the communication channel having the specified 'channelId'
        // in the optionally specified 'mode' and return 0 on success, and a
        // non-zero value otherwise.  Optionally specify a shutdown 'type' to
        // close only the reading or the writing part; by default,
        // 'e_CLOSE_BOTH' is used (i.e., both halves of the channel are
        // closed).  Note that shutting down a channel will deallocate all
        // system resources associated with 'channel' and subsequent references
        // to channel will result in undefined behavior.  Also note that, if
        // the channel does not support half-open connections (i.e., the 'mode'
        // passed to 'connect', 'listen', or 'import' was set to
        // 'e_CLOSE_BOTH'), then shutting down the channel leads to a complete
        // shutdown, irrespective of the shutdown 'type'.  If the channel does
        // support half-open connections, but is already half-closed, and the
        // 'type' (set to 'e_SHUTDOWN_BOTH', 'e_SHUTDOWN_RECEIVE' or
        // 'e_SHUTDOWN_SEND') closes the other half, then the channel is shut
        // down completely; otherwise, only one half of the channel is closed
        // but the channel itself is not, and subsequent calls to write (if
        // 'type' is 'e_SHUTDOWN_SEND'), or to 'enableRead' (if 'type' is
        // 'e_SHUTDOWN_RECEIVE'), will fail.

    int stopAndRemoveAllChannels();
        // Terminate all threads managed by this channel pool, close all
        // listening sockets, close both the read and write parts of all
        // communication channels under management, and remove all those
        // communication channels from this channel pool.  Return 0 on success,
        // and a non-zero value otherwise.  If any attempt to terminate a
        // thread "gracefully" fails, previously terminated threads are
        // restarted and a negative value is returned.  The behavior is
        // undefined if 'start' is called concurrently or subsequent to the
        // completion of this call.  Note that shutting down a channel will
        // deallocate all system resources associated with that channel.  Also
        // note that this function is intended to be called to release
        // resources held by this channel pool just prior to its destruction.

    int setWriteCacheHiWatermark(int channelId, int numBytes);
        // Set the write cache high-water mark for the specified 'channelId' to
        // the specified 'numBytes'; return 0 on success, and a non-zero value
        // if either 'channelId' does not exist or 'numBytes' is less than the
        // low-water mark for the write cache.  A 'e_WRITE_CACHE_HIWAT' alert
        // is provided (via the channel state callback) if 'numBytes' is less
        // than or equal to the current size of the write cache.  (See the
        // "Invocation of High- and Low-Water Mark Callbacks" section under
        // @DESCRIPTION in the component-level documentation for details on
        // 'e_WRITE_CACHE_HIWAT' and 'e_WRITE_CACHE_LOWWAT' alerts.)  The
        // behavior is undefined unless '0 <= numBytes'.  Note that this method
        // overrides the value configured (for all channels) by the
        // 'ChannelPoolConfiguration' supplied at construction.

    int setWriteCacheLowWatermark(int channelId, int numBytes);
        // Set the write cache low-water mark for the specified 'channelId' to
        // the specified 'numBytes'; return 0 on success, and a non-zero value
        // if either 'channelId' does not exist or 'numBytes' is greater than
        // the high-water mark for the write cache.  A 'e_WRITE_CACHE_LOWWAT'
        // alert is provided (via the channel state callback) if 'numBytes' is
        // greater than or equal to the current size of the write cache.  (See
        // the "Invocation of High- and Low-Water Mark Callbacks" section under
        // @DESCRIPTION in the component-level documentation for details on
        // 'e_WRITE_CACHE_HIWAT' and 'e_WRITE_CACHE_LOWWAT' alerts.)  The
        // behavior is undefined unless '0 <= numBytes'.  Note that this method
        // overrides the value configured (for all channels) by the
        // 'ChannelPoolConfiguration' supplied at construction.

    int setWriteCacheWatermarks(int channelId,
                                int lowWatermark,
                                int hiWatermark);
        // Set the write cache low- and high-water marks for the specified
        // 'channelId' to the specified 'lowWatermark' and 'hiWatermark'
        // values, respectively; return 0 on success, and a non-zero value if
        // 'channelId' does not exist.  A 'e_WRITE_CACHE_LOWWAT' alert is
        // provided (via the channel state callback) if 'lowWatermark' is
        // greater than or equal to the current size of the write cache, and a
        // 'e_WRITE_CACHE_HIWAT' alert is provided if 'hiWatermark' is less
        // than or equal to the current size of the write cache.  (See the
        // "Invocation of High- and Low-Water Mark Callbacks" section under
        // @DESCRIPTION in the component-level documentation for details on
        // 'e_WRITE_CACHE_HIWAT' and 'e_WRITE_CACHE_LOWWAT' alerts.)  The
        // behavior is undefined unless '0 <= lowWatermark' and
        // 'lowWatermark <= hiWatermark'.  Note that this method overrides the
        // values configured (for all channels) by the
        // 'ChannelPoolConfiguration' supplied at construction.

    int resetRecordedMaxWriteCacheSize(int channelId);
        // Reset the recorded max write cache size for the specified
        // 'channelId' to the current write cache size.  Return 0 on success,
        // or a non-zero value if 'channelId' does not exist.  Note that this
        // function resets the recorded max write cache size and does not
        // change the write cache high-water mark for 'channelId'.

                                  // *** Thread management ***

    int start();
        // Create internal threads that monitor network events and invoke
        // corresponding callbacks supplied (in the configuration) at
        // construction.  Return 0 on success, and a non-zero value otherwise.
        // The behavior is undefined if the internal threads are created (as
        // reflected by the pool's state), see 'state'.

    int stop();
        // Gracefully terminate the worker threads; return 0 on success and a
        // non-zero value otherwise.  If any attempt to terminate a thread
        // "gracefully" fails, previously terminated threads are restarted and
        // a negative value is returned.  The behavior is undefined unless the
        // threads exist (i.e., were created successfully).  Note that this
        // function has no effect on the state of any channel managed by this
        // pool.

                                  // *** Incoming messages ***

    btlb::BlobBufferFactory *incomingBlobBufferFactory();
        // Return the address of the pooled blob buffer factory used by this
        // channel pool to produce blobs for incoming messages.

                                  // *** Outgoing messages ***

    btlb::BlobBufferFactory *outboundBlobBufferFactory();
        // Return the address of the blob buffer factory used by this channel
        // pool to produce blobs for outbound messages.
        //
        // Note that this version of channel pool now uses blob internally.  It
        // is more efficient to create blob messages, using the
        // 'outboundBlobBufferFactory()', than to create data messages using
        // 'outboundBufferFactory()'.

    int write(int channelId, const btlb::Blob& message);
    int write(int channelId, const btlb::Blob& message, int enqueueWatermark);
        // Enqueue a request to write the specified 'message' into the channel
        // having the specified 'channelId'.  Optionally specify an
        // 'enqueueWaterMark' to limit the size of the enqueued portion of the
        // message.  Return 0 on success, and a non-zero value otherwise.  On
        // error, the return value *may* equal to one of the enumerators in
        // 'ChannelStatus::Enum'.

    int write(int channelId, const btls::Iovec vecs[], int numVecs);
    int write(int channelId, const btls::Ovec  vecs[], int numVecs);
        // Enqueue a request to write the specified 'vecs' into the channel
        // having the specified 'channelId'.  Return 0 on success, and a
        // non-zero value otherwise.  On error, the return value *may* equal to
        // one of the enumerators in 'ChannelStatus::Enum'.  Note that you
        // should prefer this method over the other 'write()' method *only*
        // *if* you expect that this object will be able to write most of the
        // data contained in the specified 'vecs' atomically.  If the 'vecs'
        // must be enqueued, an inefficient data copy will occur to allow to
        // control the lifetime of the data.

                                  // *** Clock management ***

    int registerClock(const bsl::function<void()>& command,
                      const bsls::TimeInterval&    startTime,
                      const bsls::TimeInterval&    period,
                      int                          clockId);
    int registerClock(const bsl::function<void()>& command,
                      const bsls::TimeInterval&    startTime,
                      const bsls::TimeInterval&    period,
                      int                          clockId,
                      int                          channelId);
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

    int getLingerOption(btlso::SocketOptUtil::LingerData *result,
                        int                               channelId) const;
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
        // otherwise.  (See 'btlso::SocketOptUtil' for the set of commonly-used
        // options.)

    int getSocketOption(int *result,
                        int  option,
                        int  level,
                        int  channelId) const;
        // Load into the specified 'result' the value of the specified 'option'
        // of the specified 'level' socket option on the channel having the
        // specified 'channelId'.  Return 0 on success and a non-zero value
        // otherwise.  (See 'btlso::SocketOptUtil' for the set of commonly-used
        // options.)

    int setLingerOption(const btlso::SocketOptUtil::LingerData& value,
                        int                                     channelId);
        // Set the linger option on the channel with the specified 'channelId'
        // to the specified 'value'.  Return 0 on success and a non-zero value
        // otherwise.

    int setServerSocketOption(int option, int level, int value, int serverId);
        // Set the specified 'option' (of the specified 'level') socket option
        // on the listening socket with the specified 'serverId' to the
        // specified 'value'.  Return 0 on success and a non-zero value
        // otherwise.  (See 'btlso_socketoptutil' for the list of commonly
        // supported options.)

    int setSocketOption(int option, int level, int value, int channelId);
        // Set the specified 'option' (of the specified 'level') socket option
        // on the channel with the specified 'channelId' to the specified
        // 'value'.  Return 0 on success and a non-zero value otherwise.  (See
        // 'btlso_socketoptutil' for the list of commonly supported options.)

                                  // *** Metrics ***

    double reportWeightedAverageReset();
        // Return the weighted average of the connections lifetime since the
        // previous call to this method or, for the first call, since this
        // object construction, if at least one millisecond passed since then,
        // otherwise return -1.  If all the connections at the time of the call
        // have been up during the considered period of time and were already
        // up at the time of the previous reset, this method will return the
        // same number as 'numChannels()'.  0 means that they were all down.

    void totalBytesReadReset(bsls::Types::Int64 *result);
        // Load, into the specified 'result', and atomically reset the total
        // number of bytes read by the pool.

    void totalBytesWrittenReset(bsls::Types::Int64 *result);
        // Load, into the specified 'result', and atomically reset the total
        // number of bytes written by the pool.

    void totalBytesRequestedToBeWrittenReset(bsls::Types::Int64 *result);
        // Load, into the specified 'result', and atomically reset the total
        // number of bytes requested to be written by the pool.

    // ACCESSORS
    int busyMetrics() const;
        // Return the (percent) value in the range [0..100] (inclusive) that
        // reflects the workload of this channel pool (e.g., how busy it is for
        // the last period).  If the 'collectTimeMetrics' property of the
        // configuration supplied at construction is 'false' (i.e., the
        // collection of time metrics has been disable), then the returned
        // value is unspecified.  The value 0 indicates that the pool is idle
        // and 100 indicates that pool operates at the configured capacity.

    void *channelContext(int channelId) const;
        // Return a user-defined channel context associated with the specified
        // 'channelId', and '(void *)0' if no such channel exists or the user
        // context for this channel was explicitly set to '(void *)0'.

    int getChannelStatistics(bsls::Types::Int64 *numRead,
                             bsls::Types::Int64 *numRequestedToBeWritten,
                             bsls::Types::Int64 *numWritten,
                             int                 channelId) const;
        // Load into the specified 'numRead', 'numRequestedToBeWritten' and
        // 'numWritten' respectively the number of bytes read, requested to be
        // written and written by the channel identified by the specified
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

    int getServerAddress(btlso::IPv4Address *result, int serverId) const;
        // Load into the specified 'result' the complete IP address associated
        // with the server with the specified 'serverId' that is managed by
        // this channel pool if the server is established.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise.

    int getLocalAddress(btlso::IPv4Address *result, int channelId) const;
        // Load into the specified 'result' the complete IP address associated
        // with the local (i.e., this process) end-point of the communication
        // channel having the specified 'channelId'.  Return 0 on success, and
        // a non-zero value with no effect on 'result' otherwise.

    int getPeerAddress(btlso::IPv4Address *result, int channelId) const;
        // Load into the specified 'result' the complete IP address associated
        // with the remote (i.e., peer process) end-point of the communication
        // channel having the specified 'channelId'.  Return 0 on success, and
        // a non-zero value with no effect on 'result' otherwise.

    int numBytesRead(bsls::Types::Int64 *result, int channelId) const;
        // Load, into the specified 'result', the number of bytes read by the
        // channel identified by the specified 'channelId' and return 0 if the
        // specified 'channelId' is a valid channel id.  Otherwise, return a
        // non-zero value.

    int numBytesWritten(bsls::Types::Int64 *result, int channelId) const;
        // Load, into the specified 'result', the number of bytes written by
        // the channel identified by the specified 'channelId' and return 0 if
        // the specified 'channelId' is a valid channel id.  Otherwise, return
        // a non-zero value.

    int numBytesRequestedToBeWritten(bsls::Types::Int64 *result,
                                     int                 channelId) const;
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
        // Return the number of threads currently managed by this channel pool.
        // Note that each thread corresponds to a single event manager, and,
        // therefore, the number of threads is the number of active event
        // managers.

    bsl::shared_ptr<const btlso::StreamSocket<btlso::IPv4Address> >
                                             streamSocket(int channelId) const;
        // Return a shared pointer to the non-modifiable stream socket
        // associated with the specified 'channelId', and an empty shared
        // pointer if a corresponding channel does not exist.  The returned
        // shared pointer is aliased to the underlying channel and the channel
        // will not be closed until this shared pointer is destroyed.
        // Therefore, it is important that clients carefully manage the
        // lifetime of the returned shared pointer.  The behavior of this
        // channel pool is undefined if the underlying socket is manipulated
        // while still under management by this channel pool.

    void totalBytesRead(bsls::Types::Int64 *result) const;
        // Load, into the specified 'result', the total number of bytes read by
        // the pool.

    void totalBytesRequestedToBeWritten(bsls::Types::Int64 *result) const;
        // Load, into the specified 'result', the total number of bytes
        // requested to be written by the pool.

    void totalBytesWritten(bsls::Types::Int64 *result) const;
        // Load, into the specified 'result', the total number of bytes written
        // by the pool.
};

                 // ============================
                 // class ChannelPool_IovecArray
                 // ============================

template <class IOVEC>
class ChannelPool_IovecArray {
    // This is an implementation type of 'ChannelPool' and should not be used
    // by clients of this component.  An 'IovecArray' is an in-core
    // value-semantic type describing a array of iovec objects of templatized
    // type 'IOVEC'.  The parameterized 'IOVEC' type must be either
    // 'bteso::Iovec' or 'btls::Ovec'.  Note that the each 'IOVEC' object in
    // the 'iovecs()' array refers to an array of data, so an 'IovecArray' is
    // an array of arrays, and the total data length of an 'IovecArray' is the
    // sum of the lengths of the 'IOVEC' objects in 'iovecs()'.

    // DATA
    const IOVEC        *d_iovecs;       // array of iovecs
    int                 d_numIovecs;    // number of iovecs
    bsls::Types::Int64  d_totalLength;  // total data length

    public:

    // CREATORS
    ChannelPool_IovecArray(const IOVEC *iovecs, int numIovecs);
        // Create an 'IovecArray' object for the specified array of 'iovecs' of
        // length 'numIovecs'.

    // ~ChannelPool_IovecArray();
        // Destroy this array of iovec objects.  Note that this operation is
        // supplied by the compiler.

    ChannelPool_IovecArray(const ChannelPool_IovecArray& original);
        // Create an iovec array with the same value as the specified original.

    // MANIPULATORS
    ChannelPool_IovecArray& operator=(const ChannelPool_IovecArray& rhs);
        // Assign this iovec array the value of the specified 'rhs', and return
        // a reference to this modifiable iovec array.

    // ACCESSORS
    const IOVEC *iovecs() const;
        // Return the array of 'IOVEC' objects.  Note that each 'IOVEC' object
        // in the returned array refers to an array of data.

    int numIovecs() const;
        // Return the length of the array of 'IOVEC' objects returned by
        // 'iovecs()'.

    bsls::Types::Int64 length() const;
        // Return the total length, in bytes, of the data referred to by
        // 'IOVEC' objects in the 'iovecs()' array of 'IOVEC' objects.
};

// FREE OPERATORS
template <class IOVEC>
inline
bool operator==(const ChannelPool_IovecArray<IOVEC> &lhs,
                const ChannelPool_IovecArray<IOVEC> &rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iovec arrays have the
    // same value, and 'false' otherwise.  Two iovec arrays have the same value
    // if their respective array addresses, array lengths, and total data
    // lengths are the same.

template <class IOVEC>
inline
bool operator!=(const ChannelPool_IovecArray<IOVEC> &lhs,
                const ChannelPool_IovecArray<IOVEC> &rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iovec arrays do not have
    // the same value, and 'false' otherwise.  Two iovec arrays do not have the
    // same value if their respective array addresses, array lengths, or total
    // data lengths are not the name.

                 // =============================
                 // class ChannelPool_MessageUtil
                 // =============================

struct ChannelPool_MessageUtil {
    // This is an implementation type of 'ChannelPool' and should not be used
    // by clients of this component.  The 'ChannelPool_MessageUtil' struct
    // provides a namespace for a set of common functions that can be applied
    // to the different message container types supported by a 'ChannelPool'.

    // PUBLIC CONSTANTS
    enum {
        // This enumeration defines the constant 'e_MAX_IOVEC_SIZE', which is
        // used to indicate the maximum length of an array of iovecs that can
        // be directly read from or written to a socket.  Note that 'IOV_MAX'
        // is defined (on POSIX unix platforms) in "limits.h", and indicates
        // the maximum number of iovecs that can be supplied to 'writev'.

#ifdef BSLS_PLATFORM_OS_UNIX
#ifdef IOV_MAX
#if IOV_MAX > 32
// If too big, this would make 'Channel' really big.
        e_MAX_IOVEC_SIZE = 32


#else
        e_MAX_IOVEC_SIZE = IOV_MAX


#endif
#else
        e_MAX_IOVEC_SIZE = 16


#endif
#else // Windows
        e_MAX_IOVEC_SIZE = 16


#endif
     };

    // CLASS METHODS
    template <class IOVEC>
    static bsls::Types::Int64 length(const ChannelPool_IovecArray<IOVEC>& msg);
    static bsls::Types::Int64 length(const btlb::Blob&                    msg);
        // Return the length of the specified 'msg'.

    template <class IOVEC>
    static int write(btlso::StreamSocket<btlso::IPv4Address> *socket,
                     btls::Iovec                             *temp,
                     const ChannelPool_IovecArray<IOVEC>&     msg);
    static int write(btlso::StreamSocket<btlso::IPv4Address> *socket,
                     btls::Iovec                             *temp,
                     const btlb::Blob&                        msg);
        // Write, to the specified 'socket', the buffers in the specified
        // 'msg', up to 'e_MAX_IOVEC_SIZE' buffers, using the specified 'temp'
        // array of iovec objects as a (temporary) intermediary for
        // 'StreamSocket::writev' (if required).  Return the return value from
        // 'StreamSocket::writev'.

    static int loadIovec(btls::Iovec *dest, const btlb::Blob& msg);
        // Load into the specified 'dest' iovec the data buffers from the
        // specified 'msg', up to 'e_MAX_IOVEC_SIZE' buffers.  Return the
        // number of buffers loaded into 'dest'.

    template <class IOVEC>
    static int loadBlob(btlb::Blob                           *dest,
                        const ChannelPool_IovecArray<IOVEC>&  msg,
                        int                                   msgOffset);
    static int loadBlob(btlb::Blob                           *dest,
                        const btlb::Blob&                     msg,
                        int                                   msgOffset);
        // Load into the specified 'dest' the data in the specified 'msg'
        // starting at the specified 'msgOffset'.  For performance reasons the
        // first blob buffer added to 'dest' may start *before* the 'msgOffset'
        // byte in 'msg'; return the offset into the first blob buffer in
        // 'dest' of the corresponding 'msgOffset' byte in 'msg'.  The behavior
        // is undefined unless 'dest' is empty.

    template <class IOVEC>
    static void appendToBlob(btlb::Blob                           *dest,
                             const ChannelPool_IovecArray<IOVEC>&  msg);
    static void appendToBlob(btlb::Blob                           *dest,
                             const btlb::Blob&                     msg);
        // Append, to the specified 'dest' blob, the data buffers in the
        // specified 'msg'.  The behavior is undefined unless the last buffer
        // in 'dest' is trimmed.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                       //------------------
                       // class ChannelPool
                       //------------------

// PRIVATE ACCESSORS
inline
int ChannelPool::findChannelHandle(ChannelHandle *handle, int channelId) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                       0 == d_channels.find(channelId, handle))
     && BSLS_PERFORMANCEHINT_PREDICT_LIKELY(*handle)) {
        return 0;                                                     // RETURN
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    return 1;
}

// MANIPULATORS
inline
int ChannelPool::write(int channelId, const btlb::Blob& message)
{
    return write(channelId, message, 0x7FFFFFFF);
}

inline
int ChannelPool::connect(const btlso::IPv4Address&   serverAddress,
                         int                         numAttempts,
                         const bsls::TimeInterval&   interval,
                         int                         sourceId,
                         bool                        readEnabledFlag,
                         KeepHalfOpenMode            mode,
                         const btlso::SocketOptions *socketOptions,
                         const btlso::IPv4Address   *localAddress)
{
    return connectImp(serverAddress,
                      numAttempts,
                      interval,
                      sourceId,
                      0,
                      readEnabledFlag,
                      mode,
                      socketOptions,
                      localAddress);
}

inline
int ChannelPool::connect(const char                 *hostname,
                         int                         portNumber,
                         int                         numAttempts,
                         const bsls::TimeInterval&   interval,
                         int                         sourceId,
                         ConnectResolutionMode       resolutionMode,
                         bool                        readEnabledFlag,
                         KeepHalfOpenMode            halfCloseMode,
                         const btlso::SocketOptions *socketOptions,
                         const btlso::IPv4Address   *localAddress)
{
    return connectImp(hostname,
                      portNumber,
                      numAttempts,
                      interval,
                      sourceId,
                      0,
                      resolutionMode,
                      readEnabledFlag,
                      halfCloseMode,
                      socketOptions,
                      localAddress);
}

inline
btlb::BlobBufferFactory *ChannelPool::incomingBlobBufferFactory()
{
    return d_readBlobFactory.ptr();
}

inline
btlb::BlobBufferFactory *ChannelPool::outboundBlobBufferFactory()
{
    return d_writeBlobFactory.ptr();
}

// ACCESSORS
inline
int ChannelPool::busyMetrics() const
{
    return bsls::AtomicOperations::getInt(&d_capacity);
}

inline
int ChannelPool::numChannels() const
{
    return d_channels.length();
}

inline
int ChannelPool::numThreads() const
{
    return d_startFlag ? static_cast<int>(d_managers.size()) : 0;
}

                 // ----------------------------
                 // class ChannelPool_IovecArray
                 // ----------------------------

// CREATORS
template <class IOVEC>
inline
ChannelPool_IovecArray<IOVEC>::ChannelPool_IovecArray(const IOVEC *iovecs,
                                                      int          numIovecs)
: d_iovecs(iovecs)
, d_numIovecs(numIovecs)
, d_totalLength(btls::IovecUtil::length(iovecs, numIovecs))
{
}

template <class IOVEC>
inline
ChannelPool_IovecArray<IOVEC>::ChannelPool_IovecArray(
                                        const ChannelPool_IovecArray& original)
: d_iovecs(original.d_iovecs)
, d_numIovecs(original.d_numIovecs)
, d_totalLength(original.d_totalLength)
{
}

// MANIPULATORS
template <class IOVEC>
inline
ChannelPool_IovecArray<IOVEC>&
ChannelPool_IovecArray<IOVEC>::operator=(const ChannelPool_IovecArray& rhs)
{
    d_iovecs      = rhs.d_iovecs;
    d_numIovecs   = rhs.d_numIovecs;
    d_totalLength = rhs.d_totalLength;
    return *this;
}

// ACCESSORS
template <class IOVEC>
inline
bsls::Types::Int64
ChannelPool_IovecArray<IOVEC>::length() const
{
    return d_totalLength;
}

template <class IOVEC>
inline
const IOVEC *
ChannelPool_IovecArray<IOVEC>::iovecs() const
{
    return d_iovecs;
}

template <class IOVEC>
inline
int ChannelPool_IovecArray<IOVEC>::numIovecs() const
{
    return d_numIovecs;
}

                 // -----------------------------
                 // class ChannelPool_MessageUtil
                 // -----------------------------

// CLASS METHODS
inline
bsls::Types::Int64
ChannelPool_MessageUtil::length(const btlb::Blob& msg)
{
    return msg.length();
}

template <class IOVEC>
inline
bsls::Types::Int64
ChannelPool_MessageUtil::length(const ChannelPool_IovecArray<IOVEC>& msg)
{
    return msg.length();
}

template <class IOVEC>
inline
int ChannelPool_MessageUtil::write(
                               btlso::StreamSocket<btlso::IPv4Address> *socket,
                               btls::Iovec                             *,
                               const ChannelPool_IovecArray<IOVEC>&     msg)
{
    int minNumVecs = msg.numIovecs();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(minNumVecs > e_MAX_IOVEC_SIZE)) {
        minNumVecs = e_MAX_IOVEC_SIZE;
    }

    return socket->writev(msg.iovecs(), minNumVecs);
}

inline
int ChannelPool_MessageUtil::write(
                               btlso::StreamSocket<btlso::IPv4Address> *socket,
                               btls::Iovec                             *temp,
                               const btlb::Blob&                        msg)
{
    int numVecs = loadIovec(temp, msg);
    BSLS_ASSERT(0 < numVecs);
    return socket->writev(temp, numVecs);
}

template <class IOVEC>
inline
int ChannelPool_MessageUtil::loadBlob(
                               btlb::Blob                           *dest,
                               const ChannelPool_IovecArray<IOVEC>&  msg,
                               int                                   msgOffset)
{
    btls::IovecUtil::appendToBlob(dest,
                                  msg.iovecs(),
                                  msg.numIovecs(),
                                  msgOffset);

    BSLS_ASSERT(dest->length() == msg.length() - msgOffset);
    return 0;
}

template <class IOVEC>
inline
void ChannelPool_MessageUtil::appendToBlob(
                                    btlb::Blob                           *dest,
                                    const ChannelPool_IovecArray<IOVEC>&  msg)
{
    btls::IovecUtil::appendToBlob(dest, msg.iovecs(), msg.numIovecs());
}

}  // close package namespace

// FREE OPERATORS
template <class IOVEC>
inline
bool btlmt::operator==(const ChannelPool_IovecArray<IOVEC> &lhs,
                       const ChannelPool_IovecArray<IOVEC> &rhs)
{
    return lhs.iovecs()    == rhs.iovecs()
        && lhs.numIovecs() == rhs.numIovecs()
        && lhs.length()    == rhs.length();
}

template <class IOVEC>
inline
bool btlmt::operator!=(const ChannelPool_IovecArray<IOVEC> &lhs,
                       const ChannelPool_IovecArray<IOVEC> &rhs)
{
    return !(lhs == rhs);
}

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
