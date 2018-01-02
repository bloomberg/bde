// bdldfp_intelimpwrapper.t.cpp                                       -*-C++-*-
#include <bdldfp_intelimpwrapper.h>

#include <bslim_testutil.h>

#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

// We include the 'fenv.h' header to ensure that there are no definition
// conflicts between the Intel DFP code and the system 'fenv' header.

#ifndef BSLS_PLATFORM_CMP_MSVC
#include <fenv.h>
#endif
#include <wchar.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::atoi;

#if !defined(BSLS_PLATFORM_OS_WINDOWS)
// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test wraps and includes the Intel DFP headers in a
// specific build mode.
//
// Global Concerns:
//: o Intel Headers are included correctly
//: o No #define detritus is left behind
//: o Intel Headers do not interact poorly with system headers
//
// TBD:
// ----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
//
// TRAITS
// ----------------------------------------------------------------------------
// [ 4] Macro clean environment
// [ 3] Simple function call tests
// [ 2] Intel Decimal types sanity test
// [ 1] Include Sanity Test
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void) veryVerbose;
    (void) veryVeryVerbose;

    _IDEC_flags flags;


    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // MACRO CLEAN ENVIRONMENT
        //
        // Concerns:
        //: 1 The various macros used in configuring, tuning, and compiling the
        //:   Intel DFP headers are disabled after inclusion.
        //
        // Plan:
        //: 1 Follow a pattern of '#ifdef <MACRO>',
        //:   'ASSERT(false && "<MACRO>   still declared");', and '#endif' in
        //:   confirming that each macro is disabled. (C-1)
        //
        // Testing:
        //   Macro clean environment
        // --------------------------------------------------------------------
        if (verbose) cout << std::endl
                          << "MACRO CLEAN ENVIRONMENT" << std::endl
                          << "=======================" << std::endl;

        #ifdef DECIMAL_CALL_BY_REFERENCE
        ASSERT(false && "DECIMAL_CALL_BY_REFERENCE still declared");
        #endif

        #ifdef DECIMAL_GLOBAL_ROUNDING
        ASSERT(false && "DECIMAL_GLOBAL_ROUNDING still declared");
        #endif

        #ifdef DECIMAL_GLOBAL_EXCEPTION_FLAGS
        ASSERT(false && "DECIMAL_GLOBAL_EXCEPTION_FLAGS still declared");
        #endif

        #ifdef BDLDFP_INTELIMPWRAPPER_FAKE_DEFINE_LINUX
        ASSERT(false &&
                    "BDLDFP_INTELIMPWRAPPER_FAKE_DEFINE_LINUX still declared");
        #endif

        // The Intel library was defining some constants that we do not want
        // polluting our global namespace.  We must test that those constants
        // are no longer '#define'ed.

        #ifdef P7
        ASSERT(false && "P7 still declared");
        #endif

        #ifdef P16
        ASSERT(false && "P16 still declared");
        #endif

        #ifdef P34
        ASSERT(false && "P34 still declared");
        #endif

        #ifndef BID_BIG_ENDIAN
        ASSERT(false && "BID_BIG_ENDIAN setting for Intel was not defined.");
        #else
        #  ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
        const bool big_endian = true;
        #  elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
        const bool big_endian = false;
        #  else
        ASSERT(false &&
               "One of big or little endian should have been available"
               " in bsls_platform");
        const bool big_endian = false;
        #  endif
        ASSERT(BID_BIG_ENDIAN == big_endian
                       && "BID_BIG_ENDIAN was not set to the correct setting");
        #endif

        #ifdef SWAP
        ASSERT(false && "SWAP still declared");
        #endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // SIMPLE FUNCTION CALL TESTS
        //   Ensure that we can call functions expected in the Intel DFP
        //   library, and that the ones we call behave reasonably.
        //
        // Concerns:
        //:  1 Intel function prototypes are declared, and are available for
        //:    use.
        //
        // Plan:
        //:  1 Declare variables of the various 'BID_UINTXX' types and call
        //:    some initialization for them. (C-1)
        //:
        //:  2 Compute a simple arithmetic sum for some 'BID_UINTXX' variables.
        //:    (C-1)
        //:
        //:  3 Verify the results of the simple arithmetic computation. (C-1)
        //
        // Testing:
        //   Simple function call tests
        // --------------------------------------------------------------------
        if (verbose) cout << std::endl
                          << "SIMPLE FUNCTION CALL TESTS" << std::endl
                          << "==========================" << std::endl;

        BID_UINT64  doubleDecimal = __bid64_from_int32( 42);
        BID_UINT128   quadDecimal = __bid128_from_int32(42);

        BID_UINT32  singleDecimal2 = __bid32_from_int32( 10, &flags);
        BID_UINT64  doubleDecimal2 = __bid64_from_int32( 10);
        BID_UINT128   quadDecimal2 = __bid128_from_int32(10);

        BID_UINT64  doubleDecimalResult = __bid64_from_int32( 52);
        BID_UINT128   quadDecimalResult = __bid128_from_int32(52);

        BID_UINT64  doubleDecimalComputed;
        BID_UINT128   quadDecimalComputed;

        (void) singleDecimal2;

        doubleDecimalComputed = __bid64_add(  doubleDecimal,
                                              doubleDecimal2,
                                             &flags);
          quadDecimalComputed = __bid128_add(
                                                quadDecimal,
                                                quadDecimal2,
                                             &flags);

        ASSERT(__bid64_quiet_equal( doubleDecimalResult,
                                    doubleDecimalComputed,
                                   &flags));
        ASSERT(__bid128_quiet_equal(  quadDecimalResult,
                                      quadDecimalComputed,
                                   &flags));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // INTEL DECIMAL TYPES SANITY TEST
        //   Ensure that we can call instantiate variables from the Intel
        //   libary.
        //
        // Concerns:
        //:  1 Intel Decimal types are declared, and are available for use.
        //
        // Plan:
        //:  1 Declare variables of the various 'BID_UINTXX' types and call
        //:    some initialization for them. (C-1)
        //:
        //:  2 Call 'memcmp' on differently initialized variables to verify
        //:    that their representations are distinct. (C-1)
        //
        // Testing:
        //   Intel Decimal types sanity test
        // --------------------------------------------------------------------
        if (verbose) cout << std::endl
                          << "INTEL DECIMAL TYPES SANITY TEST" << std::endl
                          << "===============================" << std::endl;

        // The intel library types work correctly if different values have
        // different bit patterns.

        BID_UINT32  singlePrecisionDecimalStorage = __bid32_from_int32( 42,
                                                                       &flags);
        BID_UINT64  doublePrecisionDecimalStorage = __bid64_from_int32( 42);
        BID_UINT128   quadPrecisionDecimalStorage = __bid128_from_int32(42);

        BID_UINT32  singlePrecisionDecimalStorage2 = __bid32_from_int32( 47,
                                                                       &flags);
        BID_UINT64  doublePrecisionDecimalStorage2 = __bid64_from_int32( 47);
        BID_UINT128   quadPrecisionDecimalStorage2 = __bid128_from_int32(47);

        ASSERT(bsl::memcmp(&singlePrecisionDecimalStorage,
                           &singlePrecisionDecimalStorage2,
                            sizeof(BID_UINT32)));

        ASSERT(bsl::memcmp(&doublePrecisionDecimalStorage,
                           &doublePrecisionDecimalStorage2,
                            sizeof(BID_UINT64)));

        ASSERT(bsl::memcmp(&  quadPrecisionDecimalStorage,
                           &  quadPrecisionDecimalStorage2,
                            sizeof(BID_UINT128)));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // INCLUDE SANITY TEST
        //   Find some evidence that the Intel headers are included.
        //   libary.
        //
        // Concerns:
        //:  1 Intel Decimal headers are included.
        //
        // Plan:
        //:  1 Declare a variable of 'BID_UINT64', which should be defined in
        //:    the Intel headers. (C-1)
        //
        // Testing:
        //   Include Sanity Test
        // --------------------------------------------------------------------
        if (verbose) cout << std::endl
                          << "INCLUDE SANITY TEST" << std::endl
                          << "===================" << std::endl;

        // The Intel library makes the BID_UINT64 type an integer.  This test
        // merely gives witness to the intel header inclusion by using one of
        // its types in a specific way.

        BID_UINT64 simpleDecimal;
        simpleDecimal = 42;
        (void) simpleDecimal;
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

#else // !defined(BSLS_PLATFORM_OS_WINDOWS)

int main()
{
    return -1;
}

#endif // !defined(BSLS_PLATFORM_OS_WINDOWS)

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
