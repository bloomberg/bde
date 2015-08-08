// btlsos_tcpconnector.h   -*-C++-*-
#ifndef INCLUDED_BTLSOS_TCPCONNECTOR
#define INCLUDED_BTLSOS_TCPCONNECTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a synchronous connector to TCP-based servers
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  btlsos::TcpConnector: blocking 'btesc'-style channel allocator.
//
//@SEE_ALSO: btlsos_tcpacceptor btlsos_tcptimedchannel
//
//@DESCRIPTION: This component provides a synchronous connector to TCP-based
// servers that adheres to 'btlsc::ChannelAllocator' protocol.  Both timed and
// non-timed (synchronous) channels can be allocated in a non-timed fashion
// using the 'allocateTimed' and 'allocate' methods respectively.
//
// The connector has the flexibility of changing the address of the peer server
// at run-time (and producing channels connected to this end-point) without any
// effects on the state of managed channels.
//
///Thread-safety
///-------------
// The connector is *thread safe*, meaning that any operation can be called on
// *distinct instances* from different threads without any side-effect (which,
// generally speaking, means that there is no 'static' data), but not *thread
// enabled* (i.e., two threads cannot safely call methods on the *same
// instance* without external synchronization).  Connector is not *async-safe*,
// meaning that one or more functions cannot be invoked safely from a signal
// handler.
//
///Usage
///-----
// The following usage example shows a possible implementation of an echo
// client.  (See 'btlsos_tcpacceptor' for an echo server.)  An echo server
// accepts a connection and sends any received data back to the client (until
// the connection is terminated).  The echo client demonstrated in this usage
// example creates a packet with a pre-determined data pattern, sends it to the
// server, waits for a response and then verifies that the received data is the
// same (as was send).  The operation will be repeated for a certain number of
// packets and then exit.  Without going into details of a pattern, let's
// suppose that there is a function that generates the pattern into a buffer:
//..
//    void generatePattern(char *buffer, int length);
//        // Load into the specified 'buffer' a generated data pattern of the
//        // specified 'length'.
//..
//
// First, create a concrete socket factory that is used to manage stream
// sockets:
//..
//    btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
//..
// Second, define configuration parameters for the connector:
//..
//    enum { ECHO_PORT = 1888 };
//    enum {
//        NUM_PACKETS = 5,
//        PACKET_SIZE = 10
//    }; // TCP/IP over Ethernet
//
//    const char *SERVER_IP = "127.0.0.1";           // assume local host
//    btlso::IPv4Address serverAddress(SERVER_IP, ECHO_PORT);
//
//    bsls::TimeInterval connectTimeout(120, 0);      // 2 minutes
//..
// Now, create a connector and set the configuration parameters:
//..
//   btlsos::TcpConnector connector(&factory); // Use default allocator
//   assert(0 == connector.isInvalid());
//   assert(0 == connector.setPeer(serverAddress));// 'serverAddress' is valid.
//   assert(0 == connector.isInvalid());
//..
// Set communication parameters for the channel:
//..
//   enum { READ_SIZE = 10 };                // only for demo
//   bsls::TimeInterval readTimeout(1.0);     // 1 second
//   bsls::TimeInterval writeTimeout(30.0);   // 30 seconds
//..
// Prepare the "input" packet that will be sent on every iteration, and save it
// as a "control" packet:
//..
//   char controlPacket[PACKET_SIZE];
//   char inputPacket[PACKET_SIZE];
//   generatePattern(inputPacket, PACKET_SIZE);
//   memcpy(controlPacket, inputPacket, PACKET_SIZE);
//..
// Establish a connection with the echo server:
//..
//   int status;
//   btlsc::TimedChannel *channel = connector.allocateTimed(&status);
//   if (!channel) {
//       assert(0 >= status);     // Async.  interrupts are *not* enabled.
//       bsl::cout << "Failed to connect to the peer." << bsl::endl;
//       // In any case, invalidate the allocator, and exit.
//       connector.invalidate();
//       return -1;
//   }
//..
// Send 'NUM_PACKETS' packets to the server, wait for the response for each,
// and verify that the received packet is correct:
//..
//   assert(0 == channel->isInvalid());
//   char receivePacket[PACKET_SIZE];
//   for (int i = 0; i < NUM_PACKETS; ++i) {
//       // Request/response mechanism
//       int writeStatus = channel->timedWrite(
//                                     inputPacket,
//                                     PACKET_SIZE,
//                                     bdlt::CurrentTime::now() + writeTimeout);
//       if (PACKET_SIZE != writeStatus) {
//           bsl::cout << "Failed to send data." << bsl::endl;
//           break;
//       }
//       int readStatus = channel->timedRead(
//                                      receivePacket,
//                                      PACKET_SIZE,
//                                      bdlt::CurrentTime::now() + readTimeout);
//       if (PACKET_SIZE != readStatus) {
//           bsl::cout << "Failed to read data" << bsl::endl;
//           break;
//       }
//       assert(0 == memcmp(receivedPacket, controlPacket PACKET_SIZE);
//   }
//
//   // Perform proper shut down procedure
//   channel->invalidate();
//   connector.deallocate(channel);
//....

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BTLSC_CHANNELALLOCATOR
#include <btlsc_channelallocator.h>
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

namespace btlso { template <class ADDRESS> class StreamSocketFactory; }
namespace btlso { template <class ADDRESS> class StreamSocket; }

namespace btlsos {
                        // =========================
                        // class TcpConnector
                        // =========================

class TcpConnector : public btlsc::ChannelAllocator {
    // This class implements a 'btesc'-style timed channel allocator for
    // client-side (i.e., "connected") sockets.  The allocations are not-timed
    // and produce either timed or non-timed channels.  The return status for
    // any operation mandated by 'btlsc::ChannelAllocator' protocol is negative
    // for a failure, and positive for a (restartable) interrupt on an
    // underlying system call.  This connector provides the ability to change
    // the end-points at run-time without any effect on the state of any
    // channels currently managed by this connector.

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
    // NOT IMPLEMENTED
    TcpConnector(const TcpConnector&);
    TcpConnector& operator=(const TcpConnector&);
  public:
    // CREATORS
    explicit TcpConnector(
            btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
            bslma::Allocator                             *basicAllocator = 0);
        // Create a connector that uses the specified 'factory' to create
        // stream sockets.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined if 'factory'
        // is 0.  Note that connector is created NOT in an invalid state (as
        // reported by 'isInvalid' method), though subsequent allocations will
        // *fail* until the peer address is specified (using 'setPeer').

    TcpConnector(
             btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
             int                                           numElements,
             bslma::Allocator                             *basicAllocator = 0);
        // Create a connector that uses the specified 'factory' to create
        // stream sockets with enough internal capacity to accommodate up to
        // the specified 'numElements' channels without reallocation.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined if 'factory' is 0 or
        // 0 >= numElements.  Note that connector is created NOT in an invalid
        // state (as reported by the 'isInvalid' method), though subsequent
        // allocations will *fail* until the peer is specified (using
        // 'setPeer').

    ~TcpConnector();
        // Destroy this connector.  The behavior is undefined if there are any
        // channels being managed.

    // MANIPULATORS
    btlsc::Channel *allocate(int *status, int flags = 0);
        // Allocate a stream-based channel.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::k_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Return the address of a channel
        // on success, and 0 otherwise.  On an unsuccessful allocation, load
        // the specified 'status' with a positive value if an asynchronous
        // event interrupted the allocation, and a negative value (indicating
        // an error) otherwise; 'status' is not modified on success.  A
        // non-null channel address will remain valid until deallocated
        // explicitly (see 'deallocate').  An allocation that fails with a
        // positive status is likely to succeed if retried.  An allocation
        // error does not *necessarily* invalidate this allocator, and so
        // subsequent allocations *may* succeed.  Use the 'isInvalid' method
        // for more information on the state of this allocator.  Note -2 is
        // loaded into 'status' if the peer is not set (see 'setPeer').

    btlsc::TimedChannel *allocateTimed(int *status, int flags = 0);
        // Allocate a stream-based timed channel.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::k_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Return the address of a timed
        // channel on success, and 0 otherwise.  On an unsuccessful allocation,
        // load the specified 'status' with a positive value if an asynchronous
        // event interrupted the allocation, and a negative value (indicating
        // an error) otherwise; 'status' is not modified on success.  A
        // non-null channel address will remain valid until deallocated
        // explicitly (see 'deallocate').  An allocation that fails with a
        // positive status is likely to succeed if retried.  An allocation
        // error does not *necessarily* invalidate this allocator, and so
        // subsequent allocations *may* succeed.  Use the 'isInvalid' method
        // for more information on the state of this allocator.  Note -2 is
        // loaded into 'status' if the peer is not set (see 'setPeer').

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

//-----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

// MANIPULATORS
inline
void TcpConnector::setPeer(const btlso::IPv4Address& endpoint)
{
    d_peerAddress = endpoint;
}

inline
void TcpConnector::invalidate()
{
    d_isInvalidFlag = 1;
}

// ACCESSORS
inline
const btlso::IPv4Address& TcpConnector::peer() const
{
    return d_peerAddress;
}

inline
int TcpConnector::numChannels() const
{
    return static_cast<int>(d_channels.size());
}

inline
int TcpConnector::isInvalid() const
{
    return d_isInvalidFlag;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
