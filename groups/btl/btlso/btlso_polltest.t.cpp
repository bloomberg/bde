// btlso_polltest.t.cpp                                               -*-C++-*-
#include <btlso_polltest.h>

#include <btlso_defaulteventmanagerimpl.h>
#include <btlso_inetstreamsocket.h>
#include <btlso_ipv4address.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component verifies that when btlso_defaulteventmanagerimpl.h (which
// includes <sys/poll.h>) is included ahead of btlso_inetstreamsocket.h (which
// also includes <sys/poll.h>), the template code in the latter file, which
// references names defined as macros by <sys/poll.h> on AIX continues to
// reference the correct names, despite the former file undefining those names.
//
// ----------------------------------------------------------------------------
// [ 1] CONCERN: Correct use of events/revents macros.
// ----------------------------------------------------------------------------

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
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
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
        if (verbose) cout << "\nTESTING 'events'/'revents'"
                             "\n==========================\n";

        typedef btlso::InetStreamSocket<btlso::IPv4Address> Obj;
        int (Obj::*f1)(const bsls::TimeInterval&) = &Obj::waitForConnect;
        ASSERT(f1);
        int (Obj::*f2)(btlso::Flags::IOWaitType, const bsls::TimeInterval&) =
            &Obj::waitForIO;
        ASSERT(f2);
        int (Obj::*f3)(btlso::Flags::IOWaitType) = &Obj::waitForIO;
        ASSERT(f3);
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
// Copyright 2017 Bloomberg Finance L.P.
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
