// bdldfp_decimalformatconfig.t.cpp                                   -*-C++-*-
#include <bdldfp_decimalformatconfig.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::atoi;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  This class does not have Primary
// Manipulators and provides Basic Accessors to the attributes initialized in
// this class constructor.
// ----------------------------------------------------------------------------
// CREATORS
// [2] bdldfp::DecimalFormatConfig(int, Style, Sign, str, str, str, char, char);
//
// ACCESSORS
// [3] int precision() const;
// [3] Style style() const;
// [3] Sign sign() const;
// [3] const char *infinity() const;
// [3] const char *nan() const;
// [3] const char *sNan() const;
// [3] char decimalPoint() const;
// [3] char exponent() const;
// ----------------------------------------------------------------------------
// [1] BREATHING TEST
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
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

static int                test;
static int             verbose;
static int         veryVerbose;
static int     veryVeryVerbose;
static int veryVeryVeryVerbose;
static bslma::TestAllocator *pa;

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace BDEC = BloombergLP::bdldfp;
typedef BDEC::DecimalFormatConfig Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
                   test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;  // always the last

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator  ta(veryVeryVeryVerbose);
    pa = &ta;

    cout.precision(35);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        //: 1 An object created with the constructor taking the precision
        //:   parameter only  has the rest of parameters initialized to
        //    contractually specified default values.
        //:
        //: 2 That each accessor returns a value bound to an attribute in the
        //:   constructor.
        //
        // Plan:
        //: 1 Create DecimalFormatConfig object with the constructor taking
        //:   the precision parameter only.  Use default values for optional
        //:   parameters.
        //:
        //: 2 Ensure that each accessor to an attribute returns the default
        //:   value.
        //:
        //: 3 Create DecimalFormatConfig object with the constructor taking
        //:   all configuration parameters.
        //:
        //: 4 Ensure that each accessor to an attribute returns a value bound
        //:   to the attribute in the constructor.

        if (verbose) cout <<"\nTESTING ACCESSORS"
                            "\n=================\n";

        const int PRECISION = 42;
        Obj mD(PRECISION); const Obj& D = mD;

        {
            if (veryVerbose)
                cout << "\tPrecesion accessor" << endl;

            ASSERT(PRECISION == D.precision());
        }
        {
            if (veryVerbose)
                cout << "\tStyle accessor" << endl;

            ASSERT(Obj::e_SCIENTIFIC == D.style());

            Obj mX(PRECISION, Obj::e_FIXED); const Obj& X = mX;

            ASSERT(Obj::e_FIXED == X.style());
            ASSERT(D.style() != X.style());
        }
        {
            if (veryVerbose)
                cout << "\tSign accessor" << endl;

            ASSERT(Obj::e_NEGATIVE_ONLY == D.sign());

            Obj mX(PRECISION, Obj::e_FIXED, Obj::e_ALWAYS); const Obj& X = mX;

            ASSERT(Obj::e_ALWAYS == X.sign());
            ASSERT(D.sign() != X.sign());
        }
        {
            if (veryVerbose)
                cout << "\tSign accessor" << endl;

            ASSERT(Obj::e_NEGATIVE_ONLY == D.sign());

            Obj mX(PRECISION, Obj::e_FIXED, Obj::e_ALWAYS); const Obj& X = mX;

            ASSERT(Obj::e_ALWAYS == X.sign());
            ASSERT(D.sign() != X.sign());
        }
        {
            if (veryVerbose)
                cout << "\tSign accessor" << endl;

            ASSERT(Obj::e_NEGATIVE_ONLY == D.sign());

            Obj mX(PRECISION, Obj::e_FIXED, Obj::e_ALWAYS); const Obj& X = mX;

            ASSERT(Obj::e_ALWAYS == X.sign());
            ASSERT(D.sign() != X.sign());
        }
        {
            if (veryVerbose)
                cout << "\tInfinity accessor" << endl;

            ASSERT(0 == strcmp("inf", D.infinity()));

            const char *INF = "Inf";
                  Obj   mX(PRECISION, Obj::e_FIXED, Obj::e_ALWAYS, INF);
            const Obj&   X = mX;

            ASSERT(0 == strcmp(INF,          X.infinity()));
            ASSERT(0 != strcmp(D.infinity(), X.infinity()));
        }
        {
            if (veryVerbose)
                cout << "\tNan accessor" << endl;

            ASSERT(0 == strcmp("nan", D.nan()));

            const char *QNAN = "NaN";
            Obj         mX(PRECISION, Obj::e_FIXED, Obj::e_ALWAYS, "", QNAN);
            const Obj&   X = mX;

            ASSERT(0 == strcmp(QNAN,    X.nan()));
            ASSERT(0 != strcmp(D.nan(), X.nan()));
        }
        {
            if (veryVerbose)
                cout << "\tsNan accessor" << endl;

            ASSERT(0 == strcmp("snan", D.sNan()));

            const char *SNAN = "sNaN";
            Obj         mX(PRECISION,
                           Obj::e_FIXED,
                           Obj::e_ALWAYS,
                           "",
                           "",
                           SNAN);
            const Obj&   X = mX;

            ASSERT(0 == strcmp(SNAN,     X.sNan()));
            ASSERT(0 != strcmp(D.sNan(), X.sNan()));
        }
        {
            if (veryVerbose)
                cout << "\tPoint accessor" << endl;

            ASSERT('.' == D.decimalPoint());

            const char POINT = ',';
            Obj        mX(PRECISION,
                          Obj::e_FIXED,
                          Obj::e_ALWAYS,
                          "",
                          "",
                          "",
                          POINT);
            const Obj&  X = mX;

            ASSERT(POINT            == X.decimalPoint());
            ASSERT(D.decimalPoint() != X.decimalPoint());
        }
        {
            if (veryVerbose)
                cout << "\tExponent accessor" << endl;

            ASSERT('E' == D.exponent());

            const char EXPONENT = 'e';
            Obj        mX(PRECISION,
                          Obj::e_FIXED,
                          Obj::e_ALWAYS,
                          "",
                          "",
                          "",
                          '.',
                          EXPONENT);
            const Obj&  X = mX;

            ASSERT(EXPONENT     == X.exponent());
            ASSERT(D.exponent() != X.exponent());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //: 1 An object is creatable with the constructor taking configuration
        //:   attributes.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create DecimalFormatConfig object with the constructor taking
        //:   configuration attributes.

        if (verbose) cout <<"\nTESTING CREATOR"
                            "\n===============\n";

        if (veryVerbose)
            cout << "\tDecimalFormatConfig constructor with configuration "
                 << "attributes\n";

        const int PRECISION = 42;
        Obj mX(PRECISION); const Obj& X = mX;

        ASSERT(PRECISION == X.precision());

        if (veryVerbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                         bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'Negative precision'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 0));
                ASSERT_SAFE_FAIL(Obj(-1));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object 'X'.
        //: 2 Get access to each attribute.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const int         PRECISION = 81;
        const Obj::Style  STYLE     = Obj::e_NATURAL;
        const Obj::Sign   SIGN      = Obj::e_ALWAYS;
        const char       *INF       = "Inf";
        const char       *QNAN      = "qNaN";
        const char       *SNAN      = "sNaN";
        const char        POINT     = ',';
        const char        EXPONENT  = 'e';

              Obj mX(PRECISION, STYLE, SIGN, INF, QNAN, SNAN, POINT, EXPONENT);
        const Obj& X = mX;

        ASSERT(PRECISION == X.precision());
        ASSERT(STYLE     == X.style());
        ASSERT(SIGN      == X.sign());
        ASSERT(INF       == X.infinity());
        ASSERT(QNAN      == X.nan());
        ASSERT(SNAN      == X.sNan());
        ASSERT(POINT     == X.decimalPoint());
        ASSERT(EXPONENT  == X.exponent());

      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: No memory came from the global or default allocator.

    LOOP2_ASSERT(test, globalAllocator.numBlocksTotal(),
                 0 == globalAllocator.numBlocksTotal());
    LOOP2_ASSERT(test, defaultAllocator.numBlocksTotal(),
                 0 == defaultAllocator.numBlocksTotal());

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
