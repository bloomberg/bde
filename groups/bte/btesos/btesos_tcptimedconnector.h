// btesos_tcptimedconnector.h   -*-C++-*-
#ifndef INCLUDED_BTESOS_TCPTIMEDCONNECTOR
#define INCLUDED_BTESOS_TCPTIMEDCONNECTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a synchronous connector (with timeout) to TCP-based servers
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  btesos_TcpTimedConnector: blocking 'btesc'-style timed channel allocator.
//
//@SEE_ALSO: btesos_tcptimedacceptor btesos_tcptimedchannel
//
//@DESCRIPTION: This component provides a synchronous connector to TCP-based
// servers that adheres to 'btesc_TimedChannelAllocator' protocol.  Both timed
// and non-timed (synchronous) channels can be allocated in a timed and
// non-timed fashion as indicated by the following table:
//
//       +=============================================================+
//       |  Result/Operation |        Timed         |    Non-Timed     |
//       +-------------------------------------------------------------+
//       |      Timed        | 'timedAllocateTimed' | 'allocateTimed'  |
//       +-------------------------------------------------------------+
//       |    Non-Timed      |    'timedAllocate'   |    'allocate'    |
//       +=============================================================+
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
///Performance
///-----------
// Connector is optimized for operations with the timeout.  Non-timed
// operations will have worse performance than their respective counterparts in
// the non-timed version of the connector (i.e., 'btesos_tcpconnector').  If
// timed allocations are not required, 'btesos_TcpConnector' should be used
// instead.
//
///Usage
///-----
// The following usage example shows a possible implementation of an echo
// client (see 'btesos_tcptimedacceptor' for an echo server).  An echo server
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
// First, create a concrete socket factory that is used to manage stream
// sockets:
//..
//    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
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
//    bteso_IPv4Address serverAddress(SERVER_IP, ECHO_PORT);
//
//    bdet_TimeInterval connectTimeout(120, 0);      // 2 minutes
//..
// Now, create a connector and set the configuration parameters:
//..
//   btesos_TcpTimedConnector connector(&factory); // Use default allocator
//   assert(0 == connector.isInvalid());
//   assert(0 == connector.setPeer(serverAddress));// 'serverAddress' is valid.
//   assert(0 == connector.isInvalid());
//..
// Set communication parameters for the channel:
//..
//   enum { READ_SIZE = 10 };                // only for demo
//   bdet_TimeInterval readTimeout(1.0);     // 1 second
//   bdet_TimeInterval writeTimeout(30.0);   // 30 seconds
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
//   btes_TimedChannel *channel = connector.timedAllocateTimed(
//                                   &status,
//                                   bdetu_SystemTime::now() + connectTimeout);
//
//   if (!channel) {
//       assert(0 >= status);     // Async.  interrupts are *not* enabled.
//       if (status) {
//           bsl::cout << "Failed to connect to the peer." << bsl::endl;
//       }
//       else {
//           bsl::cout << "Connection attempt has timed out." << bsl::endl;
//       }
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
//                                     bdetu_SystemTime::now() + writeTimeout);
//       if (PACKET_SIZE != writeStatus) {
//           bsl::cout << "Failed to send data." << bsl::endl;
//           break;
//       }
//       int readStatus = channel->timedRead(
//                                      receivePacket,
//                                      PACKET_SIZE,
//                                      bdetu_SystemTime::now() + readTimeout);
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

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BTESC_TIMEDCHANNELALLOCATOR
#include <btesc_timedchannelallocator.h>
#endif

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class btesc_TimedChannel;
class btesc_Channel;

template<class ADDRESS> class bteso_StreamSocketFactory;
template<class ADDRESS> class bteso_StreamSocket;

/*
template<> class bteso_StreamSocketFactory<bteso_IPv4Address>;
template<> class bteso_StreamSocket<bteso_IPv4Address>;
*/
class bdet_TimeInterval;
                        // ==============================
                        // class btesos_TcpTimedConnector
                        // ==============================

class btesos_TcpTimedConnector : public btesc_TimedChannelAllocator {
    // This class implements a 'btesc'-style timed channel allocator for
    // client-side (i.e., "connected") sockets.  The allocations can be timed
    // or not-timed and produce either timed or non-timed channels.  The return
    // status for any operation mandated by 'btesc_TimedChannelAllocator'
    // protocol is negative for a failure, 0 on timeout, and positive for a
    // (restartable) interrupt on an underlying system call.  This connector
    // provides the ability to change the end-points at run-time without any
    // effect on the state of any channels currently managed by this connector.

    bdema_Pool                                    d_pool;
        // memory pool for channels

    bsl::vector<btesc_Channel*>                   d_channels;
        // managed channels

    bteso_StreamSocketFactory<bteso_IPv4Address> *d_factory_p;
        // factory used to supply sockets

    bteso_IPv4Address                             d_peerAddress;
        // current address of the server

    int                                           d_isInvalidFlag;
        // a flag to be set if this acceptor is invalid

  private:
    btesos_TcpTimedConnector(const btesos_TcpTimedConnector&); // not impl.
    btesos_TcpTimedConnector&
            operator=(const btesos_TcpTimedConnector&);        // not impl.
  public:
    // CREATORS
    btesos_TcpTimedConnector(
            bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
            bslma_Allocator                              *basicAllocator = 0);
        // Create a timed connector that uses the specified 'factory' to create
        // stream sockets.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined if 'factory'
        // is 0.  Note that connector is created NOT in an invalid state (as
        // reported by 'isInvalid' method), though subsequent allocations will
        // *fail* until the peer address is specified (using 'setPeer').

    btesos_TcpTimedConnector(
             bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
             int                                           numElements,
             bslma_Allocator                              *basicAllocator = 0);
        // Create a timed connector that uses the specified 'factory' to create
        // stream sockets with enough internal capacity to accommodate up to
        // the specified 'numElements' channels without reallocation.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined if 'factory' is 0 or
        // 0 >= numElements.  Note that connector is created NOT in an invalid
        // state (as reported by the 'isInvalid' method), though subsequent
        // allocations will *fail* until the peer is specified (using
        // 'setPeer').

    ~btesos_TcpTimedConnector();
        // Destroy this connector.  The behavior is undefined if there are any
        // channels being managed.

    // MANIPULATORS
    btesc_Channel *allocate(int *status, int flags = 0);
        // Allocate a stream-based channel.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT',
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

    btesc_TimedChannel *allocateTimed(int *status, int flags = 0);
        // Allocate a stream-based timed channel.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT',
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

    void deallocate(btesc_Channel *channel);
        // Reclaim all afforded services for the specified 'channel'.  The
        // behavior is undefined unless 'channel' is currently allocated from
        // this allocator, (i.e., was previously obtained from this instance
        // and has not subsequently been deallocated).

    void invalidate();
        // Place this allocator in a permanently invalid state.  No subsequent
        // allocations will succeed.  Note that invalidating the allocator has
        // no effect on the state of any channel managed by it.

    void setPeer(const bteso_IPv4Address& endpoint);
        // Set the address of the peer server with which this connector is
        // associated to the specified 'endpoint'.

    btesc_Channel *timedAllocate(int                      *status,
                                 const bdet_TimeInterval&  timeout,
                                 int                       flags = 0);
        // Allocate a stream-based channel or interrupt after the specified
        // absolute 'timeout' time is reached.  If the optionally specified
        // 'flags' incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Return the address of a channel
        // on success, and 0 otherwise.  On an unsuccessful allocation, load
        // the specified 'status' with 0 if 'timeout' interrupted this
        // operation, a positive value if the interruption was due to an
        // asynchronous event, and a negative value (indicating an error)
        // otherwise; 'status' is not modified on success.  A non-null channel
        // address will remain valid until deallocated explicitly (see
        // 'deallocate').  An allocation that fails with a non-negative status
        // is likely to succeed if retried.  An allocation error does not
        // *necessarily* invalidate this allocator, and so subsequent
        // allocations *may* succeed.  Use the 'isInvalid' method for more
        // information on the state of this allocator.  Note that if the
        // specified 'timeout' value has already passed, the allocation will
        // still be attempted, but the attempt will not block.  Note that -2 is
        // loaded into 'status' if the peer is not set (see 'setPeer').

    btesc_TimedChannel *timedAllocateTimed(int                     *status,
                                           const bdet_TimeInterval& timeout,
                                           int                      flags = 0);
        // Allocate a stream-based timed channel or interrupt after the
        // specified absolute 'timeout' time is reached.  If the optionally
        // specified 'flags' incorporates 'btesc_Flag::BTESC_ASYNC_INTERRUPT',
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
        // specified 'timeout' value has already passed, the allocation will
        // still be attempted, but the attempt will not block.  Note that -2 is
        // loaded into 'status' if the peer is not set (see 'setPeer').

    // ACCESSORS
    const bteso_IPv4Address& peer() const;
        // Return the (fully qualified) address of the peer server associated
        // with this connector, or an invalid address of (ANY_ADDRESS,
        // ANY_PORT) if no such server exists (see 'bteso_IPv4address').

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

inline
void btesos_TcpTimedConnector::setPeer(const bteso_IPv4Address& endpoint)
{
    d_peerAddress = endpoint;
}

inline
void btesos_TcpTimedConnector::invalidate()
{
    d_isInvalidFlag = 1;
}

inline
const bteso_IPv4Address& btesos_TcpTimedConnector::peer() const
{
    return d_peerAddress;
}

inline
int btesos_TcpTimedConnector::numChannels() const
{
    return static_cast<int>(d_channels.size());
}

inline
int btesos_TcpTimedConnector::isInvalid() const
{
    return d_isInvalidFlag;
}

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
