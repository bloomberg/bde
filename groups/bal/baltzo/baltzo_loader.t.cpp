// baltzo_loader.t.cpp                                                -*-C++-*-
#include <baltzo_loader.h>

#include <baltzo_localtimedescriptor.h>
#include <baltzo_zoneinfo.h>

#include <bdlt_datetime.h>

#include <bdlt_epochutil.h>

#include <bsls_protocoltest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a protocol class the purpose of which is to
// provide an interface for loading time zones.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o It is possible to create a concrete implementation the protocol.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] virtual ~baltzo::Loader();
//
// MANIPULATORS
// [ 1] virtual int loadTimeZone(Zoneinfo *tz, const char *tzId) = 0;
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
//=============================================================================

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)
#define L_ __LINE__                           // current Line number

// ============================================================================
//                    GLOBAL CLASSES/TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------
namespace {

typedef baltzo::Loader ProtocolClass;

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    int loadTimeZone(baltzo::Zoneinfo *, const char *) { return markDone(); }
};

}  // close unnamed namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Implementing 'baltzo::Loader'
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates an implementation of 'baltzo::Loader' that can
// only return data for "America/New_York".
//
// Note that in general, an implementation of 'baltzo::Loader' should obtain
// time-zone information from an external data source (see
// 'baltzo_datafileloader').
//
// First, we define the interface of our implementation:
//..
    class MyLoaderImp : public baltzo::Loader {
        // This class provides a concrete implementation of the
        // 'baltzo::Loader' protocol (an abstract interface) for obtaining a
        // time zone.  This test implementation contains only partial data of
        // the "America/New_York" time zone, and is unable to obtain time-zone
        // information for any other time zones.

      public:
        // CREATORS
        MyLoaderImp();
            // Create a 'MyLoaderImp' object.

        ~MyLoaderImp();
            // Destroy this object.

        // MANIPULATORS
        virtual int loadTimeZone(baltzo::Zoneinfo *result,
                                 const char       *timeZoneId);
            // Load into the specified 'result' the "Zoneinfo" time zone
            // information for the time zone identified by the specified
            // 'timeZoneId'.  Return 0 on success, and non-zero otherwise.
    };
//..
// Then, we implement the creators, trivially, as this class contains no
// instance data members.
//..
    MyLoaderImp::MyLoaderImp()
    {
    }

    MyLoaderImp::~MyLoaderImp()
    {
    }
//..
// Next, we implement the 'loadTimeZone' function:
//..
    int MyLoaderImp::loadTimeZone(baltzo::Zoneinfo *result,
                                   const char      *timeZoneId)
    {
//..
// Then, we check the 'timeZoneId' equals to "America/New_York' as this
// implementation is designed to demonstrate only one time zone:
//..
        if (0 != strcmp("America/New_York", timeZoneId)) {
            return 1;                                                 // RETURN
        }
//..
// Next, we load 'result' with the time zone identifier for New York
//..
        result->setIdentifier("America/New_York");
//..
// Then, we create two local-time descriptors, one for standard time and one
// for daylight-saving time.
//..
        baltzo::LocalTimeDescriptor edt(-14400, true, "EDT");
        baltzo::LocalTimeDescriptor est(-18000, false, "EST");
//..
// Next, we create a series of transitions between these local time descriptors
// for the years 2007-2011.  Note that the United States transitions to
// daylight saving time on the second Sunday in March, at 2am local time (7am
// UTC), and transition back to standard time on the first Sunday in November
// at 2am local time (6am UTC), resulting in an even number of transitions.
// Also note that, the rules for generating these transitions were changed in
// 2007, and may be changed again at some point in the future.
//..
        static const bdlt::Datetime TRANSITION_TIMES[] = {
            bdlt::Datetime(2007,  3, 11, 7),
            bdlt::Datetime(2007, 11,  4, 6),
            bdlt::Datetime(2008,  3,  9, 7),
            bdlt::Datetime(2008, 11,  2, 6),
            bdlt::Datetime(2009,  3,  8, 7),
            bdlt::Datetime(2009, 11,  1, 6),
            bdlt::Datetime(2010,  3, 14, 7),
            bdlt::Datetime(2010, 11,  7, 6),
            bdlt::Datetime(2011,  3, 13, 7),
            bdlt::Datetime(2011, 11,  6, 6),
        };
        const int NUM_TRANSITION_TIMES =
                            sizeof TRANSITION_TIMES / sizeof *TRANSITION_TIMES;

        ASSERT(0 == NUM_TRANSITION_TIMES % 2);

        for (int i = 0; i < NUM_TRANSITION_TIMES; i += 2) {
            result->addTransition(
                 bdlt::EpochUtil::convertToTimeT64(TRANSITION_TIMES[i]),
                 edt);
            result->addTransition(
             bdlt::EpochUtil::convertToTimeT64(TRANSITION_TIMES[i + 1]),
             est);
        }
        return 0;
    }
//..
// Finally, we define a function 'f' that instantiates an object of type
// 'MyLoaderImp':
//..
    void f()
    {
        MyLoaderImp a;
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Example 2: Using a 'baltzo::Loader'
/// - - - - - - - - - - - - - - - - -
// In this example we use a 'MyLoaderImp' to load the data for one time zone,
// and print the time transitions, contained in the obtained time zone data, to
// standard output.  Note that, the implementation of this example is for
// illustrative purpose only, and in general, clients should use an
// implementation that loads data from an external data source (e.g.,
// 'baltzo_datafileloader').
//
// First, we create a loader 'myLoader' using the class 'MyLoaderImp' defined
// in the previous example, and obtain a 'bdlma::AlignedAllocator' pointer to
// it:
//..
    MyLoaderImp myLoader;
    baltzo::Loader& loader = myLoader;
//..
// Now, we load the time zone data for New York:
//..
    baltzo::Zoneinfo nyTimeZone;
    if (0 != loader.loadTimeZone(&nyTimeZone, "America/New_York")) {
       bsl::cout << "Failed to find time zone data." << bsl::endl;
       return -1;                                                     // RETURN
    }
//..
// Then, we verify some basic properties of the time zone:
//..
    ASSERT("America/New_York" == nyTimeZone.identifier());
//..
// Finally, we write to standard output the information about the
// daylight-saving time transitions of the loaded time zone, in New York local
// time:
//..
    baltzo::Zoneinfo::TransitionConstIterator tIt =
                                                 nyTimeZone.beginTransitions();
    for (; tIt != nyTimeZone.endTransitions(); ++tIt) {
       bdlt::Datetime transition =
                           bdlt::EpochUtil::convertFromTimeT64(tIt->utcTime());
       const baltzo::LocalTimeDescriptor& descritor = tIt->descriptor();
       if (verbose) {
           bsl::cout << "transition to "
                     << descritor.description()
                     << " at "
                     << transition
                     << " UTC"
                     << bsl::endl;
       }
    }
//..
// The resulting output will look like:
//..
//  transition to EDT at 11MAR2007_07:00:00.000 UTC
//  transition to EST at 04NOV2007_06:00:00.000 UTC
//  transition to EDT at 09MAR2008_07:00:00.000 UTC
//  transition to EST at 02NOV2008_06:00:00.000 UTC
//  transition to EDT at 08MAR2009_07:00:00.000 UTC
//  transition to EST at 01NOV2009_06:00:00.000 UTC
//  transition to EDT at 14MAR2010_07:00:00.000 UTC
//  transition to EST at 07NOV2010_06:00:00.000 UTC
//  transition to EDT at 13MAR2011_07:00:00.000 UTC
//  transition to EST at 06NOV2011_06:00:00.000 UTC
//..

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
        //   virtual ~baltzo::Loader();
        //   virtual int loadTimeZone(Zoneinfo *tz, const char *tzId) = 0;
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

        BSLS_PROTOCOLTEST_ASSERT(testObj, loadTimeZone(0, 0));

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
