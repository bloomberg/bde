// btlsos_tcpcbconnector.h                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSOS_TCPCBCONNECTOR
#define INCLUDED_BTLSOS_TCPCBCONNECTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a non-blocking connector to TCP servers.
//
//@CLASSES:
//  btlsos::TcpCbConnector: non-blocking channel allocator
//
//@SEE_ALSO: btlsos_tcptimedcbchannel btlso_tcptimereventmanager
//           btlsos_tcpcbacceptor btlsos_tcpcbchannel
//           btlso_socketoptutil btlso_inetstreamsocketfactory
//
//@DESCRIPTION: This component provides a non-blocking single-port connector of
// TCP connections with timeout capability, 'btlsos::TcpCbConnector', that
// adheres to the 'btlsc::CbChannelAllocator' protocol.  Both timed and
// non-timed (callback) channels can be allocated in a non-timed fashion as
// indicated by the following table:
//
// Allocations follow the asynchronous (callback-based) nature of the 'btesc'
// protocol; the callbacks are invoked when an allocation succeeds or an error
// occurs.  The connector uses a user-installed socket event manager in order
// to monitor (in a non-blocking fashion) the connecting socket for completion
// of a connection.
//
// The connector has the flexibility of changing the address of the peer server
// at run-time with no effect on the state of managed channels.  The enqueued
// (i.e., not-yet-completed) allocation requests will, however, fail (without
// invalidating the allocator) and the associated callbacks will be invoked
// when the peer address is changed.
//
///Thread Safety
///-------------
// The connector is *thread* *safe*, meaning that any operation can be called
// on *distinct* *instances* from different threads without any side-effects
// (which, generally speaking, means that there is no 'static' data), but not
// *thread* *enabled* (i.e., two threads cannot safely call methods on the
// *same* *instance* without external synchronization).  This connector is not
// *async-safe*, meaning that one or more functions cannot be invoked safely
// from a signal handler.  Note that the thread safety of the connector is
// subject to the thread safety of the supplied socket event manager.  If
// distinct instances of this connector use the *same* instance of a socket
// event manager, both connectors are thread-safe if and only if that socket
// event manager is *THREAD* *ENABLED*.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: An Example Echo Client
///- - - - - - - - - - - - - - - - -
// The following usage example shows a possible implementation of an echo
// client.  An echo client sends connection requests to an echo server, and for
// every connection, sends and receives data.  Various configuration parameters
// such as the timeout values, the number of connections, the port number, and
// the number of messages to be sent should be passed into a client object.
//..
//  class my_EchoClient {
//      enum {
//          k_BUFFER_SIZE = 100
//      };
//
//      btlsos::TcpCbConnector d_allocator;
//      bsls::TimeInterval     d_readTimeout;
//      bsls::TimeInterval     d_writeTimeout;
//      char                   d_controlBuffer[k_BUFFER_SIZE];
//      int                    d_numConnections;
//      int                    d_maxConnections;
//      int                    d_numMessages;
//
//      bsl::function<void(btlsc::TimedCbChannel*, int)>
//                             d_allocateFunctor;
//
//      void allocateCb(btlsc::TimedCbChannel *channel, int status);
//          // Invoked by the socket event manager when a connection is
//          // accepted.
//
//      void bufferedReadCb(const char            *buffer,
//                          int                    status,
//                          int                    asyncStatus,
//                          btlsc::TimedCbChannel *channel,
//                          int                    sequence);
//
//      void writeCb(int                    status,
//                   int                    asyncStatus,
//                   btlsc::TimedCbChannel *channel,
//                   int                    sequence);
//
//    private:
//      // Not implemented:
//      my_EchoClient(const my_EchoClient&);
//      my_EchoClient& operator=(const my_EchoClient&);
//
//    public:
//      // CREATORS
//      my_EchoClient(
//         btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
//         btlso::TimerEventManager                       *manager,
//         int                                             maxConnections,
//         int                                             numMessages,
//         bslma::Allocator                               *basicAllocator = 0);
//
//      ~my_EchoClient();
//
//      //  MANIPULATORS
//      int setPeer(const btlso::IPv4Address& address);
//          // Set the address of the echo server to the specified 'address',
//          // and start sending messages to the server.
//  };
//
//  // CREATORS
//  my_EchoClient::my_EchoClient(
//              btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
//              btlso::TimerEventManager                       *manager,
//              int                                             maxConnections,
//              int                                             numMessages,
//              bslma::Allocator                               *basicAllocator)
//  : d_allocator(factory, manager, basicAllocator)
//  , d_readTimeout(20.0)
//  , d_writeTimeout(5,0)
//  , d_numConnections(0)
//  , d_maxConnections(maxConnections)
//  , d_numMessages(numMessages)
//  {
//      assert(factory);
//      assert(manager);
//      d_allocateFunctor = bdlf::MemFnUtil::memFn(&my_EchoClient::allocateCb,
//                                                 this);
//      memset(d_controlBuffer, 'A', k_BUFFER_SIZE);
//  }
//
//  my_EchoClient::~my_EchoClient() {
//  }
//..
// All the work of accepting connections and reading/writing the data is done
// in the (private) callback methods of 'my_EchoClient'.  When the connection
// is established and the 'allocateCb' method is invoked, the buffered write
// with timeout is initiated on the channel and another allocation request is
// enqueued.  If any error occurs while allocating, the acceptor is shut down.
// If any error, including timeout, occurs while reading or writing data, the
// channel is shut down.  Note that the allocation functor is cached to improve
// performance:
//..
//  // MANIPULATORS
//  void my_EchoClient::allocateCb(btlsc::TimedCbChannel *channel, int status)
//  {
//      if (channel) {
//          // Connected to a server.  Issue a buffered write request.
//          bsl::function<void(int, int)> callback(
//                       bdlf::BindUtil::bind(
//                              bdlf::MemFnUtil::memFn(&my_EchoClient::writeCb,
//                                                     this),
//                              _1,
//                              _2,
//                              channel,
//                              0));
//          if (channel->timedBufferedWrite(
//                                   d_controlBuffer,
//                                   k_BUFFER_SIZE,
//                                   bdlt::CurrentTime::now() + d_writeTimeout,
//                                   callback)) {
//              cout << "Failed to enqueue write request." << endl;
//              assert(channel->isInvalidWrite());
//              d_allocator.deallocate(channel);
//          }
//
//          if (d_maxConnections > ++d_numConnections) {
//              int s = d_allocator.allocateTimed(d_allocateFunctor);
//              assert(0 == s);
//          }
//
//          return;                                                   // RETURN
//      }
//
//      assert(0 >= status);    // Interrupts are not enabled.
//      if (0 == status) {
//          cerr << "Timed out connecting to the server." << endl;
//      }
//      else {
//          // Hard-error accepting a connection, invalidate the allocator.
//          cerr << "Non-recoverable error connecting to the server " << endl;
//          d_allocator.invalidate();
//          return;                                                   // RETURN
//      }
//
//      // In any case, except for hard error on allocator, enqueue another
//      // connect request.
//      if (d_maxConnections > ++d_numConnections) {
//          int s = d_allocator.allocateTimed(d_allocateFunctor);
//          assert(0 == s);
//      }
//  }
//
//  void my_EchoClient::bufferedReadCb(const char            *buffer,
//                                     int                    status,
//                                     int                    asyncStatus,
//                                     btlsc::TimedCbChannel *channel,
//                                     int                    sequence)
//  {
//      assert(channel);
//      if (0 < status) {
//          assert(k_BUFFER_SIZE == status);
//          assert(0 == memcmp(buffer, d_controlBuffer, k_BUFFER_SIZE));
//
//          // If we're not done -- enqueue another request
//          if (sequence < d_numMessages) {
//              bsl::function<void(int, int)> callback(
//                      bdlf::BindUtil::bind(
//                              bdlf::MemFnUtil::memFn(&my_EchoClient::writeCb,
//                                                     this),
//                              _1,
//                              _2,
//                              channel,
//                              sequence + 1));
//              if (channel->timedBufferedWrite(
//                                   d_controlBuffer,
//                                   k_BUFFER_SIZE,
//                                   bdlt::CurrentTime::now() + d_writeTimeout,
//                                   callback))
//                  {
//                      cout << "Failed to enqueue write request." << endl;
//                      assert(channel->isInvalidWrite());
//                      d_allocator.deallocate(channel);
//                  }
//          }
//          else {
//              cout << "Done transferring data on a channel. " << endl;
//              d_allocator.deallocate(channel);
//          }
//      }
//      else if (0 == status) {
//          if (0 > asyncStatus) {
//              cout << "Callback dequeued" << endl;
//          }
//          else {
//              cerr << "Timed out on read" << endl;
//              d_allocator.deallocate(channel);
//          }
//      }
//      else {
//          cerr << "Failed to read data: non-recoverable error on channel."
//               << endl;
//          d_allocator.deallocate(channel);
//      }
//  }
//
//  void my_EchoClient::writeCb(int                    status,
//                              int                    asyncStatus,
//                              btlsc::TimedCbChannel *channel,
//                              int                    sequence)
//  {
//      if (0 < status) {
//          if (status != k_BUFFER_SIZE) {
//              d_allocator.deallocate(channel);
//              assert("Failed to send data to the server" && 0);
//          }
//          else {
//              bsl::function<void(const char *, int, int)> callback(
//                    bdlf::BindUtil::bind(
//                       bdlf::MemFnUtil::memFn(&my_EchoClient::bufferedReadCb,
//                                              this),
//                       _1,
//                       _2,
//                       _3,
//                       channel,
//                       sequence));
//              if (channel->timedBufferedRead(
//                                    k_BUFFER_SIZE,
//                                    bdlt::CurrentTime::now() + d_readTimeout,
//                                    callback)) {
//                  assert(channel->isInvalidRead());
//                  d_allocator.deallocate(channel);
//                  assert("Failed to enqueue read request" && 0);
//              }
//          }
//      }
//      else if (0 == status) {
//          if (0 > asyncStatus) {
//              cout << "Request dequeued." << endl;
//          }
//          else {
//              d_allocator.deallocate(channel);
//              assert("Timed out sending data to the server" && 0);
//          }
//      }
//      else {
//          d_allocator.deallocate(channel);
//          assert("Failed to send data: non-recoverable error on the channel"
//                 && 0);
//      }
//  }
//
//  int my_EchoClient::setPeer(const btlso::IPv4Address& address) {
//      d_allocator.setPeer(address);
//      return d_allocator.allocateTimed(d_allocateFunctor);
//  }
//..
//
///Example 2: Dual Control and Data Channels
///- - - - - - - - - - - - - - - - - - - - -
// The following usage example shows a possible implementation of a client-side
// communication layer where two communication channels are established for a
// server: one is for control messages and another for the data stream.  The
// client is implemented as a separate class that owns the connector and uses a
// socket event manager and (corresponding) stream socket factory installed at
// creation.  Various configuration parameters such as the timeout values, the
// queue size, and input buffer size are constants within this class.
//..
//  class my_DataStream {
//      enum {
//          k_DEFAULT_PORT_NUMBER = 1234,
//          k_QUEUE_SIZE = 16
//      };
//
//      btlsos::TcpCbConnector  d_allocator;
//      bsls::TimeInterval      d_connectTimeout;
//      btlsc::TimedCbChannel  *d_controlChannel;
//      btlsc::TimedCbChannel  *d_dataChannel;
//
//    private:
//      // Callbacks
//      void allocateCb(btlsc::TimedCbChannel  *channel,
//                      int                     status,
//                      btlsc::TimedCbChannel **cachedChannel);
//          // Invoked from the socket event manager when a connection is
//          // allocated (i.e., established) or an error occurs when
//          // allocating.
//
//    private:
//      // Not implemented:
//      my_DataStream(const my_DataStream&);
//      my_DataStream& operator=(const my_DataStream&);
//
//    public:
//      // CREATORS
//      my_DataStream(
//                btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
//                btlso::TimerEventManager                       *manager,
//                const btlso::IPv4Address&                       peerAddress);
//          // Create a controlled data stream that uses the specified stream
//          // socket 'factory' for system sockets, uses the specified socket
//          // event 'manager' to multiplex events on these sockets, and
//          // attempts to connect to the server at the specified
//          // 'peerAddress'.  The behavior is undefined if either 'factory' or
//          // 'manager' is 0.
//
//      ~my_DataStream();
//          // Destroy this server.
//
//      // MANIPULATORS
//      int setUpCallbacks();
//          // Register callbacks as required for establishing communication
//          // channels.  Return 0 on success, and a non-zero value otherwise,
//          // in which case all further registration attempts will fail (and
//          // the object can be only destroyed).
//  };
//..
// The implementation of the public methods of 'my_DataStream' is trivial.  For
// the constructor, the socket factory and socket event manager are passed to
// the connector, and the allocate callback and (three) event timeouts are
// initialized; the peer address is passed to the connector.  The destructor
// verifies that the state of the connector is valid:
//..
//  my_DataStream::my_DataStream(
//                 btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
//                 btlso::TimerEventManager                       *manager,
//                 const btlso::IPv4Address&                       peerAddress)
//      : d_allocator(factory, manager)
//      , d_connectTimeout(120, 0)
//      , d_controlChannel(NULL)
//      , d_dataChannel(NULL)
//  {
//      assert(factory);
//      assert(manager);
//      d_allocator.setPeer(peerAddress);
//  }
//
//  my_DataStream::~my_DataStream() {
//      assert(0 == d_allocator.numChannels());
//  }
//
//  int my_DataStream::setUpCallbacks() {
//      bsl::function<void(btlsc::TimedCbChannel*, int)> callback(
//          bdlf::BindUtil::bind(
//                           bdlf::MemFnUtil::memFn(&my_DataStream::allocateCb,
//                                                  this),
//                           _1,
//                           _2,
//                           &d_controlChannel));
//
//      if (d_allocator.allocateTimed(callback)) {
//          return -1;                                                // RETURN
//      }
//
//      callback = bdlf::BindUtil::bind(
//                           bdlf::MemFnUtil::memFn(&my_DataStream::allocateCb,
//                                                  this),
//                           _1,
//                           _2,
//                           &d_dataChannel);
//
//      return d_allocator.allocateTimed(callback);
//  }
//..
// The allocate callback will cache the newly-allocated channel for future use:
//..
//  void my_DataStream::allocateCb(btlsc::TimedCbChannel  *channel,
//                                 int                     status,
//                                 btlsc::TimedCbChannel **cachedChannel) {
//      assert(cachedChannel);
//
//      if (channel) {
//          *cachedChannel = channel;
//          if (d_controlChannel && d_dataChannel) {
//              // Ready to do data processing ...
//          }
//      }
//      else {
//          cout << "not valid channel: status: " << status << endl;
//      }
//  }
//
//  static void connectCb(btlsc::CbChannel       *channel,
//                        int                     status,
//                        btlsos::TcpCbConnector *connector,
//                        int                    *numConnections,
//                        int                     validChannel,
//                        int                     expStatus,
//                        int                     cancelFlag)
//      // Verify the result of an "ACCEPT" request by comparing against the
//      // expected values: If the specified 'validChannel' is nonzero, a new
//      // 'btlsc::CbChannel' should be established; the specified return
//      // 'status' should be the same as the specified 'expStatus'.  If the
//      // specified 'cancelFlag' is nonzero, invoke the 'cancelAll()' on the
//      // specified 'acceptor' for test.
//  {
//      if (validChannel) {
//          assert(channel);
//      }
//      else {
//          assert(0 == channel);
//      }
//      assert(status == expStatus);
//
//      if (0 == channel) {
//      }
//      else {
//          ++(*numConnections);
//      }
//      if (cancelFlag) {
//          connector->cancelAll();
//      }
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSC_CBCHANNELALLOCATOR
#include <btlsc_cbchannelallocator.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BDLMA_POOL
#include <bdlma_pool.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_DEQUE
#include <bsl_deque.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace btlso { template<class ADDRESS> class StreamSocketFactory; }
namespace btlso { template<class ADDRESS> class StreamSocket; }
namespace btlso { class TimerEventManager; }
namespace btlsos {

class TcpCbConnector_Reg; // component-local class declaration

                           // ====================
                           // class TcpCbConnector
                           // ====================

class TcpCbConnector : public btlsc::CbChannelAllocator {
    // This class implements a 'btesc'-style timed callback-based channel
    // allocator for a single client-side TCP/IPv4 socket.  The allocation
    // requests' callbacks are queued and invoked as appropriate.  The status
    // supplied to a callback is negative for failure, 0 on timeout, and
    // positive for a (restartable) interrupt on an underlying system call.  A
    // status value of -1 indicates an allocation attempt on an uninitialized
    // connector (i.e., an invalid peer address is specified), and a status
    // value of -2 to indicate failure due to changed end-points on
    // not-yet-completed (allocation) request.  This connector provides the
    // ability to change the end-points at run-time without any effect on the
    // state of any channels currently managed by this connector.

    bdlma::Pool        d_callbackPool;    // memory pool for registrations
    bdlma::Pool        d_channelPool;     // memory pool for channels

    bsl::deque<TcpCbConnector_Reg *>
                       d_callbacks;       // registered callbacks

    bsl::vector<btlsc::CbChannel *>       // managed channels
                       d_channels;

    TcpCbConnector_Reg
                     *d_currentRequest_p;// the address of the current request.

    btlso::TimerEventManager              // multiplexer of both socket events
                      *d_manager_p;       // and timers

    btlso::StreamSocketFactory<btlso::IPv4Address>
                      *d_factory_p;       // factory used to supply sockets

    btlso::StreamSocket<btlso::IPv4Address>
                     *d_connectingSocket_p;

    btlso::IPv4Address d_peerAddress;     // address of peer process

    int               d_isInvalidFlag;   // set if connector is invalid

    bsl::function<void()>
                      d_connectFunctor;  // cached callbacks

    bslma::Allocator *d_allocator_p;
  private:
    // Private (implementation) methods

    template <class CALLBACK_TYPE, class CHANNEL>
    int initiateTimedConnection(const CALLBACK_TYPE&      callback,
                                const bsls::TimeInterval& timeout,
                                int                       flags,
                                int                       createRequest);
        // Initiate a non-blocking connection to the peer server, invoke the
        // specified 'callback' if the operation completed immediately (and
        // allocate a channel, if needed), or enqueue the request for a
        // connection completion after the specified 'timeout' absolute time.
        // Return 0 if operation completed, and a non-zero value otherwise.

    template <class CALLBACK_TYPE, class CHANNEL>
    int initiateConnection(const CALLBACK_TYPE& callback,
                           int                  flags,
                           int                  createRequest);
        // Initiate a non-blocking connection to the peer server, invoke the
        // specified 'callback' if operation completed immediately (and
        // allocate a channel, if needed), or enqueue the request for a
        // connection completion.  Return 0 if the operation completed, and a
        // non-zero value otherwise.

    // Callbacks for socket event manager
    void connectCb();
        // Process the connection request at the top of the queue, and set up
        // the callbacks as required in order to process the next request, if
        // any.  Note that this method is invoked from the socket event manager
        // when an event occurs on the connecting socket.

    void deallocateCb(btlsc::CbChannel *channel);
        // Deallocate resources used by the specified 'channel'.  Note that
        // this method is invoked from the timer event manager.

    void timerCb();
        // Process the connection request at the top of the queue, and set up
        // the callbacks as required in order to process the next request, if
        // any.  Note that this method is invoked from the socket event manager
        // when the timer is expired (and the associated request is timed out).

  private:
    // Not implemented:
    TcpCbConnector(const TcpCbConnector&);
    TcpCbConnector& operator=(const TcpCbConnector&);

  public:
    // CREATORS
    TcpCbConnector(
           btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
           btlso::TimerEventManager                       *manager,
           bslma::Allocator                               *basicAllocator = 0);
        // Create a timed callback connector that uses the specified 'factory'
        // (to create stream sockets) and the specified 'manager' (to monitor
        // for incoming connections).  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The behavior is undefined if
        // either 'factory' or 'manager' is 0.  Note that the connector is
        // created in a valid state (as reported by the 'isInvalid' method).

    TcpCbConnector(
           btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
           btlso::TimerEventManager                       *manager,
           int                                             numChannels,
           bslma::Allocator                               *basicAllocator = 0);
        // Create a timed cb connector, with enough internal capacity to
        // allocate up to the specified 'numChannels' without reallocation,
        // that uses the specified 'factory' to create stream sockets, and the
        // specified 'manager' to monitor for incoming connections.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless either 'factory' or
        // 'manager' is non-zero and '0 < numElements'.  Note that the
        // allocator is used.  The behavior is undefined if either 'factory' or
        // 'manager' is 0 and unless 0 < numElements.  Note that the connector
        // is created in a valid state (as reported by the 'isInvalid' method).
    ~TcpCbConnector();
        // Destroy this connector.  The behavior is undefined unless all
        // channels allocated from this connector have been deallocated.

    // MANIPULATORS
    virtual int allocate(const Callback& callback, int flags = 0);
        // Initiate a non-blocking operation to allocate a callback channel;
        // execute the specified 'callback' functor after the allocation
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous events"
        // are permitted to interrupt the allocation; by default, such events
        // are ignored.  Return 0 on successful initiation, and a non-zero
        // value otherwise (in which case 'callback' will not be invoked).
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

    virtual int allocateTimed(const TimedCallback& timedCallback,
                              int                  flags = 0);
        // Initiate a non-blocking operation to allocate a timed callback
        // channel; execute the specified 'timedCallback' functor after the
        // allocation operation terminates.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt the allocation; by default, such
        // events are ignored.  Return 0 on successful initiation, and a
        // non-zero value otherwise (in which case 'timedCallback' will not be
        // invoked).
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

    virtual void deallocate(btlsc::CbChannel *channel);
        // Terminate all operations on the specified 'channel', invoke each
        // pending callback with the appropriate status, and reclaim all
        // afforded channel services.  The behavior is undefined unless
        // 'channel' is currently allocated from this allocator, (i.e., was
        // previously obtained from this instance and has not been subsequently
        // deallocated).  Note that this method can never block.

    virtual void invalidate();
        // Place this allocator in a permanently invalid state.  No subsequent
        // allocations will succeed.  Note that invalidating this allocator has
        // no effect on the state of any channel managed by it.  Also note that
        // if a connection attempt is in progress, it will *NOT* be terminated.

    void setPeer(const btlso::IPv4Address& endpoint);
        // Set the address of the peer server that this connector is associated
        // with to the specified 'endpoint'.

    // ACCESSORS
    const btlso::IPv4Address& peer() const;
        // Return the (fully qualified) address of the peer server that is
        // associated with this connector, or an invalid address of
        // ('ANY_ADDRESS', 'ANY_PORT') if no such server exists (see
        // 'bteso::IPv4address').

    virtual int isInvalid() const;
        // Return 1 if this channel allocator is *permanently* invalid (i.e.,
        // no subsequent allocation requests will succeed).

    int numChannels() const;
        // Return the number of channels currently allocated from this
        // connector.
};

// ----------------------------------------------------------------------------
//                             INLINE DEFINITIONS
// ----------------------------------------------------------------------------

inline
void TcpCbConnector::setPeer(const btlso::IPv4Address& endpoint)
{
    d_peerAddress = endpoint;
}

inline
const btlso::IPv4Address& TcpCbConnector::peer() const
{
    return d_peerAddress;
}

inline
int TcpCbConnector::numChannels() const
{
    return static_cast<int>(d_channels.size());
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
