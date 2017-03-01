// btlso_polltest.t.cpp                                               -*-C++-*-
#include <btlso_polltest.h>

#include <btlso_defaulteventmanagerimpl.h>
#include <btlso_inetstreamsocket.h>
#include <btlso_ipv4address.h>

#include <bsl_ostream.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <stdio.h>     // 'sprintf', 'snprintf' [NOT '<cstdio>', which does not
                       // include 'snprintf']
#include <stdlib.h>    // 'atoi'

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component verifies that when btlso_defaulteventmanagerimpl.h (which
// includes <sys/poll.h>) is included ahead of btlso_inetstreamsocket.h (which
// also includes <sys/poll.h>), the template code in the latter file, which
// references names defined as macros by <sys/poll.h> on AIX continues to
// reference the correct names, despite the former file undefining those names.
//
//-----------------------------------------------------------------------------
// [ 1] CONCERN: Correct use of events/revents macros.

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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                  << J << "\t" << #K << ": " << K << "\n"; \
                  aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_() cout << '\t' << flush            // Print tab without '\n'
#define L_ __LINE__                           // Current line number

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1:
      // --------------------------------------------------------------------
      // TESTING 'events'/'revents'
      //
      // Concerns:
      //: 1 Methods using 'events' and 'revents' compile.
      //
      // Plan:
      //: 1 Take the address of the (template) methods in question.
      //  
      // Testing:
      //   CONCERN: Correct use of events/revents macros.
      // --------------------------------------------------------------------
      {
        if (verbose) cout << "\nTESTING 'events'/'revents'"
                             "\n==========================\n";

        typedef btlso::InetStreamSocket<btlso::IPv4Address> Obj;
        int (Obj::*f1)(const bsls::TimeInterval&) = &Obj::waitForConnect;
        ASSERT(f1);
        int (Obj::*f2)(btlso::Flag::IOWaitType, const bsls::TimeInterval&) =
            &Obj::waitForIO;
        ASSERT(f2);
        int (Obj::*f3)(btlso::Flag::IOWaitType) = &Obj::waitForIO;
        ASSERT(f3);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = 255;
      }
    }

    if (testStatus > 0 && testStatus < 255) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2017
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
