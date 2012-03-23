// btesc_cbchannelallocator.h       -*-C++-*-
#ifndef INCLUDED_BTESC_CBCHANNELALLOCATOR
#define INCLUDED_BTESC_CBCHANNELALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide protocol for non-blocking stream-based channel allocators.
//
//@CLASSES:
// btesc_CbChannelAllocator: non-blocking stream-based channel allocator
//
//@SEE_ALSO: btesc_channelallocator btemc_cbchannelallocator
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component defines an abstract interface for a non-blocking
// mechanism that allocates and deallocates non-blocking channels with and
// without timeout capability; the allocation itself does *not* have timeout
// capability.  Each channel allocated through this interface is an end point
// of a bi-directional stream-based communication connection to a peer;
// connection details, such as who the peer is, whether there is indeed one
// peer or multiple similar peers, and how the connection came to be, are not
// relevant to this channel-allocator protocol, and are therefore abstracted.
//
// In particular, 'btesc_CbChannelAllocator' can allocate non-blocking channels
// of type 'btesc_CbChannel' and 'btesc_TimedCbChannel', i.e., untimed and
// timed non-blocking channels.
//
///Protocol Hierarchy
///------------------
// 'btesc_ChannelAllocator' forms the base of an interface hierarchy; other
// interfaces may be defined by direct public inheritance.
//..
//                     ,------------------------.
//                    ( btesc_CbChannelAllocator )
//                     `------------------------'
//..
//
///Non-Blocking Channel Allocation
///-------------------------------
// This protocol establishes methods for allocating non-blocking stream-based
// channels in a non-blocking manner; each method registers a user-supplied
// callback function object (functor) and returns immediately.  A successful
// return status implies that the registered callback will be invoked (and,
// conversely, an unsuccessful status implies otherwise.)  The callback, in
// turn, will communicate the results of the registered allocation attempt.
//
// Enqueued allocation operations proceed asynchronously to one of three
// possible results: (1) "success" -- the requested channel is allocated and
// its address is passes to the callback function; in this case the callback
// "status" argument has no meaning (2) "interrupted" -- the operation was
// interrupted (e.g., via an asynchronous event): the channel address is null
// and the (non-negative) status conveys the nature of the interruption, (3)
// "canceled" -- the operation was aborted (synchronously) by an explicit call
// to 'cancelAll': the channel-allocator address in null and the "status" is
// -1, or (4) "error" -- an implementation-dependent error occurred: the
// channel address is null and status is less than -1.  The user may retry
// interrupted and cancelled operations with a reasonable expectation of
// success.  An "error" status implies that the allocation is unlikely to
// succeed if retried, but does not necessarily invalidate the allocator.  The
// 'isInvalid' method may be used to confirm the occurrence of a permanent
// error.  If the allocator is valid, an allocation request will be enqueued
// and may succeed.  Otherwise, the allocation request itself will not succeed.
//
// The meanings of the callback function status value for an unsuccessful
// allocation (i.e., a null channel address) are summarized as follows:
//..
//  "status"    meaning (only when returned channel address is null)
//  --------    -------------------------------------------------------------
//  positive    Interruption by an "asynchronous event"
//
//  -1          Operation explicitly canceled (synchronously) by the user
//
//  < -1        Allocation operation unable to succeed at this time
//..
// Note that unless asynchronous events are explicitly enabled (see below),
// they are ignored, and "status" will never be positive.  Note also that
// whether the callback is invoked before or after the registering method
// returns is not specified; in either case, if the registration was
// successful, then the return value will reflect success.  Finally, note that,
// for consistency with timed allocators, a status of 0 is reserved for a
// timeout (which is not supported in this untimed allocator).
//
///Callback Functor Registration
///- - - - - - - - - - - - - - -
// Once an operation is successfully initiated, a (reference-counted) copy of
// the ('bcef') callback functor is retained by the concrete allocator until
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
// to return, if possible, with an "interrupted" status (leaving the factory
// unaffected) upon the occurrence of an AE.  Such authorizations are made
// explicitly by incorporating into the optional (trailing) integer 'flags'
// argument to a method call the 'btesc_Flag::BTESC_ASYNC_INTERRUPT' value.
//
///Usage
///-----
// TBD Usage is from timed cb allocator and needs to be redone.
//
// The purpose of the 'btesc_CbChannelAllocator' protocol is to isolate the act
// of requesting a connection from details such as to whom the connection will
// be to and which side initiated the connection.  In this example we will
// consider both the Server and Client sides of a 'my_Tick' reporting service.
// Since each side of this service could potentially be a library component, we
// do not want to embed into either side the details of how connections will be
// established.  It is sufficient that, when a tick needs to be sent or
// received, a channel is obtained, the tick is transmitted, and the channel is
// returned to its allocator.  Note that this example serves to illustrate the
// use of the 'btesc_TimedChannelAllocator' and does not represent
// production-quality software.
//
///Server Side
///- - - - - -
// Let's assume that we have a function that knows how to print platform
// neutral encodings of type 'my_Tick':
//..
//  static void myPrintTick(bsl::ostream& stream, const char *buffer);
//      // Print the value of the specified 'buffer' interpreted as a
//      // 'bdex' byte-stream representation of a 'my_Tick' value, to the
//      // specified 'stream' or report an error to 'stream' if 'buffer' is
//      // determined *not* to hold an encoding of a valid 'my_Tick' value.
//..
// The following class illustrates how we might implement a tick-reporter
// server using just the 'btesc_CbChannelAllocator' and 'btesc_TimedCbChannel'
// protocols.  In this implementation the "allocate" functor (but not the
// "read" functor) is created in the constructor and cached for repeated use.
// Note that buffered reads avoid having to supply a buffer, and *may* improve
// throughput if connections are preserved (pooled) in the particular
// *concrete* channel allocator (supplied at construction).
//..
//  class my_TickReporter {
//      // This class implements a server that accepts connections, extracts
//      // from each connection a single 'my_Tick' value, and reports that
//      // value to a console stream; both the acceptor and console stream
//      // are supplied at construction.
//
//      btesc_CbChannelAllocator *d_acceptor_p;  // incoming connections
//      bsl::ostream&             d_console;     // where to put tick info
//
//      btesc_TimedCbChannel::TimedCallback     d_allocFunctor;  // reused
//
//    private:
//      void acceptCb(btesc_TimedCbChannel     *clientChannel,
//                    int                       status,
//                    const bdet_TimeInterval&  timeout);
//          // Called when a new client channel has been accepted.
//          // ...
//
//      void readCb(const char           *buffer,
//                  int                   status,
//                  int                   asyncStatus
//                  btesc_TimedCbChannel *clientChannel);
//          // Called when a 'my_Tick' value has been read from the channel.
//          // ...
//
//    private:
//      my_TickReporter(const my_TickReporter&);             // not impl.
//      my_TickReporter& operator=(const my_TickReporter&);  // not impl.
//
//    public:
//      my_TickReporter(bsl::ostream&             console,
//                      btesc_CbChannelAllocator *acceptor)
//          // Create a non-blocking tick-reporter using the specified
//          // 'acceptor' to establish incoming client connections, each
//          // transmitting a single 'my_Tick' value; write these values
//          // to the specified 'console' stream.  If the acceptor is idle
//          // for more than five minutes, print a message to the 'console'
//          // stream supplied at construction and continue.  To guard
//          // against malicious clients, a connection that does not produce
//          // a tick value within one minute will be summarily dropped.
//
//      ~my_TickReporter();
//          // Destroy this server object.
//  };
//
//  const double ACCEPT_TIME_LIMIT = 300;               // 5 minutes
//  const double   READ_TIME_LIMIT =  60;               // 1 minutes
//
//  static int calculateMyTickMessageSize()
//      // Calculate and return the number of bytes in a 'bdex' byte-stream
//      // encoding of a (dummy) 'my_Tick' value (called just once, see below).
//  {
//      my_Tick dummy;
//      bdex_ByteOutStream bos;
//      bos << dummy;
//      return bos.length();
//  }
//
//  static int myTickMessageSize()
//      // Return the number of bytes in a 'bdex' byte-stream encoding
//      // of a 'my_Tick' value without creating a runtime-initialized
//      // file-scope static variable (which is link-order dependent).
//  {
//      static const int MESSAGE_SIZE = calculateMessageSize();
//      return MESSAGE_SIZE;
//  }
//
//  void my_TickReporter::acceptCb(btesc_TimedCbChannel     *clientChannel,
//                                 int                       status)
//  {
//      if (clientChannel) {     // Successfully created a connection.
//
//          const int               numBytes = ::myTickMessageSize();
//          const bdet_TimeInterval now      = bdetu_SystemTime::now();
//
//          // Create one-time (buffered) read functor holding 'clientChannel'.
//          btesc_TimedCbChannel::BufferedReadCallback readFunctor(
//              bdef_BindUtil::bind( bdef_MemFnUtil::memFn(&readCb, this)
//                                 , _1, _2, _3
//                                 , clientChannel));
//
//          // Install read callback (timeout, but no raw or async interrupt).
//
//          if (clientChannel->timedBufferedRead(numBytes, readFunctor,
//                                               now + READ_TIME_LIMIT) {
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
//          d_console << "Error: The channel allocator is not working now."
//                    << bsl::endl;
//
//          // Note that attempting to re-register an allocate operation below
//          // will fail only if the channel allocator is permanently disabled.
//      }
//
//      // In all cases, attempt to reinstall the (reusable) accept callback.
//
//      if (d_acceptor_p->allocate(d_acceptFunctor)) {
//          d_console << "Error: unable to register accept operation."
//                    << bsl::endl;
//          // This server is hosed.
//      }
//  }
//
//  void my_TickReporter::readCb(const char           *buffer,
//                               int                   status,
//                               int                   asyncStatus,
//                               btesc_TimedCbChannel *clientChannel)
//  {
//      assert(clientChannel);
//
//      const int msgSize = ::myTickMessageSize();
//
//      if (msgSize == status) {  // Encoded-tick value read successfully.
//          assert(buffer);
//
//          ::myPrintTick(d_console, buffer);
//      }
//      else if (0 <= status) {   // Tick message was interrupted.
//
//          assert(buffer); // Data in buffer is available for inspection
//                          // (but remains in the channel's buffer).
//
//          // Must be a timeout event since neither raw (partial) reads
//          // nor (external) asynchronous interrupts were authorized.
//
//          assert(0 == asyncStatus);   // must be timeout event!
//
//          d_console << "Error: Unable to read tick value from channel"
//                       " before timing out; read aborted." << bsl::endl
//      }
//      else { // Tick-message read failed.
//          assert (0 > status);
//
//          d_console << "Error: Unable to read tick value from channel."
//                    << bsl::endl;
//      }
//
//      d_acceptor_p->deallocate(clientChannel);
//  }
//
//  my_TickReporter::my_TickReporter(bsl::ostream&             console,
//                                   btesc_CbChannelAllocator *acceptor)
//  : d_console(console)
//  , d_acceptor_p(acceptor)
//  {
//      assert(&d_console);
//      assert(d_allocator_p);
//
//      // Attempt to install the first accept callback.
//
//      bdet_TimeInterval timeout = bdeu_SystemTime::now()
//                                + ACCEPT_TIME_LIMIT;
//
//      // load reusable allocate functor
//      d_allocFunctor = bdef_BindUtil::bind(
//                                       bdef_MemFnUtil::memFn(&acceptCb, this)
//                                    , _1, _2
//                                    , timeout);
//
//      if (d_acceptor_p->allocate(d_allocFunctor)) {
//          d_console << "Error: Unable to install accept operation."
//                    << bsl::endl;
//          // This server is hosed.
//      }
//  }
//
//  my_TickReporter::~my_TickReporter()
//  {
//      assert(&d_console);
//      assert(d_allocator_p);
//  }
//..
// In order to make this 'my_TickReporter' work, we will need to instantiate a
// concrete (e.g., socket) callback channel allocator that is hooked up to an
// appropriate event manager.
//..
//  int main (int argc, const char *argv[]) {
//      enum { DEFAULT_PORT = 5000 };
//
//      const int portNumber = argc > 1 ? itoa(argv[1]) : DEFAULT_PORT;
//
//      bteso_Ipv4Address address(bteso_Ipv4Address::ANY, portNumber);
//      bteso_InetTimedSocketEventManager sem;    // concrete manager
//      bteso_Tcpipv4StreamSocketFactory sf;      // concrete factory
//      btesos_TimedCbChannelAcceptor acceptor(address, &sf, &sem);
//
//      if (acceptor.isInvalid()) {
//           bsl::cout << "Error: Unable to create acceptor" << bsl::endl;
//           return -1;
//      }
//
//      my_TickReporter reporter(bsl::cout, &acceptor);
//
//      while (0 != sem.dispatch()) {
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
// demand.  We will use the 'btesc_TimedCbChannelAllocator' protocol to
// abstract those details out of the stable software that generates (or
// forwards) ticks.  For the purposes of this example, let's assume that ticks
// are generated in some ASCII format and arrive in fixed size chunks (e.g., 80
// bytes) from a separate process.  Note that of the three callback methods
// 'readCb', 'connectCb', and 'writeCb', only 'readCb' requires no additional,
// call-specific user data; hence we can easily create it once at construction,
// and productively cache it for repeated used.  We will choose to reload the
// others each time (which is admittedly somewhat less efficient).
//..
//  class my_TickerplantSimulator {
//      // Accept raw tick values in ASCII sent as fixed-sized packets via a
//      // single 'btesc_TimedCbChannel' and send them asynchronously one by
//      // one to a peer (or similar peers) connected via channels provided via
//      // a 'btesc_TimedCbChannelAllocator'.  Both the output channel
//      // allocator and the input channel are supplied at construction.
//
//      btesc_TimedCbChannelAllocator *d_connector_p;  // outgoing connections
//      btesc_TImedCbChannel          *d_input_p;      // incoming packets
//      bsl::ostream&                  d_console;      // where to write errors
//      const int                      d_inputSize;    // input packet size
//      int                            d_parserErrorCount; //consecutive errors
//
//      btesc_TimedCbChannel::BufferedReadCallback d_readFunctor;  // reused
//
//    private:
//      void readCb(const char               *buffer,
//                  int                       status,
//                  int                       asyncStatus)
//          // Called when a fixed-size record has been read from the input.
//          // ...
//
//      void connectCb(btesc_TimedCbChannel *serverChannel,
//                     int                   status,
//                     const my_Tick&        tick);
//          // Called when a new server channel has been established.
//          // ...
//
//      void writeCb(int                   status,
//                   int                   asyncStatus
//                   btesc_TimedCbChannel *serverChannel
//                   int                   messageSize);
//          // Called when a write operation to the server channel ends.
//          // ...
//
//    private:  // Not implemented.
//      my_TickerplantSimulator(const my_TickerplantSimulator&);
//      my_TickerplantSimulator& operator=(const my_TickerplantSimulator&);
//
//    public:
//      my_TickerplantSimulator(bsl::ostream&                       console,
//                              btesc_TimedCbChannelAllocator *connector
//                              btesc_TimedCbChannel          *input,
//                              int                            inputSize);
//          // Create a non-blocking ticker-plant simulator using the
//          // specified 'input' channel to read ASCII tick records of
//          // the specified 'inputSize' and convert each record to a
//          // 'my_Tick' structure; each tick value is sent asynchronously
//          // to a peer via a distinct channel obtained from the specified
//          // 'connector', reporting any errors to the specified 'console'.
//          // If 'connector' fails or is unable to succeed after 30 seconds,
//          // or if transmission itself exceeds 10 seconds, display a message
//          // on 'console' and abort the transmission.  If three successive
//          // reads of the input channel fail to produce a valid ticks,
//          // invalidate the channel and shut down this simulator.  The
//          // behavior is undefined unless 0 < inputSize.
//
//      ~my_TickerplantSimulator();
//          // Destroy this simulator object.
//  };
//
//  static const bdet_TimeInterval ACCEPT_TIME_LIMIT = 30;  // 30 seconds
//  static const bdet_TimeInterval  WRITE_TIME_LIMIT = 10;  // 10 seconds
//
//  enum { MAX_PARSER_ERRORS = 3 };
//
//  static inline
//  int parseTick(my_Tick *result, const char *buffer, int numBytes)
//      // Parse a tick value from the specified input 'buffer' of specified
//      // 'numBytes' and load that value into the specified 'result'.  Return
//      // 0 on success, and a non-zero value (with no effect on 'result')
//      // otherwise.  The behavior is undefined unless 0 <= numBytes.
//  {
//      // ...
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
//              btesc_TimedCbChannelAllocator::TimedCallback functor(
//                 bdef_BindUtil::bind(bdef_MemFnUtil::memFn(&connectCb, this)
//                                   , _1, _2
//                                   , tick));
//
//              bdet_TimeInterval now = bdes_SystemTime::now();
//
//              // Initiate a non-blocking timed allocate operation.
//
//              if (d_acceptor_p->allocate(functor)) {
//                  bsl::cout << "Error: Unable to install accept operation."
//                       << bsl::endl;
//
//                  // This simulator is hosed; invalidate input channel.
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
//          bsl::cout << "Error: Read Failed." << bsl::endl;
//                                                   // Cannot be re-installed.
//      }
//
//      // Attempt to reinstall buffered read operation (pass/fail).
//
//      if (d_connector_p->bufferedRead(d_inputSize, d_readFunctor)) {
//          d_console << "Error: Unable to reregister (untimed)"
//                       " read operation." << bsl::endl;
//          // This simulator is hosed.
//      }
//  }
//
//  void my_TickerplantSimulator::connectCb(
//                                         btesc_TimedCbChannel *serverChannel,
//                                         int                  status,
//                                         const my_Tick&       tick)
//  {
//      if (serverChannel) {     // Successfully created a connection.
//          bdex_ByteOutStream bos;
//          bos << tick;
//          int msgSize = bos.length();
//
//          // Install write callback and embed both 'severChannel' and the
//          // streamed tick-value's message size as user data.
//
//          btesc_TimedCbChannel::WriteCallback functor(
//              bdef_BindUtil::bind( bdef_MemFnUtil::memFn(&writeCb, this)
//                                 , _1, _2
//                                 , serverChannel
//                                 , msgSize));
//
//          bdet_TimeInterval now = bdeu_SystemTime::now();
//
//          // Initiate a timed non-blocking write operation.
//
//          if (serverChannel->bufferedWrite(bos.data(), msgSize, functor,
//                                           now + WRITE_TIME_LIMIT)) {
//
//              d_console << "Error: Unable even to register a write"
//                           " operation on this channel." << bsl::endl;
//
//              // Give this invalid output channel back (but do not
//              // invalidate the input channel).
//
//              d_acceptor_p->deallocate(serverChannel);
//          }
//      }
//      else if (status > 0 ) {  // Interrupted due to external event.
//          assert (0);  // Impossible, not authorized.
//      }
//      else if (0 == status) {  // Interrupted due to timeout event.
//          console << "Error: Connector timed out, transition aborted."
//                  << bsl::endl;
//      }
//      else {  // Connector failed.
//          assert (0 < status);
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
//  void my_TickerplantSimulator::writeCb(int                   status,
//                                        int                   asyncStatus,
//                                        btesc_TimedCbChannel *serverChannel
//                                        int                   msgSize)
//  {
//      assert(severChannel);
//      assert(0 < msgSize);
//      assert(status <= msgSize);
//
//      if (msgSize == status) {
//          // Encoded tick value written successfully.
//      }
//      else if (0 <= status) {   // Tick message timed out.
//
//          assert(   0 == asyncStatus // only form of partial-write authorized
//                 || 0 >  asyncStatus // This operations was dequeued due to
//                    && 0 == status); // a previous partial write operation.
//
//          if (0 == asyncStatus) {
//              d_console << "Write of tick data timed out." << bsl::endl;
//
//              if (status > 0) {
//                  // If we had the data, we could retry, but since it was
//                  // a buffered write, we'll just invalidate the channel.
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
//              d_console << "This operation was dequeued due to pervious"
//                           " partial write (no data was written)."
//                        << bsl::endl;
//          }
//      }
//      else {  // Tick message write failed.
//          assert(0 > status);
//          d_console << "Error: Unable to write tick value to server."
//                    << bsl::endl;
//      }
//
//      // In all cases, return the server channel to the allocator
//
//      d_acceptor_p->deallocate(serverChannel);
//  }
//
//  my_TickerplantSimulator::my_TickerplantSimulator(
//                               bsl::ostream&                       console,
//                               btesc_TimedCbChannelAllocator *connector
//                               btesc_TimedCbChannel          *input,
//                               int                            inputSize)
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
//      d_readFunctor = bdef_MemFnUtil::memFn(&readCb, this);
//
//      // Attempt to initiate the first read operation (pass/fail).
//      if (d_connector_p->bufferedRead(d_inputSize, d_readFunctor)) {
//          d_console << "Error: Unable to register (untimed) read operation."
//                    << bsl::endl;
//          // This simulator is hosed.
//      }
//  }
//
//  my_TickReporter::~my_TickReporter()
//  {
//      assert(&d_console);
//      assert(d_allocator_p);
//      assert(0 < d_inputSize);
//      assert(0 <= d_parserErrorCount);
//      assert(d_parserErrorCount <= MAX_PARSER_ERRORS);
//  }
//..
// In order to make this 'my_TickerplantSimulator' work, we will need to
// instantiate a concrete callback channel allocator (i.e., socket connector)
// and channel that is hooked up to an appropriate event manager.
//..
//  int main (int argc, const char *argv[])
//  {
//      // OUTBOUND:
//      const char *const DEFAULT_HOST = "sundev1";
//      enum { DEFAULT_PORT = 5001 };
//      enum { DEFAULT_SIZE = 80 };
//
//      const char *hostName = argc > 1 ? argv[1]       : DEFAULT_HOST;
//      const int portNumber = argc > 2 ? atoi(argv[2]) : DEFAULT_PORT;
//      const int inputSize  = argc > 3 ? atoi(argv[3]) : DEFAULT_SIZE;
//
//      // INBOUND:
//      // This simulator accepts connections on port 'DEFAULT_PORT' only.
//
//      bteso_Ipv4Address address;
//      bteso_ResolveUtil<bteso_Ipv4Address>::resolve(&address,
//                                                     hostName,
//                                                     portNumber);
//      bteso_Tcpipv4StreamSocketFactory sf;
//      bteso_InetTimedSocketEventManager sem;
//      btesos_TimedCbChannelConnector connector(address, &sf, &sem);
//
//      my_TickerplantSimulator
//                          simulator(bsl::cout, &connector, input, inputSize);
//
//      while(0 != sem.dispatch()) {
//           // Do nothing.
//      }
//      // Done only when there are no more events to process.
//      return 0;
//  }
//..
// Please remember that these example code snippets are intended to illustrate
// the use of 'btesc_TimedCbChannelAllocator' and do not represent
// production-quality software.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

namespace BloombergLP {

class bdet_TimeInterval;

class btesc_CbChannel;
class btesc_TimedCbChannel;

                        // ==============================
                        // class btesc_CbChannelAllocator
                        // ==============================

class btesc_CbChannelAllocator
{
    // This class is a protocol (pure abstract interface) for a non-blocking
    // mechanism that allocates end points of communications channels
    // supporting timed and untimed non-blocking (buffered and non-buffered)
    // read and write operations on a byte stream.  A 'bcef' callback functor
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
    btesc_CbChannelAllocator&
        operator=(const btesc_CbChannelAllocator&); // not implemented

  protected:
    // CREATORS
    virtual ~btesc_CbChannelAllocator();
        // Destroy this channel (required for syntactic consistency only).

  public:
    // TYPES
    typedef bdef_Function<void (*)(btesc_CbChannel *, int)> Callback;
        // Invoked as a result of an 'allocate' request, 'Callback' is an alias
        // for a callback function object (functor) that returns 'void' and
        // takes as arguments the (possibly null) address of a callback
        // "channel" and an integer "status" indicating either an interruption
        // (positive) or an error (negative).  Note that "status" is meaningful
        // only if "channel" is 0.

    typedef bdef_Function<void (*)(btesc_TimedCbChannel *, int)> TimedCallback;
        // Invoked as a result of an 'allocateTimed' request, 'TimedCallback'
        // is an alias for a callback function object (functor) that returns
        // 'void' and takes as arguments the (possibly null) address of a timed
        // callback "channel" and an integer "status" indicating either an
        // interruption (positive_ or an error (negative).  Note that "status"
        // is meaningful only if "channel" is 0.

    // MANIPULATORS
    virtual int allocate(const Callback& callback,
                         int             flags = 0)                        = 0;
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

    virtual int allocateTimed(const TimedCallback& timedCallback,
                              int                  flags = 0)              = 0;
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

    virtual void cancelAll() = 0;
        // Immediately cancel all pending operations on this allocator,
        // invoking each registered allocation callback in turn.  Each callback
        // will be invoked with a null channel and a status of -1.  This method
        // may be invoked successfully on an invalid allocator; however,
        // calling the method does not invalidate the allocator.  Note that
        // calling 'cancelAll' from a callback that has itself been canceled
        // simply extends the set of canceled operations to include any new
        // ones initiated since the previous 'cancelAll' was invoked.

    virtual void deallocate(btesc_CbChannel *channel) = 0;
        // Terminate all operations on the specified 'channel', invoke each
        // pending callback with the appropriate status, and reclaim all
        // afforded channel services.  The behavior is undefined unless
        // 'channel' is currently allocated from this allocator, (i.e., was
        // previously obtained from this instance and has not subsequently
        // deallocated deallocated).  Note that this method can never block.

    virtual void invalidate() = 0;
        // Place this allocator in a permanently invalid state.  No new
        // allocation operations can be initiated; deallocation and previously
        // initiated allocation requests are not affected.

    // ACCESSORS
    virtual int isInvalid() const = 0;
        // Return 1 if this allocator is permanently invalid and 0 otherwise.
        // An invalid allocator can never again register a request to allocate
        // a channel, but *may* succeed in completing existing enqueued
        // requests; deallocation operations are unaffected.  Note that the
        // significance of a 0 return cannot be relied upon beyond the return
        // of this method.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
