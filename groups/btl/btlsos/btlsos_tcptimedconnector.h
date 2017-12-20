// btlsos_tcptimedconnector.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSOS_TCPTIMEDCONNECTOR
#define INCLUDED_BTLSOS_TCPTIMEDCONNECTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a synchronous connector (with timeout) to TCP servers.
//
//@CLASSES:
//  btlsos::TcpTimedConnector: blocking 'btesc'-style timed channel allocator.
//
//@SEE_ALSO: btlsos_tcptimedacceptor btlsos_tcptimedchannel
//
//@DESCRIPTION: This component provides a synchronous connector to TCP-based
// servers, 'btlsos::TcpTimedConnector', that adheres to
// 'btlsc::TimedChannelAllocator' protocol.  Both timed and non-timed
// (synchronous) channels can be allocated in a timed and non-timed fashion as
// indicated by the following table:
//..
//       +=============================================================+
//       |  Result/Operation |        Timed         |    Non-Timed     |
//       +-------------------------------------------------------------+
//       |      Timed        | 'timedAllocateTimed' | 'allocateTimed'  |
//       +-------------------------------------------------------------+
//       |    Non-Timed      |    'timedAllocate'   |    'allocate'    |
//       +=============================================================+
//..
// The connector has the flexibility of changing the address of the peer server
// at run-time (and producing channels connected to this end-point) without any
// effects on the state of managed channels.
//
///Thread Safety
///-------------
// The connector is *thread safe*, meaning that any operation can be called on
// *distinct instances* from different threads without any side-effect (which,
// generally speaking, means that there is no 'static' data), but not *thread
// enabled* (i.e., two threads cannot safely call methods on the *same
// instance* without external synchronization).  Connector is not *async-safe*,
// meaning that one or more functions cannot be invoked safely from a signal
// handler.
//
///Performance
///-----------
// Connector is optimized for operations with the timeout.  Non-timed
// operations will have worse performance than their respective counterparts in
// the non-timed version of the connector (i.e., 'btlsos_tcpconnector').  If
// timed allocations are not required, 'btlsos::TcpConnector' should be used
// instead.
//
///Usage
///-----
// The following usage example shows a possible implementation of an echo
// client (see 'btlsos_tcptimedacceptor' for an echo server).  An echo server
// accepts a connection and sends any received data back to the client (until
// the connection is terminated).  The echo client demonstrated in this usage
// example creates a packet with a pre-determined data pattern, sends it to the
// server, waits for a response and then verifies that the received data is the
// same (as was send).  The operation will be repeated for a certain number of
// packets and then exit.  Without going into details of a pattern, let's
// suppose that there is a function that generates the pattern into a buffer:
//..
//  void generatePattern(char *buffer, int length)
//      // Load into the specified 'buffer' a generated data pattern of the
//      // specified 'length'.
//  {
//      if (buffer) {
//          #ifdef BSLS_PLATFORM_OS_UNIX
//              snprintf(buffer, length, "%d", length);
//          #else
//              _snprintf(buffer, length, "%d", length);
//          #endif
//      }
//      return;
//  }
//..
// First, create a concrete socket factory that is used to manage stream
// sockets:
//..
//  btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
//..
// Second, define configuration parameters for the connector:
//..
//  enum { k_ECHO_PORT = 1888 };
//  enum {
//      k_NUM_PACKETS =  5,
//      k_PACKET_SIZE = 10
//  }; // TCP/IP over Ethernet
//
//  const char *SERVER_IP = "127.0.0.1";  // assume local host
//  btlso::IPv4Address serverAddress(SERVER_IP, k_ECHO_PORT);
//
//  bsls::TimeInterval connectTimeout(120, 0);  // 2 minutes
//..
// Now, create a connector and set the configuration parameters:
//..
//  btlsos::TcpTimedConnector connector(&factory);  // Use default allocator
//  assert(0 == connector.isInvalid());
//  connector.setPeer(serverAddress);
//  assert(0 == connector.isInvalid());  // 'serverAddress' is valid
//..
// Set communication parameters for the channel:
//..
//  bsls::TimeInterval readTimeout(1.0);    // 1 second
//  bsls::TimeInterval writeTimeout(30.0);  // 30 seconds
//..
// Prepare the "input" packet that will be sent on every iteration, and save it
// as a "control" packet:
//..
//  char controlPacket[k_PACKET_SIZE];
//  char inputPacket[k_PACKET_SIZE];
//  generatePattern(inputPacket, k_PACKET_SIZE);
//  memcpy(controlPacket, inputPacket, k_PACKET_SIZE);
//..
// Establish a connection with the echo server:
//..
//  int status;
//  btlsc::TimedChannel *channel =
//                  connector.timedAllocateTimed(
//                                  &status,
//                                  bdlt::CurrentTime::now() + connectTimeout);
//  if (!channel) {
//      assert(0 >= status);  // Asynchronous interrupts are *not* enabled.
//      if (status) {
//          bsl::cout << "Failed to connect to the peer."
//                    << bsl::endl;
//      }
//      else {
//          bsl::cout << "Connection attempt has timed out."
//                    << bsl::endl;
//      }
//      // In any case, invalidate the allocator, and exit.
//      connector.invalidate();
//      return -1;                                                    // RETURN
//  }
//..
// Send 'k_NUM_PACKETS' packets to the server, wait for the response for each,
// and verify that the received packet is correct:
//..
//  assert(0 == channel->isInvalid());
//  char receivedPacket[k_PACKET_SIZE];
//  for (int i = 0; i < k_NUM_PACKETS; ++i) {
//      // Request/response mechanism
//      int writeStatus = channel->timedWrite(
//                                    inputPacket,
//                                    k_PACKET_SIZE,
//                                    bdlt::CurrentTime::now() + writeTimeout);
//      if (k_PACKET_SIZE != writeStatus) {
//          bsl::cout << "Failed to send data." << bsl::endl;
//          break;
//      }
//      int readStatus = channel->timedRead(
//                                     receivedPacket,
//                                     k_PACKET_SIZE,
//                                     bdlt::CurrentTime::now() + readTimeout);
//      if (k_PACKET_SIZE != readStatus) {
//          cout << "Failed to read data" << bsl::endl;
//          break;
//      }
//      assert(0 == memcmp(receivedPacket,
//                         controlPacket,
//                         k_PACKET_SIZE));
//  }
//
//  // Perform proper shut down procedure
//  channel->invalidate();
//  connector.deallocate(channel);
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BTLSC_TIMEDCHANNELALLOCATOR
#include <btlsc_timedchannelallocator.h>
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
namespace btlsc { class TimedChannel; }
namespace btlsc { class Channel; }
namespace btlso { template<class ADDRESS> class StreamSocketFactory; }
namespace btlso { template<class ADDRESS> class StreamSocket; }
namespace btlsos {

                         // =======================
                         // class TcpTimedConnector
                         // =======================

class TcpTimedConnector : public btlsc::TimedChannelAllocator {
    // This class implements a 'btesc'-style timed channel allocator for
    // client-side (i.e., "connected") sockets.  The allocations can be timed
    // or not-timed and produce either timed or non-timed channels.  The return
    // status for any operation mandated by 'btlsc::TimedChannelAllocator'
    // protocol is negative for a failure, 0 on timeout, and positive for a
    // (restartable) interrupt on an underlying system call.  This connector
    // provides the ability to change the end-points at run-time without any
    // effect on the state of any channels currently managed by this connector.

    bdlma::Pool                                    d_pool;
        // memory pool for channels

    bsl::vector<btlsc::Channel*>                   d_channels;
        // managed channels

    btlso::StreamSocketFactory<btlso::IPv4Address> *d_factory_p;
        // factory used to supply sockets

    btlso::IPv4Address                             d_peerAddress;
        // current address of the server

    int                                           d_isInvalidFlag;
        // a flag to be set if this acceptor is invalid

  private:
    // Not implemented:
    TcpTimedConnector(const TcpTimedConnector&);
    TcpTimedConnector& operator=(const TcpTimedConnector&);

  public:
    // CREATORS
    TcpTimedConnector(
           btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
           bslma::Allocator                               *basicAllocator = 0);
        // Create a timed connector that uses the specified 'factory' to create
        // stream sockets.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined if 'factory'
        // is 0.  Note that connector is created NOT in an invalid state (as
        // reported by 'isInvalid' method), though subsequent allocations will
        // *fail* until the peer address is specified (using 'setPeer').

    TcpTimedConnector(
           btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
           int                                             numElements,
           bslma::Allocator                               *basicAllocator = 0);
        // Create a timed connector that uses the specified 'factory' to create
        // stream sockets with enough internal capacity to accommodate up to
        // the specified 'numElements' channels without reallocation.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined if 'factory' is 0 or
        // '0 >= numElements'.  Note that connector is created NOT in an
        // invalid state (as reported by the 'isInvalid' method), though
        // subsequent allocations will *fail* until the peer is specified
        // (using 'setPeer').

    ~TcpTimedConnector();
        // Destroy this connector.  The behavior is undefined if there are any
        // channels being managed.

    // MANIPULATORS
    btlsc::Channel *allocate(int *status, int flags = 0);
        // Allocate a stream-based channel.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return the address of a channel on success, and
        // 0 otherwise.  On an unsuccessful allocation, load the specified
        // 'status' with a positive value if an asynchronous event interrupted
        // the allocation, and a negative value (indicating an error)
        // otherwise; 'status' is not modified on success.  A non-null channel
        // address will remain valid until deallocated explicitly (see
        // 'deallocate').  An allocation that fails with a positive status is
        // likely to succeed if retried.  An allocation error does not
        // *necessarily* invalidate this allocator, and so subsequent
        // allocations *may* succeed.  Use the 'isInvalid' method for more
        // information on the state of this allocator.  Note -2 is loaded into
        // 'status' if the peer is not set (see 'setPeer').

    btlsc::TimedChannel *allocateTimed(int *status, int flags = 0);
        // Allocate a stream-based timed channel.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return the address of a timed channel on
        // success, and 0 otherwise.  On an unsuccessful allocation, load the
        // specified 'status' with a positive value if an asynchronous event
        // interrupted the allocation, and a negative value (indicating an
        // error) otherwise; 'status' is not modified on success.  A non-null
        // channel address will remain valid until deallocated explicitly (see
        // 'deallocate').  An allocation that fails with a positive status is
        // likely to succeed if retried.  An allocation error does not
        // *necessarily* invalidate this allocator, and so subsequent
        // allocations *may* succeed.  Use the 'isInvalid' method for more
        // information on the state of this allocator.  Note -2 is loaded into
        // 'status' if the peer is not set (see 'setPeer').

    void deallocate(btlsc::Channel *channel);
        // Reclaim all afforded services for the specified 'channel'.  The
        // behavior is undefined unless 'channel' is currently allocated from
        // this allocator, (i.e., was previously obtained from this instance
        // and has not subsequently been deallocated).

    void invalidate();
        // Place this allocator in a permanently invalid state.  No subsequent
        // allocations will succeed.  Note that invalidating the allocator has
        // no effect on the state of any channel managed by it.

    void setPeer(const btlso::IPv4Address& endpoint);
        // Set the address of the peer server with which this connector is
        // associated to the specified 'endpoint'.

    btlsc::Channel *timedAllocate(int                       *status,
                                  const bsls::TimeInterval&  timeout,
                                  int                        flags = 0);
        // Allocate a stream-based channel or interrupt after the specified
        // absolute 'timeout' time is reached.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
        // events" are permitted to interrupt this operation; by default, such
        // events are ignored.  Return the address of a channel on success, and
        // 0 otherwise.  On an unsuccessful allocation, load the specified
        // 'status' with 0 if 'timeout' interrupted this operation, a positive
        // value if the interruption was due to an asynchronous event, and a
        // negative value (indicating an error) otherwise; 'status' is not
        // modified on success.  A non-null channel address will remain valid
        // until deallocated explicitly (see 'deallocate').  An allocation that
        // fails with a non-negative status is likely to succeed if retried.
        // An allocation error does not *necessarily* invalidate this
        // allocator, and so subsequent allocations *may* succeed.  Use the
        // 'isInvalid' method for more information on the state of this
        // allocator.  Note that if the 'timeout' value has already passed, the
        // allocation will still be attempted, but the attempt will not block.
        // Note that -2 is loaded into 'status' if the peer is not set (see
        // 'setPeer').

    btlsc::TimedChannel *timedAllocateTimed(
                                         int                       *status,
                                         const bsls::TimeInterval&  timeout,
                                         int                        flags = 0);
        // Allocate a stream-based timed channel or interrupt after the
        // specified absolute 'timeout' time is reached.  If the optionally
        // specified 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Return the address of a timed
        // channel on success, and 0 otherwise.  On an unsuccessful allocation,
        // load the specified 'status' with 0 if 'timeout' interrupted this
        // operation, a positive value if the interruption was due to an
        // asynchronous event, and a negative value (indicating an error)
        // otherwise; 'status' is not modified on success.  A non-null channel
        // address will remain valid until deallocated explicitly (see
        // 'deallocate').  An allocation that fails with a non-negative status
        // is likely to succeed if retried.  An allocation error does not
        // *necessarily* invalidate this allocator, and so subsequent
        // allocations *may* succeed.  Use the 'isInvalid' method for more
        // information on the state of this allocator.  Note that if the
        // 'timeout' value has already passed, the allocation will still be
        // attempted, but the attempt will not block.  Note that -2 is loaded
        // into 'status' if the peer is not set (see 'setPeer').

    // ACCESSORS
    const btlso::IPv4Address& peer() const;
        // Return the (fully qualified) address of the peer server associated
        // with this connector, or an invalid address of (ANY_ADDRESS,
        // ANY_PORT) if no such server exists (see 'bteso::IPv4address').

    int isInvalid() const;
        // Return a non-zero value if this channel allocator is *permanently*
        // invalid (i.e., no subsequent allocation requests will succeed), and
        // 0 otherwise.

    int numChannels() const;
        // Return the number of channels currently allocated from this
        // connector.

};

// ----------------------------------------------------------------------------
//                             INLINE DEFINITIONS
// ----------------------------------------------------------------------------

inline
void TcpTimedConnector::setPeer(const btlso::IPv4Address& endpoint)
{
    d_peerAddress = endpoint;
}

inline
void TcpTimedConnector::invalidate()
{
    d_isInvalidFlag = 1;
}

inline
const btlso::IPv4Address& TcpTimedConnector::peer() const
{
    return d_peerAddress;
}

inline
int TcpTimedConnector::numChannels() const
{
    return static_cast<int>(d_channels.size());
}

inline
int TcpTimedConnector::isInvalid() const
{
    return d_isInvalidFlag;
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
