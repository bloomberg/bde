// btesos_tcpcbacceptor.h   -*-C++-*-
#ifndef INCLUDED_BTESOS_TCPCBACCEPTOR
#define INCLUDED_BTESOS_TCPCBACCEPTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a non-blocking acceptor of TCP connections.
//
//@CLASSES:
//  btesos_TcpCbAcceptor: non-blocking channel allocator
//
//@SEE_ALSO: btesos_tcptimedcbchannel bteso_tcptimereventmanager
//           btesos_tcpcbconnector btesos_tcpcbchannel
//           bteso_socketoptutil btesos_streamsocketfactory
//
//@AUTHOR: Andrei Basov (abasov)
//
//@DESCRIPTION: This component provides a non-blocking single-port acceptor of
// TCP connections with the capability that adheres to the
// 'btesc_CbChannelAllocator' protocol.  Both timed and non-timed (callback)
// channels can be allocated in a non-timed fashion as indicated by the
// following table:
//
// Allocations follow the asynchronous (callback-based) nature of the 'btesc'
// protocol; the callbacks are invoked when an allocation succeeds or an error
// occurs.  The acceptor uses the user-installed socket event manager in order
// to monitor (in a non-blocking fashion) the listening socket for incoming
// connection requests.
//
// The acceptor has the flexibility of opening and closing a listening socket
// with no effect on any existing channels managed by this object.  The
// enqueued (i.e., not-yet-completed) allocation requests will, however, fail
// (without invalidating the allocator) and the associated callbacks will be
// invoked when the listening port is closed.
//
///Thread-safety
///-------------
// The acceptor is *thread* *safe*, meaning that any operation can be called on
// *distinct instances* from different threads without any side-effects (which,
// generally speaking, means that there is no 'static' data), but not *thread*
// *enabled* (i.e., two threads cannot safely call methods on the *same*
// *instance* without external synchronization).  This acceptor is not
// *async-safe*, meaning that one or more functions cannot be invoked safely
// from a signal handler.  Note that the thread safety of the acceptor is
// subject to the thread safety of the supplied socket event manager.  If
// distinct instances of acceptor use the *same* instance of a socket event
// manager, both acceptors are thread-safe if and only if that socket event
// manager is *THREAD* *ENABLED*.
//
///Usage
///-----
// The following usage example shows a possible implementation of a multi-user
// echo server.  An echo server accepts connections and, for every connection,
// sends any received data back to the client (until the connection is
// terminated).  This server requires that data is read from an accepted
// connection within a certain time interval or else the connection is dropped
// on timeout.  The echo server is implemented as a separate class
// ('my_EchoServer') that owns the timed callback acceptor.  Various
// configuration parameters such as the timeout values, the queue size and
// input buffer size, and the default port number (as mandated by RFC 862) are
// constants within this class.
//..
//  class my_EchoServer {
//      // This class implements a simple multi-user echo server as
//      // specified by the RFC 862.
//      enum {
//         READ_SIZE = 10,  // The number of bytes to be read can be changed,
//                          // but a larger 'READ_SIZE' will require the
//                          // client to input more data to be echoed.
//         DEFAULT_PORT_NUMBER = 1234,   // As specified by the RFC 862
//         QUEUE_SIZE = 16
//      };
//      btesos_TcpTimedCbAcceptor   d_allocator;
//      bdet_TimeInterval           d_acceptTimeout;
//      bdet_TimeInterval           d_readTimeout;
//      bdet_TimeInterval           d_writeTimeout;
//
//      bdef_Function<void (*)(btesc_TimedCbChannel*, int)>
//                                      d_allocateFunctor;
//                                      // Cached callback functor.
//    private:
//      // Callbacks
//      void allocateCb(btesc_TimedCbChannel *channel,
//                      int                   status);
//          // Invoked from the socket event manager when a connection is
//          // allocated (i.e., accepted) or an error occurs when allocating.
//          // [...]
//
//      void bufferedReadCb(const char           *buffer,
//                          int                   status,
//                          int                   asyncStatus,
//                          btesc_TimedCbChannel *channel);
//          // Invoked from the socket event manager when data is read from a
//          // channel.  [...]
//
//      void writeCb(int                   status,
//                   int                   asyncStatus,
//                   btesc_TimedCbChannel *channel,
//                   int                   numBytes);
//          // Invoked from the socket event manager when data is written
//          // into a channel.  [...]
//
//    private:
//      my_EchoServer(const my_EchoServer&);    // Not implemented.
//      my_EchoServer&
//          operator=(const my_EchoServer&);    // Not implemented.
//    public:
//      // CREATORS
//      my_EchoServer(bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
//                    bteso_TimerEventManager                      *manager);
//          // Create an echo server that uses the specified stream socket
//          // 'factory' for the system sockets and the specified socket event
//          // 'manager' to multiplex the events on these sockets.  The
//          // behavior is undefined if either 'factory' or 'manager' is 0.
//
//     ~my_EchoServer();
//         // Destroy this server.  The behavior is undefined unless the server
//         // is shut down properly (i.e., via 'close').
//
//     // MANIPULATORS
//     int open(int portNumber = DEFAULT_PORT_NUMBER);
//         // Establish a listening socket on the specified 'portNumber';
//         // return 0 on success, and a non-zero value otherwise.  The
//         // behavior is undefined unless 0 <= portNumber and the listening
//         // port is not currently open.
//
//     int close();
//         // Close the listening socket; return 0 on success and a non-zero
//         // value otherwise.  The behavior is undefined unless the listening
//         // socket is currently open.
// };
//..
// The implementation of the public methods of 'my_EchoServer' is trivial.  For
// the constructor, the socket factory and socket event manager are passed to
// the acceptor, and the allocate callback and (three) event timeouts are
// initialized.  The 'open' and 'close' methods just invoke appropriate methods
// of the acceptor.  The destructor ensures that the state of the acceptor is
// valid:
//..
//  my_EchoServer::my_EchoServer(
//          bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
//          bteso_EventManager                           *manager)
//  : d_allocator(factory, manager)
//  , d_acceptTimeout(120, 0)
//  , d_readTimeout(5, 0)
//  , d_writeTimeout(5.0)
//  {
//      assert(factory); assert(manager);
//      d_allocateFunctor = bdef_MemFnUtil::memFn(&allocateCb, this);
//  }
//
//  my_EchoServer::~my_EchoServer() {
//  }
//
//  int my_EchoServer::open(int portNumber) {
//      bteso_IPv4Address serverAddress;
//      serverAddress.setPortNumber(portNumber);
//
//      if (d_allocator.open(serverAddress, QUEUE_SIZE)) {
//          return -1;
//      }
//      // Set reuse address socket option on the listening socket.
//      if (d_allocator.setOption(bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
//                                bteso_SocketOptUtil::BTESO_REUSEADDRESS,
//                                1))
//      {
//          d_allocator.close();
//          return -2;
//      }
//      if (d_allocator.timedAllocateTimed(d_allocateFunctor,
//                 bdetu_SystemTime::now() + d_acceptTimeout))
//      {
//          // Failed to enqueue a request -- the allocator is invalid.
//          d_allocator.close();
//          return -3;
//      }
//      return 0;
//  }
//
//  int my_EchoServer::close() {
//      return d_allocator.close();
//  }
//..
// All the work of accepting connections and reading/writing the data is done
// in the (private) callback methods of 'my_EchoServer'.  When the connection
// is established and the 'allocateCb' method is invoked, the buffered read
// with timeout is initiated on the channel and another allocation request is
// enqueued.  If any error occurs while allocating, the acceptor is shut down.
// If any error, including timeout, occurs while reading or writing data, the
// channel is shut down.  Note that the allocation functor is cached to improve
// performance:
//..
//  void my_EchoServer::allocateCb(btesc_TimedCbChannel *channel, int status) {
//      if (channel) {
//          // Accepted a connection;  issue a buffered read request.
//          bdef_Function<void (*)(const char *, int, int)> callback(
//                  bdef_BindUtil::bind(&bufferedReadCb,
//                                      this,
//                                      _1, _2, _3
//                                      channel));
//          if (channel->timedBufferedRead(READ_SIZE,
//                  bdetu_SystemTime::now() + d_readTimeout, callback)) {
//              bsl::cout << "Failed to enqueue read request." << bsl::endl;
//              d_allocator.deallocate(channel);
//          }
//          // Re-register allocate callback functor.
//          if (d_allocator.timedAllocateTimed(d_allocateFunctor,
//                  bdetu_SystemTime::now() + d_acceptTimeout)) {
//              d_allocator.close();
//          }
//          return;
//      }
//      else {  // !channel
//          assert(status <= 0);
//          if (0 == status) {
//              bsl::cout << "Timed out while accepting a connection."
//                        << bsl::endl;
//              // Re-register the functor.
//              if (d_allocator.timedAllocateTimed(d_allocateFunctor,
//                      bdetu_SystemTime::now() + d_acceptTimeout)) {
//                d_allocator.close();
//              }
//          }
//          else {
//              // Hard-error accepting a connection, invalidate the allocator.
//              bsl::cout << "Hard error while accepting a connection."
//                        << bsl::endl;
//              d_allocator.invalidate();
//              d_allocator.close();
//          }
//      }
//  }
//
//  void my_EchoServer::bufferedReadCb(const char           *buffer,
//                                     int                   status,
//                                     int                   asyncStatus,
//                                     btesc_TimedCbChannel *channel)
//  {
//      assert(channel);
//      if (status > 0) {
//          bdef_Function<void (*)(int, int)> callback(
//                  bdef_BindUtil::bind(&writeCb,
//                                      this,
//                                      _1, _2
//                                      channel,
//                                      status));
//          if (channel->timedBufferedWrite(buffer, status,
//                  bdetu_SystemTime::now() + d_writeTimeout, callback)) {
//              bsl::cout << "Failed to enqueue write request." << bsl::endl;
//              d_allocator.deallocate(channel);
//              return;
//          }
//
//          // Re-register read request
//          bdef_Function<void (*)(const char *, int, int)> readCallback(
//                  bdef_BindUtil::bind(&bufferedReadCb,
//                                      this,
//                                      _1, _2, _3,
//                                      channel));
//
//          if (channel->timedBufferedRead(READ_SIZE,
//                  bdetu_SystemTime::now() + d_readTimeout, readCallback)) {
//              bsl::cout << "Failed to enqueue read request." << bsl::endl;
//              d_allocator.deallocate(channel);
//          }
//      }
//      else {
//          // Either time out or an error on the channel
//          bsl::cout << "Failed to read data." << bsl::endl;
//          d_allocator.deallocate(channel);
//      }
//  }
//
//  void my_EchoServer::writeCb(int                  status,
//                              int                  asyncStatus,
//                              btesc_TimedCbChannel *channel,
//                              int                   numBytes)
//  {
//      if (status != numBytes) {
//          bsl::cout << "Failed to send data." << bsl::endl;
//          channel->invalidate();
//          d_allocator.deallocate(channel);
//      }
//  }
//..
// Finally, we define the 'main' function, which will create the concrete
// socket factory and concrete socket event manager, create 'my_EchoServer' as
// required, and go into "infinite" loop dispatching registered callbacks:
//..
// int main() {
//     bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
//     bteso_TcpTimerEventManager                    manager;
//     my_EchoServer echoServer(&factory, &manager);
//
//     if (echoServer.open()) {
//         bsl::cout << "Can't open listening socket." << bsl::endl;
//         return -1;
//     }
//
//     while(manager.dispatch(bteso_TimerEventManager::NON_INTERRUPTIBLE)) {
//         // Do nothing
//     }
//
//     return 0;
// }
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESC_CBCHANNELALLOCATOR
#include <btesc_cbchannelallocator.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSL_DEQUE
#include <bsl_deque.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

template<class ADDRESS> class bteso_StreamSocketFactory;
template<class ADDRESS> class bteso_StreamSocket;

class bteso_TimerEventManager;

class bslma_Allocator;

class btesos_TcpCbAcceptor_Reg; // component-local class declaration

                        // ==========================
                        // class btesos_TcpCbAcceptor
                        // ==========================

class btesos_TcpCbAcceptor : public btesc_CbChannelAllocator {
    // This class implements a 'btesc'-style callback-based channel allocator
    // for a single server-side (i.e., listening) TCP/IPv4 socket.  The
    // allocation requests' callbacks are queued and invoked as appropriate.
    // The status supplied to the callback is negative for failure and positive
    // for a (restartable) interruption on an underlying system call.  A status
    // value of -1 indicates an allocation attempt on an uninitialized acceptor
    // (i.e., no listening socket is established), and a status value of -2
    // indicates failure due to the listening socket being closed.  The
    // acceptor provides a "delayed open" (where a listening socket is
    // established at some point after construction), and allows the listening
    // socket to be closed (and opened again) with no effect on the state of
    // any other channel currently managed by this acceptor.

    bdema_Pool          d_callbackPool;    // memory pool for registrations
    bdema_Pool          d_channelPool;     // memory pool for channels

    bsl::deque<btesos_TcpCbAcceptor_Reg *>
                        d_callbacks;       // registered callbacks

    bsl::vector<btesc_CbChannel*>
                        d_channels;        // managed channels

    bteso_TimerEventManager
                       *d_manager_p;

    bteso_StreamSocketFactory<bteso_IPv4Address>
                       *d_factory_p;       // factory used to supply sockets

    bteso_StreamSocket<bteso_IPv4Address>
                       *d_serverSocket_p;  // listening socket

    bteso_IPv4Address   d_serverAddress;   // address of listening socket

    int                 d_isInvalidFlag;   // set if acceptor is invalid

    bdef_Function<void (*)()>
                        d_acceptFunctor;   // cached callbacks

    btesos_TcpCbAcceptor_Reg
                       *d_currentRequest_p;// address of the current request

    bslma_Allocator    *d_allocator_p;

  private:
    // Callbacks for socket event manager

    void acceptCb();
        // Invoked from socket event manager when an event occurs on the
        // listening socket.

    void deallocateCb(btesc_CbChannel *channel);
        // Deallocate resources used by the specified 'channel'.  Invoked from
        // timer event manager.

  private:
    btesos_TcpCbAcceptor(const btesos_TcpCbAcceptor&); // not impl.
    btesos_TcpCbAcceptor&
        operator=(const btesos_TcpCbAcceptor&);        // not impl.

  public:
    // CREATORS
    btesos_TcpCbAcceptor(
            bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
            bteso_TimerEventManager                      *manager,
            bslma_Allocator                              *basicAllocator = 0);
        // Create a callback acceptor that uses the specified 'factory' (to
        // create stream sockets) and the specified 'manager' (to monitor for
        // incoming connections).  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined if either
        // 'factory' or 'manager' is 0.  Note that the acceptor is created in a
        // valid state (as reported by the 'isInvalid' method) and subsequent
        // allocation requests will be enqueued successfully, but that such
        // allocations will fail with a status of -1 until the listening socket
        // is created (using 'open').

    btesos_TcpCbAcceptor(
            bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
            bteso_TimerEventManager                      *manager,
            int                                           numElements,
            bslma_Allocator                              *basicAllocator = 0);
        // Create a callback acceptor, with enough internal capacity to
        // accommodate up to the specified 'numElements' channels without
        // reallocation, that uses the specified 'factory' to create stream
        // sockets and the specified 'manager' to monitor for incoming
        // connections.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  'delete' are used.  The behavior is undefined if
        // either 'factory' or 'manager' is 0 and unless 0 < numElements.  Note
        // that the acceptor is created in a valid state (as reported by the
        // 'isInvalid' method) and that subsequent allocation requests will be
        // enqueued successfully, but the allocations will fail with a status
        // of -1 until the listening socket is created (using 'open').

    ~btesos_TcpCbAcceptor();
        // Invalidate this allocator, close the listening socket, invoke all
        // pending callbacks, destroy all managed channels, and destroy this
        // allocator.

    // MANIPULATORS
    virtual int allocate(const Callback& callback,
                         int             flags = 0);
        // Initiate a non-blocking operation to allocate a callback channel;
        // execute the specified 'callback' functor after the allocation
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt the allocation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'callback' will not be
        // invoked).
        //
        // When invoked, 'callback' is passed the (possibly null) address of a
        // callback channel and an integer "status".  If that address is not 0,
        // the allocation succeeded and status has no meaning; a non-null
        // channel address will remain valid until deallocated explicitly (see
        // 'deallocate').  If the address is 0, a positive status indicates an
        // interruption due to an asynchronous event; subsequent allocation
        // attempts may succeed.  A status of -1 implies that the allocation
        // operation was "canceled" (synchronously) by the caller (see
        // 'cancelAll') and, often, may be retried successfully.  A status less
        // than -1 indicates a more persistent error, but not necessarily a
        // permanent one; the allocator itself may still be valid (see
        // 'isInvalid').  The behavior is undefined unless 'callback' is valid.

    int allocateTimed(const TimedCallback& timedCallback,
                      int                  flags = 0);
        // Initiate a non-blocking operation to allocate a timed callback
        // channel; execute the specified 'timedCallback' functor after the
        // allocation operation terminates.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt the allocation; by
        // default, such events are ignored.  Return 0 on successful
        // initiation, and a non-zero value otherwise (in which case
        // 'timedCallback' will not be invoked).
        //
        // When invoked, 'timedCallback' is passed the (possibly null) address
        // of a timed callback channel and an integer "status".  If that
        // address is not 0, the allocation succeeded and status has no
        // meaning; a non-null channel address will remain valid until
        // deallocated explicitly (see 'deallocate').  If the address is 0, a
        // positive status indicates an interruption due to an asynchronous
        // event; subsequent allocation attempts may succeed.  A status of -1
        // implies that the allocation operation was "canceled" (synchronously)
        // by the caller (see 'cancelAll') and, often, may be retried
        // successfully.  A status less than -1 indicates a more persistent
        // error, but not necessarily a permanent one; the allocator itself may
        // still be valid (see 'isInvalid').  The behavior is undefined unless
        // 'callback' is valid.

    virtual void cancelAll();
        // Immediately cancel all pending operations on this allocator,
        // invoking each registered allocation callback in turn.  Each callback
        // will be invoked with a null channel and a status of -1.  This method
        // may be invoked successfully on an invalid allocator; however,
        // calling the method does not invalidate the allocator.  Note that
        // calling 'cancelAll' from a callback that has itself been canceled
        // simply extends the set of canceled operations to include any new
        // ones initiated since the previous 'cancelAll' was invoked.

    int close();
        // Close the listening socket and invoke all enqueued callbacks with a
        // status of -2.  Return 0 on success, and a non-zero value otherwise.
        // A listening socket yields a non-zero server address (see 'address').
        // The behavior is undefined unless the listening socket is
        // successfully established.  Note that closing a listening port has no
        // effect on any other channel managed by this allocator.

    virtual void deallocate(btesc_CbChannel *channel);
        // Terminate all operations on the specified 'channel', invoke each
        // pending callback with the appropriate status, and reclaim all
        // afforded channel services.  The behavior is undefined unless
        // 'channel' is currently allocated from this allocator, (i.e., was
        // previously obtained from this instance and has not subsequently been
        // deallocated).  Note that this method can never block.

    virtual void invalidate();
        // Place this allocator in a permanently invalid state.  No subsequent
        // allocations will succeed.  Note that invalidating this allocator has
        // no effect on the state of any channel managed by it nor on the
        // listening socket, which, if established, must be closed explicitly
        // (using the 'close' method).

    int open(const bteso_IPv4Address& endpoint,
             int                      queueSize,
             int                      reuseAddress = 1);
        // Establish a listening socket having the specified 'queueSize'
        // maximum number of pending connections on the specified 'endpoint'.
        // Optionally specify a 'reuseAddress' value to be used for setting
        // 'REUSEADDRESS' option.  If 'reuseAddress' is not specified, 1 (i.e.,
        // if the REUSEADDRESS option is enabled) is used.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless 0 < queueSize and the listening socket is closed.

    int setOption(int level,
                  int option,
                  int value);
        // Set the specified socket 'option' having the specified 'level' on
        // the listening socket to the specified 'value'.  Return 0 on success
        // and a non-zero value otherwise.  (The list of commonly supported
        // options is available in 'bteso_socketoptutil'.)  The behavior is
        // undefined unless the listening socket is established.  Note that all
        // sockets allocated from this acceptor will inherit the options'
        // values set on the listening socket.

    // ACCESSORS
    const bteso_IPv4Address& address() const;
        // Return the (fully qualified) address of the listening socket, or an
        // invalid address '(ANY_ADDRESS, ANY_PORT)' if the server is not
        // established (see 'bteso_ipv4address').

    int getOption(int *result,
                  int level,
                  int option) const;
        // Load into the specified 'result' the current value of the specified
        // option of the specified 'level' set on the listening socket.  Return
        // 0 on success and a non-zero value otherwise.  The list of commonly
        // supported options (and levels) is enumerated in
        // 'bteso_socketoptutil'.  The behavior is undefined if 'result' is 0
        // and unless the listening socket has been established.

    virtual int isInvalid() const;
        // Return 1 if this channel allocator is *permanently* invalid (i.e.,
        // no subsequent allocation requests will succeed).

    int numChannels() const;
        // Return the number of channels currently allocated from this
        // acceptor.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

inline
const bteso_IPv4Address& btesos_TcpCbAcceptor::address() const {
    return d_serverAddress;
}

inline
int btesos_TcpCbAcceptor::numChannels() const {
    return d_channels.size();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
