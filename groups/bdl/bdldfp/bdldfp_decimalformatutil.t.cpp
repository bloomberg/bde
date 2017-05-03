// bdldfp_decimalformatutil.t.cpp                                     -*-C++-*-
#include <bdldfp_decimalformatutil.h>
#include <bdldfp_decimal.h>
#include <bdldfp_decimalutil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>      // 'atoi'
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// ----------------------------------------------------------------------------
// CLASS METHODS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
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

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

#define ASSERT_SAFE_FAIL_RAW(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(expr)
#define ASSERT_SAFE_PASS_RAW(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(expr)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef bdldfp::DecimalFormatUtil Util;

//=============================================================================
//                      GLOBAL CONSTANTS
//-----------------------------------------------------------------------------

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TEST FORMAT METHODS
        //
        // Concerns:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST FORMAT METHODS" << endl
                          << "===================" << endl;

        typedef bdldfp::Decimal32 Type;

#define DEC(X) BDLDFP_DECIMAL_DF(X)

        static const struct {
            const int          d_line;
            const Type         d_decimal;
            const int          d_precision;
            const char        *d_expected;
        } DATA [] = {
            //-----------------------------------------------------
            // Line | Decimal               | Precision |  Expected
            //-----------------------------------------------------
            {  L_,                  DEC(0.0),        0  ,  "0"    },
            {  L_,                  DEC(0.0),        1  ,  "0.0"  },
            {  L_,                  DEC(0.0),        2  ,  "0.00" },

            {  L_,                  DEC(1.0),        0  ,  "1"     },
            {  L_,                  DEC(1.0),        1  ,  "1.0"   },
            {  L_,                  DEC(1.0),        2  ,  "1.00"  },

            {  L_,                  DEC(0.1),        0  ,  "0"     },
            {  L_,                  DEC(0.1),        1  ,  "0.1"   },
            {  L_,                  DEC(0.1),        2  ,  "0.10"  },

            {  L_,                  DEC(0.01),       0  ,  "0"     },
            {  L_,                  DEC(0.01),       1  ,  "0.0"   },
            {  L_,                  DEC(0.01),       2  ,  "0.01"  },
            {  L_,                  DEC(0.01),       3  ,  "0.010" },

            {  L_,                  DEC(10.0),        0  ,  "10"     },
            {  L_,                  DEC(10.0),        1  ,  "10.0"  },
            {  L_,                  DEC(10.0),        2  ,  "10.00"  },

            {  L_,               DEC(-10.123),        0  ,  "-10"       },
            {  L_,               DEC(-10.123),        1  ,  "-10.1"     },
            {  L_,               DEC(-10.123),        2  ,  "-10.12"    },
            {  L_,               DEC(-10.123),        3  ,  "-10.123"   },
            {  L_,               DEC(-10.123),        4  ,  "-10.1230"  },

            {  L_,               DEC(-123e+3),        0  ,  "-123000"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int k_BUFFER_SIZE = 256;
        char      buffer[k_BUFFER_SIZE] = {0};

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int           LINE        = DATA[ti].d_line;
            const Type          DECIMAL32   = DATA[ti].d_decimal;
            const int           PRECISION   = DATA[ti].d_precision;
            const string        EXPECTED    = DATA[ti].d_expected;

            int len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   DECIMAL32,
                                   PRECISION,
                                   Util::e_FIXED);

            const string RESULT(buffer, len);
            if (veryVerbose) P_(len) P(RESULT);

            LOOP3_ASSERT(LINE, RESULT, EXPECTED, RESULT == EXPECTED);
            LOOP3_ASSERT(LINE, len, EXPECTED.length(), RESULT == EXPECTED);
        }
#undef DEC
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;




        const int          k_BUFFER_SIZE = 256;
        char               buffer[k_BUFFER_SIZE] = {0};
        {
            bdldfp::Decimal32  d32(BDLDFP_DECIMAL_DF(-51.234));
            int len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   d32,
                                   2,
                                   Util::e_FIXED);

            string s(buffer, len);
            P_(len) P(s);
        }

        {
            bsl::fill_n(buffer, k_BUFFER_SIZE, 0);

            bdldfp::Decimal32  d32(BDLDFP_DECIMAL_DF(-0.0052345));

            int len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   d32,
                                   9,
                                   Util::e_FIXED);

            string s(buffer, len);
            P_(len) P(s);
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2017
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
