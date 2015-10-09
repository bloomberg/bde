// ball_observer.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_observer.h>

#include <ball_recordattributes.h>              // for testing only
#include <ball_record.h>                        // for testing only
#include <ball_context.h>                       // for testing only
#include <ball_transmission.h>                  // for testing only
#include <ball_userfields.h>                    // for testing only

#include <bdlt_datetime.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_epochutil.h>
#include <bslma_testallocator.h>

#include <bslim_testutil.h>
#include <bsls_protocoltest.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>       // time()

#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class.  We need to verify that (1) a
// concrete derived class compiles and links, and (2) that a usage example
// obtains the behavior specified by the protocol from the concrete subclass.
//-----------------------------------------------------------------------------
// [ 1] virtual ~ball::Observer();
// [ 1] virtual void publish(const record&, const context&);
// [ 1] virtual void publish(const sharedptr<record>&, const context&);
// [ 1] virtual void releaseRecords();
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 2] USAGE TEST - Make sure main usage example compiles and works properly.

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

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                       CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------

struct ObserverTest : bsls::ProtocolTestImp<ball::Observer> {
    void publish(const ball::Record&, const ball::Context&)  { markDone(); }
    void releaseRecords() { markDone(); }
};

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

// my_ostreamobserver.h

class my_OstreamObserver : public ball::Observer {
    ostream& d_stream;

  public:
    my_OstreamObserver(ostream& stream) : d_stream(stream) { }
    virtual ~my_OstreamObserver();
    virtual void publish(const ball::Record&  record,
                         const ball::Context& context);
};

// my_ostreamobserver.cpp

my_OstreamObserver::~my_OstreamObserver() { }

void my_OstreamObserver::publish(const ball::Record&  record,
                                 const ball::Context& context)
{

    d_stream << endl;  // skip a line

    switch (context.transmissionCause()) {
      case ball::Transmission::e_PASSTHROUGH: {
        d_stream << "Single Pass-through Message:" << endl;
      } break;
      case ball::Transmission::e_TRIGGER_ALL: {
        d_stream << "Remotely ";      // no 'break'; concatenated output
      }
      case ball::Transmission::e_TRIGGER: {
        d_stream << "Triggered Publication Sequence: Message "
                 << context.recordIndex() + 1  // Account for 0-based index.
                 << " of " << context.sequenceLength()
                 << ':' << endl;
      } break;
      default: {
        d_stream << "***ERROR*** Unknown Message Cause:" << endl;
      } break;
    }

    d_stream << "\tTimestamp:  " << record.fixedFields().timestamp()
             << endl;
    d_stream << "\tProcess ID: " << record.fixedFields().processID()
             << endl;
    d_stream << "\tThread ID:  " << record.fixedFields().threadID()
             << endl;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        {
            char buf[2048];
            ostrstream out(buf, sizeof buf);

            my_OstreamObserver    myObserver(out);
            ball::Observer&        observer =
                                     dynamic_cast<ball::Observer&>(myObserver);
            bdlt::Datetime         now;
            ball::RecordAttributes fixed;
            ball::UserFields       userValues;


            if (verbose)
                cout << "Publish a single message (a sequence of 1)." << endl;
            {
                out.seekp(0);
                now = bdlt::EpochUtil::convertFromTimeT(time(0));
                fixed.setTimestamp(now);
                fixed.setProcessID(100);
                fixed.setThreadID(0);

                bsl::shared_ptr<const ball::Record> handle(
                           new (testAllocator) ball::Record(fixed, userValues),
                             &testAllocator);
                observer.publish(
                            handle,
                            ball::Context(ball::Transmission::e_PASSTHROUGH,
                            0,
                            1));
                out << ends;

                if (veryVerbose) cout << buf << endl;
            }

            if (verbose)
                cout << "Publish a sequence of three messages." << endl;
            {
                out.seekp(0);
                const int NUM_MESSAGES = 3;
                for (int n = 0; n < NUM_MESSAGES; ++n) {
                    now = bdlt::EpochUtil::convertFromTimeT(time(0));
                    fixed.setTimestamp(now);
                    fixed.setProcessID(201 + n);
                    fixed.setThreadID(31 + n);

                    bsl::shared_ptr<const ball::Record> handle(
                           new (testAllocator) ball::Record(fixed, userValues),
                             &testAllocator);
                    observer.publish(
                                handle,
                                ball::Context(ball::Transmission::e_TRIGGER,
                                              n,
                                              NUM_MESSAGES));
                }
                out << ends;
                if (veryVerbose) cout << buf << endl;
            }
            if (verbose)
                cout << "Invoke 'releaseRecords' method." << endl;
            {
                observer.releaseRecords();
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //
        // Concerns:
        //   We must ensure that (1) a subclass of the 'ball::Observer' class
        //   compiles and links when all virtual functions are defined, and
        //   (2) the functions are in fact virtual.
        //
        // Plan:
        //   Construct an object of a class derived from 'ball::Observer' and
        //   bind a 'ball::Observer' reference to the object.  Using the base
        //   class reference, invoke the 'publish' method and destructor.
        //   Verify that the correct implementations of the methods are called.
        //
        // Testing:
        //   virtual ~ball::Observer();
        //   virtual void publish(const record&, const context&) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        bsls::ProtocolTest<ObserverTest> t(veryVerbose);

        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t, publish(ball::Record(), ball::Context()));

        bsl::shared_ptr<const ball::Record> handle(
                              new (testAllocator) ball::Record(&testAllocator),
                              &testAllocator);
        BSLS_PROTOCOLTEST_ASSERT(t, publish(handle, ball::Context()));

        BSLS_PROTOCOLTEST_ASSERT(t, releaseRecords());
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
