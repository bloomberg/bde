// bdlt_formatutil.cpp                                                -*-C++-*-

#include <bdlt_formatutil.h>

#include <bdlt_formattestutil.h>

#include <bslim_testutil.h>

#include <bslfmt_padutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>

#include <bsl_climits.h>
#include <bsl_cstdio.h>      // `sprintf`
#include <bsl_cstdlib.h>     // `atoi`
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cin;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// [ 3] iter writeZeroPaddedDigits(iter, value, numDigits);
// [ 2] iter writeSecondFraction(iter, prec, dot, milli, micro);
// [ 1] BREATHING TEST

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::Types::Int64  Int64;
typedef bdlt::FormatUtil<char>    CUtil;
typedef bdlt::FormatUtil<wchar_t> WUtil;
typedef bdlt::FormatTestUtil      TUtil;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2; (void)             verbose;
    const bool         veryVerbose = argc > 3; (void)         veryVerbose;
    const bool     veryVeryVerbose = argc > 4; (void)     veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5; (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the default or
    //  global allocator.

    bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `writeZeroPaddedDigits`
        //
        // Concerns:
        // 1. `writeZeroPaddedDigits` writes the number correctly, padding
        //    properly with the right number of digits.
        //
        // 2. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Loop, incrementing the variable `value` starting from 0 and
        //    roughly geometrically after that until it exceeds 10 million.
        //
        // 2. Nest another loop, incrementing the variable `numDigits` over the
        //    range `[ 2 .. 6 ]`.
        //
        // 3. Use `to_string` to write `value` to a string `expC`.
        //
        // 4. If the length of the written value exceeds `numDigits`, use
        //    the `BSLS_ASSERTTEST_*` macros to confirm that this undefined
        //    behavior is detected.
        //
        // 5. Prepad `expC` with '0's to make it `numDigits` long.
        //
        // 6. Call the function under test and verify the output matches
        //    `expC`.  Repeat the process for `wchar_t`.
        //
        // 7. Iterate through several invalid values for `numDigits`, use the
        //    `BSLS_ASSERTTEST_*` macros to verify that this undefined behavior
        //    is detected.
        //
        // Testing:
        //   iter writeZeroPaddedDigits(iter, value, numDigits);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `writeZeroPaddedDigits`\n"
                             "===============================\n";

        wchar_t wBuf[20], *wBuf_p;
        char    cBuf[20], *cBuf_p;

        for (unsigned value = 0; value < 10 * 1000 * 1000;
                                                     value = (value | 1) * 7) {
            for (unsigned numDigits = 2; numDigits <= 6; ++numDigits) {
                bsl::string expC;    const bsl::string& EXPC = expC;
                expC = bsl::to_string(value);

                if (numDigits < EXPC.length()) {
                    bsls::AssertTestHandlerGuard guard;

                    cBuf_p = cBuf;
                    ASSERT_FAIL(CUtil::writeZeroPaddedDigits(cBuf_p,
                                                             value,
                                                             numDigits));

                    wBuf_p = wBuf;
                    ASSERT_FAIL(WUtil::writeZeroPaddedDigits(wBuf_p,
                                                             value,
                                                             numDigits));
                    continue;
                }
                ASSERT(value < 1000 * 1000);

                expC.insert(bsl::size_t(0), numDigits - EXPC.length(), '0');

                cBuf_p = cBuf;
                cBuf_p = CUtil::writeZeroPaddedDigits(cBuf_p,
                                                      value,
                                                      numDigits);
                *cBuf_p = 0;
                ASSERTV(EXPC, cBuf, EXPC == cBuf);

                bsl::wstring expW;    const bsl::wstring& EXPW = expW;
                TUtil::widen(&expW, EXPC);

                wBuf_p = wBuf;
                wBuf_p = WUtil::writeZeroPaddedDigits(wBuf_p,
                                                      value,
                                                      numDigits);
                *wBuf_p = 0;
                ASSERT(EXPW == wBuf);
            }

            // negative test illegal values for `numDigits`

            unsigned nds[] = { 0, 1, 7, 10, 100, UINT_MAX };
            enum { k_NUM_NDS = sizeof nds / sizeof *nds };

            bsls::AssertTestHandlerGuard guard;

            for (int tnds = 0; tnds < k_NUM_NDS; ++tnds) {
                unsigned numDigits = nds[tnds];    (void) numDigits;

                cBuf_p = cBuf;
                ASSERT_SAFE_FAIL(CUtil::writeZeroPaddedDigits(cBuf_p,
                                                              value,
                                                              numDigits));

                wBuf_p = wBuf;
                ASSERT_SAFE_FAIL(WUtil::writeZeroPaddedDigits(wBuf_p,
                                                              value,
                                                              numDigits));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `writeSecondFraction`
        //
        // Concerns:
        // 1. Given a precision of 0, does no output and no decimal point.
        //
        // 2. For any precision above 0, output length is `precision + 1`.
        //
        // 3. The output is an accurate representation of the fraction of a
        //     second specified by `milliseconds` and `microseconds`.
        //
        // 4. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Using the table-driven technique:
        //
        //    1. Specify a `char` array `DOTS` containing possible values to be
        //       used for the decimal.
        //
        //    2. Specify a table `DATA` with line numbers and 3 other columns:
        //       milliseconds, microseconds, and `expDigits`, where `expDigits`
        //       is a 7 digit character string of the expected output when
        //       precision is 6, not including the decimal.
        //
        // 2. For each element of the `DOTS` array described in P-1 create a
        //    `char` variable `DOT`.
        //
        //    1. Execute an inner loop through the elements of `DATA`,
        //       assigning values to the line number, milliseconds,
        //       microseconds, and 'expDigits' as described in P-1-2.
        //
        //    2. Create a `string` `CEXP` of `DOT` followed by the 6 digits of
        //       `milli` and `micro`, followed by 44 '0's, and widen that to a
        //       `wstring` `WEXP`.
        //
        //    3. Nested within the `DATA` loop, iterate `prec` over
        //       `[ 0 .. 50 ]`, call `writeSecondFraction`, and observe that:
        //
        //       1. If `prec == 0`, no output was done.
        //
        //       2. If `0 < prec`, the result matches
        //          `CEXP.substr(0, prec + 1)`.
        //
        //       3. Call `writeSecondFraction` again and repeat 2-3-1 & 2-3-2
        //          using wide strings.
        //
        //    4. After closing the loop through `DATA` and while still within
        //       the loop through `DOTS`, create two `int` arrays, one of
        //       illegal values of `precision` and one of values that are
        //       illegal for both milliseconds and microseconds.
        //
        //    5. Iterate through all the invalid values of precision, and
        //       verify with the `BSLS_ASSERTTEST_*` macros the passing such
        //       values is caught by asserts when enabled.
        //
        //    5. Iterate through all the invalid values of milliseconds and,
        //       microseconds, and then verify with the `BSLS_ASSERTTEST_*`
        //       macros tha passing such values is caught by asserts when
        //       enabled.
        //
        // Testing:
        //   iter writeSecondFraction(iter, prec, dot, milli, micro);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `writeSecondFraction`\n"
                             "=============================\n";

        static const char DOTS[] = { '.', ',', 'X', '_', ':' };
        enum { k_NUM_DOTS = sizeof DOTS / sizeof *DOTS };

        static const struct Data {
            int         d_line;
            int         d_millisecond;
            int         d_microsecond;
            const char *d_expDigits;
        } DATA[] = {
           // LINE  milli  micro   exp string
           // ----  -----  -----   ---------

            {  L_,     0,     0,   "000000" },
            {  L_,   123,   456,   "123456" },
            {  L_,   987,   654,   "987654" },
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        char    cBuf[128], *cOut;
        wchar_t wBuf[128], *wOut;

        for (int td = 0; td < k_NUM_DOTS; ++td) {
            const char DOT = DOTS[td];

            for (int ti = 0; ti < k_NUM_DATA; ++ti) {
                const Data&  data      = DATA[ti];
                const int    LINE      = data.d_line;
                const int    milli     = data.d_millisecond;
                const int    micro     = data.d_microsecond;
                const char  *expDigits = data.d_expDigits;

                bsl::string cExp(1, DOT);    const bsl::string& CEXP = cExp;
                cExp += expDigits;
                cExp.append(44, '0');

                // convert `CEXP` to `WEXP`, a wide string

                bsl::wstring wExp;    const bsl::wstring& WEXP = wExp;
                TUtil::widen(&wExp, CEXP);

                // test `precision <= 0` (special case, no output)

                for (int prec = 0; prec <= 50; ++prec) {
                    cOut = cBuf;
                    bsl::memset(cBuf, 0xa5, sizeof(cBuf));
                    cOut = CUtil::writeSecondFraction(
                                                cOut, prec, DOT, milli, micro);
                    ASSERT(cBuf + (prec ? prec + 1 : 0) == cOut);

                    if (prec) {
                        ASSERT(DOT == *cBuf);
                        const bsl::string_view outputC(cBuf, cOut - cBuf);
                        ASSERTV(LINE, prec, outputC, CEXP.substr(0, prec + 1),
                                          outputC == CEXP.substr(0, prec + 1));
                    }

                    wOut = wBuf;
                    bsl::memset(wBuf, 0xa5, sizeof(wBuf));
                    wOut = WUtil::writeSecondFraction(
                                                wOut, prec, DOT, milli, micro);
                    ASSERT(wBuf + (prec ? prec + 1 : 0) == wOut);
                    if (prec) {
                        ASSERT(static_cast<wchar_t>(DOT) == *wBuf);
                        const bsl::wstring_view outputW(wBuf, wOut - wBuf);
                        ASSERTV(LINE, prec, TUtil::narrow(outputW),
                                       TUtil::narrow(WEXP.substr(0, prec + 1)),
                                          outputW == WEXP.substr(0, prec + 1));
                    }
                }
            }

            // negative testing

            bsls::AssertTestHandlerGuard guard;

            static const int badPrecs[] = { INT_MIN, -1000 * 1000, -1000,
                                                                     -10, -1 };
            enum { k_NUM_BAD_PRECS = sizeof badPrecs / sizeof *badPrecs };

            static const int badVals[] = {
                        INT_MIN, -1000, -1, 1000, 1000 * 1000, 1001, INT_MAX };
            enum { k_NUM_BAD_VALS = sizeof badVals / sizeof *badVals };

            cOut = cBuf;
            ASSERT_PASS(cOut = CUtil::writeSecondFraction(
                                             cOut, 3,    DOT, 0,       0));

            cOut = cBuf;
            ASSERT_PASS(cOut = CUtil::writeSecondFraction(
                                             cOut, 0,    DOT, 0,       0));
            cOut = cBuf;
            ASSERT_PASS(cOut = CUtil::writeSecondFraction(
                                             cOut, 6,    DOT, 0,       0));

            for (int tbp = 0; tbp < k_NUM_BAD_PRECS; ++tbp) {
                const int badPrec = badPrecs[tbp];    (void) badPrec;

                cOut = cBuf;
                ASSERT_FAIL(cOut = CUtil::writeSecondFraction(
                                             cOut, badPrec, DOT, 0,    0));
            }

            for (int tbv = 0; tbv < k_NUM_BAD_VALS; ++tbv) {
                const int badVal = badVals[tbv];    (void) badVal;

                for (int prec = 0; prec < 10; ++prec) {
                    cOut = cBuf;
                    ASSERT_PASS(cOut = CUtil::writeSecondFraction(
                                             cOut, prec, DOT, 0,      0));
                    cOut = cBuf;
                    ASSERT_FAIL(cOut = CUtil::writeSecondFraction(
                                             cOut, prec, DOT, badVal, badVal));
                    ASSERT_FAIL(cOut = CUtil::writeSecondFraction(
                                             cOut, prec, DOT, 0,      badVal));
                    ASSERT_FAIL(cOut = CUtil::writeSecondFraction(
                                             cOut, prec, DOT, badVal, 0));

                    wOut = wBuf;
                    ASSERT_PASS(wOut = WUtil::writeSecondFraction(
                                             wOut, prec, DOT, 0,      0));
                    wOut = wBuf;
                    ASSERT_FAIL(wOut = WUtil::writeSecondFraction(
                                             wOut, prec, DOT, badVal, badVal));
                    ASSERT_FAIL(wOut = WUtil::writeSecondFraction(
                                             wOut, prec, DOT, 0,      badVal));
                    ASSERT_FAIL(wOut = WUtil::writeSecondFraction(
                                             wOut, prec, DOT, badVal, 0));
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------
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
// Copyright 2026 Bloomberg Finance L.P.
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
