// bdet_calendarloader.t.cpp                                          -*-C++-*-
#include <bdet_calendarloader.h>

#include <bsls_protocoltest.h>

#include <bsl_cstdio.h>       // printf()
#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strcmp()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a protocol class the purpose of which is to
// provide an interface for loading calendars.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o It is possible to create a concrete implementation the protocol.
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] virtual ~bdet_CalendarLoader();
//
// MANIPULATORS
// [ 1] virtual int load(PackedCalendar *result, const char *name) = 0;
// ----------------------------------------------------------------------------
// [  ] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

typedef bdet_CalendarLoader ProtocolClass;

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    int load(bdet_PackedCalendar *, const char *) { return markDone(); }
};

}

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
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
        //   virtual ~bdet_CalendarLoader();
        //   virtual int load(PackedCalendar *result, const char *name) = 0;
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

        BSLS_PROTOCOLTEST_ASSERT(testObj, load(0, 0));

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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
