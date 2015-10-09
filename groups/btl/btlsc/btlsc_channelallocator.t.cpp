// btlsc_channelallocator.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsc_channelallocator.h>

#include <btlsc_channel.h>       // for testing only
#include <btlsc_flag.h>          // for testing only
#include <btlsc_timedchannel.h>  // for testing only

#include <bdls_testutil.h>

#include <bdlt_currenttime.h>

#include <bsls_protocoltest.h>
#include <bsls_timeinterval.h>

#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>

#include <bsl_cstdlib.h>          // 'atoi'
#include <bsl_cstdio.h>           // 'snprintf'
#include <bsl_cstring.h>          // 'strcpy'
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
// [ 1] virtual ~ChannelAllocator();
//
// MANIPULATORS
// [ 1] virtual Channel *allocate(int *, int = 0) = 0;
// [ 1] virtual TimedChannel *allocateTimed(int *, int = 0) = 0;
// [ 1] virtual void deallocate(Channel *) = 0;
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

typedef btlsc::ChannelAllocator ProtocolClass;

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    btlsc::Channel *allocate(int *, int = 0)           { return markDone(); }

    btlsc::TimedChannel *allocateTimed(int *, int = 0) { return markDone(); }

    void deallocate(btlsc::Channel *)                  {        markDone(); }

    void invalidate()                                  {        markDone(); }

    int isInvalid() const                              { return markDone(); }
};

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//
///Server Side
///- - - - - -
// Let's assume that we have a function that knows how to print platform
// neutral encodings of type 'my_Tick':
//..
    #define VERSION_SELECTOR 20140601

    static int calculateMyTickMessageSize()
        // Calculate and return the number of bytes in a BDEX byte-stream
        // encoding of a (dummy) 'my_Tick' value (called just once, see below).
    {
        my_Tick dummy;
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
//..
// The following class illustrates how we might implement a tick-reporter
// server using just the 'btlsc::ChannelAllocator' and 'btlsc::Channel'
// protocols.
//..
    class my_TickReporter {
        // This class implements a server that accepts connections, extracts
        // from each connection a single 'my_Tick' value, and reports that
        // value to a console stream.  Both the acceptor and console stream
        // are supplied at construction.

        btlsc::ChannelAllocator *d_acceptor_p;  // incoming connections
        bsl::ostream&            d_console;     // where to put tick info

      private:
        // NOT IMPLEMENTED
        my_TickReporter(const my_TickReporter&);
        my_TickReporter& operator=(const my_TickReporter&);

      public:
        my_TickReporter(bsl::ostream&            console,
                        btlsc::ChannelAllocator *acceptor);
            // Create a synchronous tick-reporter using the specified
            // 'acceptor' to establish incoming client connections, each
            // transmitting a single 'my_Tick' value; write these values to
            // the specified 'console' stream.  If the 'acceptor' is idle for
            // more than five minutes, print a message to the 'console' stream
            // supplied at construction and continue.  To guard against
            // malicious clients, a connection that does not produce a tick
            // value within one minute will be summarily dropped.

        ~my_TickReporter();
            // Destroy this server object.

        // MANIPULATORS
        int reportTicker();
            // Call 'd_acceptor_p's 'allocate' to establish a channel.
    };

    my_TickReporter::my_TickReporter(bsl::ostream&            console,
                                     btlsc::ChannelAllocator *acceptor)
    : d_console(console)
    , d_acceptor_p(acceptor)
    {
        ASSERT(&d_console);
        ASSERT(d_acceptor_p);
    }

    my_TickReporter::~my_TickReporter()
    {
        ASSERT(&d_console);
        ASSERT(d_acceptor_p);
    }

    int my_TickReporter::reportTicker()
    {
        int status = 0;

        while (1) {
            btlsc::Channel *newChannel = d_acceptor_p->allocate(&status);

            if (newChannel) {     // Successfully created a connection.
                const int numBytes = ::myTickMessageSize();

                const char *buffer = 0;
                int ret = newChannel->bufferedRead(&status,
                                                   &buffer,
                                                   numBytes);
                if (0 >= ret) {
                    d_console << "Error: The read operation timeout "
                                 "on this channel."
                              << bsl::endl;
                    d_acceptor_p->deallocate(newChannel);
                }
                else {  // Read successfully.
                    ::myPrintTick(d_console, buffer, numBytes);
                    d_acceptor_p->deallocate(newChannel);
                }
            }
            else if (status > 0) {   // Interrupted by unspecified event.
                ASSERT(0); // Impossible, "async interrupts" not authorized.
            }
            else {         // Allocation operation is unable to succeed.
                ASSERT(status < 0);

                d_console << "Error: The channel allocator is not working now."
                          << bsl::endl;
                ASSERT(0);
            }
        }
    }
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
    static
    int parseTick(my_Tick *result, const char *buffer, int numBytes)
        // Parse a tick value from the specified input 'buffer' of the
        // specified 'numBytes' and load that value into the specified
        // 'result'.  Return 0 on success, and a non-zero value (with no effect
        // on 'result') otherwise.  The behavior is undefined unless
        // '0 <= numBytes'.
    {
        // ...

        return 0;
    }

    class my_TickerplantSimulator {
        // Accept raw tick values in ASCII sent as fixed-sized packets via a
        // single 'btlsc::Channel' and send them synchronously one by one to a
        // peer (or similar peers) connected via channels provided via a
        // 'btlsc::ChannelAllocator'.  Both the output channel allocator and
        // the input channel are supplied at construction.

        enum { MAX_PARSER_ERRORS = 3 };

        btlsc::ChannelAllocator *d_connector_p;       // outgoing connections
        btlsc::Channel          *d_input_p;           // incoming packets
        bsl::ostream&            d_console;           // where to write errors
        const int                d_inputSize;         // input packet size
        int                      d_parserErrorCount;  // consecutive errors

      private:
        // NOT IMPLEMENTED
        my_TickerplantSimulator(const my_TickerplantSimulator&);
        my_TickerplantSimulator& operator=(const my_TickerplantSimulator&);

      public:
        my_TickerplantSimulator(bsl::ostream&            console,
                                btlsc::ChannelAllocator *connector,
                                btlsc::Channel          *input,
                                int                      inputSize);
            // Create a non-blocking ticker-plant simulator using the
            // specified 'input' channel to read ASCII tick records of the
            // specified 'inputSize' and convert each record to a 'My_Tick'
            // structure; each tick value is sent synchronously to a peer via a
            // distinct channel obtained from the specified 'connector',
            // reporting any errors to the specified 'console'.  If 'connector'
            // fails or is unable to succeed after 30 seconds, or if
            // transmission itself exceeds 10 seconds, display a message on
            // 'console' and abort the transmission.  If three successive reads
            // of the input channel fail to produce a valid ticks, invalidate
            // the channel and shut down this simulator.  The behavior is
            // undefined unless '0 < inputSize'.

        int sendTicker();
            // Build 'My_Tick' objects by reading from channel of 'd_input_p'
            // and send the built tick, if it is valid, to the peer.  Return 0
            // on success, and a negative value otherwise.

        ~my_TickerplantSimulator();
            // Destroy this simulator object.
    };

    my_TickerplantSimulator::my_TickerplantSimulator(
                                            bsl::ostream&            console,
                                            btlsc::ChannelAllocator *connector,
                                            btlsc::Channel          *input,
                                            int                      inputSize)
    : d_connector_p(connector)
    , d_console(console)
    , d_input_p(input)
    , d_inputSize(inputSize)
    , d_parserErrorCount(0)
    {
        ASSERT(&console);
        ASSERT(connector);
        ASSERT(input);
        ASSERT(0 < inputSize);
    }

    int my_TickerplantSimulator::sendTicker()
    {
        const char *buffer    = 0;
        int         status    = 0;
        int         augStatus = 0;

        while (0 < d_input_p->bufferedRead(&augStatus, &buffer, d_inputSize)) {
            my_Tick tick;
            if (0 == ::parseTick(&tick, buffer, d_inputSize)) {
                d_parserErrorCount = 0;  // Successfully parsed tick value.

                // Initiate a non-blocking  allocate operation.

                btlsc::Channel *newChannel = d_connector_p->allocate(&status);
                if (0 == newChannel) {
                    d_console << "Error: Unable to install connect operation."
                              << bsl::endl;
                }
                else {
                    bslx::ByteOutStream bos(VERSION_SELECTOR);
                    bos << tick;
                    int msgSize = bos.length();
                    if (0 >= newChannel->write(bos.data(), msgSize)) {
                         d_console << "Error: Unable even to register a write"
                                      " operation on this channel."
                                   << bsl::endl;
                    }
                    d_connector_p->deallocate(newChannel);
                }
            }
            else {  // parsing error
                d_console << "Error: Unable to parse tick data!" << bsl::endl;

                ++d_parserErrorCount;

                if (d_parserErrorCount >= MAX_PARSER_ERRORS) {
                    d_console << "Reached maximum parser error limit;"
                                 " invalidating input channel." << bsl::endl;

                    d_input_p->invalidate();
                }
                else {
                    d_console << "Continuing with next record." << bsl::endl;
                }
            }
        }

        if (-1 == augStatus) {  // EOF
            return 0;                                                 // RETURN
        }
        else {
            return -1;                                                // RETURN
        }
    }

    my_TickerplantSimulator::~my_TickerplantSimulator()
    {
        ASSERT(&d_console);
        ASSERT(d_connector_p);
        ASSERT(0 < d_inputSize);
        ASSERT(0 <= d_parserErrorCount);
        ASSERT(d_parserErrorCount <= MAX_PARSER_ERRORS);
    }
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
//      ASSERT(0 == simulator.sendTicker());
//
//      return 0;
//  }
//..
// Please remember that these example code snippets are intended to illustrate
// the use of 'btlsc::ChannelAllocator' and do not represent production-quality
// software.

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
        //   virtual ~ChannelAllocator();
        //   virtual Channel *allocate(int *, int = 0) = 0;
        //   virtual TimedChannel *allocateTimed(int *, int = 0) = 0;
        //   virtual void deallocate(Channel *) = 0;
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

        BSLS_PROTOCOLTEST_ASSERT(testObj, allocate(0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, allocateTimed(0, 0));

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
