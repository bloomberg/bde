// btlsos_tcptimedacceptor.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSOS_TCPTIMEDACCEPTOR
#define INCLUDED_BTLSOS_TCPTIMEDACCEPTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a blocking acceptor of TCP-based connections with timeout.
//
//@CLASSES:
//  btlsos::TcpTimedAcceptor: a blocking 'btesc'-style timed channel allocator
//
//@SEE_ALSO: btlsos_tcpchannel,  btlsos_tcptimedchannel,
//           btlsos_tcptimedconnector
//
//@DESCRIPTION: This component provides a blocking single-port acceptor of TCP
// connections with timeout capability, 'btlsos::TcpTimedAcceptor', that
// adheres to 'btlsc::TimedChannelAllocator' protocol.  Both timed and
// non-timed (blocking) channels can be allocated in a timed or non-timed
// fashion correspondingly as indicated by the following table:
//..
//       +=============================================================+
//       |  Result/Operation |        Timed         |    Non-Timed     |
//       +-------------------------------------------------------------+
//       |      Timed        | 'timedAllocateTimed' | 'allocateTimed'  |
//       +-------------------------------------------------------------+
//       |    Non-Timed      |    'timedAllocate'   |    'allocate'    |
//       +=============================================================+
//..
// The acceptor has the flexibility of opening and closing a listening socket
// with no effect on any existing channels managed by this object.
//
///Thread Safety
///-------------
// The acceptor is *thread safe*, meaning that any operation can be called on
// *distinct instances* from different threads without any side-effects (which,
// generally speaking, means that there is no 'static' data), but not *thread
// enabled* (i.e., two threads cannot safely call methods on the *same
// instance* without external synchronization).  Acceptor is not *async-safe*,
// meaning that one or more functions cannot be invoked safely from a signal
// handler.
//
///Performance
///-----------
// Acceptor is optimized for operations with the timeout.  Non-timed operations
// will have worse performance than their respective counterparts in the
// non-timed version of the acceptor (i.e., 'btlsos_tcpacceptor').  If timed
// allocations are not required, 'btlsos::TcpAcceptor' should be used instead.
//
///Usage
///-----
// The following usage example shows a possible implementation of a single-user
// echo server.  An echo server accepts a connection and sends back any
// received data back to the client (until the connection is terminated).  This
// server requires that data is read from an accepted connection within certain
// time interval and be dropped on timeout.
//
// First, create a concrete socket factory that is used to manage stream
// sockets:
//..
//  btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
//..
// Second, define configuration parameters for the acceptor:
//..
//  enum {
//      k_ECHO_PORT  = 1888,
//      k_QUEUE_SIZE =   32
//  };
//  btlso::IPv4Address serverAddress;
//  serverAddress.setPortNumber(k_ECHO_PORT);
//  bsls::TimeInterval acceptTimeout(120, 0);  // 2 minutes
//..
// Now, create an acceptor and prepare it for accepting connections:
//..
//  btlsos::TcpTimedAcceptor acceptor(&factory);  // uses default allocator
//  assert(0 == acceptor.isInvalid());
//  if (0 != acceptor.open(serverAddress, k_QUEUE_SIZE)) {
//      bsl::cout << "Can't open listening socket" << bsl::endl;
//      return -1;                                                    // RETURN
//  }
//  assert(acceptor.address() == serverAddress);
//..
// Set communication parameters for a channel:
//..
//  enum { k_READ_SIZE = 8 };
//  // Note: this is OK *if and only if* it is in the 'main' function.
//  bsls::TimeInterval readTimeout(30, 0);  // 30 seconds
//  bsls::TimeInterval writeTimeout(0.5);   // 0.5 seconds
//..
// Go into "infinite" loop, accepting connections and servicing user requests:
//..
//  while (0 == acceptor.isInvalid()) {
//      int status;
//      btlsc::TimedChannel *channel = acceptor.timedAllocateTimed(
//                                   &status,
//                                   bdlt::CurrentTime::now() + acceptTimeout);
//      if (channel) {
//          while (1) {
//              const char * result;
//              int readStatus = channel->timedBufferedReadRaw(
//                                     &result,
//                                     k_READ_SIZE,
//                                     bdlt::CurrentTime::now() + readTimeout);
//              if (0 >= readStatus) {
//                  if (verbose) {
//                      bsl::cout << "Failed to read data, readStatus = "
//                                << readStatus << bsl::endl;
//                  }
//                  break;
//              }
//              else {
//                  if (verbose) {
//                      bsl::cout << "readStatus = "
//                                << readStatus
//                                << bsl::endl;
//                  }
//              }
//              int ws = channel->timedWrite(
//                                    result,
//                                    readStatus,
//                                    bdlt::CurrentTime::now() + writeTimeout);
//              if (readStatus != ws) {
//                  if (verbose) {
//                      bsl::cout << "Failed to send data, writeStatus = "
//                                << ws << bsl::endl;
//                  }
//                  break;
//              }
//              else {
//                  if (verbose) {
//                      bsl::cout << "writeStatus = " << ws << bsl::endl;
//                  }
//              }
//          }
//          acceptor.deallocate(channel);
//      }
//      else {
//          assert(status <= 0);  // Interrupts are not enabled.
//          if (0 == status) {
//              if (verbose) {
//                  bsl::cout << "Timed out accepting a connection"
//                            << bsl::endl;
//              }
//          }
//      }
//  }
//..
// At this point, acceptor became invalid.  The server port must be closed
// explicitly:
//..
//  assert(acceptor.isInvalid());
//  assert(0 == acceptor.close());
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSC_TIMEDCHANNELALLOCATOR
#include <btlsc_timedchannelallocator.h>
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

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace btlso { template<class ADDRESS> class StreamSocketFactory; }
namespace btlso { template<class ADDRESS> class StreamSocket; }
namespace btlsc { class TimedChannel; }
namespace btlsc { class Channel; }
namespace btlsos {

                          // ======================
                          // class TcpTimedAcceptor
                          // ======================

class TcpTimedAcceptor : public btlsc::TimedChannelAllocator {
    // This class implements a 'btesc'-style timed channel allocator for a
    // single, server-side (i.e., listening) socket.  The allocations can be
    // timed or not-timed and produce either timed or non-timed channels.  The
    // return status for an operation mandated by
    // 'btlsc::TimedChannelAllocator' protocol is negative for failure, 0 on
    // timeout, and positive for a (restartable) interrupt on an underlying
    // system call.  The status value of -2 is reserved to indicate an
    // allocation attempt on an uninitialized acceptor (i.e., no listening
    // socket is established).  The acceptor provides a "delayed open" (where a
    // listening socket is established at some point after construction), and
    // allows the listening socket to be closed (and opened again) with no
    // effect on the state of any other channel currently managed by this
    // acceptor.

    bdlma::Pool                                    d_pool;
        // memory pool for channels

    bsl::vector<btlsc::Channel*>                   d_channels;
        // managed channels

    btlso::StreamSocketFactory<btlso::IPv4Address> *d_factory_p;
        // factory used to supply sockets

    btlso::StreamSocket<btlso::IPv4Address>        *d_serverSocket_p;
        // listening socket
    btlso::IPv4Address                             d_serverAddress;
        // the address of the listening socket

    int                                           d_isInvalidFlag;
        // a flag to be set if this acceptor is invalid

  private: // not implemented
    TcpTimedAcceptor(const TcpTimedAcceptor&);
    TcpTimedAcceptor& operator=(const TcpTimedAcceptor&);

  public:
    // CREATORS
    TcpTimedAcceptor(
           btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
           bslma::Allocator                               *basicAllocator = 0);
        // Create a timed acceptor that uses the specified 'factory' to create
        // stream sockets.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'factory' is not 0.  Note that the acceptor is created NOT in an
        // invalid state (as reported by 'isInvalid' method), though subsequent
        // allocations will *fail* until a listening socket is created (using
        // 'open').

    TcpTimedAcceptor(
           btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
           int                                             initialCapacity,
           bslma::Allocator                               *basicAllocator = 0);
        // Create a timed acceptor that uses the specified 'factory' to create
        // stream sockets with enough internal capacity to accommodate up to
        // the specified 'initialCapacity' channels without reallocation.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined if 'factory' is 0 or unless
        // '0 < initialCapacity'.  Note that the acceptor is created NOT in an
        // invalid state (as reported by the 'isInvalid' method), though the
        // subsequent allocations will *fail* until a listening socket is
        // created (using 'open').

    ~TcpTimedAcceptor();
        // Destroy this acceptor.  The behavior is undefined unless all
        // allocated channels are deallocated.

    // MANIPULATORS
    btlsc::Channel *allocate(int *status, int flags = 0);
        // Allocate a stream-based channel.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return the address of a channel on success, and
        // 0 otherwise.  On an unsuccessful allocation, load the specified
        // 'status' with a positive value if an asynchronous event interrupted
        // the allocation, and a negative value (indicating an error)
        // otherwise; 'status' is not modified on success.  A channel address
        // will remain valid until deallocated explicitly (see 'deallocate').
        // An allocation that fails with a positive status is likely to succeed
        // if retried.  An allocation error does not *necessarily* invalidate
        // this allocator, and so subsequent allocations *may* succeed.  Use
        // the 'isInvalid' method for more information on the state of this
        // allocator.  Note that -2 is loaded into 'status' if a listening
        // socket is not established (see 'open').

    btlsc::Channel *timedAllocate(int                       *status,
                                  const bsls::TimeInterval&  timeout,
                                  int                        flags = 0);
        // Allocate a stream-based channel or interrupt after the specified
        // absolute 'timeout' time is reached.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return the address of a callback channel on
        // success, and 0 otherwise.  On an unsuccessful allocation, load the
        // specified 'status' with 0 if 'timeout' interrupted this operation, a
        // positive value if the interruption is due to an asynchronous event,
        // and a negative value (indicating an error) otherwise; 'status' is
        // not modified on success.  A channel address will remain valid until
        // deallocated explicitly (see 'deallocate').  An allocation that fails
        // with a non-negative status is likely to succeed if retried.  An
        // allocation error does not *necessarily* invalidate this allocator,
        // and so subsequent allocations *may* succeed.  Use the 'isInvalid'
        // method for more information on the state of this allocator.  Note
        // that if the 'timeout' value has already passed, the allocation will
        // still be attempted, but the attempt will not block.  Also note that
        // -2 is loaded into 'status' if a listening socket is not established
        // (see 'open').

    btlsc::TimedChannel *allocateTimed(int *status, int flags = 0);
        // Allocate a stream-based timed channel.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return the address of a timed callback channel
        // on success, and 0 otherwise.  On an unsuccessful allocation, load
        // the specified 'status' with a positive value if an asynchronous
        // event interrupted the allocation, and a negative value (indicating
        // an error) otherwise; 'status' is not modified on success.  A channel
        // address will remain valid until deallocated explicitly (see
        // 'deallocate').  An allocation that fails with a positive status is
        // likely to succeed if retried.  An allocation error does not
        // *necessarily* invalidate this allocator, and so subsequent
        // allocations *may* succeed.  Use the 'isInvalid' method for more
        // information on the state of this allocator.  Note that -2 is loaded
        // into 'status' if a listening socket is not established (see 'open').

    btlsc::TimedChannel *timedAllocateTimed(
                                         int                       *status,
                                         const bsls::TimeInterval&  timeout,
                                         int                        flags = 0);
        // Allocate a stream-based timed channel or interrupt after the
        // specified absolute 'timeout' time is reached.  If the optionally
        // specified 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Return the address of a timed
        // callback channel on success, and 0 otherwise.  On an unsuccessful
        // allocation, load the specified 'status' with 0 if 'timeout'
        // interrupts this operation, a positive value if the interruption is
        // due to an asynchronous event, and a negative value (indicating an
        // error) otherwise; 'status' is not modified on success.  A non-null
        // channel address will remain valid until deallocated explicitly (see
        // 'deallocate').  An allocation that fails with a non-negative status
        // is likely to succeed if retried.  An allocation error does not
        // *necessarily* invalidate this allocator, and so subsequent
        // allocations *may* succeed.  Use the 'isInvalid' method for more
        // information on the state of this allocator.  Note that if the
        // 'timeout' value has already passed, the allocation will still be
        // attempted, but the attempt will not block.  Also note that
        // -2 is loaded into 'status' if a listening socket is not established
        // (see 'open').

    int close();
        // Close the listening socket; return 0 on success, and a non-zero
        // value otherwise.  A listening socket yields a non-zero server
        // address (see 'address').  The behavior is undefined unless the
        // listening socket is successfully established.  Note that closing a
        // listening port has no effect on any other channels managed by this
        // allocator.

    void deallocate(btlsc::Channel *channel);
        // Reclaim all afforded services for the specified 'channel'.  The
        // behavior is undefined unless 'channel' is currently allocated from
        // this allocator, (i.e., was previously obtained from this instance
        // and has not subsequently been deallocated).

    void invalidate();
        // Place this allocator in a permanently invalid state.  No subsequent
        // allocations will succeed.  Note that invalidating the allocator has
        // no effect on the state of any channel managed by it nor on the
        // listening socket, which, if established, must be closed explicitly
        // (using 'close' method).

    int open(const btlso::IPv4Address& endpoint,
             int                       queueSize,
             int                       reuseAddressFlag = 1);
        // Establish a listening socket having the specified 'queueSize'
        // maximum number of pending connections on the specified 'endpoint'.
        // Optionally specify a 'reuseAddressFlag' used to set
        // 'SO_REUSEADDRESS' option on a listening socket.  If
        // 'reuseAddressFlag' is not specified, this option is enabled.  Return
        // 0 on success and a non-zero value otherwise.  The behavior is
        // undefined unless '0 < queueSize' and this object is not invalidated.

    int setOption(int level, int option, int value);
        // Set the specified socket 'option' of the specified 'level' on the
        // listening socket to the specified 'value'.  Return 0 on success and
        // a non-zero value otherwise.  (The list of commonly-supported options
        // is available in 'btlso_socketoptutil'.)  The behavior is undefined
        // unless the listening socket is established and this object is not
        // invalidated.  Note that all sockets allocated from this acceptor
        // will inherit the options' values set on the listening socket.

    // ACCESSORS
    const btlso::StreamSocket<btlso::IPv4Address> *socket() const;
        // Return the address of the stream-socket used by this acceptor, or 0
        // if this acceptor is not opened to establish a listening socket yet.

    const btlso::IPv4Address& address() const;
        // Return the (fully qualified) address of the listening socket, or an
        // invalid address of (ANY_ADDRESS, ANY_PORT) if the server is not
        // established (see 'btlso_ipv4address').

    int getOption(int *result, int level, int option) const;
        // Load into the specified 'result' the current value of the specified
        // 'option' of the specified 'level' set on the listening socket.
        // Return 0 on success and a non-zero value otherwise.  The list of
        // commonly-supported options (and levels) is enumerated in
        // 'btlso_socketoptutil'.  The behavior is undefined if result is 0 or
        // if the listening socket has not been established.

    int isInvalid() const;
        // Return a non-zero value if this channel allocator is *permanently*
        // invalid (i.e., no subsequent allocation requests will succeed), and
        // 0 otherwise.

    int numChannels() const;
        // Return the number of channels currently allocated from this
        // acceptor.
};

// ----------------------------------------------------------------------------
//                             INLINE DEFINITIONS
// ----------------------------------------------------------------------------

inline
const btlso::IPv4Address& TcpTimedAcceptor::address() const
{
    return d_serverAddress;
}

inline
const btlso::StreamSocket<btlso::IPv4Address> *TcpTimedAcceptor::socket()
                                                                          const
{
    return d_serverSocket_p;
}

inline
void TcpTimedAcceptor::invalidate()
{
    d_isInvalidFlag = 1;
}

inline
int TcpTimedAcceptor::isInvalid() const
{
    return d_isInvalidFlag;
}
inline
int TcpTimedAcceptor::numChannels() const
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
