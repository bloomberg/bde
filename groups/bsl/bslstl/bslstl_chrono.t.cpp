// bslstl_chrono.t.cpp                                                -*-C++-*-
#include <bslstl_chrono.h>

#include <bslmf_issame.h>
#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#include <chrono>
#endif

#include <stdio.h>     // 'sprintf', 'snprintf' [NOT '<cstdio>', which does not
                       // include 'snprintf']
#include <stdlib.h>    // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides a set of user-defined literal operators
// that forms a duration objects having various duration periods.  This test
// driver tests each implemented UDL operator.
//-----------------------------------------------------------------------------
// FREE OPERATORS
// [1] bsl::chrono::hours        operator "" _h  (unsigned long long);
// [1] bsl::chrono::duration     operator "" _h  (long double);
// [1] bsl::chrono::minutes      operator "" _min(unsigned long long);
// [1] bsl::chrono::duration     operator "" _min(long double);
// [1] bsl::chrono::seconds      operator "" _s  (unsigned long long);
// [1] bsl::chrono::duration     operator "" _s  (long double);
// [1] bsl::chrono::milliseconds operator "" _ms (unsigned long long);
// [1] bsl::chrono::duration     operator "" _ms (long double);
// [1] bsl::chrono::microseconds operator "" _us (unsigned long long);
// [1] bsl::chrono::duration     operator "" _us (long double);
// [1] bsl::chrono::nanoseconds  operator "" _ns (unsigned long long);
// [1] bsl::chrono::duration     operator "" _ns (long double);
// ----------------------------------------------------------------------------
// [5] USAGE EXAMPLE
// [2] CONCERN: clocks match those in 'bsls::SystemTime'
// [3] bsl::chrono::abs(std::chrono::duration)
// [3] bsl::chrono::ceil(std::chrono::duration)
// [3] bsl::chrono::floor(std::chrono::duration)
// [3] bsl::chrono::round(std::chrono::duration)
// [3] bsl::chrono::ceil(std::chrono::time_point)
// [3] bsl::chrono::floor(std::chrono::time_point)
// [3] bsl::chrono::round(std::chrono::time_point)
// [4] C++20 HEADER ADDITIONS

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace


// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

const bsls::Types::Int64 k_NANOSECONDS_PER_SECOND = 1000000000;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING USAGE EXAMPLE"
                            "\n=====================\n");

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Basic 'bsl'-chrono's UDLs Usage
/// - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates basic use of user-defined literal operators.
//
// First, we provide an access to 'bsl'-chrono's UDLs.

        using namespace bsl::chrono_literals;

// Then, we construct two duration objects that represent a 24-hours and a half
// an hour time intervals using 'operator "" _h'.

        auto hours_in_a_day = 24_h;
        auto halfhour       = 0.5_h;

// Finally, stream the two objects to 'stdout':

        printf("one day is %ld hours\n", hours_in_a_day.count());
        printf("half an hour is %.1f hours\n",
               static_cast<double>(halfhour.count()));
#else
        if (veryVerbose) { printf("Cannot test user-defined literals "
                                  "in pre-C++11 mode or if the compiler "
                                  "does not support inline namespaces.\n"); }
#endif

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING C++20 HEADER ADDITIONS
        //
        // Concerns:
        //: 1 The definitions from '<chrono>' defined by the C++20 Standard are
        //:   available in C++20 mode in the 'bsl::chrono' namespace to users
        //:   who include 'bslstl_chrono.h'.
        //
        // Plan:
        //: 1 For every identifier aliased from the 'std::chrono' namespace,
        //:   verify that the identifier exists and is usable with the
        //:   'bsl::chrono' namespace prefix.
        //
        // Testing:
        //   C++20 HEADER ADDITIONS
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING C++20 HEADER ADDITIONS"
                            "\n==============================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR
        (void) bsl::chrono::days{1};
        (void) bsl::chrono::weeks{1};
        (void) bsl::chrono::months{1};
        (void) bsl::chrono::years{1};

        BSLMF_ASSERT((bsl::chrono::is_clock<
                                           bsl::chrono::system_clock>::value));
        BSLMF_ASSERT((bsl::chrono::is_clock_v<bsl::chrono::system_clock>));

        (void) bsl::chrono::sys_time<bsl::chrono::hours>{
                                                        bsl::chrono::hours{1}};
        (void) bsl::chrono::sys_seconds{bsl::chrono::seconds{1}};
        (void) bsl::chrono::sys_days{bsl::chrono::days{1}};

        (void) bsl::chrono::file_clock::now();
        (void) bsl::chrono::file_time<bsl::chrono::hours>{
                                                        bsl::chrono::hours{1}};

        (void) static_cast<bsl::chrono::local_t*>(0);
        (void) bsl::chrono::local_time<bsl::chrono::hours>{
                                                        bsl::chrono::hours{1}};
        (void) bsl::chrono::local_seconds{bsl::chrono::seconds{1}};
        (void) bsl::chrono::local_days{bsl::chrono::days{1}};

        (void) bsl::chrono::last_spec{};
        (void) bsl::chrono::last;
        (void) bsl::chrono::day{1};
        (void) bsl::chrono::month{1};
        (void) bsl::chrono::January;
        (void) bsl::chrono::February;
        (void) bsl::chrono::March;
        (void) bsl::chrono::April;
        (void) bsl::chrono::May;
        (void) bsl::chrono::June;
        (void) bsl::chrono::July;
        (void) bsl::chrono::August;
        (void) bsl::chrono::September;
        (void) bsl::chrono::October;
        (void) bsl::chrono::November;
        (void) bsl::chrono::December;

        (void) bsl::chrono::year{2000};
        (void) bsl::chrono::weekday{1};
        (void) std::chrono::Sunday;
        (void) bsl::chrono::Monday;
        (void) bsl::chrono::Tuesday;
        (void) bsl::chrono::Wednesday;
        (void) bsl::chrono::Thursday;
        (void) bsl::chrono::Friday;
        (void) bsl::chrono::Saturday;

        using namespace bsl::chrono_literals;
        using bsl::chrono::May;
        using bsl::chrono::Monday;
        using bsl::chrono::last;
        (void) (1d/May/2000y);

        (void) bsl::chrono::weekday_indexed{Monday, 1};
        (void) bsl::chrono::weekday_last{Monday};
        (void) bsl::chrono::month_day{May, 1d};
        (void) bsl::chrono::month_day_last{May};
        (void) bsl::chrono::month_weekday{May, Monday[1]};
        (void) bsl::chrono::month_weekday_last{May, Monday[last]};
        (void) bsl::chrono::year_month{2000y, May};
        (void) bsl::chrono::year_month_day{2000y, May, 1d};
        (void) bsl::chrono::year_month_day_last{2000y, May/last};
        (void) bsl::chrono::year_month_weekday{2000y, May, Monday[1]};
        (void) bsl::chrono::year_month_weekday_last{2000y, May, Monday[last]};

        (void) bsl::chrono::hh_mm_ss<std::chrono::seconds>{1s};

        (void) bsl::chrono::is_am(1h);
        (void) bsl::chrono::is_pm(1h);
        (void) bsl::chrono::make12(1h);
        (void) bsl::chrono::make24(1h, true);

#ifndef BSLS_PLATFORM_OS_WINDOWS
        (void) bsl::chrono::utc_clock::now();
        (void) bsl::chrono::utc_time<bsl::chrono::hours>{
                                                        bsl::chrono::hours{1}};
        (void) bsl::chrono::utc_seconds{bsl::chrono::seconds{1}};

        (void) bsl::chrono::tai_clock::now();
        (void) bsl::chrono::tai_time<bsl::chrono::hours>{
                                                        bsl::chrono::hours{1}};
        (void) bsl::chrono::tai_seconds{bsl::chrono::seconds{1}};

        (void) bsl::chrono::gps_clock::now();
        (void) bsl::chrono::gps_time<bsl::chrono::hours>{
                                                        bsl::chrono::hours{1}};
        (void) bsl::chrono::gps_seconds{bsl::chrono::seconds{1}};

        (void) bsl::chrono::clock_time_conversion<
            bsl::chrono::utc_clock,
            bsl::chrono::system_clock
        >{}(bsl::chrono::system_clock::now());
        (void) bsl::chrono::clock_cast<bsl::chrono::utc_clock>(
                                             bsl::chrono::system_clock::now());

        (void) static_cast<bsl::chrono::tzdb_list*>(0);
        (void) static_cast<bsl::chrono::tzdb*>(0);
        (void) static_cast<bsl::chrono::sys_info*>(0);
        (void) static_cast<bsl::chrono::local_info*>(0);

        (void) bsl::chrono::get_tzdb_list();
        (void) bsl::chrono::get_tzdb();
        (void) bsl::chrono::remote_version();
        (void) bsl::chrono::reload_tzdb();

        const bsl::chrono::time_zone *tz = bsl::chrono::zoned_traits<
                               const bsl::chrono::time_zone *>::default_zone();
        (void) bsl::chrono::zoned_time<bsl::chrono::seconds>(tz);
        (void) bsl::chrono::zoned_seconds(tz);

        (void) bsl::chrono::choose::latest;
        (void) static_cast<bsl::chrono::time_zone_link*>(0);

        (void) static_cast<bsl::chrono::nonexistent_local_time*>(0);
        (void) static_cast<bsl::chrono::ambiguous_local_time*>(0);

        (void) static_cast<bsl::chrono::leap_second*>(0);
        (void) static_cast<bsl::chrono::leap_second_info*>(0);
        (void) bsl::chrono::get_leap_second_info<bsl::chrono::seconds>(
                            bsl::chrono::utc_seconds{bsl::chrono::seconds{1}});
#endif

        using std::istream; // 'bsl::istream' is unavailable in this package
        (void) [](istream &is) {
            bsl::chrono::sys_seconds result;
            bsl::chrono::from_stream(is, "", result);
        };
        (void) [](istream &is) {
            bsl::chrono::sys_seconds result;
            is >> bsl::chrono::parse("", result);
        };
#endif
      } break;
    case 3: {
      // ----------------------------------------------------------------------
      // TESTING C++17 MATH ADDITIONS TO <CHRONO>
      //
      // Concerns:
      //: 1 That the functions 'chrono::abs', chrono::ceil, chrono::floor and
      //:   chrono::round have been imported into the 'bsl::chrono' namespace.
      //
      // Plan:
      //: 1 Verify that simple calls to these functions successfully compile
      //:   and that they return sane results.
      //
      // Testing:
      //   bsl::chrono::abs(std::chrono::duration)
      //   bsl::chrono::ceil(std::chrono::duration)
      //   bsl::chrono::floor(std::chrono::duration)
      //   bsl::chrono::round(std::chrono::duration)
      //   bsl::chrono::ceil(std::chrono::time_point)
      //   bsl::chrono::floor(std::chrono::time_point)
      //   bsl::chrono::round(std::chrono::time_point)
      // ----------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 MATH ADDITIONS TO <CHRONO>"
                            "\n========================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        using namespace bsl::chrono_literals;

        ASSERT(42s == std::chrono::abs(-42s));
        ASSERT(2min == std::chrono::ceil<bsl::chrono::minutes>(112s));
        ASSERT(1min == std::chrono::floor<bsl::chrono::minutes>(112s));
        ASSERT(2min == std::chrono::round<bsl::chrono::minutes>(112s));

        using TimePoint = bsl::chrono::time_point<bsl::chrono::system_clock,
                                                  bsl::chrono::seconds>;
        TimePoint tp{112s};
        ASSERT(2min ==
               std::chrono::ceil<bsl::chrono::minutes>(tp).time_since_epoch());
        ASSERT(1min ==
              std::chrono::floor<bsl::chrono::minutes>(tp).time_since_epoch());
        ASSERT(2min ==
              std::chrono::round<bsl::chrono::minutes>(tp).time_since_epoch());

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

      } break;
    case 2: {
        // --------------------------------------------------------------------
        // CLOCK TESTS
        //   Ensure 'bsl::realtime_clock' matches
        //   'bsls::SystemTime::nowRealtimeClock' and 'bsl::monotonic_clock'
        //   matches 'bsls::SystemTime::nowMonotonicClock'.
        //
        // Concerns:
        //: 1 The epoch of 'bsl::realtime_clock' matches the epoch of
        //:   'bsls::SystemTime::nowRealtimeClock'.
        //:
        //: 2 The epoch of 'bsl::monotonic_clock' matches the epoch of
        //:   'bsls::SystemTime::nowMonotonicClock'.
        //
        // Plan:
        //: 1 For each clock, repeatedly measure the number of nanoseconds
        //:   since the epoch and verify the 90% percentile of these
        //:   differences is small.  (C-1,2)
        //
        // Testing:
        //   CONCERN: clocks match those in 'bsls::SystemTime'
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLOCK TESTS"
                            "\n===========\n");

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        ASSERT(false == bsl::chrono::system_clock::is_steady);
        ASSERT(true  == bsl::chrono::steady_clock::is_steady);

        const int k_ITERATIONS = 50;

        if (verbose) {
            printf("'bsl::chrono::system_clock' matches 'nowRealtimeClock'\n");
        }
        {
            int numSuccessCoarse = 0;
            int numSuccessFine   = 0;
            for (int i = 0; i < k_ITERATIONS; ++i) {
                using namespace bsl::chrono;

                nanoseconds::rep diff =
                      duration_cast<nanoseconds>(
                   bsl::chrono::system_clock::now().time_since_epoch()).count()
                     - bsls::SystemTime::nowRealtimeClock().totalNanoseconds();

                if (   -10 * k_NANOSECONDS_PER_SECOND <= diff
                    &&  10 * k_NANOSECONDS_PER_SECOND >= diff) {
                    ++numSuccessCoarse;
#if defined(BSLS_PLATFORM_OS_WINDOWS)
                    // Resolution on Windows is poor and acquisition is slow.
                    const bsls::Types::Int64 k_THRESHOLD =
                                                 k_NANOSECONDS_PER_SECOND / 10;
                    if (-k_THRESHOLD <= diff && k_THRESHOLD >= diff) {
#else
                    if (-1050 <= diff && 950 >= diff) {
                        // biased 50ns to account for duration of 'now' call
#endif

                        ++numSuccessFine;
                    }
                }
            }

            ASSERTV(numSuccessCoarse,
                    90 * k_ITERATIONS <= 100 * numSuccessCoarse);

            ASSERTV(numSuccessFine, 80 * k_ITERATIONS <= 100 * numSuccessFine);
        }

        if (verbose) {
            printf(
                  "'bsl::chrono::steady_clock' matches 'nowMonotonicClock'\n");
        }
        {
            int numSuccessCoarse = 0;
            int numSuccessFine   = 0;
            for (int i = 0; i < k_ITERATIONS; ++i) {
                using namespace bsl::chrono;

                nanoseconds::rep diff =
                      duration_cast<nanoseconds>(
                   bsl::chrono::steady_clock::now().time_since_epoch()).count()
                    - bsls::SystemTime::nowMonotonicClock().totalNanoseconds();

                if (   -10 * k_NANOSECONDS_PER_SECOND <= diff
                    &&  10 * k_NANOSECONDS_PER_SECOND >= diff) {
                    ++numSuccessCoarse;
                    if (-1050 <= diff && 950 >= diff) {
                        // biased 50ns to account for duration of 'now' call

                        ++numSuccessFine;
                    }
                }
            }

            ASSERTV(numSuccessCoarse,
                    90 * k_ITERATIONS <= 100 * numSuccessCoarse);

            ASSERTV(numSuccessFine, 80 * k_ITERATIONS <= 100 * numSuccessFine);
        }

        if (false == (bsl::is_same<std::chrono::system_clock,
                                   bsl::chrono::system_clock>::value)) {
            if (verbose) {
                printf("'std::chrono::system_clock' is not "
                       "'bsl::chrono::system_clock'\n");
            }

            int numSuccess = 0;
            for (int i = 0; i < k_ITERATIONS; ++i) {
                using namespace bsl::chrono;

                nanoseconds::rep diff =
                       duration_cast<nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count()
                     - duration_cast<nanoseconds>(
                  bsl::chrono::system_clock::now().time_since_epoch()).count();

                if (   -k_NANOSECONDS_PER_SECOND <= diff
                    &&  k_NANOSECONDS_PER_SECOND >= diff) {
                    ++numSuccess;
                }
            }

            ASSERTV(numSuccess, 0 == numSuccess);
        }

        if (false == (bsl::is_same<std::chrono::steady_clock,
                                   bsl::chrono::steady_clock>::value)) {
            if (verbose) {
                printf("'std::chrono::steady_clock' is not "
                       "'bsl::chrono::steady_clock'\n");
            }

            int numSuccess = 0;
            for (int i = 0; i < k_ITERATIONS; ++i) {
                using namespace bsl::chrono;

                nanoseconds::rep diff =
                       duration_cast<nanoseconds>(
                   std::chrono::steady_clock::now().time_since_epoch()).count()
                     - duration_cast<nanoseconds>(
                  bsl::chrono::steady_clock::now().time_since_epoch()).count();

                if (   -k_NANOSECONDS_PER_SECOND <= diff
                    &&  k_NANOSECONDS_PER_SECOND >= diff) {
                    ++numSuccess;
                }
            }

            ASSERTV(numSuccess, 0 == numSuccess);
        }
#else
        if (veryVerbose) printf("Cannot test clocks in pre-C++11 mode.\n");
#endif

    } break;
    case 1: {
      // ----------------------------------------------------------------------
      // TESTING User-defined literal operators
      //
      // Concerns:
      //: 1 That UDL operators correctly forward arguments to corresponding
      //:   constructors of 'bsl::chrono::duration' objects.
      //:
      //: 2 That an access to UDL operators can be gained using either
      //:   'bsl::literals', 'bsl::chrono_literals' or
      //:   'bsl::literals::chrono_literals' namespaces.
      //
      // Plan:
      //: 1 Verify that all UDL operators create expected chrono type objects
      //:   having the expected values.
      //
      // Testing:
      //   bsl::chrono::hours        operator "" _h  (unsigned long long);
      //   bsl::chrono::duration     operator "" _h  (long double);
      //   bsl::chrono::minutes      operator "" _min(unsigned long long);
      //   bsl::chrono::duration     operator "" _min(long double);
      //   bsl::chrono::seconds      operator "" _s  (unsigned long long);
      //   bsl::chrono::duration     operator "" _s  (long double);
      //   bsl::chrono::milliseconds operator "" _ms (unsigned long long);
      //   bsl::chrono::duration     operator "" _ms (long double);
      //   bsl::chrono::microseconds operator "" _us (unsigned long long);
      //   bsl::chrono::duration     operator "" _us (long double);
      //   bsl::chrono::nanoseconds  operator "" _ns (unsigned long long);
      //   bsl::chrono::duration     operator "" _ns (long double);
      // ----------------------------------------------------------------------

        if (verbose) printf("\nTesting User-defined literal operators"
                            "\n======================================\n");

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)


        if (verbose) printf("Testing 'operator \"\" _h'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::hours h = 1_h;
            ASSERT(1 == h.count());
            auto duration = 1.5_h;
            ASSERT(1.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_h;
            (void) mX;
            auto mY = 1.5_h;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_h;
            (void) mX;
            auto mY = 1.5_h;
            (void) mY;
        }

        if (verbose) printf("Testing 'operator \"\" _min'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::minutes min = 2_min;
            ASSERT(2 == min.count());
            auto duration = 2.5_min;
            ASSERTV(2.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_min;
            (void) mX;
            auto mY = 1.5_min;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_min;
            (void) mX;
            auto mY = 1.5_min;
            (void) mY;
        }

        if (verbose) printf("Testing 'operator \"\" _s'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::seconds sec = 3_s;
            ASSERT(3 == sec.count());
            auto duration = 3.5_s;
            ASSERTV(3.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_s;
            (void) mX;
            auto mY = 1.5_s;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_s;
            (void) mX;
            auto mY = 1.5_s;
            (void) mY;
        }

        if (verbose) printf("Testing 'operator \"\" _ms'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::milliseconds ms = 4_ms;
            ASSERT(4 == ms.count());
            auto duration = 4.5_ms;
            ASSERTV(4.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_ms;
            (void) mX;
            auto mY = 1.5_ms;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_ms;
            (void) mX;
            auto mY = 1.5_ms;
            (void) mY;
        }

        if (verbose) printf("Testing 'operator \"\" _us'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::microseconds us = 5_us;
            ASSERT(5 == us.count());
            auto duration = 5.5_us;
            ASSERTV(5.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_us;
            (void) mX;
            auto mY = 1.5_us;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_us;
            (void) mX;
            auto mY = 1.5_us;
            (void) mY;
        }

        if (verbose) printf("Testing 'operator \"\" _ns'\n");
        { // C-1,2
            using namespace bsl::chrono_literals;
            bsl::chrono::nanoseconds ns = 6_ns;
            ASSERT(6 == ns.count());
            auto duration = 6.5_ns;
            ASSERTV(6.5 == duration.count());
        }
        { // C-2
            using namespace bsl::literals;
            auto mX = 1_ns;
            (void) mX;
            auto mY = 1.5_ns;
            (void) mY;
        }
        { // C-2
            using namespace bsl::literals::chrono_literals;
            auto mX = 1_ns;
            (void) mX;
            auto mY = 1.5_ns;
            (void) mY;
        }
#else
        if (veryVerbose) { printf("Cannot test user-defined literals "
                                  "in pre-C++11 mode or if the compiler "
                                  "does not support inline namespaces.\n"); }

#endif  // BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
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
