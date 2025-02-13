// bdlt_fuzzutil.t.cpp                                                -*-C++-*-
#include <bdlt_fuzzutil.h>

#include <bslim_fuzzdataview.h>
#include <bslim_fuzzutil.h>
#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// `bdlt::FuzzUtil` provides a suite of functions for producing dates from fuzz
// data.  This test driver tests each implemented utility function
// independently.
//
//-----------------------------------------------------------------------------
// CLASS METHODS
// [12] DatetimeTz consumeDatetimeTzInRange(bslim::FuzzDataView *, b, e);
// [11] DatetimeTz consumeDatetimeTz(bslim::FuzzDataView *);
// [10] Datetime consumeDatetimeInRange(bslim::FuzzDataView *, b, e);
// [ 9] Datetime consumeDatetime(bslim::FuzzDataView *);
// [ 8] TimeTz consumeTimeTz(bslim::FuzzDataView *);
// [ 7] Time consumeTime(bslim::FuzzDataView *);
// [ 6] DateTz consumeDateTzInRange(bslim::FuzzDataView *, begin, end);
// [ 5] DateTz consumeDateTz(bslim::FuzzDataView *);
// [ 4] int consumeTz(bslim::FuzzDataView *);
// [ 3] Date consumeDate(FuzzDataView *);
// [ 2] Date consumeDateInRange(FuzzDataView *, begin, end);
// ----------------------------------------------------------------------------
// [13] USAGE EXAMPLE
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

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef bdlt::FuzzUtil Util;

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test)  { case 0:  // Zero is always the leading case.
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Provide `bdlt::Datetime` within a Range
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// The provided fuzz data is here represented by an array of bytes:
// ```
   const uint8_t data[] = {0x8A, 0x19, 0x0D, 0x44, 0x37, 0x0D,
                           0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA};
// ```
// First, we default construct a `bslim::FuzzDataView` object, `fdv`:
// ```
   bslim::FuzzDataView fdv(data, sizeof(data));
//
   ASSERT(12 == fdv.length());
// ```
// Next, we construct `Date` objects to represent the `begin` and `end` of the
// time interval in which we wish to construct our new `Date` from the fuzz
// data:
// ```
   bdlt::Date begin(1833, 5, 7);
   bdlt::Date end(1897, 4, 3);
// ```
// Finally, we crate a `Date` object, `within`, by employing `bdlt_fuzzutil`:
// ```
   bdlt::Date within = Util::consumeDateInRange(&fdv, begin, end);
//
   ASSERT(begin  <= within);
   ASSERT(within <= end);
// ```
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING `consumeDatetimeTzInRange`
        //
        // Concerns:
        // 1. The function returns valid `DatetimeTz` values within the
        //    specified range.
        //
        // Plan:
        // 1. Create a data array of size 80+20 bytes to simulate fuzz data.
        //
        // 2. Create a `FuzzDataView` from the fuzz data array.
        //
        // 3. Using the table-driven technique specify a set of valid date
        //    ranges in columns `BEGIN` and `END`.
        //
        // 4. For each row in the table described in P-3:  (C-1)
        //
        //   1. Create a `DatetimeTz` object using the `FuzzDataView`.
        //
        //   2. Verify that the function returns valid values within the
        //      specified range.  (C-1)
        //
        // Testing:
        //   DateTz consumeDatetimeTzInRange(bslim::FuzzDataView *, b, e);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `consumeDatetimeTzInRange`"
                          << "\n==============================" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0xbc, 0x27, 0x3a, 0x9d, 0x9c, 0xbb,
                                          0x03, 0x92, 0x3d, 0x32, 0x4e, 0x98,
                                          0x23, 0x50, 0x28, 0x37, 0xb7, 0xe1,
                                          0xe0, 0xef, 0xcf, 0xc5, 0x29, 0x11,
                                          0x97, 0xab, 0xa4, 0xb3, 0xd8, 0xd8,
                                          0x75, 0x6f, 0xf3, 0x08, 0x42, 0xde,
                                          0xe0, 0x60, 0xc8, 0x0a, 0x70, 0x89,
                                          0x6b, 0x74, 0x60, 0x27, 0xde, 0xf1,
                                          0x2e, 0x2d, 0x7c, 0xd6, 0xd6, 0xf9,
                                          0x54, 0x67, 0xa3, 0x34, 0xcd, 0xf0,
                                          0xa3, 0xe5, 0x22, 0x6c, 0x4b, 0x37,
                                          0x24, 0x4b, 0x22, 0x79, 0x58, 0xc4,
                                          0xef, 0x98, 0x62, 0x88, 0x1e, 0xca,
                                          0xc3, 0x5e, 0xd6, 0xa4, 0x04, 0x43,
                                          0xaa, 0x7b, 0xa6, 0x61, 0x92, 0xa0,
                                          0x6a, 0x4a, 0xa8, 0xc4, 0xf7, 0xce,
                                          0xd7, 0x6c, 0x61, 0x87};
        static const struct {
            int        d_line;   // source line number
            bdlt::Date d_begin;  // expected result
            bdlt::Date d_end;    // expected result
        } DATA[] = {
            //LN          BEGIN                     END
            //--  -----------------------  ------------------------
            { L_, bdlt::Date(1900, 2, 28), bdlt::Date(1900, 3,  28) },
            { L_, bdlt::Date(1999, 1, 31), bdlt::Date(2010, 1,  11) },
            { L_, bdlt::Date(2000, 1, 31), bdlt::Date(2000, 12, 31) },
            { L_, bdlt::Date(2000, 2, 29), bdlt::Date(2001, 7,  28) },
            { L_, bdlt::Date(2001, 4, 30), bdlt::Date(2010, 8,  28) },
            { L_, bdlt::Date(2003, 2, 28), bdlt::Date(2030, 9,  22) },
            { L_, bdlt::Date(2004, 2, 29), bdlt::Date(2044, 2,  28) }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int        LINE  = DATA[ti].d_line;
            const bdlt::Date BEGIN = DATA[ti].d_begin;
            const bdlt::Date END   = DATA[ti].d_end;

            bdlt::DatetimeTz d = Util::consumeDatetimeTzInRange(&fdv,
                                                                BEGIN,
                                                                END);
            ASSERT(d.dateTz().localDate() >= BEGIN);
            ASSERT(d.dateTz().localDate() <= END);

            if (veryVerbose) {
                T_ P_(LINE) P_(BEGIN) P_(END) P(d);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING `consumeDatetimeTz`
        //
        // Concerns:
        // 1. The function returns valid `DatetimeTz` values.
        //
        // Plan:
        // 1. Create a data array of size 80+20 bytes to simulate fuzz data.
        //
        // 2. Create a `FuzzDataView` from the fuzz data array.
        //
        // 3. Repeat the following steps 10 times:  (C-1)
        //
        //   1. Create a `DatetimeTz` object using the `FuzzDataView`.
        //
        //   2. Verify that the function returns valid values.  (C-1)
        //
        // Testing:
        //   DatetimeTz consumeDatetimeTz(bslim::FuzzDataView *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `consumeDatetimeTz`"
                          << "\n===========================" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0xbc, 0x27, 0x3a, 0x9d, 0x9c, 0xbb,
                                          0x03, 0x92, 0x3d, 0x32, 0x4e, 0x98,
                                          0x23, 0x50, 0x28, 0x37, 0xb7, 0xe1,
                                          0xe0, 0xef, 0xcf, 0xc5, 0x29, 0x11,
                                          0x97, 0xab, 0xa4, 0xb3, 0xd8, 0xd8,
                                          0x75, 0x6f, 0xf3, 0x08, 0x42, 0xde,
                                          0xe0, 0x60, 0xc8, 0x0a, 0x70, 0x89,
                                          0x6b, 0x74, 0x60, 0x27, 0xde, 0xf1,
                                          0x2e, 0x2d, 0x7c, 0xd6, 0xd6, 0xf9,
                                          0x54, 0x67, 0xa3, 0x34, 0xcd, 0xf0,
                                          0xa3, 0xe5, 0x22, 0x6c, 0x4b, 0x37,
                                          0x24, 0x4b, 0x22, 0x79, 0x58, 0xc4,
                                          0xef, 0x98, 0x62, 0x88, 0x1e, 0xca,
                                          0xc3, 0x5e, 0xd6, 0xa4, 0x04, 0x43,
                                          0xaa, 0x7b, 0xa6, 0x61, 0x92, 0xa0,
                                          0x6a, 0x4a, 0xa8, 0xc4, 0xf7, 0xce,
                                          0xd7, 0x6c, 0x61, 0x87};

        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
        const int           NUM_TESTS = 10;

        for (int ti = 0; ti < NUM_TESTS; ++ti) {
            bdlt::DatetimeTz d = Util::consumeDatetimeTz(&fdv);
            ASSERT(bdlt::DatetimeTz::isValid(d.localDatetime(), d.offset()));
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING `consumeDatetimeInRange`
        //
        // Concerns:
        // 1. The function returns valid `Datetime` values within the specified
        //    range.
        //
        // Plan:
        // 1. Create a data array of size 30+50 bytes to simulate fuzz data.
        //
        // 2. Create a `FuzzDataView` from the fuzz data array.
        //
        // 3. Using the table-driven technique specify a set of valid date
        //    ranges in columns `BEGIN` and `END`.
        //
        // 4. For each row in the table described in P-3:  (C-1)
        //
        //   1. Create a `Datetime` object using the `FuzzDataView`.
        //
        //   2. Verify that the function returns valid values within the
        //      specified range.  (C-1)
        //
        // Testing:
        //   Datetime consumeDatetimeInRange(bslim::FuzzDataView *, b, e);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `consumeDateTzInRange`"
                          << "\n==============================" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0xF8, 0x33, 0x45, 0x1F, 0x9D, 0x31,
                                          0xD9, 0x52, 0x1A, 0xF6, 0xAB, 0x73,
                                          0xEF, 0x18, 0x42, 0x67, 0xB3, 0x52,
                                          0xD2, 0x29, 0x5C, 0x9B, 0x0C, 0x94,
                                          0x43, 0x1F, 0xBA, 0x02, 0x95, 0x08,
                                          0x0E, 0xDA, 0x34, 0xDA, 0x6D, 0x25,
                                          0xD2, 0x9D, 0xA8, 0x24, 0xF1, 0xC2,
                                          0xD5, 0xD9, 0xCF, 0xED, 0x39, 0x54,
                                          0x7D, 0xAD, 0xA5, 0xD3, 0x23, 0x59,
                                          0xE8, 0x46, 0xB6, 0x85, 0x68, 0xB5,
                                          0x84, 0x0E, 0x71, 0x85, 0x09, 0xC1,
                                          0x4F, 0xC9, 0x1B, 0xD2, 0x00, 0x06,
                                          0xCD, 0xEC, 0x52, 0x5E, 0x35, 0x93,
                                          0x44, 0xB4};
        static const struct {
            int        d_line;   // source line number
            bdlt::Date d_begin;  // expected result
            bdlt::Date d_end;    // expected result
        } DATA[] = {
            //LN          BEGIN                     END
            //--  -----------------------  ------------------------
            { L_, bdlt::Date(1900, 2, 28), bdlt::Date(1900, 3,  28) },
            { L_, bdlt::Date(1999, 1, 31), bdlt::Date(2010, 1,  11) },
            { L_, bdlt::Date(2000, 1, 31), bdlt::Date(2000, 12, 31) },
            { L_, bdlt::Date(2000, 2, 29), bdlt::Date(2001, 7,  28) },
            { L_, bdlt::Date(2001, 4, 30), bdlt::Date(2010, 8,  28) },
            { L_, bdlt::Date(2003, 2, 28), bdlt::Date(2030, 9,  22) },
            { L_, bdlt::Date(2004, 2, 29), bdlt::Date(2044, 2,  28) }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int        LINE  = DATA[ti].d_line;
            const bdlt::Date BEGIN = DATA[ti].d_begin;
            const bdlt::Date END   = DATA[ti].d_end;

            bdlt::Datetime d = Util::consumeDatetimeInRange(&fdv, BEGIN, END);

            ASSERT(d.date() >= BEGIN);
            ASSERT(d.date() <= END);

            if (veryVerbose) {
                T_ P_(LINE) P_(BEGIN) P_(END) P(d);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING `consumeDatetime`
        //
        // Concerns:
        // 1. The function returns valid `Datetime` values.
        //
        // Plan:
        // 1. Create a data array of size 30+50 bytes to simulate fuzz data.
        //
        // 2. Create a `FuzzDataView` from the fuzz data array.
        //
        // 3. Repeat the following steps 10 times:  (C-1)
        //
        //   1. Create a `Datetime` object using the `FuzzDataView`.
        //
        //   2. Verify that the function returns valid values.  (C-1)
        //
        // Testing:
        //   Datetime consumeDatetime(bslim::FuzzDataView *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `consumeDatetime`"
                          << "\n=========================" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0xF8, 0x33, 0x45, 0x1F, 0x9D, 0x31,
                                          0xD9, 0x52, 0x1A, 0xF6, 0xAB, 0x73,
                                          0xEF, 0x18, 0x42, 0x67, 0xB3, 0x52,
                                          0xD2, 0x29, 0x5C, 0x9B, 0x0C, 0x94,
                                          0x43, 0x1F, 0xBA, 0x02, 0x95, 0x08,
                                          0x0E, 0xDA, 0x34, 0xDA, 0x6D, 0x25,
                                          0xD2, 0x9D, 0xA8, 0x24, 0xF1, 0xC2,
                                          0xD5, 0xD9, 0xCF, 0xED, 0x39, 0x54,
                                          0x7D, 0xAD, 0xA5, 0xD3, 0x23, 0x59,
                                          0xE8, 0x46, 0xB6, 0x85, 0x68, 0xB5,
                                          0x84, 0x0E, 0x71, 0x85, 0x09, 0xC1,
                                          0x4F, 0xC9, 0x1B, 0xD2, 0x00, 0x06,
                                          0xCD, 0xEC, 0x52, 0x5E, 0x35, 0x93,
                                          0x44, 0xB4};

        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
        const int           NUM_TESTS = 10;

        for (int ti = 0; ti < NUM_TESTS; ++ti) {
            bdlt::Datetime d = Util::consumeDatetime(&fdv);
            ASSERT(bdlt::Datetime::isValid(d.year(),
                                           d.month(),
                                           d.day(),
                                           d.hour(),
                                           d.minute(),
                                           d.second(),
                                           d.millisecond(),
                                           d.microsecond()));
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING `consumeTimeTz`
        //
        // Concerns:
        // 1. The function returns valid `TimeTz` values.
        //
        // Plan:
        // 1. Create a data array of size 50+20 bytes to simulate fuzz data.
        //
        // 2. Create a `FuzzDataView` from the fuzz data array.
        //
        // 3. Repeat the following steps 10 times:  (C-1)
        //
        //   1. Create a `TimeTz` object using the `FuzzDataView`.
        //
        //   2. Verify that the function returns valid values.  (C-1)
        //
        // Testing:
        //   TimeTz consumeTimeTz(bslim::FuzzDataView *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `consumeTimeTz`"
                          << "\n=======================" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0x09, 0xe8, 0x9e, 0x37, 0x00, 0xeb,
                                          0xeb, 0x37, 0x82, 0x8d, 0xd0, 0x8d,
                                          0xb4, 0x79, 0x09, 0xaa, 0x84, 0x2f,
                                          0x81, 0x85, 0xab, 0xb8, 0x89, 0x40,
                                          0xcc, 0x22, 0x04, 0x1f, 0x61, 0xa4,
                                          0x69, 0xd9, 0xa3, 0x31, 0x43, 0xb7,
                                          0xba, 0x6d, 0x6c, 0x55, 0xc8, 0xb6,
                                          0x41, 0xc9, 0xed, 0xfc, 0x64, 0x53,
                                          0x33, 0x3b, 0x1e, 0x58, 0x39, 0x28,
                                          0x01, 0x8f, 0xba, 0x27, 0x9b, 0x62,
                                          0x79, 0x6e, 0x38, 0x56, 0x53, 0xd6,
                                          0xa9, 0x93, 0x3c, 0x04};

        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
        const int           NUM_TESTS = 10;

        for (int ti = 0; ti < NUM_TESTS; ++ti) {
            bdlt::TimeTz t = Util::consumeTimeTz(&fdv);
            ASSERT(bdlt::TimeTz::isValid(t.localTime(), t.offset()));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING `consumeTime`
        //
        // Concerns:
        // 1. The function returns valid `Time` values.
        //
        // Plan:
        // 1. Using the table-driven technique specify a set of input bytes and
        //    the resulting time values in columns `BYTES` and `TIME`.
        //
        // 2. For each row in the table described in P-1:  (C-1)
        //
        //   1. Create a `Time` object using the `consumeTime` function and
        //      `BYTES`.
        //
        //   2. Verify that the resulting object is equal to `TIME`.
        //
        // Testing:
        //   Time consumeTime(bslim::FuzzDataView *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `consumeTime`"
                          << "\n=====================" << endl;

        static const struct {
            int          d_line;      // source line number
            bsl::uint8_t d_bytes[5];  // input bytes
            bdlt::Time   d_time;      // expected result
        } DATA[] = {
            //LN            BYTES                         TIME
            //--  --------------------------  ----------------------------
            { L_, {0x00,0x00,0x00,0x00,0x00}, bdlt::Time(0,0,0,0,0) },
            { L_, {0x14,0x1D,0xD7,0x5F,0xFF}, bdlt::Time(23,59,59,999,999) },
            { L_, {0x00,0xDD,0xE8,0x88,0x65}, bdlt::Time(1,2,3,4,5) },
            { L_, {0x04,0x3F,0x5E,0x1E,0x91}, bdlt::Time(5,4,3,2,1) },
            { L_, {0x0A,0x34,0x7B,0xC8,0xB4}, bdlt::Time(12,10,30,200,500) },
            { L_, {0xFF,0xFF,0xFF,0xFF,0xFF}, bdlt::Time(17,25,11,627,775) }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int        LINE = DATA[ti].d_line;
            const bdlt::Time TIME = DATA[ti].d_time;

            bslim::FuzzDataView fdv(DATA[ti].d_bytes,
                                    sizeof(DATA[ti].d_bytes));

            bdlt::Time t = Util::consumeTime(&fdv);
            LOOP2_ASSERT(LINE, t, t == TIME);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING `consumeDateTzInRange`
        //
        // Concerns:
        // 1. The function returns valid `DateTz` values within the specified
        //    range.
        //
        // Plan:
        // 1. Create a data array of size 30+20 bytes to simulate fuzz data.
        //
        // 2. Create a `FuzzDataView` from the fuzz data array.
        //
        // 3. Using the table-driven technique specify a set of valid date
        //    ranges in columns `BEGIN` and `END`.
        //
        // 4. For each row in the table described in P-3:  (C-1)
        //
        //   1. Create a `DateTz` object using the `FuzzDataView`.
        //
        //   2. Verify that the function returns valid values within the
        //      specified range.  (C-1)
        //
        // Testing:
        //   DateTz consumeDateTzInRange(bslim::FuzzDataView *, begin, end);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `consumeDateTzInRange`"
                          << "\n==============================" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0x74, 0x8a, 0x42, 0x0e, 0x6e, 0xab,
                                          0x80, 0x8e, 0x70, 0x4a, 0x79, 0x78,
                                          0x8f, 0x55, 0x2b, 0x86, 0x7c, 0x2a,
                                          0x04, 0x55, 0x78, 0xbd, 0xea, 0x52,
                                          0x14, 0x50, 0xed, 0xf7, 0xff, 0x74,
                                          0xf5, 0xfa, 0x1f, 0x8f, 0x6c, 0x2e,
                                          0x55, 0x98, 0x2e, 0xcd, 0x41, 0xcd,
                                          0xcc, 0x76, 0xf3, 0xc7, 0x9b, 0xcb,
                                          0x55, 0x23};
        static const struct {
            int        d_line;   // source line number
            bdlt::Date d_begin;  // expected result
            bdlt::Date d_end;    // expected result
        } DATA[] = {
            //LN          BEGIN                     END
            //--  -----------------------  ------------------------
            { L_, bdlt::Date(1900, 2, 28), bdlt::Date(1900, 3,  28) },
            { L_, bdlt::Date(1999, 1, 31), bdlt::Date(2010, 1,  11) },
            { L_, bdlt::Date(2000, 1, 31), bdlt::Date(2000, 12, 31) },
            { L_, bdlt::Date(2000, 2, 29), bdlt::Date(2001, 7,  28) },
            { L_, bdlt::Date(2001, 4, 30), bdlt::Date(2010, 8,  28) },
            { L_, bdlt::Date(2003, 2, 28), bdlt::Date(2030, 9,  22) },
            { L_, bdlt::Date(2004, 2, 29), bdlt::Date(2044, 2,  28) }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int        LINE  = DATA[ti].d_line;
            const bdlt::Date BEGIN = DATA[ti].d_begin;
            const bdlt::Date END   = DATA[ti].d_end;

            bdlt::DateTz d = Util::consumeDateTzInRange(&fdv, BEGIN, END);

            ASSERT(d.localDate() >= BEGIN);
            ASSERT(d.localDate() <= END);

            if (veryVerbose) {
                T_ P_(LINE) P_(BEGIN) P_(END) P(d);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING `consumeDateTz`
        //
        // Concerns:
        // 1. The function returns valid `DateTz` values.
        //
        // Plan:
        // 1. Create a data array of size 30+20 bytes to simulate fuzz data.
        //
        // 2. Create a `FuzzDataView` from the fuzz data array.
        //
        // 3. Repeat the following steps 10 times:  (C-1)
        //
        //   1. Create a `DateTz` object using the `FuzzDataView`.
        //
        //   2. Verify that the function returns valid values.  (C-1)
        //
        // Testing:
        //   DateTz consumeDateTz(bslim::FuzzDataView *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `consumeDateTz`"
                          << "\n=======================" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0x74, 0x8a, 0x42, 0x0e, 0x6e, 0xab,
                                          0x80, 0x8e, 0x70, 0x4a, 0x79, 0x78,
                                          0x8f, 0x55, 0x2b, 0x86, 0x7c, 0x2a,
                                          0x04, 0x55, 0x78, 0xbd, 0xea, 0x52,
                                          0x14, 0x50, 0xed, 0xf7, 0xff, 0x74,
                                          0xf5, 0xfa, 0x1f, 0x8f, 0x6c, 0x2e,
                                          0x55, 0x98, 0x2e, 0xcd, 0x41, 0xcd,
                                          0xcc, 0x76, 0xf3, 0xc7, 0x9b, 0xcb,
                                          0x55, 0x23};

        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
        const int           NUM_TESTS = 10;

        for (int ti = 0; ti < NUM_TESTS; ++ti) {
            bdlt::DateTz d = Util::consumeDateTz(&fdv);
            ASSERT(bdlt::DateTz::isValid(d.localDate(), d.offset()));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `consumeTz`
        //
        // Concerns:
        // 1. The function returns valid timezone offset values in minutes from
        //    the range (-24*60, 24*60).
        //
        // Plan:
        // 1. Using the table-driven technique specify a set of input bytes and
        //    the resulting TZ values in columns `BYTES` and `TZ`.
        //
        // 2. For each row in the table described in P-1:  (C-1)
        //
        //   1. Create a TZ value using the `consumeTz` function and `BYTES`.
        //
        //   2. Verify that the resulting value is equal to `TZ`.
        //
        // Testing:
        //   int consumeTz(bslim::FuzzDataView *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `consumeTz`"
                          << "\n===================" << endl;

        static const struct {
            int          d_line;      // source line number
            bsl::uint8_t d_bytes[2];  // input bytes
            int          d_tz;        // expected result
        } DATA[] = {
            //LN     BYTES        TZ
            //--  -----------  --------
            { L_, {0x00,0x00}, -24*60+1 },
            { L_, {0x0B,0x3E},  24*60-1 },
            { L_, {0x05,0x9F},  0       },
            { L_, {0xFF,0xFF},  758     }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE = DATA[ti].d_line;
            const int TZ   = DATA[ti].d_tz;

            bslim::FuzzDataView fdv(DATA[ti].d_bytes,
                                    sizeof(DATA[ti].d_bytes));

            int tz = Util::consumeTz(&fdv);
            LOOP2_ASSERT(LINE, tz, tz == TZ);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `consumeDate`
        //
        // Concerns:
        // 1. The function returns valid `Date` values.
        //
        // 2. The function uses the expected number of bytes (i.e., 3) of fuzz
        //    data to produce the `Date`.
        //
        // Plan:
        // 1. Create a data array of size 30 bytes to simulate fuzz data.
        //
        // 2. Create a `FuzzDataView` from the fuzz data array.
        //
        // 3. Repeat the following steps 10 times:  (C-1..2)
        //
        //   1. Create a `Date` object using the `FuzzDataView`.
        //
        //   2. Verify that the function returns valid `Date` values.  (C-1)
        //
        //   3. Verify that the number of bytes consumed is what was expected.
        //      (C-2)
        //
        // Testing:
        //   Date consumeDate(FuzzDataView *);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING `consumeDate`" << endl
                 << "====================" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0x43, 0x19, 0x0D, 0x44, 0x37, 0x0D,
                                          0x28, 0xDB, 0x01, 0xB4, 0x52, 0x41,
                                          0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA,
                                          0x20, 0x24, 0xF8, 0xD9, 0x1F, 0x14,
                                          0x7C, 0x5C, 0xDD, 0x6F, 0xB3, 0xD7};

        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
        const int           NUM_TESTS = 10;

        for (int ti = 0; ti < NUM_TESTS; ++ti) {

            bsl::size_t beforeBytes = fdv.length();
            bdlt::Date  d = Util::consumeDate(&fdv);
            bsl::size_t afterBytes = fdv.length();

            ASSERT(
                bdlt::Date::isValidYearMonthDay(d.year(), d.month(), d.day()));
            ASSERT(3 == beforeBytes - afterBytes);

            if (veryVerbose) {
                T_ P_(ti) P(d);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `consumeDateInRange`
        //
        // Concerns:
        // 1. The function returns valid `Date` values within the specified
        //    range.
        //
        // 2. The function uses the expected number of bytes of fuzz data to
        //     produce the `Date`.
        //
        // Plan:
        // 1. Create a data array of size 30 bytes to simulate fuzz data.
        //
        // 2. Create a `FuzzDataView` from the fuzz data array.
        //
        // 3. Using the table-driven technique:
        //
        //   1. Specify a set of valid date ranges in columns `BEGIN` and
        //      `END`.
        //
        //   2. Additionally, provide a column, `NUM_BYTES`, for the expected
        //      number of consumed bytes.
        //
        // 4. For each row in the table described in P-3:  (C-1..2)
        //
        //   1. Create a `Date` object by invoking `consumeDateInRange` with
        //      the `FuzzDataView` and the date range from the row.
        //
        //   2. Verify that the function returns dates within the specified
        //      date range.  (C-1)
        //
        //   3. Verify that the number of bytes consumed is what was specified
        //      in the table.  (C-2)
        //
        // Testing:
        //   Date consumeDateInRange(FuzzDataView *, begin, end);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING `consumeDateInRange`" << endl
                 << "===========================" << endl;

        const bsl::uint8_t  FUZZ_DATA[] = {0x43, 0x19, 0x0D, 0x44, 0x37, 0x0D,
                                           0x28, 0xDB, 0x01, 0xB4, 0x52, 0x41,
                                           0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA,
                                           0x20, 0x24, 0xF8, 0xD9, 0x1F, 0x14,
                                           0x7C, 0x5C, 0xDD, 0x6F, 0xB3, 0xD7};
        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));

        static const struct {
            int              d_line;     // source line number
            bdlt::Date       d_begin;    // expected result
            bdlt::Date       d_end;      // expected result
            bsl::size_t      d_numBytes; // expected number of consumed bytes
        } DATA[] = {
            //LN          BEGIN                     END              NUM_BYTES
            //--  -----------------------  ------------------------  ---------
            { L_, bdlt::Date(1900, 2, 28), bdlt::Date(1900, 3,  28),    1 },
            { L_, bdlt::Date(1999, 1, 31), bdlt::Date(2010, 1,  11),    2 },
            { L_, bdlt::Date(2000, 1, 31), bdlt::Date(2000, 12, 31),    2 },
            { L_, bdlt::Date(2000, 2, 29), bdlt::Date(2001, 7,  28),    2 },
            { L_, bdlt::Date(2001, 4, 30), bdlt::Date(2010, 8,  28),    2 },
            { L_, bdlt::Date(2003, 2, 28), bdlt::Date(2030, 9,  22),    2 },
            { L_, bdlt::Date(2004, 2, 29), bdlt::Date(2044, 2,  28),    2 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE      = DATA[ti].d_line;
            const bdlt::Date  BEGIN     = DATA[ti].d_begin;
            const bdlt::Date  END       = DATA[ti].d_end;
            const bsl::size_t NUM_BYTES = DATA[ti].d_numBytes;

            bsl::size_t beforeBytes = fdv.length();
            bdlt::Date  d = Util::consumeDateInRange(&fdv, BEGIN, END);
            bsl::size_t afterBytes = fdv.length();

            ASSERT(
                bdlt::Date::isValidYearMonthDay(d.year(), d.month(), d.day()));
            ASSERT(BEGIN     <= d);
            ASSERT(d         <= END);
            ASSERT(NUM_BYTES == beforeBytes - afterBytes);

            if (veryVerbose) {
                T_ P_(LINE) P_(BEGIN) P_(END) P(d);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Perform a test of the primary utilities.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0x43, 0x19, 0x0D, 0x44, 0x37, 0x0D,
                                          0x28, 0xDB, 0x01, 0xB4, 0x52, 0x41,
                                          0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA,
                                          0x20, 0x24, 0xF8, 0xD9, 0x1F, 0x14,
                                          0x7C, 0x5C, 0xDD, 0x6F, 0xB3, 0xD7};
        int                testIters   = 10;
        {
            bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
            ASSERTV(30 == fdv.length());

            if (veryVerbose) {
                cout << "Testing `consumeDate`" << endl;
            }
            for (int j = 0; j < testIters; ++j) {
                bdlt::Date date = Util::consumeDate(&fdv);
                if (veryVeryVerbose) {
                    T_ T_ P_(fdv.length()) P(date)
                }
                int y, m, d;
                date.getYearMonthDay(&y, &m, &d);
                ASSERT(bdlt::Date::isValidYearMonthDay(y, m, d));
            }
        }
        {
            bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
            ASSERTV(30 == fdv.length());

            bdlt::Date begin(1685, 3, 21);
            bdlt::Date end(1750, 7, 28);

            if (veryVerbose) {
                cout << "Testing `consumeDateInRange`" << endl;
            }
            for (int j = 0; j < testIters; ++j) {
                bdlt::Date d = Util::consumeDateInRange(&fdv, begin, end);
                if (veryVeryVerbose) {
                    T_ T_ P_(fdv.length()) P(d)
                }
                ASSERT(begin <= d);
                ASSERT(d     <= end);
            }
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
