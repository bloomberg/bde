// bsls_timeinterval.t.cpp                                            -*-C++-*-

#include <bsls_timeinterval.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_fuzztest.h>
#include <bsls_keyword.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>

#include <limits.h>  // LLONG_MAX
#include <math.h>    // fabs
#include <stdio.h>   // printf
#include <stdlib.h>
#include <string.h>  // memset, memcpy

#if BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <array>
#include <cstdint>    // std::intmax_t
#include <ratio>
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

using namespace BloombergLP;
using namespace std;
using bsls::NameOf;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test implements a value-semantic type.  The canonical
// representation for the time interval is in seconds and nanoseconds, where
// nanoseconds must be in the range '[ -999999999..999999999 ]'.  This test
// driver is boot-strapped using the manipulators and accessors for the
// canonical representation, and then extended for the complete set of methods.
//
// We follow our standard ten-case approach to testing value-semantic types,
// except that we will verify test apparatus in case 3 (in lieu of the
// generator function, 'gg'), with the default constructor, (trivial)
// destructor, and primary manipulator ('setTotalMilliseconds') tested fully in
// case 2.
//
// Primary Manipulators:
//: o void setIntervalRaw(bsls::Types::Int64 secs, int nanoseconds);
//
//
// Basic Accessors:
//: o int nanoseconds() const;
//: o bsls::Types::Int64 seconds() const;
//
// Certain standard value-semantic-type test cases are omitted:
//: o [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR/MANIPULATOR/OPERATOR ptr./ref. parameters are declared 'const'.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [22] bool isValid(Int64, int);
// [25] bool isValid(const std::chrono::duration& duration);
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [ 2] TimeInterval();
// [12] TimeInterval(Int64, int);
// [13] TimeInterval(double);
// [ 7] TimeInterval(const TimeInterval&);
// [26] TimeInterval(const std::chrono::duration<REP, PERIOD>& duration);
// [ 2] ~TimeInterval();
//
// MANIPULATORS
// [ 9] TimeInterval& operator=(const TimeInterval&);
// [13] TimeInterval& operator=(double);
// [19] TimeInterval& operator+=(const TimeInterval&);
// [19] TimeInterval& operator+=(double);
// [19] TimeInterval& operator-=(const TimeInterval&);
// [19] TimeInterval& operator-=(double);
// [18] TimeInterval& addDays(bsls::Types::Int64);
// [18] TimeInterval& addHours(bsls::Types::Int64);
// [18] TimeInterval& addMinutes(bsls::Types::Int64);
// [18] TimeInterval& addSeconds(bsls::Types::Int64);
// [18] TimeInterval& addMilliseconds(Int64);
// [18] TimeInterval& addMicroseconds(Int64);
// [18] TimeInterval& addNanoseconds(Int64);
// [16] void setTotalDays(int);
// [16] void setTotalHours(Int64);
// [16] void setTotalMinutes(Int64);
// [16] void setTotalSeconds(Int64);
// [16] void setTotalMilliseconds(Int64);
// [16] void setTotalMicroseconds(Int64);
// [16] void setTotalNanoseconds(Int64);
// [18] TimeInterval& addInterval(Int64, int);
// [26] TimeInterval& addDuration(const chrono::duration<REP, PERIOD>& d);
// [11] void setInterval(Int64, int);
// [ 2] void setIntervalRaw(Int64, int);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] bsls::Types::Int64 seconds() const;
// [ 4] int nanoseconds() const;
// [17] Int64 totalSeconds() const;
// [17] Int64 totalMinutes() const;
// [17] Int64 totalHours() const;
// [17] Int64 totalDays() const;
// [17] Int64 totalMilliseconds() const;
// [17] Int64 totalMicroseconds() const;
// [17] Int64 totalNanoseconds() const;
// [17] double totalSecondsAsDouble() const;
// [26] bool isInDurationRange() const;
// [27] DURATION_TYPE asDuration() const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 5] ostream& print(ostream&, int, int) const;
//
// FREE OPERATORS
// [20] TimeInterval operator+(const Obj& lhs, const Obj& rhs);
// [20] TimeInterval operator+(const Obj& lhs, double rhs);
// [20] TimeInterval operator+(double lhs, const Obj& rhs);
// [20] TimeInterval operator-(const Obj& lhs, const Obj& rhs);
// [20] TimeInterval operator-(const Obj& lhs, double rhs);
// [20] TimeInterval operator-(double lhs, const Obj& rhs);
// [21] TimeInterval operator-(const TimeInterval& rhs);
// [ 6] bool operator==(const TimeInterval&, const TimeInterval&);
// [14] bool operator==(const TimeInterval&, double);
// [14] bool operator==(double, const TimeInterval&);
// [ 6] bool operator!=(const TimeInterval&, const TimeInterval&);
// [14] bool operator!=(const TimeInterval&, double);
// [14] bool operator!=(double, const TimeInterval&);
// [15] bool operator< (const TimeInterval&, const TimeInterval&);
// [15] bool operator< (const TimeInterval&, double);
// [15] bool operator< (double, const TimeInterval&);
// [15] bool operator<=(const TimeInterval&, const TimeInterval&);
// [15] bool operator<=(const TimeInterval&, double);
// [15] bool operator<=(double, const TimeInterval&);
// [15] bool operator> (const TimeInterval&, const TimeInterval&);
// [15] bool operator> (const TimeInterval&, double);
// [15] bool operator> (double, const TimeInterval&);
// [15] bool operator<=(const TimeInterval&, const TimeInterval&);
// [15] bool operator<=(const TimeInterval&, double);
// [15] bool operator>=(double, const TimeInterval&);
// [28] TimeInterval operator ""_h(  unsigned long long int hours);
// [28] TimeInterval operator ""_min(unsigned long long int minutes);
// [28] TimeInterval operator ""_s(  unsigned long long int seconds);
// [28] TimeInterval operator ""_ms( unsigned long long int milliseconds);
// [28] TimeInterval operator ""_us( unsigned long long int microseconds);
// [28] TimeInterval operator ""_ns( unsigned long long int nanoseconds);
#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation
// DEPRECATED
// [10] static int maxSupportedBdexVersion();
#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
// [10] static int maxSupportedVersion();
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
// [ 5] ostream& operator<<(ostream&, const TimeInterval&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TESTING TEST-DRIVER MACHINERY
// [29] USAGE EXAMPLE
// [ 8] Reserved for 'swap' testing.
// [23] CONCERN: DRQS 65043434
// [24] TESTING STD::CHRONO HELPER CLASSES

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

// ============================================================================
//                  MACROS FOR PLATFORM SPECIFIC TESTS
// ----------------------------------------------------------------------------

#if BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#define BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
     // This macro definition parallels that defined in the header file.
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::TimeInterval Obj;
typedef bsls::Types::Int64 Int64;

#define BUFFER_SIZE 512            // must be large enough to enable BDEX tests
#define VERSION_SELECTOR 20140601

const int k_MILLISECS_PER_SEC     = 1000;        // one thousand
const int k_MICROSECS_PER_SEC     = 1000000;     // one million
const int k_NANOSECS_PER_MICROSEC = 1000;        // one thousand
const int k_NANOSECS_PER_MILLISEC = 1000000;     // one million
const int k_NANOSECS_PER_SEC      = 1000000000;  // one billion

const int k_SECS_PER_MIN          = 60;
const int k_SECS_PER_HOUR         = 60 * k_SECS_PER_MIN;
const int k_SECS_PER_DAY          = 24 * k_SECS_PER_HOUR;

const bsls::Types::Int64 k_SECS_MAX = LLONG_MAX;         // max number of secs
const bsls::Types::Int64 k_SECS_MIN = LLONG_MIN;         // min number of secs

const bsls::Types::Int64 k_MINS_MAX  = LLONG_MAX / 60;   // max number of mins
const bsls::Types::Int64 k_MINS_MIN  = LLONG_MIN / 60;   // min number of mins

const bsls::Types::Int64 k_HOURS_MAX = LLONG_MAX / 3600; // max number of hours
const bsls::Types::Int64 k_HOURS_MIN = LLONG_MIN / 3600; // min number of hours

const bsls::Types::Int64 k_DAYS_MAX  = LLONG_MAX / 86400; // max number of days
const bsls::Types::Int64 k_DAYS_MIN  = LLONG_MIN / 86400; // min number of days

const int k_BDEX_SIZEOF_INT32 = 4;
const int k_BDEX_SIZEOF_INT64 = 8;

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
template <class REP, class PERIOD> using DurationTraits =
                                bsls::TimeInterval_DurationTraits<REP, PERIOD>;
    // Shortcut alias for 'bsls::TimeInterval_DurationTraits' types.

enum RangeCheckResultIndices {
    // Exposition only, explanations for the individual elements of
    // 'RangeCheckResults.  Separate 'RangeCheckResults' are used for 'signed'
    // and 'unsigned' integers.

    e_EXP_8,      // expected range check result for 8  bit representation
    e_EXP_16,     // expected range check result for 16 bit representation
    e_EXP_32,     // expected range check result for 32 bit representation
    e_EXP_64,     // expected range check result for 64 bit representation
    e_NUM_OF_EXP  // the number of integer sizes tested
};

using RangeCheckResults = std::array<bool, e_NUM_OF_EXP>;
    // This array type is intended to store range-check results of
    // 'TimeInterval' <==> 'std::chrono::duration' conversions for each integer
    // (duration representation) size we test for.  Two object of this array
    // type are used to hold results for both 'signed' and 'unsigned' types.
    // Then there is a one of those pairs-of-arrays for each duration period we
    // test:
    //..
    //  using TestedPeriod = std::nano;
    //
    //  const RangeCheckResults
    //                        expectedNanoSignedChar{ true, true, true, true };
    //
    //  ~~~ testing signed 'duration' to 'TimeInterval' range checker
    //..

template <class VALUE_TYPE, class REP, class PERIOD>
void verifyIsInTimeIntervalRange(int        line,
                                 VALUE_TYPE count,
                                 bool       expectedResult)
    // Verify that the result of the 'TimeInterval::isValid' call for the
    // 'std::chrono::duration<REP, PERIOD>' object having the specified
    // 'count', is equal to the specified 'expectedResult'.   As conventional,
    // the specified 'line' identifies the row of the test table that is the
    // source of the data.  The behavior is undefined unless 'VALUE_TYPE' is
    // either of 'std::int64_t' or 'std::uint64_t'.  We require the largest
    // possible types (signed or unsigned depending on the test) so that we can
    // represent all possible values we need to work with.
{
    using std::chrono::duration;

    BSLS_KEYWORD_CONSTEXPR VALUE_TYPE k_REP_MIN =
                                               std::numeric_limits<REP>::min();
    BSLS_KEYWORD_CONSTEXPR VALUE_TYPE k_REP_MAX =
                                               std::numeric_limits<REP>::max();

    // We do not attempt to test values that do not fit into "REP' range.

    if (count >= k_REP_MIN && count <= k_REP_MAX) {
        const bool                  k_EXPECTED = expectedResult;
        const duration<REP, PERIOD> k_DUR_VALUE(count);
        const bool                  k_RESULT =
                         bsls::TimeInterval::isValid<REP, PERIOD>(k_DUR_VALUE);

        ASSERTV(line,
                NameOf<VALUE_TYPE>().name(),
                NameOf<REP>().name(),
                count,
                k_RESULT,
                k_EXPECTED == k_RESULT);
    }
}

template <class PERIOD>
void verifyTimeIntervalRangeCheck(int                      line,
                                  std::uint64_t            count,
                                  const RangeCheckResults& expectedResults)
    // Verify that the result of the 'TimeInterval::isValid' call for the
    // 'std::chrono::duration' object, having the specified 'count', the
    // (template parameter) PERIOD and any signed integer representation
    // matches the corresponding entry of the specified 'expectedResults'
    // array.  As conventional, the specified 'line' identifies the row of the
    // test table that is the source of the data.
{
    verifyIsInTimeIntervalRange<std::int64_t, std::int8_t,  PERIOD>(
                                                     line,
                                                     count,
                                                     expectedResults[e_EXP_8]);
    verifyIsInTimeIntervalRange<std::int64_t, std::int16_t, PERIOD>(
                                                    line,
                                                    count,
                                                    expectedResults[e_EXP_16]);
    verifyIsInTimeIntervalRange<std::int64_t, std::int32_t, PERIOD>(
                                                    line,
                                                    count,
                                                    expectedResults[e_EXP_32]);
    verifyIsInTimeIntervalRange<std::int64_t, std::int64_t, PERIOD>(
                                                    line,
                                                    count,
                                                    expectedResults[e_EXP_64]);
}

template <class PERIOD>
void verifyTimeIntervalRangeCheckUnsigned(
                                      int                      line,
                                      std::uint64_t            count,
                                      const RangeCheckResults& expectedResults)
    // Verify that the result of the 'TimeInterval::isValid' call for the
    // 'std::chrono::duration' object, having the specified 'count', the
    // (template parameter) PERIOD and any unsigned integer representation
    // matches the corresponding entry of the specified 'expectedResults'
    // array.  As conventional, the specified 'line' identifies the row of the
    // test table that is the source of the data.
{
    verifyIsInTimeIntervalRange<std::uint64_t, std::uint8_t,  PERIOD>(
                                                     line,
                                                     count,
                                                     expectedResults[e_EXP_8]);
    verifyIsInTimeIntervalRange<std::uint64_t, std::uint16_t, PERIOD>(
                                                    line,
                                                    count,
                                                    expectedResults[e_EXP_16]);
    verifyIsInTimeIntervalRange<std::uint64_t, std::uint32_t, PERIOD>(
                                                    line,
                                                    count,
                                                    expectedResults[e_EXP_32]);
    verifyIsInTimeIntervalRange<std::uint64_t, std::uint64_t, PERIOD>(
                                                    line,
                                                    count,
                                                    expectedResults[e_EXP_64]);
}

template <class REP, class PERIOD>
void verifyIsInDurationRange(int                       line,
                             const bsls::TimeInterval& interval,
                             bool                      expectedResult)
    // Verify that the result of the
    // 'TimeInterval::isInDurationRange<std::chrono::duration<REP, PERIOD> >'
    // call for the specified 'interval' is equal to the specified
    // 'expectedResult'.   The specified 'line' points to the specific row in
    // the test table to facilitate debugging in case of error.
{
    using std::chrono::duration;
    using Duration   = duration<REP, PERIOD>;

    const bool k_EXPECTED = expectedResult;
    const bool k_RESULT   =
                    interval.bsls::TimeInterval::isInDurationRange<Duration>();

    ASSERTV(line,
            NameOf<REP>().name(),
            interval.seconds(),
            interval.nanoseconds(),
            k_RESULT,
            k_EXPECTED == k_RESULT);
}

template <class PERIOD>
void verifyDurationRangeCheck(int                       line,
                              const bsls::TimeInterval& interval,
                              const RangeCheckResults&  expectedResults)
    // Verify that the result of the 'TimeInterval::isInDurationRange' call
    // parameterized by 'std::chrono::duration' type having any signed integer
    // representation and the (template parameter) PERIOD, for the specified
    // 'interval' matches the corresponding entry of the specified
    // 'expectedResults' array.  The specified 'line' points to the specific
    // row in the test table to facilitate debugging in case of error.
{
    verifyIsInDurationRange<std::int8_t,  PERIOD>(line,
                                                  interval,
                                                  expectedResults[e_EXP_8]);
    verifyIsInDurationRange<std::int16_t, PERIOD>(line,
                                                  interval,
                                                  expectedResults[e_EXP_16]);
    verifyIsInDurationRange<std::int32_t, PERIOD>(line,
                                                  interval,
                                                  expectedResults[e_EXP_32]);
    verifyIsInDurationRange<std::int64_t, PERIOD>(line,
                                                  interval,
                                                  expectedResults[e_EXP_64]);
}

template <class PERIOD>
void verifyDurationRangeCheckUnsigned(int                       line,
                                      const bsls::TimeInterval& interval,
                                      const RangeCheckResults&  expectedResults)
    // Verify that the result of the 'TimeInterval::isInDurationRange' call
    // parameterized by 'std::chrono::duration' type having any unsigned
    // integer representation and the (template parameter) PERIOD, for the
    // specified 'interval' matches the corresponding entry of the specified
    // 'expectedResults' array.  The specified 'line' points to the specific
    // row in the test table to facilitate debugging in case of error.
{
    verifyIsInDurationRange<std::uint8_t,  PERIOD>(line,
                                                   interval,
                                                   expectedResults[e_EXP_8]);
    verifyIsInDurationRange<std::uint16_t, PERIOD>(line,
                                                   interval,
                                                   expectedResults[e_EXP_16]);
    verifyIsInDurationRange<std::uint32_t, PERIOD>(line,
                                                   interval,
                                                   expectedResults[e_EXP_32]);
    verifyIsInDurationRange<std::uint64_t, PERIOD>(line,
                                                   interval,
                                                   expectedResults[e_EXP_64]);
}

template <class TYPE>
bool isDuration(bool expected)
    // Return 'true' if the value of the 'bsls::TimeInterval_IsDuration' trait
    // for the (template parameter) 'TYPE' is equal to the specified
    // 'expected', and 'false' otherwise.
{
    return (expected == bsls::TimeInterval_IsDuration<TYPE>::value);
}

template <class TYPE>
bool isFloat(bool expected)
    // Return 'true' if the value of the 'k_IS_FLOAT' member of the
    // 'bsls::TimeInterval_RepTraits' trait for the (template parameter) 'TYPE'
    // is equal to the specified 'expected', and 'false' otherwise.
{
    return (expected == bsls::TimeInterval_RepTraits<TYPE>::k_IS_FLOAT);
}

template <class REP, class PERIOD>
bool isDurationFloat(bool expected)
    // Return 'true' if the value of the 'k_IS_FLOAT' member of the
    // 'bsls::TimeInterval_DurationTraits' trait for the (template parameter)
    // 'TYPE' is equal to the specified 'expected', and 'false' otherwise.
{
    return (expected == DurationTraits<REP, PERIOD>::k_IS_FLOAT);
}

template <class REP, class PERIOD>
bool isDurationImplicit(bool expected)
    // Return 'true' if the value of the 'k_IS_IMPLICIT' member of the
    // 'bsls::TimeInterval_DurationTraits' trait for the (template parameter)
    // 'TYPE' is equal to the specified 'expected', and 'false' otherwise.
{
    return (expected == DurationTraits<REP, PERIOD>::k_IS_IMPLICIT);
}

template <class REP, class PERIOD>
bool isImplicitEnabled(bool expected)
    // Return 'true' if the value of the 'k_IMPLICIT_CONVERSION_ENABLED' member
    // of the 'bsls::TimeInterval_DurationTraits' trait for the (template
    // parameter) 'TYPE' is equal to the specified 'expected', and 'false'
    // otherwise.
{
    return (
       expected == DurationTraits<REP, PERIOD>::k_IMPLICIT_CONVERSION_ENABLED);
}

template <class REP, class PERIOD>
bool isExplicitEnabled(bool expected)
    // Return 'true' if the value of the 'k_EXPLICIT_CONVERSION_ENABLED' member
    // of the 'bsls::TimeInterval_DurationTraits' trait for the (template
    // parameter) 'TYPE' is equal to the specified 'expected', and 'false'
    // otherwise.
{
    return (
       expected == DurationTraits<REP, PERIOD>::k_EXPLICIT_CONVERSION_ENABLED);
}


template <class REP, class PERIOD>
struct DummyDuration {
    // Primitive imitation of the 'std::chrono::duration' that is used for
    // 'TimeInterval_IsDuration' metafunction testing.  Only the member types
    // are provided because nothing else is required for the testing purposes.

    // TYPES
    using Rep    = REP;
    using Period = PERIOD;
};

template <class REP, class PERIOD>
void testChronoConversion(std::chrono::duration<REP, PERIOD> value)
    // ------------------------------------------------------------------------
    // TESTING CONVERSION FROM/TO STD::CHRONO::DURATION
    //
    // Concerns:
    //: 1 Any valid 'std::chrono::duration' object that does not violate the
    //:   constructor's documented preconditions (i.e., non-float
    //:   representation type and fitting within the specified frames value)
    //:   can be successfully converted to 'TimeInterval'.
    //:
    //: 2 Negative 'std::chrono::duration' values can be used in the same way
    //:   as positive ones.
    //:
    //: 4 Any valid 'TimeInterval' object that does not violate the
    //:   method's documented preconditions can be successfully converted
    //:   to 'std::chrono::duration'.
    //
    // Plan:
    //: 1 Pass several different 'std::chrono::duration' values (positive and
    //:   negative) to 'TimeInterval' constructor and verify the result.
    //:   (C-1..2)
    //:
    //: 2 Using 'asDuration' method store the value of the 'TimeInterval'
    //:   object, obtained in the P-1 to 'std::chrono::duration' object and
    //:   verify the result.  (C-3)
    //
    // Testing:
    //   TimeInterval(const std::chrono::duration<REP, PERIOD>& duration);
    //   DURATION_TYPE asDuration() const;
    // ------------------------------------------------------------------------
{
    using Seconds     = std::chrono::duration<std::int64_t, std::ratio<1> >;
    using Nanoseconds = std::chrono::duration<std::int64_t, std::nano     >;

    const Seconds     k_SECONDS = std::chrono::duration_cast<Seconds>(value);
    const Nanoseconds k_NANOSECONDS = std::chrono::duration_cast<Nanoseconds>(
                                                            value - k_SECONDS);

    // Constructor from 'std::chrono::duration'.

    Obj        mX(value);
    const Obj& X = mX;

    ASSERTV(NameOf<REP>().name(),
            value.count(),
            k_SECONDS.count(),
            X.seconds(),
            k_SECONDS.count() == X.seconds());

    ASSERTV(NameOf<REP>().name(),
            value.count(),
            k_NANOSECONDS.count(),
            X.nanoseconds(),
            k_NANOSECONDS.count() == X.nanoseconds());

    // 'asDuration'.

    const std::chrono::duration<REP, PERIOD> resultDuration =
                           X.asDuration<std::chrono::duration<REP, PERIOD> >();

    if (PERIOD::den <= 1000000000) {
        // Types, equal or bigger than nanoseconds.

        ASSERTV(NameOf<REP>().name(), value == resultDuration);
    } else {
        // As 'TimeInterval' constructor cuts off the fractions of nanoseconds,
        // we will get rounded result from the 'asDuration' for types less than
        // nanoseconds.

        typedef std::chrono::duration<REP, PERIOD> Duration;
        const Duration EXPECTED = std::chrono::duration_cast<Duration>(
                            std::chrono::duration_cast<Nanoseconds>(value));

        ASSERTV(NameOf<REP>().name(), EXPECTED == resultDuration);
    }
}

template <class REP, class PERIOD>
void testChronoAddition(
      std::chrono::duration<REP, PERIOD> duration,
      typename std::enable_if<!bsls::TimeInterval_DurationTraits<REP, PERIOD>::
                                   k_IMPLICIT_CONVERSION_ENABLED,
                              int>::type * = 0)
    // ------------------------------------------------------------------------
    // The 'addDuration' is not implemented for 'std::chrono::duration' types
    // with a floating point representation or if the 'duration' itself can
    // *not* be *exactly* represented by an integer nanoseconds.  This function
    // is called for such cases and does nothing.
    // ------------------------------------------------------------------------
{
    (void) duration;  // suppress compiler warning
}

template <class REP, class PERIOD>
void testChronoAddition(
       std::chrono::duration<REP, PERIOD> value,
       typename std::enable_if<bsls::TimeInterval_DurationTraits<REP, PERIOD>::
                                   k_IMPLICIT_CONVERSION_ENABLED,
                               int>::type * = 0)
    // ------------------------------------------------------------------------
    // TESTING 'addDuration'
    //
    // Concerns:
    //: 1 Any valid 'std::chrono::duration' object that does not violate the
    //:   method's documented preconditions (i.e., non-float representation
    //:   type and fitting within the specified frames value) can be
    //:   successfully added to 'TimeInterval' object.
    //:
    //: 2 'std::chrono::duration' object can be successfully added to any
    //:   valid 'TimeInterval' object that does not violate the method's
    //:   documented preconditions.
    //:
    //: 3 Negative 'std::chrono::duration' values can be used in the same way
    //:   as positive ones.
    //:
    //: 4  The reference returned is to the target object (i.e., '*this').
    //
    // Plan:
    //: 1 Add several different 'std::chrono::duration' values (positive and
    //:   negative) to several different 'TimeInterval' objects and verify
    //:   the value of returned reference.  (C-4)
    //:
    //: 2 Verify that the result is equivalent to using the 'addInterval'.
    //:   (C-1..3)
    //
    // Testing:
    //   TimeInterval& addDuration(const chrono::duration<REP, PERIOD>& d);
    // ------------------------------------------------------------------------
{
    using std::chrono::duration;

    using Seconds             = duration<std::int64_t, std::ratio<1> >;
    using Nanoseconds         = duration<std::int64_t, std::nano     >;

    const Seconds     k_SECONDS     =
                                    std::chrono::duration_cast<Seconds>(value);
    const Nanoseconds k_NANOSECONDS =
                    std::chrono::duration_cast<Nanoseconds>(value - k_SECONDS);

    const std::int64_t k_SHIFT =  k_SECONDS.count() > 0
                               ?  k_SECONDS.count()
                               : -k_SECONDS.count();
        // Absolute value for calculating the minimum allowable and maximum
        // allowable values of 'TimeInterval' object.  Incoming parameters can
        // be either positive or negative, so we need to find absolute value to
        // avoid overflow.

    // Positive 'TimeInterval' values.

    const Int64 k_POSITIVE_SECONDS_LIMIT     = LLONG_MAX - k_SHIFT;
    const int   k_POSITIVE_NANOSECONDS_LIMIT = 999999999;

    for (Int64 i = 0, s = k_POSITIVE_SECONDS_LIMIT; i > 0; i = s, s /= 10) {
        for (int j = 0, ns = k_POSITIVE_NANOSECONDS_LIMIT;
             j > 0;
             j = ns, ns /= 10) {
            Obj        mX(s, ns);  // test object
            const Obj& X = mX;
            Obj        mZ(s, ns);  // control object
            const Obj& Z = mZ;

            ASSERTV(NameOf<REP>().name(), s  == X.seconds());
            ASSERTV(NameOf<REP>().name(), ns == X.nanoseconds());
            ASSERTV(NameOf<REP>().name(), s  == Z.seconds());
            ASSERTV(NameOf<REP>().name(), ns == Z.nanoseconds());

            Obj *mR = &(mX.addDuration(value));
            mZ.addInterval(static_cast<Int64>(k_SECONDS.count()),
                           static_cast<int>(k_NANOSECONDS.count()));

            ASSERTV(mR == &mX);

            ASSERTV(NameOf<REP>().name(), Z.seconds(), Z.nanoseconds(),
                                          X.seconds(), X.nanoseconds(),
                    Z == X);
        }
    }

    // Negative 'TimeInterval' values.

    const Int64 k_NEGATIVE_SECONDS_LIMIT     = LLONG_MIN + k_SHIFT;
    const int   k_NEGATIVE_NANOSECONDS_LIMIT = -999999999;

    for (Int64 i = 0, s = k_NEGATIVE_SECONDS_LIMIT; i < 0; i = s, s /= 10) {
        for (int j = 0, ns = k_NEGATIVE_NANOSECONDS_LIMIT;
             j < 0;
             j = ns, ns /= 10) {
            Obj        mX(s, ns);  // test object
            const Obj& X = mX;
            Obj        mZ(s, ns);  // control object
            const Obj& Z = mZ;

            ASSERTV(NameOf<REP>().name(), s  == X.seconds());
            ASSERTV(NameOf<REP>().name(), ns == X.nanoseconds());
            ASSERTV(NameOf<REP>().name(), s  == Z.seconds());
            ASSERTV(NameOf<REP>().name(), ns == Z.nanoseconds());

            Obj *mR = &(mX.addDuration(value));
            mZ.addInterval(static_cast<Int64>(k_SECONDS.count()),
                           static_cast<int>(k_NANOSECONDS.count()));

            ASSERTV(mR == &mX);

            ASSERTV(NameOf<REP>().name(), Z.seconds(), Z.nanoseconds(),
                                          X.seconds(), X.nanoseconds(),
                    Z == X);
        }
    }
}

template <class REP, class PERIOD>
void testChronoInteraction()
    // Iterate through several 'std::chrono::duration<REP, PERIOD>' objects
    // (both positive or negative) and call functions that verify these objects
    // conversion to 'TimeInterval'.
{
    typedef std::chrono::duration<REP, PERIOD> Duration;

    const REP k_MAX_COUNT            = Duration::max().count();
    const REP k_POSITIVE_LIMIT_COUNT = PERIOD::num >= 1
                                     ? LLONG_MAX / PERIOD::num > k_MAX_COUNT
                                         ? k_MAX_COUNT
                                         : LLONG_MAX / PERIOD::num
                                     :  k_MAX_COUNT;
        // Any value of the duration type with a period less than a second can
        // be converted to 'TimeInterval' value.  But if the period is larger
        // or equal to the second ratio, we need to take either the duration's
        // maximum or the maximum possible value that can be converted to
        // 'TimeInterval', depending on which of them is smaller.

    const REP k_MIN_COUNT            = Duration::min().count();
    const REP k_NEGATIVE_LIMIT_COUNT = PERIOD::num >= 1
                                     ? LLONG_MIN / PERIOD::num < k_MIN_COUNT
                                         ? k_MIN_COUNT
                                         : LLONG_MIN / PERIOD::num
                                     :  k_MIN_COUNT;

    // Positive 'std::chrono::duration' values.

    Duration positiveValue(k_POSITIVE_LIMIT_COUNT);
    while (positiveValue.count() > 0) {
        testChronoConversion(positiveValue);
        testChronoAddition(positiveValue);
        positiveValue /= 100;
    }

    // Zero 'std::chrono::duration' value.

    Duration k_ZERO_VALUE(0);
    testChronoConversion(k_ZERO_VALUE);
    testChronoAddition(k_ZERO_VALUE);

    // Negative 'std::chrono::duration' values.

    Duration negativeValue(k_NEGATIVE_LIMIT_COUNT);
    while (negativeValue.count() < 0) {
        testChronoConversion(negativeValue);
        testChronoAddition(negativeValue);
        negativeValue /= 100;
    }
}
#endif  // BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS

} // close unnamed namespace

// ----------------------------------------------------------------------------
//                   GENERIC STREAMING OPERATOR SIMULATION
// ----------------------------------------------------------------------------

namespace testadl {

struct CustomStream {
    int d_counter;
};

template <class OBJECT_TYPE>
CustomStream& operator<<(CustomStream& stream, const OBJECT_TYPE& /* object */)
    // Increment the internal counter of the specified 'stream', and return a
    // reference providing modifiable access to the 'stream'.
{
    ++stream.d_counter;

    return stream;
}

template <class OBJECT_TYPE>
std::ostream& operator<<(std::ostream&      stream,
                         const OBJECT_TYPE& /* object */)
    // Return a reference providing modifiable access to the specified
    // 'stream'.
{
    return stream;
}

struct UserType {
};

std::ostream& operator<<(std::ostream&   stream,
                         const UserType& /* object */)
    // Return a reference providing modifiable access to the specified
    // 'stream'.
{
    return stream;
}

}  // close namespace testadl


// ----------------------------------------------------------------------------
//                           BSLS_TESTUTIL SUPPORT
// ----------------------------------------------------------------------------


namespace BloombergLP {
namespace bsls {

void debugprint(const bsls::TimeInterval& timeInterval)
    // Write the specified 'timeInterval' to the console.
{
    printf("(%lld, %d)", timeInterval.seconds(), timeInterval.nanoseconds());
}

}  // close package namespace
}  // close enterprise namespace

#ifdef BSLS_PLATFORM_CMP_MSVC
// Microsoft Visual Studio annoyingly doesn't (yet) implement 'snprintf'.  The
// following will do for the purposes of 'TestStream'; note however the
// error behavior for '_snprintf' is different from 'snprintf'.

#define snprintf _snprintf
#endif

                        // ===========================
                        // class TestInStreamException
                        // ===========================

class TestInStreamException {
    // This class defines an exception object for unexternalization operations.
    // Instances of this class contain information about an unexternalization
    // request.

    // DATA
    int d_dataType;  // type of the input data requested

  public:
    // CREATORS
    explicit TestInStreamException(int type);
        // Create an exception object initialized with the specified 'type'.

    //! ~TestInStreamException() = default;
        // Destroy this object.  Note that this method's definition is compiler
        // generated.

    // ACCESSORS
    int dataType() const;
        // Return the type code that was supplied at construction of this
        // exception object.
};
// CREATORS
inline
TestInStreamException::TestInStreamException(int type)
: d_dataType(type)
{
}

// ACCESSORS
inline
int TestInStreamException::dataType() const
{
    return d_dataType;
}

               // ============================================
               // macro BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN
               // ============================================

#ifdef BDE_BUILD_TARGET_EXC

class TestInStream_ProxyBase {
    // This class provides a common base class for the parameterized
    // 'TestInStream_Proxy' class (below).  Note that the 'virtual'
    // 'setInputLimit' method, although a "setter", *must* be declared 'const'.

  public:
    virtual ~TestInStream_ProxyBase()
    {
    }

    // ACCESSORS
    virtual void setInputLimit(int limit) const = 0;
};

template <class BSLX_STREAM_TYPE>
class TestInStream_Proxy: public TestInStream_ProxyBase {
    // This class provides a proxy to the test stream that is supplied to the
    // 'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' macro.  This proxy may be
    // instantiated with 'TestInStream', or with a type that supports the same
    // interface as 'TestInStream'.

    // DATA
    BSLX_STREAM_TYPE *d_stream_p;  // stream used in '*_BEGIN' and
                                   // '*_END' macros (held, not owned)

  public:
    // CREATORS
    TestInStream_Proxy(BSLX_STREAM_TYPE *stream)
    : d_stream_p(stream)
    {
    }

    ~TestInStream_Proxy()
    {
    }

    // ACCESSORS
    virtual void setInputLimit(int limit) const
    {
        d_stream_p->setInputLimit(limit);
    }
};

template <class BSLX_STREAM_TYPE>
inline
TestInStream_Proxy<BSLX_STREAM_TYPE>
TestInStream_getProxy(BSLX_STREAM_TYPE *stream)
    // Return, by value, a test stream proxy for the specified parameterized
    // 'stream'.
{
    return TestInStream_Proxy<BSLX_STREAM_TYPE>(stream);
}

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(BSLX_TESTINSTREAM)             \
{                                                                             \
    const TestInStream_ProxyBase& testInStream =                              \
                             TestInStream_getProxy(&BSLX_TESTINSTREAM);       \
    {                                                                         \
        static int firstTime = 1;                                             \
        if (veryVerbose && firstTime)                                         \
                printf("### BSLX EXCEPTION TEST -- (ENABLED) --\n");          \
        firstTime = 0;                                                        \
    }                                                                         \
    if (veryVeryVerbose) printf("### Begin BSLX exception test.\n");          \
    int bslxExceptionCounter = 0;                                             \
    static int bslxExceptionLimit = 100;                                      \
    testInStream.setInputLimit(bslxExceptionCounter);                         \
    do {                                                                      \
        try {
#endif  // BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN

#else // !defined(BDE_BUILD_TARGET_EXC)

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(testInStream)                  \
{                                                                             \
    static int firstTime = 1;                                                 \
    if (verbose && firstTime) {                                               \
        printf("### BSLX EXCEPTION TEST -- (NOT ENABLED) --\n");              \
        firstTime = 0;                                                        \
    }                                                                         \
}
#endif  // BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN

#endif  // BDE_BUILD_TARGET_EXC

               // ==========================================
               // macro BSLX_TESTINSTREAM_EXCEPTION_TEST_END
               // ==========================================

#ifdef BDE_BUILD_TARGET_EXC

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_END
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_END                                  \
        } catch (TestInStreamException& e) {                                  \
            if ((veryVerbose && bslxExceptionLimit) || veryVeryVerbose)       \
            {                                                                 \
                --bslxExceptionLimit;                                         \
                printf("(%i)", bslxExceptionCounter);                         \
                if (veryVeryVerbose) {                                        \
                    printf(" BSLX EXCEPTION: input limit = %i,"               \
                           "last data type = %i",                             \
                           bslxExceptionCounter,                              \
                           e.dataType());                                     \
                }                                                             \
                else if (0 == bslxExceptionLimit) {                           \
                    printf(" [ Note: 'bslxExceptionLimit' reached. ]");       \
                }                                                             \
                printf("\n");                                                 \
            }                                                                 \
            testInStream.setInputLimit(++bslxExceptionCounter);               \
            continue;                                                         \
        }                                                                     \
        testInStream.setInputLimit(-1);                                       \
        break;                                                                \
    } while (1);                                                              \
    if (veryVeryVerbose) {                                                    \
        printf("### End BSLX exception test.\n");                             \
    }                                                                         \
}
#endif  // BSLX_TESTINSTREAM_EXCEPTION_TEST_END

#else // !defined(BDE_BUILD_TARGET_EXC)

#ifndef BSLX_TESTINSTREAM_EXCEPTION_TEST_END
#define BSLX_TESTINSTREAM_EXCEPTION_TEST_END
#endif

#endif  // BDE_BUILD_TARGET_EXC

                         // ==================
                         // class ByteInStream
                         // ==================

class ByteInStream {
    // This class provides input methods to unexternalize 'bsls::Types::Int64'
    // and 'int' values from their byte representations.

    // DATA
    const char *d_buffer;      // bytes to be unexternalized

    int         d_numBytes;    // number of bytes in 'd_buffer'

    bool        d_validFlag;   // stream validity flag; 'true' if stream is in
                               // valid state, 'false' otherwise

    int         d_inputLimit;  // number of input op's before exception

    int         d_cursor;      // index of the next byte to be extracted from
                               // this stream

  private:
    // PRIVATE MANIPULATORS
    void throwExceptionIfInputLimitExhausted(int code);
        // Decrement the internal input limit of this test stream.  If the
        // input limit becomes negative and exception-handling is enabled
        // (i.e., '-DBDE_BUILD_TARGET_EXC' was supplied at compile time), then
        // throw a 'TestInStreamException' object initialized with the
        // specified type 'code'.  If exception-handling is not enabled, this
        // method has no effect.

    // NOT IMPLEMENTED
    ByteInStream(const ByteInStream&);
    ByteInStream& operator=(const ByteInStream&);

  public:
    // CREATORS
    ByteInStream(const char *buffer, int numBytes);
        // Create an input byte stream containing the specified initial
        // 'numBytes' from the specified 'buffer'.  The behavior is undefined
        // unless '0 <= numBytes' and, if '0 == buffer', then '0 == numBytes'.

    ~ByteInStream();
        // Destroy this object.

    // MANIPULATORS
    ByteInStream& getInt64(bsls::Types::Int64& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the
        // 64-bit signed integer value into the specified 'variable', update
        // the cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.

    ByteInStream& getInt32(int& variable);
        // If required, throw a 'TestInStreamException' (see
        // 'throwExceptionIfInputLimitExhausted'); otherwise, consume the
        // 32-bit signed integer value into the specified 'variable', update
        // the cursor location, and return a reference to this stream.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.

    void invalidate();
        // Put this input stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a value extracted from this stream is
        // determined to be invalid, inconsistent, or otherwise incorrect.

    void setInputLimit(int limit);
        // Set the number of input operations allowed on this stream to the
        // specified 'limit' before an exception is thrown.  If 'limit' is less
        // than 0, no exception is to be thrown.  By default, no exception is
        // scheduled.

    void reset();
        // Set the index of the next byte to be extracted from this stream to 0
        // (i.e., the beginning of the stream) and validate this stream if it
        // is currently invalid.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an input operation was
        // detected to have failed.

    int cursor() const;
        // Return the index of the next byte to be extracted from this stream.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable external memory
        // buffer of this stream.  The behavior of accessing elements outside
        // the range '[ data() .. data() + (length() - 1) ]' is undefined.

    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false' otherwise.  An
        // invalid stream is a stream in which insufficient or invalid data was
        // detected during an extraction operation.  Note that an empty stream
        // will be valid unless an extraction attempt or explicit invalidation
        // causes it to be otherwise.

    bool isEmpty() const;
        // Return 'true' if this stream is empty, and 'false' otherwise.  Note
        // that this function enables higher-level components to verify that,
        // after successfully reading all expected data, no data remains.

    int length() const;
        // Return the total number of bytes stored in the external memory
        // buffer.
};

// PRIVATE MANIPULATORS
inline
void ByteInStream::throwExceptionIfInputLimitExhausted(int code)
{
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw TestInStreamException(code);
        }
    }
#endif
}

// CREATORS
inline
ByteInStream::ByteInStream(const char *buffer, int numBytes)
: d_buffer(buffer)
, d_numBytes(numBytes)
, d_validFlag(1)
, d_inputLimit(-1)
, d_cursor(0)
{
    BSLS_ASSERT(buffer || 0 == numBytes);
    BSLS_ASSERT(0 <= numBytes);
}

inline
ByteInStream::~ByteInStream()
{
}

// MANIPULATORS
inline
ByteInStream& ByteInStream::getInt64(bsls::Types::Int64& variable)
{
    throwExceptionIfInputLimitExhausted(64);

    if (!isValid()) {
        return *this;                                                 // RETURN
    }

    if (cursor() + k_BDEX_SIZEOF_INT64 <= length()) {
        const char *buffer = d_buffer + cursor();

        if (sizeof variable > k_BDEX_SIZEOF_INT64) {
            variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
        }

        char *bytes = reinterpret_cast<char *>(&variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
        bytes[7] = buffer[0];
        bytes[6] = buffer[1];
        bytes[5] = buffer[2];
        bytes[4] = buffer[3];
        bytes[3] = buffer[4];
        bytes[2] = buffer[5];
        bytes[1] = buffer[6];
        bytes[0] = buffer[7];
#else
        memcpy(bytes, buffer, 8);
#endif

        d_cursor += k_BDEX_SIZEOF_INT64;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt32(int& variable)
{
    throwExceptionIfInputLimitExhausted(32);

    if (!isValid()) {
        return *this;                                                 // RETURN
    }

    if (cursor() + k_BDEX_SIZEOF_INT32 <= length()) {
        const char *buffer = d_buffer + cursor();

        if (sizeof variable > k_BDEX_SIZEOF_INT32) {
            variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
        }

        char *bytes = reinterpret_cast<char *>(&variable);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
        bytes[3] = buffer[0];
        bytes[2] = buffer[1];
        bytes[1] = buffer[2];
        bytes[0] = buffer[3];
#else
        memcpy(bytes, buffer, 4);
#endif

        d_cursor += k_BDEX_SIZEOF_INT32;
    }
    else {
        invalidate();
    }

    return *this;
}

inline
void ByteInStream::invalidate()
{
    d_validFlag = false;
}

inline
void ByteInStream::setInputLimit(int limit)
{
    d_inputLimit = limit;
}

inline
void ByteInStream::reset()
{
    d_validFlag = true;
    d_cursor    = 0;
}

// ACCESSORS
inline
ByteInStream::operator const void *() const
{
    return isValid() ? this : 0;
}

inline
int ByteInStream::cursor() const
{
    return d_cursor;
}

inline
const char *ByteInStream::data() const
{
    return d_numBytes ? d_buffer : 0;
}

inline
bool ByteInStream::isValid() const
{
    return d_validFlag;
}

inline
bool ByteInStream::isEmpty() const
{
    return cursor() == length();
}

inline
int ByteInStream::length() const
{
    return d_numBytes;
}

typedef ByteInStream  In;

                         // ===================
                         // class ByteOutStream
                         // ===================

class ByteOutStream {
    // This class provides output methods to externalize 'bsls::Types::Int64'
    // and 'int' values from their byte representations.  In particular, each
    // 'put' method of this class is guaranteed to write stream data that can
    // be read by the corresponding 'get' method of 'ByteInStream'.

    // DATA
    char            d_buffer[BUFFER_SIZE];  // byte buffer to write to

    int             d_size;         // size of data written to buffer

    int             d_validFlag;    // stream validity flag; 'true' if stream
                                    // is in valid state, 'false' otherwise

    // NOT IMPLEMENTED
    ByteOutStream(const ByteOutStream&);
    ByteOutStream& operator=(const ByteOutStream&);

  private:
    // PRIVATE MANIPULATORS
    void validate();
        // Put this output stream into a valid state.  This function has no
        // effect if this stream is already valid.

  public:
    // CREATORS
    explicit ByteOutStream(int serializationVersion);
        // Create an empty output byte stream.  The specified
        // 'serializationVersion' is ignored.

    ~ByteOutStream();
        // Destroy this object.

    // MANIPULATORS
    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.

    ByteOutStream& putInt64(bsls::Types::Int64 value);
        // Write to this stream the eight-byte, two's complement integer (in
        // network byte order) comprised of the least-significant eight bytes
        // of the specified 'value' (in host byte order), and return a
        // reference to this stream.  If this stream is initially invalid, this
        // operation has no effect.

    ByteOutStream& putInt32(int value);
        // Write to this stream the four-byte, two's complement integer (in
        // network byte order) comprised of the least-significant four bytes of
        // the specified 'value' (in host byte order), and return a reference
        // to this stream.  If this stream is initially invalid, this operation
        // has no effect.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed or 'invalidate' was called.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this stream is not destroyed or modified.  The behavior of accessing
        // elements outside the range '[ data() .. data() + (length() - 1) ]'
        // is undefined.

    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false' otherwise.  An
        // invalid stream is a stream for which an output operation was
        // detected to have failed or 'invalidate' was called.

    int length() const;
        // Return the number of bytes in this stream.
};

// PRIVATE MANIPULATORS
inline
void ByteOutStream::validate()
{
    d_validFlag = true;
}

// CREATORS
inline
ByteOutStream::ByteOutStream(int /* serializationVersion */)
: d_size(0)
, d_validFlag(true)
{
}

inline
ByteOutStream::~ByteOutStream()
{
}

// MANIPULATORS
inline
void ByteOutStream::invalidate()
{
    d_validFlag = false;
}
inline
ByteOutStream& ByteOutStream::putInt64(bsls::Types::Int64 value)
{
    if (!isValid()) {
        return *this;                                                 // RETURN
    }

    // Write to the buffer the specified 'value'.

    char *buffer = d_buffer + d_size;

    char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[7];
    buffer[1] = bytes[6];
    buffer[2] = bytes[5];
    buffer[3] = bytes[4];
    buffer[4] = bytes[3];
    buffer[5] = bytes[2];
    buffer[6] = bytes[1];
    buffer[7] = bytes[0];
#else
    buffer[0] = bytes[sizeof value - 8];
    buffer[1] = bytes[sizeof value - 7];
    buffer[2] = bytes[sizeof value - 6];
    buffer[3] = bytes[sizeof value - 5];
    buffer[4] = bytes[sizeof value - 4];
    buffer[5] = bytes[sizeof value - 3];
    buffer[6] = bytes[sizeof value - 2];
    buffer[7] = bytes[sizeof value - 1];
#endif

    d_size += k_BDEX_SIZEOF_INT64;

    return *this;
}

inline
ByteOutStream& ByteOutStream::putInt32(int value)
{
    if (!isValid()) {
        return *this;                                                 // RETURN
    }

    // Write to the buffer the specified 'value'.

    char *buffer = d_buffer + d_size;

    char *bytes = reinterpret_cast<char *>(&value);

#if BSLS_PLATFORM_IS_LITTLE_ENDIAN
    buffer[0] = bytes[3];
    buffer[1] = bytes[2];
    buffer[2] = bytes[1];
    buffer[3] = bytes[0];
#else
    buffer[0] = bytes[sizeof value - 4];
    buffer[1] = bytes[sizeof value - 3];
    buffer[2] = bytes[sizeof value - 2];
    buffer[3] = bytes[sizeof value - 1];
#endif

    d_size += k_BDEX_SIZEOF_INT32;

    return *this;
}

// ACCESSORS
inline
ByteOutStream::operator const void *() const
{
    return isValid() ? this : 0;
}

inline
const char *ByteOutStream::data() const
{
    return d_buffer;
}

inline
bool ByteOutStream::isValid() const
{
    return d_validFlag;
}

inline
int ByteOutStream::length() const
{
    return static_cast<int>(d_size);
}

typedef ByteOutStream Out;

//=============================================================================
//                              FUZZ TESTING
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The following function, 'LLVMFuzzerTestOneInput', is the entry point for the
// clang fuzz testing facility.  See {http://bburl/BDEFuzzTesting} for details
// on how to build and run with fuzz testing enabled.
//-----------------------------------------------------------------------------

#ifdef BDE_ACTIVATE_FUZZ_TESTING
#define main test_driver_main
#endif

extern "C"
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
    // Use the specified 'data' array of 'size' bytes as input to methods of
    // this component and return zero.
{
    size_t      LENGTH = size;
    int         test;

    if (data && LENGTH) {
        test = 1 + static_cast<unsigned char>(*data) % 99;
        ++data;
        --LENGTH;
    }
    else {
        test = 0;
    }

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // FUZZ TESTING 'TimeInterval::addInterval'
        //
        // Concerns:
        //: 1 That in-contract invocation of 'addInterval' with an 'Int64' and
        //:   an integer created from fuzz data succeeds.
        //
        // Plan:
        //: 1 Create two 'Int64' values from fuzz data.
        //:
        //: 2 Create two 'int' values from fuzz data.
        //:
        //: 3 Create a 'TimeInterval' object.
        //:
        //: 4 Invoke 'setInterval' on the 'TimeInterval' object with one pair
        //:   of 'Int64' and 'int' values inside the 'BSLS_FUZZTEST_EVALUATE'
        //:   macro.
        //:
        //: 5 Invoke 'addInterval' on the 'TimeInterval' object with the other
        //:   pair of 'Int64' and 'int' values inside the
        //:   'BSLS_FUZZTEST_EVALUATE' macro.
        //
        // Testing:
        //   TimeInterval::addInterval(bsls::Types::Int64 secs, int nanosecs)
        // --------------------------------------------------------------------

        size_t minLength = 2 * (sizeof(bsls::Types::Int64) + sizeof(int));

        if (LENGTH < minLength) {
            return 0;                                                 // RETURN
        }

        bsls::Types::Int64  seconds1, seconds2;
        int                 nanoseconds1, nanoseconds2;

        memcpy(&seconds1, data, sizeof(bsls::Types::Int64));
        data += sizeof(bsls::Types::Int64);

        memcpy(&seconds2, data, sizeof(bsls::Types::Int64));
        data += sizeof(bsls::Types::Int64);

        memcpy(&nanoseconds1, data, sizeof(int));
        data += sizeof(int);

        memcpy(&nanoseconds2, data, sizeof(int));
        data += sizeof(int);

        {
            bsls::FuzzTestHandlerGuard hG;

            Obj ti;

            BSLS_FUZZTEST_EVALUATE(ti.setInterval(seconds1, nanoseconds1));
            BSLS_FUZZTEST_EVALUATE(ti.addInterval(seconds2, nanoseconds2));
        }
      } break;
      default: {
      } break;
    }

    if (testStatus > 0) {
        BSLS_ASSERT_INVOKE("FUZZ TEST FAILURES");
    }

    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 29: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        {
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Modifying a 'bsls::TimeInterval'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create and manipulate a
// 'bsls::TimeInterval' object.
//
// First, we default construct a 'TimeInterval' object, 'interval':
//..
    bsls::TimeInterval interval;
//
    ASSERT(0 == interval.seconds());
    ASSERT(0 == interval.nanoseconds());
//..
// Next, we set the value of 'interval' to 1 second and 10 nanoseconds (a
// time interval of 1000000010 nanoseconds):
//..
    interval.setInterval(1, 10);
//
    ASSERT( 1 == interval.seconds());
    ASSERT(10 == interval.nanoseconds());
//..
// Then, we add 3 seconds to 'interval':
//..
    interval.addInterval(3, 0);
//
    ASSERT( 4 == interval.seconds());
    ASSERT(10 == interval.nanoseconds());
//..
// Next, we create a copy of 'interval', 'intervalPrime':
//..
    bsls::TimeInterval intervalPrime(interval);
//
    ASSERT(intervalPrime == interval);
//..
// Finally, we assign 3.14 seconds to 'intervalPrime', and then add 2.73
// seconds more:
//..
    intervalPrime =  3.14;
    intervalPrime += 2.73;
//
    ASSERT(        5 == intervalPrime.seconds());
    ASSERT(870000000 == intervalPrime.nanoseconds());
//..

        }
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING TIME INTERVAL USER-DEFINED LITERALS
        //
        // Concerns:
        //: 1 Call of user-defined literal operators are properly forwarded to
        //:   the appropriate implementation.
        //:
        //: 2 Any valid value is properly distributed among 'TimeInterval'
        //:   fields (the number of stored nanoseconds does not exceed
        //:   documented limits).
        //:
        //: 3 That an access to operators can be gained using either
        //    'bsl::literals', 'bsl::TimeIntervalLiterals' or
        //:   'bsl::literals::TimeIntervalLiterals' namespaces.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a set of representative values using user-defined literal
        //:   operators and compare each of them against the expected value.
        //:   (C-1..2)
        //:
        //: 2 Use different using-directives and create values using
        //:   user-defined literal operators.  (C-3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   TimeInterval operator ""_h(  unsigned long long int hours);
        //   TimeInterval operator ""_min(unsigned long long int minutes);
        //   TimeInterval operator ""_s(  unsigned long long int seconds);
        //   TimeInterval operator ""_ms( unsigned long long int milliseconds);
        //   TimeInterval operator ""_us( unsigned long long int microseconds);
        //   TimeInterval operator ""_ns( unsigned long long int nanoseconds);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TIME INTERVAL USER-DEFINED LITERALS"
                            "\n===========================================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)

        if (verbose) printf("\tTesting basic behavior.\n");
        {
            using namespace bsls;
            using namespace TimeIntervalLiterals;

            struct {
                int          d_line;
                TimeInterval d_literalValue;
                TimeInterval d_expected;
            } DATA[] = {
                // LINE LITERAL           EXPECTED    ( SECONDS   NANOSECONDS )
                //----- ----------------- -------------------------------------
                // hours
                { L_,                0_h, TimeInterval(          0,        0)},
                { L_,                1_h, TimeInterval(   1 * 3600,        0)},
                { L_,               10_h, TimeInterval(  10 * 3600,        0)},
                { L_,              100_h, TimeInterval( 100 * 3600,        0)},
                // minutes
                { L_,              0_min, TimeInterval(        0,          0)},
                { L_,              1_min, TimeInterval(   1 * 60,          0)},
                { L_,             10_min, TimeInterval(  10 * 60,          0)},
                { L_,            100_min, TimeInterval( 100 * 60,          0)},
                // seconds
                { L_,                0_s, TimeInterval(   0,               0)},
                { L_,                1_s, TimeInterval(   1,               0)},
                { L_,               10_s, TimeInterval(  10,               0)},
                { L_,              100_s, TimeInterval( 100,               0)},
                // milliseconds
                { L_,               0_ms, TimeInterval(   0,               0)},
                { L_,               1_ms, TimeInterval(   0,         1000000)},
                { L_,              10_ms, TimeInterval(   0,        10000000)},
                { L_,             100_ms, TimeInterval(   0,       100000000)},
                { L_,            1000_ms, TimeInterval(   1,               0)},
                { L_,            1100_ms, TimeInterval(   1,       100000000)},
                { L_,            1110_ms, TimeInterval(   1,       110000000)},
                { L_,            1111_ms, TimeInterval(   1,       111000000)},
                { L_,           10000_ms, TimeInterval(  10,               0)},
                { L_,          100000_ms, TimeInterval( 100,               0)},
                // microseconds
                { L_,               0_us, TimeInterval(   0,               0)},
                { L_,               1_us, TimeInterval(   0,            1000)},
                { L_,              10_us, TimeInterval(   0,           10000)},
                { L_,             100_us, TimeInterval(   0,          100000)},
                { L_,            1000_us, TimeInterval(   0,         1000000)},
                { L_,           10000_us, TimeInterval(   0,        10000000)},
                { L_,          100000_us, TimeInterval(   0,       100000000)},
                { L_,         1000000_us, TimeInterval(   1,               0)},
                { L_,         1100000_us, TimeInterval(   1,       100000000)},
                { L_,         1110000_us, TimeInterval(   1,       110000000)},
                { L_,         1111000_us, TimeInterval(   1,       111000000)},
                { L_,         1111100_us, TimeInterval(   1,       111100000)},
                { L_,         1111110_us, TimeInterval(   1,       111110000)},
                { L_,         1111111_us, TimeInterval(   1,       111111000)},
                { L_,        10000000_us, TimeInterval(  10,               0)},
                { L_,       100000000_us, TimeInterval( 100,               0)},
                // nanoseconds
                { L_,               0_ns, TimeInterval(   0,               0)},
                { L_,               1_ns, TimeInterval(   0,               1)},
                { L_,              10_ns, TimeInterval(   0,              10)},
                { L_,             100_ns, TimeInterval(   0,             100)},
                { L_,            1000_ns, TimeInterval(   0,            1000)},
                { L_,           10000_ns, TimeInterval(   0,           10000)},
                { L_,          100000_ns, TimeInterval(   0,          100000)},
                { L_,         1000000_ns, TimeInterval(   0,         1000000)},
                { L_,        10000000_ns, TimeInterval(   0,        10000000)},
                { L_,       100000000_ns, TimeInterval(   0,       100000000)},
                { L_,      1000000000_ns, TimeInterval(   1,               0)},
                { L_,      1100000000_ns, TimeInterval(   1,       100000000)},
                { L_,      1110000000_ns, TimeInterval(   1,       110000000)},
                { L_,      1111000000_ns, TimeInterval(   1,       111000000)},
                { L_,      1111100000_ns, TimeInterval(   1,       111100000)},
                { L_,      1111110000_ns, TimeInterval(   1,       111110000)},
                { L_,      1111111000_ns, TimeInterval(   1,       111111000)},
                { L_,      1111111100_ns, TimeInterval(   1,       111111100)},
                { L_,      1111111110_ns, TimeInterval(   1,       111111110)},
                { L_,      1111111111_ns, TimeInterval(   1,       111111111)},
                { L_,     10000000000_ns, TimeInterval(  10,               0)},
                { L_,    100000000000_ns, TimeInterval( 100,               0)},
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE     = DATA[ti].d_line;
                const TimeInterval& X        = DATA[ti].d_literalValue;
                const TimeInterval& EXPECTED = DATA[ti].d_expected;

                int nanoseconds = X.nanoseconds();

                ASSERTV(LINE, X, EXPECTED,  EXPECTED           == X          );
                ASSERTV(LINE, nanoseconds, -k_NANOSECS_PER_SEC <  nanoseconds);
                ASSERTV(LINE, nanoseconds,  k_NANOSECS_PER_SEC >  nanoseconds);
            }
        }

        if (verbose) printf("\tTesting that namespace is inline.\n");
        {
            {
                using namespace bsls::literals;
                Obj mXH   = 1_h;
                Obj mXMin = 1_min;
                Obj mXS   = 1_s;
                Obj mXMs  = 1_ms;
                Obj mXUs  = 1_us;
                Obj mXNs  = 1_ns;

                (void) mXH;
                (void) mXMin;
                (void) mXS;
                (void) mXMs;
                (void) mXUs;
                (void) mXNs;
            }
            {
                using namespace bsls::TimeIntervalLiterals;
                Obj mXH   = 1_h;
                Obj mXMin = 1_min;
                Obj mXS   = 1_s;
                Obj mXMs  = 1_ms;
                Obj mXUs  = 1_us;
                Obj mXNs  = 1_ns;

                (void) mXH;
                (void) mXMin;
                (void) mXS;
                (void) mXMs;
                (void) mXUs;
                (void) mXNs;
            }
            {
                using namespace bsls::literals::TimeIntervalLiterals;
                Obj mXH   = 1_h;
                Obj mXMin = 1_min;
                Obj mXS   = 1_s;
                Obj mXMs  = 1_ms;
                Obj mXUs  = 1_us;
                Obj mXNs  = 1_ns;

                (void) mXH;
                (void) mXMin;
                (void) mXS;
                (void) mXMs;
                (void) mXUs;
                (void) mXNs;
            }
        }

        if (verbose) printf("\tUsage example.\n");
        {
            using namespace bsls::TimeIntervalLiterals;

            bsls::TimeInterval i0 = 10_h;
            ASSERT(36000   == i0.seconds()    );
            ASSERT(0       == i0.nanoseconds());

            bsls::TimeInterval i1 = 10001_ms;
            ASSERT(10      == i1.seconds()    );
            ASSERT(1000000 == i1.nanoseconds());

            bsls::TimeInterval i2 = 100_ns;
            ASSERT(0       == i2.seconds()    );
            ASSERT(100     == i2.nanoseconds());
        }

        if (verbose) printf("\tNegative Testing.\n");
        {
            using namespace bsls::TimeIntervalLiterals;

            bsls::AssertTestHandlerGuard hG;

            ASSERT_PASS(2562047788015215_h);
            ASSERT_FAIL(2562047788015216_h);

            ASSERT_PASS(153722867280912930_min);
            ASSERT_FAIL(153722867280912931_min);

            ASSERT_PASS(9223372036854775806_s);
            ASSERT_FAIL(9223372036854775807_s);
        }
#else
        if (verbose) printf("\tUser-defined literals or inline namespaces are"
                            " not supported.\n");
#endif  // BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE &&
        // BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION FROM/TO STD::CHRONO
        //  To avoid excessive duplication of test machinery, all methods and
        //  operators accepting 'std::chrono::duration' are checked in one
        //  test case.  See descriptions of
        //    - 'testChronoConversion'
        //    - 'testChronoAddition'
        //  for specific concerns and plans.
        //
        //  At the same time, checks for undefined behavior for each function
        //  are gathered together at the end of this test case.
        //
        // Concerns:
        //: 1 Any 'std::chrono::duration' object can be successfully converted
        //:   to 'TimeInterval' and each function accepting such object behaves
        //:   the same way as corresponding function accepting 'TimeInterval'
        //:   object with the same value.
        //:
        //: 2 'std::chrono::duration' literals are supported.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Pass several different values to methods that accept
        //:   'std::chrono::duration' object and compare the result with the
        //:   result of corresponding function accepting 'TimeInterval' object.
        //:   (C-1)
        //:
        //: 2 Pass several different 'std::chrono::duration' literals to
        //:   methods that accept 'std::chrono::duration' object and verify the
        //:   results. (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid 'std::chrono::duration' values, but not
        //:   triggered for adjacent valid ones (using the 'BSLS_ASSERTTEST_*'
        //:   macros).  (C-3)
        //
        // Testing:
        //   TimeInterval(const std::chrono::duration<REP, PERIOD>& duration);
        //   DURATION_TYPE asDuration() const;
        //   TimeInterval& addDuration(const chrono::duration<REP, PERIOD>& d);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERSION FROM/TO STD::CHRONO"
                            "\n======================================\n");

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS

        testChronoInteraction<std::int8_t, std::ratio<3600, 1> >();  // hours
        testChronoInteraction<std::int8_t, std::ratio<60, 1> >();    // minutes
        testChronoInteraction<std::int8_t, std::ratio<1> >();        // seconds
        testChronoInteraction<std::int8_t, std::milli>();            // milli
        testChronoInteraction<std::int8_t, std::micro>();            // micro
        testChronoInteraction<std::int8_t, std::nano >();            // nano
        testChronoInteraction<std::int8_t, std::pico >();            // pico

        testChronoInteraction<std::int16_t, std::ratio<3600, 1> >();
        testChronoInteraction<std::int16_t, std::ratio<60, 1> >();
        testChronoInteraction<std::int16_t, std::ratio<1> >();
        testChronoInteraction<std::int16_t, std::milli>();
        testChronoInteraction<std::int16_t, std::micro>();
        testChronoInteraction<std::int16_t, std::nano >();
        testChronoInteraction<std::int16_t, std::pico >();

        testChronoInteraction<std::int32_t, std::ratio<3600, 1> >();
        testChronoInteraction<std::int32_t, std::ratio<60, 1> >();
        testChronoInteraction<std::int32_t, std::ratio<1> >();
        testChronoInteraction<std::int32_t, std::milli>();
        testChronoInteraction<std::int32_t, std::micro>();
        testChronoInteraction<std::int32_t, std::nano >();
        testChronoInteraction<std::int32_t, std::pico >();

        testChronoInteraction<std::int64_t, std::ratio<3600, 1> >();
        testChronoInteraction<std::int64_t, std::ratio<60, 1> >();
        testChronoInteraction<std::int64_t, std::ratio<1> >();
        testChronoInteraction<std::int64_t, std::milli>();
        testChronoInteraction<std::int64_t, std::micro>();
        testChronoInteraction<std::int64_t, std::nano >();
        testChronoInteraction<std::int64_t, std::pico >();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
        if (verbose) printf("\tTesting 'std::chrono:duration' literals.\n");
        {
            using namespace std::chrono_literals;

            Obj        mX(5s);
            const Obj& X = mX;

            ASSERTV(X.seconds(),     5       == X.seconds()    );
            ASSERTV(X.nanoseconds(), 0       == X.nanoseconds());

            mX.addDuration(1000ns);

            ASSERTV(X.seconds(),     5       == X.seconds()    );
            ASSERTV(X.nanoseconds(), 1000    == X.nanoseconds());

            mX = mX + 5000us;

            ASSERTV(X.seconds(),     5       == X.seconds()    );
            ASSERTV(X.nanoseconds(), 5001000 == X.nanoseconds());

            mX = mX + 5000ms;

            ASSERTV(X.seconds(),     10      == X.seconds()    );
            ASSERTV(X.nanoseconds(), 5001000 == X.nanoseconds());

        }
#else
        if (verbose) printf(
                     "\t'std::chrono:duration' literals are not supported.\n");
#endif

        if (verbose) printf("\tNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            const std::chrono::duration<long long int>
                                                 DURATION_LLONG_MIN(LLONG_MIN);
            const std::chrono::duration<long long int>
                                                 DURATION_LLONG_MAX(LLONG_MAX);

            const std::chrono::duration<long long int> VALID_MIN =
                                                            DURATION_LLONG_MIN;
            const std::chrono::duration<long long int> MIN_HALF =
                                                        DURATION_LLONG_MIN / 2;

            // To get the invalid value, we need to find something less, than
            // 'LLONG_MIN'.  We don't have integer type with bigger capacity
            // for duration's representation, so we have to play with the
            // duration's period.  Let's take the minimum fitting value of
            // 'minutes - 15'.  The usual change by one is not enough in this
            // case, since we have to convert the values to 'long double' that
            // suffers from a lack of precision on such large numbers.  The
            // specific value is selected empirically.

            const std::chrono::duration<long long int, std::ratio<60, 1> >
                                              INVALID_MIN(LLONG_MIN / 60 - 15);

            const std::chrono::duration<long long int> VALID_MAX =
                                                            DURATION_LLONG_MAX;
            const std::chrono::duration<long long int> MAX_HALF =
                                                        DURATION_LLONG_MAX / 2;

            // Using the same approach as for the mininmal invalid value.

            const std::chrono::duration<long long int, std::ratio<60, 1> >
                                              INVALID_MAX(LLONG_MAX / 60 + 15);


            // Constructor.
            {
                ASSERT_PASS((Obj(  VALID_MIN)));
                ASSERT_FAIL((Obj(INVALID_MIN)));

                ASSERT_PASS((Obj(  VALID_MAX)));
                ASSERT_FAIL((Obj(INVALID_MAX)));
            }

            // Assignment.

            {
                Obj mX;

                ASSERT_PASS(mX =   VALID_MIN);
                ASSERT_FAIL(mX = INVALID_MIN);

                ASSERT_PASS(mX =   VALID_MAX);
                ASSERT_FAIL(mX = INVALID_MAX);
            }

            // 'addDuration'.

            {
                {
                    Obj mX;

                    ASSERT_PASS(mX.addDuration(  VALID_MIN));
                    ASSERT_FAIL(mX.addDuration(INVALID_MIN));
                }
                {
                    Obj mX(MIN_HALF);

                    ASSERT_PASS(mX.addDuration(MIN_HALF));
                    ASSERT_FAIL(mX.addDuration(VALID_MIN));
                }
                {
                    Obj mX;

                    ASSERT_PASS(mX.addDuration(  VALID_MAX));
                    ASSERT_FAIL(mX.addDuration(INVALID_MAX));
                }
                {
                    Obj mX(MAX_HALF);

                    ASSERT_PASS(mX.addDuration(MAX_HALF));
                    ASSERT_FAIL(mX.addDuration(VALID_MAX));
                }
            }

            // Addition/subtraction operators.

            {
                {
                    Obj mX;

                    ASSERT_PASS(mX +   VALID_MAX);
                    ASSERT_FAIL(mX + INVALID_MAX);
                }
                {
                    Obj mX(MAX_HALF);

                    ASSERT_PASS(mX + MAX_HALF);
                    ASSERT_FAIL(mX + VALID_MAX);
                }
                {
                    Obj mX;

                    ASSERT_PASS(mX -   VALID_MAX);
                    ASSERT_FAIL(mX - INVALID_MAX);
                }
                {
                    Obj mX(MIN_HALF);

                    ASSERT_PASS(mX - MAX_HALF);
                    ASSERT_FAIL(mX - VALID_MAX);
                }
            }

            // Comparison operators.

            {
                Obj        mX;
                const Obj& X = mX;

                ASSERT_PASS(X           == VALID_MIN  );
                ASSERT_FAIL(X           == INVALID_MIN);

                ASSERT_PASS(VALID_MIN   == X          );
                ASSERT_FAIL(INVALID_MIN == X          );

                ASSERT_PASS(X           != VALID_MIN  );
                ASSERT_FAIL(X           != INVALID_MIN);

                ASSERT_PASS(VALID_MIN   != X          );
                ASSERT_FAIL(INVALID_MIN != X          );

                ASSERT_PASS(X           >  VALID_MIN  );
                ASSERT_FAIL(X           >  INVALID_MIN);

                ASSERT_PASS(VALID_MIN   >  X          );
                ASSERT_FAIL(INVALID_MIN >  X          );

                ASSERT_PASS(X           >= VALID_MIN  );
                ASSERT_FAIL(X           >= INVALID_MIN);

                ASSERT_PASS(VALID_MIN   >= X          );
                ASSERT_FAIL(INVALID_MIN >= X          );

                ASSERT_PASS(X           <  VALID_MIN  );
                ASSERT_FAIL(X           <  INVALID_MIN);

                ASSERT_PASS(VALID_MIN   <  X          );
                ASSERT_FAIL(INVALID_MIN <  X          );

                ASSERT_PASS(X           <= VALID_MIN  );
                ASSERT_FAIL(X           <= INVALID_MIN);

                ASSERT_PASS(VALID_MIN   <= X          );
                ASSERT_FAIL(INVALID_MIN <= X          );
            }

            // Conversion to 'std::chrono::duration'.

            {
                const Obj VALID_SEC_OBJ(std::chrono::seconds::max());

                ASSERT_PASS(VALID_SEC_OBJ.asDuration<std::chrono::seconds>());
                ASSERT_FAIL(
                         VALID_SEC_OBJ.asDuration<std::chrono::nanoseconds>());
            }
        }
#else
        if (verbose) printf("'std::chrono' is not supported\n");
#endif
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING 'isInDurationRange'
        //
        // NOTE THAT this first implementation of 'chrono::duration'
        // interoperability does not support durations with floating point
        // representations, therefore we test integral types only.  Similarly,
        // there is no support for user defined arithmetic types (especially
        // those that would extend range beyond 'std::intmax_t'), so no tests
        // are present for those either.
        //
        // Concerns:
        //: 1 An 'isInDurationRange' instance returns 'true' for 'TimeInterval'
        //:   values that can be represented (even if not precisely) within the
        //:   boundaries of the 'std::chrono::duration<>' instance that
        //:   'isInDurationRange' has been parameterized with.
        //:
        //: 2 Certain platforms do not provide an arithmetic type that is
        //:   capable of representing a 64 bit signed integer without loss of
        //:   precision. On such platforms we want range check on all in-bounds
        //:   'TimeInterval' values to succeed, and verify that there *are*
        //:   out-of-bounds values (if such values exist in the range of the
        //:   'duration') for which range checking using 'isInDurationRange'
        //:   fails.  We do that test to ensure that 'isInDurationRange' does
        //:   not just report 'true' for any 'TimeInterval' value.
        //:
        //: 3 'isInDurationRange' operates as described in C-1 and C-2 for
        //:   'duration' types with all fundamental integral types as
        //:   representation. Therefore we will test C-1 and 2 concerns with
        //:   'durations' sporting 'char', 'short', 'int', 'long', 'long long'
        //:   representation types, as well as their 'unsigned' variants.
        //:
        //: 4 'isInDurationRange' operates as described in C-1 and C-2 with
        //:   reasonable and unusual 'duration' periods provided.  Note that
        //:   C-3 and C-4 combine, so we have a combinatorial "explosion" of
        //:   tests. We aim to verify that reasonable units (periods) work very
        //:   well.  We aim to verify that unusual unit, such as one third
        //:   nanosecond work reasonably well.
        //
        // Plan:
        //: 1 Use the table-based approach to specify a set of time intervals,
        //:   that are expected to be border values for 'std::chrono::duration'
        //:   types with different integer representations and intervals that
        //:   are that are expected to be inconvertible to these durations.
        //:
        //: 2 Iterate through several common and some specific ratios (such as
        //:   one third nanoseconds) and through integer types using them as
        //:   representation for duration types and verify whether the values
        //:   from P-1 can be converted.  (C-1..4)
        //
        // Testing:
        //   bool isInDurationRange() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'isInDurationRange'"
                            "\n===========================\n");

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS

        BSLS_KEYWORD_CONSTEXPR bool isRealLongDouble =
                                       sizeof( double) != sizeof( long double);
            // Some compilers (in particular MSVC) make 'long double' a synonym
            // for 'double'.  This leads to a loss of precision for large
            // values and we must take this into account in the testing
            // process.

        using third = std::ratio<1, 3>;  // third of a second ratio
        using half  = std::ratio<1, 2>;  // half of a second  ratio
        using one   = std::ratio<1>;     // one second        ratio
        using two   = std::ratio<2, 1>;  // two seconds       ratio

        // Although 'isInDurationRange' accepts seconds and nanoseconds as
        // parameters, we store 'TimeInterval' objects in the 'Data' structure
        // to reduce the length of the table rows.

        struct Data {
            int               d_line;      // line number

            Obj               d_interval;  // input value to range check

            RangeCheckResults d_expected;  // expected validation results per
                                           // representation type
        };

        // Test tables contain values of 'TimeInterval' objects and expected
        // results of the range check for these intervals (since
        // 'isInDurationRange' accepts seconds and nanoseconds as parameters).
        // We only use integral 'duration' representations.  For each such
        // representation type with a given period ('std::pico', for the table
        // below) we verify range-check results for boundary values that fit
        // into the 'duration' (maximum as 'k_*_VALID_MAX', minimum as
        // 'k_*_VALID_MIN) and the smallest out-of-range values (for the given
        // platform) that we expect to fail range check (k_*_INVALID_MAX' and
        // 'k_*_INVALID_MIN' respectively).
        //
        // So 'k_PICO_16_BOUNDARY_MAX'  - maximum positive value that can be
        //                              stored into a duration object with
        //                              'int16_t' representation and
        //                              'std::pico' ratio
        //    'k_PICO_16_BOUNDARY_MIN'  - minimum negative value that can be
        //                              stored into a duration object with
        //                              'int16_t' representation and
        //                              'std::pico' ratio
        //    'k_PICO_16_INVALID_MAX' - a too large out-of-boundary value
        //                              reasonably close to the
        //                              'k_PICO_16_BOUNDARY_MAX' value that can
        //                              not be stored into a duration object
        //                              with'int16_t' representation and
        //                              'std::pico' ratio
        //    'k_PICO_16_INVALID_MIN' - a too small out-of-boundary value
        //                              reasonably close to the
        //                              'k_PICO_16_BOUNDARY_MIN' value that can
        //                              not be stored into a duration object
        //                              with'int16_t' representation and
        //                              'std::pico' ratio

        const Obj               k_ZERO(0, 0);
        const RangeCheckResults k_ZERO_EXP{ true,   true,   true,  true };

        // The following table explains the values we choose for testing.  We
        // take maximum and minimum values that particular type can take,
        // consider them as the number of picoseconds and convert to the
        // seconds/nanoseconds values.  So, 32767 picoseconds (maximum value,
        // that can have 'std::chrono::duration' with 'int16_t' representation
        // and 'std::pico' period) turn into 'TimeInterval' with 32 nanoseconds
        // value, since the tiniest non-zero interval that can be represented
        // with 'TimeInterval' is 1 nanosecond or 1000 picoseconds, therefore
        // 767 picoseconds are discarded.
        //
        // +------------------------------------------------------+
        // |                    PICOSECONDS                       |
        // +-------------+------------+-------------+-------------+
        // |             |  SECONDS   | NANOSECONDS | PICOSECONDS |
        // +-------------+------------+-------------+-------------+
        // | int8_t max  |            |             |  127        |
        // | int8_t min  |            |             | -128        |
        // +-------------+------------+-------------+-------------+
        // | int16_t max |            |          32 |  767        |
        // | int16_t min |            |         -32 |  768        |
        // +-------------+------------+-------------+-------------+
        // | int32_t max |            |   2 147 483 |  647        |
        // | int32_t min |            |  -2 147 483 |  648        |
        // +-------------+------------+-------------+-------------+
        // | int64_t max |  9 223 372 | 036 854 775 |  807        |
        // | int64_t min | -9 223 372 | 036 854 775 |  808        |
        // +-------------+------------+-------------+-------------+

        const Obj k_PICO_16_BOUNDARY_MAX( 0,        32      );
        const Obj k_PICO_16_BOUNDARY_MIN( 0,       -32      );

        const Obj k_PICO_16_INVALID_MAX(  0,        33      );
        const Obj k_PICO_16_INVALID_MIN(  0,       -33      );

        const Obj k_PICO_32_BOUNDARY_MAX( 0,        2147483 );
        const Obj k_PICO_32_BOUNDARY_MIN( 0,       -2147483 );
        const Obj k_PICO_32_INVALID_MAX(  0,        2147484 );
        const Obj k_PICO_32_INVALID_MIN(  0,       -2147484 );

        const Obj k_PICO_64_BOUNDARY_MAX( 9223372,  36854775);
        const Obj k_PICO_64_BOUNDARY_MIN(-9223372, -36854775);

        const Obj k_PICO_64_INVALID_MAX = isRealLongDouble
                                   ? Obj( 9223372,  36854776)
                                   : Obj( 9223372,  36854779);

        const Obj k_PICO_64_INVALID_MIN= isRealLongDouble
                                   ? Obj(-9223372, -36854776)
                                   : Obj(-9223372, -36854779);
            // Since MSVC does not support 'long double' type and uses 'double'
            // that has lower precision instead, 'isInDurationRange' can give
            // false positive results for 'TimeInterval' values that are
            // expected to be (and actually are) unacceptable for conversion to
            // 'std::chrono::duration' object.  This only happens with huge
            // values.
            // To avoid this problem we had to calculate invalid values for
            // Windows separately. We took 'double' representation of
            // 'std::chrono::duration' border value, used 'std::nextafter'
            // function toget following existing value and then converted it
            // back to 'TimeInterval' object.
            // Here and below in the test.

        using RCR = RangeCheckResults;
            // Shortcut alias for 'RangeCheckResults' used to reduce length of
            // table rows.

        // The period and the representation type of a duration both affect the
        // range of time interval it can represent, therefore we need to
        // specify expected values for each tested representation tested within
        // testing a period.  I.e., the 'EXP_32' column in the table represents
        // the expected results of the check for the exact 'TimeInterval'
        // values and 'std::chrono::duration' type having signed integer
        // representation with width of exactly 32 bits.

        //        NAME                         EXP_8   EXP_16  EXP_32  EXP_64
        //        --------------------------   ------- ------  ------  ------
        const RCR k_PICO_64_INVALID_MIN_EXP  { false,  false,  false,  false };
        const RCR k_PICO_64_BOUNDARY_MIN_EXP { false,  false,  false,   true };
        const RCR k_PICO_32_INVALID_MIN_EXP  { false,  false,  false,   true };
        const RCR k_PICO_32_BOUNDARY_MIN_EXP { false,  false,   true,   true };
        const RCR k_PICO_16_INVALID_MIN_EXP  { false,  false,   true,   true };
        const RCR k_PICO_16_BOUNDARY_MIN_EXP { false,   true,   true,   true };
        const RCR k_PICO_16_BOUNDARY_MAX_EXP { false,   true,   true,   true };
        const RCR k_PICO_16_INVALID_MAX_EXP  { false,  false,   true,   true };
        const RCR k_PICO_32_BOUNDARY_MAX_EXP { false,  false,   true,   true };
        const RCR k_PICO_32_INVALID_MAX_EXP  { false,  false,  false,   true };
        const RCR k_PICO_64_BOUNDARY_MAX_EXP { false,  false,  false,   true };
        const RCR k_PICO_64_INVALID_MAX_EXP  { false,  false,  false,  false };

        const Data PICO_DATA[] = {
            //LINE INTERVAL                EXPECTED RESULTS
            //---- ----------------------  ---------------------------
            { L_,  k_PICO_64_INVALID_MIN,  k_PICO_64_INVALID_MIN_EXP  },
            { L_,  k_PICO_64_BOUNDARY_MIN, k_PICO_64_BOUNDARY_MIN_EXP },
            { L_,  k_PICO_32_INVALID_MIN,  k_PICO_32_INVALID_MIN_EXP  },
            { L_,  k_PICO_32_BOUNDARY_MIN, k_PICO_32_BOUNDARY_MIN_EXP },
            { L_,  k_PICO_16_INVALID_MIN,  k_PICO_16_INVALID_MIN_EXP  },
            { L_,  k_PICO_16_BOUNDARY_MIN, k_PICO_16_BOUNDARY_MIN_EXP },
            { L_,  k_ZERO,                 k_ZERO_EXP                 },
            { L_,  k_PICO_16_BOUNDARY_MAX, k_PICO_16_BOUNDARY_MAX_EXP },
            { L_,  k_PICO_16_INVALID_MAX,  k_PICO_16_INVALID_MAX_EXP  },
            { L_,  k_PICO_32_BOUNDARY_MAX, k_PICO_32_BOUNDARY_MAX_EXP },
            { L_,  k_PICO_32_INVALID_MAX,  k_PICO_32_INVALID_MAX_EXP  },
            { L_,  k_PICO_64_BOUNDARY_MAX, k_PICO_64_BOUNDARY_MAX_EXP },
            { L_,  k_PICO_64_INVALID_MAX,  k_PICO_64_INVALID_MAX_EXP  },
        };
        const auto NUM_PICO_DATA = sizeof PICO_DATA / sizeof *PICO_DATA;

        for (size_t i = 0; i < NUM_PICO_DATA; i++) {
            const int               LINE        = PICO_DATA[i].d_line;
            const Obj&              INTERVAL    = PICO_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = PICO_DATA[i].d_expected;

            verifyDurationRangeCheck<pico>(LINE, INTERVAL, EXPECTED);
        }

        // +-----------------------------------------------------+
        // |                   PICOSECONDS                       |
        // +------------+------------+-------------+-------------+
        // |            |  SECONDS   | NANOSECONDS | PICOSECONDS |
        // +------------+------------+-------------+-------------+
        // | uint8 max  |            |             |  255        |
        // +------------+------------+-------------+-------------+
        // | uint16 max |            |          65 |  535        |
        // +------------+------------+-------------+-------------+
        // | uint32 max |            |   4 294 967 |  295        |
        // +------------+------------+-------------+-------------+
        // | uint64 max | 18 446 744 | 073 709 551 |  615        |
        // +------------+------------+-------------+-------------+

        const Obj k_U_PICO_16_BOUNDARY_MAX(0,        65     );
        const Obj k_U_PICO_16_INVALID_MAX( 0,        66     );

        const Obj k_U_PICO_32_BOUNDARY_MAX(0,        4294967);
        const Obj k_U_PICO_32_INVALID_MAX( 0,        4294968);

        const Obj k_U_PICO_64_BOUNDARY_MAX(18446744, 73709551);

        const Obj k_U_PICO_64_INVALID_MAX = isRealLongDouble
                                     ? Obj(18446744, 73709552)
                                     : Obj(18446744, 73709556);

        //        NAME                         EXP_8   EXP_16  EXP_32  EXP_64
        //        ---------------------------- ------- ------  ------  ------
        const RCR k_U_PICO_16_BOUNDARY_MAX_EXP{ false,   true,   true,  true };
        const RCR k_U_PICO_16_INVALID_MAX_EXP { false,  false,   true,  true };
        const RCR k_U_PICO_32_BOUNDARY_MAX_EXP{ false,  false,   true,  true };
        const RCR k_U_PICO_32_INVALID_MAX_EXP { false,  false,  false,  true };
        const RCR k_U_PICO_64_BOUNDARY_MAX_EXP{ false,  false,  false,  true };
        const RCR k_U_PICO_64_INVALID_MAX_EXP { false,  false,  false, false };

        const Data U_PICO_DATA[] = {
            //LINE INTERVAL                  EXPECTED RESULTS
            //---- -----------------------   -----------------------------
            { L_,  k_ZERO,                   k_ZERO_EXP                   },
            { L_,  k_U_PICO_16_BOUNDARY_MAX, k_U_PICO_16_BOUNDARY_MAX_EXP },
            { L_,  k_U_PICO_16_INVALID_MAX,  k_U_PICO_16_INVALID_MAX_EXP  },
            { L_,  k_U_PICO_32_BOUNDARY_MAX, k_U_PICO_32_BOUNDARY_MAX_EXP },
            { L_,  k_U_PICO_32_INVALID_MAX,  k_U_PICO_32_INVALID_MAX_EXP  },
            { L_,  k_U_PICO_64_BOUNDARY_MAX, k_U_PICO_64_BOUNDARY_MAX_EXP },
            { L_,  k_U_PICO_64_INVALID_MAX,  k_U_PICO_64_INVALID_MAX_EXP  },
        };
        const auto NUM_U_PICO_DATA = sizeof U_PICO_DATA / sizeof *U_PICO_DATA;

        for (size_t i = 0; i < NUM_U_PICO_DATA; i++) {
            const int               LINE        = U_PICO_DATA[i].d_line;
            const Obj&              INTERVAL    = U_PICO_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_PICO_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<pico>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------+
        // |                 NANOSECONDS               |
        // +------------+----------------+-------------+
        // |            |  SECONDS       | NANOSECONDS |
        // +------------+----------------+-------------+
        // | int8 max   |                |         127 |
        // | int8 min   |                |        -128 |
        // +------------+----------------+-------------+
        // | int16 max  |                |      32 767 |
        // | int16 min  |                |     -32 768 |
        // +------------+----------------+-------------+
        // | int32 max  |              2 | 147 483 647 |
        // | int32 min  |             -2 | 147 483 648 |
        // +------------+----------------+-------------+
        // | int64 max  |  9 223 372 036 | 854 775 807 |
        // | int64 min  | -9 223 372 036 | 854 775 808 |
        // +------------+----------------+-------------+

        const Obj k_NANO_8_BOUNDARY_MAX(  0,           127       );
        const Obj k_NANO_8_BOUNDARY_MIN(  0,          -128       );
        const Obj k_NANO_8_INVALID_MAX(   0,           128       );
        const Obj k_NANO_8_INVALID_MIN(   0,          -129       );

        const Obj k_NANO_16_BOUNDARY_MAX( 0,           32767     );
        const Obj k_NANO_16_BOUNDARY_MIN( 0,          -32768     );
        const Obj k_NANO_16_INVALID_MAX(  0,           32768     );
        const Obj k_NANO_16_INVALID_MIN(  0,          -32769     );

        const Obj k_NANO_32_BOUNDARY_MAX( 2,           147483647 );
        const Obj k_NANO_32_BOUNDARY_MIN(-2,          -147483648 );
        const Obj k_NANO_32_INVALID_MAX(  2,           147483648 );
        const Obj k_NANO_32_INVALID_MIN( -2,          -147483649 );

        const Obj k_NANO_64_BOUNDARY_MAX( 9223372036,  854775807 );
        const Obj k_NANO_64_BOUNDARY_MIN(-9223372036, -854775808 );

        const Obj k_NANO_64_INVALID_MAX = isRealLongDouble
                                   ? Obj( 9223372036,  854775808 )
                                   : Obj( 9223372036,  854777856 );
        const Obj k_NANO_64_INVALID_MIN = isRealLongDouble
                                   ? Obj(-9223372036, -854775809 )
                                   : Obj(-9223372036, -854777856 );

        const Data NANO_DATA[] = {
            //LINE INTERVAL                 EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------  ------- ------  ------  ------
            { L_,  k_NANO_64_INVALID_MIN,  { false,  false,  false, false } },
            { L_,  k_NANO_64_BOUNDARY_MIN, { false,  false,  false,  true } },
            { L_,  k_NANO_32_INVALID_MIN,  { false,  false,  false,  true } },
            { L_,  k_NANO_32_BOUNDARY_MIN, { false,  false,   true,  true } },
            { L_,  k_NANO_16_INVALID_MIN,  { false,  false,   true,  true } },
            { L_,  k_NANO_16_BOUNDARY_MIN, { false,   true,   true,  true } },
            { L_,  k_NANO_8_INVALID_MIN,   { false,   true,   true,  true } },
            { L_,  k_NANO_8_BOUNDARY_MIN,  {  true,   true,   true,  true } },
            { L_,  k_ZERO,                 {  true,   true,   true,  true } },
            { L_,  k_NANO_8_BOUNDARY_MAX,  {  true,   true,   true,  true } },
            { L_,  k_NANO_8_INVALID_MAX,   { false,   true,   true,  true } },
            { L_,  k_NANO_16_BOUNDARY_MAX, { false,   true,   true,  true } },
            { L_,  k_NANO_16_INVALID_MAX,  { false,  false,   true,  true } },
            { L_,  k_NANO_32_BOUNDARY_MAX, { false,  false,   true,  true } },
            { L_,  k_NANO_32_INVALID_MAX,  { false,  false,  false,  true } },
            { L_,  k_NANO_64_BOUNDARY_MAX, { false,  false,  false,  true } },
            { L_,  k_NANO_64_INVALID_MAX,  { false,  false,  false, false } },
        };
        const auto NUM_NANO_DATA = sizeof NANO_DATA / sizeof *NANO_DATA;

        for (size_t i = 0; i < NUM_NANO_DATA; i++) {
            const int               LINE        = NANO_DATA[i].d_line;
            const Obj&              INTERVAL    = NANO_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = NANO_DATA[i].d_expected;

            verifyDurationRangeCheck<nano>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------+
        // |                NANOSECONDS                |
        // +------------+----------------+-------------+
        // |            |     SECONDS    | NANOSECONDS |
        // +------------+----------------+-------------+
        // | uint8 max  |                |         255 |
        // +------------+----------------+-------------+
        // | uint16 max |                |      65 535 |
        // +------------+----------------+-------------+
        // | uint32 max |              4 | 294 967 295 |
        // +------------+----------------+-------------+
        // | uint64 max | 18 446 744 073 | 709 551 615 |
        // +------------+----------------+-------------+

        const Obj k_U_NANO_8_BOUNDARY_MAX(  0,           255       );
        const Obj k_U_NANO_8_INVALID_MAX(   0,           256       );

        const Obj k_U_NANO_16_BOUNDARY_MAX( 0,           65535     );
        const Obj k_U_NANO_16_INVALID_MAX(  0,           65536     );

        const Obj k_U_NANO_32_BOUNDARY_MAX( 4,           294967295 );
        const Obj k_U_NANO_32_INVALID_MAX(  4,           294967296 );

        const Obj k_U_NANO_64_BOUNDARY_MAX( 18446744073, 709551615 );
        const Obj k_U_NANO_64_INVALID_MAX = isRealLongDouble
                                     ? Obj( 18446744073, 709551616 )
                                     : Obj( 18446744073, 709555712 );

        const Data U_NANO_DATA[] = {
            //LINE INTERVAL                  EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------   ------- ------  ------  ------
            { L_,  k_ZERO,                  {  true,   true,   true,  true } },
            { L_,  k_U_NANO_8_BOUNDARY_MAX, {  true,   true,   true,  true } },
            { L_,  k_U_NANO_8_INVALID_MAX,  { false,   true,   true,  true } },
            { L_,  k_U_NANO_16_BOUNDARY_MAX,{ false,   true,   true,  true } },
            { L_,  k_U_NANO_16_INVALID_MAX, { false,  false,   true,  true } },
            { L_,  k_U_NANO_32_BOUNDARY_MAX,{ false,  false,   true,  true } },
            { L_,  k_U_NANO_32_INVALID_MAX, { false,  false,  false,  true } },
            { L_,  k_U_NANO_64_BOUNDARY_MAX,{ false,  false,  false,  true } },
            { L_,  k_U_NANO_64_INVALID_MAX, { false,  false,  false, false } },
        };
        const auto NUM_U_NANO_DATA = sizeof U_NANO_DATA / sizeof *U_NANO_DATA;

        for (size_t i = 0; i < NUM_U_NANO_DATA; i++) {
            const int               LINE        = U_NANO_DATA[i].d_line;
            const Obj&              INTERVAL    = U_NANO_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_NANO_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<nano>(LINE, INTERVAL, EXPECTED);
        }

        // +-----------------------------------------------+
        // |                    MICROSECONDS               |
        // +------------+--------------------+-------------+
        // |            |  SECONDS           | NANOSECONDS |
        // +------------+--------------------+-------------+
        // | int8 max   |                    |         127 |
        // | int8 min   |                    |        -128 |
        // +------------+--------------------+-------------+
        // | int16 max  |                    |      32 767 |
        // | int16 min  |                    |     -32 768 |
        // +------------+--------------------+-------------+
        // | int32 max  |              2 147 |     483 647 |
        // | int32 min  |             -2 147 |     483 648 |
        // +------------+--------------------+-------------+
        // | int64 max  |  9 223 372 036 854 |     775 807 |
        // | int64 min  | -9 223 372 036 854 |     775 808 |
        // +------------+--------------------+-------------+

        const Obj k_MICRO_8_BOUNDARY_MAX(   0,              127000    );
        const Obj k_MICRO_8_BOUNDARY_MIN(   0,             -128000    );
        const Obj k_MICRO_8_INVALID_MAX(    0,              127001    );
        const Obj k_MICRO_8_INVALID_MIN(    0,             -128001    );

        const Obj k_MICRO_16_BOUNDARY_MAX(  0,              32767000  );
        const Obj k_MICRO_16_BOUNDARY_MIN(  0,             -32768000  );
        const Obj k_MICRO_16_INVALID_MAX(   0,              32767001  );
        const Obj k_MICRO_16_INVALID_MIN(   0,             -32768001  );

        const Obj k_MICRO_32_BOUNDARY_MAX(  2147,           483647000 );
        const Obj k_MICRO_32_BOUNDARY_MIN( -2147,          -483648000 );
        const Obj k_MICRO_32_INVALID_MAX(   2147,           483647001 );
        const Obj k_MICRO_32_INVALID_MIN(  -2147,          -483648001 );

        const Obj k_MICRO_64_BOUNDARY_MAX(  9223372036854,  775807000 );
        const Obj k_MICRO_64_BOUNDARY_MIN( -9223372036854, -775808000 );

        const Obj k_MICRO_64_INVALID_MAX = isRealLongDouble
                                     ? Obj( 9223372036854,  775808000 )
                                     : Obj( 9223372036854,  777856000 );
        const Obj k_MICRO_64_INVALID_MIN = isRealLongDouble
                                     ? Obj(-9223372036854, -775809000 )
                                     : Obj(-9223372036854, -777856000 );

        const Data MICRO_DATA[] = {
            //LINE INTERVAL                 EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------  ------- ------  ------  ------
            { L_,  k_MICRO_64_INVALID_MIN,  { false,  false,  false, false } },
            { L_,  k_MICRO_64_BOUNDARY_MIN, { false,  false,  false,  true } },
            { L_,  k_MICRO_32_INVALID_MIN,  { false,  false,  false,  true } },
            { L_,  k_MICRO_32_BOUNDARY_MIN, { false,  false,   true,  true } },
            { L_,  k_MICRO_16_INVALID_MIN,  { false,  false,   true,  true } },
            { L_,  k_MICRO_16_BOUNDARY_MIN, { false,   true,   true,  true } },
            { L_,  k_MICRO_8_INVALID_MIN,   { false,   true,   true,  true } },
            { L_,  k_MICRO_8_BOUNDARY_MIN,  {  true,   true,   true,  true } },
            { L_,  k_ZERO,                  {  true,   true,   true,  true } },
            { L_,  k_MICRO_8_BOUNDARY_MAX,  {  true,   true,   true,  true } },
            { L_,  k_MICRO_8_INVALID_MAX,   { false,   true,   true,  true } },
            { L_,  k_MICRO_16_BOUNDARY_MAX, { false,   true,   true,  true } },
            { L_,  k_MICRO_16_INVALID_MAX,  { false,  false,   true,  true } },
            { L_,  k_MICRO_32_BOUNDARY_MAX, { false,  false,   true,  true } },
            { L_,  k_MICRO_32_INVALID_MAX,  { false,  false,  false,  true } },
            { L_,  k_MICRO_64_BOUNDARY_MAX, { false,  false,  false,  true } },
            { L_,  k_MICRO_64_INVALID_MAX,  { false,  false,  false, false } },
        };
        const auto NUM_MICRO_DATA = sizeof MICRO_DATA / sizeof *MICRO_DATA;

        for (size_t i = 0; i < NUM_MICRO_DATA; i++) {
            const int               LINE        = MICRO_DATA[i].d_line;
            const Obj&              INTERVAL    = MICRO_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = MICRO_DATA[i].d_expected;

            verifyDurationRangeCheck<micro>(LINE, INTERVAL, EXPECTED);
        }

        // +-----------------------------------------------+
        // |                   MICROSECONDS                |
        // +------------+--------------------+-------------+
        // |            |     SECONDS        | NANOSECONDS |
        // +------------+--------------------+-------------+
        // | uint8 max  |                    |         255 |
        // +------------+--------------------+-------------+
        // | uint16 max |                    |      65 535 |
        // +------------+--------------------+-------------+
        // | uint32 max |              4 294 |     967 295 |
        // +------------+--------------------+-------------+
        // | uint64 max | 18 446 744 073 709 |     551 615 |
        // +------------+--------------------+-------------+

        const Obj k_U_MICRO_8_BOUNDARY_MAX(  0,              255000    );
        const Obj k_U_MICRO_8_INVALID_MAX(   0,              255001    );

        const Obj k_U_MICRO_16_BOUNDARY_MAX( 0,              65535000  );
        const Obj k_U_MICRO_16_INVALID_MAX(  0,              65535001  );

        const Obj k_U_MICRO_32_BOUNDARY_MAX( 4294,           967295000 );
        const Obj k_U_MICRO_32_INVALID_MAX(  4294,           967295001 );

        const Obj k_U_MICRO_64_BOUNDARY_MAX( 18446744073709, 551615000 );
        const Obj k_U_MICRO_64_INVALID_MAX = isRealLongDouble
                                      ? Obj( 18446744073709, 551616000 )
                                      : Obj( 18446744073709, 555712000 );

        const Data U_MICRO_DATA[] = {
            //LINE INTERVAL                    EXP_8   EXP_16 EXP_32  EXP_64
            //---- -----------------------     ------- ------ ------  ------
            { L_,  k_ZERO,                   {  true,   true,  true,  true } },
            { L_,  k_U_MICRO_8_BOUNDARY_MAX, {  true,   true,  true,  true } },
            { L_,  k_U_MICRO_8_INVALID_MAX,  { false,   true,  true,  true } },
            { L_,  k_U_MICRO_16_BOUNDARY_MAX,{ false,   true,  true,  true } },
            { L_,  k_U_MICRO_16_INVALID_MAX, { false,  false,  true,  true } },
            { L_,  k_U_MICRO_32_BOUNDARY_MAX,{ false,  false,  true,  true } },
            { L_,  k_U_MICRO_32_INVALID_MAX, { false,  false, false,  true } },
            { L_,  k_U_MICRO_64_BOUNDARY_MAX,{ false,  false, false,  true } },
            { L_,  k_U_MICRO_64_INVALID_MAX, { false,  false, false, false } },
        };
        const auto NUM_U_MICRO_DATA =
                                    sizeof U_MICRO_DATA / sizeof *U_MICRO_DATA;

        for (size_t i = 0; i < NUM_U_MICRO_DATA; i++) {
            const int               LINE        = U_MICRO_DATA[i].d_line;
            const Obj&              INTERVAL    = U_MICRO_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_MICRO_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<micro>(LINE, INTERVAL, EXPECTED);
        }

        // +---------------------------------------------------+
        // |                      MILLISECONDS                 |
        // +------------+------------------------+-------------+
        // |            |       SECONDS          | NANOSECONDS |
        // +------------+------------------------+-------------+
        // | int8 max   |                        |         127 |
        // | int8 min   |                        |        -128 |
        // +------------+------------------------+-------------+
        // | int16 max  |                     32 |         767 |
        // | int16 min  |                    -32 |         768 |
        // +------------+------------------------+-------------+
        // | int32 max  |              2 147 483 |         647 |
        // | int32 min  |             -2 147 483 |         648 |
        // +------------+------------------------+-------------+
        // | int64 max  |  9 223 372 036 854 775 |         807 |
        // | int64 min  | -9 223 372 036 854 775 |         808 |
        // +------------+------------------------+-------------+

        const Obj k_MILLI_8_BOUNDARY_MAX(   0,                 127000000 );
        const Obj k_MILLI_8_BOUNDARY_MIN(   0,                -128000000 );
        const Obj k_MILLI_8_INVALID_MAX(    0,                 127000001 );
        const Obj k_MILLI_8_INVALID_MIN(    0,                -128000001 );

        const Obj k_MILLI_16_BOUNDARY_MAX(  32,                767000000 );
        const Obj k_MILLI_16_BOUNDARY_MIN( -32,               -768000000 );
        const Obj k_MILLI_16_INVALID_MAX(   32,                767000001 );
        const Obj k_MILLI_16_INVALID_MIN(  -32,               -768000001 );

        const Obj k_MILLI_32_BOUNDARY_MAX(  2147483,           647000000 );
        const Obj k_MILLI_32_BOUNDARY_MIN( -2147483,          -648000000 );
        const Obj k_MILLI_32_INVALID_MAX(   2147483,           647000001 );
        const Obj k_MILLI_32_INVALID_MIN(  -2147483,          -648000001 );

        const Obj k_MILLI_64_BOUNDARY_MAX(  9223372036854775,  807000000 );
        const Obj k_MILLI_64_BOUNDARY_MIN( -9223372036854775, -808000000 );

       const Obj k_MILLI_64_INVALID_MAX = isRealLongDouble
                                    ? Obj(  9223372036854775,  808000000 )
                                    : Obj(  9223372036854778,  0         );
       const Obj k_MILLI_64_INVALID_MIN= isRealLongDouble
                                    ? Obj( -9223372036854775, -809000000 )
                                    : Obj( -9223372036854778,  0         );
           // Note that 'k_MILLI_64_INVALID_MIN' differs from the
           // 'k_MILLI_64_BOUNDARY_MAX' not by 1 but by 1000 (i.e. we add 1
           // microsecond) because of insufficient precision of 'long double'
           // for such big values.

        const Data MILLI_DATA[] = {
            //LINE INTERVAL                 EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------  ------- ------  ------  ------
            { L_,  k_MILLI_64_INVALID_MIN,  { false,  false,  false, false } },
            { L_,  k_MILLI_64_BOUNDARY_MIN, { false,  false,  false,  true } },
            { L_,  k_MILLI_32_INVALID_MIN,  { false,  false,  false,  true } },
            { L_,  k_MILLI_32_BOUNDARY_MIN, { false,  false,   true,  true } },
            { L_,  k_MILLI_16_INVALID_MIN,  { false,  false,   true,  true } },
            { L_,  k_MILLI_16_BOUNDARY_MIN, { false,   true,   true,  true } },
            { L_,  k_MILLI_8_INVALID_MIN,   { false,   true,   true,  true } },
            { L_,  k_MILLI_8_BOUNDARY_MIN,  {  true,   true,   true,  true } },
            { L_,  k_ZERO,                  {  true,   true,   true,  true } },
            { L_,  k_MILLI_8_BOUNDARY_MAX,  {  true,   true,   true,  true } },
            { L_,  k_MILLI_8_INVALID_MAX,   { false,   true,   true,  true } },
            { L_,  k_MILLI_16_BOUNDARY_MAX, { false,   true,   true,  true } },
            { L_,  k_MILLI_16_INVALID_MAX,  { false,  false,   true,  true } },
            { L_,  k_MILLI_32_BOUNDARY_MAX, { false,  false,   true,  true } },
            { L_,  k_MILLI_32_INVALID_MAX,  { false,  false,  false,  true } },
            { L_,  k_MILLI_64_BOUNDARY_MAX, { false,  false,  false,  true } },
            { L_,  k_MILLI_64_INVALID_MAX,  { false,  false,  false, false } },
        };
        const auto NUM_MILLI_DATA = sizeof MILLI_DATA / sizeof *MILLI_DATA;

        for (size_t i = 0; i < NUM_MILLI_DATA; i++) {
            const int               LINE        = MILLI_DATA[i].d_line;
            const Obj&              INTERVAL    = MILLI_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = MILLI_DATA[i].d_expected;

            verifyDurationRangeCheck<milli>(LINE, INTERVAL, EXPECTED);
        }

        // +---------------------------------------------------+
        // |                   MILLISECONDS                    |
        // +------------+------------------------+-------------+
        // |            |         SECONDS        | NANOSECONDS |
        // +------------+------------------------+-------------+
        // | uint8 max  |                        |         255 |
        // +------------+------------------------+-------------+
        // | uint16 max |                     65 |         535 |
        // +------------+------------------------+-------------+
        // | uint32 max |              4 294 967 |         295 |
        // +------------+------------------------+-------------+
        // | uint64 max | 18 446 744 073 709 551 |         615 |
        // +------------+------------------------+-------------+

        const Obj k_U_MILLI_8_BOUNDARY_MAX(  0,                 255000000 );
        const Obj k_U_MILLI_8_INVALID_MAX(   0,                 255000001 );

        const Obj k_U_MILLI_16_BOUNDARY_MAX( 65,                535000000 );
        const Obj k_U_MILLI_16_INVALID_MAX(  65,                535000001 );

        const Obj k_U_MILLI_32_BOUNDARY_MAX( 4294967,           295000000 );
        const Obj k_U_MILLI_32_INVALID_MAX(  4294967,           295000001 );

        const Obj k_U_MILLI_64_BOUNDARY_MAX( 18446744073709551, 615000000 );
        const Obj k_U_MILLI_64_INVALID_MAX = isRealLongDouble
                                      ? Obj( 18446744073709551, 616000000 )
                                      : Obj( 18446744073709555, 712000000 );

        const Data U_MILLI_DATA[] = {
            //LINE INTERVAL                    EXP_8   EXP_16 EXP_32  EXP_64
            //---- -----------------------     ------- ------ ------  ------
            { L_,  k_ZERO,                   {  true,   true,  true,  true } },
            { L_,  k_U_MILLI_8_BOUNDARY_MAX, {  true,   true,  true,  true } },
            { L_,  k_U_MILLI_8_INVALID_MAX,  { false,   true,  true,  true } },
            { L_,  k_U_MILLI_16_BOUNDARY_MAX,{ false,   true,  true,  true } },
            { L_,  k_U_MILLI_16_INVALID_MAX, { false,  false,  true,  true } },
            { L_,  k_U_MILLI_32_BOUNDARY_MAX,{ false,  false,  true,  true } },
            { L_,  k_U_MILLI_32_INVALID_MAX, { false,  false, false,  true } },
            { L_,  k_U_MILLI_64_BOUNDARY_MAX,{ false,  false, false,  true } },
            { L_,  k_U_MILLI_64_INVALID_MAX, { false,  false, false, false } },
        };
        const auto NUM_U_MILLI_DATA =
                                    sizeof U_MILLI_DATA / sizeof *U_MILLI_DATA;

        for (size_t i = 0; i < NUM_U_MILLI_DATA; i++) {
            const int               LINE        = U_MILLI_DATA[i].d_line;
            const Obj&              INTERVAL    = U_MILLI_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_MILLI_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<milli>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                     THIRD SECONDS                     |
        // +------------+----------------------------+-------------+
        // |            |          SECONDS           | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | int8 max   |                        127 |             |
        // | int8 min   |                       -128 |             |
        // +------------+----------------------------+-------------+
        // | int16 max  |                     32 767 |             |
        // | int16 min  |                    -32 768 |             |
        // +------------+----------------------------+-------------+
        // | int32 max  |              2 147 483 647 |             |
        // | int32 min  |             -2 147 483 648 |             |
        // +------------+----------------------------+-------------+
        // | int64 max  |  9 223 372 036 854 775 807 |             |
        // | int64 min  | -9 223 372 036 854 775 808 |             |
        // +------------+----------------------------+-------------+

        const Obj k_THIRD_8_BOUNDARY_MAX(   42,                   333333333 );
        const Obj k_THIRD_8_BOUNDARY_MIN(  -42,                  -666666666 );
        const Obj k_THIRD_8_INVALID_MAX(    42,                   333333334 );
        const Obj k_THIRD_8_INVALID_MIN(   -42,                  -666666667 );

        const Obj k_THIRD_16_BOUNDARY_MAX(  10922,                333333333 );
        const Obj k_THIRD_16_BOUNDARY_MIN( -10922,               -666666666 );
        const Obj k_THIRD_16_INVALID_MAX(   10922,                333333334 );
        const Obj k_THIRD_16_INVALID_MIN(  -10922,               -666666667 );

        const Obj k_THIRD_32_BOUNDARY_MAX(  715827882,            333333333 );
        const Obj k_THIRD_32_BOUNDARY_MIN( -715827882,           -666666666 );

        const Obj k_THIRD_32_INVALID_MAX = isRealLongDouble
                                    ? Obj(  715827882,            333333334 )
                                    : Obj(  715827882,            666666667 );
        const Obj k_THIRD_32_INVALID_MIN = isRealLongDouble
                                    ? Obj( -715827882,           -666666667 )
                                    : Obj( -715827882,           -666666880 );

        const Obj k_THIRD_64_BOUNDARY_MAX(  3074457345618258602,  333333333 );
        const Obj k_THIRD_64_BOUNDARY_MIN( -3074457345618258602, -666666666 );

        const Obj k_THIRD_64_INVALID_MAX = isRealLongDouble
                                   ? Obj(  3074457345618258603,   0         )
                                   : Obj(  3074457345618259519,   0         );
        const Obj k_THIRD_64_INVALID_MIN = isRealLongDouble
                                   ? Obj( -3074457345618258603,   0         )
                                   : Obj( -3074457345618259519,   0         );

        const Data THIRD_DATA[] = {
            //LINE INTERVAL                  EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------   ------- ------  ------  ------
            { L_,  k_THIRD_64_INVALID_MIN,  { false,  false,  false, false } },
            { L_,  k_THIRD_64_BOUNDARY_MIN, { false,  false,  false,  true } },
            { L_,  k_THIRD_32_INVALID_MIN,  { false,  false,  false,  true } },
            { L_,  k_THIRD_32_BOUNDARY_MIN, { false,  false,   true,  true } },
            { L_,  k_THIRD_16_INVALID_MIN,  { false,  false,   true,  true } },
            { L_,  k_THIRD_16_BOUNDARY_MIN, { false,   true,   true,  true } },
            { L_,  k_THIRD_8_INVALID_MIN,   { false,   true,   true,  true } },
            { L_,  k_THIRD_8_BOUNDARY_MIN,  {  true,   true,   true,  true } },
            { L_,  k_ZERO,                  {  true,   true,   true,  true } },
            { L_,  k_THIRD_8_BOUNDARY_MAX,  {  true,   true,   true,  true } },
            { L_,  k_THIRD_8_INVALID_MAX,   { false,   true,   true,  true } },
            { L_,  k_THIRD_16_BOUNDARY_MAX, { false,   true,   true,  true } },
            { L_,  k_THIRD_16_INVALID_MAX,  { false,  false,   true,  true } },
            { L_,  k_THIRD_32_BOUNDARY_MAX, { false,  false,   true,  true } },
            { L_,  k_THIRD_32_INVALID_MAX,  { false,  false,  false,  true } },
            { L_,  k_THIRD_64_BOUNDARY_MAX, { false,  false,  false,  true } },
            { L_,  k_THIRD_64_INVALID_MAX,  { false,  false,  false, false } },
        };
        const auto NUM_THIRD_DATA = sizeof THIRD_DATA / sizeof *THIRD_DATA;

        for (size_t i = 0; i < NUM_THIRD_DATA; i++) {
            const int               LINE        = THIRD_DATA[i].d_line;
            const Obj&              INTERVAL    = THIRD_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = THIRD_DATA[i].d_expected;

            verifyDurationRangeCheck<third>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                   THIRD SECONDS                       |
        // +------------+----------------------------+-------------+
        // |            |         SECONDS            | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | uint8 max  |                        255 |             |
        // +------------+----------------------------+-------------+
        // | uint16 max |                     65 535 |             |
        // +------------+----------------------------+-------------+
        // | uint32 max |              4 294 967 295 |             |
        // +------------+----------------------------+-------------+
        // | uint64 max | 18 446 744 073 709 551 615 |             |
        // +------------+----------------------------+-------------+

        const Obj k_U_THIRD_8_BOUNDARY_MAX(  85,                   0   );
        const Obj k_U_THIRD_8_INVALID_MAX(   85,                   1   );

        const Obj k_U_THIRD_16_BOUNDARY_MAX( 21845,                0   );
        const Obj k_U_THIRD_16_INVALID_MAX(  21845,                1   );

        const Obj k_U_THIRD_32_BOUNDARY_MAX( 1431655765,           0   );

        const Obj k_U_THIRD_32_INVALID_MAX = isRealLongDouble
                                      ? Obj( 1431655765,           1   )
                                      : Obj( 1431655765,           256 );

        const Obj k_U_THIRD_64_BOUNDARY_MAX( 6148914691236517205,  0   );
        const Obj k_U_THIRD_64_INVALID_MAX(  6148914691236519038,  0   );

        //        NAME                          EXP_8   EXP_16  EXP_32  EXP_64
        //        ----------------------------  ------- ------  ------  ------
        const RCR k_U_THIRD_8_BOUNDARY_MAX_EXP {  true,   true,  true,  true };
        const RCR k_U_THIRD_8_INVALID_MAX_EXP  { false,   true,  true,  true };
        const RCR k_U_THIRD_16_BOUNDARY_MAX_EXP{ false,   true,  true,  true };
        const RCR k_U_THIRD_16_INVALID_MAX_EXP { false,  false,  true,  true };
        const RCR k_U_THIRD_32_BOUNDARY_MAX_EXP{ false,  false,  true,  true };
        const RCR k_U_THIRD_32_INVALID_MAX_EXP { false,  false, false,  true };
        const RCR k_U_THIRD_64_BOUNDARY_MAX_EXP{ false,  false, false,  true };
        const RCR k_U_THIRD_64_INVALID_MAX_EXP { false,  false, false, false };

        const Data U_THIRD_DATA[] = {
            //LINE INTERVAL                   EXPECTED RESULTS
            //---- -----------------------    -----------------------------
            { L_,  k_ZERO,                    k_ZERO_EXP                    },
            { L_,  k_U_THIRD_8_BOUNDARY_MAX,  k_U_THIRD_8_BOUNDARY_MAX_EXP  },
            { L_,  k_U_THIRD_8_INVALID_MAX,   k_U_THIRD_8_INVALID_MAX_EXP   },
            { L_,  k_U_THIRD_16_BOUNDARY_MAX, k_U_THIRD_16_BOUNDARY_MAX_EXP },
            { L_,  k_U_THIRD_16_INVALID_MAX,  k_U_THIRD_16_INVALID_MAX_EXP  },
            { L_,  k_U_THIRD_32_BOUNDARY_MAX, k_U_THIRD_32_BOUNDARY_MAX_EXP },
            { L_,  k_U_THIRD_32_INVALID_MAX,  k_U_THIRD_32_INVALID_MAX_EXP  },
            { L_,  k_U_THIRD_64_BOUNDARY_MAX, k_U_THIRD_64_BOUNDARY_MAX_EXP },
            { L_,  k_U_THIRD_64_INVALID_MAX,  k_U_THIRD_64_INVALID_MAX_EXP  },
        };
        const auto NUM_U_THIRD_DATA =
                                    sizeof U_THIRD_DATA / sizeof *U_THIRD_DATA;

        for (size_t i = 0; i < NUM_U_THIRD_DATA; i++) {
            const int               LINE        = U_THIRD_DATA[i].d_line;
            const Obj&              INTERVAL    = U_THIRD_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_THIRD_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<third>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                      HALF SECONDS                     |
        // +------------+----------------------------+-------------+
        // |            |          SECONDS           | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | int8 max   |                        127 |             |
        // | int8 min   |                       -128 |             |
        // +------------+----------------------------+-------------+
        // | int16 max  |                     32 767 |             |
        // | int16 min  |                    -32 768 |             |
        // +------------+----------------------------+-------------+
        // | int32 max  |              2 147 483 647 |             |
        // | int32 min  |             -2 147 483 648 |             |
        // +------------+----------------------------+-------------+
        // | int64 max  |  9 223 372 036 854 775 807 |             |
        // | int64 min  | -9 223 372 036 854 775 808 |             |
        // +------------+----------------------------+-------------+

        const Obj k_HALF_8_BOUNDARY_MAX(   63,                   500000000 );
        const Obj k_HALF_8_BOUNDARY_MIN(  -64,                   0         );
        const Obj k_HALF_8_INVALID_MAX(    63,                   500000001 );
        const Obj k_HALF_8_INVALID_MIN(   -64,                  -1         );

        const Obj k_HALF_16_BOUNDARY_MAX(  16383,                500000000 );
        const Obj k_HALF_16_BOUNDARY_MIN( -16384,                0         );
        const Obj k_HALF_16_INVALID_MAX(   16383,                500000001 );
        const Obj k_HALF_16_INVALID_MIN(  -16384,               -1         );

        const Obj k_HALF_32_BOUNDARY_MAX(  1073741823,           500000000 );
        const Obj k_HALF_32_BOUNDARY_MIN( -1073741824,           0         );

        const Obj k_HALF_32_INVALID_MAX = isRealLongDouble
                                   ? Obj(  1073741823,           500000001 )
                                   : Obj(  1073741824,           256000000 );
        const Obj k_HALF_32_INVALID_MIN = isRealLongDouble
                                   ? Obj( -1073741824,          -1         )
                                   : Obj( -1073741824,          -256000000 );

        const Obj k_HALF_64_BOUNDARY_MAX(  4611686018427387903,  500000000 );
        const Obj k_HALF_64_BOUNDARY_MIN( -4611686018427387904,  0         );

        const Obj k_HALF_64_INVALID_MAX = isRealLongDouble
                                   ? Obj(  4611686018427387904,  0         )
                                   : Obj(  4611686018427388928,  0         );
        const Obj k_HALF_64_INVALID_MIN = isRealLongDouble
                                   ? Obj( -4611686018427387905,  0         )
                                   : Obj( -4611686018427388928,  0         );

        const Data HALF_DATA[] = {
            //LINE INTERVAL                EXP_8   EXP_16  EXP_32  EXP_64
            //---- ----------------------- ------- ------  ------  ------
            { L_,  k_HALF_64_INVALID_MIN, { false,  false,  false, false } },
            { L_,  k_HALF_64_BOUNDARY_MIN,{ false,  false,  false,  true } },
            { L_,  k_HALF_32_INVALID_MIN, { false,  false,  false,  true } },
            { L_,  k_HALF_32_BOUNDARY_MIN,{ false,  false,   true,  true } },
            { L_,  k_HALF_16_INVALID_MIN, { false,  false,   true,  true } },
            { L_,  k_HALF_16_BOUNDARY_MIN,{ false,   true,   true,  true } },
            { L_,  k_HALF_8_INVALID_MIN,  { false,   true,   true,  true } },
            { L_,  k_HALF_8_BOUNDARY_MIN, {  true,   true,   true,  true } },
            { L_,  k_ZERO,                {  true,   true,   true,  true } },
            { L_,  k_HALF_8_BOUNDARY_MAX, {  true,   true,   true,  true } },
            { L_,  k_HALF_8_INVALID_MAX,  { false,   true,   true,  true } },
            { L_,  k_HALF_16_BOUNDARY_MAX,{ false,   true,   true,  true } },
            { L_,  k_HALF_16_INVALID_MAX, { false,  false,   true,  true } },
            { L_,  k_HALF_32_BOUNDARY_MAX,{ false,  false,   true,  true } },
            { L_,  k_HALF_32_INVALID_MAX, { false,  false,  false,  true } },
            { L_,  k_HALF_64_BOUNDARY_MAX,{ false,  false,  false,  true } },
            { L_,  k_HALF_64_INVALID_MAX, { false,  false,  false, false } },
        };
        const auto NUM_HALF_DATA = sizeof HALF_DATA / sizeof *HALF_DATA;

        for (size_t i = 0; i < NUM_HALF_DATA; i++) {
            const int               LINE        = HALF_DATA[i].d_line;
            const Obj&              INTERVAL    = HALF_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = HALF_DATA[i].d_expected;

            verifyDurationRangeCheck<half>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                   HALF SECONDS                        |
        // +------------+----------------------------+-------------+
        // |            |         SECONDS            | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | uint8 max  |                        255 |             |
        // +------------+----------------------------+-------------+
        // | uint16 max |                     65 535 |             |
        // +------------+----------------------------+-------------+
        // | uint32 max |              4 294 967 295 |             |
        // +------------+----------------------------+-------------+
        // | uint64 max | 18 446 744 073 709 551 615 |             |
        // +------------+----------------------------+-------------+

        const Obj k_U_HALF_8_BOUNDARY_MAX(  127,        500000000 );
        const Obj k_U_HALF_8_INVALID_MAX(   127,        500000001 );

        const Obj k_U_HALF_16_BOUNDARY_MAX( 32767,      500000000 );
        const Obj k_U_HALF_16_INVALID_MAX(  32767,      500000001 );

        const Obj k_U_HALF_32_BOUNDARY_MAX( 2147483647, 500000000 );

        const Obj k_U_HALF_32_INVALID_MAX = isRealLongDouble
                                     ? Obj( 2147483647, 500000001 )
                                     : Obj( 2147483647, 500000256 );

        const Obj k_U_HALF_64_BOUNDARY_MAX( LLONG_MAX,  500000000 );
        const Obj k_U_HALF_64_INVALID_MAX(  LLONG_MAX,  999999999 );

        //        NAME                         EXP_8   EXP_16  EXP_32  EXP_64
        //        ---------------------------- ------- ------  ------  ------
        const RCR k_U_HALF_8_BOUNDARY_MAX_EXP  {  true,   true,  true,  true };
        const RCR k_U_HALF_8_INVALID_MAX_EXP   { false,   true,  true,  true };
        const RCR k_U_HALF_16_BOUNDARY_MAX_EXP { false,   true,  true,  true };
        const RCR k_U_HALF_16_INVALID_MAX_EXP  { false,  false,  true,  true };
        const RCR k_U_HALF_32_BOUNDARY_MAX_EXP { false,  false,  true,  true };
        const RCR k_U_HALF_32_INVALID_MAX_EXP  { false,  false, false,  true };
        const RCR k_U_HALF_64_BOUNDARY_MAX_EXP { false,  false, false,  true };
        const RCR k_U_HALF_64_INVALID_MAX_EXP  = isRealLongDouble
                                         ? RCR { false,  false, false, false }
                                         : RCR { false,  false, false,  true };
            // Note that 'k_U_HALF_64_INVALID_MAX' is the maximum
            // 'TimeInterval' value.  But due to the low precision of
            // 'long double' type on Windows, this value is still considered
            // valid for conversion to
            // 'std:chrono::duration<uint64_t, std::ratio<1, 2> >' variable.
            // As we can't get bigger 'TimeInterval' value, we have to consider
            // this in the expected results.

        const Data U_HALF_DATA[] = {
            //LINE INTERVAL                  EXPECTED RESULTS
            //---- -----------------------   -----------------------------
            { L_,  k_ZERO,                   k_ZERO_EXP                   },
            { L_,  k_U_HALF_8_BOUNDARY_MAX,  k_U_HALF_8_BOUNDARY_MAX_EXP  },
            { L_,  k_U_HALF_8_INVALID_MAX,   k_U_HALF_8_INVALID_MAX_EXP   },
            { L_,  k_U_HALF_16_BOUNDARY_MAX, k_U_HALF_16_BOUNDARY_MAX_EXP },
            { L_,  k_U_HALF_16_INVALID_MAX,  k_U_HALF_16_INVALID_MAX_EXP  },
            { L_,  k_U_HALF_32_BOUNDARY_MAX, k_U_HALF_32_BOUNDARY_MAX_EXP },
            { L_,  k_U_HALF_32_INVALID_MAX,  k_U_HALF_32_INVALID_MAX_EXP  },
            { L_,  k_U_HALF_64_BOUNDARY_MAX, k_U_HALF_64_BOUNDARY_MAX_EXP },
            { L_,  k_U_HALF_64_INVALID_MAX,  k_U_HALF_64_INVALID_MAX_EXP  },
        };
        const auto NUM_U_HALF_DATA = sizeof U_HALF_DATA / sizeof *U_HALF_DATA;

        for (size_t i = 0; i < NUM_U_HALF_DATA; i++) {
            const int               LINE        = U_HALF_DATA[i].d_line;
            const Obj&              INTERVAL    = U_HALF_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_HALF_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<half>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                       SECONDS                         |
        // +------------+----------------------------+-------------+
        // |            |          SECONDS           | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | int8 max   |                        127 |             |
        // | int8 min   |                       -128 |             |
        // +------------+----------------------------+-------------+
        // | int16 max  |                     32 767 |             |
        // | int16 min  |                    -32 768 |             |
        // +------------+----------------------------+-------------+
        // | int32 max  |              2 147 483 647 |             |
        // | int32 min  |             -2 147 483 648 |             |
        // +------------+----------------------------+-------------+
        // | int64 max  |  9 223 372 036 854 775 807 |             |
        // | int64 min  | -9 223 372 036 854 775 808 |             |
        // +------------+----------------------------+-------------+

        const Obj k_ONE_8_BOUNDARY_MAX(   127,         0         );
        const Obj k_ONE_8_BOUNDARY_MIN(  -128,         0         );
        const Obj k_ONE_8_INVALID_MAX(    127,         1         );
        const Obj k_ONE_8_INVALID_MIN(   -128,        -1         );

        const Obj k_ONE_16_BOUNDARY_MAX(  32767,       0         );
        const Obj k_ONE_16_BOUNDARY_MIN( -32768,       0         );
        const Obj k_ONE_16_INVALID_MAX(   32767,       1         );
        const Obj k_ONE_16_INVALID_MIN(  -32768,      -1         );

        const Obj k_ONE_32_BOUNDARY_MAX(  2147483647,  0         );
        const Obj k_ONE_32_BOUNDARY_MIN = isRealLongDouble
                                  ? Obj( -2147483648,  0         )
                                  : Obj( -2147483647,  256       );

        const Obj k_ONE_32_INVALID_MAX = isRealLongDouble
                                  ? Obj(  2147483647,  1         )
                                  : Obj(  2147483647,  256       );
        const Obj k_ONE_32_INVALID_MIN = isRealLongDouble
                                  ? Obj( -2147483648, -1         )
                                  : Obj( -2147483648, -256       );

        const Obj k_ONE_64_BOUNDARY_MAX( LLONG_MAX,    0         );
        const Obj k_ONE_64_BOUNDARY_MIN( LLONG_MIN,    0         );

        const Obj k_ONE_64_INVALID_MAX(  LLONG_MAX,    999999999 );
        const Obj k_ONE_64_INVALID_MIN(  LLONG_MIN,   -999999999 );

        //       NAME                       EXP_8   EXP_16  EXP_32  EXP_64
        //       -------------------------- ------- ------  ------  ------
       const RCR k_ONE_64_INVALID_MIN_EXP = isRealLongDouble
                                    ? RCR  { false,  false,  false, false }
                                    : RCR  { false,  false,  false,  true };

       const RCR k_ONE_64_BOUNDARY_MIN_EXP { false,  false,  false,  true };
       const RCR k_ONE_32_INVALID_MIN_EXP  { false,  false,  false,  true };
       const RCR k_ONE_32_BOUNDARY_MIN_EXP { false,  false,   true,  true };
       const RCR k_ONE_16_INVALID_MIN_EXP  { false,  false,   true,  true };
       const RCR k_ONE_16_BOUNDARY_MIN_EXP { false,   true,   true,  true };
       const RCR k_ONE_8_INVALID_MIN_EXP   { false,   true,   true,  true };
       const RCR k_ONE_8_BOUNDARY_MIN_EXP  {  true,   true,   true,  true };
       const RCR k_ONE_8_BOUNDARY_MAX_EXP  {  true,   true,   true,  true };
       const RCR k_ONE_8_INVALID_MAX_EXP   { false,   true,   true,  true };
       const RCR k_ONE_16_BOUNDARY_MAX_EXP { false,   true,   true,  true };
       const RCR k_ONE_16_INVALID_MAX_EXP  { false,  false,   true,  true };
       const RCR k_ONE_32_BOUNDARY_MAX_EXP { false,  false,   true,  true };
       const RCR k_ONE_32_INVALID_MAX_EXP  { false,  false,  false,  true };
       const RCR k_ONE_64_BOUNDARY_MAX_EXP { false,  false,  false,  true };
       const RCR k_ONE_64_INVALID_MAX_EXP   = isRealLongDouble
                                    ? RCR  { false,  false,  false, false }
                                    : RCR  { false,  false,  false,  true };

        const Data ONE_DATA[] = {
            //LINE INTERVAL                  EXPECTED RESULTS
            //---- -----------------------   ---------------------------
            { L_,  k_ONE_64_INVALID_MIN,     k_ONE_64_INVALID_MIN_EXP   },
            { L_,  k_ONE_64_BOUNDARY_MIN,    k_ONE_64_BOUNDARY_MIN_EXP  },
            { L_,  k_ONE_32_INVALID_MIN,     k_ONE_32_INVALID_MIN_EXP   },
            { L_,  k_ONE_32_BOUNDARY_MIN,    k_ONE_32_BOUNDARY_MIN_EXP  },
            { L_,  k_ONE_16_INVALID_MIN,     k_ONE_16_INVALID_MIN_EXP   },
            { L_,  k_ONE_16_BOUNDARY_MIN,    k_ONE_16_BOUNDARY_MIN_EXP  },
            { L_,  k_ONE_8_INVALID_MIN,      k_ONE_8_INVALID_MIN_EXP    },
            { L_,  k_ONE_8_BOUNDARY_MIN,     k_ONE_8_BOUNDARY_MIN_EXP   },
            { L_,  k_ZERO,                   k_ZERO_EXP                 },
            { L_,  k_ONE_8_BOUNDARY_MAX,     k_ONE_8_BOUNDARY_MAX_EXP   },
            { L_,  k_ONE_8_INVALID_MAX,      k_ONE_8_INVALID_MAX_EXP    },
            { L_,  k_ONE_16_BOUNDARY_MAX,    k_ONE_16_BOUNDARY_MAX_EXP  },
            { L_,  k_ONE_16_INVALID_MAX,     k_ONE_16_INVALID_MAX_EXP   },
            { L_,  k_ONE_32_BOUNDARY_MAX,    k_ONE_32_BOUNDARY_MAX_EXP  },
            { L_,  k_ONE_32_INVALID_MAX,     k_ONE_32_INVALID_MAX_EXP   },
            { L_,  k_ONE_64_BOUNDARY_MAX,    k_ONE_64_BOUNDARY_MAX_EXP  },
            { L_,  k_ONE_64_INVALID_MAX,     k_ONE_64_INVALID_MAX_EXP   },
        };
        const auto NUM_ONE_DATA = sizeof ONE_DATA / sizeof *ONE_DATA;

        for (size_t i = 0; i < NUM_ONE_DATA; i++) {
            const int               LINE        = ONE_DATA[i].d_line;
            const Obj&              INTERVAL    = ONE_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = ONE_DATA[i].d_expected;

            verifyDurationRangeCheck<one>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                      SECONDS                          |
        // +------------+----------------------------+-------------+
        // |            |         SECONDS            | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | uint8 max  |                        255 |             |
        // +------------+----------------------------+-------------+
        // | uint16 max |                     65 535 |             |
        // +------------+----------------------------+-------------+
        // | uint32 max |              4 294 967 295 |             |
        // +------------+----------------------------+-------------+
        // | uint64 max | 18 446 744 073 709 551 615 |             |
        // +------------+----------------------------+-------------+

        const Obj k_U_ONE_8_BOUNDARY_MAX(  255,        0         );
        const Obj k_U_ONE_8_INVALID_MAX(   255,        1         );

        const Obj k_U_ONE_16_BOUNDARY_MAX( 65535,      0         );
        const Obj k_U_ONE_16_INVALID_MAX(  65535,      1         );

        const Obj k_U_ONE_32_BOUNDARY_MAX( 4294967295, 0         );
        const Obj k_U_ONE_32_INVALID_MAX = isRealLongDouble
                                    ? Obj( 4294967295, 1         )
                                    : Obj( 4294967295, 256       );

        const Obj k_U_ONE_64_BOUNDARY_MAX( LLONG_MAX,  999999999 );

        const Data U_ONE_DATA[] = {
            //LINE INTERVAL                   EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------    ------- ------  ------  ------
            { L_,  k_ZERO,                  {  true,   true,   true,  true } },
            { L_,  k_U_ONE_8_BOUNDARY_MAX,  {  true,   true,   true,  true } },
            { L_,  k_U_ONE_8_INVALID_MAX,   { false,   true,   true,  true } },
            { L_,  k_U_ONE_16_BOUNDARY_MAX, { false,   true,   true,  true } },
            { L_,  k_U_ONE_16_INVALID_MAX,  { false,  false,   true,  true } },
            { L_,  k_U_ONE_32_BOUNDARY_MAX, { false,  false,   true,  true } },
            { L_,  k_U_ONE_32_INVALID_MAX,  { false,  false,  false,  true } },
            { L_,  k_U_ONE_64_BOUNDARY_MAX, { false,  false,  false,  true } },
        };
        const auto NUM_U_ONE_DATA = sizeof U_ONE_DATA / sizeof *U_ONE_DATA;

        for (size_t i = 0; i < NUM_U_ONE_DATA; i++) {
            const int               LINE        = U_ONE_DATA[i].d_line;
            const Obj&              INTERVAL    = U_ONE_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_ONE_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<one>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                       TWO SECONDS                     |
        // +------------+----------------------------+-------------+
        // |            |          SECONDS           | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | int8 max   |                        127 |             |
        // | int8 min   |                       -128 |             |
        // +------------+----------------------------+-------------+
        // | int16 max  |                     32 767 |             |
        // | int16 min  |                    -32 768 |             |
        // +------------+----------------------------+-------------+
        // | int32 max  |              2 147 483 647 |             |
        // | int32 min  |             -2 147 483 648 |             |
        // +------------+----------------------------+-------------+
        // | int64 max  |  9 223 372 036 854 775 807 |             |
        // | int64 min  | -9 223 372 036 854 775 808 |             |
        // +------------+----------------------------+-------------+

        const Obj k_TWO_8_BOUNDARY_MAX(   254,         0         );
        const Obj k_TWO_8_BOUNDARY_MIN(  -256,         0         );
        const Obj k_TWO_8_INVALID_MAX(    254,         1         );
        const Obj k_TWO_8_INVALID_MIN(   -256,        -1         );

        const Obj k_TWO_16_BOUNDARY_MAX(  65534,       0         );
        const Obj k_TWO_16_BOUNDARY_MIN( -65536,       0         );
        const Obj k_TWO_16_INVALID_MAX(   65534,       1         );
        const Obj k_TWO_16_INVALID_MIN(  -65536,      -1         );

        const Obj k_TWO_32_BOUNDARY_MAX(  4294967294,  0         );
        const Obj k_TWO_32_BOUNDARY_MIN( -4294967296,  0         );

        const Obj k_TWO_32_INVALID_MAX = isRealLongDouble
                                  ? Obj(  4294967294,  1         )
                                  : Obj(  4294967294,  512       );
        const Obj k_TWO_32_INVALID_MIN = isRealLongDouble
                                  ? Obj( -4294967296, -1         )
                                  : Obj( -4294967296, -1024      );

        const Obj k_TWO_64_BOUNDARY_MAX(  LLONG_MAX,   999999999 );
        const Obj k_TWO_64_BOUNDARY_MIN(  LLONG_MIN,   999999999 );


        const Data TWO_DATA[] = {
            //LINE INTERVAL                EXP_8   EXP_16  EXP_32  EXP_64
            //---- ----------------------- ------- ------  ------  ------
            { L_,  k_TWO_64_BOUNDARY_MIN,{ false,  false,  false,  true } },
            { L_,  k_TWO_32_INVALID_MIN, { false,  false,  false,  true } },
            { L_,  k_TWO_32_BOUNDARY_MIN,{ false,  false,   true,  true } },
            { L_,  k_TWO_16_INVALID_MIN, { false,  false,   true,  true } },
            { L_,  k_TWO_16_BOUNDARY_MIN,{ false,   true,   true,  true } },
            { L_,  k_TWO_8_INVALID_MIN,  { false,   true,   true,  true } },
            { L_,  k_TWO_8_BOUNDARY_MIN, {  true,   true,   true,  true } },
            { L_,  k_ZERO,               {  true,   true,   true,  true } },
            { L_,  k_TWO_8_BOUNDARY_MAX, {  true,   true,   true,  true } },
            { L_,  k_TWO_8_INVALID_MAX,  { false,   true,   true,  true } },
            { L_,  k_TWO_16_BOUNDARY_MAX,{ false,   true,   true,  true } },
            { L_,  k_TWO_16_INVALID_MAX, { false,  false,   true,  true } },
            { L_,  k_TWO_32_BOUNDARY_MAX,{ false,  false,   true,  true } },
            { L_,  k_TWO_32_INVALID_MAX, { false,  false,  false,  true } },
            { L_,  k_TWO_64_BOUNDARY_MAX,{ false,  false,  false,  true } },
        };
        const auto NUM_TWO_DATA = sizeof TWO_DATA / sizeof *TWO_DATA;

        for (size_t i = 0; i < NUM_TWO_DATA; i++) {
            const int               LINE        = TWO_DATA[i].d_line;
            const Obj&              INTERVAL    = TWO_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = TWO_DATA[i].d_expected;

            verifyDurationRangeCheck<two>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                    TWO SECONDS                        |
        // +------------+----------------------------+-------------+
        // |            |         SECONDS            | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | uint8 max  |                        255 |             |
        // +------------+----------------------------+-------------+
        // | uint16 max |                     65 535 |             |
        // +------------+----------------------------+-------------+
        // | uint32 max |              4 294 967 295 |             |
        // +------------+----------------------------+-------------+
        // | uint64 max | 18 446 744 073 709 551 615 |             |
        // +------------+----------------------------+-------------+

        const Obj k_U_TWO_8_BOUNDARY_MAX(  510,        0        );
        const Obj k_U_TWO_8_INVALID_MAX(   510,        1        );

        const Obj k_U_TWO_16_BOUNDARY_MAX( 131070,     0        );
        const Obj k_U_TWO_16_INVALID_MAX(  131070,     1        );

        const Obj k_U_TWO_32_BOUNDARY_MAX( 8589934590, 0        );
        const Obj k_U_TWO_32_INVALID_MAX = isRealLongDouble
                                    ? Obj( 8589934590, 1        )
                                    : Obj( 8589934590, 1024     );


        const Obj k_U_TWO_64_BOUNDARY_MAX( LLONG_MAX,  999999999);

        const Data U_TWO_DATA[] = {
            //LINE INTERVAL                  EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------   ------- ------  ------  ------
            { L_,  k_ZERO,                 {  true,   true,   true,  true } },
            { L_,  k_U_TWO_8_BOUNDARY_MAX, {  true,   true,   true,  true } },
            { L_,  k_U_TWO_8_INVALID_MAX,  { false,   true,   true,  true } },
            { L_,  k_U_TWO_16_BOUNDARY_MAX,{ false,   true,   true,  true } },
            { L_,  k_U_TWO_16_INVALID_MAX, { false,  false,   true,  true } },
            { L_,  k_U_TWO_32_BOUNDARY_MAX,{ false,  false,   true,  true } },
            { L_,  k_U_TWO_32_INVALID_MAX, { false,  false,  false,  true } },
            { L_,  k_U_TWO_64_BOUNDARY_MAX,{ false,  false,  false,  true } },
        };
        const auto NUM_U_TWO_DATA = sizeof U_TWO_DATA / sizeof *U_TWO_DATA;

        for (size_t i = 0; i < NUM_U_TWO_DATA; i++) {
            const int               LINE        = U_TWO_DATA[i].d_line;
            const Obj&              INTERVAL    = U_TWO_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_TWO_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<two>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                  THOUSAND SECONDS                     |
        // +------------+----------------------------+-------------+
        // |            |          SECONDS           | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | int8 max   |                        127 |             |
        // | int8 min   |                       -128 |             |
        // +------------+----------------------------+-------------+
        // | int16 max  |                     32 767 |             |
        // | int16 min  |                    -32 768 |             |
        // +------------+----------------------------+-------------+
        // | int32 max  |              2 147 483 647 |             |
        // | int32 min  |             -2 147 483 648 |             |
        // +------------+----------------------------+-------------+
        // | int64 max  |  9 223 372 036 854 775 807 |             |
        // | int64 min  | -9 223 372 036 854 775 808 |             |
        // +------------+----------------------------+-------------+

        const Obj k_KILO_8_BOUNDARY_MAX(   127000,         0 );
        const Obj k_KILO_8_BOUNDARY_MIN(  -128000,         0 );
        const Obj k_KILO_8_INVALID_MAX(    127000,         1 );
        const Obj k_KILO_8_INVALID_MIN(   -128000,        -1 );

        const Obj k_KILO_16_BOUNDARY_MAX(  32767000,       0 );
        const Obj k_KILO_16_BOUNDARY_MIN( -32768000,       0 );

        const Obj k_KILO_16_INVALID_MAX = isRealLongDouble
                                   ? Obj(  32767000,       1 )
                                   : Obj(  32767000,       4 );
        const Obj k_KILO_16_INVALID_MIN = isRealLongDouble
                                   ? Obj( -32768000,      -1 )
                                   : Obj( -32768000,      -8 );

        const Obj k_KILO_32_BOUNDARY_MAX(  2147483647000,  0 );
        const Obj k_KILO_32_BOUNDARY_MIN( -2147483648000,  0 );
        const Obj k_KILO_32_INVALID_MAX(   2147483647001,  0 );
        const Obj k_KILO_32_INVALID_MIN(  -2147483648001,  0 );

        const Obj k_KILO_64_BOUNDARY_MAX(  LLONG_MAX,      0 );
        const Obj k_KILO_64_BOUNDARY_MIN(  LLONG_MIN,      0 );

        const Data KILO_DATA[] = {
            //LINE INTERVAL                 EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------  ------- ------  ------  ------
            { L_,  k_KILO_64_BOUNDARY_MIN, { false,  false,  false,  true } },
            { L_,  k_KILO_32_INVALID_MIN,  { false,  false,  false,  true } },
            { L_,  k_KILO_32_BOUNDARY_MIN, { false,  false,   true,  true } },
            { L_,  k_KILO_16_INVALID_MIN,  { false,  false,   true,  true } },
            { L_,  k_KILO_16_BOUNDARY_MIN, { false,   true,   true,  true } },
            { L_,  k_KILO_8_INVALID_MIN,   { false,   true,   true,  true } },
            { L_,  k_KILO_8_BOUNDARY_MIN,  {  true,   true,   true,  true } },
            { L_,  k_ZERO,                 {  true,   true,   true,  true } },
            { L_,  k_KILO_8_BOUNDARY_MAX,  {  true,   true,   true,  true } },
            { L_,  k_KILO_8_INVALID_MAX,   { false,   true,   true,  true } },
            { L_,  k_KILO_16_BOUNDARY_MAX, { false,   true,   true,  true } },
            { L_,  k_KILO_16_INVALID_MAX,  { false,  false,   true,  true } },
            { L_,  k_KILO_32_BOUNDARY_MAX, { false,  false,   true,  true } },
            { L_,  k_KILO_32_INVALID_MAX,  { false,  false,  false,  true } },
            { L_,  k_KILO_64_BOUNDARY_MAX, { false,  false,  false,  true } },
        };
        const auto NUM_KILO_DATA = sizeof KILO_DATA / sizeof *KILO_DATA;

        for (size_t i = 0; i < NUM_KILO_DATA; i++) {
            const int               LINE        = KILO_DATA[i].d_line;
            const Obj&              INTERVAL    = KILO_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = KILO_DATA[i].d_expected;

            verifyDurationRangeCheck<kilo>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                     THOUSAND SECONDS                  |
        // +------------+----------------------------+-------------+
        // |            |         SECONDS            | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | uint8 max  |                        255 |             |
        // +------------+----------------------------+-------------+
        // | uint16 max |                     65 535 |             |
        // +------------+----------------------------+-------------+
        // | uint32 max |              4 294 967 295 |             |
        // +------------+----------------------------+-------------+
        // | uint64 max | 18 446 744 073 709 551 615 |             |
        // +------------+----------------------------+-------------+

        const Obj k_U_KILO_8_BOUNDARY_MAX(  255000,        0        );
        const Obj k_U_KILO_8_INVALID_MAX(   255000,        1        );

        const Obj k_U_KILO_16_BOUNDARY_MAX( 65535000,      0        );
        const Obj k_U_KILO_16_INVALID_MAX = isRealLongDouble
                                     ? Obj( 65535000,      1        )
                                     : Obj( 65535000,      8        );

        const Obj k_U_KILO_32_BOUNDARY_MAX( 4294967295000, 0        );
        const Obj k_U_KILO_32_INVALID_MAX(  4294967295001, 0        );

        const Obj k_U_KILO_64_BOUNDARY_MAX( LLONG_MAX,     999999999);

        const Data U_KILO_DATA[] = {
            //LINE INTERVAL                  EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------   ------- ------  ------  ------
            { L_,  k_ZERO,                  {  true,   true,   true,  true } },
            { L_,  k_U_KILO_8_BOUNDARY_MAX, {  true,   true,   true,  true } },
            { L_,  k_U_KILO_8_INVALID_MAX,  { false,   true,   true,  true } },
            { L_,  k_U_KILO_16_BOUNDARY_MAX,{ false,   true,   true,  true } },
            { L_,  k_U_KILO_16_INVALID_MAX, { false,  false,   true,  true } },
            { L_,  k_U_KILO_32_BOUNDARY_MAX,{ false,  false,   true,  true } },
            { L_,  k_U_KILO_32_INVALID_MAX, { false,  false,  false,  true } },
            { L_,  k_U_KILO_64_BOUNDARY_MAX,{ false,  false,  false,  true } },
        };
        const auto NUM_U_KILO_DATA = sizeof U_KILO_DATA / sizeof *U_KILO_DATA;

        for (size_t i = 0; i < NUM_U_KILO_DATA; i++) {
            const int               LINE        = U_KILO_DATA[i].d_line;
            const Obj&              INTERVAL    = U_KILO_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_KILO_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<kilo>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                  MILLION OF SECONDS                   |
        // +------------+----------------------------+-------------+
        // |            |          SECONDS           | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | int8 max   |                        127 |             |
        // | int8 min   |                       -128 |             |
        // +------------+----------------------------+-------------+
        // | int16 max  |                     32 767 |             |
        // | int16 min  |                    -32 768 |             |
        // +------------+----------------------------+-------------+
        // | int32 max  |              2 147 483 647 |             |
        // | int32 min  |             -2 147 483 648 |             |
        // +------------+----------------------------+-------------+
        // | int64 max  |  9 223 372 036 854 775 807 |             |
        // | int64 min  | -9 223 372 036 854 775 808 |             |
        // +------------+----------------------------+-------------+

        const Obj k_MEGA_8_BOUNDARY_MAX(   127000000,         0  );
        const Obj k_MEGA_8_BOUNDARY_MIN(  -128000000,         0  );

        const Obj k_MEGA_8_INVALID_MAX = isRealLongDouble
                                  ? Obj(   127000000,         1  )
                                  : Obj(   127000000,         16 );
        const Obj k_MEGA_8_INVALID_MIN = isRealLongDouble
                                  ? Obj(  -128000000,        -1  )
                                  : Obj(  -128000000,        -32 );

        const Obj k_MEGA_16_BOUNDARY_MAX(  32767000000,       0  );
        const Obj k_MEGA_16_BOUNDARY_MIN( -32768000000,       0  );
        const Obj k_MEGA_16_INVALID_MAX(   32767000001,       0  );
        const Obj k_MEGA_16_INVALID_MIN(  -32768000001,       0  );

        const Obj k_MEGA_32_BOUNDARY_MAX(  2147483647000000,  0  );
        const Obj k_MEGA_32_BOUNDARY_MIN( -2147483648000000,  0  );
        const Obj k_MEGA_32_INVALID_MAX(   2147483647000001,  0  );
        const Obj k_MEGA_32_INVALID_MIN(  -2147483648000001,  0  );

        const Obj k_MEGA_64_BOUNDARY_MAX(  LLONG_MAX,         0  );
        const Obj k_MEGA_64_BOUNDARY_MIN(  LLONG_MIN,         0  );

        const Data MEGA_DATA[] = {
            //LINE INTERVAL                 EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------  ------- ------  ------  ------
            { L_,  k_MEGA_64_BOUNDARY_MIN, { false,  false,  false,  true } },
            { L_,  k_MEGA_32_INVALID_MIN,  { false,  false,  false,  true } },
            { L_,  k_MEGA_32_BOUNDARY_MIN, { false,  false,   true,  true } },
            { L_,  k_MEGA_16_INVALID_MIN,  { false,  false,   true,  true } },
            { L_,  k_MEGA_16_BOUNDARY_MIN, { false,   true,   true,  true } },
            { L_,  k_MEGA_8_INVALID_MIN,   { false,   true,   true,  true } },
            { L_,  k_MEGA_8_BOUNDARY_MIN,  {  true,   true,   true,  true } },
            { L_,  k_ZERO,                 {  true,   true,   true,  true } },
            { L_,  k_MEGA_8_BOUNDARY_MAX,  {  true,   true,   true,  true } },
            { L_,  k_MEGA_8_INVALID_MAX,   { false,   true,   true,  true } },
            { L_,  k_MEGA_16_BOUNDARY_MAX, { false,   true,   true,  true } },
            { L_,  k_MEGA_16_INVALID_MAX,  { false,  false,   true,  true } },
            { L_,  k_MEGA_32_BOUNDARY_MAX, { false,  false,   true,  true } },
            { L_,  k_MEGA_32_INVALID_MAX,  { false,  false,  false,  true } },
            { L_,  k_MEGA_64_BOUNDARY_MAX, { false,  false,  false,  true } },
        };
        const auto NUM_MEGA_DATA = sizeof MEGA_DATA / sizeof *MEGA_DATA;

        for (size_t i = 0; i < NUM_MEGA_DATA; i++) {
            const int               LINE        = MEGA_DATA[i].d_line;
            const Obj&              INTERVAL    = MEGA_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = MEGA_DATA[i].d_expected;

            verifyDurationRangeCheck<mega>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                  MILLION OF SECONDS                   |
        // +------------+----------------------------+-------------+
        // |            |         SECONDS            | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | uint8 max  |                        255 |             |
        // +------------+----------------------------+-------------+
        // | uint16 max |                     65 535 |             |
        // +------------+----------------------------+-------------+
        // | uint32 max |              4 294 967 295 |             |
        // +------------+----------------------------+-------------+
        // | uint64 max | 18 446 744 073 709 551 615 |             |
        // +------------+----------------------------+-------------+

        const Obj k_U_MEGA_8_BOUNDARY_MAX(  255000000,        0        );
        const Obj k_U_MEGA_8_INVALID_MAX = isRealLongDouble
                                    ? Obj(  255000000,        1        )
                                    : Obj(  255000000,        32       );

        const Obj k_U_MEGA_16_BOUNDARY_MAX( 65535000000,      0        );
        const Obj k_U_MEGA_16_INVALID_MAX(  65535000001,      0        );

        const Obj k_U_MEGA_32_BOUNDARY_MAX( 4294967295000000, 0        );
        const Obj k_U_MEGA_32_INVALID_MAX(  4294967295000001, 0        );

        const Obj k_U_MEGA_64_BOUNDARY_MAX( LLONG_MAX,        999999999);

        const Data U_MEGA_DATA[] = {
            //LINE INTERVAL                  EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------   ------- ------  ------  ------
            { L_,  k_ZERO,                  {  true,   true,   true,  true } },
            { L_,  k_U_MEGA_8_BOUNDARY_MAX, {  true,   true,   true,  true } },
            { L_,  k_U_MEGA_8_INVALID_MAX,  { false,   true,   true,  true } },
            { L_,  k_U_MEGA_16_BOUNDARY_MAX,{ false,   true,   true,  true } },
            { L_,  k_U_MEGA_16_INVALID_MAX, { false,  false,   true,  true } },
            { L_,  k_U_MEGA_32_BOUNDARY_MAX,{ false,  false,   true,  true } },
            { L_,  k_U_MEGA_32_INVALID_MAX, { false,  false,  false,  true } },
            { L_,  k_U_MEGA_64_BOUNDARY_MAX,{ false,  false,  false,  true } },
        };
        const auto NUM_U_MEGA_DATA = sizeof U_MEGA_DATA / sizeof *U_MEGA_DATA;

        for (size_t i = 0; i < NUM_U_MEGA_DATA; i++) {
            const int               LINE        = U_MEGA_DATA[i].d_line;
            const Obj&              INTERVAL    = U_MEGA_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_MEGA_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<mega>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                  BILLION OF SECONDS                   |
        // +------------+----------------------------+-------------+
        // |            |          SECONDS           | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | int8 max   |                        127 |             |
        // | int8 min   |                       -128 |             |
        // +------------+----------------------------+-------------+
        // | int16 max  |                     32 767 |             |
        // | int16 min  |                    -32 768 |             |
        // +------------+----------------------------+-------------+
        // | int32 max  |              2 147 483 647 |             |
        // | int32 min  |             -2 147 483 648 |             |
        // +------------+----------------------------+-------------+
        // | int64 max  |  9 223 372 036 854 775 807 |             |
        // | int64 min  | -9 223 372 036 854 775 808 |             |
        // +------------+----------------------------+-------------+

        const Obj k_GIGA_8_BOUNDARY_MAX(   127000000000,        0  );
        const Obj k_GIGA_8_BOUNDARY_MIN(  -128000000000,        0  );
        const Obj k_GIGA_8_INVALID_MAX(    127000000001,        0  );
        const Obj k_GIGA_8_INVALID_MIN(   -128000000001,        0  );

        const Obj k_GIGA_16_BOUNDARY_MAX(  32767000000000,      0 );
        const Obj k_GIGA_16_BOUNDARY_MIN( -32768000000000,      0 );
        const Obj k_GIGA_16_INVALID_MAX(   32767000000001,      0 );
        const Obj k_GIGA_16_INVALID_MIN(  -32768000000001,      0 );

        const Obj k_GIGA_32_BOUNDARY_MAX(  2147483647000000000, 0 );
        const Obj k_GIGA_32_BOUNDARY_MIN( -2147483648000000000, 0 );

        const Obj k_GIGA_32_INVALID_MAX = isRealLongDouble
                                   ? Obj(  2147483647000000001, 0 )
                                   : Obj(  2147483647000000256, 0 );
        const Obj k_GIGA_32_INVALID_MIN = isRealLongDouble
                                   ? Obj( -2147483648000000001, 0 )
                                   : Obj( -2147483648000000512, 0 );

        const Obj k_GIGA_64_BOUNDARY_MAX(  LLONG_MAX,           0 );
        const Obj k_GIGA_64_BOUNDARY_MIN(  LLONG_MIN,           0 );

        const Data GIGA_DATA[] = {
            //LINE INTERVAL                 EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------  ------- ------  ------  ------
            { L_,  k_GIGA_64_BOUNDARY_MIN, { false,  false,  false,  true } },
            { L_,  k_GIGA_32_INVALID_MIN,  { false,  false,  false,  true } },
            { L_,  k_GIGA_32_BOUNDARY_MIN, { false,  false,   true,  true } },
            { L_,  k_GIGA_16_INVALID_MIN,  { false,  false,   true,  true } },
            { L_,  k_GIGA_16_BOUNDARY_MIN, { false,   true,   true,  true } },
            { L_,  k_GIGA_8_INVALID_MIN,   { false,   true,   true,  true } },
            { L_,  k_GIGA_8_BOUNDARY_MIN,  {  true,   true,   true,  true } },
            { L_,  k_ZERO,                 {  true,   true,   true,  true } },
            { L_,  k_GIGA_8_BOUNDARY_MAX,  {  true,   true,   true,  true } },
            { L_,  k_GIGA_8_INVALID_MAX,   { false,   true,   true,  true } },
            { L_,  k_GIGA_16_BOUNDARY_MAX, { false,   true,   true,  true } },
            { L_,  k_GIGA_16_INVALID_MAX,  { false,  false,   true,  true } },
            { L_,  k_GIGA_32_BOUNDARY_MAX, { false,  false,   true,  true } },
            { L_,  k_GIGA_32_INVALID_MAX,  { false,  false,  false,  true } },
            { L_,  k_GIGA_64_BOUNDARY_MAX, { false,  false,  false,  true } },
        };
        const auto NUM_GIGA_DATA = sizeof GIGA_DATA / sizeof *GIGA_DATA;

        for (size_t i = 0; i < NUM_GIGA_DATA; i++) {
            const int               LINE        = GIGA_DATA[i].d_line;
            const Obj&              INTERVAL    = GIGA_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = GIGA_DATA[i].d_expected;

            verifyDurationRangeCheck<giga>(LINE, INTERVAL, EXPECTED);
        }

        // +-------------------------------------------------------+
        // |                  BILLION OF SECONDS                   |
        // +------------+----------------------------+-------------+
        // |            |         SECONDS            | NANOSECONDS |
        // +------------+----------------------------+-------------+
        // | uint8 max  |                        255 |             |
        // +------------+----------------------------+-------------+
        // | uint16 max |                     65 535 |             |
        // +------------+----------------------------+-------------+
        // | uint32 max |              4 294 967 295 |             |
        // +------------+----------------------------+-------------+
        // | uint64 max | 18 446 744 073 709 551 615 |             |
        // +------------+----------------------------+-------------+

        const Obj k_U_GIGA_8_BOUNDARY_MAX(  255000000000,        0        );
        const Obj k_U_GIGA_8_INVALID_MAX(   255000000001,        0        );

        const Obj k_U_GIGA_16_BOUNDARY_MAX( 65535000000000,      0        );
        const Obj k_U_GIGA_16_INVALID_MAX(  65535000000001,      0        );

        const Obj k_U_GIGA_32_BOUNDARY_MAX( 4294967295000000000, 0        );
        const Obj k_U_GIGA_32_INVALID_MAX = isRealLongDouble
                                     ? Obj( 4294967295000000001, 0        )
                                     : Obj( 4294967295000000512, 0        );

        const Obj k_U_GIGA_64_BOUNDARY_MAX( LLONG_MAX,           999999999);

        const Data U_GIGA_DATA[] = {
            //LINE INTERVAL                  EXP_8   EXP_16  EXP_32  EXP_64
            //---- -----------------------   ------- ------  ------  ------
            { L_,  k_ZERO,                  {  true,   true,   true,  true } },
            { L_,  k_U_GIGA_8_BOUNDARY_MAX, {  true,   true,   true,  true } },
            { L_,  k_U_GIGA_8_INVALID_MAX,  { false,   true,   true,  true } },
            { L_,  k_U_GIGA_16_BOUNDARY_MAX,{ false,   true,   true,  true } },
            { L_,  k_U_GIGA_16_INVALID_MAX, { false,  false,   true,  true } },
            { L_,  k_U_GIGA_32_BOUNDARY_MAX,{ false,  false,   true,  true } },
            { L_,  k_U_GIGA_32_INVALID_MAX, { false,  false,  false,  true } },
            { L_,  k_U_GIGA_64_BOUNDARY_MAX,{ false,  false,  false,  true } },
        };
        const auto NUM_U_GIGA_DATA = sizeof U_GIGA_DATA / sizeof *U_GIGA_DATA;

        for (size_t i = 0; i < NUM_U_GIGA_DATA; i++) {
            const int               LINE        = U_GIGA_DATA[i].d_line;
            const Obj&              INTERVAL    = U_GIGA_DATA[i].d_interval;
            const RangeCheckResults EXPECTED    = U_GIGA_DATA[i].d_expected;

            verifyDurationRangeCheckUnsigned<giga>(LINE, INTERVAL, EXPECTED);
        }
#else
        if (verbose) printf("'std::chrono' is not supported\n");
#endif
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING 'isValid(duration)'
        //
        // NOTE THAT this first implementation of 'chrono::duration'
        // interoperability does not support durations with floating point
        // representations, therefore we test integral types only.  Similarly,
        // there is no support for user defined arithmetic types (especially
        // those that would extend range beyond 'std::intmax_t'), so no tests
        // are present for those either.
        //
        // Concerns:
        //: 1 An 'isValid' instance function returns 'true' for any
        //:   'std::chrono::duration' object whose value can be represented
        //:   (even if not precisely) within the boundaries of the
        //:   'TimeInterval'.
        //:
        //: 2 Certain platforms do not provide an arithmetic type that is
        //:   capable of representing a 64 bit signed integer without loss of
        //:   precision. On such platforms we want range check on all in-bounds
        //:   'std::chrono::duration' values to succeed , and verify that there
        //:   *are* out-of-bounds values (if such values exist in the range of
        //:   the 'TimeInterval')  for which range checking using 'isValid'
        //:   fails.  We do that test to ensure that 'isValid' does not just
        //:   report 'true' for any 'std::chrono::duration' value.
        //:
        //: 3 'isValid' is parameterized as described in C-1 and C-2 with
        //:   'duration'types with all fundamental integral types as
        //:   representation. Therefore we will test C-1 and 2 concerns with
        //:   'durations' sporting 'char', 'short', 'int', 'long', 'long long'
        //:   representation types, as well as their 'unsigned' variants.
        //:
        //: 4 'isValid' is parameterized with thee 'duration' types having both
        //:   reasonable and unusual periods.  Note that C-3 and C-4 combine,
        //:   so we have a combinatorial "explosion" of tests. We aim to verify
        //:   that reasonable units (periods) work very well.  We aim to verify
        //:   that unusual unit, such as one third nanosecond work reasonably
        //:   well.
        //
        // Plan:
        //: 1 Use the table-based approach to specify a set of test values,
        //:   that are limits of integer types, expected border values and
        //:   duration values that are expected to be inconvertible to
        //:   'TimeInterval.
        //:
        //: 2 Iterate through integer types using them as representation for
        //:   duration types and through several common ratios and some
        //:   specific ratios (such as one third nanoseconds), create duration
        //:   objects with the values from P-1, call 'isValid' and verify the
        //:   results.  (C-1..4)
        //
        // Testing:
        //   bool isValid(const std::chrono::duration& duration);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'isValid(duration)'"
                            "\n===========================\n");

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS

        BSLS_KEYWORD_CONSTEXPR bool isRealLongDouble =
                                       sizeof( double) != sizeof( long double);
            // Some compilers (in particular MSVC) make 'long double' a synonym
            // for 'double'.  This leads to a loss of precision for large
            // values and we must take this into account in the testing
            // process.

        using third = std::ratio<1, 3>;  // third of a second ratio
        using half  = std::ratio<1, 2>;  // half of a second  ratio
        using one   = std::ratio<1>;     // one second        ratio
        using two   = std::ratio<2, 1>;  // two seconds       ratio

        const std::int64_t k_INT8_MIN =
                                       std::numeric_limits<std::int8_t>::min();
        const std::int64_t k_INT8_MAX =
                                       std::numeric_limits<std::int8_t>::max();
        const std::int64_t k_INT16_MIN =
                                      std::numeric_limits<std::int16_t>::min();
        const std::int64_t k_INT16_MAX =
                                      std::numeric_limits<std::int16_t>::max();
        const std::int64_t k_INT32_MIN =
                                      std::numeric_limits<std::int32_t>::min();
        const std::int64_t k_INT32_MAX =
                                      std::numeric_limits<std::int32_t>::max();
        const std::int64_t k_INT64_MIN =
                                      std::numeric_limits<std::int64_t>::min();
        const std::int64_t k_INT64_MAX =
                                      std::numeric_limits<std::int64_t>::max();

        const std::uint64_t k_UINT8_MAX =
                                      std::numeric_limits<std::uint8_t>::max();
        const std::uint64_t k_UINT16_MAX =
                                     std::numeric_limits<std::uint16_t>::max();
        const std::uint64_t k_UINT32_MAX =
                                     std::numeric_limits<std::uint32_t>::max();
        const std::uint64_t k_UINT64_MAX =
                                     std::numeric_limits<std::uint64_t>::max();

        struct Data {
            int               d_line;      // line number
            std::int64_t      d_count;     // duration value
            RangeCheckResults d_expected;  // expected results
        };

        struct UnsignedData {
            int               d_line;      // line number
            std::uint64_t     d_count;     // duration value
            RangeCheckResults d_expected;  // expected results
        };

        // Test tables contain counts of duration objects and expected results
        // of the range check for these objects. As we  only use integral
        // 'duration' representations we want to check their maximum
        // ('k_INT*_MAX') and minimum ('k_INT*_MIN') values. For duration types
        // with period less than a second, we check only these limits, because
        // any object of these types is expected to be able to be converted to
        // 'TimeInterval'.
        //
        // Duration types with period more than a second have objects with
        // values beyong the 'TimeInterval' boundaries. For such types we want
        // to check maximum (and minimum) value, that can be converted to
        // 'TimeInterval' ('k_*_BOUNDARY_MAX' and 'k_*_BOUNDARY_MIN'
        // respectively) and some reasonably close to them, but out-of-boundary
        // value that cannot ('k_*_INVALID_MAX' and 'k_*_INVALID_MIN'
        // respectively).  It can be difficult to specify exact first invalid
        // value, because of lack of precision for the big values and platform
        // differences.
        //
        // So 'k_TWO_BOUNDARY_MAX' - maximum positive value of
        //                           'std::chrono::duration' type with signed
        //                           integer representation and two seconds
        //                           period that can be converted to
        //                           'TimeInterval'
        //    'k_TWO_BOUNDARY_MIN' - minimum negative value of
        //                           'std::chrono::duration' type with signed
        //                           integer representation and two seconds
        //                           period that can be converted to
        //                           'TimeInterval'
        //    'k_TWO_INVALID_MAX'  - a too large out-of-boundary value
        //                           reasonably close to the
        //                           'k_TWO_BOUNDARY_MAX' value of
        //                           'std::chrono::duration' type with signed
        //                           integer representation and two seconds
        //                           period that can not be converted to
        //                           'TimeInterval'
        //    'k_TWO_INVALID_MIN'  - a too small out-of-boundary value
        //                           reasonably close to the
        //                           'k_TWO_BOUNDARY_MIN' value of
        //                           'std::chrono::duration' type with signed
        //                           integer representation and two seconds
        //                           period that can not be converted to
        //                           'TimeInterval'.

        const bool k_NA = false;
            // If the exact value cannot be represented with some type, the
            // test is not performed.  Such cases are marked as 'k_NA' in the
            // tables.  E.g. maximum value of type 'std::int16_t'
            // ('k_INT16_MAX') can not be stored in the variable of
            // 'std::int8_t', so the first element of the 'RangeCheckResults'
            // array for this variable containts 'k_NA' value.

        using RCR = RangeCheckResults;
            // Shortcut alias for 'RangeCheckResults' used to reduce length of
            // table rows.

        // The period and the representation type of a duration both affect the
        // range of time interval it can represent, therefore we need to
        // specify expected values for each tested representation tested within
        // testing a period.  I.e., the 'EXP_32' column in the table represents
        // the expected results of the range check for the exact value of
        // duration having signed integer representation with width of exactly
        // 32 bits.
        //
        // Any object of any duration type with integer representation and
        // period less than a second can be converted to 'TimeInterval' object.
        // Therefore we have single table for all such scenarios.

        //        NAME                   EXP_8   EXP_16   EXP_32  EXP_64
        //        --------------------   ------- ------   ------  ------
        const RCR k_ZERO_EXP           {  true,    true,   true,   true };

        const RCR k_LESS_INT8_MAX_EXP  {  true,    true,   true,   true };
        const RCR k_LESS_INT8_MIN_EXP  {  true,    true,   true,   true };
        const RCR k_LESS_INT16_MAX_EXP {  k_NA,    true,   true,   true };
        const RCR k_LESS_INT16_MIN_EXP {  k_NA,    true,   true,   true };
        const RCR k_LESS_INT32_MAX_EXP {  k_NA,    k_NA,   true,   true };
        const RCR k_LESS_INT32_MIN_EXP {  k_NA,    k_NA,   true,   true };
        const RCR k_LESS_INT64_MAX_EXP {  k_NA,    k_NA,   k_NA,   true };
        const RCR k_LESS_INT64_MIN_EXP {  k_NA,    k_NA,   k_NA,   true };

        const Data LESS_THAN_SECOND_DATA[] = {
            //LINE COUNT          EXPECTED RESULTS
            //---- ------------   --------------------
            { L_,  0,             k_ZERO_EXP           },
            { L_,  k_INT8_MAX,    k_LESS_INT8_MAX_EXP  },
            { L_,  k_INT8_MIN,    k_LESS_INT8_MIN_EXP  },
            { L_,  k_INT16_MAX,   k_LESS_INT16_MAX_EXP },
            { L_,  k_INT16_MIN,   k_LESS_INT16_MIN_EXP },
            { L_,  k_INT32_MAX,   k_LESS_INT32_MAX_EXP },
            { L_,  k_INT32_MIN,   k_LESS_INT32_MIN_EXP },
            { L_,  k_INT64_MAX,   k_LESS_INT64_MAX_EXP },
            { L_,  k_INT64_MIN,   k_LESS_INT64_MIN_EXP },
        };
        const auto NUM_LESS_THAN_SECOND_DATA =
                  sizeof LESS_THAN_SECOND_DATA / sizeof *LESS_THAN_SECOND_DATA;

        for (size_t i = 0; i < NUM_LESS_THAN_SECOND_DATA; i++) {
            const int          LINE     = LESS_THAN_SECOND_DATA[i].d_line;
            const std::int64_t COUNT    = LESS_THAN_SECOND_DATA[i].d_count;
            const RCR          EXPECTED = LESS_THAN_SECOND_DATA[i].d_expected;

            verifyTimeIntervalRangeCheck<pico >(LINE, COUNT, EXPECTED);
            verifyTimeIntervalRangeCheck<nano >(LINE, COUNT, EXPECTED);
            verifyTimeIntervalRangeCheck<micro>(LINE, COUNT, EXPECTED);
            verifyTimeIntervalRangeCheck<milli>(LINE, COUNT, EXPECTED);
            verifyTimeIntervalRangeCheck<half >(LINE, COUNT, EXPECTED);
        }

        //        NAME                    EXP_8    EXP_16   EXP_32  EXP_64
        //        ---------------         -------  ------   ------  ------
        const RCR k_LESS_UINT8_MAX_EXP  {  true,    true,    true,   true };
        const RCR k_LESS_UINT16_MAX_EXP {  k_NA,    true,    true,   true };
        const RCR k_LESS_UINT32_MAX_EXP {  k_NA,    k_NA,    true,   true };
        const RCR k_LESS_UINT64_MAX_EXP {  k_NA,    k_NA,    k_NA,   true };

        const UnsignedData U_LESS_THAN_SECOND_DATA[] = {
            //LINE COUNT          EXPECTED RESULTS
            //---- -------------- ---------------------
            { L_,  0,             k_ZERO_EXP            },
            { L_,  k_UINT8_MAX,   k_LESS_UINT8_MAX_EXP  },
            { L_,  k_UINT16_MAX,  k_LESS_UINT16_MAX_EXP },
            { L_,  k_UINT32_MAX,  k_LESS_UINT32_MAX_EXP },
            { L_,  k_UINT64_MAX,  k_LESS_UINT64_MAX_EXP },
        };
        const auto NUM_U_LESS_THAN_SECOND_DATA =
              sizeof U_LESS_THAN_SECOND_DATA / sizeof *U_LESS_THAN_SECOND_DATA;

        for (size_t i = 0; i < NUM_U_LESS_THAN_SECOND_DATA; i++) {
            const int           LINE     = U_LESS_THAN_SECOND_DATA[i].d_line;
            const std::uint64_t COUNT    = U_LESS_THAN_SECOND_DATA[i].d_count;
            const RCR           EXPECTED =
                                         U_LESS_THAN_SECOND_DATA[i].d_expected;

            verifyTimeIntervalRangeCheckUnsigned<pico >(LINE, COUNT, EXPECTED);
            verifyTimeIntervalRangeCheckUnsigned<nano >(LINE, COUNT, EXPECTED);
            verifyTimeIntervalRangeCheckUnsigned<micro>(LINE, COUNT, EXPECTED);
            verifyTimeIntervalRangeCheckUnsigned<milli>(LINE, COUNT, EXPECTED);
            verifyTimeIntervalRangeCheckUnsigned<third>(LINE, COUNT, EXPECTED);
            verifyTimeIntervalRangeCheckUnsigned<half >(LINE, COUNT, EXPECTED);
        }

        // One second period.

        const std::uint64_t k_U_ONE_BOUNDARY_MAX  = k_INT64_MAX;

        const std::uint64_t k_U_ONE_INVALID_MAX = isRealLongDouble
                                                 ? k_U_ONE_BOUNDARY_MAX + 2
                                                 : k_U_ONE_BOUNDARY_MAX + 2048;
            // Since MSVC does not support 'long double' type and uses 'double'
            // that has lower precision instead, 'isValid' can
            // give false positive results for 'std::chrono::duration' values,
            // that are expected to be (and actually are) unacceptable for
            // conversion to 'TimeInterval' object.  This only happens with
            // huge values.
            // To avoid this problem we had to calculate invalid values for
            // Windows separately. We took 'double' representation of
            // 'TimeInterval' boundary value, used 'std::nextafter' function to
            // get following existing value and then converted it back to
            // the count of 'std::chrono::duration' object.
            // Here and below in the test.

        //        NAME                 EXP_8   EXP_16  EXP_32  EXP_64
        //        ------------------   ------- ------  ------  ------
        const RCR k_ONE_INT8_MAX_EXP  {  true,  true,  true,   true  };
        const RCR k_ONE_INT8_MIN_EXP  {  true,  true,  true,   true  };
        const RCR k_ONE_INT16_MAX_EXP {  k_NA,  true,  true,   true  };
        const RCR k_ONE_INT16_MIN_EXP {  k_NA,  true,  true,   true  };
        const RCR k_ONE_INT32_MAX_EXP {  k_NA,  k_NA,  true,   true  };
        const RCR k_ONE_INT32_MIN_EXP {  k_NA,  k_NA,  true,   true  };
        const RCR k_ONE_INT64_MAX_EXP {  k_NA,  k_NA,  k_NA,   true  };
        const RCR k_ONE_INT64_MIN_EXP {  k_NA,  k_NA,  k_NA,   true  };

        const Data ONE_DATA[] = {
            //LINE COUNT         EXPECTED RESULTS
            //---- ------------  -------------------
            { L_,  0,            k_ZERO_EXP          },
            { L_,  k_INT8_MAX,   k_ONE_INT8_MAX_EXP  },
            { L_,  k_INT8_MIN,   k_ONE_INT8_MIN_EXP  },
            { L_,  k_INT16_MAX,  k_ONE_INT16_MAX_EXP },
            { L_,  k_INT16_MIN,  k_ONE_INT16_MIN_EXP },
            { L_,  k_INT32_MAX,  k_ONE_INT32_MAX_EXP },
            { L_,  k_INT32_MIN,  k_ONE_INT32_MIN_EXP },
            { L_,  k_INT64_MAX,  k_ONE_INT64_MAX_EXP },
            { L_,  k_INT64_MIN,  k_ONE_INT64_MIN_EXP },
        };
        const auto NUM_ONE_DATA = sizeof ONE_DATA / sizeof *ONE_DATA;

        for (size_t i = 0; i < NUM_ONE_DATA; i++) {
            const int               LINE     = ONE_DATA[i].d_line;
            const std::int64_t      COUNT    = ONE_DATA[i].d_count;
            const RangeCheckResults EXPECTED = ONE_DATA[i].d_expected;

            verifyTimeIntervalRangeCheck<one>(LINE, COUNT, EXPECTED);
        }

        //        NAME                       EXP_8   EXP_16  EXP_32  EXP_64
        //        ------------------------   ------  ------  ------  ------
        const RCR k_ONE_UINT8_MAX_EXP      {  true,  true,   true,    true };
        const RCR k_ONE_UINT16_MAX_EXP     {  k_NA,  true,   true,    true };
        const RCR k_ONE_UINT32_MAX_EXP     {  k_NA,  k_NA,   true,    true };
        const RCR k_ONE_UINT64_MAX_EXP     {  k_NA,  k_NA,   k_NA,   false };
        const RCR k_ONE_U_BOUNDARY_MAX_EXP {  k_NA,  k_NA,   k_NA,    true };
        const RCR k_ONE_U_INVALID_MAX_EXP  {  k_NA,  k_NA,   k_NA,   false };

        const UnsignedData U_ONE_DATA[] = {
            //LINE COUNT                 EXPECTED
            //---- ------------          -----------------------
            { L_,  0,                    k_ZERO_EXP               },
            { L_,  k_UINT8_MAX,          k_ONE_UINT8_MAX_EXP      },
            { L_,  k_UINT16_MAX,         k_ONE_UINT16_MAX_EXP     },
            { L_,  k_UINT32_MAX,         k_ONE_UINT32_MAX_EXP     },
            { L_,  k_UINT64_MAX,         k_ONE_UINT64_MAX_EXP     },
            { L_,  k_U_ONE_BOUNDARY_MAX, k_ONE_U_BOUNDARY_MAX_EXP },
            { L_,  k_U_ONE_INVALID_MAX,  k_ONE_U_INVALID_MAX_EXP  },
        };
        const auto NUM_U_ONE_DATA = sizeof U_ONE_DATA / sizeof *U_ONE_DATA;

        for (size_t i = 0; i < NUM_U_ONE_DATA; i++) {
            const int               LINE     = U_ONE_DATA[i].d_line;
            const std::uint64_t     COUNT    = U_ONE_DATA[i].d_count;
            const RangeCheckResults EXPECTED = U_ONE_DATA[i].d_expected;

            verifyTimeIntervalRangeCheckUnsigned<one>(LINE, COUNT, EXPECTED);
        }

        // Two seconds period.

        const std::int64_t k_TWO_BOUNDARY_MAX     =
                                             k_INT64_MAX / two::num * two::den;
        const std::int64_t  k_TWO_BOUNDARY_MIN    =
                                             k_INT64_MIN / two::num * two::den;
        const std::uint64_t k_U_TWO_BOUNDARY_MAX  = k_TWO_BOUNDARY_MAX;

        const std::int64_t  k_TWO_INVALID_MAX   = isRealLongDouble
                                                ? k_TWO_BOUNDARY_MAX + 2
                                                : k_TWO_BOUNDARY_MAX + 1024;
        const std::int64_t  k_TWO_INVALID_MIN   = isRealLongDouble
                                                ? k_TWO_BOUNDARY_MIN - 2
                                                : k_TWO_BOUNDARY_MIN - 1024;
        const std::uint64_t k_U_TWO_INVALID_MAX = isRealLongDouble
                                                ? k_TWO_BOUNDARY_MAX + 2
                                                : k_TWO_BOUNDARY_MAX + 1024;
            // Range check has concessions in order to allow all acceptable
            // values (at the cost of allowing a few invalid). Therefore, in
            // order to obtain the required incorrect value, we have to add (or
            // subtract) two (and 1024 for Windows) instead of one.

        const Data TWO_DATA[] = {
            //LINE COUNT                 EXP_8   EXP_16   EXP_32   EXP_64
            //---- ----------------      ------  ------   ------   ------
            { L_,  0,                  {  true,    true,    true,   true } },
            { L_,  k_INT8_MAX,         {  true,    true,    true,   true } },
            { L_,  k_INT8_MIN,         {  true,    true,    true,   true } },
            { L_,  k_INT16_MAX,        {  k_NA,    true,    true,   true } },
            { L_,  k_INT16_MIN,        {  k_NA,    true,    true,   true } },
            { L_,  k_INT32_MAX,        {  k_NA,    k_NA,    true,   true } },
            { L_,  k_INT32_MIN,        {  k_NA,    k_NA,    true,   true } },
            { L_,  k_INT64_MAX,        {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_INT64_MIN,        {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_TWO_BOUNDARY_MAX, {  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_TWO_INVALID_MAX,  {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_TWO_BOUNDARY_MIN, {  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_TWO_INVALID_MIN,  {  k_NA,    k_NA,    k_NA,  false } },
        };
        const auto NUM_TWO_DATA = sizeof TWO_DATA / sizeof *TWO_DATA;

        for (size_t i = 0; i < NUM_TWO_DATA; i++) {
            const int               LINE     = TWO_DATA[i].d_line;
            const std::int64_t      COUNT    = TWO_DATA[i].d_count;
            const RangeCheckResults EXPECTED = TWO_DATA[i].d_expected;

            verifyTimeIntervalRangeCheck<two>(LINE, COUNT, EXPECTED);
        }

        const UnsignedData U_TWO_DATA[] = {
            //LINE COUNT                  EXP_8   EXP_16   EXP_32   EXP_64
            //---- ------------------     ------  ------   ------   ------
            { L_,  0,                   {  true,    true,    true,   true } },
            { L_,  k_UINT8_MAX,         {  true,    true,    true,   true } },
            { L_,  k_UINT16_MAX,        {  k_NA,    true,    true,   true } },
            { L_,  k_UINT32_MAX,        {  k_NA,    k_NA,    true,   true } },
            { L_,  k_UINT64_MAX,        {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_U_TWO_BOUNDARY_MAX,{  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_U_TWO_INVALID_MAX, {  k_NA,    k_NA,    k_NA,  false } },
        };
        const auto NUM_U_TWO_DATA = sizeof U_TWO_DATA / sizeof *U_TWO_DATA;

        for (size_t i = 0; i < NUM_U_TWO_DATA; i++) {
            const int               LINE     = U_TWO_DATA[i].d_line;
            const std::uint64_t     COUNT    = U_TWO_DATA[i].d_count;
            const RangeCheckResults EXPECTED = U_TWO_DATA[i].d_expected;

            verifyTimeIntervalRangeCheckUnsigned<two>(LINE, COUNT, EXPECTED);
        }

        // Thousand seconds period.

        const std::int64_t  k_KILO_BOUNDARY_MAX    =
                                           k_INT64_MAX / kilo::num * kilo::den;
        const std::int64_t  k_KILO_BOUNDARY_MIN    =
                                           k_INT64_MIN / kilo::num * kilo::den;
        const std::uint64_t k_U_KILO_BOUNDARY_MAX  = k_KILO_BOUNDARY_MAX;

        const std::int64_t  k_KILO_INVALID_MAX   = isRealLongDouble
                                                 ? k_KILO_BOUNDARY_MAX + 2
                                                 : k_KILO_BOUNDARY_MAX + 4;
        const std::int64_t  k_KILO_INVALID_MIN   = isRealLongDouble
                                                 ? k_KILO_BOUNDARY_MIN - 2
                                                 : k_KILO_BOUNDARY_MIN - 4;
        const std::uint64_t k_U_KILO_INVALID_MAX = isRealLongDouble
                                                 ? k_KILO_BOUNDARY_MAX + 2
                                                 : k_KILO_BOUNDARY_MAX + 4;

        const Data KILO_DATA[] = {
            //LINE COUNT                  EXP_8   EXP_16   EXP_32   EXP_64
            //---- ----------------       ------  ------   ------   ------
            { L_,  0,                   {  true,    true,    true,   true } },
            { L_,  k_INT8_MAX,          {  true,    true,    true,   true } },
            { L_,  k_INT8_MIN,          {  true,    true,    true,   true } },
            { L_,  k_INT16_MAX,         {  k_NA,    true,    true,   true } },
            { L_,  k_INT16_MIN,         {  k_NA,    true,    true,   true } },
            { L_,  k_INT32_MAX,         {  k_NA,    k_NA,    true,   true } },
            { L_,  k_INT32_MIN,         {  k_NA,    k_NA,    true,   true } },
            { L_,  k_INT64_MAX,         {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_INT64_MIN,         {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_KILO_BOUNDARY_MAX, {  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_KILO_INVALID_MAX,  {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_KILO_BOUNDARY_MIN, {  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_KILO_INVALID_MIN,  {  k_NA,    k_NA,    k_NA,  false } },
        };
        const auto NUM_KILO_DATA = sizeof KILO_DATA / sizeof *KILO_DATA;

        for (size_t i = 0; i < NUM_KILO_DATA; i++) {
            const int               LINE     = KILO_DATA[i].d_line;
            const std::int64_t      COUNT    = KILO_DATA[i].d_count;
            const RangeCheckResults EXPECTED = KILO_DATA[i].d_expected;

            verifyTimeIntervalRangeCheck<kilo>(LINE, COUNT, EXPECTED);
        }

        const UnsignedData U_KILO_DATA[] = {
            //LINE COUNT                   EXP_8   EXP_16   EXP_32   EXP_64
            //---- ------------------      ------  ------   ------   ------
            { L_,  0,                    {  true,    true,    true,   true } },
            { L_,  k_UINT8_MAX,          {  true,    true,    true,   true } },
            { L_,  k_UINT16_MAX,         {  k_NA,    true,    true,   true } },
            { L_,  k_UINT32_MAX,         {  k_NA,    k_NA,    true,   true } },
            { L_,  k_UINT64_MAX,         {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_U_KILO_BOUNDARY_MAX,{  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_U_KILO_INVALID_MAX, {  k_NA,    k_NA,    k_NA,  false } },
        };
        const auto NUM_U_KILO_DATA = sizeof U_KILO_DATA / sizeof *U_KILO_DATA;

        for (size_t i = 0; i < NUM_U_KILO_DATA; i++) {
            const int               LINE     = U_KILO_DATA[i].d_line;
            const std::uint64_t     COUNT    = U_KILO_DATA[i].d_count;
            const RangeCheckResults EXPECTED = U_KILO_DATA[i].d_expected;

            verifyTimeIntervalRangeCheckUnsigned<kilo>(LINE, COUNT, EXPECTED);
        }

        // Million seconds period.

        const std::int64_t k_MEGA_BOUNDARY_MAX     =
                                           k_INT64_MAX / mega::num * mega::den;

        const std::int64_t k_MEGA_INVALID_MAX    = k_MEGA_BOUNDARY_MAX + 2;
        const std::int64_t k_MEGA_BOUNDARY_MIN     =
                                           k_INT64_MIN / mega::num * mega::den;
        const std::int64_t k_MEGA_INVALID_MIN    = k_MEGA_BOUNDARY_MIN - 2;

        const std::uint64_t k_U_MEGA_BOUNDARY_MAX  = k_MEGA_BOUNDARY_MAX;
        const std::uint64_t k_U_MEGA_INVALID_MAX = k_MEGA_BOUNDARY_MAX + 2;

        const Data MEGA_DATA[] = {
            //LINE COUNT                  EXP_8   EXP_16   EXP_32   EXP_64
            //---- ----------------       ------  ------   ------   ------
            { L_,  0,                   {  true,    true,    true,   true } },
            { L_,  k_INT8_MAX,          {  true,    true,    true,   true } },
            { L_,  k_INT8_MIN,          {  true,    true,    true,   true } },
            { L_,  k_INT16_MAX,         {  k_NA,    true,    true,   true } },
            { L_,  k_INT16_MIN,         {  k_NA,    true,    true,   true } },
            { L_,  k_INT32_MAX,         {  k_NA,    k_NA,    true,   true } },
            { L_,  k_INT32_MIN,         {  k_NA,    k_NA,    true,   true } },
            { L_,  k_INT64_MAX,         {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_INT64_MIN,         {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_MEGA_BOUNDARY_MAX, {  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_MEGA_INVALID_MAX,  {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_MEGA_BOUNDARY_MIN, {  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_MEGA_INVALID_MIN,  {  k_NA,    k_NA,    k_NA,  false } },
        };
        const auto NUM_MEGA_DATA = sizeof MEGA_DATA / sizeof *MEGA_DATA;

        for (size_t i = 0; i < NUM_MEGA_DATA; i++) {
            const int               LINE     = MEGA_DATA[i].d_line;
            const std::int64_t      COUNT    = MEGA_DATA[i].d_count;
            const RangeCheckResults EXPECTED = MEGA_DATA[i].d_expected;

            verifyTimeIntervalRangeCheck<mega>(LINE, COUNT, EXPECTED);
        }

        const UnsignedData U_MEGA_DATA[] = {
            //LINE COUNT                   EXP_8   EXP_16   EXP_32   EXP_64
            //---- ------------------      ------  ------   ------   ------
            { L_,  0,                    {  true,    true,    true,   true } },
            { L_,  k_UINT8_MAX,          {  true,    true,    true,   true } },
            { L_,  k_UINT16_MAX,         {  k_NA,    true,    true,   true } },
            { L_,  k_UINT32_MAX,         {  k_NA,    k_NA,    true,   true } },
            { L_,  k_UINT64_MAX,         {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_U_MEGA_BOUNDARY_MAX,{  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_U_MEGA_INVALID_MAX, {  k_NA,    k_NA,    k_NA,  false } },
        };
        const auto NUM_U_MEGA_DATA = sizeof U_MEGA_DATA / sizeof *U_MEGA_DATA;

        for (size_t i = 0; i < NUM_U_MEGA_DATA; i++) {
            const int               LINE     = U_MEGA_DATA[i].d_line;
            const std::uint64_t     COUNT    = U_MEGA_DATA[i].d_count;
            const RangeCheckResults EXPECTED = U_MEGA_DATA[i].d_expected;

            verifyTimeIntervalRangeCheckUnsigned<mega>(LINE, COUNT, EXPECTED);
        }

        // Billion seconds period.

        const std::int64_t k_GIGA_BOUNDARY_MAX     =
                                           k_INT64_MAX / giga::num * giga::den;

        const std::int64_t k_GIGA_INVALID_MAX    = k_GIGA_BOUNDARY_MAX + 2;
        const std::int64_t k_GIGA_BOUNDARY_MIN     =
                                           k_INT64_MIN / giga::num * giga::den;
        const std::int64_t k_GIGA_INVALID_MIN    = k_GIGA_BOUNDARY_MIN - 2;

        const std::uint64_t k_U_GIGA_BOUNDARY_MAX  = k_GIGA_BOUNDARY_MAX;
        const std::uint64_t k_U_GIGA_INVALID_MAX = k_GIGA_BOUNDARY_MAX + 2;

        const Data GIGA_DATA[] = {
            //LINE COUNT                  EXP_8   EXP_16   EXP_32   EXP_64
            //---- ----------------       ------  ------   ------   ------
            { L_,  0,                   {  true,    true,    true,   true } },
            { L_,  k_INT8_MAX,          {  true,    true,    true,   true } },
            { L_,  k_INT8_MIN,          {  true,    true,    true,   true } },
            { L_,  k_INT16_MAX,         {  k_NA,    true,    true,   true } },
            { L_,  k_INT16_MIN,         {  k_NA,    true,    true,   true } },
            { L_,  k_INT32_MAX,         {  k_NA,    k_NA,    true,   true } },
            { L_,  k_INT32_MIN,         {  k_NA,    k_NA,    true,   true } },
            { L_,  k_INT64_MAX,         {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_INT64_MIN,         {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_GIGA_BOUNDARY_MAX, {  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_GIGA_INVALID_MAX,  {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_GIGA_BOUNDARY_MIN, {  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_GIGA_INVALID_MIN,  {  k_NA,    k_NA,    k_NA,  false } },
        };
        const auto NUM_GIGA_DATA = sizeof GIGA_DATA / sizeof *GIGA_DATA;

        for (size_t i = 0; i < NUM_GIGA_DATA; i++) {
            const int               LINE     = GIGA_DATA[i].d_line;
            const std::int64_t      COUNT    = GIGA_DATA[i].d_count;
            const RangeCheckResults EXPECTED = GIGA_DATA[i].d_expected;

            verifyTimeIntervalRangeCheck<giga>(LINE, COUNT, EXPECTED);
        }

        const UnsignedData U_GIGA_DATA[] = {
            //LINE COUNT                   EXP_8   EXP_16   EXP_32   EXP_64
            //---- ------------------      ------  ------   ------   ------
            { L_,  0,                    {  true,    true,    true,   true } },
            { L_,  k_UINT8_MAX,          {  true,    true,    true,   true } },
            { L_,  k_UINT16_MAX,         {  k_NA,    true,    true,   true } },
            { L_,  k_UINT32_MAX,         {  k_NA,    k_NA,    true,   true } },
            { L_,  k_UINT64_MAX,         {  k_NA,    k_NA,    k_NA,  false } },
            { L_,  k_U_GIGA_BOUNDARY_MAX,{  k_NA,    k_NA,    k_NA,   true } },
            { L_,  k_U_GIGA_INVALID_MAX, {  k_NA,    k_NA,    k_NA,  false } },
        };
        const auto NUM_U_GIGA_DATA = sizeof U_GIGA_DATA / sizeof *U_GIGA_DATA;

        for (size_t i = 0; i < NUM_U_GIGA_DATA; i++) {
            const int               LINE     = U_GIGA_DATA[i].d_line;
            const std::uint64_t     COUNT    = U_GIGA_DATA[i].d_count;
            const RangeCheckResults EXPECTED = U_GIGA_DATA[i].d_expected;

            verifyTimeIntervalRangeCheckUnsigned<giga>(LINE, COUNT, EXPECTED);
        }
#else
        if (verbose) printf("'std::chrono' is not supported\n");
#endif
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING STD::CHRONO HELPER CLASSES
        //
        // Concerns:
        //: 1 Helper metafunctions have correct values for any class.
        //:
        //: 2 'bsls::TimeInterval_IsDuration' correctly distinguishes
        //:   'std::chrono::duration' types from the classes with the member
        //:   types having the same names as 'std::chrono::duration' types.
        //:
        //: 3 Any floating point type (float, double and long double) is
        //:   correctly determined by helper metafunctions.
        //
        // Plan:
        //: 1 Verify helper class member values for several types including all
        //:   floating point types and some type, having the same member types
        //:   as 'std::chrono::duration' types.  (C-1..3)
        //
        // Testing:
        //   TESTING STD::CHRONO HELPER CLASSES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STD::CHRONO HELPER CLASSES"
                            "\n==================================\n");

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
        using std::chrono::duration;

        using one = std::ratio<1>;

        if (verbose) printf("\nTesting 'TimeInterval_IsDuration'\n");
        {
         //-^
         //      CHECK      TYPE                                      EXPECTED
         //      ---------- ----------------------------------------- --------
         ASSERT((isDuration<char                                    >(false)));
         ASSERT((isDuration<unsigned char                           >(false)));
         ASSERT((isDuration<int                                     >(false)));
         ASSERT((isDuration<unsigned int                            >(false)));
         ASSERT((isDuration<long int                                >(false)));
         ASSERT((isDuration<unsigned long int                       >(false)));
         ASSERT((isDuration<long long int                           >(false)));
         ASSERT((isDuration<float                                   >(false)));
         ASSERT((isDuration<double                                  >(false)));
         ASSERT((isDuration<long double                             >(false)));
         ASSERT((isDuration<char *                                  >(false)));
         ASSERT((isDuration<char&                                   >(false)));

         ASSERT((isDuration<DummyDuration<bool,              tera > >(false)));
         ASSERT((isDuration<DummyDuration<char,              giga > >(false)));
         ASSERT((isDuration<DummyDuration<unsigned char,     mega > >(false)));
         ASSERT((isDuration<DummyDuration<int,               kilo > >(false)));
         ASSERT((isDuration<DummyDuration<unsigned int,      hecto> >(false)));
         ASSERT((isDuration<DummyDuration<long int,          deca > >(false)));
         ASSERT((isDuration<DummyDuration<unsigned long int, one  > >(false)));
         ASSERT((isDuration<DummyDuration<long long int,     milli> >(false)));
         ASSERT((isDuration<DummyDuration<float,             micro> >(false)));
         ASSERT((isDuration<DummyDuration<double,            nano > >(false)));
         ASSERT((isDuration<DummyDuration<long double,       pico > >(false)));

         ASSERT((isDuration<     duration<bool,              tera > >(true )));
         ASSERT((isDuration<     duration<char,              giga > >(true )));
         ASSERT((isDuration<     duration<unsigned char,     mega > >(true )));
         ASSERT((isDuration<     duration<int,               kilo > >(true )));
         ASSERT((isDuration<     duration<unsigned int,      hecto> >(true )));
         ASSERT((isDuration<     duration<long int,          deca > >(true )));
         ASSERT((isDuration<     duration<unsigned long int, one  > >(true )));
         ASSERT((isDuration<     duration<long long int,     milli> >(true )));
         ASSERT((isDuration<     duration<float,             micro> >(true )));
         ASSERT((isDuration<     duration<double,            nano > >(true )));
         ASSERT((isDuration<     duration<long double,       pico > >(true )));
         //-v
        }

        if (verbose) printf("\nTesting 'TimeInterval_RepTraits'\n");
        {
            //      CHECK   TYPE                EXPECTED
            //      ------- ------------------- --------
            ASSERT((isFloat<bool              >(false)));
            ASSERT((isFloat<char              >(false)));
            ASSERT((isFloat<unsigned char     >(false)));
            ASSERT((isFloat<int               >(false)));
            ASSERT((isFloat<unsigned int      >(false)));
            ASSERT((isFloat<long int          >(false)));
            ASSERT((isFloat<unsigned long int >(false)));
            ASSERT((isFloat<long long int     >(false)));
            ASSERT((isFloat<float             >(true )));
            ASSERT((isFloat<double            >(true )));
            ASSERT((isFloat<long double       >(true )));
            ASSERT((isFloat<char *            >(false)));
            ASSERT((isFloat<char&             >(false)));
        }

        if (verbose) printf("\nTesting 'TimeInterval_DurationTraits'\n");
        {
            //      CHECK              TYPE                      EXPECTED
            //      ------------------ ------------------------- --------
            ASSERT((isDurationFloat<   bool,              tera >(false)));
            ASSERT((isDurationFloat<   char,              giga >(false)));
            ASSERT((isDurationFloat<   unsigned char,     mega >(false)));
            ASSERT((isDurationFloat<   int,               kilo >(false)));
            ASSERT((isDurationFloat<   unsigned int,      hecto>(false)));
            ASSERT((isDurationFloat<   long int,          deca >(false)));
            ASSERT((isDurationFloat<   unsigned long int, one  >(false)));
            ASSERT((isDurationFloat<   long long int,     milli>(false)));
            ASSERT((isDurationFloat<   float,             micro>(true )));
            ASSERT((isDurationFloat<   double,            nano >(true )));
            ASSERT((isDurationFloat<   long double,       pico >(true )));

            ASSERT((isDurationImplicit<bool,              tera >(true )));
            ASSERT((isDurationImplicit<char,              giga >(true )));
            ASSERT((isDurationImplicit<unsigned char,     mega >(true )));
            ASSERT((isDurationImplicit<int,               kilo >(true )));
            ASSERT((isDurationImplicit<unsigned int,      hecto>(true )));
            ASSERT((isDurationImplicit<long int,          deca >(true )));
            ASSERT((isDurationImplicit<unsigned long int, one  >(true )));
            ASSERT((isDurationImplicit<long long int,     milli>(true )));
            ASSERT((isDurationImplicit<float,             micro>(true )));
            ASSERT((isDurationImplicit<double,            nano >(true )));
            ASSERT((isDurationImplicit<long double,       pico >(false)));

            // Full test for all representation and period types will take too
            // many lines: 11x11x2.  Since we have tested separate conditions
            // (i.e. k_IS_FLOAT and k_IS_IMPLICIT) already, we just need to
            // check that all their combinations are handled correctly.

            ASSERT((isImplicitEnabled< long long int,     nano >(true )));
            ASSERT((isImplicitEnabled< long long int,     pico >(false)));
            ASSERT((isImplicitEnabled< float,             nano >(false)));
            ASSERT((isImplicitEnabled< float,             pico >(false)));

            ASSERT((isExplicitEnabled< long long int,     nano >(false)));
            ASSERT((isExplicitEnabled< long long int,     pico >(true )));
            ASSERT((isExplicitEnabled< float,             nano >(false)));
            ASSERT((isExplicitEnabled< float,             pico >(false)));
        }
#else
        if (verbose) printf("'std::chrono' is not supported\n");
#endif
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING: DRQS 65043434
        //   The previous implementation of 'operator<<' for
        //   'bsls::TimeInterval' was templatized on the *stream* type to avoid
        //   a dependency on 'bsl::ostream' (which would have been a
        //   levelization violation).  Ticket DRQS 65043434 reported that this
        //   signature caused ambiguity in ADL lookup in the presence of
        //   facilities that provide a streaming operator templatized on the
        //   *object* type.  Facilities that provide such a fall-back streaming
        //   operator include Google Test, BDEX, and various logging
        //   frameworks.
        //
        // Concerns:
        //:  1 'bsls::operator<<' for 'bsls::TimeInterval' can be unambiguously
        //:    called in a scope that can also see a generic 'operator<<' that
        //:    is templatized on the 'object' parameter.
        //:
        //:  2 Invoking 'operator<<' with arguments of types
        //:    'bsl::ostream' and 'bsls::TimeInterval' calls the function
        //:    provided by 'bsls_timeinterval'.
        //:
        //:  3 Invoking a generic 'operator<<' that does not provide a
        //:    specialization for 'bsls::TimeInterval' will call the generic
        //:    function.
        //
        // Plan:
        //:  1 Define generic streaming operators templatized on the 'object'
        //:    type, and attempt to use them to stream objects of type
        //:    'bsls::TimeInterval'.  Under the old implementation of
        //:    'bsls::operator<<' for 'bsls::TimeInterval', these calls will
        //:    not compile.  Under the new implementation, these calls will
        //:    compile.  (C-1)
        //:
        //:  2 Define a streaming operator that accepts arbitrary
        //:    objects to be streamed to an 'bsl::ostream', and that does not
        //:    modify the stream.  Confirm that streaming a
        //:    'bsls::TimeInterval' to a 'bsl::ostream' does change the stream,
        //:    thereby confirming that the correct streaming operator is
        //:    selected.  (C-2)
        //:
        //:  3 Define a streaming operator that accepts arbitrary
        //:    objects to be streamed to a user-defined stream, with some
        //:    observable side-effect.  Confirm that streaming a
        //:    'bsls::TimeInterval' to such a 'bsl::ostream' causes the
        //:    side-effect, thereby confirming that the correct streaming
        //:    operator is selected.  (C-3)
        //
        // Testing:
        //   CONCERN: DRQS 65043434
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: DRQS 65043434"
                            "\n======================\n");

        const bsls::TimeInterval interval;
        const int                integer = 42;
        const testadl::UserType  object  = {};

        using namespace testadl;

        if (verbose) printf("\nStreaming to std::ostream\n");
        {
            std::ostringstream stream;

            std::string::size_type lastLength = 0;
            ASSERT(lastLength == stream.str().length());

            stream << interval;
            ASSERT(lastLength < stream.str().length());
            lastLength = stream.str().length();

            stream << integer;
            ASSERT(lastLength < stream.str().length());
            lastLength = stream.str().length();

            stream << object;
            ASSERT(lastLength == stream.str().length());
        }

        if (verbose) printf("\nStreaming to custom stream\n");
        {
            CustomStream stream = { 0 };

            stream << interval;
            ASSERT(1 == stream.d_counter);

            stream << integer;
            ASSERT(2 == stream.d_counter);

            stream << object;
            ASSERT(3 == stream.d_counter);
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING: 'isValid'
        //   Ensure that 'isValid' correctly indicates whether the
        //   input is a valid time interval.
        //
        // Concerns:
        //: 1 That 'isValid' is a class method.
        //:
        //: 2 That 'isValid' returns true for any combination of 'second' and
        //:   'nanosecond' values that can be represented using a
        //:   'TimeInterval', and 'false' otherwise.
        //
        // Plan:
        //: 1 Using a table driven technique, call 'isValid' and test the
        //:   result against the expected result. (C-1..2)
        //
        // Testing:
        //   bool isValid(Int64, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: 'isValid'"
                            "\n==================\n");

        struct {
            int    d_lineNum;
            Int64  d_secs;
            int    d_nsecs;
            bool   d_isValid;
        } DATA[] = {
            //line     secs     nanosecs isValid
            //----     ----     -------- -------
            { L_,         0,           0,  true },
            { L_, LLONG_MAX,           0,  true },
            { L_, LLONG_MAX,   999999999,  true },
            { L_, LLONG_MAX,  -999999999,  true },
            { L_, LLONG_MAX, -1000000000,  true },
            { L_, LLONG_MAX,  1000000000, false },
            { L_, LLONG_MAX,  1000000001, false },
            { L_, LLONG_MAX,  1999999999, false },
            { L_, LLONG_MAX,     INT_MAX, false },
            { L_, LLONG_MIN,           0,  true },
            { L_, LLONG_MIN,  -999999999,  true },
            { L_, LLONG_MIN,   999999999,  true },
            { L_, LLONG_MIN,  1000000000,  true },
            { L_, LLONG_MIN, -1000000000, false },
            { L_, LLONG_MIN, -1000000001, false },
            { L_, LLONG_MIN, -1999999999, false },
            { L_, LLONG_MIN,     INT_MIN, false },
            { L_, 0x44444444,    0x66666,  true },  // {DRQS 164912552}
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf(
            "\nTesting: 'isValid' with test table\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;
            const bool  IEXP   = DATA[i].d_isValid;

            if (veryVerbose) { T_;  P_(ISECS); P_(INSECS); P(IEXP); }

            ASSERTV(ILINE, IEXP == Obj::isValid(ISECS, INSECS));
        }

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING: 'operator-' (NEGATION)
        //   Ensure the negation operator returns a time interval having the
        //   negation of the supplied input.
        //
        // Concerns:
        //: 1 The object that is returned by the free 'operator-' (negation)
        //:   has a time interval that represents the negation of the input
        //:   value.
        //:
        //: 2 The operator accepts the contractually specified range of
        //:   argument values.
        //:
        //: 3 The value of the source object is not modified.
        //:
        //: 4 Non-modifiable objects can be negated (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 5 The operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the address of 'operator-' to initialize a function pointer
        //:   having the appropriate signature and return type for the
        //:   negation operator defined in this component.  (C-4..5)
        //:
        //: 2 Using a table driven technique, generate a test object for each
        //:   test value, negate that test object and copy the value to a
        //:   new object.  Verify the canonical properties of the resulting
        //:   object are the negation of the original object. (C-1..3)
        //
        // Testing:
        //   TimeInterval operator-(const TimeInterval& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: 'operator-' (NEGATION)"
                           "\n===============================\n");

        if (verbose) printf(
                     "\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj (*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorNeg = bsls::operator-;

            (void)operatorNeg;  // quash potential compiler warnings
        }

        struct {
            int d_lineNum;
            int d_secs;
            int d_nsecs;
        } DATA[] = {
            //line   secs   nanosecs
            //----   ----   --------
            { L_,      0,          0 },
            { L_,      5,          1 },
            { L_,      2,  999999999 },
            { L_,     -3,         -1 },
            { L_,     -1, -999999999 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf(
            "\nTesting: 'operator-' with 'TimeInterval'\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;


            if (veryVerbose) { T_;  P(X); }

            Obj mY = -X; const Obj& Y = mY;

            ASSERTV(ILINE, ILINE, -ISECS   == Y.seconds());
            ASSERTV(ILINE, ILINE, -INSECS  == Y.nanoseconds());
            ASSERTV(ILINE, ILINE,  ISECS   == X.seconds());
            ASSERTV(ILINE, ILINE,  INSECS  == X.nanoseconds());
        }

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // ARITHMETIC FREE OPERATORS (+, -)
        //   Ensure that each operator correctly computes the underlying
        //   canonical representation of the result object.
        //
        // Concerns:
        //: 1 The object that is returned by the free 'operator+' ('operator-')
        //:   has a time interval that represents the sum (difference) of those
        //:   of the two operands.
        //:
        //: 2 Each operator accepts the contractually specified range of
        //:   argument values.
        //:
        //: 3 The values of the two source objects supplied to each operator
        //:   are not modified.
        //:
        //: 4 Non-modifiable objects can be added and subtracted (i.e., objects
        //:   or references providing only non-modifiable access).
        //:
        //: 5 The operators' signatures and return types are standard.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator+' and 'operator-' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the six free binary arithmetic operators
        //:   defined in this component.  (C-4..5)
        //:
        //: 2 Using a table driven technique, generate 2 test objects for a
        //:   cross-product of test values, and then add and subtract (using
        //:   the operator under test) those two values, comparing the result
        //:   to an expected value generated by using previously tested
        //:   arithmetic member methods.  (C-1..3)
        //:
        //: 3 Using a table driven technique, generate a test object and
        //:   a 'double' value for a cross-product of test values, and then add
        //:   and subtract (using the operator under test) those two values,
        //:   comparing the result to an expected value generated by using
        //:   previously tested arithmetic member methods.  (C-1..3)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   TimeInterval operator+(const Obj& lhs, const Obj& rhs);
        //   TimeInterval operator+(const Obj& lhs, double rhs);
        //   TimeInterval operator+(double lhs, const Obj& rhs);
        //   TimeInterval operator-(const Obj& lhs, const Obj& rhs);
        //   TimeInterval operator-(const Obj& lhs, double rhs);
        //   TimeInterval operator-(double lhs, const Obj& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nARITHMETIC FREE OPERATORS (+, -)"
                            "\n================================\n");

        if (verbose) printf(
            "\nAssign the address of each operator to a variable.\n");
        {
            {
                typedef Obj (*operatorPtr)(const Obj&, const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorAdd = bsls::operator+;
                operatorPtr operatorSub = bsls::operator-;

                (void)operatorAdd;  // quash potential compiler warnings
                (void)operatorSub;
            }
            {
                typedef Obj (*operatorPtr)(double, const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorAdd = bsls::operator+;
                operatorPtr operatorSub = bsls::operator-;

                (void)operatorAdd;  // quash potential compiler warnings
                (void)operatorSub;
            }
            {
                typedef Obj (*operatorPtr)(const Obj&, double);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorAdd = bsls::operator+;
                operatorPtr operatorSub = bsls::operator-;

                (void)operatorAdd;  // quash potential compiler warnings
                (void)operatorSub;
            }
        }

        struct {
            int d_lineNum;
            int d_secs;
            int d_nsecs;
        } DATA[] = {
            //line   secs   nanosecs
            //----   ----   --------
            { L_,      0,          0 },
            { L_,      5,          1 },
            { L_,      2,  999999999 },
            { L_,     -3,         -1 },
            { L_,     -1, -999999999 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf(
            "\nTesting: 'operator+' and 'operator-' with 'TimeInterval'\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;

            for (int j = 0; j < NUM_DATA; j++) {
                const int   JLINE  = DATA[j].d_lineNum;
                const Int64 JSECS  = DATA[j].d_secs;
                const int   JNSECS = DATA[j].d_nsecs;

                Obj mY(JSECS, JNSECS); const Obj& Y = mY;

                if (veryVerbose) { T_;  P_(X);  P(Y); }

                {
                    // Addition

                    Obj mZ(X); const Obj& Z = mZ;
                    Obj mW;    const Obj& W = mW;
                    mZ += Y;

                    mW = X + Y;

                    if (veryVerbose) { T_; T_; P_(W);  P(Z); }

                    ASSERTV(ILINE, JLINE, Z       == W);
                    ASSERTV(ILINE, JLINE, ISECS   == X.seconds());
                    ASSERTV(ILINE, JLINE, INSECS  == X.nanoseconds());
                    ASSERTV(ILINE, JLINE, JSECS   == Y.seconds());
                    ASSERTV(ILINE, JLINE, JNSECS  == Y.nanoseconds());
                }
                {
                    // Subtraction.

                    Obj mZ(X); const Obj& Z = mZ;
                    Obj mW;    const Obj& W = mW;
                    mZ -= Y;

                    mW = X - Y;

                    if (veryVerbose) { T_; T_; P_(W);  P(Z); }

                    ASSERTV(ILINE, JLINE, Z       == W);
                    ASSERTV(ILINE, JLINE, ISECS   == X.seconds());
                    ASSERTV(ILINE, JLINE, INSECS  == X.nanoseconds());
                    ASSERTV(ILINE, JLINE, JSECS   == Y.seconds());
                    ASSERTV(ILINE, JLINE, JNSECS  == Y.nanoseconds());
                }
            }
        }

        if (verbose) printf(
            "\nTesting: 'operator+' and 'operator-' with 'double'\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;

            for (int j = 0; j < NUM_DATA; j++) {
                const int   JLINE  = DATA[j].d_lineNum;
                const Int64 JSECS  = DATA[j].d_secs;
                const int   JNSECS = DATA[j].d_nsecs;

                const double dYv = static_cast<double>(JSECS) +
                                   static_cast<double>(JNSECS) /
                                   static_cast<double>(k_NANOSECS_PER_SEC);


                if (veryVerbose) { T_;  P_(X);  P(dYv); }

                {
                    // Addition

                    Obj mZ(X); const Obj& Z = mZ;
                    Obj mW;    const Obj& W = mW;
                    Obj mV;    const Obj& V = mV;

                    mZ += dYv;

                    mW = X + dYv;
                    mV = dYv + X;

                    if (veryVerbose) { T_; T_; P_(V); P_(W);  P(Z); }

                    ASSERTV(ILINE, JLINE, Z       == W);
                    ASSERTV(ILINE, JLINE, Z       == V);
                    ASSERTV(ILINE, JLINE, ISECS   == X.seconds());
                    ASSERTV(ILINE, JLINE, INSECS  == X.nanoseconds());
                }
                {
                    // Subtraction.

                    Obj mZ(X); const Obj& Z = mZ;
                    Obj mW;    const Obj& W = mW;
                    Obj mV;    const Obj& V = mV;

                    mZ -= dYv;

                    // TimeInterval - double.

                    mW = X - dYv;

                    if (veryVerbose) { T_; T_; P_(W);  P(Z); }

                    ASSERTV(ILINE, JLINE, Z       == W);
                    ASSERTV(ILINE, JLINE, ISECS   == X.seconds());
                    ASSERTV(ILINE, JLINE, INSECS  == X.nanoseconds());

                    mZ = Obj(dYv);
                    mZ -= X;

                    // double - TimeInterval

                    mV = dYv - X;

                    if (veryVerbose) { T_; T_; P_(V);  P(Z); }

                    ASSERTV(ILINE, JLINE, Z       == V);
                    ASSERTV(ILINE, JLINE, ISECS   == X.seconds());
                    ASSERTV(ILINE, JLINE, INSECS  == X.nanoseconds());
                }
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) printf("\t'operator+ w/ 'TimeInterval'\n");
            {
                Obj mX(LLONG_MAX, 999999998); const Obj& X = mX;
                Obj mY(        0,         1); const Obj& Y = mY;


                ASSERT_PASS(X + Y);

                mX.addNanoseconds(1);

                ASSERT_FAIL(X + Y);

                mX.setInterval(LLONG_MIN, -999999998);
                mY.setInterval(        0,         -1);

                ASSERT_PASS(X + Y);

                mX.addNanoseconds(-1);

                ASSERT_FAIL(X + Y);
            }

            if (veryVerbose) printf("\t'operator- w/ 'TimeInterval'\n");
            {
                Obj mX(LLONG_MAX, 999999998); const Obj& X = mX;
                Obj mY(        0,        -1); const Obj& Y = mY;


                ASSERT_PASS(X - Y);

                mX.addNanoseconds(1);

                ASSERT_FAIL(X - Y);

                mX.setInterval(LLONG_MIN, -999999998);
                mY.setInterval(        0,          1);

                ASSERT_PASS(X - Y);

                mX.addNanoseconds(-1);

                ASSERT_FAIL(X - Y);
            }

            if (veryVerbose) printf("\t'operator+ w/ 'double'\n");
            {
                Obj mX(static_cast<double>(LLONG_MAX) * .9);
                const Obj& X = mX;

                double dYv = static_cast<double>(LLONG_MAX) * .08;

                ASSERT_PASS(X   + dYv);
                ASSERT_PASS(dYv + X);

                ASSERT_PASS(mX += dYv);

                ASSERT_FAIL(X   + dYv);
                ASSERT_FAIL(dYv + X);

                mX  = Obj(static_cast<double>(LLONG_MIN) * .9);
                dYv = -1 * static_cast<double>(LLONG_MAX) * .08;

                dYv = LLONG_MIN * .09;

                ASSERT_PASS(X   + dYv);
                ASSERT_PASS(dYv + X);

                ASSERT_PASS(mX += dYv);

                ASSERT_FAIL(X   + dYv);
                ASSERT_FAIL(dYv + X);
            }

            if (veryVerbose) printf("\t'operator- w/ 'double'\n");
            {
                Obj mX(static_cast<double>(LLONG_MAX) * -.9);
                const Obj& X = mX;

                double dYv = static_cast<double>(LLONG_MAX) * .08;

                ASSERT_PASS(X   - dYv);
                ASSERT_PASS(dYv - X);

                ASSERT_PASS(mX -= dYv);

                ASSERT_FAIL(X   - dYv);
                ASSERT_FAIL(dYv - X);

                mX  = Obj(static_cast<double>(LLONG_MIN) * .9);
                dYv = -1 * static_cast<double>(LLONG_MAX) * .08;

                ASSERT_PASS(X   - dYv);
                ASSERT_PASS(dYv - X);

                ASSERT_PASS(mX -= dYv);

                ASSERT_PASS(X   - dYv);
                ASSERT_PASS(dYv - X);
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // ARITHMETIC ASSIGNMENT OPERATORS (+=, -=)
        //   Ensure that each operator correctly adjusts the underlying
        //   canonical representation of the object.
        //
        // Concerns:
        //: 1 Each compound assignment operator can change the value of any
        //:   modifiable target object based on any source object that does not
        //:   violate the method's documented preconditions.
        //:
        //: 2 The signatures and return types are standard.
        //:
        //: 3 The reference returned from each operator is to the target object
        //:   (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified, unless it is an
        //:   alias for the target object.
        //:
        //: 5 A compound assignment of an object to itself behaves as expected
        //:   (alias-safety).
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator+=' and 'operator-=' to
        //:   initialize member-function pointers having the appropriate
        //:   signatures and return types for the two compound assignment
        //:   operators defined in this component.  (C-2)
        //:
        //: 2 Using a table driven technique, generate a test object for a
        //:   series of initial values, and then add and subtract (using the
        //:   operator under test) a series of 'TimeInterval' values from that
        //:   initial value.  Verify the results. (C-1, 3..4).
        //:
        //: 3 Using a table driven technique, create a test-value and add and
        //:   subtract it from itself.  Verify the return value, and the
        //:   resulting object value. (C-5).
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   TimeInterval& operator+=(const TimeInterval&);
        //   TimeInterval& operator-=(const TimeInterval&);
        //   TimeInterval& operator+=(double);
        //   TimeInterval& operator-=(double);
        // --------------------------------------------------------------------

        if (verbose) printf("\nARITHMETIC ASSIGNMENT OPERATORS (+=, -=)"
                            "\n========================================\n");

        if (verbose) printf(
            "\nAssign the address of each operator to a variable.\n");
        {
            {
                typedef Obj& (Obj::*operatorPtr)(const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorAddAssignment = &Obj::operator+=;
                operatorPtr operatorSubAssignment = &Obj::operator-=;

                (void)operatorAddAssignment; // quash compiler warning
                (void)operatorSubAssignment;
            }
            {
                typedef Obj& (Obj::*operatorPtr)(double);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorAddAssignment = &Obj::operator+=;
                operatorPtr operatorSubAssignment = &Obj::operator-=;

                (void)operatorAddAssignment; // quash compiler warning
                (void)operatorSubAssignment;
            }
        }

        struct {
            int d_lineNum;
            int d_secs;
            int d_nsecs;
        } DATA[] = {
            //line   secs   nanosecs
            //----   ----   --------
            { L_,      0,          0 },
            { L_,      5,          1 },
            { L_,      2,  999999999 },
            { L_,     -3,         -1 },
            { L_,     -1, -999999999 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf(
            "\nTesting: 'operator+=' and 'operator-=' with 'TimeInterval'\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;

            for (int j = 0; j < NUM_DATA; j++) {
                const int   JLINE  = DATA[j].d_lineNum;
                const Int64 JSECS  = DATA[j].d_secs;
                const int   JNSECS = DATA[j].d_nsecs;

                Obj mY(JSECS, JNSECS); const Obj& Y = mY;

                if (veryVerbose) { T_;  P_(X);  P_(Y); }

                {
                    // Addition.

                    Obj *mR = &(mX += Y);

                    Obj mZ(ISECS + JSECS, INSECS + JNSECS); const Obj& Z = mZ;

                    if (veryVerbose) { P_(X);  P_(Z); }

                    ASSERTV(ILINE, JLINE, Z       == X);
                    ASSERTV(ILINE, JLINE, mR      == &X);
                    ASSERTV(ILINE, JLINE, JSECS   == Y.seconds());
                    ASSERTV(ILINE, JLINE, JNSECS  == Y.nanoseconds());
                }
                {
                    // Subtraction.

                    Obj *mR = &(mX -= Y);

                    Obj mZ(ISECS, INSECS); const Obj& Z = mZ;

                    if (veryVerbose) { P_(X);  P_(Z); }

                    ASSERTV(ILINE, JLINE, Z       == X);
                    ASSERTV(ILINE, JLINE, mR      == &X);
                    ASSERTV(ILINE, JLINE, JSECS   == Y.seconds());
                    ASSERTV(ILINE, JLINE, JNSECS  == Y.nanoseconds());

                }
            }
        }

        if (verbose) printf("\t'Testing self-assignment.\n");

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;

            const Obj& Z = mX;

            if (veryVerbose) { T_ P_(X) }

            Obj mY(2 * ISECS, 2 * INSECS);  const Obj& Y = mY;

            if (veryVerbose) { T_ T_ P(Y) }

            {
                // Addition.

                Obj *mR = &(mX += Z);

                ASSERTV(ILINE,  Y,   X,  Y == X);
                ASSERTV(ILINE, mR, &mX, mR == &mX);
            }

            mY.setInterval(0, 0);

            {
                // Subtraction.

                Obj *mR = &(mX -= Z);

                ASSERTV(ILINE,  Y,   X,  Y == X);
                ASSERTV(ILINE, mR, &mX, mR == &mX);
            }
        }

        if (verbose) printf(
            "\nTesting: 'operator+=' and 'operator-=' with 'double'\n");

        for (int i = 0; i < NUM_DATA; i++) {
            const int   ILINE  = DATA[i].d_lineNum;
            const Int64 ISECS  = DATA[i].d_secs;
            const int   INSECS = DATA[i].d_nsecs;

            Obj mX(ISECS, INSECS); const Obj& X = mX;

            for (int j = 0; j < NUM_DATA; j++) {
                const int   JLINE  = DATA[j].d_lineNum;
                const Int64 JSECS  = DATA[j].d_secs;
                const int   JNSECS = DATA[j].d_nsecs;

                double y = static_cast<double>(JSECS) +
                           static_cast<double>(JNSECS) /
                           static_cast<double>(k_NANOSECS_PER_SEC);

                if (veryVerbose) { T_;  P_(X);  P_(y); }

                {
                    // Addition.

                    Obj *mR = &(mX += y);

                    Obj mZ(ISECS + JSECS, INSECS + JNSECS); const Obj& Z = mZ;

                    if (veryVerbose) { P_(X);  P_(Z); }

                    ASSERTV(ILINE, JLINE, Z  == X);
                    ASSERTV(ILINE, JLINE, mR == &X);
                }
                {
                    // Subtraction.

                    Obj *mR = &(mX -= y);

                    Obj mZ(ISECS, INSECS); const Obj& Z = mZ;

                    if (veryVerbose) { P_(X);  P_(Z); }

                    ASSERTV(ILINE, JLINE, Z  == X);
                    ASSERTV(ILINE, JLINE, mR == &X);
                }
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) printf("\t'operator+= w/ 'TimeInterval'\n");
            {
                Obj mX(0, 0);
                Obj mY(LLONG_MAX, 999999999); const Obj& Y = mY;

                ASSERT_PASS(mX += Y);
                ASSERT_FAIL(mX += Y);

                mX.setInterval(0, 0);
                mY.setInterval(LLONG_MIN, -999999999);

                ASSERT_PASS(mX += Y);
                ASSERT_FAIL(mX += Y);

            }

            if (veryVerbose) printf("\t'operator-= w/ 'TimeInterval'\n");
            {
                Obj mX(0, 0);
                Obj mY(LLONG_MAX, 999999999); const Obj& Y = mY;

                ASSERT_PASS(mX -= Y);
                ASSERT_FAIL(mX -= Y);


                // Note that we cannot subtract a LLONG_MIN number of seconds.
                mX.setInterval(0, 0);
                mY.setInterval(LLONG_MIN + 1, -999999999);

                ASSERT_PASS(mX -= Y);
                ASSERT_FAIL(mX -= Y);

                mX.setInterval(0, 0);
                mY.setInterval(LLONG_MIN, -999999999);

                ASSERT_SAFE_FAIL(mX -= Y);
            }

            if (veryVerbose) printf("\t'operator+= w/ 'double'\n");
            {
                Obj mX(0, 0);
                double y = static_cast<double>(LLONG_MAX) * .999;

                ASSERT_PASS(mX += y);
                ASSERT_FAIL(mX += y);

                mX.setInterval(0, 0);
                y = -y;

                ASSERT_PASS(mX += y);
                ASSERT_FAIL(mX += y);

            }

            if (veryVerbose) printf("\t'operator-= w/ 'double'\n");
            {
                Obj mX(0, 0);
                double y = static_cast<double>(LLONG_MAX) * .999;

                ASSERT_PASS(mX -= y);
                ASSERT_FAIL(mX -= y);

                mX.setInterval(0, 0);
                y = -y;

                ASSERT_PASS(mX -= y);
                ASSERT_FAIL(mX -= y);
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING: 'add*' MANIPULATORS
        //   Ensure that each method correctly adjusts the underlying
        //   canonical representation of the object.
        //
        // Concerns:
        //: 1 Each method correctly updates the object state from the state on
        //:   entry and the supplied argument.
        //:
        //: 2 The 'nanoseconds' argument to 'addInterval' defaults to 0.
        //:
        //: 3 Each manipulator accepts the contractually specified range of
        //:   argument values.
        //:
        //: 4 Each manipulator returns the expected value.
        //
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, call 'addInterval' on a
        //:   default constructed 'TimeInterval' and verify the result is
        //:   equivalent to using the value constructor.
        //:
        //: 2 Using the table-driven technique, call 'addInterval' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 3 Using the table-driven technique, call 'addInterval' on a
        //:   'TimeInterval' assigned some initial value, without supplying a
        //:   number of 'nanoseconds', and verify the result uses the expected
        //:   default argument value of 0.  (C-2)
        //:
        //: 4 Using the table-driven technique, call 'addDays' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 5 Using the table-driven technique, call 'addHours' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 6 Using the table-driven technique, call 'addMinutes' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 7 Using the table-driven technique, call 'addSeconds' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 8 Using the table-driven technique, call 'addMilliseconds' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //: 9 Using the table-driven technique, call 'addMicroseconds' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //:
        //:10 Using the table-driven technique, call 'addNanoseconds' on a
        //:   'TimeInterval' assigned some initial value and verify the result
        //:   is equivalent to using the value constructor.  (C-1, 3)
        //
        //:11 On each invokation of a method under test, compare the address of
        //:   the objected referenced by the return value to the address of the
        //:   object under test.  (C-4)
        //:
        //:12 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not
        //:   triggered for adjacent valid ones (using the
        //:   'BSLS_ASSERTTEST_*' macros). (C-5)
        //
        // Testing:
        //   TimeInterval& addDays(bsls::Types::Int64);
        //   TimeInterval& addHours(bsls::Types::Int64);
        //   TimeInterval& addMinutes(bsls::Types::Int64);
        //   TimeInterval& addSeconds(bsls::Types::Int64);
        //   TimeInterval& addMilliseconds(Int64);
        //   TimeInterval& addMicroseconds(Int64);
        //   TimeInterval& addNanoseconds(Int64);
        //   void addInterval(Int64, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: 'add*' MANIPULATORS"
                            "\n============================\n");

        if (verbose) printf(
            "\nTesting: 'addInterval' w/o initial value\n");
        {
            static const struct {
                int                d_lineNum;  // Source line number
                bsls::Types::Int64 d_secs;     // Input seconds
                int                d_nsecs;    // Input nanoseconds
            } DATA[] = {
                //line         secs      nanosecs
                //----         ----      --------
                { L_,            0,             0 },
                { L_,            1,             2 },
                { L_,            1,    1000000000 },
                { L_,           -1,   -1000000000 },
                { L_,            2,   -1000000001 },
                { L_,           -2,    1000000001 },
                { L_,   k_SECS_MAX,     999999999 },
                { L_,   k_SECS_MIN,    -999999999 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE  = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS  = DATA[i].d_secs;
                const int                NSECS = DATA[i].d_nsecs;

                Obj mX; const Obj& X = mX;

                Obj& RETVAL = mX.addInterval(SECS, NSECS);

                ASSERTV(LINE, &RETVAL == &X);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(SECS, NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf(
            "\nTesting: 'addInterval' w/ initial value\n");

        {
            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addSecs;     // Add seconds
                int                d_addNSecs;    // Add nanoseconds
            } DATA[] = {
                //line    init secs init nanosecs    add secs     add nsecs
                //----    --------- -------------    --------     ---------
                { L_,            1,             2,      50000,      123123 },
                { L_,            1,             2,     -50000,      123123 },
                { L_,            1,             2,     -50000,     -123123 },
                { L_,            1,             2,     -50000,  1000000000 },
                { L_,            1,             2,     -50000,  1100000000 },
                { L_,            1,             2,     -50000, -1100000000 },
                { L_,       -56711,         76522,      50000,      123123 },
                { L_,       -56711,         76542,     -50000,      123123 },
                { L_,       -56711,         53245,     -50000,     -123123 },
                { L_,       -56711,         74562,     -50000,  1000000000 },
                { L_,       -56711,         43552,     -50000,  1100000000 },
                { L_,       -56711,         54322,     -50000, -1100000000 },
                { L_,   k_SECS_MAX,     999999999,          0,           0 },
                { L_,   k_SECS_MAX,     999999999,          0,          -1 },
                { L_,   k_SECS_MAX,     999999998,          0,           1 },
                { L_,   k_SECS_MIN,    -999999999,          0,           0 },
                { L_,   k_SECS_MIN,    -999999999,          0,           1 },
                { L_,   k_SECS_MIN,    -999999998,          0,          -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS  = DATA[i].d_initSecs;
                const int                INIT_NSECS = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_SECS   = DATA[i].d_addSecs;
                const int                ADD_NSECS  = DATA[i].d_addNSecs;

                Obj mX; const Obj& X = mX;

                              mX.setInterval(INIT_SECS, INIT_NSECS);
                Obj& RETVAL = mX.addInterval( ADD_SECS, ADD_NSECS);

                ASSERTV(LINE, &RETVAL == &X);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_SECS, INIT_NSECS + ADD_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf(
            "\nTesting: 'addInterval' default 'nanoseconds' value\n");

        {
            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addSecs;     // Add seconds
            } DATA[] = {
                //line    init secs init nanosecs    add secs
                //----    --------- -------------    --------
                { L_,            1,             2,      50000 },
                { L_,            1,             2,     -50000 },
                { L_,       -56711,         76522,      50000 },
                { L_,       -56711,         76542,     -50000 },
                { L_,       -56711,         43552,     -50000 },
                { L_,       -56711,         54322,     -50000 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS  = DATA[i].d_initSecs;
                const int                INIT_NSECS = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_SECS   = DATA[i].d_addSecs;

                Obj mX; const Obj& X = mX;

                              mX.setInterval(INIT_SECS, INIT_NSECS);
                Obj& RETVAL = mX.addInterval(ADD_SECS);

                ASSERTV(LINE, &RETVAL == &X);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_SECS, INIT_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addDays'\n");
        {

            // These constants define the maximum (and minimum) second value to
            // which 'k_DAYS_MAX' (and 'k_DAYS_MIN') days can be added
            // (respectively).

            const Int64 k_SEC_MINUS_MAX = k_SECS_MAX - k_DAYS_MAX * 86400;
            const Int64 k_SEC_PLUS_MIN  = k_SECS_MIN + k_DAYS_MAX * 86400;

            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addDays;     // Add days
            } DATA[] = {
                //line           init secs    nanosecs        add days
                //----           ---------    --------       --------
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,               123456,     654321,         10000 },
                { L_,               123456,     654321,        -10000 },
                { L_,              -123456,    -654321,         10000 },
                { L_,              -123456,    -654321,        -10000 },
                { L_,                    0,          0,    k_DAYS_MAX },
                { L_,                    0,          0,    k_DAYS_MIN },
                { L_,      k_SEC_MINUS_MAX,          0,    k_DAYS_MAX },
                { L_,       k_SEC_PLUS_MIN,          0,    k_DAYS_MIN },
                { L_,           k_SECS_MAX,  999999999,             0 },
                { L_,           k_SECS_MIN, -999999999,             0 },
                { L_,   k_SECS_MAX - 86400,  999999999,             1 },
                { L_,   k_SECS_MIN + 86400,  999999999,            -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS  = DATA[i].d_initSecs;
                const int                INIT_NSECS = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_DAYS   = DATA[i].d_addDays;

                Obj mX; const Obj& X = mX;

                              mX.setInterval(INIT_SECS, INIT_NSECS);
                Obj& RETVAL = mX.addDays(ADD_DAYS);

                ASSERTV(LINE, &RETVAL == &X);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_DAYS * 86400, INIT_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addHours'\n");
        {
            // These constants define the maximum (and minimum) second value to
            // which 'k_HOURS_MAX' (and 'k_HOURS_MIN') hours can be added
            // (respectively).

            const Int64 k_SEC_MINUS_MAX = k_SECS_MAX - k_HOURS_MAX * 3600;
            const Int64 k_SEC_PLUS_MIN  = k_SECS_MIN + k_HOURS_MAX * 3600;

            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addHours;    // Add hours
            } DATA[] = {
                //line           init secs    nanosecs         add hours
                //----           ---------    --------         --------
                { L_,                    1,          2,             123 },
                { L_,                    1,          2,            -321 },
                { L_,                    1,          2,             123 },
                { L_,                    1,          2,            -321 },
                { L_,               123456,     654321,           10000 },
                { L_,               123456,     654321,          -10000 },
                { L_,              -123456,    -654321,           10000 },
                { L_,              -123456,    -654321,          -10000 },
                { L_,                    0,          0,     k_HOURS_MAX },
                { L_,                    0,          0,     k_HOURS_MIN },
                { L_,      k_SEC_MINUS_MAX,          0,     k_HOURS_MAX },
                { L_,       k_SEC_PLUS_MIN,          0,     k_HOURS_MIN },
                { L_,           k_SECS_MAX,  999999999,               0 },
                { L_,           k_SECS_MIN, -999999999,               0 },
                { L_,    k_SECS_MAX - 3600,  999999999,               1 },
                { L_,    k_SECS_MIN + 3600,  999999999,              -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS  = DATA[i].d_initSecs;
                const int                INIT_NSECS = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_HOURS  = DATA[i].d_addHours;

                Obj mX; const Obj& X = mX;

                              mX.setInterval(INIT_SECS, INIT_NSECS);
                Obj& RETVAL = mX.addHours(ADD_HOURS);

                ASSERTV(LINE, &RETVAL == &X);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_HOURS * 3600, INIT_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addMinutes'\n");
        {
            // These constants define the maximum (and minimum) second value to
            // which 'k_MINS_MAX' (and 'k_MINS_MIN') minutes can be added
            // (respectively).

            const Int64 k_SEC_MINUS_MAX = k_SECS_MAX - k_MINS_MAX * 60;
            const Int64 k_SEC_PLUS_MIN  = k_SECS_MIN + k_MINS_MAX * 60;

            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addMins;     // Add mins
            } DATA[] = {
                //line           init secs    nanosecs        add mins
                //----           ---------    --------       --------
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,               123456,     654321,         10000 },
                { L_,               123456,     654321,        -10000 },
                { L_,              -123456,    -654321,         10000 },
                { L_,              -123456,    -654321,        -10000 },
                { L_,                    0,          0,    k_MINS_MAX },
                { L_,                    0,          0,    k_MINS_MIN },
                { L_,      k_SEC_MINUS_MAX,          0,    k_MINS_MAX },
                { L_,       k_SEC_PLUS_MIN,          0,    k_MINS_MIN },
                { L_,           k_SECS_MAX,  999999999,             0 },
                { L_,           k_SECS_MIN, -999999999,             0 },
                { L_,      k_SECS_MAX - 60,  999999999,             1 },
                { L_,      k_SECS_MIN + 60,  999999999,            -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS  = DATA[i].d_initSecs;
                const int                INIT_NSECS = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_MINS   = DATA[i].d_addMins;

                Obj mX; const Obj& X = mX;

                              mX.setInterval(INIT_SECS, INIT_NSECS);
                Obj& RETVAL = mX.addMinutes(ADD_MINS);

                ASSERTV(LINE, &RETVAL == &X);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_MINS * 60, INIT_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addSeconds'\n");
        {

            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addSecs;     // Add secs
            } DATA[] = {
                //line           init secs    nanosecs        add secs
                //----           ---------    --------       --------
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,                    1,          2,           123 },
                { L_,                    1,          2,          -321 },
                { L_,               123456,     654321,         10000 },
                { L_,               123456,     654321,        -10000 },
                { L_,              -123456,    -654321,         10000 },
                { L_,              -123456,    -654321,        -10000 },
                { L_,                    0,          0,    k_SECS_MAX },
                { L_,                    0,          0,    k_SECS_MIN },
                { L_,                    1,          0, -1+k_SECS_MAX },
                { L_,                   -1,          0,  1+k_SECS_MIN },
                { L_,           k_SECS_MAX,  999999999,             0 },
                { L_,           k_SECS_MIN, -999999999,             0 },
                { L_,       k_SECS_MAX - 1,  999999999,             1 },
                { L_,       k_SECS_MIN + 1,  999999999,            -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS  = DATA[i].d_initSecs;
                const int                INIT_NSECS = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_SECS   = DATA[i].d_addSecs;

                Obj mX; const Obj& X = mX;

                              mX.setInterval(INIT_SECS, INIT_NSECS);
                Obj& RETVAL = mX.addSeconds(  ADD_SECS);

                ASSERTV(LINE, &RETVAL == &X);

                if (veryVerbose) { T_;  P_(X); }

                const Obj Y(INIT_SECS + ADD_SECS, INIT_NSECS);

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addMilliseconds'\n");
        {

            // These constants define the maximum (and minimum) second and
            // nanoseconds value to which 'LLONG_MAX' (and 'LLONG_MIN')
            // milliseconds can be added (respectively).

            const Int64  k_S_MINUS_MAX = LLONG_MAX - LLONG_MAX /
                                                     k_MILLISECS_PER_SEC;

            const int   k_NS_MINUS_MAX = static_cast<int>(
                999999999 - (LLONG_MAX % k_MILLISECS_PER_SEC) *
                                                      k_NANOSECS_PER_MILLISEC);

            const Int64  k_S_PLUS_MIN  = LLONG_MIN - LLONG_MIN /
                                                     k_MILLISECS_PER_SEC;

            const int    k_NS_PLUS_MIN = static_cast<int>(
               -999999999 - (LLONG_MIN % k_MILLISECS_PER_SEC) *
                                                      k_NANOSECS_PER_MILLISEC);


            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addMillis;   // Add millis
            } DATA[] = {
                //line        init secs           nanosecs      add millis
                //----        ---------           --------      ----------
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,            123456,               654321,        10000 },
                { L_,            123456,               654321,       -10000 },
                { L_,           -123456,              -654321,        10000 },
                { L_,           -123456,              -654321,       -10000 },
                { L_,                 0,                    0,    LLONG_MAX },
                { L_,                 0,                    0,    LLONG_MIN },
                { L_,     k_S_MINUS_MAX,       k_NS_MINUS_MAX,    LLONG_MAX },
                { L_,      k_S_PLUS_MIN,        k_NS_PLUS_MIN,    LLONG_MIN },
                { L_,        k_SECS_MAX,            999999999,            0 },
                { L_,        k_SECS_MIN,           -999999999,            0 },
                { L_,        k_SECS_MAX,  999999999 - 1000000,            1 },
                { L_,        k_SECS_MIN, -999999999 + 1000000,           -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS  = DATA[i].d_initSecs;
                const int                INIT_NSECS = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_MILLIS = DATA[i].d_addMillis;

                Obj mX; const Obj& X = mX;

                              mX.setInterval(INIT_SECS, INIT_NSECS);
                Obj& RETVAL = mX.addMilliseconds(ADD_MILLIS);

                ASSERTV(LINE, &RETVAL == &X);

                if (veryVerbose) { T_;  P_(LINE); P_(X); }

                const Obj Y(
                    INIT_SECS  + ADD_MILLIS / k_MILLISECS_PER_SEC,
                    static_cast<int>(
                        INIT_NSECS + (ADD_MILLIS % k_MILLISECS_PER_SEC) *
                                                   k_NANOSECS_PER_MILLISEC));

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addMicroseconds'\n");
        {

            // These constants define the maximum (and minimum) second and
            // nanoseconds value to which 'LLONG_MAX' (and 'LLONG_MIN')
            // microseconds can be added (respectively).

            const Int64  k_S_MINUS_MAX = LLONG_MAX - LLONG_MAX /
                                                     k_MICROSECS_PER_SEC;

            const int   k_NS_MINUS_MAX = static_cast<int>(
                999999999 - (LLONG_MAX % k_MICROSECS_PER_SEC) *
                                                      k_NANOSECS_PER_MICROSEC);

            const Int64  k_S_PLUS_MIN  = LLONG_MIN - LLONG_MIN /
                                                     k_MICROSECS_PER_SEC;

            const int    k_NS_PLUS_MIN = static_cast<int>(
               -999999999 - (LLONG_MIN % k_MICROSECS_PER_SEC) *
                                                      k_NANOSECS_PER_MICROSEC);


            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addMicros;   // Add micros
            } DATA[] = {
                //line        init secs           nanosecs      add micros
                //----        ---------           --------      ----------
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,            123456,               654321,        10000 },
                { L_,            123456,               654321,       -10000 },
                { L_,           -123456,              -654321,        10000 },
                { L_,           -123456,              -654321,       -10000 },
                { L_,                 0,                    0,    LLONG_MAX },
                { L_,                 0,                    0,    LLONG_MIN },
                { L_,     k_S_MINUS_MAX,       k_NS_MINUS_MAX,    LLONG_MAX },
                { L_,      k_S_PLUS_MIN,        k_NS_PLUS_MIN,    LLONG_MIN },
                { L_,        k_SECS_MAX,            999999999,            0 },
                { L_,        k_SECS_MIN,           -999999999,            0 },
                { L_,        k_SECS_MAX,     999999999 - 1000,            1 },
                { L_,        k_SECS_MIN,    -999999999 + 1000,           -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS  = DATA[i].d_initSecs;
                const int                INIT_NSECS = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_MICROS = DATA[i].d_addMicros;

                Obj mX; const Obj& X = mX;

                              mX.setInterval(INIT_SECS, INIT_NSECS);
                Obj& RETVAL = mX.addMicroseconds(ADD_MICROS);

                ASSERTV(LINE, &RETVAL == &X);

                if (veryVerbose) { T_;  P_(LINE); P_(X); }

                const Obj Y(
                    INIT_SECS  + ADD_MICROS / k_MICROSECS_PER_SEC,
                    static_cast<int>(
                        INIT_NSECS + (ADD_MICROS % k_MICROSECS_PER_SEC) *
                                                   k_NANOSECS_PER_MICROSEC));

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nTesting: 'addNanoseconds'\n");
        {

            // These constants define the maximum (and minimum) second and
            // nanoseconds value to which 'LLONG_MAX' (and 'LLONG_MIN')
            // nanoseconds can be added (respectively).

            const Int64  k_S_MINUS_MAX = LLONG_MAX - LLONG_MAX /
                                                     k_NANOSECS_PER_SEC;

            const int   k_NS_MINUS_MAX = static_cast<int>(
                                 999999999 - (LLONG_MAX % k_NANOSECS_PER_SEC));

            const Int64  k_S_PLUS_MIN  = LLONG_MIN - LLONG_MIN /
                                                     k_NANOSECS_PER_SEC;

            const int    k_NS_PLUS_MIN = static_cast<int>(
                                -999999999 - (LLONG_MIN % k_NANOSECS_PER_SEC));


            static const struct {
                int                d_lineNum;     // Source line number
                bsls::Types::Int64 d_initSecs;    // Initial seconds
                int                d_initNSecs;   // Initial nanoseconds
                bsls::Types::Int64 d_addNanos;    // Add nanos
            } DATA[] = {
                //line        init secs           nanosecs      add nanos
                //----        ---------           --------      ----------
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,                 1,                    2,          123 },
                { L_,                 1,                    2,         -321 },
                { L_,            123456,               654321,        10000 },
                { L_,            123456,               654321,       -10000 },
                { L_,           -123456,              -654321,        10000 },
                { L_,           -123456,              -654321,       -10000 },
                { L_,                 0,                    0,    LLONG_MAX },
                { L_,                 0,                    0,    LLONG_MIN },
                { L_,     k_S_MINUS_MAX,       k_NS_MINUS_MAX,    LLONG_MAX },
                { L_,      k_S_PLUS_MIN,        k_NS_PLUS_MIN,    LLONG_MIN },
                { L_,        k_SECS_MAX,            999999999,            0 },
                { L_,        k_SECS_MIN,           -999999999,            0 },
                { L_,        k_SECS_MAX,        999999999 - 1,            1 },
                { L_,        k_SECS_MIN,       -999999999 + 1,           -1 },

            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_lineNum;
                const bsls::Types::Int64 INIT_SECS  = DATA[i].d_initSecs;
                const int                INIT_NSECS = DATA[i].d_initNSecs;
                const bsls::Types::Int64 ADD_NANOS  = DATA[i].d_addNanos;

                Obj mX; const Obj& X = mX;

                              mX.setInterval(INIT_SECS, INIT_NSECS);
                Obj& RETVAL = mX.addNanoseconds(ADD_NANOS);

                ASSERTV(LINE, &RETVAL == &X);

                if (veryVerbose) { T_;  P_(LINE); P_(X); }

                const Obj Y(
                    INIT_SECS  + ADD_NANOS / k_NANOSECS_PER_SEC,
                    static_cast<int>(
                        INIT_NSECS + (ADD_NANOS % k_NANOSECS_PER_SEC)));

                if (veryVerbose) { P_(X);  P(Y); }

                ASSERTV(LINE, Y == X);
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) printf("\t'addInterval'\n");
            {
                {
                    // Maximum.

                    Obj mX;
                    ASSERT_PASS(mX.addInterval(LLONG_MAX, 999999999));

                    mX.setInterval(0, 0);

                    ASSERT_FAIL(mX.addInterval(LLONG_MAX, 1000000000));
                }
                {
                    // Minimum.

                    Obj mX;
                    ASSERT_PASS(mX.addInterval(LLONG_MIN, -999999999));

                    mX.setInterval(0, 0);

                    ASSERT_FAIL(mX.addInterval(LLONG_MIN, -1000000000));
                }
            }

            if (veryVerbose) printf("\t'addDays'\n");
            {
                {
                    // Maximum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addDays(k_DAYS_MAX));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addDays(k_DAYS_MAX + 1));

                    mX.setInterval(k_SECS_MAX - 86400,  999999999);

                    ASSERT_SAFE_PASS(mX.addDays(1));

                    mX.setInterval(k_SECS_MAX - 86399,  999999999);

                    ASSERT_SAFE_FAIL(mX.addDays(1));
                }
                {
                    // Minimum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addDays(k_DAYS_MIN));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addDays(k_DAYS_MIN - 1));

                    mX.setInterval(k_SECS_MIN + 86400,  -999999999);

                    ASSERT_SAFE_PASS(mX.addDays(-1));

                    mX.setInterval(k_SECS_MIN + 86399,  -999999999);

                    ASSERT_SAFE_FAIL(mX.addDays(-1));
                }
            }

            if (veryVerbose) printf("\t'addHours'\n");
            {
                {
                    // Maximum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addHours(k_HOURS_MAX));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addHours(k_HOURS_MAX + 1));

                    mX.setInterval(k_SECS_MAX - 3600,  999999999);

                    ASSERT_SAFE_PASS(mX.addHours(1));

                    mX.setInterval(k_SECS_MAX - 3599,  999999999);

                    ASSERT_SAFE_FAIL(mX.addHours(1));
                }
                {
                    // Minimum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addHours(k_HOURS_MIN));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addHours(k_HOURS_MIN - 1));

                    mX.setInterval(k_SECS_MIN + 3600,  -999999999);

                    ASSERT_SAFE_PASS(mX.addHours(-1));

                    mX.setInterval(k_SECS_MIN + 3599,  -999999999);

                    ASSERT_SAFE_FAIL(mX.addHours(-1));
                }
            }

            if (veryVerbose) printf("\t'addMinutes'\n");
            {
                {
                    // Maximum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addMinutes(k_MINS_MAX));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addMinutes(k_MINS_MAX + 1));

                    mX.setInterval(k_SECS_MAX - 60,  999999999);

                    ASSERT_SAFE_PASS(mX.addMinutes(1));

                    mX.setInterval(k_SECS_MAX - 59,  999999999);

                    ASSERT_SAFE_FAIL(mX.addMinutes(1));
                }
                {
                    // Minimum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addMinutes(k_MINS_MIN));

                    mX.setInterval(0, 0);

                    ASSERT_SAFE_FAIL(mX.addMinutes(k_MINS_MIN - 1));

                    mX.setInterval(k_SECS_MIN + 60,  -999999999);

                    ASSERT_SAFE_PASS(mX.addMinutes(-1));

                    mX.setInterval(k_SECS_MIN + 59,  -999999999);

                    ASSERT_SAFE_FAIL(mX.addMinutes(-1));
                }
            }

            if (veryVerbose) printf("\t'addSeconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addSeconds(LLONG_MAX));

                    mX.setInterval(1, 0);

                    ASSERT_SAFE_FAIL(mX.addSeconds(LLONG_MAX));

                    mX.setInterval(LLONG_MAX - 1,  999999999);

                    ASSERT_SAFE_PASS(mX.addSeconds(1));

                    mX.setInterval(LLONG_MAX,  999999999);

                    ASSERT_SAFE_FAIL(mX.addSeconds(1));
                }
                {
                    // Minimum

                    Obj mX;
                    ASSERT_SAFE_PASS(mX.addSeconds(LLONG_MIN));

                    mX.setInterval(-1, 0);

                    ASSERT_SAFE_FAIL(mX.addSeconds(LLONG_MIN));

                    mX.setInterval(LLONG_MIN + 1,  -999999999);

                    ASSERT_SAFE_PASS(mX.addSeconds(-1));

                    mX.setInterval(LLONG_MIN,  -999999999);

                    ASSERT_SAFE_FAIL(mX.addSeconds(-1));
                }
            }

            if (veryVerbose) printf("\t'addMilliseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;

                    mX.setInterval(LLONG_MAX, 999999999 - 1000000);

                    ASSERT_PASS(mX.addMilliseconds(1));

                    mX.setInterval(LLONG_MAX, 999999999 - 999999);

                    ASSERT_FAIL(mX.addMilliseconds(1));
                }
                {
                    // Minimum.

                    Obj mX;

                    mX.setInterval(LLONG_MIN, -999999999 + 1000000);

                    ASSERT_PASS(mX.addMilliseconds(-1));

                    mX.setInterval(LLONG_MIN, -999999999 + 999999);

                    ASSERT_FAIL(mX.addMilliseconds(-1));
                }
            }

            if (veryVerbose) printf("\t'addMicroseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;

                    mX.setInterval(LLONG_MAX, 999999999 - 1000);

                    ASSERT_PASS(mX.addMicroseconds(1));

                    mX.setInterval(LLONG_MAX, 999999999 - 999);

                    ASSERT_FAIL(mX.addMicroseconds(1));
                }
                {
                    // Minimum.

                    Obj mX;

                    mX.setInterval(LLONG_MIN, -999999999 + 1000);

                    ASSERT_PASS(mX.addMicroseconds(-1));

                    mX.setInterval(LLONG_MIN, -999999999 + 999);

                    ASSERT_FAIL(mX.addMicroseconds(-1));
                }
            }

            if (veryVerbose) printf("\t'addNanoseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;

                    mX.setInterval(LLONG_MAX, 999999999 - 1);

                    ASSERT_PASS(mX.addNanoseconds(1));

                    mX.setInterval(LLONG_MAX, 999999999);

                    ASSERT_FAIL(mX.addNanoseconds(1));
                }
                {
                    // Minimum.

                    Obj mX;

                    mX.setInterval(LLONG_MIN, -999999999 + 1);

                    ASSERT_PASS(mX.addNanoseconds(-1));

                    mX.setInterval(LLONG_MIN, -999999999);

                    ASSERT_FAIL(mX.addNanoseconds(-1));
                }
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // 'total*' ACCESSORS
        //   Ensure each 'total*' accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the "total" value corresponding to the
        //:   units indicated by the method's name.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 The 'totalSecondsAsDouble' method does not lose precision in the
        //:   conversion to 'double' of reasonably large time intervals.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        //
        // Plan:
        //   In case 15, we demonstrated that each "total" accessor (with the
        //   exception of 'totalSecondsAsDouble') works properly when invoked
        //   immediately following an application of its corresponding "total"
        //   setter (e.g., 'setTotalMinutes' followed by 'totalMinutes').  Here
        //   we use a more varied set of object values to further corroborate
        //   that these accessors properly interpret object state, and that
        //   'totalSecondsAsDouble' works as expected.
        //
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their canonical
        //:   representation.
        //:
        //: 2 For each row 'R' in the table of P-1   (C-1..3)
        //:
        //:   1 Use the value constructor to create a 'const' 'Obj', 'X', with
        //:     the value from 'R'.
        //:
        //:   2 Verify that each 'totalSeconds', 'totalHours', and 'totalDays'
        //:     return values consistent with 'seconds' (C-1..2)
        //:
        //:   3 In the case of 'totalSecondsAsDouble', verification of that
        //:     method is restricted to those values that should not lose
        //:     precision when converted among 'Int64' and 'double'.  (C-3)
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) with which to exercise the
        //:   'totalMilliseconds' accessor (note that the range of
        //:   'totalMilliseconds' is restricted to a subset of the range of
        //:   valid 'TimeInterval' values).  Test 'totalMilliseconds' against
        //:   the expected number of milliseconds. (C-1..2)
        //:
        //: 4 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) with which to exercise the
        //:   'totalMicroseconds' accessor (note that the range of
        //:   'totalMicroseconds' is restricted to a subset of the range of
        //:   valid 'TimeInterval' values).  Test 'totalMicroseconds' against
        //:   the expected number of microseconds.  (C-1..2)
        //:
        //: 5 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) with which to exercise the
        //:   'totalNanoseconds' accessor (note that the range of
        //:   'totalNanoseconds' is restricted to a subset of the range of
        //:   valid 'TimeInterval' values).  Test 'totalNanoseconds' against
        //:   the expected number of nanoseconds.  (C-1..2)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   Int64 totalSeconds() const;
        //   Int64 totalMinutes() const;
        //   Int64 totalHours() const;
        //   Int64 totalDays() const;
        //   Int64 totalMicroseconds() const;
        //   Int64 totalMilliseconds() const;
        //   Int64 totalNanoseconds() const;
        //   double totalSecondsAsDouble() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\n'total*' ACCESSORS"
                            "\n==================\n");

        // These constants define the seconds and milliseconds attributes of
        // the canonical representations for the maximum and minimum
        // 'TimeInterval' value upon which you can call 'milliseconds' and
        // 'microseconds (respectively).

        // Nanoseconds.

        const Int64 k_MAX_NANOSECS_SECS  = LLONG_MAX / k_NANOSECS_PER_SEC;
        const int   k_MAX_NANOSECS_NANOS = LLONG_MAX % k_NANOSECS_PER_SEC;

        const Int64 k_MIN_NANOSECS_SECS  = LLONG_MIN / k_NANOSECS_PER_SEC;
        const int   k_MIN_NANOSECS_NANOS = LLONG_MIN % k_NANOSECS_PER_SEC;

        // Microseconds.

        const Int64 k_MAX_MICROSECS_SECS  = LLONG_MAX / k_MICROSECS_PER_SEC;
        const int   k_MAX_MICROSECS_NANOS =
           (LLONG_MAX % k_MICROSECS_PER_SEC + 1) * k_NANOSECS_PER_MICROSEC - 1;

        const Int64 k_MIN_MICROSECS_SECS  = LLONG_MIN / k_MICROSECS_PER_SEC;
        const int   k_MIN_MICROSECS_NANOS =
           (LLONG_MIN % k_MICROSECS_PER_SEC - 1) * k_NANOSECS_PER_MICROSEC + 1;

        // Milliseconds.

        const Int64 k_MAX_MILLISECS_SECS  = LLONG_MAX / k_MILLISECS_PER_SEC;
        const int   k_MAX_MILLISECS_NANOS =
           (LLONG_MAX % k_MILLISECS_PER_SEC + 1) * k_NANOSECS_PER_MILLISEC - 1;

        const Int64 k_MIN_MILLISECS_SECS  = LLONG_MIN / k_MILLISECS_PER_SEC;
        const int   k_MIN_MILLISECS_NANOS =
           (LLONG_MIN % k_MILLISECS_PER_SEC - 1) * k_NANOSECS_PER_MILLISEC + 1;

        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,           513454,     2131241 },
            { L_,          -123123,      982173 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
            { L_,        LLONG_MAX,   999999999 },
            { L_,        LLONG_MIN,  -999999999 },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf(
           "\nTest secs, mins, hours, days, totalSecondsAsDouble w/ table.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 SECONDS = DATA[ti].d_seconds;
            const int                NSECS   = DATA[ti].d_nsecs;


            const Obj X(SECONDS, NSECS);

            if (veryVerbose) { T_ P_(LINE) P(X) }

            ASSERTV(LINE, X, X.totalDays(),
                    X.seconds() / k_SECS_PER_DAY == X.totalDays());

            ASSERTV(LINE, X, X.totalHours(),
                    X.seconds() / k_SECS_PER_HOUR == X.totalHours());

            ASSERTV(LINE, X, X.totalMinutes(),
                    X.seconds() / k_SECS_PER_MIN == X.totalMinutes());

            ASSERTV(LINE, X, X.totalSeconds(),
                    X.seconds() +
                    X.nanoseconds() / k_NANOSECS_PER_SEC == X.totalSeconds());

            // The intermediate, volatile values are required to ensure that
            // the assertion compares two variables of type 'double', not one
            // variable and one floating point expression.  A floating-point
            // expression is allowed to have more precision than a variable of
            // type 'double'.

            volatile double EXP =
                static_cast<double>(X.seconds()) +
                static_cast<double>(X.nanoseconds()) / k_NANOSECS_PER_SEC;

            volatile double RESULT = X.totalSecondsAsDouble();

            ASSERTV(LINE, X, EXP, RESULT, EXP == RESULT);
        }

        if (verbose) printf("\nTest milliseconds with test table.\n");

        {
            static const struct {
                    int                d_lineNum;
                    bsls::Types::Int64 d_seconds;
                    int                d_nsecs;
            } DATA[] = {
                //line              seconds                 nanosecs
                //----             --------                 --------
                { L_,                     0,                       0 },
                { L_,                     1,                       1 },
                { L_,                     1,               999999999 },
                { L_,                    -1,                      -1 },
                { L_,                    -1,              -999999999 },
                { L_,                513454,                 2131241 },
                { L_,               -123123,                  982173 },
                { L_,  k_MAX_MILLISECS_SECS,   k_MAX_MILLISECS_NANOS },
                { L_,  k_MIN_MILLISECS_SECS,   k_MIN_MILLISECS_NANOS },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                LINE    = DATA[ti].d_lineNum;
                const bsls::Types::Int64 SECONDS = DATA[ti].d_seconds;
                const int                NSECS   = DATA[ti].d_nsecs;

                const Obj X(SECONDS, NSECS);

                if (veryVerbose) { T_ P_(LINE) P(X) }

                const Int64 EXP = X.seconds()     * k_MILLISECS_PER_SEC +
                                  X.nanoseconds() / k_NANOSECS_PER_MILLISEC;

                ASSERTV(LINE, X, EXP, X.totalMilliseconds(),
                        EXP == X.totalMilliseconds());
            }
        }

        if (verbose) printf("\nTest microseconds with test table.\n");

        {

            static const struct {
                int                d_lineNum;
                bsls::Types::Int64 d_seconds;
                int                d_nsecs;
            } DATA[] = {
                //line              seconds                 nanosecs
                //----             --------                 --------
                { L_,                     0,                       0 },
                { L_,                     1,                       1 },
                { L_,                     1,               999999999 },
                { L_,                    -1,                      -1 },
                { L_,                    -1,              -999999999 },
                { L_,                513454,                 2131241 },
                { L_,               -123123,                  982173 },
                { L_,  k_MAX_MICROSECS_SECS,   k_MAX_MICROSECS_NANOS },
                { L_,  k_MIN_MICROSECS_SECS,   k_MIN_MICROSECS_NANOS },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                LINE    = DATA[ti].d_lineNum;
                const bsls::Types::Int64 SECONDS = DATA[ti].d_seconds;
                const int                NSECS   = DATA[ti].d_nsecs;

                const Obj X(SECONDS, NSECS);

                if (veryVerbose) { T_ P_(LINE) P(X) }

                const Int64 EXP = X.seconds()     * k_MICROSECS_PER_SEC +
                                  X.nanoseconds() / k_NANOSECS_PER_MICROSEC;

                ASSERTV(LINE, X, EXP, X.totalMicroseconds(),
                        EXP == X.totalMicroseconds());
            }
        }

        if (verbose) printf("\nTest nanoseconds with test table.\n");

        {

            static const struct {
                int                d_lineNum;
                bsls::Types::Int64 d_seconds;
                int                d_nsecs;
            } DATA[] = {
                //line              seconds                 nanosecs
                //----             --------                 --------
                { L_,                     0,                       0 },
                { L_,                     1,                       1 },
                { L_,                     1,               999999999 },
                { L_,                    -1,                      -1 },
                { L_,                    -1,              -999999999 },
                { L_,                513454,                 2131241 },
                { L_,               -123123,                  982173 },
                { L_,   k_MAX_NANOSECS_SECS,    k_MAX_NANOSECS_NANOS },
                { L_,   k_MIN_NANOSECS_SECS,    k_MIN_NANOSECS_NANOS },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                LINE    = DATA[ti].d_lineNum;
                const bsls::Types::Int64 SECONDS = DATA[ti].d_seconds;
                const int                NSECS   = DATA[ti].d_nsecs;

                const Obj X(SECONDS, NSECS);

                if (veryVerbose) { T_ P_(LINE) P(X) }

                const Int64 EXP =
                    X.seconds() * k_NANOSECS_PER_SEC + X.nanoseconds();

                ASSERTV(LINE, X, EXP, X.totalNanoseconds(),
                        EXP == X.totalNanoseconds());
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) printf("\t'totalMilliseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MAX_MILLISECS_SECS,
                                      k_MAX_MILLISECS_NANOS);

                    ASSERT_SAFE_PASS(X.totalMilliseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_FAIL(X.totalMilliseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_PASS(X.totalMilliseconds());

                    mX.setIntervalRaw(LLONG_MAX, 999999999);

                    ASSERT_SAFE_FAIL(X.totalMilliseconds());

                }
                {
                    // Minimum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MIN_MILLISECS_SECS,
                                      k_MIN_MILLISECS_NANOS);


                    ASSERT_SAFE_PASS(X.totalMilliseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_FAIL(X.totalMilliseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_PASS(X.totalMilliseconds());

                    mX.setIntervalRaw(LLONG_MIN, -999999999);

                    ASSERT_SAFE_FAIL(X.totalMilliseconds());

                }
            }

            if (veryVerbose) printf("\t'totalMicroseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MAX_MICROSECS_SECS,
                                      k_MAX_MICROSECS_NANOS);


                    ASSERT_SAFE_PASS(X.totalMicroseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_FAIL(X.totalMicroseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_PASS(X.totalMicroseconds());

                    mX.setIntervalRaw(LLONG_MAX, 999999999);

                    ASSERT_SAFE_FAIL(X.totalMicroseconds());

                }
                {
                    // Minimum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MIN_MICROSECS_SECS,
                                      k_MIN_MICROSECS_NANOS);


                    ASSERT_SAFE_PASS(X.totalMicroseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_FAIL(X.totalMicroseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_PASS(X.totalMicroseconds());

                    mX.setIntervalRaw(LLONG_MIN, 0);

                    ASSERT_SAFE_FAIL(X.totalMicroseconds());

                }
            }
            if (veryVerbose) printf("\t'totalNanoseconds'\n");
            {
                {
                    // Maximum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MAX_NANOSECS_SECS,
                                      k_MAX_NANOSECS_NANOS);


                    ASSERT_SAFE_PASS(X.totalNanoseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_FAIL(X.totalNanoseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_PASS(X.totalNanoseconds());

                    mX.setIntervalRaw(LLONG_MAX, 999999999);

                    ASSERT_SAFE_FAIL(X.totalNanoseconds());

                }
                {
                    // Minimum.

                    Obj mX;  const Obj& X = mX;
                    mX.setIntervalRaw(k_MIN_NANOSECS_SECS,
                                      k_MIN_NANOSECS_NANOS);


                    ASSERT_SAFE_PASS(X.totalNanoseconds());

                    mX.addNanoseconds(-1);

                    ASSERT_SAFE_FAIL(X.totalNanoseconds());

                    mX.addNanoseconds(1);

                    ASSERT_SAFE_PASS(X.totalNanoseconds());

                    mX.setIntervalRaw(LLONG_MIN, 0);

                    ASSERT_SAFE_FAIL(X.totalNanoseconds());

                }
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // 'setTotal*' MANIPULATORS
        //   Ensure that each method correctly computes the underlying
        //   canonical representation of the object.
        //
        // Concerns:
        //: 1 Each manipulator can set an object to have any "total" value that
        //:   does not violate the method's documented preconditions.
        //:
        //: 2 Each manipulator is not affected by the state of the object on
        //:   entry.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 To test 'setTotalDays', use the table-driven technique to specify
        //:   a set of distinct object values (one per row) in terms of their
        //:   total-days representation.
        //:
        //: 2 For each row 'R1' in the table of P-1:  (C-1..2)
        //:
        //:   1 Use the default constructor and 'setIntervalRaw' to
        //:     create an object, 'W', having the value from 'R1' interpreted
        //:     as total milliseconds.  (This expedient reuse of 'R1' is for
        //:     giving 'W' a unique value in each iteration of the loop.)
        //:
        //:   2 For each row 'R2' in the table of P-1:  (C-1..2)
        //:
        //:     1 Use the copy constructor to create an object, 'X', from 'W'.
        //:
        //:     2 Use 'setTotalDays' to set 'X' to have the total number of
        //:       days from 'R2'.
        //:
        //:     3 Create an object 'EXP' using the previously tested
        //:       'setIntervalRaw' function.
        //:
        //:     4 Verify 'EXP' equals 'X'.
        //:
        //: 3 Repeat steps similar to those described in P-1..2 to test
        //:   'setTotalHours' ('setTotalMinutes', 'setTotalSeconds',
        //:   'setTotalMilliseconds', 'setTotalMicroseconds',
        //:   'setTotalNanoseconds') except that, this time (a) the rows of
        //:   the table (P-1) are in terms of total hours (total minutes,
        //:   total seconds, total milliseconds, total microseconds, total
        //:   nanoseconds) instead of total days, and (b) 'setTotalHours'
        //:   ('setTotalMinutes', 'setTotalSeconds', 'setTotalMilliseconds',
        //:   'setTotalMicroseconds', 'setTotalNanoseconds') is applied
        //:   instead of 'setTotalDays'.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   void setTotalDays(int);
        //   void setTotalHours(Int64);
        //   void setTotalMinutes(Int64);
        //   void setTotalSeconds(Int64);
        //   void setTotalMilliseconds(Int64);
        //   void setTotalMicroseconds(Int64);
        //   void setTotalNanoseconds(Int64);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'setTotal*' MANIPULATORS"
                            "\n========================\n");

        if (verbose) printf("\nTesting 'setTotalDays'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalDays;
            } DATA[] = {
                //LINE   TOTAL DAYS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,    k_DAYS_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,    k_DAYS_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE       = DATA[ti].d_line;
                const Int64 ITOTAL_DAYS = DATA[ti].d_totalDays;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_DAYS * 86400, 0);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE       = DATA[tj].d_line;
                    const Int64 JTOTAL_DAYS = DATA[tj].d_totalDays;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalDays(JTOTAL_DAYS);

                    Obj exp(JTOTAL_DAYS * 86400, 0); const Obj& EXP = exp;

                    if (veryVeryVerbose) { T_ P_(X) P_(EXP) P(JTOTAL_DAYS) }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);

                    ASSERTV(JTOTAL_DAYS, X, JTOTAL_DAYS == X.totalDays());
                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalHours'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalHours;
            } DATA[] = {
                //LINE   TOTAL HOURS
                //----   -----------
                { L_,              0 },

                { L_,              1 },
                { L_,          13027 },
                { L_,    k_HOURS_MAX },

                { L_,             -1 },
                { L_,         -42058 },
                { L_,    k_HOURS_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE        = DATA[ti].d_line;
                const Int64 ITOTAL_HOURS = DATA[ti].d_totalHours;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_HOURS * 3600, 0);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE        = DATA[tj].d_line;
                    const Int64 JTOTAL_HOURS = DATA[tj].d_totalHours;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalHours(JTOTAL_HOURS);

                    Obj exp(JTOTAL_HOURS * 3600, 0); const Obj& EXP = exp;

                    if (veryVeryVerbose) { T_ P_(X) P_(EXP) P(JTOTAL_HOURS) }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);

                    ASSERTV(JTOTAL_HOURS, X, JTOTAL_HOURS == X.totalHours());
                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalMinutes'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalMins;
            } DATA[] = {
                //LINE   TOTAL MINS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,    k_MINS_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,    k_MINS_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE       = DATA[ti].d_line;
                const Int64 ITOTAL_MINS = DATA[ti].d_totalMins;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_MINS * 60, 0);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE       = DATA[tj].d_line;
                    const Int64 JTOTAL_MINS = DATA[tj].d_totalMins;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalMinutes(JTOTAL_MINS);

                    Obj exp(JTOTAL_MINS * 60, 0); const Obj& EXP = exp;

                    if (veryVeryVerbose) { T_ P_(X) P_(EXP) P(JTOTAL_MINS) }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);

                    ASSERTV(JTOTAL_MINS, X, JTOTAL_MINS == X.totalMinutes());
                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalSeconds'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalSecs;
            } DATA[] = {
                //LINE   TOTAL SECS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,    k_SECS_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,    k_SECS_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE       = DATA[ti].d_line;
                const Int64 ITOTAL_SECS = DATA[ti].d_totalSecs;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_SECS, 0);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE       = DATA[tj].d_line;
                    const Int64 JTOTAL_SECS = DATA[tj].d_totalSecs;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalSeconds(JTOTAL_SECS);

                    Obj exp(JTOTAL_SECS, 0); const Obj& EXP = exp;

                    if (veryVeryVerbose) { T_ P_(X) P_(EXP) P(JTOTAL_SECS) }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);


                    ASSERTV(JTOTAL_SECS, X, JTOTAL_SECS == X.totalSeconds());
                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalMilliseconds'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalMillisecs;
            } DATA[] = {
                //LINE   TOTAL MILLISECS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,     LLONG_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,     LLONG_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE            = DATA[ti].d_line;
                const Int64 ITOTAL_MILLISECS = DATA[ti].d_totalMillisecs;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_MILLISECS / k_MILLISECS_PER_SEC,
                               static_cast<int>(
                                (ITOTAL_MILLISECS % k_MILLISECS_PER_SEC)
                                                  * k_NANOSECS_PER_MILLISEC));

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE            = DATA[tj].d_line;
                    const Int64 JTOTAL_MILLISECS = DATA[tj].d_totalMillisecs;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalMilliseconds(JTOTAL_MILLISECS);

                    Obj exp(JTOTAL_MILLISECS / k_MILLISECS_PER_SEC,
                            static_cast<int>(
                                (JTOTAL_MILLISECS % k_MILLISECS_PER_SEC)
                                                  * k_NANOSECS_PER_MILLISEC));
                    const Obj& EXP = exp;

                    if (veryVeryVerbose) {
                        T_ P_(X) P_(EXP) P(JTOTAL_MILLISECS);
                    }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);

                    ASSERTV(JTOTAL_MILLISECS, X,
                            JTOTAL_MILLISECS == X.totalMilliseconds());
                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalMicroseconds'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalMicrosecs;
            } DATA[] = {
                //LINE   TOTAL MICROSECS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,     LLONG_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,     LLONG_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE            = DATA[ti].d_line;
                const Int64 ITOTAL_MICROSECS = DATA[ti].d_totalMicrosecs;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_MICROSECS / k_MICROSECS_PER_SEC,
                               static_cast<int>(
                                (ITOTAL_MICROSECS % k_MICROSECS_PER_SEC)
                                                  * k_NANOSECS_PER_MICROSEC));

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE            = DATA[tj].d_line;
                    const Int64 JTOTAL_MICROSECS = DATA[tj].d_totalMicrosecs;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalMicroseconds(JTOTAL_MICROSECS);

                    Obj exp(JTOTAL_MICROSECS / k_MICROSECS_PER_SEC,
                            static_cast<int>(
                                (JTOTAL_MICROSECS % k_MICROSECS_PER_SEC)
                                                  * k_NANOSECS_PER_MICROSEC));
                    const Obj& EXP = exp;

                    if (veryVeryVerbose) {
                        T_ P_(X) P_(EXP) P(JTOTAL_MICROSECS);
                    }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);

                    ASSERTV(JTOTAL_MICROSECS, X,
                            JTOTAL_MICROSECS == X.totalMicroseconds());

                }
            }
        }

        if (verbose) printf("\nTesting 'setTotalNanoseconds'.\n");
        {
            static const struct {
                int    d_line;       // source line number
                Int64  d_totalNanosecs;
            } DATA[] = {
                //LINE   TOTAL NANOSECS
                //----   ----------
                { L_,             0 },

                { L_,             1 },
                { L_,         13027 },
                { L_,     LLONG_MAX },

                { L_,            -1 },
                { L_,        -42058 },
                { L_,     LLONG_MIN },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   ILINE            = DATA[ti].d_line;
                const Int64 ITOTAL_NANOSECS = DATA[ti].d_totalNanosecs;

                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ITOTAL_NANOSECS / k_NANOSECS_PER_SEC,
                               static_cast<int>(
                               ITOTAL_NANOSECS % k_NANOSECS_PER_SEC));

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int   JLINE            = DATA[tj].d_line;
                    const Int64 JTOTAL_NANOSECS = DATA[tj].d_totalNanosecs;

                    Obj mX(W);  const Obj& X = mX;

                    mX.setTotalNanoseconds(JTOTAL_NANOSECS);

                    Obj exp(JTOTAL_NANOSECS / k_NANOSECS_PER_SEC,
                            static_cast<int>(
                            JTOTAL_NANOSECS % k_NANOSECS_PER_SEC));

                    const Obj& EXP = exp;

                    if (veryVeryVerbose) {
                        T_ P_(X) P_(EXP) P(JTOTAL_NANOSECS);
                    }

                    ASSERTV(ILINE, JLINE, EXP, X, EXP == X);
                }
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) printf("\t'setTotalDays'\n");
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setTotalDays(k_DAYS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setTotalDays(k_DAYS_MIN   ));

                ASSERT_SAFE_PASS(mX.setTotalDays(k_DAYS_MAX   ));
                ASSERT_SAFE_FAIL(mX.setTotalDays(k_DAYS_MAX + 1));
            }

            if (veryVerbose) printf("\t'setTotalHours'\n");
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setTotalHours(k_HOURS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setTotalHours(k_HOURS_MIN   ));

                ASSERT_SAFE_PASS(mX.setTotalHours(k_HOURS_MAX   ));
                ASSERT_SAFE_FAIL(mX.setTotalHours(k_HOURS_MAX + 1));
            }

            if (veryVerbose) printf("\t'setTotalMinutes'\n");
            {
                Obj mX;

                ASSERT_SAFE_FAIL(mX.setTotalMinutes(k_MINS_MIN - 1));
                ASSERT_SAFE_PASS(mX.setTotalMinutes(k_MINS_MIN   ));

                ASSERT_SAFE_PASS(mX.setTotalMinutes(k_MINS_MAX   ));
                ASSERT_SAFE_FAIL(mX.setTotalMinutes(k_MINS_MAX + 1));
            }

            // 'setTotalSeconds', 'setTotalMilliseconds',
            // 'setTotalMicroseconds' and 'setTotalNanoseconds' do not have
            // undefined behavior.
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // RELATIONAL-COMPARISON OPERATORS (<, <=, >, >=)
        //   Ensure that each operator defines the correct relationship between
        //   the underlying canonical representations of its operands.
        //
        // Concerns:
        //: 1 An object 'X' is in relation to an object 'Y' as the
        //:   canonical representation of 'X' is in relation to the
        //:   canonical representation of 'Y'.
        //:
        //: 2 'false == (X <  X)' (i.e., irreflexivity).
        //:
        //: 3 'true  == (X <= X)' (i.e., reflexivity).
        //:
        //: 4 'false == (X >  X)' (i.e., irreflexivity).
        //:
        //: 5 'true  == (X >= X)' (i.e., reflexivity).
        //:
        //: 6 If 'X < Y', then '!(Y < X)' (i.e., asymmetry).
        //:
        //: 7 'X <= Y' if and only if 'X < Y' exclusive-or 'X == Y'.
        //:
        //: 8 If 'X > Y', then '!(Y > X)' (i.e., asymmetry).
        //:
        //: 9 'X >= Y' if and only if 'X > Y' exclusive-or 'X == Y'.
        //:
        //:10 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., all relational-comparison operators are free functions).
        //:
        //:11 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:12 The relational-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator<', 'operator<=',
        //:   'operator>', and 'operator>=' to initialize function pointers
        //:   having the appropriate signatures and return types for the 12
        //:   relational-comparison operators defined in this component.
        //:   (C-10..12)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their second and
        //:   milliseconds.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..9)
        //:
        //:   1 Use the default constructor and 'setIntervalRaw' to
        //:     create an object, 'W', having the value from 'R1'.
        //:
        //:   2 Using 'W', verify the anti-reflexive (reflexive) property of
        //:     '<' and '>' ('<=' and '>=') in the presence of aliasing.
        //:     (C-2..5)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 6..9)
        //:
        //:     1 Use the default constructor and 'setIntervalRaw' to create an
        //:       object, 'X', having the value from 'R1'.
        //:
        //:     2 Use the default constructor and 'setIntervalRaw' to create a
        //:       second object, 'Y', having the value from 'R2'.
        //:
        //:     3 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare less than a (distinct)
        //:       object set to the value from 'R2'.
        //:
        //:     4 Using 'X' and 'Y', verify the asymmetric property and
        //:       expected return value for '<'.  (C-6)
        //:
        //:     5 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare less or equal to a
        //:       (distinct) object set to the value from 'R2'.
        //:
        //:     6 Using 'X' and 'Y', verify the expected return value for '<='.
        //:       (C-7)
        //:
        //:     7 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare greater than a (distinct)
        //:       object set to the value from 'R2'.
        //:
        //:     8 Using 'X' and 'Y', verify the asymmetric property and
        //:       expected return value for '>'.  (C-8)
        //:
        //:     9 Record, in 'EXP', whether or not an object set to the value
        //:       from 'R1' is expected to compare greater than or equal to a
        //:       (distinct) object set to the value from 'R2'.
        //:
        //:    10 Using 'X' and 'Y', verify the expected return value for '>='.
        //:       (C-1, 9)
        //
        // Testing:
        //   bool operator< (const TimeInterval&, const TimeInterval&);
        //   bool operator< (const TimeInterval&, double);
        //   bool operator< (double, const TimeInterval&);
        //   bool operator<=(const TimeInterval&, const TimeInterval&);
        //   bool operator<=(const TimeInterval&, double);
        //   bool operator<=(double, const TimeInterval&);
        //   bool operator> (const TimeInterval&, const TimeInterval&);
        //   bool operator> (const TimeInterval&, double);
        //   bool operator> (double, const TimeInterval&);
        //   bool operator<=(const TimeInterval&, const TimeInterval&);
        //   bool operator<=(const TimeInterval&, double);
        //   bool operator>=(double, const TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) printf(
                         "\nRELATIONAL-COMPARISON OPERATORS (<, <=, >, >=)"
                         "\n==============================================\n");

        if (verbose) printf(
                "\nAssign the address of each operator to a variable.\n");
        {
            {
                typedef bool (*operatorPtr)(const Obj&, const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorLt = bsls::operator<;
                operatorPtr operatorLe = bsls::operator<=;
                operatorPtr operatorGt = bsls::operator>;
                operatorPtr operatorGe = bsls::operator>=;

                (void)operatorLt;  // quash potential compiler warnings
                (void)operatorLe;
                (void)operatorGt;
                (void)operatorGe;
            }
            {
                typedef bool (*operatorPtr)(double, const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorLt = bsls::operator<;
                operatorPtr operatorLe = bsls::operator<=;
                operatorPtr operatorGt = bsls::operator>;
                operatorPtr operatorGe = bsls::operator>=;

                (void)operatorLt;  // quash potential compiler warnings
                (void)operatorLe;
                (void)operatorGt;
                (void)operatorGe;
            }
            {
                typedef bool (*operatorPtr)(const Obj&, double);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorLt = bsls::operator<;
                operatorPtr operatorLe = bsls::operator<=;
                operatorPtr operatorGt = bsls::operator>;
                operatorPtr operatorGe = bsls::operator>=;

                (void)operatorLt;  // quash potential compiler warnings
                (void)operatorLe;
                (void)operatorGt;
                (void)operatorGe;
            }
        }
        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
            { L_,        LLONG_MAX,   999999999 },
            { L_,        LLONG_MIN,  -999999999 },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);


        if (verbose) printf("\nCompare every value with every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                ILINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 ISECONDS = DATA[ti].d_seconds;
            const int                INSECS   = DATA[ti].d_nsecs;


            if (veryVerbose) { T_ P_(ILINE) P_(ISECONDS) P(INSECS) }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ISECONDS, INSECS);

                ASSERTV(ILINE, W, !(W <  W));
                ASSERTV(ILINE, W,   W <= W);
                ASSERTV(ILINE, W, !(W >  W));
                ASSERTV(ILINE, W,   W >= W);
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int                JLINE    = DATA[tj].d_lineNum;
                const bsls::Types::Int64 JSECONDS = DATA[tj].d_seconds;
                const int                JNSECS   = DATA[tj].d_nsecs;

                double dIv = static_cast<double>(ISECONDS) +
                                 static_cast<double>(INSECS) /
                                 static_cast<double>(k_NANOSECS_PER_SEC);
                double dJv = static_cast<double>(JSECONDS) +
                                 static_cast<double>(JNSECS) /
                                 static_cast<double>(k_NANOSECS_PER_SEC);

                if (veryVerbose) { T_ P_(JLINE) P_(JSECONDS) P(JNSECS) }

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(ISECONDS, INSECS);

                Obj mY;  const Obj& Y = mY;
                mY.setIntervalRaw(JSECONDS, JNSECS);

                if (veryVerbose) { T_ T_ P_(X) P(Y) }

                // Verify 'operator<'.

                {
                    const bool EXP = dIv < dJv;

                    ASSERTV(ILINE, JLINE, X, Y, EXP == (X   < Y));

                    if (ISECONDS != LLONG_MIN && ISECONDS != LLONG_MAX &&
                        JSECONDS != LLONG_MIN && JSECONDS != LLONG_MAX &&
                        ti != tj) {
                        // Tests near edge values are disabled for comparisons
                        // to doubles, as the conversions double require
                        // rounding, which breaks the test.

                        ASSERTV(ILINE, JLINE, X, Y, EXP == (X   < dJv));
                        ASSERTV(ILINE, JLINE, X, Y, EXP == (dIv < Y));
                    }

                    if (EXP) {
                        ASSERTV(ILINE, JLINE, Y, X, !(Y < X));
                    }
                }

                // Verify 'operator<='.

                {
                    const bool EXP = dIv <= dJv;

                    ASSERTV(ILINE, JLINE, X, Y, EXP == (X   <= Y));

                    if (ISECONDS != LLONG_MIN && ISECONDS != LLONG_MAX &&
                        JSECONDS != LLONG_MIN && JSECONDS != LLONG_MAX &&
                        ti != tj) {
                        // Tests near edge values are disabled for comparisons
                        // to doubles, as the conversions double require
                        // rounding, which breaks the test.

                        ASSERTV(ILINE, JLINE, X, Y, EXP == (X   <= dJv));
                        ASSERTV(ILINE, JLINE, X, Y, EXP == (dIv <= Y));
                    }
                    ASSERTV(ILINE, JLINE, X, Y,
                                 EXP == ((X < Y) ^ (X == Y)));

                }

                // Verify 'operator>'.

                {
                    const bool EXP = dIv > dJv;

                    ASSERTV(ILINE, JLINE, X, Y, EXP == (X   > Y));

                    if (ISECONDS != LLONG_MIN && ISECONDS != LLONG_MAX &&
                        JSECONDS != LLONG_MIN && JSECONDS != LLONG_MAX &&
                        ti != tj) {
                        // Tests near edge values are disabled for comparisons
                        // to doubles, as the conversions double require
                        // rounding, which breaks the test.

                        ASSERTV(ILINE, JLINE, X, Y, EXP == (X   > dJv));
                        ASSERTV(ILINE, JLINE, X, Y, EXP == (dIv > Y));
                    }

                    if (EXP) {
                        ASSERTV(ILINE, JLINE, Y, X, !(Y > X));
                    }

                }

                // Verify 'operator>='.

                {
                    const bool EXP = dIv >= dJv;

                    ASSERTV(ILINE, JLINE, X, Y, EXP == (X   >= Y));

                    if (ISECONDS != LLONG_MIN && ISECONDS != LLONG_MAX &&
                        JSECONDS != LLONG_MIN && JSECONDS != LLONG_MAX &&
                        ti != tj) {
                        // Tests near edge values are disabled for comparisons
                        // to doubles, as the conversions double require
                        // rounding, which breaks the test.

                        ASSERTV(ILINE, JLINE, X, Y, EXP == (X   >= dJv));
                        ASSERTV(ILINE, JLINE, X, Y, EXP == (dIv >= Y));
                    }

                    ASSERTV(ILINE, JLINE, X, Y,
                                 EXP == ((X > Y) ^ (X == Y)));

                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // COMPARISONS WITH DOUBLE (==, !=)
        //   Ensure that '==' and '!=' correctly compare values with 'double'.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y' (where one "object" is a 'double'
        //:   value), compare equal if and only if their corresponding
        //:   canonical representations compare equal.
        //:
        //: 2 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 3 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator<', 'operator<=',
        //:   'operator>', and 'operator>=' to initialize function pointers
        //:   having the appropriate signatures and return types for the 12
        //:   relational-comparison operators defined in this component.
        //:   (C-2)
        //:
        //: 2 Using a table-driven technique, create objects 'X' and 'Y' for a
        //:   cross-product of test values using the explicit conversion from
        //:   double (previously tested).  Verify the comparisons with double
        //:   return true if, and only-if, the original test value is the same
        //:   (for all the comparison overloads).  (C-1..3)
        //:
        // Testing:
        //   bool operator==(double, const TimeInterval&);
        //   bool operator==(const TimeInterval&, double);
        //   bool operator!=(double, const TimeInterval&);
        //   bool operator!=(const TimeInterval&, double);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOMPARISONS WITH DOUBLE (==, !=)"
                            "\n================================\n");


        if (verbose) printf(
                "\nAssign the address of each operator to a variable.\n");
        {
            {
                typedef bool (*operatorPtr)(double, const Obj&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorEq = bsls::operator==;
                operatorPtr operatorNe = bsls::operator!=;

                (void)operatorEq;  // quash potential compiler warnings
                (void)operatorNe;
            }
            {
                typedef bool (*operatorPtr)(const Obj&, double);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorEq = bsls::operator==;
                operatorPtr operatorNe = bsls::operator!=;

                (void)operatorEq;  // quash potential compiler warnings
                (void)operatorNe;
            }
        }

        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nCompare every value with every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                ILINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 ISECONDS = DATA[ti].d_seconds;
            const int                INSECS   = DATA[ti].d_nsecs;

            if (veryVerbose) { T_ P_(ILINE) P_(ISECONDS) P(INSECS) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int                JLINE    = DATA[tj].d_lineNum;
                const bsls::Types::Int64 JSECONDS = DATA[tj].d_seconds;
                const int                JNSECS   = DATA[tj].d_nsecs;

                double dIv = static_cast<double>(ISECONDS) +
                             static_cast<double>(INSECS) /
                             static_cast<double>(k_NANOSECS_PER_SEC);
                double dJv = static_cast<double>(JSECONDS) +
                             static_cast<double>(JNSECS) /
                             static_cast<double>(k_NANOSECS_PER_SEC);

                if (veryVerbose) { T_ P_(JLINE) P_(JSECONDS) P(JNSECS) }

                Obj mX(dIv);  const Obj& X = mX;
                Obj mY(dJv);  const Obj& Y = mY;

                if (veryVerbose) { T_ T_ P_(X) P(Y) }

                // Verify 'operator=='.

                bool EXP = ti == tj;

                ASSERTV(ILINE, JLINE, X, Y, EXP == (X   == Y));
                ASSERTV(ILINE, JLINE, X, Y, EXP == (X   == dJv));
                ASSERTV(ILINE, JLINE, X, Y, EXP == (dIv == Y));

                ASSERTV(ILINE, JLINE, X, Y, EXP != (X   != Y));
                ASSERTV(ILINE, JLINE, X, Y, EXP != (X   != dJv));
                ASSERTV(ILINE, JLINE, X, Y, EXP != (dIv != Y));
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING CONVERT AND ASSIGN FROM 'double'
        //   Ensure that we can put an object into any valid initial state.
        //
        // Concerns:
        //: 1 The 'double' conversion can create an object having any value
        //:   that does not violate the constructor's documented preconditions.
        //:
        //: 2 The 'double' assignment can set an object to any value
        //:   that does not violate the assignment operator's documented
        //:   preconditions.
        //:
        //: 3 The 'double' assignment returns a reference providing modifiable
        //:   access to the assigned object.
        //
        // Plan:
        //: 1 Using the table-driven technique, for a set of test values,
        //:   construct a test object from a 'double' and verify its
        //:   properties. (C-1)
        //:
        //: 2 Using the table-driven technique, for a set of test values,
        //:   assign a test object using a 'double' and verify its
        //:   properties. (C-2..3)
        //
        // Testing:
        //   TimeInterval(double);
        //   TimeInterval& operator=(double);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONVERT AND ASSIGN FROM 'double'"
                            "\n========================================\n");

        static const struct {
            int    d_lineNum;
            double d_secs;
            int    e_secs;
            int    e_nsecs;
        } DATA[] = {
            //line     double      secs     nanosecs
            //----     ------      ----     --------
            { L_,            0.0,   0,            0 },
            { L_,            1.0,   1,            0 },
            { L_,   1.9999999994,   1,    999999999 },
            { L_,  -1.9999999994,  -1,   -999999999 },
            { L_,   2.9999999995,   3,            0 },
            { L_,  -2.9999999995,  -3,            0 },
            { L_,   3.0000000004,   3,            0 },
            { L_,  -3.0000000004,  -3,            0 },
            { L_,   4.0000000005,   4,            1 },
            { L_,  -4.0000000005,  -4,           -1 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nTest conversion from 'double'.\n");

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE  = DATA[i].d_lineNum;
            const double DSECS = DATA[i].d_secs;
            const int    SECS  = DATA[i].e_secs;
            const int    NSECS = DATA[i].e_nsecs;

            // Test conversion from 'double'.
            {
                Obj mX(DSECS);  const Obj& X = mX;
                if (veryVerbose) {
                    T_; P_(X); P_(SECS); P(NSECS);
                }
                ASSERTV(LINE, SECS  == X.seconds());
                ASSERTV(LINE, NSECS == X.nanoseconds());
            }
        }

        if (verbose) printf("\nTest assignment from 'double'.\n");

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE  = DATA[i].d_lineNum;
            const double DSECS = DATA[i].d_secs;
            const int    SECS  = DATA[i].e_secs;
            const int    NSECS = DATA[i].e_nsecs;

            // Test assignment  from 'double'.
            {
                Obj mX; const Obj& X = mX;

                Obj *mR = &(mX = DSECS);

                if (veryVerbose) {
                    T_; P_(X); P_(SECS); P(NSECS);
                }

                ASSERTV(LINE, &X    == mR);
                ASSERTV(LINE, SECS  == X.seconds());
                ASSERTV(LINE, NSECS == X.nanoseconds());
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING: 'TimeInterval(Int64, int)'
        //   Ensure that we can put an object into any valid initial state.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the constructor's documented preconditions.
        //:
        //: 2 The value constructor accepts time intervals that are specified
        //:   using a mix of positive, negative, and zero values for seconds
        //:   and nanoseconds fields.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of distinct object values (one per row) in terms
        //:     of their 'second' and 'nanosecond' values.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..2)
        //:
        //:   1 Use the value constructor to create an object, 'X', from the
        //:     value in 'R'.
        //:
        //:   2 Construct a expected value 'EXPECTED' using the primary
        //:     manipulators.
        //:
        //:   3 Verify 'X' equals 'EXPECTED', and verify the attributes of 'X'
        //:     using its primary manipulators.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   TimeInterval(Int64, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: 'TimeInterval(Int64, int)'"
                            "\n===================================\n");

        static const struct {
            int                d_lineNum;  // Source line number
            bsls::Types::Int64 d_secs;     // Input seconds
            int                d_nsecs;    // Input nanoseconds
            bsls::Types::Int64 d_expSecs;  // Expected seconds
            int                d_expNsecs; // Expected nanoseconds
        } DATA[] = {
     //line         secs      nanosecs      expected secs   expected nanosecs
     //----         ----      --------      -------------   -----------------
      { L_,            0,             0,                0,                0 },
      { L_,            1,             2,                1,                2 },
      { L_,            1,    1000000000,                2,                0 },
      { L_,           -1,   -1000000000,               -2,                0 },
      { L_,            2,   -1000000001,                0,        999999999 },
      { L_,           -2,    1000000001,                0,       -999999999 },
      { L_,   k_SECS_MAX,     999999999,       k_SECS_MAX,        999999999 },
      { L_,   k_SECS_MIN,    -999999999,       k_SECS_MIN,       -999999999 },
            };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nUse a table of distinct object values.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE   = DATA[ti].d_lineNum;
            const bsls::Types::Int64 SECS   = DATA[ti].d_secs;
            const int                NSECS  = DATA[ti].d_nsecs;
            const bsls::Types::Int64 ESECS  = DATA[ti].d_expSecs;
            const int                ENSECS = DATA[ti].d_expNsecs;

            Obj mX(SECS, NSECS); const Obj& X = mX;

            if (veryVerbose) { T_ P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE, Obj(), X, Obj() == X);
                firstFlag = false;
            }

            Obj expected; const Obj& EXPECTED = expected;
            expected.setInterval(SECS, NSECS);

            ASSERT(EXPECTED == X);
            ASSERT(ESECS    == X.seconds());
            ASSERT(ENSECS   == X.nanoseconds());
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) printf("\tMaximum interval\n");
            {
                ASSERT_SAFE_PASS(Obj(k_SECS_MAX, k_NANOSECS_PER_SEC - 1));
                ASSERT_FAIL(Obj(k_SECS_MAX, k_NANOSECS_PER_SEC));
            }

            if (veryVerbose) printf("\tMinimum interval\n");
            {
                ASSERT_SAFE_PASS(Obj(k_SECS_MIN, -k_NANOSECS_PER_SEC + 1));
                ASSERT_FAIL(Obj(k_SECS_MIN, -k_NANOSECS_PER_SEC));
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING: 'setInterval(Int64, int)'
        //
        // Concerns:
        //: 1 The 'setInterval' method can set an object to have any
        //:   value that does not violate the method's documented
        //:   preconditions.
        //:
        //: 2 'setInterval' is not affected by the state of the object
        //:   on entry.
        //:
        //: 3 The 'nanoseconds' parameter of 'setInterval' defaults to 0.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row).
        //:
        //: 2 For each row 'R1' in the table of P-3:  (C-1..3)
        //:
        //:   1 Create an object 'X' using the default constructor.
        //:
        //:   2 Using the 'setInterval' manipulator, set 'X' to the value
        //:     specified in 'R1'.
        //:
        //:   3 Verify, using 'seconds' and 'nanoseconds', that 'X' has the
        //:     expected value.  (C-1..3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   void setInterval(Int64, int);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING: 'setInterval(Int64, int)'"
                            "\n==================================\n");

        if (verbose) printf("\nTesting 'setInterval'.\n");
        {
            static const struct {
                int                d_lineNum;  // Source line number
                bsls::Types::Int64 d_secs;     // Input seconds
                int                d_nsecs;    // Input nanoseconds
                bsls::Types::Int64 d_expSecs;  // Expected seconds
                int                d_expNsecs; // Expected nanoseconds
            } DATA[] = {
     //line         secs      nanosecs      expected secs   expected nanosecs
     //----         ----      --------      -------------   -----------------
      { L_,            0,             0,                0,                0 },
      { L_,            1,             2,                1,                2 },
      { L_,            1,    1000000000,                2,                0 },
      { L_,           -1,   -1000000000,               -2,                0 },
      { L_,            2,   -1000000001,                0,        999999999 },
      { L_,           -2,    1000000001,                0,       -999999999 },
      { L_,   k_SECS_MAX,     999999999,       k_SECS_MAX,        999999999 },
      { L_,   k_SECS_MIN,    -999999999,       k_SECS_MIN,       -999999999 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE   = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS   = DATA[i].d_secs;
                const int                NSECS  = DATA[i].d_nsecs;
                const bsls::Types::Int64 ESECS  = DATA[i].d_expSecs;
                const int                ENSECS = DATA[i].d_expNsecs;

                Obj mX;  const Obj& X = mX;
                mX.setInterval(SECS, NSECS);
                if (veryVerbose) {
                    T_; P_(X); P_(SECS); P(NSECS);
                }
                ASSERTV(LINE, ESECS  == X.seconds());
                ASSERTV(LINE, ENSECS == X.nanoseconds());
            }
        }

        if (verbose) printf("\nTesting default 'nanoseconds' value.\n");
        {
            static const struct {
                int                d_lineNum;  // Source line number
                bsls::Types::Int64 d_secs;     // Input seconds
            } DATA[] = {
                //line         secs
                //----         ----
                { L_,            0 },
                { L_,            1 },
                { L_,           -1 },
                { L_,         1000 },
                { L_,        -1000 },
                { L_,   k_SECS_MIN },
                { L_,   k_SECS_MAX },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE   = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS   = DATA[i].d_secs;

                Obj mX;  const Obj& X = mX;
                mX.setInterval(SECS);
                if (veryVerbose) {
                    T_; P_(X); P_(SECS);
                }
                ASSERTV(LINE, SECS == X.seconds());
                ASSERTV(LINE, 0    == X.nanoseconds());
            }
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            const Int64 SMAX = k_SECS_MAX;
            const Int64 SMIN = k_SECS_MIN;

            ASSERT( Obj::isValid(SMAX, k_NANOSECS_PER_SEC - 1));
            ASSERT(!Obj::isValid(SMAX, k_NANOSECS_PER_SEC));
            ASSERT(!Obj::isValid(SMAX, k_NANOSECS_PER_SEC + 1));

            ASSERT( Obj::isValid(SMIN, -k_NANOSECS_PER_SEC + 1));
            ASSERT(!Obj::isValid(SMIN, -k_NANOSECS_PER_SEC));
            ASSERT(!Obj::isValid(SMIN, -k_NANOSECS_PER_SEC - 1));

            if (veryVerbose) printf("\tMaximum interval\n");
            {
                Obj mX;
                ASSERT_FAIL(mX.setInterval(SMAX, k_NANOSECS_PER_SEC));
                ASSERT_SAFE_PASS(mX.setInterval(SMAX, k_NANOSECS_PER_SEC-1));
            }

            if (veryVerbose) printf("\tMinimum interval\n");
            {
                Obj mX;
                ASSERT_FAIL(mX.setInterval(SMIN, -k_NANOSECS_PER_SEC));
                ASSERT_SAFE_PASS(mX.setInterval(SMIN, -k_NANOSECS_PER_SEC+1));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //   Verify the BDEX streaming implementation works correctly.
        //   Specific concerns include wire format, handling of stream states
        //   (valid, empty, invalid, incomplete, and corrupted), and exception
        //   neutrality.
        //
        // Concerns:
        //: 1 The class method 'maxSupportedBdexVersion' returns the correct
        //:   version to be used for the specified 'versionSelector'.
        //:
        //: 2 The 'bdexStreamOut' method is callable on a reference providing
        //:   only non-modifiable access.
        //:
        //: 3 For valid streams, externalization and unexternalization are
        //:   inverse operations.
        //:
        //: 4 For invalid streams, externalization leaves the stream invalid
        //:   and unexternalization does not alter the value of the object and
        //:   leaves the stream invalid.
        //:
        //: 5 Unexternalizing of incomplete, invalid, or corrupted data results
        //:   in a valid object of unspecified value and an invalidated stream.
        //:
        //: 6 The wire format of the object is as expected.
        //:
        //: 7 All methods are exception neutral.
        //:
        //: 8 The 'bdexStreamIn' and 'bdexStreamOut' methods return a reference
        //:   to the provided stream in all situations.
        //:
        //: 9 The initial value of the object has no affect on
        //:   unexternalization.
        //
        // Plan:
        //: 1 Test 'maxSupportedBdexVersion' explicitly.  (C-1)
        //:
        //: 2 All calls to the 'bdexStreamOut' accessor will be done from a
        //:   'const' object or reference.  (C-2)
        //:
        //: 3 Perform a direct test of the 'bdexStreamOut' and 'bdexStreamIn'
        //:   methods.
        //:
        //: 4 Define a set 'S' of test values to be used throughout the test
        //:   case.
        //:
        //: 5 For all '(u, v)' in the cross product 'S X S', stream the value
        //:   of 'u' into (a temporary copy of) 'v', 'T', and assert 'T == u'.
        //:   (C-3, 9)
        //:
        //: 6 For all 'u' in 'S', create a copy of 'u' and attempt to stream
        //:   into it from an invalid stream.  Verify after each attempt that
        //:   the object is unchanged and that the stream is invalid.  (C-4)
        //:
        //: 7 Write 3 distinct objects to an output stream buffer of total
        //:   length 'N'.  For each partial stream length from 0 to 'N - 1',
        //:   construct an input stream and attempt to read into objects
        //:   initialized with distinct values.  Verify values of objects
        //:   that are either successfully modified or left entirely
        //:   unmodified, and that the stream became invalid immediately after
        //:   the first incomplete read.  Finally, ensure that each object
        //:   streamed into is in some valid state.
        //:
        //: 8 Use the underlying stream package to simulate a typical valid
        //:   (control) stream and verify that it can be streamed in
        //:   successfully.  Then for each data field in the stream (beginning
        //:   with the version number), provide one or more similar tests with
        //:   that data field corrupted.  After each test, verify that the
        //:   object is in some valid state after streaming, and that the
        //:   input stream has become invalid.  (C-5)
        //:
        //: 9 Explicitly test the wire format.  (C-6)
        //:
        //:10 In all cases, confirm exception neutrality using the specially
        //:   instrumented 'bslx::TestInStream' and a pair of standard macros,
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END', which configure the
        //:   'bslx::TestInStream' object appropriately in a loop.  (C-7)
        //:
        //:11 In all cases, verify the return value of the tested method.
        //:   (C-8)
        //
        // Testing:
        //   static int maxSupportedBdexVersion(int versionSelector);
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation
        //   static int maxSupportedBdexVersion();
#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
        //   static int maxSupportedVersion();
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDEX STREAMING"
                            "\n======================\n");

        // Scalar object values used in various stream tests.
        const Obj VA(  0,  0);
        const Obj VB(  0,  1);
        const Obj VC(  1,  0);
        const Obj VD( 10, 59);
        const Obj VE( 23,  0);
        const Obj VF( 23, 22);
        const Obj VG( 24,  0);

        // Array object used in various stream tests.
        const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG };
        const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                / sizeof *VALUES);

        if (verbose) {
            printf("\nTesting 'maxSupportedBdexVersion'.\n");
        }
        {
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(1 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
        }

        const int VERSION = Obj::maxSupportedBdexVersion(0);

        if (verbose) {
            printf("\nDirect initial trial of 'bdexStreamOut' and (valid) "
                   "'bdexStreamIn' functionality.\n");
        }
        {
            const Obj X(VC);
            Out       out(VERSION_SELECTOR);

            Out& rvOut = X.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut);
            ASSERT(out);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            In in(OD, LOD);
            ASSERT(in);
            ASSERT(!in.isEmpty());

            Obj mT(VA);  const Obj& T = mT;
            ASSERT(X != T);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(X == T);
            ASSERT(in);
            ASSERT(in.isEmpty());
        }

        if (verbose) {
            printf("\nThorough test.\n");
        }
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);

                Out out(VERSION_SELECTOR);

                Out& rvOut = X.bdexStreamOut(out, VERSION);
                LOOP_ASSERT(i, &out == &rvOut);
                LOOP_ASSERT(i, out);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj mT(VALUES[j]);  const Obj& T = mT;
                    LOOP2_ASSERT(i, j, (X == T) == (i == j));

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();
                        In& rvIn = mT.bdexStreamIn(in, VERSION);
                        LOOP2_ASSERT(i, j, &in == &rvIn);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END

                    LOOP2_ASSERT(i, j, X == T);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) {
            printf("\tOn empty streams and non-empty, invalid streams.\n");
        }

        // Verify correct behavior for empty streams (valid and invalid).

        {
            Out               out(VERSION_SELECTOR);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();

                    // Stream is valid.
                    In& rvIn1 = mT.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn1);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);

                    // Stream is invalid.
                    In& rvIn2 = mT.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn2);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        // Verify correct behavior for non-empty, invalid streams.

        {
            Out               out(VERSION_SELECTOR);

            Out& rvOut = Obj().bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut);
            ASSERT(out);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 < LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                in.invalidate();
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, !in.isEmpty());

                // Ensure that reading from a non-empty, invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj  X(VALUES[i]);
                Obj        mT(X);
                const Obj& T = mT;
                LOOP_ASSERT(i, X == T);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    in.invalidate();
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    In& rvIn = mT.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(i, &in == &rvIn);
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, X == T);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            printf("\tOn incomplete (but otherwise valid) data.\n");
        }
        {
            const Obj W1 = VA, X1 = VB;
            const Obj W2 = VB, X2 = VC;
            const Obj W3 = VC, X3 = VD;

            Out out(VERSION_SELECTOR);

            Out& rvOut1 = X1.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut1);
            ASSERT(out);
            const int         LOD1 = out.length();

            Out& rvOut2 = X2.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut2);
            ASSERT(out);
            const int         LOD2 = out.length();

            Out& rvOut3 = X3.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut3);
            ASSERT(out);
            const int         LOD3 = out.length();
            const char *const OD3  = out.data();

            for (int i = 0; i < LOD3; ++i) {
                In in(OD3, i);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, !i == in.isEmpty());

                    Obj mT1(W1);  const Obj& T1 = mT1;
                    Obj mT2(W2);  const Obj& T2 = mT2;
                    Obj mT3(W3);  const Obj& T3 = mT3;

                    if (i < LOD1) {
                        In& rvIn1 = mT1.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i, !in);
                        if (0 == i) LOOP_ASSERT(i, W1 == T1);
                        In& rvIn2 = mT2.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W2 == T2);
                        In& rvIn3 = mT3.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W3 == T3);
                    }
                    else if (i < LOD2) {
                        In& rvIn1 = mT1.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X1 == T1);
                        In& rvIn2 = mT2.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i, !in);
                        if (LOD1 <= i) LOOP_ASSERT(i, W2 == T2);
                        In& rvIn3 = mT3.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, W3 == T3);
                    }
                    else {  // 'LOD2 <= i < LOD3'
                        In& rvIn1 = mT1.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X1 == T1);
                        In& rvIn2 = mT2.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X2 == T2);
                        In& rvIn3 = mT3.bdexStreamIn(in, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        if (LOD2 <= i) LOOP_ASSERT(i, W3 == T3);
                    }

                    // Verify the objects are in a valid state.

                    LOOP_ASSERT(i, Obj::isValid(T1.seconds(),
                                                T1.nanoseconds()));

                    LOOP_ASSERT(i, Obj::isValid(T2.seconds(),
                                                T2.nanoseconds()));

                    LOOP_ASSERT(i, Obj::isValid(T3.seconds(),
                                                T3.nanoseconds()));

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            printf("\tOn corrupted data.\n");
        }

        const Obj W;             // default value
        const Obj X(1, 3);       // original (control)
        const Obj Y(0, 2);       // new (streamed-out)

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        if (verbose) {
            printf("\t\tGood stream (for control).\n");
        }
        {
            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(0);
            out.putInt32(2);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(in);
            ASSERT(Y == T);
        }

        if (verbose) {
            printf("\t\tBad version.\n");
        }
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(0);
            out.putInt32(2);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, version);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            const char version = 2 ; // too large (current version is 1)

            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(0);
            out.putInt32(2);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, version);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            printf("\t\tSeconds positive, nanoseconds negative.\n");
        }
        {
            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64( 1);
            out.putInt32(-1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            printf("\t\tSeconds negative, nanoseconds positive.\n");
        }
        {
            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(-1);
            out.putInt32( 1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            printf("\t\tNanoseconds too small.\n");
        }
        {
            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(0);
            out.putInt32(-k_NANOSECS_PER_SEC);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            printf("\t\tNanoseconds too large.\n");
        }
        {
            Out out(VERSION_SELECTOR);

            // Stream out "new" value.
            out.putInt64(0);
            out.putInt32(k_NANOSECS_PER_SEC);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            In& rvIn = mT.bdexStreamIn(in, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            printf("\nWire format direct tests.\n");
        }
        {
            static const struct {
                int         d_lineNum;      // source line number
                int         d_seconds;      // specification seconds
                int         d_nanoseconds;  // specification nanoseconds
                int         d_version;      // version to stream with
                int         d_length;       // expect output length
                const char *d_fmt_p;        // expected output format
            } DATA[] = {
                //LINE  SEC   NS   VER  LEN  FORMAT
                //----  ----  ---  ---  ---  -------------------
                { L_,      0,   0,   1,  12,
                         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"  },
                { L_,      0,   1,   1,  12,
                         "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"  },
                { L_,      1,   0,   1,  12,
                         "\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00"  },
                { L_,     20,   8,   1,  12,
                         "\x00\x00\x00\x00\x00\x00\x00\x14\x00\x00\x00\x08"  }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_lineNum;
                const int         SECONDS     = DATA[i].d_seconds;
                const int         NANOSECONDS = DATA[i].d_nanoseconds;
                const int         VERSION     = DATA[i].d_version;
                const int         LEN         = DATA[i].d_length;
                const char *const FMT         = DATA[i].d_fmt_p;

                {
                    Obj        mX(SECONDS, NANOSECONDS);
                    const Obj& X = mX;

                    Out  out(VERSION_SELECTOR);
                    Out& rvOut = X.bdexStreamOut(out, VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);
                    LOOP_ASSERT(LINE, out);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < out.length(); ++j) {
                            printf("\\x%c%c",
                                   hex[static_cast<unsigned char>
                                           ((*(out.data() + j) >> 4) & 0x0f)],
                                   hex[static_cast<unsigned char>
                                                  (*(out.data() + j) & 0x0f)]);
                        }
                        printf("\n");
                    }

                    Obj mY;  const Obj& Y = mY;

                    In  in(out.data(), out.length());
                    In& rvIn = mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, &in == &rvIn);
                    LOOP_ASSERT(LINE, X == Y);
                }
            }
        }

#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation

        if (verbose) {
            printf("\nTesting deprecated methods.\n");
        }
        {
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
            ASSERT(Obj::maxSupportedVersion()
                                           == Obj::maxSupportedBdexVersion(0));
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
            ASSERT(Obj::maxSupportedBdexVersion()
                                           == Obj::maxSupportedBdexVersion(0));
        }

#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their canonical
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1, 3..4)
        //:
        //:   1 Use the default constructor and 'setIntervalRaw' to
        //:     create two 'const' 'Obj', 'Z' and 'ZZ', having the value from
        //:     'R1'.
        //:
        //:   2 For each row 'R2' in the table of P-2:  (C-1, 3..4)
        //:
        //:     1 Use the default constructor and 'setIntervalRaw' to
        //:       create a modifiable 'Obj', 'mX', having the value from 'R2'.
        //:
        //:     2 Assign 'mX' from 'Z'.
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-3 except that, this
        //:   time, there is no inner loop (as in P-3.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are created to have the value from 'R1'.  For each
        //:   'R1' in the table of P-2:  (C-5)
        //:
        //:   1 Use the default constructor and 'setIntervalRaw' to
        //:     create a modifiable 'Obj', 'mX', having the value from 'R1';
        //:     also use the default constructor and 'setIntervalRaw' to
        //:     create a 'const' 'Obj', 'ZZ', also having the value from 'R1'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'Z' ('mX'), still has the same value as that of
        //:     'ZZ'.  (C-5)
        //
        // Testing:
        //   TimeInterval& operator=(const TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose) printf(
                 "\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
            { L_,        LLONG_MAX,   999999999 },
            { L_,        LLONG_MIN,  -999999999 },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                ILINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 ISECONDS = DATA[ti].d_seconds;
            const int                INSECS   = DATA[ti].d_nsecs;

            Obj mZ;   const Obj& Z  = mZ;
            mZ. setIntervalRaw(ISECONDS, INSECS);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setIntervalRaw(ISECONDS, INSECS);

            if (veryVerbose) { T_ P_(ILINE) P(Z) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(ILINE, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int                JLINE    = DATA[tj].d_lineNum;
                const bsls::Types::Int64 JSECONDS = DATA[tj].d_seconds;
                const int                JNSECS   = DATA[tj].d_nsecs;

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(JSECONDS, JNSECS);

                if (veryVerbose) { T_ T_ P_(JLINE) P(X) }

                ASSERTV(ILINE, JLINE, Z, X,
                             (Z == X) == (ILINE == JLINE));

                Obj *mR = &(mX = Z);

                ASSERTV(ILINE, JLINE,  Z,   X,  Z == X);
                ASSERTV(ILINE, JLINE, mR, &mX, mR == &mX);
                ASSERTV(ILINE, JLINE, ZZ,   Z, ZZ == Z);
            }

            // self-assignment

            {
                Obj mX;
                mX. setIntervalRaw(ISECONDS, INSECS);

                Obj mZZ;  const Obj& ZZ = mZZ;
                mZZ.setIntervalRaw(ISECONDS, INSECS);

                const Obj& Z = mX;

                ASSERTV(ILINE, ZZ,   Z, ZZ == Z);

                Obj *mR = &(mX = Z);

                ASSERTV(ILINE, mR, &mX, mR == &mX);
                ASSERTV(ILINE, ZZ,   Z, ZZ == Z);
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for 'swap' testing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                            "\n==============================\n");

        if (verbose) printf("Not implemented for 'bsls::TimeInterval'.\n");

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their seconds and
        //:   nanoseconds.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Use the default constructor to create two objects, 'Z' and
        //:     'ZZ', then set both 'Z' and 'ZZ' to the value from 'R' (using
        //:     'setTotalMilliseconds').
        //:
        //:   2 Use the copy constructor to create an object 'X', supplying it
        //:     with a reference providing non-modifiable access to 'Z'.  (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:  (C-1, 3)
        //:
        //:     1 'X' has the same value as that of 'Z'.  (C-1)
        //:
        //:     2 'Z' still has the same value as that of 'ZZ'.  (C-3)
        //
        // Testing:
        //   TimeInterval(const TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTOR"
                            "\n================\n");

        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
            { L_,        LLONG_MAX,   999999999 },
            { L_,        LLONG_MIN,  -999999999 },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                LINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 SECONDS = DATA[ti].d_seconds;
            const int                NSECS   = DATA[ti].d_nsecs;

            Obj mZ;   const Obj& Z  = mZ;
            mZ. setIntervalRaw(SECONDS, NSECS);

            Obj mZZ;  const Obj& ZZ = mZZ;
            mZZ.setIntervalRaw(SECONDS, NSECS);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            const Obj X(Z);

            if (veryVerbose) { T_ T_ P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE, Obj(), X, Obj() == X);
                firstFlag = false;
            }

            // Verify the value of the object.

            ASSERTV(LINE, Z,  X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            ASSERTV(LINE, ZZ, Z, ZZ == Z);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if their
        //:   corresponding canonical representations compare equal.
        //:
        //: 2 'true  == (X == X)' (i.e., identity).
        //:
        //: 3 'false == (X != X)' (i.e., identity).
        //:
        //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity).
        //:
        //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity).
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'.
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 9 The equality-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-7..9)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their seconds and
        //:   nanoseconds.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Create an object 'W' using the default constructor, then set
        //:     'W' to the value from 'R1' (using 'setTimeInterval').
        //:
        //:   2 Using 'W', verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-2..3)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 4..6)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects set to
        //:       values from 'R1' and 'R2', respectively, are expected to have
        //:       the same value.
        //:
        //:     2 Create an object 'X' using the default constructor, then set
        //:       'X' to the value from 'R1' (using 'setTimeInterval').
        //:
        //:     3 Create an object 'Y' using the default constructor, then set
        //:       'Y' to the value from 'R2'.
        //:
        //:     4 Using 'X' and 'Y', verify the commutativity property and
        //:       expected return value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(const TimeInterval&, const TimeInterval&);
        //   bool operator!=(const TimeInterval&, const TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                            "\n=============================\n");

        if (verbose) printf(
                "\nAssign the address of each operator to a variable.\n");
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bsls::operator==;
            operatorPtr operatorNe = bsls::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        static const struct {
            int                d_lineNum;
            bsls::Types::Int64 d_seconds;
            int                d_nsecs;
        } DATA[] = {
            //line         seconds       nanosecs
            //----        --------       --------
            { L_,                0,           0 },
            { L_,                1,           1 },
            { L_,                1,   999999999 },
            { L_,               -1,          -1 },
            { L_,               -1,  -999999999 },
            { L_,     3000000000LL,   999999999 },
            { L_,    -3000000000LL,  -999999999 },
            { L_,        LLONG_MAX,   999999999 },
            { L_,        LLONG_MIN,  -999999999 },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nCompare every value with every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                ILINE    = DATA[ti].d_lineNum;
            const bsls::Types::Int64 ISECONDS = DATA[ti].d_seconds;
            const int                INSECS   = DATA[ti].d_nsecs;

            if (veryVerbose) { T_; P_(ILINE); P_(ISECONDS); P(INSECS); }

            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj& W = mW;
                mW.setIntervalRaw(ISECONDS, INSECS);

                ASSERTV(ILINE, W,   W == W);
                ASSERTV(ILINE, W, !(W != W));

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    ASSERTV(ILINE, Obj(), W, Obj() == W);
                    firstFlag = false;
                }
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int                JLINE    = DATA[tj].d_lineNum;
                const bsls::Types::Int64 JSECONDS = DATA[tj].d_seconds;
                const int                JNSECS   = DATA[tj].d_nsecs;

                if (veryVerbose) { T_; P_(JLINE); P_(JSECONDS); P(JNSECS); }

                const bool EXP = ti == tj;  // expected for equality comparison

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(ISECONDS, INSECS);

                Obj mY;  const Obj& Y = mY;
                mY.setIntervalRaw(JSECONDS, JNSECS);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify expected against canonical comparison.

                ASSERTV(ILINE, JLINE, X, Y,
                             EXP == ((X.seconds()     == Y.seconds() &&
                                     (X.nanoseconds() == Y.nanoseconds()))));

               ASSERTV(ILINE, JLINE, X, Y,
                            !EXP == ((X.seconds()     != Y.seconds() ||
                                     (X.nanoseconds() != Y.nanoseconds()))));

                // Verify value and commutativity.

                ASSERTV(ILINE, JLINE, X, Y,  EXP == (X == Y));
                ASSERTV(ILINE, JLINE, Y, X,  EXP == (Y == X));

                ASSERTV(ILINE, JLINE, X, Y, !EXP == (X != Y));
                ASSERTV(ILINE, JLINE, Y, X, !EXP == (Y != X));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR (<<)
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method's signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values (0 and 4, respectively).
        //:
        //: 7 The output 'operator<<'s signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the destination 'ostream'.
        //:
        //: 9 The output 'operator<<' respects the output width set by the
        //:   'std::setw' manipulator on the specified 'ostream'.
        //:   {DRQS 166236963}
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding time interval field, and various values for the
        //:     two formatting parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel'):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, '(-9, -9)', then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //:
        //: 3 Using the table-driven technique, further corroborate that the
        //:   'print' method and 'operator<<' format object values correctly by
        //:   testing an additional set of time intervals (including extremal
        //:   values), but this time fixing the 'level' and 'spacesPerLevel'
        //:   arguments to 0 and -1, respectively.
        //:
        //: 4 Using the table-driven technique, set the different field width
        //:   used on oputput operations and ensure that the object value is
        //:   formatted correctly.
        //
        // Testing:
        //   ostream& print(ostream&, int, int) const;
        //   ostream& operator<<(ostream&, const TimeInterval&);
        // --------------------------------------------------------------------


        if (verbose) printf("\nPRINT AND OUTPUT OPERATOR (<<)"
                            "\n==============================\n");

        if (verbose) printf("\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables.\n");
        {

            typedef std::ostream& (Obj::*funcPtr)(
                                                std::ostream&, int, int) const;
            typedef std::ostream& (*operatorPtr)(std::ostream&, const Obj&);


            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOut = bsls::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOut;
        }

        if (verbose) printf(
             "\nCreate a table of distinct value/format combinations.\n");

        static const struct {
            int         d_line;
            int         d_level;
            int         d_spacesPerLevel;
            Int64       d_secs;
            int         d_nsecs;
            const char *d_expected;
        } DATA[] = {

#define NL "\n"

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL       S       Ns   EXP
        //---- - ---       -       --   ---

        { L_,  0,  0,      0,      0,  "(0, 0)"              NL },
        { L_,  0,  1,      0,      0,  "(0, 0)"              NL },
        { L_,  0, -1,      0,      0,  "(0, 0)"                 },
        { L_,  0,  4,      0,      0,  "(0, 0)"              NL },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL       S       Ns   EXP
        //---- - ---       -       --   ---
        { L_,  3,  0,      0,      0,  "(0, 0)"              NL },
        { L_,  3,  2,      0,      0,  "      (0, 0)"        NL },
        { L_,  3, -2,      0,      0,  "      (0, 0)"           },
        { L_,  3,  4,      0,      0,  "            (0, 0)"  NL },
        { L_, -3,  0,      0,      0,  "(0, 0)"              NL },
        { L_, -3,  2,      0,      0,  "(0, 0)"              NL },
        { L_, -3, -2,      0,      0,  "(0, 0)"                 },
        { L_, -3,  4,      0,      0,  "(0, 0)"              NL },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL       S       Ns   EXP
        //---- - ---       -       --   ---
        { L_,  2,  3,   -123,  -5000,  "      (-123, -5000)" NL },


        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL       S       Ns   EXP
        //---- - ---       -       --   ---
        { L_,  0,  4,      0,      0,  "(0, 0)"              NL },
        { L_,  0,  4,   -123,  -5000,  "(-123, -5000)"       NL },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL       S       Ns   EXP
        //---- - ---       -       --   ---
        { L_,  0, -1,      0,      0,  "(0, 0)"                 },
        { L_,  0, -1,   -123,  -5000,  "(-123, -5000)"          },

#undef NL

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nTesting with various print specifications.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const int         L     = DATA[ti].d_level;
                const int         SPL   = DATA[ti].d_spacesPerLevel;
                const Int64       SECS  = DATA[ti].d_secs;
                const int         NSECS = DATA[ti].d_nsecs;
                const char *const EXP   = DATA[ti].d_expected;

                typedef std::ostringstream       OSStream;
                typedef std::ostream             OStream;

                Obj mX(SECS, NSECS); const Obj& X = mX;

                if (veryVerbose) {
                    T_; P_(LINE); P_(L); P(SPL); P_(SECS); P_(NSECS); P(EXP);
                }

                // Test with no default arguments.

                {
                    OSStream out;
                    OStream *mR = &X.print(out, L, SPL);

                    const std::string RESULT_STRING = out.str();
                    const char *RESULT = RESULT_STRING.c_str();

                    ASSERTV(LINE,  mR,     &X, mR == &out);
                    ASSERTV(LINE, EXP, RESULT, 0  == strcmp(EXP, RESULT));

                }

                // Test with default 'spacesPerLevel'.


                if (4 == SPL) {
                    OSStream out;
                    OStream *mR = &X.print(out, L);

                    const std::string RESULT_STRING = out.str();
                    const char *RESULT = RESULT_STRING.c_str();

                    ASSERTV(LINE,  mR,     &X, mR == &out);
                    ASSERTV(LINE, EXP, RESULT, 0  == strcmp(EXP, RESULT));

                }

                // Test with default 'level' and 'spacesPerLevel'.


                if (0 == L && 4 == SPL) {
                    OSStream out;
                    OStream *mR = &X.print(out);

                    const std::string RESULT_STRING = out.str();
                    const char *RESULT = RESULT_STRING.c_str();

                    ASSERTV(LINE,  mR,     &X, mR == &out);
                    ASSERTV(LINE, EXP, RESULT, 0  == strcmp(EXP, RESULT));
                }
            }
        }

        if (verbose) printf("\nTesting 'operator<<' (ostream).\n");
        {
            static const struct {
                int                 d_lineNum;  // source line number
                bsls::Types::Int64  d_seconds;  // second field value
                int                 d_nsecs;    // nanosecond field value
                const char         *d_expected; // expected output format
            } DATA[] = {
                //line     secs        nsecs        output format
                //---- ------------   ---------   ---------------------------
                { L_,             0,          0, "(0, 0)"                    },
                { L_,             0,        100, "(0, 100)"                  },
                { L_,             0,       -100, "(0, -100)"                 },
                { L_,          -123,      -5000, "(-123, -5000)"             },
                { L_,  3000000000LL,  999999999, "(3000000000, 999999999)"   },
                { L_, -3000000000LL, -999999999, "(-3000000000, -999999999)" },
                { L_,     LLONG_MAX,  999999999, "(9223372036854775807, "
                                                 "999999999)"                },
                { L_,     LLONG_MIN, -999999999, "(-9223372036854775808, "
                                                 "-999999999)"               },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int                LINE     = DATA[di].d_lineNum;
                const bsls::Types::Int64 SECONDS  = DATA[di].d_seconds;
                const int                NSECS    = DATA[di].d_nsecs;
                const char *const        EXPECTED = DATA[di].d_expected;

                typedef std::ostringstream       OSStream;
                typedef std::ostream             OStream;

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(SECONDS, NSECS);

                OSStream out;

                OStream *mR = &(out << X);

                const std::string RESULT_STRING = out.str();
                const char *RESULT = RESULT_STRING.c_str();

                if (veryVerbose) { T_; P_(EXPECTED); P(RESULT); }

                ASSERTV(LINE, mR               == &out);
                ASSERTV(LINE, strlen(EXPECTED) == strlen(RESULT));
                ASSERTV(LINE, 0                == strcmp(EXPECTED, RESULT));

                // Compare with 'print(stream, 0, -1)
                {
                    OSStream printResult;
                    const std::string PRINT_RESULT_STRING = out.str();
                    const char *PRINT_RESULT = PRINT_RESULT_STRING.c_str();

                    X.print(printResult, 0, -1);

                    ASSERTV(LINE, PRINT_RESULT, RESULT,
                                            0 == strcmp(PRINT_RESULT, RESULT));
                }
            }
        }

        if (verbose) printf("\nTesting i/o manipulators).\n");
        {   // P-4
            static const struct {
                int                 d_lineNum;  // source line number
                int                 d_width;    // output width
                bool                d_left;
                bool                d_right;
                char                d_fillChar;
                const char         *d_expected; // expected output format
            } DATA[] = {
                //line   width  left  right  fill  output format
                //----   -----  ----  -----  ----  -------------
                { L_,    -1,    0,    0,     0,    "(0, 0)"    },
                { L_,     0,    0,    0,     0,    "(0, 0)"    },
                { L_,     8,    0,    0,     0,    "  (0, 0)"  },
                { L_,     7,    0,    0,     0,    " (0, 0)"   },
                { L_,     6,    0,    0,     0,    "(0, 0)"    },
                { L_,     5,    0,    0,     0,    "(0, 0)"    },
                { L_,     8,    0,    1,   '*',    "**(0, 0)"  },
                { L_,     7,    0,    1,   '*',    "*(0, 0)"   },
                { L_,     6,    0,    1,   '*',    "(0, 0)"    },
                { L_,     5,    0,    1,   '*',    "(0, 0)"    },
                { L_,     8,    1,    0,   '*',    "(0, 0)**"  },
                { L_,     7,    1,    0,   '*',    "(0, 0)*"   },
                { L_,     6,    1,    0,   '*',    "(0, 0)"    },
                { L_,     5,    1,    0,   '*',    "(0, 0)"    },
           };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int                LINE     = DATA[di].d_lineNum;
                const int                WIDTH    = DATA[di].d_width;
                const bool               LEFT     = DATA[di].d_left;
                const bool               RIGHT    = DATA[di].d_right;
                const char               FILL     = DATA[di].d_fillChar;
                const char *const        EXPECTED = DATA[di].d_expected;

                typedef std::ostringstream OSStream;

                Obj      mX;  const Obj& X = mX;
                OSStream out;

                if (WIDTH > 0) {
                    out << std::setw(WIDTH);
                }

                if (LEFT) {
                    out << std::left;
                }

                if (RIGHT) {
                    out << std::right;
                }

                if (FILL) {
                    out << std::setfill(FILL);
                }

                out << X;

                const std::string  RESULT_STRING = out.str();
                const char        *RESULT = RESULT_STRING.c_str();

                if (veryVerbose) { T_; P_(EXPECTED); P(RESULT); }

                ASSERTV(LINE, strlen(EXPECTED) == strlen(RESULT));
                ASSERTV(LINE, 0                == strcmp(EXPECTED, RESULT));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 The 'seconds' and 'nanoseconds' accessors return the value of
        //:   the corresponding field of the time interval.
        //:
        //: 2 Each basic accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their five-field
        //:   representation.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Create an object 'X' using the default constructor.
        //:
        //:   3 Using 'setIntervalRaw', set 'X' to the value computed in
        //:     P-2.1.
        //:
        //:   4 Verify that each basic accessor, invoked on a reference
        //:     providing non-modifiable access to the object created in P-2.2,
        //:     returns the expected value.  (C-1..3)
        //
        // Testing:
        //   bsls::Types::Int64 seconds() const;
        //   int nanoseconds() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        if (verbose) printf(
                "\nVerify all basic accessors report expected values.\n");

        {
            static const struct {
                int                d_lineNum;
                bsls::Types::Int64 d_secs;
                int                d_nsecs;
            } DATA[] = {
                //line             secs      nanosecs
                //----       ----------    ---------
                { L_,                 0,           0 },
                { L_,                 0,           1 },
                { L_,                 1,           1 },
                { L_,                -1,          -1 },
                { L_,        k_SECS_MAX,   999999999 },
                { L_,        k_SECS_MIN,  -999999999 }
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const bsls::Types::Int64 SECS = DATA[i].d_secs;
                const int LINE  = DATA[i].d_lineNum;
                const int NSECS = DATA[i].d_nsecs;

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(SECS, NSECS);

                if (veryVerbose) {
                    T_; P_(i);    P(X);
                    T_; P_(SECS); P(NSECS);
                }

                ASSERTV(LINE, SECS  == X.seconds());
                ASSERTV(LINE, NSECS == X.nanoseconds());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TEST-DRIVER MACHINERY
        //   Test the test-driver machinery used in this test-driver
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   TESTING TEST-DRIVER MACHINERY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST-DRIVER MACHINERY"
                            "\n=============================\n");

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 An object can be safely destroyed.
        //:
        //: 3 The 'setIntervalRaw' method can set an object to have any
        //:   value that does not violate the method's documented
        //:   preconditions.
        //:
        //: 4 'setIntervalRaw' is not affected by the state of the object
        //:   on entry.
        //:
        //: 5 The 'nanoseconds' parameter of 'setIntervalRaw' defaults to 0.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using the default constructor.  Verify, using
        //:   the (as yet unproven) 'seconds' and 'nanoseconds' accessors,
        //:   that the resulting object has a time interval of 0.  (C-1)
        //:
        //: 2 Let the object created in P-1 go out of scope.  (C-2)
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their canonical
        //:   representation.
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-3..5)
        //:
        //:   1 Create an object 'X' using the default constructor.
        //:
        //:   2 Using the 'setIntervalRaw' (primary) manipulator, set 'X'
        //:     to the value specified in 'R1'.
        //:
        //:   3 Verify, using 'seconds' and 'nanoseconds'' that 'X' has the
        //:     expected value.  (C-3..5)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid time interval values, but not triggered
        //:   for adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   TimeInterval();
        //   ~TimeInterval();
        //   void setIntervalRaw(Int64, int);
        // --------------------------------------------------------------------
        if (verbose) printf("\nDEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR"
                            "\n==========================================\n");

        if (verbose) printf("\nTesting default constructor.\n");

        {
            Obj mX;  const Obj& X = mX;

            if (veryVerbose) P(X);

            ASSERT(0 == X.seconds());
            ASSERT(0 == X.nanoseconds());
        }

        if (verbose) printf("\nTesting 'setIntervalRaw'.\n");
        {
            static const struct {
                int                d_lineNum;  // Source line number
                bsls::Types::Int64 d_secs;     // Input seconds
                int                d_nsecs;    // Input nanoseconds
            } DATA[] = {
                //line         secs      nanosecs
                //----         ----      --------
                { L_,            0,                       0 },
                { L_,            0,  k_NANOSECS_PER_SEC - 1 },
                { L_,            0, -k_NANOSECS_PER_SEC + 1 },
                { L_,         9999,                    9999 },
                { L_,        -9999,                   -9999 },
                { L_,   k_SECS_MAX,                       0 },
                { L_,   k_SECS_MIN,                       0 },
                { L_,   k_SECS_MAX,  k_NANOSECS_PER_SEC - 1 },
                { L_,   k_SECS_MIN, -k_NANOSECS_PER_SEC + 1 },


            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE   = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS   = DATA[i].d_secs;
                const int                NSECS  = DATA[i].d_nsecs;

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(SECS, NSECS);
                if (veryVerbose) {
                    T_; P_(X); P_(SECS); P(NSECS);
                }
                ASSERTV(LINE, SECS  == X.seconds());
                ASSERTV(LINE, NSECS == X.nanoseconds());
            }
        }

        if (verbose) printf("\nTesting default 'nanoseconds' value.\n");
        {
            static const struct {
                int                d_lineNum;  // Source line number
                bsls::Types::Int64 d_secs;     // Input seconds
            } DATA[] = {
                //line         secs
                //----         ----
                { L_,            0 },
                { L_,            1 },
                { L_,           -1 },
                { L_,         1000 },
                { L_,        -1000 },
                { L_,   k_SECS_MIN },
                { L_,   k_SECS_MAX },

            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE   = DATA[i].d_lineNum;
                const bsls::Types::Int64 SECS   = DATA[i].d_secs;

                Obj mX;  const Obj& X = mX;
                mX.setIntervalRaw(SECS);
                if (veryVerbose) {
                    T_; P_(X); P_(SECS);
                }
                ASSERTV(LINE, SECS == X.seconds());
                ASSERTV(LINE, 0    == X.nanoseconds());
            }
        }


        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) printf("\tMaximum interval\n");
            {
                Obj mX;
                ASSERT_SAFE_FAIL(mX.setIntervalRaw(0, k_NANOSECS_PER_SEC));
                ASSERT_SAFE_PASS(mX.setIntervalRaw(0, k_NANOSECS_PER_SEC-1));
            }

            if (veryVerbose) printf("\tMinimum interval\n");
            {
                Obj mX;
                ASSERT_SAFE_FAIL(mX.setIntervalRaw(0, -k_NANOSECS_PER_SEC));
                ASSERT_SAFE_PASS(mX.setIntervalRaw(0, -k_NANOSECS_PER_SEC+1));
            }

            if (veryVerbose) printf("\tMatching sign values\n");
            {
                Obj mX;
                ASSERT_SAFE_PASS(mX.setIntervalRaw( 0,  0));
                ASSERT_SAFE_PASS(mX.setIntervalRaw( 1,  0));
                ASSERT_SAFE_PASS(mX.setIntervalRaw(-1,  0));
                ASSERT_SAFE_PASS(mX.setIntervalRaw( 0,  1));
                ASSERT_SAFE_PASS(mX.setIntervalRaw( 0, -1));
                ASSERT_SAFE_PASS(mX.setIntervalRaw( 1,  1));
                ASSERT_SAFE_PASS(mX.setIntervalRaw(-1, -1));

                ASSERT_SAFE_FAIL(mX.setIntervalRaw( 1, -1));
                ASSERT_SAFE_FAIL(mX.setIntervalRaw(-1,  1));
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
        //: 1 Create an object 'w' (default ctor).      { w:0             }
        //: 2 Create an object 'x' (copy from 'w').     { w:0 x:0         }
        //: 3 Set 'x' to 'A' (value distinct from 0).   { w:0 x:A         }
        //: 4 Create an object 'y' (init. to 'A').      { w:0 x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').     { w:0 x:A y:A z:A }
        //: 6 Set 'z' to 0 (the default value).         { w:0 x:A y:A z:0 }
        //: 7 Assign 'w' from 'x'.                      { w:A x:A y:A z:0 }
        //: 8 Assign 'w' from 'z'.                      { w:0 x:A y:A z:0 }
        //: 9 Assign 'x' from 'x' (aliasing).           { w:0 x:A y:A z:0 }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const int SA = 1, NA = 2;  // VA fields
        const int SB = 3, NB = 4;  // VB fields
        const int SC = 5, NC = 6;  // VC fields

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 1. Create a default object x1."
                            "\t\t\t{ x1:0 }\n");
        Obj mX1;  const Obj& X1 = mX1;
        if (verbose) { T_;  P(X1); }

        if (verbose) printf("\ta. Check initial state of x1.\n");
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x1 <op> x1.\n");
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 2. Create an object x2 (copy from x1)."
                            "\t\t{ x1:0  x2:0 }\n");
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { T_;  P(X2); }

        if (verbose) printf("\ta. Check the initial state of x2.\n");
        ASSERT(0 == X2.seconds());
        ASSERT(0 == X2.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x2 <op> x1, x2.\n");
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 3. Set x1 to a new value VA."
                            "\t\t\t{ x1:VA x2:0 }\n");
        mX1.setInterval(SA, NA);
        if (verbose) { T_;  P(X1); }

        if (verbose) printf("\ta. Check new state of x1.\n");
        ASSERT(SA == X1.seconds());
        ASSERT(NA == X1.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x1 <op> x1, x2.\n");
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 4. Set x2 to a new value VA."
                            "\t\t\t{ x1:VA x2:VA }\n");
        mX2.addSeconds(SA);     // Use different manipulators to get to the
        mX2.addNanoseconds(NA); // same place...
        if (verbose) { T_;  P(X2); }

        if (verbose) printf("\ta. Check initial state of x2.\n");
        ASSERT(SA == X2.seconds());
        ASSERT(NA == X2.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x2 <op> x1, x2.\n");
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 5. Set x2 to VB."
                            "\t\t\t\t{ x1:VA x2:VB }\n");
        mX2.setInterval(SB, NB);
        if (verbose) { T_;  P(X2); }

        if (verbose) printf("\ta. Check new state of x2.\n");
        ASSERT(SB == X2.seconds());
        ASSERT(NB == X2.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x2 <op> x1, x2.\n");
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 6. Set x1 to 0."
                            "\t\t\t\t{ x1:0  x2:VB }\n");
        mX1.setInterval(0, 0);
        if (verbose) { T_;  P(X1); }

        if (verbose) printf("\ta. Check new state of x1.\n");
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x1 <op> x1, x2.\n");
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 7. Create an object x3 with value VC."
                            "\t\t{ x1:0  x2:VB x3:VC }\n");

        Obj mX3(SC, NC);  const Obj& X3 = mX3;
        if (verbose) { T_;  P(X3); }

        if (verbose) printf("\ta. Check new state of x3.\n");
        ASSERT(SC == X3.seconds());
        ASSERT(NC == X3.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x3 <op> x1, x2, x3.\n");
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 8. Create an object x4 (copy from x1)."
                             "\t\t{ x1:0  x2:VB x3:VC x4:0 }\n");

        Obj mX4(X1);  const Obj& X4 = mX4;
        if (verbose) { T_;  P(X4); }

        if (verbose) printf("\ta. Check initial state of x4.\n");
        ASSERT(0 == X4.seconds());
        ASSERT(0 == X4.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4.\n");
        ASSERT(1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 9. Assign x2 = x1."
                            "\t\t\t\t{ x1:0  x2:0  x3:VC x4:0 }\n");

        mX2 = X1;
        if (verbose) { T_;  P(X2); }

        if (verbose) printf("\ta. Check new state of x2.\n");
        ASSERT(0 == X2.seconds());
        ASSERT(0 == X2.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4.\n");
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(1 == (X2 == X4));        ASSERT(0 == (X2 != X4));


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 10. Assign x2 = x3."
                            "\t\t\t\t{ x1:0  x2:VC x3:VC x4:0 }\n");

        mX2 = X3;
        if (verbose) { T_;  P(X2); }

        if (verbose) printf("\ta. Check new state of x2.\n");
        ASSERT(SC == X2.seconds());
        ASSERT(NC == X2.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4.\n");
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 11. Assign x1 = x1 (aliasing)."
                            "\t\t\t{ x1:0  x2:VC x3:VC x4:0 }\n");

        mX1 = X1;
        if (verbose) { T_;  P(X1); }

        if (verbose) printf("\ta. Check new state of x1.\n");
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.nanoseconds());

        if (verbose) printf(
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4.\n");
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

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
// Copyright 2020 Bloomberg Finance L.P.
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
