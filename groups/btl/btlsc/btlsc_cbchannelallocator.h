// btlsc_cbchannelallocator.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSC_CBCHANNELALLOCATOR
#define INCLUDED_BTLSC_CBCHANNELALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide protocol for non-blocking stream-based channel allocators.
//
//@CLASSES:
//  btlsc::CbChannelAllocator: non-blocking stream-based channel allocator
//
//@SEE_ALSO: btlsc_channelallocator
//
//@DESCRIPTION: This component provides a class, 'btlsc::CbChannelAllocator',
// that defines an abstract interface for a non-blocking mechanism that
// allocates and deallocates non-blocking channels with and without timeout
// capability; the allocation itself does *not* have timeout capability.  Each
// channel allocated through this interface is an end point of a bi-directional
// stream-based communication connection to a peer; connection details, such as
// who the peer is, whether there is indeed one peer or multiple similar peers,
// and how the connection came to be, are not relevant to this
// channel-allocator protocol, and are therefore abstracted.
//
// In particular, 'btlsc::CbChannelAllocator' can allocate non-blocking
// channels of type 'btlsc::CbChannel' and 'btlsc::TimedCbChannel', i.e.,
// untimed and timed non-blocking channels.
//
///Protocol Hierarchy
///------------------
// 'btlsc::CbChannelAllocator' forms the base of an interface hierarchy; other
// interfaces may be defined by direct public inheritance:
//..
//                     ,-------------------------.
//                    ( btlsc::CbChannelAllocator )
//                     `-------------------------'
//..
//
///Non-Blocking Channel Allocation
///-------------------------------
// This protocol establishes methods for allocating non-blocking stream-based
// channels in a non-blocking manner; each method registers a user-supplied
// callback function object (functor) and returns immediately.  A successful
// return status implies that the registered callback will be invoked (and,
// conversely, an unsuccessful status implies otherwise).  The callback, in
// turn, will communicate the results of the registered allocation attempt.
//
// Enqueued allocation operations proceed asynchronously to one of three
// possible results: (1) "success" -- the requested channel is allocated and
// its address is passes to the callback function: in this case the callback
// "status" argument has no meaning, (2) "interrupted" -- the operation was
// interrupted (e.g., via an asynchronous event): the channel address is null
// and the (non-negative) status conveys the nature of the interruption, (3)
// "canceled" -- the operation was aborted (synchronously) by an explicit call
// to 'cancelAll': the channel address in null and the "status" is -1, or (4)
// "error" -- an implementation-dependent error occurred: the channel address
// is null and status is less than -1.  The user may retry interrupted and
// canceled operations with a reasonable expectation of success.  An "error"
// status implies that the allocation is unlikely to succeed if retried, but
// does not necessarily invalidate the allocator.  The 'isInvalid' method may
// be used to confirm the occurrence of a permanent error.  If the allocator is
// valid, an allocation request will be enqueued and may succeed.  Otherwise,
// the allocation request itself will not succeed.
//
// The meanings of the callback function status value for an unsuccessful
// allocation (i.e., a null channel address) are summarized as follows:
//..
//  "status"    meaning (only when returned channel address is null)
//  --------    ---------------------------------------------------------
//  positive    Interruption by an "asynchronous event"
//
//  -1          Operation explicitly canceled (synchronously) by the user
//
//  < -1        Allocation operation unable to succeed at this time
//..
// Note that unless asynchronous events are explicitly enabled (see below),
// they are ignored, and "status" will never be positive.  Also note that
// whether the callback is invoked before or after the registering method
// returns is not specified; in either case, if the registration was
// successful, then the return value will reflect success.  Finally, note that,
// for consistency with timed allocators, a status of 0 is reserved for a
// timeout (which is not supported in this untimed allocator).
//
///Callback Functor Registration
///- - - - - - - - - - - - - - -
// Once an operation is successfully initiated, a (reference-counted) copy of
// the ('bdlf') callback functor is retained by the concrete allocator until
// the callback is executed.  Therefore, the user need not be concerned with
// preserving any resources used by the callback.
//
///Asynchronous Events
///-------------------
// Allocation methods in this protocol anticipate the possible occurrence of an
// "asynchronous event" (AE) during execution.  A common example of an AE is a
// Unix signal, but note that a specific Unix signal *may* not result in an AE,
// and an AE is certainly not limited to signals, even on Unix platforms.
//
// This interface cannot fully specify either the nature of or the behavior
// resulting from an AE, but certain restrictions can be (and are) imposed.  By
// default, AEs are either ignored or, if that is not possible, cause an error.
// At the user's option, however, a concrete implementation can be authorized
// to return, if possible, with an "interrupted" status (leaving the allocator
// unaffected) upon the occurrence of an AE.  Such authorizations are made
// explicitly by incorporating into the optional (trailing) integer 'flags'
// argument to a method call the 'btlsc::Flag::k_ASYNC_INTERRUPT' value.
//
///Usage
///-----
// The purpose of the 'btlsc::CbChannelAllocator' protocol is to isolate the
// act of requesting a connection from details such as to whom the connection
// will be to and which side initiated the connection.  In this example we will
// consider both the Server and Client sides of a 'my_Tick' reporting service.
// Since each side of this service could potentially be a library component, we
// do not want to embed into either side the details of how connections will be
// established.  It is sufficient that, when a tick needs to be sent or
// received, a channel is obtained, the tick is transmitted, and the channel is
// returned to its allocator.  Note that this example serves to illustrate the
// use of the 'btlsc::CbChannelAllocator' and does not represent
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
// The following class illustrates how we might implement a tick-reporter
// server using just the 'btlsc::CbChannelAllocator' and
// 'btlsc::TimedCbChannel' protocols.  In this implementation the "allocate"
// functor (but not the "read" functor) is created in the constructor and
// cached for repeated use.  Note that buffered reads avoid having to supply a
// buffer, and *may* improve throughput if connections are preserved (pooled)
// in the particular *concrete* channel allocator (supplied at construction):
//..
//  class my_TickReporter {
//      // This class implements a server that accepts connections, extracts
//      // from each connection a single 'my_Tick' value, and reports that
//      // value to a console stream; both the acceptor and console stream are
//      // supplied at construction.
//
//      btlsc::CbChannelAllocator *d_acceptor_p;    // incoming connections
//      bsl::ostream&              d_console;       // where to put tick info
//
//      btlsc::TimedCbChannel::TimedCallback
//                                 d_allocFunctor;  // reused
//
//    private:
//      void acceptCb(btlsc::TimedCbChannel     *clientChannel,
//                    int                        status,
//                    const bsls::TimeInterval&  timeout);
//          // Called when a new client channel has been accepted.
//          // ...
//
//      void readCb(const char            *buffer,
//                  int                    status,
//                  int                    asyncStatus,
//                  btlsc::TimedCbChannel *clientChannel);
//          // Called when a 'my_Tick' value has been read from the channel.
//          // ...
//
//    private:
//      // NOT IMPLEMENTED
//      my_TickReporter(const my_TickReporter&);
//      my_TickReporter& operator=(const my_TickReporter&);
//
//    public:
//      my_TickReporter(bsl::ostream&              console,
//                      btlsc::CbChannelAllocator *acceptor);
//          // Create a non-blocking tick-reporter using the specified
//          // 'acceptor' to establish incoming client connections, each
//          // transmitting a single 'my_Tick' value; write these values to the
//          // specified 'console' stream.  If the 'acceptor' is idle for more
//          // than five minutes, print a message to the 'console' stream
//          // supplied at construction and continue.  To guard against
//          // malicious clients, a connection that does not produce a tick
//          // value within one minute will be summarily dropped.
//
//      ~my_TickReporter();
//          // Destroy this server object.
//  };
//
//  #define VERSION_SELECTOR 20140601
//
//  const double ACCEPT_TIME_LIMIT = 300;               // 5 minutes
//  const double   READ_TIME_LIMIT =  60;               // 1 minutes
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
//
//  void my_TickReporter::acceptCb(btlsc::TimedCbChannel     *clientChannel,
//                                 int                        status,
//                                 const bsls::TimeInterval&  timeout))
//  {
//      if (clientChannel) {     // Successfully created a connection.
//
//          const int                numBytes = ::myTickMessageSize();
//          const bsls::TimeInterval now      = bdlt::CurrentTime::now();
//
//          // Create one-time (buffered) read functor holding 'clientChannel'.
//
//          using namespace bdlf::PlaceHolders;
//          btlsc::TimedCbChannel::BufferedReadCallback readFunctor(
//              bdlf::BindUtil::bind(
//                       bdlf::MemFnUtil::memFn(&my_TickReporter::readCb, this)
//                     , _1, _2, _3
//                     , clientChannel));
//
//          // Install read callback (timeout, but no raw or async interrupt).
//
//          if (clientChannel->timedBufferedRead(numBytes,
//                                               now + READ_TIME_LIMIT,
//                                               readFunctor)) {
//              d_console << "Error: Unable even to register a read operation"
//                           " on this channel." << bsl::endl;
//              d_acceptor_p->deallocate(clientChannel);
//          }
//      }
//      else if (0 == status) {  // Not possible for this untimed allocator.
//          assert(status)       // Might as well abort.
//      }
//      else if (status > 0) {   // Interrupted by unspecified event.
//          assert(0); // Impossible, "async interrupts" were not authorized.
//      }
//      else {                   // Allocation operation is unable to succeed.
//          assert(status < 0);
//
//          d_console << "Error: The channel allocator is not working now."
//                    << bsl::endl;
//
//          // Note that attempting to re-register an allocate operation below
//          // will fail only if the channel allocator is permanently disabled.
//      }
//
//      // In all cases, attempt to reinstall the (reusable) accept callback.
//
//      if (d_acceptor_p->allocateTimed(d_allocFunctor)) {
//          d_console << "Error: unable to register accept operation."
//                    << bsl::endl;
//          // This server is broken.
//      }
//  }
//
//  void my_TickReporter::readCb(const char            *buffer,
//                               int                    status,
//                               int                    asyncStatus,
//                               btlsc::TimedCbChannel *clientChannel)
//  {
//      assert(clientChannel);
//
//      const int msgSize = ::myTickMessageSize();
//
//      if (msgSize == status) {  // Encoded-tick value read successfully.
//          assert(buffer);
//
//          ::myPrintTick(d_console, buffer, msgSize);
//      }
//      else if (0 <= status) {   // Tick message was interrupted.
//
//          assert(buffer); // Data in buffer is available for inspection (but
//                          // remains in the channel's buffer).
//
//          // Must be a timeout event since neither raw (partial) reads nor
//          // (external) asynchronous interrupts were authorized.
//
//          assert(0 == asyncStatus);   // must be timeout event!
//
//          d_console << "Error: Unable to read tick value from channel"
//                       " before timing out; read aborted." << bsl::endl;
//      }
//      else { // Tick-message read failed.
//          assert(0 > status);
//
//          d_console << "Error: Unable to read tick value from channel."
//                    << bsl::endl;
//      }
//
//      d_acceptor_p->deallocate(clientChannel);
//  }
//
//  my_TickReporter::my_TickReporter(bsl::ostream&              console,
//                                   btlsc::CbChannelAllocator *acceptor)
//  : d_console(console)
//  , d_acceptor_p(acceptor)
//  {
//      assert(&d_console);
//      assert(d_acceptor_p);
//
//      // Attempt to install the first accept callback.
//
//      bsls::TimeInterval timeout =
//                     bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)
//                     + ACCEPT_TIME_LIMIT;
//
//      // load reusable allocate functor
//
//      using namespace bdlf::PlaceHolders;
//      d_allocFunctor = bdlf::BindUtil::bind(
//                     bdlf::MemFnUtil::memFn(&my_TickReporter::acceptCb, this)
//                   , _1, _2
//                   , timeout);
//
//      if (d_acceptor_p->allocateTimed(d_allocFunctor)) {
//          d_console << "Error: Unable to install accept operation."
//                    << bsl::endl;
//          // This server is broken.
//      }
//  }
//
//  my_TickReporter::~my_TickReporter()
//  {
//      assert(&d_console);
//      assert(d_acceptor_p);
//  }
//..
// In order to make this 'my_TickReporter' work, we will need to instantiate a
// concrete (e.g., socket) callback channel allocator that is hooked up to an
// appropriate event manager.
//..
//  int main(int argc, const char *argv[])
//  {
//      enum { DEFAULT_PORT = 5000 };
//
//      const int portNumber = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;
//
//      btlso::IPv4Address address(btlso::IPv4Address::ANY, portNumber);
//      btlso::InetTimedSocketEventManager manager;  // concrete manager
//      btlso::Tcpipv4StreamSocketFactory sf;        // concrete factory
//      btlsos::TimedCbChannelAcceptor acceptor(address, &sf, &manager);
//
//      if (acceptor.isInvalid()) {
//           bsl::cout << "Error: Unable to create acceptor" << bsl::endl;
//           return -1;                                               // RETURN
//      }
//
//      my_TickReporter reporter(bsl::cout, &acceptor);
//
//      while (0 != manager.dispatch()) {
//           // Do nothing.
//      }
//
//      // Done only when there are no more events to process.
//
//      return 0;
//  }
//..
// Note that when the server can go out of scope before all events are
// processed, it is important to implement it using the envelope/letter pattern
// where a *counted* *handle* to the internal letter representation is present
// in each active callback functor to preserve the server's internal state
// until all functors operating on it have been invoked.
//
///Client Side
///- - - - - -
// In order to use this 'my_Tick' reporting service, clients will need to know
// where such a service resides and how to establish such connections on
// demand.  We will use the 'btlsc::CbChannelAllocator' protocol to abstract
// those details out of the stable software that generates (or forwards) ticks.
// For the purposes of this example, let's assume that ticks are generated in
// some ASCII format and arrive in fixed size chunks (e.g., 80 bytes) from a
// separate process.  Note that of the three callback methods 'readCb',
// 'connectCb', and 'writeCb', only 'readCb' requires no additional,
// call-specific user data; hence we can easily create it once at construction,
// and productively cache it for repeated used.  We will choose to reload the
// others each time (which is admittedly somewhat less efficient).
//..
//  class my_TickerplantSimulator {
//      // Accept raw tick values in ASCII sent as fixed-sized packets via a
//      // single 'btlsc::TimedCbChannel' and send them asynchronously one by
//      // one to a peer (or similar peers) connected via channels provided via
//      // a 'btlsc::CbChannelAllocator'.  Both the output channel allocator
//      // and the input channel are supplied at construction.
//
//      btlsc::CbChannelAllocator
//                            *d_connector_p;       // outgoing connections
//      btlsc::TimedCbChannel *d_input_p;           // incoming packets
//      bsl::ostream&          d_console;           // where to write errors
//      const int              d_inputSize;         // input packet size
//      int                    d_parserErrorCount;  // consecutive errors
//
//      btlsc::TimedCbChannel::BufferedReadCallback d_readFunctor;  // reused
//
//    private:
//      void readCb(const char               *buffer,
//                  int                       status,
//                  int                       asyncStatus);
//          // Called when a fixed-size record has been read from the input.
//          // ...
//
//      void connectCb(btlsc::TimedCbChannel *serverChannel,
//                     int                    status,
//                     const my_Tick&         tick);
//          // Called when a new server channel has been established.
//          // ...
//
//      void writeCb(int                    status,
//                   int                    asyncStatus,
//                   btlsc::TimedCbChannel *serverChannel,
//                   int                    msgSize);
//          // Called when a write operation to the server channel ends.
//          // ...
//
//    private:
//      // NOT IMPLEMENTED
//      my_TickerplantSimulator(const my_TickerplantSimulator&);
//      my_TickerplantSimulator& operator=(const my_TickerplantSimulator&);
//
//    public:
//      my_TickerplantSimulator(bsl::ostream&              console,
//                              btlsc::CbChannelAllocator *connector,
//                              btlsc::TimedCbChannel     *input,
//                              int                        inputSize);
//          // Create a non-blocking ticker-plant simulator using the specified
//          // 'input' channel to read ASCII tick records of the specified
//          // 'inputSize' and convert each record to a 'my_Tick' structure;
//          // each tick value is sent asynchronously to a peer via a distinct
//          // channel obtained from the specified 'connector', reporting any
//          // errors to the specified 'console'.  If 'connector' fails or is
//          // unable to succeed after 30 seconds, or if transmission itself
//          // exceeds 10 seconds, display a message on 'console' and abort the
//          // transmission.  If three successive reads of the input channel
//          // fail to produce a valid ticks, invalidate the channel and shut
//          // down this simulator.  The behavior is undefined unless
//          // '0 < inputSize'.
//
//      ~my_TickerplantSimulator();
//          // Destroy this simulator object.
//  };
//
//  const double CONNECT_TIME_LIMIT = 30;  // 30 seconds
//  const double WRITE_TIME_LIMIT   = 10;  // 10 seconds
//
//  enum { MAX_PARSER_ERRORS = 3 };
//
//  static
//  int parseTick(my_Tick *result, const char *buffer, int numBytes)
//      // Parse a tick value from the specified input 'buffer' of the
//      // specified 'numBytes' and load that value into the specified
//      // 'result'.  Return 0 on success, and a non-zero value (with no effect
//      // on 'result') otherwise.  The behavior is undefined unless
//      // '0 <= numBytes'.
//  {
//      // ...
//      return 0;
//  }
//
//  void my_TickerplantSimulator::readCb(const char *buffer,
//                                       int         status,
//                                       int         asyncStatus)
//  {
//      assert(status <= d_inputSize);
//
//      if (d_inputSize == status) {  // Tick message read successfully.
//          my_Tick tick;
//
//          if (0 == ::parseTick(&tick, buffer, d_inputSize)) {
//              d_parserErrorCount = 0;  // Successfully parsed tick value.
//
//              // Create a connect functor with this tick value as user data.
//
//              using namespace bdlf::PlaceHolders;
//              btlsc::CbChannelAllocator::TimedCallback functor(
//                  bdlf::BindUtil::bind(
//            bdlf::MemFnUtil::memFn(&my_TickerplantSimulator::connectCb, this)
//          , _1, _2
//          , tick));
//
//              bsls::TimeInterval now =
//                    bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME);
//
//              // Initiate a non-blocking timed allocate operation.
//
//              if (d_connector_p->allocateTimed(functor)) {
//                  bsl::cout << "Error: Unable to install accept operation."
//                            << bsl::endl;
//
//                  // This simulator is broken; invalidate input channel.
//
//                  d_input_p->invalidate();
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
//      else if (status >= 0) {  // partial (incomplete) read
//          assert(0);  // Impossible, no form of partial read was authorized.
//      }
//      else {  // Hard error.
//          assert(status < 0);
//
//          bsl::cout << "Error: Read Failed." << bsl::endl;
//                                                   // Cannot be re-installed.
//      }
//
//      // Attempt to reinstall buffered read operation (pass/fail).
//
//      if (d_input_p->bufferedRead(d_inputSize, d_readFunctor)) {
//          d_console << "Error: Unable to reregister (untimed)"
//                       " read operation." << bsl::endl;
//          // This simulator is broken.
//      }
//  }
//
//  void my_TickerplantSimulator::connectCb(
//                                        btlsc::TimedCbChannel *serverChannel,
//                                        int                    status,
//                                        const my_Tick&         tick)
//  {
//      if (serverChannel) {     // Successfully created a connection.
//          bslx::ByteOutStream bos(VERSION_SELECTOR);
//          bos << tick;
//          int msgSize = bos.length();
//
//          // Install write callback and embed both 'serverChannel' and the
//          // streamed tick-value's message size as user data.
//
//          using namespace bdlf::PlaceHolders;
//          btlsc::TimedCbChannel::WriteCallback functor(
//              bdlf::BindUtil::bind(
//              bdlf::MemFnUtil::memFn(&my_TickerplantSimulator::writeCb, this)
//            , _1, _2
//            , serverChannel
//            , msgSize));
//
//          bsls::TimeInterval now =
//                    bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME);
//
//          // Initiate a timed non-blocking write operation.
//
//          if (serverChannel->timedBufferedWrite(bos.data(), msgSize,
//                                                now + WRITE_TIME_LIMIT,
//                                                functor)) {
//
//              d_console << "Error: Unable even to register a write"
//                           " operation on this channel." << bsl::endl;
//
//              // Give this invalid output channel back (but do not
//              // invalidate the input channel).
//
//              d_connector_p->deallocate(serverChannel);
//          }
//      }
//      else if (status > 0) {  // Interrupted due to external event.
//          assert(0);  // Impossible, not authorized.
//      }
//      else if (0 == status) {  // Interrupted due to timeout event.
//          d_console << "Error: Connector timed out, transition aborted."
//                    << bsl::endl;
//      }
//      else {  // Connector failed.
//          assert(0 < status);
//
//          bsl::cout << "Error: Unable to connect to server." << bsl::endl;
//
//          // The server is down; invalidate the input channel, allowing
//          // existing write operations to complete before the simulator
//          // shuts down.
//
//          d_input_p->invalidate();
//      }
//  }
//
//  void my_TickerplantSimulator::writeCb(int                    status,
//                                        int                    asyncStatus,
//                                        btlsc::TimedCbChannel *serverChannel,
//                                        int                    msgSize)
//  {
//      assert(serverChannel);
//      assert(0 < msgSize);
//      assert(status <= msgSize);
//
//      if (msgSize == status) {
//          // Encoded tick value written successfully.
//      }
//      else if (0 <= status) {   // Tick message timed out.
//
//          assert(0 == asyncStatus  // only form of partial-write authorized
//              || 0 >  asyncStatus  // This operations was dequeued due to a
//                 && 0 == status);  // previous partial write operation.
//
//          if (0 == asyncStatus) {
//              d_console << "Write of tick data timed out." << bsl::endl;
//
//              if (status > 0) {
//                  // If we had the data, we could retry, but since it was a
//                  // buffered write, we'll just invalidate the channel.
//
//                  d_console << "Partial tick data written;"
//                               " invalidating channel." << bsl::endl;
//
//                  // If we know its data stream is corrupted, we typically
//                  // invalidate a channel before allocating it.
//
//                  serverChannel->invalidate();
//              }
//              else {
//                  d_console << "No data was written; channel is still valid."
//                            << bsl::endl;
//              }
//          }
//          else {
//              assert(0 < asyncStatus && 0 == status);
//
//              d_console << "This operation was dequeued due to previous"
//                           " partial write (no data was written)."
//                        << bsl::endl;
//          }
//      }
//      else {  // Tick message write failed.
//          assert(0 > status);
//
//          d_console << "Error: Unable to write tick value to server."
//                    << bsl::endl;
//      }
//
//      // In all cases, return the server channel to the allocator.
//
//      d_connector_p->deallocate(serverChannel);
//  }
//
//  my_TickerplantSimulator::my_TickerplantSimulator(
//                               bsl::ostream&              console,
//                               btlsc::CbChannelAllocator *connector,
//                               btlsc::TimedCbChannel     *input,
//                               int                        inputSize)
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
//
//      // load reusable buffered read functor
//      d_readFunctor = bdlf::MemFnUtil::memFn(
//                                     &my_TickerplantSimulator::readCb, this);
//
//      // Attempt to initiate the first read operation (pass/fail).
//      if (d_input_p->bufferedRead(d_inputSize, d_readFunctor)) {
//          d_console << "Error: Unable to register (untimed) read operation."
//                    << bsl::endl;
//          // This simulator is broken.
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
// In order to make this 'my_TickerplantSimulator' work, we will need to
// instantiate a concrete callback channel allocator (i.e., socket connector)
// and channel that is hooked up to an appropriate event manager:
//..
//  int main(int argc, const char *argv[])
//  {
//      // OUTBOUND:
//      const char *const DEFAULT_HOST = "widget";
//      enum { DEFAULT_PORT = 5001 };
//      enum { DEFAULT_SIZE = 80   };
//
//      const char *hostName   = argc > 1 ? argv[1]       : DEFAULT_HOST;
//      const int   portNumber = argc > 2 ? atoi(argv[2]) : DEFAULT_PORT;
//      const int   inputSize  = argc > 3 ? atoi(argv[3]) : DEFAULT_SIZE;
//
//      // INBOUND:
//      // This simulator accepts connections on port 'DEFAULT_PORT' only.
//
//      btlso::IPv4Address address;
//      btlso::ResolveUtil<btlso::IPv4Address>::resolve(&address,
//                                                      hostName,
//                                                      portNumber);
//      btlso::Tcpipv4StreamSocketFactory sf;
//      btlso::InetTimedSocketEventManager manager;
//      btlsos::TimedCbChannelConnector connector(address, &sf, &manager);
//
//      my_TickerplantSimulator
//                          simulator(bsl::cout, &connector, input, inputSize);
//
//      while (0 != manager.dispatch()) {
//          // Do nothing.
//      }
//      // Done only when there are no more events to process.
//      return 0;
//  }
//..
// Please remember that these example code snippets are intended to illustrate
// the use of 'btlsc::CbChannelAllocator' and do not represent
// production-quality software.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

namespace BloombergLP {
namespace btlsc {

class CbChannel;
class TimedCbChannel;

                          // ========================
                          // class CbChannelAllocator
                          // ========================

class CbChannelAllocator {
    // This class is a protocol (pure abstract interface) for a non-blocking
    // mechanism that allocates end points of communication channels supporting
    // timed and untimed non-blocking (buffered and non-buffered) read and
    // write operations on a byte stream.  A 'bdlf' callback functor
    // communicates the results of the asynchronous allocation.  A successful
    // allocation passes to the callback the address of a channel, in which
    // case the callback's "status" argument has no significance.  Otherwise, a
    // non-negative callback status indicates an asynchronous interruption, a
    // status of -1 implies that the operation was canceled (synchronously) by
    // the caller (see 'cancelAll'), and a negative status value other than -1
    // implies an error (i.e., an inability of this object to succeed at the
    // present time).  A failure to register an allocation operation (or an
    // explicit call to 'invalidate') implies that the channel allocator is
    // permanently invalid (see 'isInvalid').  An invalid allocator cannot
    // successfully register new allocation operations, but pending allocation
    // operations will not necessarily fail.  Note that an invalid allocator is
    // still capable of deallocation.

  private:
    // NOT IMPLEMENTED
    CbChannelAllocator& operator=(const CbChannelAllocator&);

  protected:
    // CREATORS
    virtual ~CbChannelAllocator();
        // Destroy this object.

  public:
    // TYPES
    typedef bsl::function<void(CbChannel *, int)> Callback;
        // Invoked as a result of an 'allocate' request, 'Callback' is an alias
        // for a callback function object (functor) that returns 'void' and
        // takes as arguments the (possibly null) address of a callback
        // "channel" and an integer "status" indicating either an interruption
        // (positive) or an error (negative).  Note that "status" is meaningful
        // only if "channel" is 0.

    typedef bsl::function<void(TimedCbChannel *, int)> TimedCallback;
        // Invoked as a result of an 'allocateTimed' request, 'TimedCallback'
        // is an alias for a callback function object (functor) that returns
        // 'void' and takes as arguments the (possibly null) address of a timed
        // callback "channel" and an integer "status" indicating either an
        // interruption (positive) or an error (negative).  Note that "status"
        // is meaningful only if "channel" is 0.

    // MANIPULATORS
    virtual int allocate(const Callback& callback, int flags = 0) = 0;
        // Initiate a non-blocking operation to allocate a callback channel;
        // execute the specified 'callback' functor after the allocation
        // operation terminates.  If the optionally specified 'flags'
        // incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT', "asynchronous
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

    virtual int allocateTimed(const TimedCallback& timedCallback,
                              int                  flags = 0) = 0;
        // Initiate a non-blocking operation to allocate a timed callback
        // channel; execute the specified 'timedCallback' functor after the
        // allocation operation terminates.  If the optionally specified
        // 'flags' incorporates 'btlsc::Flag::k_ASYNC_INTERRUPT',
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
        // 'timedCallback' is valid.

    virtual void cancelAll() = 0;
        // Immediately cancel all pending operations on this allocator,
        // invoking each registered allocation callback in turn.  Each callback
        // will be invoked with a null channel and a status of -1.  This method
        // may be invoked successfully on an invalid allocator; however,
        // calling the method does not invalidate the allocator.  Note that
        // calling 'cancelAll' from a callback that has itself been canceled
        // simply extends the set of canceled operations to include any new
        // ones initiated since the previous 'cancelAll' was invoked.

    virtual void deallocate(CbChannel *channel) = 0;
        // Terminate all operations on the specified 'channel', invoke each
        // pending callback with the appropriate status, and reclaim all
        // afforded channel services.  The behavior is undefined unless
        // 'channel' is currently allocated from this allocator (i.e., was
        // previously obtained from this instance and has not subsequently been
        // deallocated).  Note that this method can never block.

    virtual void invalidate() = 0;
        // Place this allocator in a permanently invalid state.  No new
        // allocation operations can be initiated; deallocation and previously
        // initiated allocation requests are not affected.

    // ACCESSORS
    virtual int isInvalid() const = 0;
        // Return 1 if this allocator is permanently invalid, and 0 otherwise.
        // An invalid allocator can never again register a request to allocate
        // a channel, but *may* succeed in completing existing enqueued
        // requests; deallocation operations are unaffected.  Note that the
        // significance of a 0 return cannot be relied upon beyond the return
        // of this method.
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
