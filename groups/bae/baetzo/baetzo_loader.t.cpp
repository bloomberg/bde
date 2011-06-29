// baetzo_loader.t.cpp                                                -*-C++-*-
#include <baetzo_loader.h>

#include <baetzo_zoneinfo.h>

#include <bdet_datetime.h>

#include <bdetu_epoch.h>

#include <bsls_protocoltest.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a protocol class [...]
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o It is possible to create a concrete implementation the protocol.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] virtual ~baetzo_Loader();
//
// MANIPULATORS
// [ 1] virtual int loadTimeZone(Zoneinfo *tz, const char *tzId) = 0;
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
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
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------
typedef baetzo_Loader Obj;

//=============================================================================
//                      CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------
namespace {

struct ObjTestImp : bsls_ProtocolTestImp<Obj> {
    int loadTimeZone(baetzo_Zoneinfo *, const char *) { return markDone(); }
};

}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Implementing 'baetzo_Loader'
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates an implementation of 'baetzo_Loader' that can only
// return data for "America/New_York".
//
// Note that in general, an implementation of 'baetzo_Loader' should obtain
// time-zone information from an external data source (see
// 'baetzo_datafileloader').
//
// First, we define the interface of our implementation:
//..
    class MyLoaderImp : public baetzo_Loader {
        // This class provides a concrete implementation of the 'baetzo_Loader'
        // protocol (an abstract interface) for obtaining a time zone.  This
        // test implementation contains only partial data of the
        // "America/New_York" time zone, and is unable to obtain time-zone
        // information for any other time zones.

      public:
        // CREATORS
        MyLoaderImp();
            // Create a 'MyLoaderImp' object.

        ~MyLoaderImp();
            // Destroy this object.

        // MANIPULATORS
        virtual int loadTimeZone(baetzo_Zoneinfo *result,
                                 const char      *timeZoneId);
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
    int MyLoaderImp::loadTimeZone(baetzo_Zoneinfo *result,
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
        baetzo_LocalTimeDescriptor edt(-14400, true, "EDT");
        baetzo_LocalTimeDescriptor est(-18000, false, "EST");
//..
// Next, we create a series of transitions between these local time descriptors
// for the years 2007-2011.  Note that the United States transitions to
// daylight savings time on the second Sunday in March, at 2am local time (7am
// UTC), and transition back to standard time on the first Sunday in November
// at 2am local time (6am UTC), resulting in an even number of transitions.
// Also note that, the rules for generating these transitions were changed in
// 2007, and may be changed again at some point in the future.
//..
        static const bdet_Datetime TRANSITION_TIMES[] = {
            bdet_Datetime(2007,  3, 11, 7),
            bdet_Datetime(2007, 11,  4, 6),
            bdet_Datetime(2008,  3,  9, 7),
            bdet_Datetime(2008, 11,  2, 6),
            bdet_Datetime(2009,  3,  8, 7),
            bdet_Datetime(2009, 11,  1, 6),
            bdet_Datetime(2010,  3, 14, 7),
            bdet_Datetime(2010, 11,  7, 6),
            bdet_Datetime(2011,  3, 13, 7),
            bdet_Datetime(2011, 11,  6, 6),
        };
        const int NUM_TRANSITION_TIMES =
                            sizeof TRANSITION_TIMES / sizeof *TRANSITION_TIMES;

        ASSERT(0 == NUM_TRANSITION_TIMES % 2);

        for (int i = 0; i < NUM_TRANSITION_TIMES; i += 2) {
            result->addTransition(
                 bdetu_Epoch::convertToTimeT64(TRANSITION_TIMES[i]),
                 edt);
            result->addTransition(
             bdetu_Epoch::convertToTimeT64(TRANSITION_TIMES[i + 1]),
             est);
        }
        return 0;
    }
//..
// Finally, we define a function 'f' that instantiates an object of type
// 'MyLoaderImp':
//..
    void f() {
        MyLoaderImp a;
    }
//..

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

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

///Example 2: Using a 'baetzo_Loader'
/// - - - - - - - - - - - - - - - - -
// In this example we use a 'MyLoaderImp' to load the data for one time zone,
// and print the time transitions, contained in the obtained time zone data, to
// standard output.  Note that, the implementation of this example is for
// illustrative purpose only, and in general, clients should use an
// implementation that loads data from an external data source (e.g.,
// 'baetzo_datafileloader').
//
// First, we create a loader 'myLoader' using the class 'MyLoaderImp'
// defined in the previous example, and obtain a 'bdema_AlignedAllocator'
// pointer to it:
//..
    MyLoaderImp myLoader;
    baetzo_Loader& loader = myLoader;
//..
// Now, we load the time zone data for New York:
//..
    baetzo_Zoneinfo nyTimeZone;
    if (0 != loader.loadTimeZone(&nyTimeZone, "America/New_York")) {
       bsl::cout << "Failed to find time zone data." << bsl::endl;
       return -1;
    }
//..
// Then, we verify some basic properties of the time zone:
//..
    ASSERT("America/New_York" == nyTimeZone.identifier());
//..
// Finally, we write to standard output the information about the
// daylight-savings time transitions of the loaded time zone, in New York local
// time:
//..
    baetzo_Zoneinfo::TransitionConstIterator tIt =
                                                 nyTimeZone.beginTransitions();
    for (; tIt != nyTimeZone.endTransitions(); ++tIt) {
       bdet_Datetime transition =
                               bdetu_Epoch::convertFromTimeT64(tIt->utcTime());
       const baetzo_LocalTimeDescriptor& descritor = tIt->descriptor();
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
        //: 1 The protocol is an abstract class, and therefore no objects
        //:   of this class can be created.
        //:
        //: 2 The protocol class has no data members.
        //:
        //: 3 The protocol class has a pure virtual destructor.
        //:
        //: 4 All members of the protocol class are pure virtual.
        //:
        //: 5 All methods of the protocol class are publicly accessible.
        //
        // Plan:
        //: 1 Define a concrete derived implementation, 'ObjTestImp' of the
        //:   protocol.
        //:
        //: 2 Create an object of the 'bsls_ProtocolTest' class parameterized
        //:   with 'ObjTestImp'.
        //:
        //: 3 Use the 'bsls_ProtocolTest' object to verify that the protocol is
        //:   an abstract class.  (C-1)
        //:
        //: 4 Use the 'bsls_ProtocolTest' object to verify that the protocol
        //:   has no data members.  (C-2)
        //:
        //: 5 Use the 'bsls_ProtocolTest' object to verify that the protocol
        //:   has a virtual destructor.  (C-3)
        //:
        //: 6 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that:
        //:
        //:   1 All the methods of the protocol class are virtual.  (C-4)
        //:
        //:   2 All the methods of the protocol class are publicly accessible.
        //:     (C-5)
        //
        // Testing:
        //   virtual ~baetzo_Loader();
        //   virtual int loadTimeZone(Zoneinfo *tz, const char *tzId) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nCreate an object of the 'bsls_ProtocolTest'"
                             " class parameterized with 'ObjTestImp'"
                          << endl;

        bsls_ProtocolTest<ObjTestImp> t(veryVerbose);

        if (verbose) cout << "\nVerify that the protocol is an abstract class."
                          << endl;

        ASSERT(t.testAbstract());

        if (verbose) cout << "\nVerify that the protocol has no data members."
                          << endl;
        ASSERT(t.testNoDataMembers());

        if (verbose) cout << "\nVerify that the protocol has a virtual dtor."
                          << endl;
        ASSERT(t.testVirtualDestructor());


        if (verbose) cout << "\nVerify that each method is virtual and"
                             " publicly accessible."
                          << endl;

        BSLS_PROTOCOLTEST_ASSERT(t, loadTimeZone(0, 0));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
