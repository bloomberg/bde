// btlsc_channelallocator.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSC_CHANNELALLOCATOR
#define INCLUDED_BTLSC_CHANNELALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide protocol for blocking stream-based channel allocators.
//
//@CLASSES:
//  btlsc::ChannelAllocator: blocking stream channel allocator
//
//@SEE_ALSO: btlsc_cbchannelallocator
//
//@DESCRIPTION: This component provides a class, 'btlsc::ChannelAllocator',
// that defines an abstract interface for a synchronous (i.e., blocking)
// mechanism that allocates and deallocates synchronous channels having timeout
// capability; the allocation itself does *not* have timeout capability.  Each
// channel is an end point of a bi-directional stream-based communication
// connection to a peer; connection details, such as who the peer is, whether
// there is indeed one peer or multiple similar peers, and how the connection
// came to be, are not relevant to this channel-allocator protocol, and are
// therefore abstracted.
//
// In particular, 'btlsc::ChannelAllocator' can allocate channels of type
// 'btlsc::Channel' and 'btlsc::TimedChannel', i.e., untimed and timed blocking
// channels.
//
///Protocol Hierarchy
///------------------
// 'btlsc::ChannelAllocator' forms the base of an interface hierarchy; other
// interfaces may be defined by direct public inheritance:
//..
//                     ,-----------------------.
//                    ( btlsc::ChannelAllocator )
//                     `-----------------------'
//..
//
///Channel Allocation
///------------------
// This protocol establishes methods for allocating synchronous, stream-based
// channels.  Each allocation method blocks until (1) "success" -- the
// requested channel is allocated and its address returned to the user (in
// which case the 'status' argument is not modified), (2) "interrupted" -- the
// operation was interrupted (e.g., via an asynchronous event), the channel
// address is null, and 'status' is set to a non-negative value (see below), or
// (3) "error" -- an implementation-dependent error occurred, the channel
// address is null, and 'status' is less than -1.  The user may retry
// interrupted operations with a reasonable expectation of success.  An "error"
// status implies that the allocation is unlikely to succeed if retried, but
// does not *necessarily* invalidate the allocator.  The 'isInvalid' method may
// be used to confirm the occurrence of a permanent error.  An allocation can
// *never* succeed once an allocator is invalid.
//
// The meanings of the 'status' value for an unsuccessful allocation (i.e., a
// null channel address) are summarized as follows:
//..
//  "status"    meaning (only when returned channel address is null)
//  --------    ----------------------------------------------------
//  positive    Interruption by an "asynchronous event"
//
//  < -1        Allocation operation unable to succeed at this time
//..
// Note that unless asynchronous events are explicitly enabled (see below),
// they are ignored, and "status" will never be positive.  Also note that, for
// consistency with non-blocking transport, a status of -1 is reserved for a
// "canceled" operation, which is not possible in this blocking protocol, and
// for consistency with timed allocators a status of 0 is reserved for a
// timeout (which also is not supported in this untimed allocator).
//
///Asynchronous Events
///-------------------
// Allocation methods in this protocol anticipate the possible occurrence of an
// "asynchronous event" (AE) during execution.  A common example of an AE is a
// Unix signal, but note that a specific Unix signal, if not detected or
// implemented, *may* not result in an AE.
//
// This interface cannot fully specify either the nature of or the behavior
// resulting from an AE, but certain restrictions can be (and are) imposed.  By
// default, AEs are either ignored or, if that is not possible, cause an error.
// At the user's option, however, a concrete implementation can be authorized
// to return, if such occurrence is detected, with an "interrupted" status
// (leaving the allocator unaffected) upon the occurrence of an AE.  Such
// authorizations are made explicitly by incorporating into the optional
// (trailing) integer 'flags' argument to a method call the
// 'btlsc::Flag::k_ASYNC_INTERRUPT' value.
//
///Usage
///-----
// The purpose of the 'btlsc::ChannelAllocator' protocol is to isolate the act
// of requesting a connection from details such as to whom the connection will
// be to and which side initiated the connection.  In this example we will
// consider both the Server and Client sides of a 'my_Tick' reporting service.
// Since each side of this service could potentially be a library component, we
// do not want to "bake" into either side the details of how connections will
// be established.  It is sufficient that, when a tick needs to be sent or
// received, a channel is obtained, the tick is transmitted, and the channel is
// returned to its allocator.  Note that this example serves to illustrate the
// use of the 'btlsc::ChannelAllocator' and does not represent
// production-quality software.
//..
//  class my_Tick {
//      char   d_name[5];
//      double d_bestBid;
//      double d_bestOffer;
//
//    public:
//      my_Tick() { }
//      my_Tick(const char *ticker);
//      my_Tick(const char *ticker, double bestBid, double bestOffer);
//      ~my_Tick() { assert(d_bestBid > 0); };
//
//      static int maxSupportedBdexVersion(int versionSelector) { return 1; }
//
//      template <class STREAM>
//      STREAM& bdexStreamOut(STREAM& stream, int version) const;
//          // Write this value to the specified output 'stream' using the
//          // specified 'version' format, and return a reference to 'stream'.
//          // If 'stream' is initially invalid, this operation has no effect.
//          // If 'version' is not supported, 'stream' is invalidated but
//          // otherwise unmodified.  Note that 'version' is not written to
//          // 'stream'.  See the 'bslx' package-level documentation for more
//          // information on BDEX streaming of value-semantic types and
//          // containers.
//
//      template <class STREAM>
//      STREAM& bdexStreamIn(STREAM& stream, int version);
//          // Assign to this object the value read from the specified input
//          // 'stream' using the specified 'version' format, and return a
//          // reference to 'stream'.  If 'stream' is initially invalid, this
//          // operation has no effect.  If 'version' is not supported, this
//          // object is unaltered and 'stream' is invalidated but otherwise
//          // unmodified.  If 'version' is supported but 'stream' becomes
//          // invalid during this operation, this object has an undefined, but
//          // valid, state.  Note that no version is read from 'stream'.  See
//          // the 'bslx' package-level documentation for more information on
//          // BDEX streaming of value-semantic types and containers.
//
//      void print(bsl::ostream& stream) const;
//  };
//
//  my_Tick::my_Tick(const char *ticker)
//  : d_bestBid(0)
//  , d_bestOffer(0)
//  {
//     snprintf(d_name, sizeof d_name, "%s", ticker);
//  }
//
//  my_Tick::my_Tick(const char *ticker, double bestBid, double bestOffer)
//  : d_bestBid(bestBid)
//  , d_bestOffer(bestOffer)
//  {
//      snprintf(d_name, sizeof d_name, "%s", ticker);
//  }
//
//  void my_Tick::print(bsl::ostream& stream) const
//  {
//      stream << "(" << d_name << ", " << d_bestBid << ", " << d_bestOffer
//             << ")" << endl;
//  }
//
//  inline
//  bsl::ostream& operator<<(bsl::ostream& stream, const my_Tick& tick)
//  {
//      tick.print(stream);
//      return stream;
//  }
//
//  template <class STREAM>
//  STREAM& my_Tick::bdexStreamOut(STREAM& stream, int version) const
//  {
//      switch (version) {
//        case 1: {
//          stream.putString(d_name);
//          stream.putFloat64(d_bestBid);
//          stream.putFloat64(d_bestOffer);
//        } break;
//        default: {
//          stream.invalidate();
//        } break;
//      }
//      return stream;
//  }
//
//  template <class STREAM>
//  STREAM& my_Tick::bdexStreamIn(STREAM& stream, int version)
//  {
//      switch (version) {
//        case 1: {
//          bsl::string temp1;
//          stream.getString(temp1);
//          int maxLen = sizeof d_name - 1;  // the valid name length
//          int len    = temp1.length();
//          if (len < maxLen) {
//              strcpy(d_name, temp1.c_str());
//          }
//          else {
//              strncpy(d_name, temp1.c_str(), len);
//              d_name[len] = 0;
//          }
//          stream.getFloat64(d_bestBid);
//          stream.getFloat64(d_bestOffer);
//        } break;
//        default: {
//          stream.invalidate();
//        } break;
//      }
//      return stream;
//  }
//
//..
// Let's also assume that we have a function that knows how to print platform
// neutral encodings of type 'my_Tick':
//..
//  static void myPrintTick(bsl::ostream& stream, const char *buffer, int len)
//      // Print the value of the specified 'buffer' interpreted as a
//      // BDEX byte-stream representation of a 'my_Tick' value, to the
//      // specified 'stream' or report an error to 'stream' if 'buffer' is
//      // determined *not* to hold an encoding of a valid 'my_Tick' value.
//  {
//      my_Tick tick;
//      bslx::ByteInStream input(buffer, len);
//      input >> tick;
//
//      stream << tick;
//  }
//..
//
///Server Side
///- - - - - -
// Let's assume that we have a function that knows how to print platform
// neutral encodings of type 'my_Tick':
//..
//  #define VERSION_SELECTOR 20140601
//
//  static int calculateMyTickMessageSize()
//      // Calculate and return the number of bytes in a BDEX byte-stream
//      // encoding of a (dummy) 'my_Tick' value (called just once, see below).
//  {
//      my_Tick dummy;
//      bslx::ByteOutStream bos(VERSION_SELECTOR);
//      bos << dummy;
//      return bos.length();
//  }
//
//  static int myTickMessageSize()
//      // Return the number of bytes in a BDEX byte-stream encoding of a
//      // 'my_Tick' value without creating a runtime-initialized file-scope
//      // static variable (which is link-order dependent).
//  {
//      static const int MESSAGE_SIZE = calculateMyTickMessageSize();
//      return MESSAGE_SIZE;
//  }
//..
// The following class illustrates how we might implement a tick-reporter
// server using just the 'btlsc::ChannelAllocator' and 'btlsc::Channel'
// protocols.
//..
//  class my_TickReporter {
//      // This class implements a server that accepts connections, extracts
//      // from each connection a single 'my_Tick' value, and reports that
//      // value to a console stream.  Both the acceptor and console stream
//      // are supplied at construction.
//
//      btlsc::ChannelAllocator *d_acceptor_p;  // incoming connections
//      bsl::ostream&            d_console;     // where to put tick info
//
//    private:
//      // NOT IMPLEMENTED
//      my_TickReporter(const my_TickReporter&);
//      my_TickReporter& operator=(const my_TickReporter&);
//
//    public:
//      my_TickReporter(bsl::ostream&            console,
//                      btlsc::ChannelAllocator *acceptor);
//          // Create a synchronous tick-reporter using the specified
//          // 'acceptor' to establish incoming client connections, each
//          // transmitting a single 'my_Tick' value; write these values to
//          // the specified 'console' stream.  If the 'acceptor' is idle for
//          // more than five minutes, print a message to the 'console' stream
//          // supplied at construction and continue.  To guard against
//          // malicious clients, a connection that does not produce a tick
//          // value within one minute will be summarily dropped.
//
//      ~my_TickReporter();
//          // Destroy this server object.
//
//      // MANIPULATORS
//      int reportTicker();
//          // Call 'd_acceptor_p's 'allocate' to establish a channel.
//  };
//
//  my_TickReporter::my_TickReporter(bsl::ostream&            console,
//                                   btlsc::ChannelAllocator *acceptor)
//  : d_console(console)
//  , d_acceptor_p(acceptor)
//  {
//      assert(&d_console);
//      assert(d_acceptor_p);
//  }
//
//  my_TickReporter::~my_TickReporter()
//  {
//      assert(&d_console);
//      assert(d_acceptor_p);
//  }
//
//  int my_TickReporter::reportTicker()
//  {
//      int status = 0;
//
//      while (1) {
//          btlsc::Channel *newChannel = d_acceptor_p->allocate(&status);
//
//          if (newChannel) {     // Successfully created a connection.
//              const int numBytes = ::myTickMessageSize();
//
//              const char *buffer = 0;
//              int ret = newChannel->bufferedRead(&status,
//                                                 &buffer,
//                                                 numBytes);
//              if (0 >= ret) {
//                  d_console << "Error: The read operation timeout "
//                               "on this channel."
//                            << bsl::endl;
//                  d_acceptor_p->deallocate(newChannel);
//              }
//              else {  // Read successfully.
//                  ::myPrintTick(d_console, buffer, numBytes);
//                  d_acceptor_p->deallocate(newChannel);
//              }
//          }
//          else if (status > 0) {   // Interrupted by unspecified event.
//              assert(0); // Impossible, "async interrupts" not authorized.
//          }
//          else {         // Allocation operation is unable to succeed.
//              assert(status < 0);
//
//              d_console << "Error: The channel allocator is not working now."
//                        << bsl::endl;
//              assert(0);
//          }
//      }
//  }
//..
// The following is the server driver, which first creates supporting objects
// to construct the 'reporter', and then invokes 'reportTicker' of the
// 'reporter':
//..
//  int main(int argc, const char *argv[])
//  {
//      enum { DEFAULT_PORT = 5000 };
//
//      const int portNumber = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;
//
//      btlso::IPv4Address address(btlso::IPv4Address::ANY, portNumber);
//      btlso::Tcpipv4StreamSocketFactory sf;      // concrete factory
//      btlsos::CbChannelAcceptor acceptor(address, &sf);
//
//      if (acceptor.isInvalid()) {
//           bsl::cout << "Error: Unable to create acceptor." << bsl::endl;
//           return -1;                                               // RETURN
//      }
//
//      my_TickReporter reporter(bsl::cout, &acceptor);
//      reporter.reportTicker();
//      return 0;
//  }
//..
//
///Client Side
///- - - - - -
// In order to use this 'my_Tick' reporting service, clients will need to know
// where such a service resides and how to establish such connections on
// demand.  We will use the 'btlsc::ChannelAllocator' protocol to abstract
// those details out of the stable software that generates (or forwards) ticks.
// For the purposes of this example, let's assume that ticks are generated in
// some ASCII format and arrive in fixed size chunks (e.g., 80 bytes) from a
// separate process.  The client will ask the "allocator" instance for a new
// channel to send each tick until all ticks are sent.
//..
//  static
//  int parseTick(my_Tick *result, const char *buffer, int numBytes)
//      // Parse a tick value from the specified input 'buffer' of the
//      // specified 'numBytes' and load that value into the specified
//      // 'result'.  Return 0 on success, and a non-zero value (with no effect
//      // on 'result') otherwise.  The behavior is undefined unless
//      // '0 <= numBytes'.
//  {
//      // ...
//
//      return 0;
//  }
//
//  class my_TickerplantSimulator {
//      // Accept raw tick values in ASCII sent as fixed-sized packets via a
//      // single 'btlsc::Channel' and send them synchronously one by one to a
//      // peer (or similar peers) connected via channels provided via a
//      // 'btlsc::ChannelAllocator'.  Both the output channel allocator and
//      // the input channel are supplied at construction.
//
//      enum { MAX_PARSER_ERRORS = 3 };
//
//      btlsc::ChannelAllocator *d_connector_p;       // outgoing connections
//      btlsc::Channel          *d_input_p;           // incoming packets
//      bsl::ostream&            d_console;           // where to write errors
//      const int                d_inputSize;         // input packet size
//      int                      d_parserErrorCount;  // consecutive errors
//
//    private:
//      // NOT IMPLEMENTED
//      my_TickerplantSimulator(const my_TickerplantSimulator&);
//      my_TickerplantSimulator& operator=(const my_TickerplantSimulator&);
//
//    public:
//      my_TickerplantSimulator(bsl::ostream&            console,
//                              btlsc::ChannelAllocator *connector,
//                              btlsc::Channel          *input,
//                              int                      inputSize);
//          // Create a non-blocking ticker-plant simulator using the
//          // specified 'input' channel to read ASCII tick records of the
//          // specified 'inputSize' and convert each record to a 'My_Tick'
//          // structure; each tick value is sent synchronously to a peer via a
//          // distinct channel obtained from the specified 'connector',
//          // reporting any errors to the specified 'console'.  If 'connector'
//          // fails or is unable to succeed after 30 seconds, or if
//          // transmission itself exceeds 10 seconds, display a message on
//          // 'console' and abort the transmission.  If three successive reads
//          // of the input channel fail to produce a valid ticks, invalidate
//          // the channel and shut down this simulator.  The behavior is
//          // undefined unless '0 < inputSize'.
//
//      int sendTicker();
//          // Build 'My_Tick' objects by reading from channel of 'd_input_p'
//          // and send the built tick, if it is valid, to the peer.  Return 0
//          // on success, and a negative value otherwise.
//
//      ~my_TickerplantSimulator();
//          // Destroy this simulator object.
//  };
//
//  my_TickerplantSimulator::my_TickerplantSimulator(
//                                          bsl::ostream&            console,
//                                          btlsc::ChannelAllocator *connector,
//                                          btlsc::Channel          *input,
//                                          int                      inputSize)
//  : d_connector_p(connector)
//  , d_console(console)
//  , d_input_p(input)
//  , d_inputSize(inputSize)
//  , d_parserErrorCount(0)
//  {
//      assert(&console);
//      assert(connector);
//      assert(input);
//      assert(0 < inputSize);
//  }
//
//  int my_TickerplantSimulator::sendTicker()
//  {
//      const char *buffer    = 0;
//      int         status    = 0;
//      int         augStatus = 0;
//
//      while (0 < d_input_p->bufferedRead(&augStatus, &buffer, d_inputSize)) {
//          my_Tick tick;
//          if (0 == ::parseTick(&tick, buffer, d_inputSize)) {
//              d_parserErrorCount = 0;  // Successfully parsed tick value.
//
//              // Initiate a non-blocking  allocate operation.
//
//              btlsc::Channel *newChannel = d_connector_p->allocate(&status);
//              if (0 == newChannel) {
//                  d_console << "Error: Unable to install connect operation."
//                            << bsl::endl;
//              }
//              else {
//                  bslx::ByteOutStream bos(VERSION_SELECTOR);
//                  bos << tick;
//                  int msgSize = bos.length();
//                  if (0 >= newChannel->write(bos.data(), msgSize)) {
//                       d_console << "Error: Unable even to register a write"
//                                    " operation on this channel."
//                                 << bsl::endl;
//                  }
//                  d_connector_p->deallocate(newChannel);
//              }
//          }
//          else {  // parsing error
//              d_console << "Error: Unable to parse tick data!" << bsl::endl;
//
//              ++d_parserErrorCount;
//
//              if (d_parserErrorCount >= MAX_PARSER_ERRORS) {
//                  d_console << "Reached maximum parser error limit;"
//                               " invalidating input channel." << bsl::endl;
//
//                  d_input_p->invalidate();
//              }
//              else {
//                  d_console << "Continuing with next record." << bsl::endl;
//              }
//          }
//      }
//
//      if (-1 == augStatus) {  // EOF
//          return 0;                                                 // RETURN
//      }
//      else {
//          return -1;                                                // RETURN
//      }
//  }
//
//  my_TickerplantSimulator::~my_TickerplantSimulator()
//  {
//      assert(&d_console);
//      assert(d_connector_p);
//      assert(0 < d_inputSize);
//      assert(0 <= d_parserErrorCount);
//      assert(d_parserErrorCount <= MAX_PARSER_ERRORS);
//  }
//..
// In order to make this 'my_TickerplantSimulator' work, we will need to create
// those supporting objects first, then simply call 'sendTicker' of the
// "simulator":
//..
//  int main(int argc, const char *argv[])
//  {
//      const char *const DEFAULT_HOST = "widget";
//      enum { DEFAULT_PORT = 5000 };
//      enum { DEFAULT_SIZE = 80 };
//
//      const char *hostName = argc > 1 ? argv[1]       : DEFAULT_HOST;
//      const int portNumber = argc > 2 ? atoi(argv[2]) : DEFAULT_PORT;
//      const int inputSize  = argc > 3 ? atoi(argv[3]) : DEFAULT_SIZE;
//
//      btlso::IPv4Address address;
//      btlso::ResolveUtil<btlso::IPv4Address>::getAddress(&address, hostName);
//      address.setPortNumber(portNumber);
//      btlso::InetStreamSocketFactory sf;
//      btlsos::CbChannelConnector connector(address, &sf);
//
//      my_TickerplantSimulator simulator(bsl::cout,
//                                        &connector,
//                                        input,
//                                        inputSize);
//      assert(0 == simulator.sendTicker());
//
//      return 0;
//  }
//..
// Please remember that these example code snippets are intended to illustrate
// the use of 'btlsc::ChannelAllocator' and do not represent production-quality
// software.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

namespace BloombergLP {
namespace btlsc {

class Channel;
class TimedChannel;

                           // ======================
                           // class ChannelAllocator
                           // ======================

class ChannelAllocator {
    // This class is a protocol (pure abstract interface) for a (synchronous)
    // mechanism that allocates end points of communication channels supporting
    // timed and untimed synchronous (buffered and non-buffered) read and write
    // operations on a byte stream.  A successful allocation returns the
    // address of a channel and leaves 'status' unmodified.  If the channel
    // address is null, then a non-negative status indicates an interruption,
    // while a negative status indicates an inability to succeed at the present
    // time, but does not guarantee that the allocator is invalid.  An invalid
    // allocator cannot successfully allocate a channel.

  protected:
    // CREATORS
    virtual ~ChannelAllocator();
        // Destroy this object.

  public:
    // MANIPULATORS
    virtual Channel *allocate(int *status, int flags = 0) = 0;
        // Allocate a stream-based channel.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Return the address of a channel
        // on success, and null otherwise.  On an unsuccessful allocation, load
        // the specified 'status' with a positive value if an asynchronous
        // event interrupted the allocation, and with a value less than -1
        // (indicating an error) otherwise; 'status' is not modified on
        // success.  A non-null channel address will remain valid until
        // deallocated explicitly (see 'deallocate').  An allocation that fails
        // with a positive status is likely to succeed if retried.  An
        // allocation error does not *necessarily* invalidate this allocator,
        // and so subsequent allocations *may* succeed.  Use the 'isInvalid'
        // method for more information on the state of this allocator.  Note
        // that status values of 0 and -1 are not possible for this method.

    virtual TimedChannel *allocateTimed(int *status, int flags = 0) = 0;
        // Allocate a stream-based timed channel.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
        // "asynchronous events" are permitted to interrupt this operation; by
        // default, such events are ignored.  Return the address of a timed
        // channel on success, and null otherwise.  On an unsuccessful
        // allocation, load the specified 'status' with a positive value if an
        // asynchronous event interrupted the allocation, and with a value less
        // than -1 (indicating an error) otherwise; 'status' is not modified on
        // success.  A non-null channel address will remain valid until
        // deallocated explicitly (see 'deallocate').  An allocation that fails
        // with a positive status is likely to succeed if retried.  An
        // allocation error does not *necessarily* invalidate this allocator,
        // and so subsequent allocations *may* succeed.  Use the 'isInvalid'
        // method for more information on the state of this allocator.  Note
        // that status values of 0 and -1 are not possible for this method.

    virtual void deallocate(Channel *channel) = 0;
        // Reclaim all afforded services for the specified 'channel'.  The
        // behavior is undefined unless 'channel' is currently allocated from
        // this allocator, (i.e., was previously obtained from this instance
        // and has not subsequently been deallocated).

    virtual void invalidate() = 0;
        // Place this allocator in a permanently invalid state.  No subsequent
        // allocation operations will succeed.  Note that invalidating the
        // allocator has no effect on the state of any channel managed by it
        // nor on the listening socket, which, if established, must be closed
        // explicitly (using the 'close' method).

    // ACCESSORS
    virtual int isInvalid() const = 0;
        // Return a non-zero value if this channel allocator is *permanently*
        // invalid (i.e., no subsequent allocation requests will succeed), and
        // 0 otherwise.
};

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
