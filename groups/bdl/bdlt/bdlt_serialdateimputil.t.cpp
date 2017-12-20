// bdlt_serialdateimputil.t.cpp                                       -*-C++-*-
#include <bdlt_serialdateimputil.h>

#include <bslim_testutil.h>

#include <bslmf_issame.h>    // 'bsl::is_same'

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a single utility class,
// 'bdlt::SerialDateImpUtil', by 'typedef'-ing that class to one of two other
// classes that implement the same (documentation-only) protocol.  This test
// driver uses a template meta-function, 'bsl::is_same', to confirm that the
// mapping is correct for the current build mode.  Each of those implementation
// classes is exhautively tested in their own test drivers (not here).
// ----------------------------------------------------------------------------
// [ 1] typedef ProlepticDateImpUtil SerialDateImpUtil;
// [ 1] typedef     PosixDateImpUtil SerialDateImpUtil;
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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlt::SerialDateImpUtil Util;

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int     test = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'typedef'
        //
        // Concerns:
        //: 1 The 'typedef' maps 'SerialDateImpUtil' to the implementation
        //:   class required by the build mode.
        //
        // Plan:
        //: 1 Explicitly test using the 'bsl::is_same' template that 'Util'
        //:   is the same as the class expected for the current build mode.
        //
        // Testing:
        //   typedef SerialDateTimeUtil
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'typedef'" << endl
                          << "=================" << endl;

#ifdef BDE_USE_PROLEPTIC_DATES
        ASSERT((bsl::is_same<Util, bdlt::ProlepticDateImpUtil>::value));
#else
        ASSERT((bsl::is_same<Util, bdlt::    PosixDateImpUtil>::value));
#endif

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
// Copyright 2014 Bloomberg Finance L.P.
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
