// bdldfp_decimal.fwd.t.cpp                                           -*-C++-*-
#include <bdldfp_decimal.fwd.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::atoi;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test forward declares IEEE-754 decimal floating-point
// types.
// ----------------------------------------------------------------------------
// TYPES
//: o 'Decimal32'
//: o 'Decimal64'
//: o 'Decimal128'
//
// ----------------------------------------------------------------------------
// [ 1] FORWARD DECLARATIONS
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

#define ASSERT BSLIM_TESTUTIL_ASSERT
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q BSLIM_TESTUTIL_Q    // Quote identifier literally.
#define P BSLIM_TESTUTIL_P    // Print identifier and value.
#define P_ BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//               STANDARD FORWARDING HEADER TESTING UTILITIES
// ----------------------------------------------------------------------------

namespace u {
                    // ================================
                    // 'isIncomplete' function template
                    // ================================

template <class t_TYPE> bool isIncomplete(int(*)[sizeof(t_TYPE)]);
template <class t_TYPE> bool isIncomplete(...);
    // Return 'true' when invoked with literal '0' argument if 'T&' is
    // incomplete, 'false' if 'T' is complete.

                    // --------------------------------
                    // 'isIncomplete' function template
                    // --------------------------------

template <class t_TYPE>
bool isIncomplete(int(*)[sizeof(TYPE)])
{
    return false;
}

template <class t_TYPE>
bool isIncomplete(...)
{
    return true;
}

void testTypeCompleteness();
    // Verify that the names declared in this forwarding header are complete
    // when the primary header is included.  Note that the definition of this
    // function is at the end of this file after 'main' and the inclusion of
    // the primary header.

}  // close namespace u

//-----------------------------------------------------------------------------
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;  (void)veryVerbose;
    bool veryVeryVerbose = argc > 4;  (void)veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // FORWARD DECLARATIONS
        //
        // Concerns:
        //: 1 Each type that should be available has been declared by having
        //: included the forwarding header for this component.
        //:
        //: 2 The types are incomplete.
        //:
        //: 3 The types are completed by including the primary header.
        //
        // Plan:
        //: 1 Verify that each expected name is declared and is incomplete
        //: using 'u::isIncomplete'.
        //:
        //: 2 Invoke 'testTypeCompleteness', defined below after the inclusion
        //: of the primary header, to verify that the forward names are
        //: completed by including the primary header.
        //
        // Testing:
        //   FORWARD DECLARATIONS
        // --------------------------------------------------------------------
        if (verbose) puts("\nFORWARD DECLARATIONS"
                          "\n====================");

        ASSERT(u::isIncomplete<bdldfp::Decimal32>(0));
        ASSERT(u::isIncomplete<bdldfp::Decimal64>(0));
        ASSERT(u::isIncomplete<bdldfp::Decimal128>(0));

        testTypeCompleteness();
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status - " << testStatus << "." << endl;
    }

    return testStatus;
}

// Verify that the primary header can be included *after* the forwarding
// header.

#include <bdldfp_decimal.h>

// Verify that the primary header defined the types only declared by '.fwd.h'.

namespace u {

void testTypeCompleteness()
{
    ASSERT(!u::isIncomplete<bdldfp::Decimal32>(0));
    ASSERT(!u::isIncomplete<bdldfp::Decimal64>(0));
    ASSERT(!u::isIncomplete<bdldfp::Decimal128>(0));

}

}  // close namespace u

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
