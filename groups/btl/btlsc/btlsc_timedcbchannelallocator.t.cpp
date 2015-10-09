// btlsc_timedcbchannelallocator.t.cpp                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsc_timedcbchannelallocator.h>

#include <btlsc_cbchannel.h>       // for testing only
#include <btlsc_flag.h>            // for testing only
#include <btlsc_timedcbchannel.h>  // for testing only

#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

#include <bdls_testutil.h>

#include <bdlt_currenttime.h>

#include <bsls_platform.h>
#include <bsls_protocoltest.h>
#include <bsls_timeinterval.h>

#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>

#include <bsl_cstdlib.h>          // 'atoi'
#include <bsl_cstdio.h>           // 'snprintf'
#include <bsl_cstring.h>          // 'strcpy'
#include <bsl_functional.h>
#include <bsl_iostream.h>

#include <bsl_c_stdio.h>

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf_s
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a protocol class the purpose of which is to
// provide an interface for loading calendars.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o It is possible to create a concrete implementation the protocol.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] virtual ~TimedCbChannelAllocator();
//
// MANIPULATORS
// [ 1] virtual int allocate(const Callback&, int = 0) = 0;
// [ 1] virtual int timedAllocate(const Cb&, const TI&, int = 0) = 0;
// [ 1] virtual int allocateTimed(const TimedCallback&, int = 0) = 0;
// [ 1] virtual int timedAllocateTimed(const TimedCb&, TI&, int = 0) = 0;
// [ 1] virtual void cancelAll() = 0;
// [ 1] virtual void deallocate(CbChannel *) = 0;
// [ 1] virtual void invalidate() = 0;
//
// ACCESSORS
// [ 1] virtual int isInvalid() const = 0;
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

typedef btlsc::TimedCbChannelAllocator ProtocolClass;
typedef ProtocolClass                  PC;

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    int allocate(const PC::Callback&, int = 0)           { return markDone(); }

    int timedAllocate(const PC::Callback&, const bsls::TimeInterval&, int = 0)
                                                         { return markDone(); }

    int allocateTimed(const PC::TimedCallback&, int = 0) { return markDone(); }

    int timedAllocateTimed(const PC::TimedCallback&,
                           const bsls::TimeInterval&,
                           int = 0)                      { return markDone(); }

    void cancelAll()                                     {        markDone(); }

    void deallocate(btlsc::CbChannel *)                  {        markDone(); }

    void invalidate()                                    {        markDone(); }

    int isInvalid() const                                { return markDone(); }
};

}  // close unnamed namespace

// Free functions used as callbacks.

void myCbFn(btlsc::CbChannel *, int) { }
void myTimedCbFn(btlsc::TimedCbChannel *, int) { }

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The purpose of the 'btlsc::TimedCbChannelAllocator' protocol is to isolate
// the act of requesting a connection from details such as who it will be
// connected to and which side initiated the connection.  In this example we
// will consider both the Server and Client sides of a 'my_Tick' reporting
// service.  Since each side of this service could potentially be a library
// component, we do not want to embed into either side the details of how
// connections will be established.  It is sufficient that, when a tick needs
// to be sent or received, a channel is obtained, the tick is transmitted, and
// the channel is returned to its allocator.  Note that this example serves to
// illustrate the use of the 'btlsc::TimedCbChannelAllocator' and does not
// represent production-quality software.  Also note that 'my_Tick' will be
// used by both the server and client:
//..
    class my_Tick {
        char   d_name[5];
        double d_bestBid;
        double d_bestOffer;

      public:
        my_Tick() { }
        my_Tick(const char *ticker);
        my_Tick(const char *ticker, double bestBid, double bestOffer);
        ~my_Tick() { ASSERT(d_bestBid > 0); };

        static int maxSupportedBdexVersion(int versionSelector) { return 1; }

        template <class STREAM>
        STREAM& bdexStreamOut(STREAM& stream, int version) const;
            // Write this value to the specified output 'stream' using the
            // specified 'version' format, and return a reference to 'stream'.
            // If 'stream' is initially invalid, this operation has no effect.
            // If 'version' is not supported, 'stream' is invalidated but
            // otherwise unmodified.  Note that 'version' is not written to
            // 'stream'.  See the 'bslx' package-level documentation for more
            // information on BDEX streaming of value-semantic types and
            // containers.

        template <class STREAM>
        STREAM& bdexStreamIn(STREAM& stream, int version);
            // Assign to this object the value read from the specified input
            // 'stream' using the specified 'version' format, and return a
            // reference to 'stream'.  If 'stream' is initially invalid, this
            // operation has no effect.  If 'version' is not supported, this
            // object is unaltered and 'stream' is invalidated but otherwise
            // unmodified.  If 'version' is supported but 'stream' becomes
            // invalid during this operation, this object has an undefined, but
            // valid, state.  Note that no version is read from 'stream'.  See
            // the 'bslx' package-level documentation for more information on
            // BDEX streaming of value-semantic types and containers.

        void print(bsl::ostream& stream) const;
    };

    my_Tick::my_Tick(const char *ticker)
    : d_bestBid(0)
    , d_bestOffer(0)
    {
       snprintf(d_name, sizeof d_name, "%s", ticker);
    }

    my_Tick::my_Tick(const char *ticker, double bestBid, double bestOffer)
    : d_bestBid(bestBid)
    , d_bestOffer(bestOffer)
    {
        snprintf(d_name, sizeof d_name, "%s", ticker);
    }

    void my_Tick::print(bsl::ostream& stream) const
    {
        stream << "(" << d_name << ", " << d_bestBid << ", " << d_bestOffer
               << ")" << endl;
    }

    inline
    bsl::ostream& operator<<(bsl::ostream& stream, const my_Tick& tick)
    {
        tick.print(stream);
        return stream;
    }

    template <class STREAM>
    STREAM& my_Tick::bdexStreamOut(STREAM& stream, int version) const
    {
        switch (version) {
          case 1: {
            stream.putString(d_name);
            stream.putFloat64(d_bestBid);
            stream.putFloat64(d_bestOffer);
          } break;
          default: {
            stream.invalidate();
          } break;
        }
        return stream;
    }

    template <class STREAM>
    STREAM& my_Tick::bdexStreamIn(STREAM& stream, int version)
    {
        switch (version) {
          case 1: {
            bsl::string temp1;
            stream.getString(temp1);
            int maxLen = sizeof d_name - 1;  // the valid name length
            int len    = temp1.length();
            if (len < maxLen) {
                strcpy(d_name, temp1.c_str());
            }
            else {
                strncpy(d_name, temp1.c_str(), len);
                d_name[len] = 0;
            }
            stream.getFloat64(d_bestBid);
            stream.getFloat64(d_bestOffer);
          } break;
          default: {
            stream.invalidate();
          } break;
        }
        return stream;
    }

//..
// Let's also assume that we have a function that knows how to print platform
// neutral encodings of type 'my_Tick':
//..
    static void myPrintTick(bsl::ostream& stream, const char *buffer, int len)
        // Print the value of the specified 'buffer' interpreted as a
        // BDEX byte-stream representation of a 'my_Tick' value, to the
        // specified 'stream' or report an error to 'stream' if 'buffer' is
        // determined *not* to hold an encoding of a valid 'my_Tick' value.
    {
        my_Tick tick;
        bslx::ByteInStream input(buffer, len);
        input >> tick;

        stream << tick;
    }
//..
///Server Side
///-----------
// The following class illustrates how we might implement a tick-reporter
// server using just the 'btlsc::TimedCbChannelAllocator' and
// 'btlsc::TimedCbChannel' protocols.  In this implementation the "allocate"
// functor (but not the "read" functor) is created in the constructor and
// cached for repeated use.  Note that buffered reads avoid having to supply a
// buffer, and *may* improve throughput if connections are preserved (pooled)
// in the particular *concrete* channel allocator (supplied at construction).
//..
    class my_TickReporter {
        // This class implements a server that accepts connections, extracts
        // from each connection a single 'my_Tick' value, and reports that
        // value to a console stream; both the acceptor and console stream are
        // supplied at construction.

        btlsc::TimedCbChannelAllocator *d_acceptor_p;      // incoming
                                                           // connections
// TBD 'btlso' is above 'btlsc'
// TBD  btlso::TcpTimerEventManager    *d_eventManager_p;  // Only needed for
                                                           // 'timeCb's use,
                                                           // could be erased
                                                           // otherwise.
        bsl::ostream&                   d_console;         // where to put tick
                                                           // info

        btlsc::TimedCbChannelAllocator::TimedCallback
                                        d_allocFunctor;    // reused

      private:
        void acceptCb(btlsc::TimedCbChannel     *clientChannel,
                      int                        status,
                      const bsls::TimeInterval&  timeout);
            // Called when a new client channel has been accepted.
            // ...

        void readCb(const char            *buffer,
                    int                    status,
                    int                    asyncStatus,
                    btlsc::TimedCbChannel *clientChannel);
            // Called when a 'my_Tick' value has been read from the channel.
            // ...

        void timeCb(int                 lastNumTicks,
                    int                *curNumTicks,
                    bsls::TimeInterval  lastTime);
            // To calculate the tick send/receive rate (Ticks/second).

      private:
        // NOT IMPLEMENTED
        my_TickReporter(const my_TickReporter&);
        my_TickReporter& operator=(const my_TickReporter&);

      public:
        my_TickReporter(bsl::ostream&                   console,
                        btlsc::TimedCbChannelAllocator *acceptor);
// TBD                  btlsc::TimedCbChannelAllocator *acceptor,
// TBD                  btlso::TcpTimerEventManager    *eventManager);
            // Create a non-blocking tick-reporter using the specified
            // 'acceptor' to establish incoming client connections, each
            // transmitting a single 'my_Tick' value; write these values to the
            // specified 'console' stream.  If the 'acceptor' is idle for more
            // than five minutes, print a message to the 'console' stream
            // supplied at construction and continue.  To guard against
            // malicious clients, a connection that does not produce a tick
            // value within one minute will be summarily dropped.

        ~my_TickReporter();
            // Destroy this server object.
    };

    #define VERSION_SELECTOR 20140601

    const double ACCEPT_TIME_LIMIT = 300;  // 5 minutes
    const double   READ_TIME_LIMIT =  60;  // 1 minutes

    static int calculateMyTickMessageSize()
        // Calculate and return the number of bytes encoding of a 'my_Tick'
        // value (called just once, see below).
    {
        double bid = 0, offer = 0;
        my_Tick dummy("dummy", bid, offer);

        bslx::ByteOutStream bos(VERSION_SELECTOR);
        bos << dummy;

        return bos.length();
    }

    static int myTickMessageSize()
        // Return the number of bytes in a BDEX byte-stream encoding of a
        // 'my_Tick' value without creating a runtime-initialized file-scope
        // static variable (which is link-order dependent).
    {
        static const int MESSAGE_SIZE = calculateMyTickMessageSize();
        return MESSAGE_SIZE;
    }

    void my_TickReporter::acceptCb(btlsc::TimedCbChannel     *clientChannel,
                                   int                        status,
                                   const bsls::TimeInterval&  timeout)
    {
        bsls::TimeInterval nextTimeout(timeout);

        if (clientChannel) {     // Successfully created a connection.

            const int                numBytes = ::myTickMessageSize();

            const bsls::TimeInterval now      = bdlt::CurrentTime::now();

            // Create one-time (buffered) read functor holding 'clientChannel'.

            using namespace bdlf::PlaceHolders;
            btlsc::TimedCbChannel::BufferedReadCallback readFunctor(
                bdlf::BindUtil::bind(
                        bdlf::MemFnUtil::memFn(&my_TickReporter::readCb, this),
                        _1, _2, _3,
                        clientChannel));

            // Install read callback (timeout, but no raw or async interrupt).

            if (clientChannel->timedBufferedRead(numBytes,
                                                 now + READ_TIME_LIMIT,
                                                 readFunctor)) {
                d_console << "Error: Unable even to register a read operation"
                             " on this channel." << bsl::endl;
                d_acceptor_p->deallocate(clientChannel);
            }
            nextTimeout += ACCEPT_TIME_LIMIT;
        }
        else if (0 == status) {  // Interrupted due to timeout event.
            d_console << "Acceptor timed out, continuing..." << bsl::endl;
            nextTimeout += ACCEPT_TIME_LIMIT;
        }
        else if (status > 0) {   // Interrupted by unspecified event.
            ASSERT(0); // Impossible, "async interrupts" were not authorized.
        }
        else {                   // Allocation operation is unable to succeed.
            ASSERT(status < 0);

            d_console << "Error: The channel allocator is not working now."
                      << bsl::endl;

            // Note that attempting to re-register an allocate operation below
            // will fail only if the channel allocator is permanently disabled.
        }

        // In all cases, attempt to reinstall the (reusable) accept callback.

        if (d_acceptor_p->timedAllocateTimed(
                                     d_allocFunctor,
                                     nextTimeout + bdlt::CurrentTime::now())) {
            d_console << "Error: unable to register accept operation."
                      << bsl::endl;
            // This server is broken.
        }
    }

    void my_TickReporter::readCb(const char            *buffer,
                                 int                    status,
                                 int                    asyncStatus,
                                 btlsc::TimedCbChannel *clientChannel)
    {
        static int curNumTicks = 0;

        const int msgSize = ::myTickMessageSize();

        enum { TIME_LEN = 15 };  // 15 seconds
        bsls::TimeInterval now = bdlt::CurrentTime::now();
        static bsls::TimeInterval lastTime(now);
        const bsls::TimeInterval PERIOD(TIME_LEN, 0);

        if (msgSize == status) {  // Encoded-tick value read successfully.
            ASSERT(buffer);

            ++curNumTicks;
            if (0 == (curNumTicks % 200000)) {
                cout << curNumTicks << " ticks has been received. " << endl;
                cout << "The current time value: " << now << endl;
            }
            // Just to print one tick to verify the format.
            if (1 == curNumTicks) {
                cout << "The first tick's value: ";
                ::myPrintTick(bsl::cout, buffer, msgSize);
            }
            if (1 == curNumTicks) {
                bsl::function<void()> timerFunctor(bdlf::BindUtil::bind(
                        bdlf::MemFnUtil::memFn(&my_TickReporter::timeCb, this),
                        0,
                        &curNumTicks,
                        now));
// TBD          d_eventManager_p->registerTimer(now + TIME_LEN, timerFunctor);
            }

            using namespace bdlf::PlaceHolders;
            btlsc::TimedCbChannel::BufferedReadCallback readFunctor(
                bdlf::BindUtil::bind(
                        bdlf::MemFnUtil::memFn(&my_TickReporter::readCb, this),
                        _1, _2, _3,
                        clientChannel));
            if (clientChannel->timedBufferedRead(msgSize,
                                                 now + READ_TIME_LIMIT,
                                                 readFunctor)) {
                d_console << "Error: Unable even to register a read operation"
                             " on this channel." << bsl::endl;
                d_acceptor_p->deallocate(clientChannel);
            }
        }
        else if (0 <= status) {   // Tick message was interrupted.

            ASSERT(buffer);  // Data in buffer is available for inspection (but
                             // remains in the channel's buffer).

            // Must be a TIMEOUT event since neither raw (partial) reads nor
            // (external) asynchronous interrupts were authorized.

            ASSERT(0 == asyncStatus);   // must be timeout event!

            d_console << "Error: Unable to read tick value before timing out; "
                         "read aborted."
                      << " status = " << status << "; asyncStatus = "
                      << asyncStatus << bsl::endl;
        }
        else { // Tick-message read failed.
            if (-1 == status) {
                d_console << "Error: Unable to read tick value from channel: "
                             " The connection is closed by the client."
                          << bsl::endl;
            }
            else {
                d_console << "Unknown Error: Unable to read tick from channel."
                          << bsl::endl;
            }
        }
    }

    void my_TickReporter::timeCb(int                 lastNumTicks,
                                 int                *curNumTicks,
                                 bsls::TimeInterval  lastTime)
    {
        int numTicks = *curNumTicks - lastNumTicks;
        enum { TIME_LEN = 15 };  // 15 seconds
        bsls::TimeInterval now = bdlt::CurrentTime::now();

        bsls::TimeInterval timePeriod = now - lastTime;
        double numSeconds = timePeriod.seconds()
                            + (double)timePeriod.nanoseconds() / 1000000000;
        cout << numTicks   << " ticks were sent in "
             << numSeconds << " seconds." << endl;

        cout << "The read rate is " << (int)(numTicks / numSeconds)
             << " Ticks/second."    << endl << endl;

        bsl::function<void()> timerFunctor(bdlf::BindUtil::bind(
                        bdlf::MemFnUtil::memFn(&my_TickReporter::timeCb, this),
                        *curNumTicks,
                        curNumTicks,
                        now));
// TBD  d_eventManager_p->registerTimer(now + TIME_LEN, timerFunctor);
    }

    my_TickReporter::my_TickReporter(
                                  bsl::ostream&                   console,
                                  btlsc::TimedCbChannelAllocator *acceptor)
// TBD                            btlsc::TimedCbChannelAllocator *acceptor,
// TBD                            btlso::TcpTimerEventManager    *eventManager)
    : d_console(console)
    , d_acceptor_p(acceptor)
// TBD
//  , d_eventManager_p(eventManager)
    {
        ASSERT(&d_console);
        ASSERT(d_acceptor_p);

        // Attempt to install the first accept callback.

        bsls::TimeInterval timeout = bdlt::CurrentTime::now()
                                     + ACCEPT_TIME_LIMIT;

        // load reusable allocate functor

        using namespace bdlf::PlaceHolders;
        d_allocFunctor = bdlf::BindUtil::bind(
                      bdlf::MemFnUtil::memFn(&my_TickReporter::acceptCb, this),
                      _1, _2,
                      timeout);

        if (d_acceptor_p->timedAllocateTimed(d_allocFunctor, timeout)) {
            d_console << "Error: Unable to install accept operation."
                      << bsl::endl;
            // This server is broken.
        }
    }

    my_TickReporter::~my_TickReporter()
    {
        ASSERT(&d_console);
        ASSERT(d_acceptor_p);
    }

//  int main(int argc, const char *argv[])
//  {
//      enum { DEFAULT_PORT = 5000 };
//
//      const int portNumber = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;
//
//      btlso::IPv4Address address;
//      address.setPortNumber(portNumber);
//
//      btlso::TcpTimerEventManager manager;    // concrete manager
//      btlso::InetStreamSocketFactory<btlso::IPv4Address> sf;
//      btlsos::TcpTimedCbAcceptor acceptor(&sf, &manager);
//
//      ASSERT(0 == acceptor.open(address, DEFAULT_QUEUE_SIZE));
//
//      if (acceptor.isInvalid()) {
//          bsl::cout << "Error: Unable to create acceptor" << bsl::endl;
//          return -1;                                                // RETURN
//      }
//
//      my_TickReporter reporter(bsl::cout, &acceptor, &manager);
//
//      cout << "server: " << address
//           << "; begins to dispatch()....." << endl;
//
//      while (0 != manager.dispatch(0)) {
//          // Do nothing.
//      }
//  }
//
//..
// Note that when the server can go out of scope before all events are
// processed, it is important to implement it using the envelope/letter pattern
// where a *counted* *handle* to the internal letter representation is present
// in each active callback functor to preserve the server's internal state
// until all functors operating on it have been invoked.
//
///Client Side
///-----------
// In order to use this 'my_Tick' reporting service, clients will need to know
// where such a service resides and how to establish such connections on
// demand.  We will use the 'btlsc::TimedCbChannelAllocator' protocol to
// abstract those details out of the stable software that generates (or
// forwards) ticks.  For the purposes of this example, let's assume that ticks
// are generated in some ASCII format and arrive in fixed size chunks (e.g., 80
// bytes) from a separate process.  Note that of the three callback methods
// 'readCb', 'connectCb', and 'writeCb', only 'readCb' requires no additional,
// call-specific user data; hence we can easily create it once at construction,
// and productively cache it for repeated used.  We will choose to reload the
// others each time (which is admittedly somewhat less efficient).
//..
    class my_TickerplantSimulator {
        // Accept raw tick values in ASCII sent as fixed-sized packets via a
        // single 'btlsc::TimedCbChannel' and send them asynchronously one by
        // one to a peer (or similar peers) connected via channels provided via
        // a 'btlsc::TimedCbChannelAllocator'.  Both the output channel
        // allocator and the input channel are supplied at construction.

        btlsc::TimedCbChannelAllocator *d_connector_p;  // outgoing connections
// TBD  btlso::TcpTimerEventManager    *d_eventManager_p;
        bsl::ostream&                   d_console;      // where to write
                                                        // errors
        const int                       d_inputSize;    // input packet size

        bsl::function<void(btlsc::TimedCbChannel*, int)>
                                        d_allocateFunctor;
        btlsc::TimedCbChannel::BufferedReadCallback
                                        d_readFunctor;  // reused

      private:
        void connectCb(btlsc::TimedCbChannel *serverChannel,
                       int                    status);
            // Called when a new server channel has been established.
            // ...

        void writeCb(int                    status,
                     int                    asyncStatus,
                     btlsc::TimedCbChannel *serverChannel,
                     int                    msgSize,
                     int                    maxTicks);
            // Called when a write operation to the server channel ends.
            // ...

        void timeCb(int                 lastNumTicks,
                    int                *curNumTicks,
                    int                 maxTicks,
                    bsls::TimeInterval  lastTime);
            // To calculate the tick send/receive rate (Ticks/second).

      private:
        // NOT IMPLEMENTED
        my_TickerplantSimulator(const my_TickerplantSimulator&);
        my_TickerplantSimulator& operator=(const my_TickerplantSimulator&);

      public:
        my_TickerplantSimulator(
                              bsl::ostream&                   console,
                              btlsc::TimedCbChannelAllocator *connector,
// TBD                        btlso::TcpTimerEventManager    *d_eventManager_p,
                              int                             inputSize);
            // Create a non-blocking ticker-plant simulator using the specified
            // 'input' channel to read ASCII tick records of the specified
            // 'inputSize' and convert each record to a 'my_Tick' structure;
            // each tick value is sent asynchronously to a peer via a distinct
            // channel obtained from the specified 'connector', reporting any
            // errors to the specified 'console'.  If 'connector' fails or is
            // unable to succeed after 30 seconds, or if transmission itself
            // exceeds 10 seconds, display a message on 'console' and abort the
            // transmission.  If three successive reads of the input channel
            // fail to produce a valid ticks, invalidate the channel and shut
            // down this simulator.  The behavior is undefined unless
            // '0 < inputSize'.

        ~my_TickerplantSimulator();
            // Destroy this simulator object.
        int connect();
    };

    static const bsls::TimeInterval CONNECT_TIME_LIMIT(30, 0);  // 30 seconds
    static const bsls::TimeInterval WRITE_TIME_LIMIT(3600, 0);  // 1 hour

    void my_TickerplantSimulator::connectCb(
                                          btlsc::TimedCbChannel *serverChannel,
                                          int                    status)
    {
        if (serverChannel) {            // Successfully created a connection.
            struct Tick {               // for usage example
                char   d_name[5];
                double d_bestBid;
                double d_bestOffer;
            } ticks[] = {
              {"MSF1", 20.5, 10.5},
              {"MSF2", 21.5, 11.5},
              {"MSF3", 22.5, 12.5},
              {"MSF4", 23.5, 13.5},
              {"MSF5", 24.5, 14.5},
              {"MSF6", 25.5, 15.5},
              {"MSF7", 26.5, 16.5},
              {"MSF8", 27.5, 17.5},
            };
            const int NUM_TICKS = sizeof ticks / sizeof *ticks;
            enum { NUM_ITERATIONS = 1000000 };

            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                 my_Tick tick(ticks[i % NUM_TICKS].d_name,
                              ticks[i % NUM_TICKS].d_bestBid,
                              ticks[i % NUM_TICKS].d_bestOffer);

                 bslx::ByteOutStream bos(VERSION_SELECTOR);
                 bos << tick;
                 int msgSize = bos.length();

                 using namespace bdlf::PlaceHolders;
                 btlsc::TimedCbChannel::WriteCallback functor(
                     bdlf::BindUtil::bind(
                      bdlf::MemFnUtil::memFn(&my_TickerplantSimulator::writeCb,
                                             this),
                      _1, _2,
                      serverChannel,
                      msgSize,
                      (int)NUM_ITERATIONS));

                 bsls::TimeInterval now = bdlt::CurrentTime::now();

                 // Initiate a timed non-blocking write operation.
                 if (serverChannel->timedBufferedWrite(
                                               bos.data(),
                                               msgSize,
                                               now + WRITE_TIME_LIMIT,
                                               functor)) {

                     d_console << "Error: Unable even to register a write"
                                  " operation on this channel." << bsl::endl;

                     d_connector_p->deallocate(serverChannel);
                 }
            }
        }
        else if (status > 0) {  // Interrupted due to external event.
            ASSERT(0);  // Impossible, not authorized.
        }
        else if (0 == status) {  // Interrupted due to timeout event.
            d_console << "Error: Connector timed out, transition aborted."
                      << bsl::endl;
        }
        else {  // Connector failed.
            ASSERT(0 < status);

            bsl::cout << "Error: Unable to connect to server." << bsl::endl;

            // The server is down; invalidate the input channel, allowing
            // existing write operations to complete before the simulator
            // shuts down.
        }
    }

    void my_TickerplantSimulator::writeCb(int                    status,
                                          int                    asyncStatus,
                                          btlsc::TimedCbChannel *serverChannel,
                                          int                    msgSize,
                                          int                    maxTicks)
    {
        static int curNumTicks  = 0;
        static int lastNumTicks = 0;

        ASSERT(serverChannel);
        ASSERT(0 < msgSize);
        ASSERT(status <= msgSize);

        enum { TIME_LEN = 15 };  // 15 seconds
        bsls::TimeInterval now = bdlt::CurrentTime::now();
        static bsls::TimeInterval lastTime(now);
        const bsls::TimeInterval PERIOD(TIME_LEN, 0);

        // Tracing message: Print out any abnormal tick sending information.
        if (msgSize != status) {
            d_console << "msgSize = " << msgSize << "; status = "
                      << status << "; asyncStatus = " << asyncStatus
                      << bsl::endl;
        }

        if (msgSize == status) {
            // Encoded tick value written successfully.
            ++curNumTicks;
            if (0 == (curNumTicks % 200000)) {
                d_console << curNumTicks << " ticks has been sent. "
                          << bsl::endl;
                d_console << "The current time value: " << now << bsl::endl;
            }

            if (1 == curNumTicks) {
                bsl::function<void()> timerFunctor(bdlf::BindUtil::bind(
                       bdlf::MemFnUtil::memFn(&my_TickerplantSimulator::timeCb,
                                              this),
                       0,
                       &curNumTicks,
                       maxTicks,
                       now));
// TBD          d_eventManager_p->registerTimer(now + TIME_LEN, timerFunctor);
                d_console << "registered a timer.  time" << now + TIME_LEN
                          << bsl::endl;
            }
        }
        else if (0 <= status) {   // Tick message timed out.

            ASSERT(0 == asyncStatus    // only form of partial-write authorized
                   || 0 >  asyncStatus // This operations was dequeued due to
                   && 0 == status);    // a previous partial write operation.

            if (0 < asyncStatus) {
                d_console << "The request is interrupted." << bsl::endl;
            }
            else if (0 == asyncStatus) {
                d_console << "Write of tick data timed out." << bsl::endl;

                if (status > 0) {
                    d_console << "Partial tick data written;"
                              << "  status = " << status
                              << "asyncStatus = " << asyncStatus << bsl::endl;
                }
                else {
                    d_console << "No data was written; channel is still valid."
                              << bsl::endl;
                }
            }
            else {
                ASSERT(0 > asyncStatus && 0 == status);

                if (0 == status) {
                    d_console << "This operation was dequeued. "
                              << "status = " << status << "; asyncStatus = "
                              << asyncStatus << bsl::endl;
                }
                else {
                    d_console << "Write (efficiently) transmitted " << status
                              << " of bytes." << bsl::endl;
                }
                ASSERT(0);
            }
        }
        else {  // Tick message write failed.
            ASSERT(0 > status);

            d_console << "Error: Unable to write tick value to server."
                      << bsl::endl;
        }
    }

    void my_TickerplantSimulator::timeCb(int                 lastNumTicks,
                                         int                *curNumTicks,
                                         int                 maxTicks,
                                         bsls::TimeInterval  lastTime)
    {
        int numTicks = *curNumTicks - lastNumTicks;
        enum { TIME_LEN = 15 };  // 15 seconds
        bsls::TimeInterval now = bdlt::CurrentTime::now();

        bsls::TimeInterval timePeriod = now - lastTime;
        double numSeconds = timePeriod.seconds()
                            + (double)timePeriod.nanoseconds() / 1000000000;
        cout << numTicks   << " ticks were sent in "
             << numSeconds << " seconds." << endl;

        cout << "The send rate is " << (int)(numTicks / numSeconds)
             << " Ticks/second."    << endl << endl;
        if (*curNumTicks < maxTicks) {
            bsl::function<void()> timerFunctor(bdlf::BindUtil::bind(
                       bdlf::MemFnUtil::memFn(&my_TickerplantSimulator::timeCb,
                                              this),
                       *curNumTicks, curNumTicks, maxTicks,
                       now));
// TBD      d_eventManager_p->registerTimer(now + TIME_LEN, timerFunctor);
        }
    }

    my_TickerplantSimulator::
          my_TickerplantSimulator(bsl::ostream&                   console,
                                  btlsc::TimedCbChannelAllocator *connector,
// TBD                            btlso::TcpTimerEventManager    *eventManager,
                                  int                             inputSize)
    : d_connector_p(connector)
// TBD
//  , d_eventManager_p(eventManager)
    , d_console(console)
    , d_inputSize(inputSize)
    {
        ASSERT(&console);
        ASSERT(connector);
// TBD  ASSERT(eventManager);
        ASSERT(0 < inputSize);

        using namespace bdlf::PlaceHolders;
        d_allocateFunctor = bdlf::BindUtil::bind(
            bdlf::MemFnUtil::memFn(&my_TickerplantSimulator::connectCb, this),
                                   _1, _2);
    }

    int my_TickerplantSimulator::connect() {
        return d_connector_p->timedAllocateTimed(
                                d_allocateFunctor,
                                bdlt::CurrentTime::now() + CONNECT_TIME_LIMIT);

    }

    my_TickerplantSimulator::~my_TickerplantSimulator()
    {
        ASSERT(&d_console);
        ASSERT(d_connector_p);
        ASSERT(0 < d_inputSize);
    }

//  int main(int argc, const char *argv[])
//  {
//      const char *const DEFAULT_HOST = "widget";
//      enum { DEFAULT_PORT = 5000 };
//      enum { DEFAULT_SIZE = 10 };
//
//      const char *hostName = argc > 1 ? argv[1]       : DEFAULT_HOST;
//      const int portNumber = argc > 2 ? atoi(argv[2]) : DEFAULT_PORT;
//      const int inputSize  = argc > 3 ? atoi(argv[3]) : DEFAULT_SIZE;
//
//      // INBOUND:
//      // This simulator accepts connections on port 'DEFAULT_PORT'
//      // only.
//
//      btlso::IPv4Address serverAddress;
//
//      int ret = btlso::ResolveUtil::getAddress(&serverAddress, hostName);
//      ASSERT(0 == ret);
//
//      serverAddress.setPortNumber(portNumber);
//      cout << "serverAddress = " << serverAddress << endl;
//
//      btlso::InetStreamSocketFactory<btlso::IPv4Address> sf;
//      btlso::TcpTimerEventManager::Hint
//      infrequentRegistrationHint =
//                   btlso::TcpTimerEventManager::e_INFREQUENT_REGISTRATION;
//      btlso::TcpTimerEventManager manager(infrequentRegistrationHint);
//
//      btlsos::TcpTimedCbConnector connector(&sf, &manager);
//      connector.setPeer(serverAddress);
//
//      my_TickerplantSimulator simulator(bsl::cout, &connector, &manager,
//                                        inputSize);
//      ASSERT(0 == simulator.connect());
//
//      cout << "client begins to dispatch()....." << endl;
//
//      while (0 != manager.dispatch(0)) {
//          // Do nothing.
//      }
//  }
//
//..
// Please remember that these example code snippets are intended to illustrate
// the use of 'btlsc::TimedCbChannelAllocator' and do not represent
// production-quality software.

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int         test = argc > 1 ? atoi(argv[1]) : 0;
    const bool     verbose = argc > 2;
    const bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   Ensure this class is a properly defined protocol.
        //
        // Concerns:
        //: 1 The protocol is abstract: no objects of it can be created.
        //:
        //: 2 The protocol has no data members.
        //:
        //: 3 The protocol has a virtual destructor.
        //:
        //: 4 All methods of the protocol are pure virtual.
        //:
        //: 5 All methods of the protocol are publicly accessible.
        //
        // Plan:
        //: 1 Define a concrete derived implementation, 'ProtocolClassTestImp',
        //:   of the protocol.
        //:
        //: 2 Create an object of the 'bsls::ProtocolTest' class template
        //:   parameterized by 'ProtocolClassTestImp', and use it to verify
        //:   that:
        //:
        //:   1 The protocol is abstract. (C-1)
        //:
        //:   2 The protocol has no data members. (C-2)
        //:
        //:   3 The protocol has a virtual destructor. (C-3)
        //:
        //: 3 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that
        //:   non-creator methods of the protocol are:
        //:
        //:   1 virtual, (C-4)
        //:
        //:   2 publicly accessible. (C-5)
        //
        // Testing:
        //   virtual ~TimedCbChannelAllocator();
        //   virtual int allocate(const Callback&, int = 0) = 0;
        //   virtual int timedAllocate(const Cb&, const TI&, int = 0) = 0;
        //   virtual int allocateTimed(const TimedCallback&, int = 0) = 0;
        //   virtual int timedAllocateTimed(const TimedCb&, TI&, int = 0) = 0;
        //   virtual void cancelAll() = 0;
        //   virtual void deallocate(CbChannel *) = 0;
        //   virtual void invalidate() = 0;
        //   virtual int isInvalid() const = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nCreate a test object.\n";

        bsls::ProtocolTest<ProtocolClassTestImp> testObj(veryVerbose);

        if (verbose) cout << "\nVerify that the protocol is abstract.\n";

        ASSERT(testObj.testAbstract());

        if (verbose) cout << "\nVerify that there are no data members.\n";

        ASSERT(testObj.testNoDataMembers());

        if (verbose) cout << "\nVerify that the destructor is virtual.\n";

        ASSERT(testObj.testVirtualDestructor());

        if (verbose) cout << "\nVerify that methods are public and virtual.\n";

        const bsls::TimeInterval TI;

        BSLS_PROTOCOLTEST_ASSERT(testObj, allocate(&myCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedAllocate(&myCbFn, TI, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, allocateTimed(&myTimedCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedAllocateTimed(&myTimedCbFn, TI,
                                                             0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, cancelAll());

        BSLS_PROTOCOLTEST_ASSERT(testObj, deallocate(0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, invalidate());

        BSLS_PROTOCOLTEST_ASSERT(testObj, isInvalid());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
