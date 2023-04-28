// bdljsn_numberutil.t.cpp                                            -*-C++-*-
#include <bdljsn_numberutil.h>

#include <bdldfp_decimal.h>
#include <bdldfp_decimalconvertutil.h>
#include <bdldfp_decimalutil.h>

#include <bdlma_guardingallocator.h>

#include <bdlb_numericparseutil.h>
#include <bdlb_string.h>
#include <bdlb_stringviewutil.h>

#include <bdlpcre_regex.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_log.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsltf_templatetestfacility.h>

#include <bsl_cstddef.h>  // 'bsl::size_t'
#include <bsl_cstdlib.h>  // 'bsl::atoi'
#include <bsl_cstring.h>  // 'bsl::strlen', 'bsl::memcpy'
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using namespace BloombergLP;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test provide a utility for working with JSON number text
// format.  As a state-less utility, the functions can largely be tested
// independently.  However, 'isValidNumber' is fundamental to the other
// functions (most require 'isValidNumber' as a precondition), so that is
// tested here first.
// ----------------------------------------------------------------------------
//
// NumberUtil
// ----------
// CLASS METHODS
// [ 9] bool isIntegralNumber(const bsl::string_view&);
// [ 3] bool isValidNumber(const bsl::string_view&);
// [ 6] float asFloat(const bsl::string_view&);
// [ 6] double asDouble(const bsl::string_view&);
// [ 7] bdldfp::Decimal64 asDecimal64(const bsl::string_view&);
// [ 7] int asDecimal64Exact(Decimal64 *, const bsl::string_view&);
// [11] int asInt(int *, const bsl::string_view&);
// [11] int asInt64(Int64 *, const bsl::string_view&);
// [11] int asUint(unsigned int *, const bsl::string_view&);
// [ 5] int asUint64(Uint64 *, const bsl::string_view&);
// [10] int asInteger(t_INTEGER_TYPE *, const bsl::string_view&);
// [12] void stringify(bsl::string *, Int64);
// [12] void stringify(bsl::string *, Uint64);
// [12] void stringify(bsl::string *, double);
// [12] void stringify(bsl::string *, const bdldfp::Decimal64& vlue);
// [ 8] bool areEqual(const string_view& , const string_view&);
//
// NumberUtil_ImpUtil
// ------------------
// CLASS METHODS
// [ 4] void decompose(...);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] USAGE EXAMPLE
// [ 3] CONCERN: 'IsValidNumber' functor can be used as an oracle.
// [ 2] CONCERN: Test Machinery
// [  ] CONCERN: Unexpected 'BSLS_REVIEW' failures should lead to test failures
// [  ] CONCERN: 'bsls::Log' messages are sent by the tested module only.
// [-1] BENCHMARK: asDecimal64Exact vs asDecimal64ExactOracle

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
//                    NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                              TEST APPARATUS
// ----------------------------------------------------------------------------

typedef BloombergLP::bdljsn::NumberUtil          Obj;
typedef BloombergLP::bdljsn::NumberUtil_ImpUtil  ImpUtil;
typedef bsls::Types::Int64                       Int64;
typedef bsls::Types::Uint64                      Uint64;

const int OK      = 0;
const int EOVER   = Obj::k_OVERFLOW;
const int EUNDER  = Obj::k_UNDERFLOW;
const int ENOTINT = Obj::k_NOT_INTEGRAL;

BSLA_MAYBE_UNUSED bool             verbose;
BSLA_MAYBE_UNUSED bool         veryVerbose;
BSLA_MAYBE_UNUSED bool     veryVeryVerbose;
BSLA_MAYBE_UNUSED bool veryVeryVeryVerbose;

// ============================================================================
//                HELPERS FOR REVIEW & LOG MESSAGE HANDLING
// ----------------------------------------------------------------------------

static bool containsCaseless(const bsl::string_view& string,
                             const bsl::string_view& subString)
    // Return 'true' if the specified 'subString' is present in the specified
    // 'string' disregarding case of alphabet characters '[a-zA-Z]', otherwise
    // return 'false'.
{
    if (subString.empty()) {
        return true;                                                  // RETURN
    }

    typedef bdlb::StringViewUtil SVU;
    const bsl::string_view rsv = SVU::strstrCaseless(string, subString);

    return !rsv.empty();
}

// ============================================================================
//                    EXPECTED 'BSLS_REVIEW' TEST HANDLERS
// ----------------------------------------------------------------------------

// These handlers are needed only temporarily until we determine how to fix the
// broken contract of 'bdlb::NumericParseUtil::parseDouble()' that says under-
// and overflow is not allowed yet the function supports it.

bool isBdlbNumericParseUtilReview(const bsls::ReviewViolation& reviewViolation)
    // Return 'true' if the specified 'reviewViolation' has been raised by the
    // 'bdlb_numericparseutil' component or no source file names are supported
    // by the build, otherwise return 'false'.
{
    const char *fn = reviewViolation.fileName();
    const bool fileOk = ('\0' == fn[0]) // empty or has the component name
                              || containsCaseless(fn, "bdlb_numericparseutil");
    return fileOk;
}

bool isRangeReview(const bsls::ReviewViolation& reviewViolation)
    // Return 'true' if the specified 'reviewViolation' is an overflow or
    // underflow message from the 'bdlb_numericparseutil' component (or no
    // source file names are supported by the build), otherwise return 'false'.
{
    return isBdlbNumericParseUtilReview(reviewViolation) &&
                    (containsCaseless(reviewViolation.comment(), "overflow") ||
                     containsCaseless(reviewViolation.comment(), "underflow"));
}

void ignoreRangeMsgs(const bsls::ReviewViolation& reviewViolation)
    // If the specified 'reviewViolation' is an expected overflow-related
    // message from 'parseDouble' do nothing, otherwise call
    // 'bsls::Review::failByAbort()'.
{
    if (!isRangeReview(reviewViolation)) {
        bsls::Review::failByAbort(reviewViolation);
    }
}

// ============================================================================
//                        JSON NUMBER VALIDATOR ORACLE
// ----------------------------------------------------------------------------

                           // ===================
                           // class IsValidNumber
                           // ===================
class IsValidNumber {
    // This functor class matches the textual pattern of a valid JSON number.

    // DATA
    bdlpcre::RegEx d_regEx;

  public:
    // CREATORS
    IsValidNumber();
        // Create a 'IsValidNumber' object that is prepared to match the
        // textual pattern of a valid JSON number.

    // ACCESSORS
    bool operator()(const bsl::string_view& text) const;
        // Return 'true' if the specified 'text' matches the pattern of a valid
        // JSON number, and 'false' otherwise.
};

                           // -------------------
                           // class IsValidNumber
                           // -------------------

// CREATORS
IsValidNumber::IsValidNumber()
{
    const char *pattern = "^"
                          "-?(0|[1-9][0-9]*)(\\.[0-9]+)?([eE][-+]?[0-9]+)?"
                          "\\z";  // matches end-of-string but not preceding
                                  // newline
    bsl::string errorMessage;
    bsl::size_t errorOffset;
    int         rc = d_regEx.prepare(&errorMessage,
                                     &errorOffset,
                                     pattern,
                                     0);
    ASSERT(0 == rc);
}

// ACCESSORS
bool IsValidNumber::operator()(const bsl::string_view& text) const
{
    ASSERT(d_regEx.isPrepared());

    return 0 == d_regEx.match(text);
}

// ============================================================================
//                             HELPER FUNCTIONS
// ----------------------------------------------------------------------------

template <class FP_TYPE>
FP_TYPE fabsval(FP_TYPE input)
    // Return the absolute value of the specified 'input'.
{
    return input >= FP_TYPE(0) ? input : -input;
}

// CLASS METHODS
template <class FP_TYPE>
int fuzzyCompare(FP_TYPE a, FP_TYPE b)
    // Return 'true' if the specified 'a' and 'b' are approximately the same
    // floating point value, and 'false' otherwise'.
{
    const FP_TYPE S_REL_TOL = FP_TYPE(1e-12);  // Default relative and
    const FP_TYPE S_ABS_TOL = FP_TYPE(1e-24);  // absolute tolerances.

    // First check for the *very* special case of true equality.

    if (a == b) {
        return true;                                                  // RETURN
    }

    // Next check for special case where the relative difference is "infinite".

    if (a == -b) {
        return fabsval(a - b) <= S_ABS_TOL;                           // RETURN
    }

    // Otherwise process the normal case involving both absolute and relative
    // tolerances.

    const FP_TYPE difference = fabsval(a - b);
    const FP_TYPE average    = fabsval((a + b) / FP_TYPE(2.0));

    return (difference <= S_ABS_TOL || difference / average <= S_REL_TOL);
}

int convertValue(double *result, const bsl::string_view& value)
    // Load the specified 'result' with the specified 'value'.  Note that these
    // function overloads provide a common interface for converting JSON number
    // text to an in-process number type that can be called generically.
{
  *result = Obj::NumberUtil::asDouble(value);
  return 0;
}

int convertValue(bdldfp::Decimal64 *result, const bsl::string_view& value)
    // Load the specified 'result' with the specified 'value'.  Note that these
    // function overloads provide a common interface for converting JSON number
    // text to an in-process number type that can be called generically.
{
    *result = Obj::NumberUtil::asDecimal64(value);
    return 0;
}

template <class t_INTEGER_TYPE>
int convertValue(t_INTEGER_TYPE *result, const bsl::string_view& value)
    // Load the specified 'result' with the specified 'value'.  Note that these
    // function overloads provide a common interface for converting JSON number
    // text to an in-process number type that can be called generically.
{
  return Obj::NumberUtil::asInteger(result, value);
}

void extractSignificand(bsl::string             *significand,
                        const bsl::string_view&  value)
    // Load the specified 'significand' from the specified 'value'.
{
    BSLS_ASSERT(Obj::isValidNumber(value));
    typedef bsl::string_view::size_type size_type;
    const size_type npos = bsl::string_view::npos;

    bsl::string_view text = value;

    bool isNeg = false;
    if ('-' == text[0]) {
        isNeg = true;
        text = text.substr(1);
    }

    significand->clear();
    size_type separator = text.find_first_of('.');
    if (separator != npos) {
        *significand += text.substr(0, separator);
        text = text.substr(separator + 1);
    }

    separator = text.find_first_of("eE");
    if (separator != npos) {
        *significand += text.substr(0, separator);
        text = text.substr(separator + 1);
    }
    else {
        *significand+=text;
        text = bsl::string_view();
    }

    // Remove leading 0s
    if (significand->size() > 1) {
        size_type firstNonZero = significand->find_first_not_of('0');
        if (bsl::string_view::npos != firstNonZero) {
            *significand = significand->substr(
                firstNonZero, significand->size() - firstNonZero);
        }
    }

    if (isNeg) {
        *significand = '-' +  *significand;
    }
}

void generateEquivalenceSet(bsl::vector<bsl::string> *result,
                            Int64                     minExponent,
                            Int64                     maxExponent,
                            const bsl::string_view&   value)
    // Load into the specified 'result' strings with equivalent numeric values
    // to the specified 'value' with different exponents, starting from the
    // specified 'minExponent' up to and including the specified 'maxExponent'.
    // If 'value' already has an exponent, its exponent will be adjusted in the
    // range 'minExponent' to 'maxExponent'.  For example, (-1, 1, "1e1") will
    // produce the strings ["10e0", "1e1", "0.1e2"].
{
    BSLS_ASSERT(Obj::isValidNumber(value));

    bool                        isNeg, isExpNegative;
    Int64                       exponentBias;
    bsl::string_view            integer, fraction, exponent, significant;
    bsl::string_view::size_type significantDotOffset;

    ImpUtil::decompose(&isNeg,
                       &isExpNegative,
                       &integer,
                       &fraction,
                       &exponent,
                       &significant,
                       &exponentBias,
                       &significantDotOffset,
                       value);

    Int64 originalExponent = 0;

    if (exponent.size() > 0) {
        bsl::string_view remainder;
        int rc = bdlb::NumericParseUtil::parseInt64(&originalExponent,
                                                          &remainder,
                                                          exponent);
        if (0 != rc || 0 != remainder.size()) {
            return;                                                   // RETURN
        }
        if (isExpNegative) {
            originalExponent = -originalExponent;
        }
    }

    bsl::string digits(significant.data(), fraction.data() + fraction.size());
    if ('0' == digits[0]) {
        digits.resize(1);
    }

    if (fraction.data() <= significant.data()) {
        // Removed leading 0's, so we must bias the exponent.
        originalExponent += exponentBias;
    }

    typedef bsl::string::size_type SizeType;
    SizeType decimalPos = digits.find_first_of('.');
    if (decimalPos != bsl::string::npos) {
        digits.erase(digits.begin() + decimalPos);
    }
    else {
        decimalPos = digits.size();
    }

    Int64 startExponent = originalExponent + minExponent;
    Int64 endExponent   = originalExponent + maxExponent;

    for (Int64 exp = startExponent; exp <= endExponent; ++exp) {
        bsl::ostringstream output;

        Int64 adjustedDecimalPos = Int64(decimalPos) + originalExponent - exp;
        if (adjustedDecimalPos <= 0) {
            bsl::string zeros(SizeType(-adjustedDecimalPos), '0');

            output << "0." << zeros << digits << 'e' << bsl::to_string(exp);
        }
        else if (adjustedDecimalPos > static_cast<int>(digits.size())) {
            bsl::string zeros(SizeType(adjustedDecimalPos) - digits.size(),
                              '0');
            output << digits << zeros << 'e' << bsl::to_string(exp);
        }
        else if (adjustedDecimalPos == static_cast<int>(digits.size())) {
            output << digits << 'e' << bsl::to_string(exp);
        }
        else {
            bsl::string adjusted(digits);
            adjusted.insert(SizeType(adjustedDecimalPos), 1, '.');
            output << adjusted << 'e' << bsl::to_string(exp);
        }

        // If the number is 0, remove any leading 0's ("00" is not a valid JSON
        // number).

        bsl::string number = output.str();
        if (number[0] == '0' && number.size() > 1) {
            bsl::string::size_type firstNonZeroPos =
                                                 number.find_first_not_of('0');

            if (firstNonZeroPos == number.size() ||
                number[firstNonZeroPos] == 'e' ||
                number[firstNonZeroPos] == '.') {
                firstNonZeroPos -= 1;
            }

            if (firstNonZeroPos >= 1) {
                number.erase(0, firstNonZeroPos);
            }
        }

        // We apply the negative sign last, to avoid complexity when removing
        // leading 0s.

        if (isNeg) {
            number = "-" + number;
        }
        result->push_back(number);
    }
}

void removeTrailingZeros(bsl::string *value)
    // Remove any trailing '0' characters from the specified 'value'.
{
    bsl::string::size_type end = value->find_last_not_of('0');
    if (end == bsl::string::npos) {
        *value = "0";
        return;                                                       // RETURN
    }
    *value = value->substr(0, end + 1);
}

int verifyDecimal64Exactness(const char *value)
    // Return 0 if the specified  'value' is represented exactly as a
    // 'Decimal64' and 'DecimalUtil::k_INEXACT' if 'value' is not represented
    // exactly.  Note that this function is used as a sanity check for the test
    // data for testing 'asDecimal64Exact', providing an alternative
    // implementation to verify the expected status.
{
    enum {
        k_NOT_PRECISE = 0
    };

    bdldfp::Decimal64 d;
    int rc = bdldfp::DecimalUtil::parseDecimal64(&d, value);
    BSLS_ASSERT(0 == rc);
    (void)rc;

    bsl::ostringstream text;
    text << d;

    if (bdldfp::DecimalUtil::isInf(d)) {
        return Obj::k_INEXACT;                                        // RETURN
    }

    bsl::string significand, originalSignificand;
    extractSignificand(&originalSignificand, value);
    extractSignificand(&significand, text.str());

    if (significand == originalSignificand) {
        // Exact and precise.

        return 0;                                                     // RETURN
    }
    removeTrailingZeros(&significand);
    removeTrailingZeros(&originalSignificand);

    if (significand == originalSignificand) {
        // Dropped trailing 0s (exact but loss of precision)
        return 0;                                                     // RETURN
    }

    return Obj::k_INEXACT;
}

int asDecimal64ExactOracle(bdldfp::Decimal64       *result,
                           const bsl::string_view&  value)
    // Load the specified 'result' with the specified 'value', and return 0 if
    // 'result' is an exact representation of value.  Note that this is an
    // alternative implementation of 'NumberUtil::asDecimal64' that, rather
    // than entirely relying on the underlying 'inteldfp' library, converts the
    // digits and then uses 'makeDecimal64Raw' to pack the digits into a
    // 'Decimal64'.  This implementation might eventually be preferred because
    // it avoids a bug with 0 values with large or small exponents (e.g.,
    // "0e200") being treated as an inexact conversion, and also avoids the
    // need for a 0 terminated string except in cases where the conversion is
    // inexact (which we expect to be rare).  See benchmark in case -1.
{
    BSLS_ASSERT(Obj::isValidNumber(value));

    bool                        isNeg, isExpNegative;
    Int64                       exponentBias;
    bsl::string_view            integer, fraction, exponentStr, significant;
    bsl::string_view::size_type significantDotOffset;

    ImpUtil::decompose(&isNeg,
                       &isExpNegative,
                       &integer,
                       &fraction,
                       &exponentStr,
                       &significant,
                       &exponentBias,
                       &significantDotOffset,
                       value);

    if ('0' == significant[0]) {
        // Handle 0 as a special case (no need to consider the exponent).

        *result = bdldfp::Decimal64(0);
        return 0;                                                     // RETURN
    }

    const Int64  INT_MIN_VALUE  = bsl::numeric_limits<Int64>::min();
    const Uint64 UINT_MAX_VALUE = bsl::numeric_limits<Uint64>::max();

    Uint64 uExponent;

    int rc = ImpUtil::appendDigits(&uExponent, 0, exponentStr);

    // The 'exponentBias' is determined by the number of leading or trailing
    // 0s, an ridiculously high value should not be possible.  Note we negate
    // 'exponentBias' below.

    BSLS_ASSERT(exponentBias != INT_MIN_VALUE);

    // Determine the maximum valid exponent that the arithmetic below can
    // support (because 'uExponent' is converted to a signed integer).

    Uint64 maxComputableExponent = UINT_MAX_VALUE -
                                   static_cast<Uint64>((exponentBias > 0)
                                                           ? exponentBias
                                                           : -exponentBias);

    if (0 != rc || uExponent > maxComputableExponent) {
        if (isExpNegative) {
            *result = bdldfp::Decimal64(0);
            return Obj::k_INEXACT;                                    // RETURN
        }
        *result = bsl::numeric_limits<bdldfp::Decimal64>::infinity();
        return Obj::k_INEXACT;                                        // RETURN
    }

    Int64 exponent = (isExpNegative ? -1 : 1) * static_cast<Int64>(uExponent) +
                     exponentBias;

    if (exponent < -398 || exponent > 369) {
        *result = Obj::asDecimal64(value);
        return Obj::k_INEXACT;                                        // RETURN
    }

    // A flag indicating if the significant digits range includes a '.'

    bool sigDigitsSeparated = significantDotOffset != bsl::string_view::npos;
    Int64 numSigDigits = significant.size() - (sigDigitsSeparated  ? 1 : 0);

    if (numSigDigits > 16) {
        *result = Obj::asDecimal64(value);
        return Obj::k_INEXACT;                                        // RETURN
    }

    Uint64 significand;
    bsl::string_view digits, moreDigits;
    if (!sigDigitsSeparated) {
        // If significant digits do not have a '.' in the middle

        digits = bsl::string_view(significant);
    }
    else {
        digits     = significant.substr(0, significantDotOffset);
        moreDigits = significant.substr(significantDotOffset + 1);
        BSLS_ASSERT(0 != digits.size());
        BSLS_ASSERT(0 != moreDigits.size());
    }

    rc = ImpUtil::appendDigits(&significand, 0, digits);
    BSLS_ASSERT(0 == rc);
    rc = ImpUtil::appendDigits(&significand, significand, moreDigits);
    BSLS_ASSERT(0 == rc);
    Int64 signedSignficand =
        isNeg ? -static_cast<Int64>(significand) : significand;

    *result = bdldfp::DecimalUtil::makeDecimalRaw64(
        signedSignficand, static_cast<int>(exponent));

    return 0;
}

// ============================================================================
//                          TEMPLATED TEST CASE 10
// ----------------------------------------------------------------------------

template <class t_INTEGRAL_TYPE>
struct AsIntegralTest {
    // Provide a 'bsltf_templatetestfacility' compatible test case for testing
    // 'asIntegral'.

    // CONSTANTS
    static const bool d_isSignedType =
              BloombergLP::bdljsn::NumberUtil_IsSigned<t_INTEGRAL_TYPE>::value;

    static void testCase10()
        // See documentation for test case 10.

    {
        t_INTEGRAL_TYPE k_MAX_VALUE =
                                   bsl::numeric_limits<t_INTEGRAL_TYPE>::max();

        const bool N = true;
        const bool P = false;

        static const Uint64 k_UNSET = 42;

        const Uint64 k_MAX = bsl::numeric_limits<Uint64>::max();

        // 'd_result' is held in a 'Uint64' to allow for testing 'Uint64', but
        // requires holding a separate 'd_isNegative' flag.

        struct Data {
            int         d_line;
            int         d_status;
            const char *d_input;
            bool        d_isNegative;
            Uint64      d_result;
        } DATA[] = {
                // Test a variety of 0 values
            { L_, OK,                        "0", P,           0    },
            { L_, OK,                      "0.0", P,           0    },
            { L_, OK,                   "0.0000", P,           0    },
            { L_, OK,            "0.0000e100000", P,           0    },
            { L_, OK,              "0.0e-100000", P,           0    },
            { L_, OK,             "0.00e+100000", P,           0    },
            { L_, OK,            "0.000e-100000", P,           0    },
            { L_, OK,    "0.00000000e1000000000", P,           0    },
            { L_, OK,   "0e18446744073709551615", P,           0    },
            { L_, OK,  "0e-18446744073709551615", P,           0    },

                // Test a variety of integers
            { L_, OK,                        "1", P,           1    },
            { L_, OK,                     "1e-0", P,           1    },
            { L_, OK,                     "1e+0", P,           1    },
            { L_, OK,                       "10", P,          10    },
            { L_, OK,                      "105", P,         105    },
            { L_, OK,                      "1e1", P,          10    },
            { L_, OK,                    "1.5e1", P,          15    },
            { L_, OK,                    "1.5e1", P,          15    },
            { L_, OK,                    "1.5e2", P,         150    },
            { L_, OK,                    "1.0e2", P,         100    },
            { L_, OK,                 "150.0e-1", P,          15    },
            { L_, OK,                "-150.0e-1", N,          15    },
            { L_, OK,            "1E+0000000001", P,          10    },
            { L_, OK,                    "-1e10", N, 10000000000ULL },

                // min and max values for various integral types
            { L_, OK,                    "-128", N,                 128    },
            { L_, OK,                     "127", P,                 127    },
            { L_, OK,                     "255", P,                 255    },
            { L_, OK,                  "-32768", N,               32768    },
            { L_, OK,                   "32767", P,               32767    },
            { L_, OK,                   "65535", P,               65535    },
            { L_, OK,             "-2147483648", N,          2147483648ULL },
            { L_, OK,              "2147483647", P,          2147483647ULL },
            { L_, OK,              "4294967295", P,          4294967295ULL },
            { L_, OK,    "-9223372036854775808", N, 9223372036854775808ULL },
            { L_, OK,     "9223372036854775807", P, 9223372036854775807ULL },
            { L_, OK,    "18446744073709551615", P, k_MAX                  },

                // test values whose expected values can't be represented
                // 'd_expected' should not be used.

            { L_,    EOVER,   "1e184467440737095516160", P,    k_UNSET },
            { L_,    EOVER,      "18446744073709551616", P,    k_UNSET },
            { L_,    EOVER,     "-18446744073709551616", N,    k_UNSET },
            { L_,    EOVER, "0.18446744073709551616e20", P,    k_UNSET },
            { L_,    EOVER,"-0.18446744073709551616e20", N,    k_UNSET },

                            // a variety of non-integral values
            { L_, ENOTINT,                       "0.5", P,        0 },
            { L_, ENOTINT,                      "-1.5", N,        1 },
            { L_, ENOTINT,                     "-10.5", N,       10 },
            { L_, ENOTINT,                      "10.5", P,       10 },
            { L_, ENOTINT,                     "1e-10", P,        0 },
            { L_, ENOTINT,                    "1.5e-0", P,        1 },
            { L_, ENOTINT,                    "1.5e+0", P,        1 },
            { L_, ENOTINT,                  "1e-99999", P,        0 },
            { L_, ENOTINT,  "1e-184467440737095516160", P,        0 },
            { L_, ENOTINT,             "1E-0000000001", P,        0 },
            { L_, ENOTINT,                     "1.5e0", P,        1 },
            { L_, ENOTINT,                 "1000.5e-1", P,      100 },
            { L_, ENOTINT,    "18446744073709551614.5", P, k_MAX -1 },
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        const char *TYPE = bsls::NameOf<t_INTEGRAL_TYPE>();

        if (verbose) {
            bsl::cout << "\t\tTest " << TYPE << " for a range of varied input"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE         = DATA[i].d_line;
            const int     STATUS       = DATA[i].d_status;
            const char   *INPUT        = DATA[i].d_input;
            const bool    ISNEG        = DATA[i].d_isNegative;
            const Uint64  EXPECTED_U64 = DATA[i].d_result;

            if (veryVerbose) {
                P_(INPUT); P_(STATUS); P_(TYPE); P_(ISNEG); P(EXPECTED_U64);
            }

            t_INTEGRAL_TYPE result;
            int             rc = Obj::asInteger(&result, INPUT);

            // The following logic computes the expected result and expectedRc
            // for 't_INTEGRAL_TYPE'.

            Uint64 maxAsU64 = static_cast<Uint64>(k_MAX_VALUE);
            Uint64 minAsU64 = (d_isSignedType) ? maxAsU64 + 1 : 0;

            t_INTEGRAL_TYPE expected;
            int             expectedRc = (STATUS == ENOTINT) ? ENOTINT : 0;

            if (!ISNEG && (EXPECTED_U64 > maxAsU64 || STATUS == EOVER)) {
                // The 'INPUT' is a positive value, and its 'VALUE' is above
                // the maximum representable value for this type (or outside
                // the range 'EXPECTED_U64' can represent).

                expectedRc = Obj::k_OVERFLOW;
                expected   = bsl::numeric_limits<t_INTEGRAL_TYPE>::max();
            }
            else if (ISNEG && (EXPECTED_U64 > minAsU64 || STATUS == EOVER)) {
                // The 'INPUT' is a negative value, and its 'VALUE' is below
                // the minimum representable value for this type (or outside
                // the range 'EXPECTED_U64' can represent).

                expectedRc = Obj::k_UNDERFLOW;
                expected   = bsl::numeric_limits<t_INTEGRAL_TYPE>::min();
            }
            else {
                // The expected result can be represented in 't_INTEGRAL_TYPE'.

                expected = static_cast<t_INTEGRAL_TYPE>(EXPECTED_U64);
                if (ISNEG) {
                    expected = static_cast<t_INTEGRAL_TYPE>(
                                  static_cast<t_INTEGRAL_TYPE>(-1) * expected);
                }
            }

            ASSERTV(LINE, INPUT, TYPE, minAsU64, maxAsU64,
                    (Int64)expected, (Int64)result,
                    expected == result);
            ASSERTV(LINE, INPUT, TYPE, (Int64)expectedRc, (Int64)rc,
                     expectedRc == rc);

            if (expected != result || expectedRc != rc) {
                // Do not unnecessarily repeat a large number of error reports.
                continue;                                           // CONTINUE
            }

            // Generate equivalent numeric values but with a wide range of
            // exponents (e.g, if we have the value 1e1, generate the strings
            // "1000e-2" through ".01e3").

            bsl::vector<bsl::string> equivalentData;
            generateEquivalenceSet(&equivalentData, -100, 100, INPUT);
            bsl::vector<bsl::string>::const_iterator it =
                                                        equivalentData.begin();
            for (; it != equivalentData.end(); ++it) {
                const bsl::string& input = *it;

                t_INTEGRAL_TYPE result;

                int rc = Obj::asInteger(&result, input);

                ASSERTV(LINE, input, TYPE, expected, result,
                        expected == result);
                ASSERTV(LINE, input, TYPE, expectedRc, rc,
                        expectedRc == rc);
            }
        }
    }
};

// ============================================================================
//                         JSONTestSuite  Data
// ----------------------------------------------------------------------------

// Test data generated from https://github.com/nst/JSONTestSuite (MIT License)
// Generation done by the python script embedded below.  Then the numeric test
// points extracted and messaged a bit.
//
// Two modified tests:
// n_multidigit_number_then_00.json - test data format can't handle embedded 0,
//                                    manually tested
//
// y_number_after_space.json - this is not a valid number according to the JSON
//                             spec.  A whitespace prefix will be handled by
//                             the tokenizer it bdljsn.

struct JSonSuiteNumericData {
    int         d_line;
    const char *d_testName;
    bool        d_isValid;
    const char *d_input;
} JSON_SUITE_DATA[] =
{
  { L_, "i_number_double_huge_neg_exp.json", true, "123.456e-789" }
, { L_, "i_number_huge_exp.json", true, "0."
    "4e00669999999999999999999999999999999999999999999999999999999999999999999"
    "999999999999999999999999999999999999999999999999969999999006" }
, { L_, "i_number_neg_int_huge_exp.json", true, "-1e+9999" }
, { L_, "i_number_pos_double_huge_exp.json", true, "1.5e+9999" }
, { L_, "i_number_real_neg_overflow.json", true, "-123123e100000" }
, { L_, "i_number_real_pos_overflow.json", true, "123123e100000" }
, { L_, "i_number_real_underflow.json", true, "123e-10000000" }
, { L_, "i_number_too_big_neg_int.json", true,
        "-123123123123123123123123123123" }
, { L_, "i_number_too_big_pos_int.json", true, "100000000000000000000" }
, { L_, "i_number_very_big_negative_int.json", true,
        "-237462374673276894279832749832423479823246327846" }
, { L_, "n_number_++.json", false, "++1234" }
, { L_, "n_number_+1.json", false, "+1" }
, { L_, "n_number_+Inf.json", false, "+Inf" }
, { L_, "n_number_-01.json", false, "-01" }
, { L_, "n_number_-1.0..json", false, "-1.0." }
, { L_, "n_number_-2..json", false, "-2." }
, { L_, "n_number_-NaN.json", false, "-NaN" }
, { L_, "n_number_.-1.json", false, ".-1" }
, { L_, "n_number_.2e-3.json", false, ".2e-3" }
, { L_, "n_number_0.1.2.json", false, "0.1.2" }
, { L_, "n_number_0.3e+.json", false, "0.3e+" }
, { L_, "n_number_0.3e.json", false, "0.3e" }
, { L_, "n_number_0.e1.json", false, "0.e1" }
, { L_, "n_number_0_capital_E+.json", false, "0E+" }
, { L_, "n_number_0_capital_E.json", false, "0E" }
, { L_, "n_number_0e+.json", false, "0e+" }
, { L_, "n_number_0e.json", false, "0e" }
, { L_, "n_number_1.0e+.json", false, "1.0e+" }
, { L_, "n_number_1.0e-.json", false, "1.0e-" }
, { L_, "n_number_1.0e.json", false, "1.0e" }
, { L_, "n_number_1_000.json", false, "1 000.0" }
, { L_, "n_number_1eE2.json", false, "1eE2" }
, { L_, "n_number_2.e+3.json", false, "2.e+3" }
, { L_, "n_number_2.e-3.json", false, "2.e-3" }
, { L_, "n_number_2.e3.json", false, "2.e3" }
, { L_, "n_number_9.e+.json", false, "9.e+" }
, { L_, "n_number_Inf.json", false, "Inf" }
, { L_, "n_number_NaN.json", false, "NaN" }
, { L_, "n_number_U+FF11_fullwidth_digit_one.json", false, "１" }
, { L_, "n_number_expression.json", false, "1+2" }
, { L_, "n_number_hex_1_digit.json", false, "0x1" }
, { L_, "n_number_hex_2_digits.json", false, "0x42" }
, { L_, "n_number_infinity.json", false, "Infinity" }
, { L_, "n_number_invalid+-.json", false, "0e+-1" }
, { L_, "n_number_invalid-negative-real.json", false, "-123.123foo" }
, { L_, "n_number_invalid-utf-8-in-bigger-int.json", false, "123�" }
, { L_, "n_number_invalid-utf-8-in-exponent.json", false, "1e1�" }
, { L_, "n_number_invalid-utf-8-in-int.json", false, "0�" }
, { L_, "n_number_minus_infinity.json", false, "-Infinity" }
, { L_, "n_number_minus_sign_with_trailing_garbage.json", false, "-foo" }
, { L_, "n_number_minus_space_1.json", false, "- 1" }
, { L_, "n_number_neg_int_starting_with_zero.json", false, "-012" }
, { L_, "n_number_neg_real_without_int_part.json", false, "-.123" }
, { L_, "n_number_neg_with_garbage_at_end.json", false, "-1x" }
, { L_, "n_number_real_garbage_after_e.json", false, "1ea" }
, { L_, "n_number_real_with_invalid_utf8_after_e.json", false, "1e�" }
, { L_, "n_number_real_without_fractional_part.json", false, "1." }
, { L_, "n_number_starting_with_dot.json", false, ".123" }
, { L_, "n_number_with_alpha.json", false, "1.2a-3" }
, { L_, "n_number_with_alpha_char.json", false, "1.8011670033376514H-308" }
, { L_, "n_number_with_leading_zero.json", false, "012" }
, { L_, "y_number.json", true, "123e65" }
, { L_, "y_number_0e+1.json", true, "0e+1" }
, { L_, "y_number_0e1.json", true, "0e1" }
, { L_, "y_number_after_space.json", false, " 4" }   // modified
, { L_, "y_number_double_close_to_zero.json", true,
     "-0.0000000000000000000000000000000000"
     "00000000000000000000000000000000000000000001" }
, { L_, "y_number_int_with_exp.json", true, "20e1" }
, { L_, "y_number_minus_zero.json", true, "-0" }
, { L_, "y_number_negative_int.json", true, "-123" }
, { L_, "y_number_negative_one.json", true, "-1" }
, { L_, "y_number_negative_zero.json", true, "-0" }
, { L_, "y_number_real_capital_e.json", true, "1E22" }
, { L_, "y_number_real_capital_e_neg_exp.json", true, "1E-2" }
, { L_, "y_number_real_capital_e_pos_exp.json", true, "1E+2" }
, { L_, "y_number_real_exponent.json", true, "123e45" }
, { L_, "y_number_real_fraction_exponent.json", true, "123.456e78" }
, { L_, "y_number_real_neg_exp.json", true, "1e-2" }
, { L_, "y_number_real_pos_exponent.json", true, "1e+2" }
, { L_, "y_number_simple_int.json", true, "123" }
, { L_, "y_number_simple_real.json", true, "123.456789" }
};

const int NUM_JSON_SUITE_DATA = sizeof(JSON_SUITE_DATA) /
                                sizeof(*JSON_SUITE_DATA);

#if 0
const char *JSONTESTSUITE_PYTHON_SCRIPT =
R"python(
import argparse
import os
import sys

DESCRIPTION = "Convert the JSON Test Suite to a BDE-style test table."

def main():
    parser = argparse.ArgumentParser(
        description=DESCRIPTION,
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument('directory',type=str,
                        help="directory location containing json examples "
                        "(JSONTestSuite/test_parsing)")

    args = parser.parse_args()

    directory = os.fsencode(args.directory)
    for file in sorted(os.listdir(directory)):
        filename = os.fsdecode(file)
        if filename.endswith(".json"):
            success = filename.startswith(
                "y_") or filename.startswith("i_")
            with open(os.path.join(directory, file), 'rb') as input:
                json = input.read()
            if len(json) > 100:
                json = b"Skipped Too Long"
            success_text = " true" if success else "false";
            print(f', {{ L_, "{filename}", {success_text}, "',
                  end = "", flush = True)
            sys.stdout.buffer.write(json)
            print('" }', flush = True)

if __name__ == "__main__":
    main()
)python";
#endif // C++11

// ============================================================================
//                              FUZZ TESTING
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The following function, 'LLVMFuzzerTestOneInput', is the entry point for the
// clang fuzz testing facility.  See {http://bburl/BDEFuzzTesting} for details
// on how to build and run with fuzz testing enabled.
// ----------------------------------------------------------------------------

#ifdef BDE_ACTIVATE_FUZZ_TESTING
#define main test_driver_main
#endif

extern "C"
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
    // Use the specified 'data' array of 'size' bytes as input to methods of
    // this component and return zero.
{
    const char *FUZZ  = reinterpret_cast<const char *>(data);
    const int  LENGTH = static_cast<int>(size);
    const int  test   = 3;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        static const IsValidNumber oracle;

        bsl::string_view INPUT(FUZZ, LENGTH);

        const bool expected = oracle(INPUT);
        const bool result   = Obj::isValidNumber(INPUT);

        ASSERTV(INPUT, LENGTH, expected, result, expected == result);

      } break;
      default: {
      } break;
    }

    if (testStatus > 0) {
        BSLS_ASSERT_INVOKE("FUZZ TEST FAILURES");
    }

    return 0;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: Unexpected 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&ignoreRangeMsgs);

    // CONCERN: In no case does memory come from the global allocator.
    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 13: {
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

        if (verbose) {
            bsl::cout << "\nUSAGE EXAMPLE"
                         "\n=============" << bsl::endl;
        }

        if (!verbose) {
            // Usage examples are supposed to use ASSERT to make their point,
            // not direct printouts as a test driver is not supposed to print
            // anything but the test case number executed when no verbosity was
            // requested on the command line.  Therefore we do not execute this
            // usage example unless output was requested.
            break;                                                     // BREAK
        }

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Interpreting a JSON Number String
/// - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using 'bdljsn::NumberUtil' to work with a JSON
// number string.  Imagine we are given and array of strings for numbers we
// expect to be integers, for each string we want to render some properties for
// that number.
//
// First, we define an interesting set of example data:
//..
    const char *EXAMPLE_DATA[] = {
       // value                               converted int value & notes
       // -----                               ---------------------------
       "NaN",                                // invalid number
       "INF",                                // invalid number
       "1",                                  // 1,         exact
       "1.5",                                // 1,         not an integer
       "-9223372036854775809",               // INT64_MIN, underflow
       "1.5e27",                             // INT64_MAX, overflow
    };
    const int NUM_DATA = sizeof(EXAMPLE_DATA) / sizeof(*EXAMPLE_DATA);
//..
// Then, for each number, we first check whether it is a valid JSON Number
// (note that the behavior for the other methods is undefined unless the text
// is a valid JSON Number):
//..
    for (int i = 0; i < NUM_DATA; ++i) {
        const char *EXAMPLE = EXAMPLE_DATA[i];
        bsl::cout << "\"" << EXAMPLE << "\": " << bsl::endl;
        if (!bdljsn::NumberUtil::isValidNumber(EXAMPLE)) {
            bsl::cout << "  * is NOT a JSON Number" << bsl::endl;
            continue;                                               // CONTINUE
        }
//..
// Next we verify that the number is an integer.  This will return an accurate
// result even when the integer cannot be represented.
//..
        if (bdljsn::NumberUtil::isIntegralNumber(EXAMPLE)) {
            bsl::cout << "  * is an integer" << bsl::endl;
        }
        else {
            bsl::cout << "  * is not an integer" << bsl::endl;
        }
//..
// Finally, we convert that number to an integer:
//..
        bsls::Types::Int64 value;
        int rc = bdljsn::NumberUtil::asInt64(&value, EXAMPLE);
//
        bsl::cout << "  * value: " << value;
//
        if (bdljsn::NumberUtil::k_NOT_INTEGRAL == rc) {
            bsl::cout << "  (truncated)";
        }
        if (bdljsn::NumberUtil::k_OVERFLOW == rc) {
            bsl::cout << "  (overflow)";
        }
        if (bdljsn::NumberUtil::k_UNDERFLOW == rc) {
            bsl::cout << "  (underflow)";
        }
        bsl::cout << bsl::endl;
    }
//..
// This will output the text:
//..
//  "NaN":
//    * is NOT a JSON Number
//  "INF":
//    * is NOT a JSON Number
//  "1":
//    * is an integer
//    * value: 1
//  "1.5":
//    * is not an integer
//    * value: 1  (truncated)
//  "-9223372036854775809":
//    * is an integer
//    * value: -9223372036854775808  (underflow)
//  "1.5e27":
//    * is an integer
//    * value: 9223372036854775807  (overflow)
//..

      } break;

      case 12: {
        // --------------------------------------------------------------------
        // TESTING: stringify
        //   This is a white box test, the methods under test all simply
        //   forward to 'NumericFormatterUtil' and 'DecimalUtil'
        //
        //Concerns:
        //: 1 That 'stringify' correctly renders strings for a range of input.
        //:
        //: 2 That the resulting numbers can be converted back to an
        //:   equivalent in-process representation.
        //
        //Plan:
        //: 1 For each of the types, create a small set of test data checking
        //:   a range of values.  stringify the input, and verify the expected
        //:   result.  Then convert the string back to the original
        //:   representation and verify the round-tripped value is equal to the
        //:   original.
        //
        // Testing:
        //   void stringify(bsl::string *, Int64 );
        //   void stringify(bsl::string *, Uint64 );
        //   void stringify(bsl::string *, double );
        //   void stringify(bsl::string *, const bdldfp::Decimal64& vlue);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING: stringify"
                      << "\n==================" << bsl::endl;
        }

        if (verbose) bsl::cout << "\tTest stringify Int64" << bsl::endl;
        {
            struct Data {
                int         d_line;
                Int64       d_input;
                const char *d_expected;
            } DATA[] = {
                { L_,                          0,                    "0" },
                { L_,                         15,                   "15" },
                { L_,                        -15,                  "-15" },
                { L_,                        100,                  "100" },

                        // boundary checks
                { L_,      9223372036854775807LL,  "9223372036854775807" },
                { L_, -9223372036854775807LL - 1, "-9223372036854775808" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE     = DATA[i].d_line;
                const Int64  INPUT    = DATA[i].d_input;
                const char  *EXPECTED = DATA[i].d_expected;

                if (veryVeryVerbose) {
                    P_(LINE); P_(INPUT); P(EXPECTED);
                }
                bsl::string result;
                Obj::stringify(&result, INPUT);

                ASSERTV(LINE, INPUT, EXPECTED, result, EXPECTED == result);

                Int64 roundTrip;
                int rc = Obj::asInt64(&roundTrip, result);

                ASSERTV(LINE, INPUT, rc, roundTrip, result, 0 == rc);
                ASSERTV(LINE, INPUT, roundTrip, result, roundTrip == INPUT);
            }
        }

        if (verbose) bsl::cout << "\tTest stringify Uint64" << bsl::endl;
        {
            struct Data {
                int         d_line;
                Uint64      d_input;
                const char *d_expected;
            } DATA[] = {
                { L_,                          0,                    "0"  },
                { L_,                         15,                    "15" },
                { L_,                        100,                   "100" },

                        // boundary checks
                { L_,    18446744073709551615ULL,  "18446744073709551615" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int     LINE     = DATA[i].d_line;
                const Uint64  INPUT    = DATA[i].d_input;
                const char   *EXPECTED = DATA[i].d_expected;

                if (veryVeryVerbose) {
                    P_(LINE); P_(INPUT); P(EXPECTED);
                }
                bsl::string result;
                Obj::stringify(&result, INPUT);

                ASSERTV(LINE, INPUT, EXPECTED, result, EXPECTED == result);

                Uint64 roundTrip;
                int rc = Obj::asUint64(&roundTrip, result);

                ASSERTV(LINE, INPUT, rc, roundTrip, result, 0 == rc);
                ASSERTV(LINE, INPUT, roundTrip, result, roundTrip == INPUT);
            }
        }

        if (verbose) bsl::cout << "\tTest stringify double" << bsl::endl;
        {
            struct Data {
                int         d_line;
                double      d_input;
                const char *d_expected;
            } DATA[] = {
                { L_,                  0,                     "0" },
                { L_,                 15,                    "15" },
                { L_,                100,                   "100" },

                { L_,                1.5,                   "1.5" },
                { L_,        2.22045e-16,           "2.22045e-16" },
                { L_, 1.4142135623730951,    "1.4142135623730951" },
                { L_,  3.141592653589793,     "3.141592653589793" },

                        // boundary checks
                { L_,       1.79769e+308,          "1.79769e+308" },
                { L_,       2.22507e-308,          "2.22507e-308" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int     LINE     = DATA[i].d_line;
                const double  INPUT    = DATA[i].d_input;
                const char   *EXPECTED = DATA[i].d_expected;

                if (veryVeryVerbose) {
                    P_(LINE); P_(INPUT); P(EXPECTED);
                }
                bsl::string result;
                Obj::stringify(&result, INPUT);

                ASSERTV(LINE, INPUT, EXPECTED, result, EXPECTED == result);

                double roundTrip = Obj::asDouble(result);

                ASSERTV(LINE, INPUT, roundTrip, result, roundTrip == INPUT);
            }
        }

        if (verbose)
            bsl::cout << "\tTest stringify bdldfp::Decimal64" << bsl::endl;
        {
            struct Data {
                int         d_line;
                const char *d_text;
            } DATA[] = {
                { L_,                       "0" },
                { L_,                      "15" },
                { L_,                     "100" },

                    // boundary checks
                { L_,   "9.999999999999999e+384"},
                { L_,  "-9.999999999999999e+384"},
                { L_,   "9.999999999999999e-383"},
                { L_,  "-9.999999999999999e-383"},

            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int     LINE = DATA[i].d_line;
                const char   *TEXT = DATA[i].d_text;

                bdldfp::Decimal64 INPUT;

                int rc = bdldfp::DecimalUtil::parseDecimal64Exact(&INPUT,
                                                                  TEXT);
                ASSERT(0 == rc);

                if (veryVeryVerbose) {
                    P_(LINE); P_(TEXT); P(INPUT)
                }

                bsl::string result;
                Obj::stringify(&result, INPUT);

                ASSERTV(LINE, INPUT, TEXT, result, TEXT == result);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING: as[Ui|I]nt[64]
        //   This is a white box test, the methods under test all simply
        //   forward to 'asInteger' which was previously tested.
        //
        //Concerns:
        //: 1 That 'asInt', 'asUint', and 'asInt64' delegate to 'asInteger'
        //:   correctly.
        //:
        //: 2 That input at the boundaries of the valid range of the type are
        //:   correct
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        //Plan:
        //: 1 For each of the types, create a small set of test data checking
        //:   all possible error codes, and the boundaries of the
        //:   representable range of the type.  Verify the expected results
        //:   are returned for each input.
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when.
        //
        // Testing:
        //   int asInt(int *, const bsl::string_view& );
        //   int asUint(unsigned int *, const bsl::string_view& );
        //   int asInt64(Int64 *, const bsl::string_view& );
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING: as[Ui|I]nt[64]"
                      << "\n=======================" << bsl::endl;
        }

        if (verbose) bsl::cout << "\tTest asInt" << bsl::endl;
        {
            struct Data {
                int         d_line;
                int         d_expectedRc;
                int         d_expected;
                const char *d_input;
            } DATA[] = {
                { L_,      OK,                  0,                    "0" },
                { L_,      OK,                  0,              "-0.0000" },
                { L_,      OK,                 15,                   "15" },
                { L_,      OK,                -15,                  "-15" },
                { L_,      OK,                100,                  "1e2" },

                    // boundary checks
                { L_,      OK,         2147483647,           "2147483647" },
                { L_,   EOVER,         2147483647,           "2147483648" },
                { L_,      OK,        -2147483647-1,        "-2147483648" },
                { L_,  EUNDER,        -2147483647-1,        "-2147483649" },

                    // fraction
                { L_, ENOTINT,                  1,                "15e-1" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const int  EXPRC   = DATA[i].d_expectedRc;
                const int  EXPECTED = DATA[i].d_expected;
                const char *INPUT = DATA[i].d_input;

                if (veryVeryVerbose) {
                    P_(LINE); P_(INPUT); P_(EXPRC); P(EXPECTED);
                }
                int result;
                int rc = Obj::asInt(&result, INPUT);

                ASSERTV(LINE, INPUT, EXPECTED, result, EXPECTED == result);
                ASSERTV(LINE, INPUT, EXPRC, rc, EXPRC == rc);

            }
        }

        if (verbose) bsl::cout << "\tTest asUint" << bsl::endl;
        {
            struct Data {
                int           d_line;
                int           d_expectedRc;
                unsigned int  d_expected;
                const char   *d_input;
            } DATA[] = {
                { L_,      OK,                  0U,                   "0" },
                { L_,      OK,                  0U,             "-0.0000" },
                { L_,      OK,                 15U,                  "15" },
                { L_,      OK,                100U,                 "1e2" },

                    // boundary checks
                { L_,      OK,         4294967295U,          "4294967295" },
                { L_,   EOVER,         4294967295U,          "4294967296" },
                { L_,  EUNDER,                  0U,                  "-1" },

                    // fraction
                { L_, ENOTINT,                  1U,               "15e-1" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int           LINE     = DATA[i].d_line;
                const int           EXPRC    = DATA[i].d_expectedRc;
                const unsigned int  EXPECTED = DATA[i].d_expected;
                const char         *INPUT    = DATA[i].d_input;

                if (veryVeryVerbose) {
                    P_(LINE); P_(INPUT); P_(EXPRC); P(EXPECTED);
                }
                unsigned int result;
                int rc = Obj::asUint(&result, INPUT);

                ASSERTV(LINE, INPUT, EXPECTED, result, EXPECTED == result);
                ASSERTV(LINE, INPUT, EXPRC, rc, EXPRC == rc);

            }
        }

        if (verbose) bsl::cout << "\tTest asInt64" << bsl::endl;
        {
            struct Data {
                int         d_line;
                int         d_expectedRc;
                Int64       d_expected;
                const char *d_input;
            } DATA[] = {
        { L_,      OK,                  0,                    "0" },
        { L_,      OK,                  0,              "-0.0000" },
        { L_,      OK,                 15,                   "15" },
        { L_,      OK,                -15,                  "-15" },
        { L_,      OK,                100,                  "1e2" },

                    // boundary checks
        { L_,     OK,  9223372036854775807LL,  "9223372036854775807" },
        { L_,  EOVER,  9223372036854775807LL,  "9223372036854775808" },
        { L_,     OK, -9223372036854775807LL - 1, "-9223372036854775808" },
        { L_, EUNDER, -9223372036854775807LL - 1, "-9223372036854775809" },

                   // fraction
        { L_, ENOTINT,                  1,                "15e-1" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const int   EXPRC   = DATA[i].d_expectedRc;
                const Int64 EXPECTED = DATA[i].d_expected;
                const char *INPUT = DATA[i].d_input;

                if (veryVeryVerbose) {
                    P_(LINE); P_(INPUT); P_(EXPRC); P(EXPECTED);
                }
                Int64 result;
                int rc = Obj::asInt64(&result, INPUT);

                ASSERTV(LINE, INPUT, EXPECTED, result, EXPECTED == result);
                ASSERTV(LINE, INPUT, EXPRC, rc, EXPRC == rc);

            }
        }

        if (verbose) bsl::cout << "\tNegative Testing." << bsl::endl;
        {
            bsls::AssertTestHandlerGuard hG;
            {
                int result;
                ASSERT_PASS(Obj::asInt(&result, "0"));
                ASSERT_FAIL(Obj::asInt(&result, "NaN"));
            }
            {
                unsigned int result;
                ASSERT_PASS(Obj::asUint(&result, "0"));
                ASSERT_FAIL(Obj::asUint(&result, "NaN"));
            }
            {
                Int64 result;
                ASSERT_PASS(Obj::asInt64(&result, "0"));
                ASSERT_FAIL(Obj::asInt64(&result, "NaN"));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING: asInteger
        //   This is a white box test. 'asInteger' delegates to 'asUint64' and
        //   then performs appropriate bounds checks for the result type.
        //
        //Concerns:
        //: 1 For integers between bsl::numeric_limits<T> min and max,
        //:   that number and a status of 0 are returned.
        //:
        //: 2 For values with fractions between bsl::numeric_limits<T> min and
        //:   max, the truncated integer a status of k_NOT_INTEGER are
        //:   returned.
        //:
        //: 3 For numbers outside of the range bsl::numeric_limits<T> min and
        //:   max, that the min or max value (respectively) is returned with a
        //:   status of k_UNDERFLOW or k_OVERFLOW (respectively).
        //:
        //: 4 That negative values are correctly handled.
        //:
        //: 5 That input with a large number of digits is correctly handled.
        //:
        //: 6 That input with very large (or small) exponents is correctly
        //:   handled.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        //Plan:
        //: 1 Use the 'bsltf_templatetestfacility' to apply the test over
        //:   a range of signed and unsigned integral types.  For a test table
        //:   that tests a range of varied input, compute the expected result
        //:   from the canonical expected result in the test table, and verify
        //:   'asInteger' returns the computed expected result.
        //:
        //:   1 For each test point, generate a set of equivalent
        //:     representations with varying exponents, and verify the same
        //:     result is returned.
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when.
        //
        // Testing:
        //   int asInteger(t_INTEGER_TYPE *, const bsl::string_view& );
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING: asInteger"
                      << "\n==================" << bsl::endl;
        }
        if (verbose)
            bsl::cout << "\tUse the template test facility" << bsl::endl;
        {
#define INTEGRAL_TYPES char, unsigned char, short, unsigned short, int, \
        unsigned int, long, unsigned long, long long, unsigned long long

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(AsIntegralTest,
                                                 testCase10,
                                                 INTEGRAL_TYPES);
#undef INTEGRAL_TYPES
        }

        if (verbose) bsl::cout << "\tNegative Testing." << bsl::endl;
        {
            bsls::AssertTestHandlerGuard hG;

            int dummy;
            ASSERT_PASS(Obj::asInteger(&dummy, "0"));
            ASSERT_FAIL(Obj::asInteger(&dummy, "NaN"));
        }
      } break;

      case 9: {
        // --------------------------------------------------------------------
        // TESTING: isIntegerNumber
        //
        //Concerns:
        //: 1 That 'isIntegerNumber' returns 'true' when the text supplied for
        //:   the arguments represents an integer, and 'false' otherwise.
        //:
        //: 2 That 'isIntegerNumber' correctly handles a wide variety of
        //:   representations for 0.
        //:
        //: 3 That 'isIntegerNumber' correctly handles positive and negative
        //:   numbers.
        //:
        //: 4 That 'isIntegerNumber' correctly handles positive and negative
        //:   exponents.
        //:
        //: 5 That 'isIntegerNumber' correctly handles very large numbers of
        //:   significand digits.
        //:
        //: 6 That 'isIntegerNumber' correctly handles very large, and very
        //:   small exponents.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        //Plan:
        //: 1 For a test table that tests a range of varied input,
        //:   verify that 'isIntegerNumber' returns the expected value.
        //:
        //:   1 For each test point, generate a set of equivalent
        //:     representations with varying exponents, and verify the same
        //:     result is returned.
        //:
        //: 2 For the same test table, test a couple extreme points from the
        //:   equivalence set with thousands of '0' characters.  Verify the
        //:   same result is returned.
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when.
        //
        // Testing:
        //   bool isIntegralNumber(const bsl::string_view& );
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING: isIntegerNumber"
                      << "\n========================" << bsl::endl;
        }

        if (verbose) {
            bsl::cout << "\tTest a wide array of valid JSON number text"
                      << bsl::endl;
        }

        const bool T = true;
        const bool F = false;

        struct Data {
            int         d_line;
            bool        d_isInteger;
            const char *d_input;
        } DATA[] = {
            { L_, T,                         "0" },
            { L_, T,                        "-0" },
            { L_, T,                       "0.0" },
            { L_, T,                    "0.0000" },
            { L_, T,             "0.0000e100000" },
            { L_, T,               "0.0e-100000" },
            { L_, T,              "0.00e+100000" },
            { L_, T,             "0.000e-100000" },
            { L_, T,    "0e18446744073709551615" },
            { L_, T,   "0e-18446744073709551615" },

            { L_, T,                         "1" },
            { L_, T,                     "1.0e0" },
            { L_, T,                     "0.1e1" },
            { L_, T,                    "0.1e+1" },
            { L_, T,                    "0.01e2" },
            { L_, T,                     "10e-1" },
            { L_, T,                   "10.0e-1" },

            { L_, F,                       "0.1" },
            { L_, F,                    "1.0e-1" },
            { L_, F,                     "0.1e0" },
            { L_, F,                   "0.01e+1" },
            { L_, F,                    "0.01e1" },
            { L_, F,                     "10e-2" },
            { L_, F,                   "10.0e-2" },

            { L_, T,                        "-1" },
            { L_, T,     "-18446744073709551615" },
            { L_, T,      "18446744073709551615" },
            { L_, T, "-184467440737095516151234" },
            { L_, T,  "184467440737095516151234" },
            { L_, T,   "18446744073709551610e-1" },
            { L_, T,  "10000000000000000000e-19" },
            { L_, F,  "10000000000000000000e-20" },
            { L_, F,   "18446744073709551615e-1" },
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const bool   EXP   = DATA[i].d_isInteger;
            const char *INPUT = DATA[i].d_input;

            if (veryVeryVerbose) {
                P_(LINE); P_(INPUT); P(EXP);
            }

            ASSERTV(LINE, INPUT, EXP, EXP == Obj::isIntegralNumber(INPUT));

            bsl::vector<bsl::string> equivalentData;
            generateEquivalenceSet(&equivalentData, -100, 100, INPUT);
            bsl::vector<bsl::string>::const_iterator it =
                                                        equivalentData.begin();
            for (; it != equivalentData.end(); ++it) {
                const bsl::string& input = *it;
                if (veryVeryVerbose) {
                    bsl::cout << "   ";
                    P_(LINE);
                    P(input);
                }
                ASSERTV(LINE, INPUT, EXP, EXP == Obj::isIntegralNumber(input));
            }
        }

        if (verbose) {
          bsl::cout << "\tTest an extreme number of digits" << bsl::endl;
        }
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const bool  EXP   = DATA[i].d_isInteger;
            const char *INPUT = DATA[i].d_input;

            if (veryVeryVerbose) {
                P_(LINE); P_(INPUT); P(EXP);
            }

            bsl::vector<bsl::string> equivalentData;
            generateEquivalenceSet(&equivalentData, -100000, -99999, INPUT);
            generateEquivalenceSet(&equivalentData,   99999, 100000, INPUT);
            bsl::vector<bsl::string>::const_iterator it =
                                                        equivalentData.begin();
            for (; it != equivalentData.end(); ++it) {
              const bsl::string& input = *it;
              if (veryVeryVerbose) {
                  bsl::cout << "   ";
                  P_(LINE); P_(INPUT); P(input.size());
              }
              ASSERTV(LINE, EXP, EXP == Obj::isIntegralNumber(input));
            }
        }

        if (verbose) bsl::cout << "\tNegative Testing." << bsl::endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bdldfp::Decimal64 result;
            ASSERT_PASS(Obj::isIntegralNumber("0"));
            ASSERT_FAIL(Obj::isIntegralNumber("NaN"));
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING: areEqual
        //
        //Concerns:
        //: 1 That 'areEqual' returns 'true' when the text supplied for
        //:   the arguments is the same.
        //:
        //: 2 That 'areEqual' returns 'true' when the text supplied for the
        //:   arguments is different, but the numeric values are the same.
        //:
        //: 3 That 'areEqual' returns 'false' when the values of numeric
        //:   arguments is different.
        //:
        //: 4 That 'areEqual' returns textual equality if the exponent of one
        //:   of the values cannot be represented in a 64-bit integer.
        //:
        //: 6 That 'areEqual' correctly handles very large numbers of digits in
        //: the representation or exponent.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        //Plan:
        //: 1 For a test table that tests a range of varied input broken down
        //    into equivalence groups, perform a double-nested iteration,
        //    comparing values in the test table against each other using the
        //    equivalence group to determine the expected result.
        //:
        //:   1 For each test point, generate a set of equivalent
        //:     representations with varying exponents, and verify the same
        //:     result is returned.
        //:
        //: 2 For the JSONTestSuite number data, for those numbers that are
        //:   valid, perform a double-nested iteration comparing values
        //:   against each other.  Use the iterator position and
        //:   'asDecimal64[Exact]' to determine the expected result.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when.
        //
        // Testing:
        //   bool areEqual(const string_view& , const string_view& );
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING: areEqual"
                      << "\n=================" << bsl::endl;
        }

        if (verbose) {
            bsl::cout << "\tTest a wide array of valid JSON number text"
                      << bsl::endl;
        }

        struct Data {
            int         d_line;              // line
            int         d_equalityGroup;     // group of equal numbers
            const char *d_input;             // input text
        } DATA[] = {
            { L_,     0,                         "0" },
            { L_,     0,                        "-0" },
            { L_,     0,                       "0.0" },
            { L_,     0,                    "0.0000" },
            { L_,     0,             "0.0000e100000" },
            { L_,     0,               "0.0e-100000" },
            { L_,     0,              "0.00e+100000" },
            { L_,     0,             "0.000e-100000" },
            { L_,     0,    "0e18446744073709551615" },
            { L_,     0,   "0e-18446744073709551615" },

            { L_,     1,                         "1" },
            { L_,     1,                     "1.0e0" },
            { L_,     1,                     "0.1e1" },
            { L_,     1,                    "0.1e+1" },
            { L_,     1,                    "0.01e2" },
            { L_,     1,                     "10e-1" },
            { L_,     1,                   "10.0e-1" },

            { L_,     2,                         "2" },
            { L_,     2,                     "2.0e0" },
            { L_,     2,                     "0.2e1" },
            { L_,     2,                    "0.2e+1" },
            { L_,     2,                    "0.02e2" },
            { L_,     2,                     "20e-1" },
            { L_,     2,                   "20.0e-1" },

            { L_,     3,                        "-1" },
            { L_,     3,                    "-1.0e0" },
            { L_,     3,                    "-0.1e1" },
            { L_,     3,                   "-0.1e+1" },
            { L_,     3,                   "-0.01e2" },
            { L_,     3,                    "-10e-1" },
            { L_,     3,                  "-10.0e-1" },

            { L_,     4,      "18446744073709551615" },
            { L_,     4, "0.18446744073709551615e20" },

            { L_,     5,    "1e18446744073709551615" },
            { L_,     5,   "1e+18446744073709551615" },
            { L_,     6,   "10e18446744073709551614" },

        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LHS_LINE  = DATA[i].d_line;
            const int   LHS_GROUP = DATA[i].d_equalityGroup;
            const char *LHS       = DATA[i].d_input;

            for (int j = 0; j < NUM_DATA; ++j) {
                const int   RHS_LINE  = DATA[j].d_line;
                const int   RHS_GROUP = DATA[j].d_equalityGroup;
                const char *RHS       = DATA[j].d_input;

                bool EXP = RHS_GROUP == LHS_GROUP;

                if (veryVeryVerbose) {
                    P_(LHS_LINE); P_(LHS_GROUP);
                    P_(RHS_LINE); P_(RHS_GROUP);
                    P_(LHS); P(RHS)

                }

                ASSERTV(LHS, RHS, LHS_GROUP, RHS_GROUP,
                        EXP == Obj::areEqual(LHS, RHS));

                bsl::vector<bsl::string> equivalentData;
                generateEquivalenceSet(&equivalentData, -100000, -99999, LHS);
                generateEquivalenceSet(&equivalentData,  99999,  100000, LHS);
                bsl::vector<bsl::string>::const_iterator it =
                                                        equivalentData.begin();
                for (; it != equivalentData.end(); ++it) {
                    const bsl::string& lhsBig = *it;
                    if (veryVeryVeryVerbose) {
                        bsl::cout << "   ";
                        P_(LHS); P_(lhsBig); P(lhsBig.size());
                    }
                    ASSERTV(LHS, RHS, EXP == Obj::areEqual(lhsBig, RHS));
                }
            }
        }
        if (verbose) {
            bsl::cout << "\tTest JSONSuite Test Data"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_JSON_SUITE_DATA; ++i) {
            const bool  L_EXP   = JSON_SUITE_DATA[i].d_isValid;
            const char *L_INPUT = JSON_SUITE_DATA[i].d_input;
            const char *L_NAME  = JSON_SUITE_DATA[i].d_testName;

            if (!L_EXP) {  // Ignore invalid JSON numbers.
                continue;                                           // CONTINUE
            }

            for (int j = 0; j < NUM_JSON_SUITE_DATA; ++j) {
                const bool  R_EXP   = JSON_SUITE_DATA[j].d_isValid;
                const char *R_INPUT = JSON_SUITE_DATA[j].d_input;
                const char *R_NAME  = JSON_SUITE_DATA[j].d_testName;
                if (!R_EXP) {  // Ignore invalid JSON numbers.
                    continue;                                       // CONTINUE
                }

                // We must jump through some hoops to determine the expected
                // result because the test input has values like "123.456e-789"
                // and "123e-10000000", which are not at the same iterator
                // position, but will compare true when converted to another
                // representation (both convert to 0).  The logic here assumes
                // that the test data does *not* include two different test
                // points that are numerically equal but do not convert to a
                // Decimal64 exactly (a property we manually verified).

                bdldfp::Decimal64 junk;
                bool lhsIsExact = 0 == Obj::asDecimal64Exact(&junk, L_INPUT);
                bool rhsIsExact = 0 == Obj::asDecimal64Exact(&junk, R_INPUT);

                bool expected = (i == j) || (lhsIsExact && rhsIsExact &&
                                             Obj::asDecimal64(L_INPUT) ==
                                                 Obj::asDecimal64(R_INPUT));

                ASSERTV(i, j, L_INPUT, R_INPUT, L_NAME, R_NAME, expected,
                        expected == Obj::areEqual(L_INPUT, R_INPUT));
            }
        }
        if (verbose) bsl::cout << "\tNegative Testing." << bsl::endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bdldfp::Decimal64 result;
            ASSERT_PASS(Obj::areEqual("0", "0"));
            ASSERT_FAIL(Obj::areEqual("NaN", "0"));
            ASSERT_FAIL(Obj::areEqual("0", "NaN"));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING: asDecimal64
        //   This is a white box test. 'asDecimal64' delegates to
        //   'DecimalUtil' which we assume to be correct.  This
        //   test primarily looks for edge cases in the JSON number
        //   specification that might result in strtod reporting an error.
        //
        //Concerns:
        //: 1 That 'asDecimal64[Exact]' returns the closest decimal floating
        //:   point representation of the input text.
        //:
        //: 2 That 'asDecimal64[Exact]' behaves correctly for numbers with
        //:   extremely large or small exponents
        //:
        //: 3 That 'asDecimal64[Exact]' returns +INF and -INF if the input is
        //    outside of the range that can be represented by a 'double'.
        //
        //: 4 That 'asDecimal64Exact' returns a 0 status if the number has 16
        //    or fewer significant digits, and its exponent is between
        //    [-398, 369] (inclusive).
        //
        //: 5 That 'asDecimal64Exact' returns a status 'k_NOT_EXACT' if the
        //    input is above or below the representable range of values, and
        //    returns a value of +INF or -INFO respective.
        //
        //: 6 That 'asDecimal64Exact' returns a status 'k_NOT_EXACT' if the
        //    input is in the representable range but has too many significant
        //    digits to represent exactly.
        //
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        //Plan:
        //: 1 For a test table that tests a range of varied input,
        //:   verify that 'asDecimal64[Exact]' returns the expected result,
        //:   and, in addition, 'asDecimal64Exact' returns the expected status.
        //:
        //:   1 For each test point, generate a set of equivalent
        //:     representations with varying exponents, and verify the same
        //:     result and status are returned.
        //:
        //: 2 For the same test table, test a couple extreme points from the
        //:   equivalence set with thousands of '0' characters.  Verify the
        //:   same results and status are returned.
        //:
        //: 3 For the JSONTestSuite number data, for those numbers that are
        //:   valid verify 'asDecimal64[Exact]' returns the same result as an
        //:   'oracle' (in this). We use 'NumberUtil::asDouble' as an oracle.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when.
        //
        // Testing:
        //   bdldfp::Decimal64 asDecimal64(const bsl::string_view& );
        //   int asDecimal64Exact(Decimal64 *, const bsl::string_view&  );
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING: asDecimal"
                      << "\n==================" << bsl::endl;
        }

        const int k_INE = Obj::k_INEXACT;
        const int k_NP  = 0;  // Not precise (treated as success).

        struct Data {
            int         d_line;
            int         d_status;
            const char *d_input;
        } DATA[] = {
            { L_,     0,                       "0" },
            { L_,     0,                      "-0" },
            { L_,     0,                     "0.0" },
            { L_,     0,                  "0.0000" },

// The following tests are disabled because of an issue in the inteldfp
// library.  See note in the implementation.
//            { L_,     0,           "0.0000e100000" },
//            { L_,     0,             "0.0e-100000" },
//            { L_,     0,            "0.00e+100000" },
//            { L_,     0,           "0.000e-100000" },
//            { L_,     0,  "0e18446744073709551615" },
//            { L_,     0, "0e-18446744073709551615" },

            { L_,     0,                       "1" },
            { L_,     0,                      "10" },
            { L_,     0,                     "105" },
            { L_,     0,                     "1e1" },
            { L_,     0,                    "-1E1" },
            { L_,     0,                  "-0.1E1" },
            { L_,     0,                 "-0.1E-1" },
            { L_,     0,                 "-0.1E+1" },
            { L_,     0,           "1E+0000000001" },
            { L_,     0,           "1E-0000000001" },
            { L_,     0,          "-1E-0000000001" },

            { L_,     0,                   "1.5e1" },
            { L_,     0,                   "1.5e1" },
            { L_,     0,                   "1.5e2" },
            { L_,     0,                   "1.0e2" },
            { L_,     0,                "150.0e-1" },
            { L_,     0,            "2.22507E-308" },
            { L_,     0,            "2.22500E-308" },
            { L_,     0,           "-2.22500E-308" },

            { L_,  k_NP,   "10000000000000000e-16" },
            { L_,  k_NP,  "1000000000000000.0e-15" },
            { L_,  k_NP,      "1.0000000000000000" },
            { L_,  k_NP, "1.7976931348623160E+308" },
            { L_,  k_NP,"-1.7976931348623160E+308" },

            { L_, k_INE,      "1.0000000000000001" },
            {L_,  k_INE,   "10000000000000001e-16" },
            { L_, k_INE,    "18446744073709551615" },
            { L_, k_INE,   "-18446744073709551615" },
            { L_, k_INE, "1.7976931348623157E+308" },
            { L_, k_INE,"-1.7976931348623157E+308" },

            { L_, k_INE, "1797693134862316E+99999" },
            { L_, k_INE,"-1797693134862316E+99999" },
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        if (verbose) {
            bsl::cout << "\tTest a wide array of valid JSON number text"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_line;
            const char *INPUT  = DATA[i].d_input;
            const int   STATUS = DATA[i].d_status;

            // This is a sanity check that our test data is correct.
            int rc = verifyDecimal64Exactness(INPUT);
            ASSERTV(LINE, INPUT, STATUS, rc, STATUS == rc);

            bdldfp::Decimal64 EXPECTED;
            rc = bdldfp::DecimalUtil::parseDecimal64(&EXPECTED, INPUT);
            ASSERTV(rc, 0 == rc);

            bdldfp::Decimal64 result;
            rc = asDecimal64ExactOracle(&result, INPUT);
            ASSERTV(LINE, INPUT, EXPECTED, result, EXPECTED == result);
            ASSERTV(LINE, INPUT, STATUS, rc, STATUS == rc);

            if (veryVerbose) {
                P_(LINE); P(INPUT);
            }

            // Test 'asDecimal64' and 'asDecimal64Exact'.

            result = Obj::asDecimal64(INPUT);
            ASSERTV(LINE, INPUT, EXPECTED, result, EXPECTED == result);

            rc = Obj::asDecimal64Exact(&result, INPUT);
            ASSERTV(LINE, INPUT, EXPECTED, result, EXPECTED == result);
            ASSERTV(LINE, INPUT, result, rc, STATUS, rc == STATUS);

            if (EXPECTED != result) {
                // Don't need hundreds of duplicate errors for the equivalence
                // set.

                continue;                                           // CONTINUE
            }

            bsl::vector<bsl::string> equivalentData;
            generateEquivalenceSet(&equivalentData, -100, 100, INPUT);
            bsl::vector<bsl::string>::const_iterator it =
                                                        equivalentData.begin();
            for (; it != equivalentData.end(); ++it) {
                const bsl::string& input = *it;
                if (veryVeryVerbose) {
                    bsl::cout << "   ";
                    P_(LINE); P(input);
                }
                bdldfp::Decimal64 result = Obj::asDecimal64(input);
                ASSERTV(LINE, input, EXPECTED, result, EXPECTED == result);

                rc = Obj::asDecimal64Exact(&result, input);
                ASSERTV(LINE, input, EXPECTED, result, EXPECTED == result);
                ASSERTV(LINE, INPUT, result, rc, STATUS, rc == STATUS);
            }
        }
        if (verbose) {
          bsl::cout << "\tTest an extreme number of digits" << bsl::endl;
        }
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_line;
            const char *INPUT  = DATA[i].d_input;
            const int   STATUS = DATA[i].d_status;

            if (veryVerbose) {
                P_(LINE); P(INPUT);
            }

            bdldfp::Decimal64 EXPECTED;
            int rc = bdldfp::DecimalUtil::parseDecimal64(&EXPECTED, INPUT);

            ASSERTV(rc, 0 == rc);

            bsl::vector<bsl::string> equivalentData;
            generateEquivalenceSet(&equivalentData, -100000, -99999, INPUT);
            generateEquivalenceSet(&equivalentData,   99999, 100000, INPUT);
            bsl::vector<bsl::string>::const_iterator it =
                equivalentData.begin();
            for (; it != equivalentData.end(); ++it) {
                const bsl::string& input = *it;
                if (veryVeryVerbose) {
                    bsl::cout << "   ";
                    P_(LINE); P_(INPUT); P(input.size());
                }

                bdldfp::Decimal64 result = Obj::asDecimal64(input);
                ASSERTV(LINE, EXPECTED, result, EXPECTED == result);
                if (result == bdldfp::Decimal64(0)) {
                    // There is a known issue with the inteldfp library
                    // incorrectly returning INEXACT for 0 with large or small
                    // exponents.

                    continue;                                       // CONTINUE
                }
                rc = Obj::asDecimal64Exact(&result, input);
                ASSERTV(LINE, EXPECTED, result, EXPECTED == result);
                ASSERTV(LINE, EXPECTED, result, rc, STATUS, rc == STATUS);

                rc = asDecimal64ExactOracle(&result, input);
                ASSERTV(LINE, EXPECTED, result, EXPECTED == result);
                ASSERTV(LINE, EXPECTED, result, rc, STATUS, rc == STATUS);
            }
        }

        if (verbose) {
            bsl::cout << "\tTest JSONSuite Test Data"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_JSON_SUITE_DATA; ++i) {
            const int   LINE  = JSON_SUITE_DATA[i].d_line;
            const bool  EXP   = JSON_SUITE_DATA[i].d_isValid;
            const char *INPUT = JSON_SUITE_DATA[i].d_input;
            const char *NAME  = JSON_SUITE_DATA[i].d_testName;

            if (!EXP) {
                // Ignore invalid JSON numbers.

                continue;                                           // CONTINUE
            }

            double  expected;
            int rc = bdlb::NumericParseUtil::parseDouble(&expected, INPUT);
            ASSERTV(LINE, INPUT, 0 == rc);

            bdldfp::Decimal64 result;
            result = Obj::asDecimal64(INPUT);

            ASSERTV(LINE, NAME, INPUT, expected, result,
                    bdldfp::Decimal64(expected) == result);
        }

        if (verbose) bsl::cout << "\tNegative Testing." << bsl::endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bdldfp::Decimal64 result;
            ASSERT_PASS(Obj::asDecimal64("0"));
            ASSERT_PASS(Obj::asDecimal64Exact(&result, "0"));
            ASSERT_FAIL(Obj::asDecimal64("NaN"));
            ASSERT_FAIL(Obj::asDecimal64Exact(&result, "NaN"));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING: asDouble, asFloat
        //   This is a white box test. 'asDouble' delegates to
        //   NumericParseUtil, which delegates to the standard library, which
        //   we assume to be correct.  This test primarily looks for edge cases
        //   in the JSON number specification that might result in strtod
        //   reporting an error.
        //
        //Concerns:
        //: 1 That 'asDouble' returns the closest binary floating point
        //:   representation of the input text.
        //:
        //: 2 That 'asDouble' behaves correctly for numbers with extremely
        //:   large or small exponents
        //:
        //: 3 That 'asDouble' returns +INF and -INF if the input is outside of
        //:   the range that can be represented by a 'double'.
        //:
        //: 4 That 'asFloat' returns the same value as 'asDouble' casted to a
        //:   'float'.
        //
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        //Plan:
        //: 1 For a test table that tests a range of varied input,
        //:   verify that 'asDouble' returns the expected value.
        //:
        //:   1 For each test point, generate a set of equivalent
        //:     representations with varying exponents, and verify the same
        //:     result is returned.
        //:
        //: 2 For the same test table, test a couple extreme points from the
        //:   equivalence set with thousands of '0' characters.  Verify the
        //:   same result is returned.
        //:
        //: 3 For the JSONTestSuite number data, for those numbers that are
        //:   valid verify 'asDouble' returns the same value as an 'oracle' (in
        //:   this).  We use 'NumberUtil::asDouble' as an oracle.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when.
        //
        // Testing:
        //   float asFloat(const bsl::string_view& );
        //   double asDouble(const bsl::string_view& );
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING: asDouble, asFloat"
                      << "\n==========================" << bsl::endl;
        }

        const double k_BIG_INT =
                       static_cast<double>(bsl::numeric_limits<Uint64>::max());
        const double k_SMALL_INT =
                      -static_cast<double>(bsl::numeric_limits<Uint64>::max());

        const double k_MAX_DBL = bsl::numeric_limits<double>::max();
        const double k_MIN_DBL = bsl::numeric_limits<double>::min();
        const double k_INF     = bsl::numeric_limits<double>::infinity();

        struct Data {
            int         d_line;
            const char *d_input;
            double      d_result;
        } DATA[] = {
            { L_,                        "0",             0 },
            { L_,                       "-0",             0 },
            { L_,                      "0.0",             0 },
            { L_,                     "-0.0",             0 },
            { L_,                   "0.0000",             0 },
            { L_,            "0.0000e100000",             0 },
            { L_,              "0.0e-100000",             0 },
            { L_,             "0.00e+100000",             0 },
            { L_,            "0.000e-100000",             0 },
            { L_,    "0.00000000e1000000000",             0 },
            { L_,   "0e18446744073709551615",             0 },
            { L_,  "0e-18446744073709551615",             0 },
            { L_,                        "1",             1 },
            { L_,                       "10",            10 },
            { L_,                      "105",           105 },
            { L_,                      "1e1",            10 },
            { L_,                     "-1E1",           -10 },
            { L_,            "1E+0000000001",            10 },
            { L_,            "1E-0000000001",            .1 },
            { L_,           "-1E-0000000001",           -.1 },
            { L_,                    "1.5e1",            15 },
            { L_,                    "1.5e1",            15 },
            { L_,                    "1.5e2",           150 },
            { L_,                    "1.0e2",           100 },
            { L_,                 "150.0e-1",            15 },
            { L_,     "18446744073709551615",     k_BIG_INT },
            { L_,    "-18446744073709551615",   k_SMALL_INT },
            { L_,  "1.7976931348623157E+308",     k_MAX_DBL },
            { L_, "-1.7976931348623157E+308",    -k_MAX_DBL },
            { L_,             "2.22507E-308",     k_MIN_DBL },

            { L_, "1e-184467440737095516160",             0 },
            { L_,  "1e184467440737095516160",         k_INF },

            { L_,  "1.7976931348623160E+308",         k_INF },
            { L_, "-1.7976931348623160E+308",        -k_INF },
            { L_,  "1797693134862316E+99999",         k_INF },
            { L_, "-1797693134862316E+99999",        -k_INF },
            { L_,             "2.22500E-308",             0 },
            { L_,            "-2.22500E-308",             0 }

        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        if (verbose) {
            bsl::cout << "\tTest a wide array of valid JSON number text"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_DATA; ++i) {
            const int              LINE     = DATA[i].d_line;
            const char            *INPUT    = DATA[i].d_input;
            const volatile double  EXPECTED = DATA[i].d_result;

            if (veryVerbose) {
                P_(LINE); P(INPUT);
            }

            volatile double result = Obj::asDouble(INPUT);

            ASSERTV(LINE, INPUT, EXPECTED, result,
                    fuzzyCompare(EXPECTED, result));

            if (!fuzzyCompare(EXPECTED, result)) {
                // Don't need hundreds of duplicate errors for the equivalence
                // set.

                continue;                                           // CONTINUE
            }

            bsl::vector<bsl::string> equivalentData;
            generateEquivalenceSet(&equivalentData, -100, 100, INPUT);
            bsl::vector<bsl::string>::const_iterator it =
                                                        equivalentData.begin();
            for (; it != equivalentData.end(); ++it) {
                const bsl::string& input = *it;
                if (veryVeryVerbose) {
                    bsl::cout << "   ";
                    P_(LINE); P(input);
                    }
                    volatile double result = Obj::asDouble(input);

                    ASSERTV(LINE, input, EXPECTED, result,
                            fuzzyCompare(EXPECTED, result));

                  volatile float fR = Obj::asFloat(input);
                  ASSERTV(LINE, input, fR == static_cast<float>(result));
                }
        }

        if (verbose) {
          bsl::cout << "\tTest an extreme number of digits" << bsl::endl;
        }
        for (int i = 0; i < NUM_DATA; ++i) {
            const int              LINE     = DATA[i].d_line;
            const char            *INPUT    = DATA[i].d_input;
            const volatile double  EXPECTED = DATA[i].d_result;

            if (veryVerbose) {
                P_(LINE);
                P(INPUT);
            }

            bsl::vector<bsl::string> equivalentData;

            // Both MSVC and MacOS have limits to the number of digits that can
            // be handled by the standard library 'strtod' or 'from_chars' used
            // by 'parseDouble', thus limiting the size of the possible input
            // data to only a few thousand characters.  The limit for the
            // for 'strtod' on MacOS is 19,999 digits as of MacOS Monterey 12.6
            // and Apple clang version 14.0.0.  'from_chars' from MSVC limits
            // the accepted exponent value range (instead of string length).
            generateEquivalenceSet(&equivalentData, -2000, -1999, INPUT);
            generateEquivalenceSet(&equivalentData,  1999,  2000, INPUT);

            bsl::vector<bsl::string>::const_iterator it =
                                                        equivalentData.begin();
            for (; it != equivalentData.end(); ++it) {
                const bsl::string& input = *it;
                if (veryVeryVerbose) {
                    bsl::cout << "   ";
                    P_(LINE); P_(INPUT); P(input.size());
                }
                volatile double result = Obj::asDouble(input);

                ASSERTV(LINE, EXPECTED, result,
                        fuzzyCompare(EXPECTED, result));
            }
        }
        if (verbose) {
            bsl::cout << "\tTest JSONSuite Test Data"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_JSON_SUITE_DATA; ++i) {
            const int   LINE  = JSON_SUITE_DATA[i].d_line;
            const bool  EXP   = JSON_SUITE_DATA[i].d_isValid;
            const char *INPUT = JSON_SUITE_DATA[i].d_input;
            const char *NAME  = JSON_SUITE_DATA[i].d_testName;

            if (!EXP) {  // Ignore invalid JSON numbers.
                continue;                                           // CONTINUE
            }

            double result, expected;
            int rc = bdlb::NumericParseUtil::parseDouble(&expected, INPUT);

            ASSERTV(LINE, INPUT, 0 == rc);
            result = Obj::asDouble(INPUT);

            ASSERTV(LINE, NAME, INPUT, expected, result, expected == result);
        }

        if (verbose) bsl::cout << "\tNegative Testing." << bsl::endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_PASS(Obj::asDouble("0"));
            ASSERT_FAIL(Obj::asDouble("NaN"));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING: asUint64
        //
        // Concerns:
        //: 1 For basic integral text input in the [0, k_MAX] return that
        //:   number and a status of 0.
        //:
        //: 2 For integrals in the range [0, UINT64_MAX] that any alternative
        //:   equivalent JSON Number rendering for the number (e.g., for "1",
        //:  "1e-1", "0.1e1" etc) will correctly return that integer and a
        //:   status of 0.
        //:
        //: 3 That for input in the range [0, k_MAX] that has a fraction,
        //:   that the truncated integer value is returned with a status of
        //:   'k_NOT_INTEGER'.
        //:
        //: 4 That for negative input the value 0 is returned with a status of
        //:   'k_UNDERFLOW'.
        //:
        //: 5 That for positive input above k_MAX, that k_MAX is
        //:   returned with a status 'k_OVERFLOW'.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        //Plan:
        //: 1 For a test table that tests a range of varied input,
        //:   verify that 'asUint64' returns the expected value and expected
        //:   status.
        //:
        //:   1 For each test point, generate a set of equivalent
        //:     representations with varying exponents, and verify the same
        //:     result and status are returned.
        //:
        //: 2 For the same test table, test a couple extreme points from the
        //:   equivalence set with thousands of '0' characters.  Verify the
        //:   same results and status are returned.
        //:
        //: 3 For the JSONTestSuite number data, for those numbers that are
        //:   valid verify 'asUint64' returns the same value as an 'oracle'
        //:   (in this).  We use NumberUtil::parseDouble as an oracle.
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when.
        //
        // Testing:
        //   int asUint64(Uint64 *, const bsl::string_view& );
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING: asUint64"
                      << "\n=================" << bsl::endl;
        }

        const Uint64 k_MAX = bsl::numeric_limits<Uint64>::max();

        struct Data {
            int         d_line;
            int         d_status;
            const char *d_input;
            Uint64      d_result;
        } DATA[] = {
            { L_,      OK,                        "0",         0 },
            { L_,      OK,                       "-0",         0 },
            { L_,      OK,                      "0.0",         0 },
            { L_,      OK,                   "0.0000",         0 },
            { L_,      OK,            "0.0000e100000",         0 },
            { L_,      OK,              "0.0e-100000",         0 },
            { L_,      OK,             "0.00e+100000",         0 },
            { L_,      OK,            "0.000e-100000",         0 },
            { L_,      OK,    "0.00000000e1000000000",         0 },
            { L_,      OK,   "0e18446744073709551615",         0 },
            { L_,      OK,  "0e-18446744073709551615",         0 },

            { L_,      OK,                        "1",         1 },
            { L_,      OK,                     "1e-0",         1 },
            { L_,      OK,                     "1e+0",         1 },
            { L_,      OK,                       "10",        10 },
            { L_,      OK,                      "105",       105 },
            { L_,      OK,                      "1e1",        10 },
            { L_,      OK,                    "1.5e1",        15 },
            { L_,      OK,                    "1.5e1",        15 },
            { L_,      OK,                    "1.5e2",       150 },
            { L_,      OK,                    "1.0e2",       100 },
            { L_,      OK,                 "150.0e-1",        15 },
            { L_,      OK,            "1E+0000000001",        10 },
            { L_,      OK,     "18446744073709551614", k_MAX - 1 },
            { L_,      OK,     "18446744073709551615",     k_MAX },

            { L_,  EUNDER,                        "-1",        0 },
            { L_, ENOTINT,                       "0.5",        0 },
            { L_, ENOTINT,                     "1e-10",        0 },
            { L_, ENOTINT,                    "1.5e-0",        1 },
            { L_, ENOTINT,                    "1.5e+0",        1 },
            { L_, ENOTINT,                  "1e-99999",        0 },
            { L_, ENOTINT,  "1e-184467440737095516160",        0 },
            { L_,   EOVER,   "1e184467440737095516160",    k_MAX },
            { L_,  EUNDER,                     "-1e10",        0 },
            { L_, ENOTINT,             "1E-0000000001",        0 },
            { L_,  EUNDER,            "-1E-0000000001",        0 },

            { L_, ENOTINT,                     "1.5e0",        1 },
            { L_, ENOTINT,                 "1000.5e-1",      100 },
            { L_,   EOVER,      "18446744073709551616",    k_MAX },
            { L_,   EOVER,    "18446744073709551616.5",    k_MAX },
            { L_,   EOVER,    "18446744073709551615.5",    k_MAX },
            { L_, ENOTINT,    "18446744073709551614.5", k_MAX -1 },
            { L_,   EOVER, "0.18446744073709551616e20",    k_MAX },
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        if (verbose) {
            bsl::cout << "\tTest a wide array of valid JSON number text"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE     = DATA[i].d_line;
            const int     STATUS   = DATA[i].d_status;
            const char   *INPUT    = DATA[i].d_input;
            const Uint64  EXPECTED = DATA[i].d_result;

            if (veryVerbose) {
                P_(LINE); P(INPUT);
            }

            Uint64 result;
            int rc = Obj::asUint64(&result, INPUT);

            ASSERTV(LINE, INPUT, STATUS, rc, STATUS == rc);
            ASSERTV(LINE, INPUT, EXPECTED, result, EXPECTED == result);

            if (STATUS != rc || EXPECTED != result) {
                // We already have an error with this number, no need to
                // generate 200 more with an equivalence set.

                continue;                                           // CONTINUE
            }

            // Generate equivalent numeric values but with a wide range of
            // exponents (e.g, if we have the value 1e1, generate the strings
            // "1000e-2" through ".01e3").

            bsl::vector<bsl::string> equivalentData;
            generateEquivalenceSet(&equivalentData, -100, 100, INPUT);
            bsl::vector<bsl::string>::const_iterator it =
                                                        equivalentData.begin();
            for (; it != equivalentData.end(); ++it) {
              const bsl::string& input = *it;
              if (veryVeryVerbose) {
                  bsl::cout << "   ";
                  P_(LINE); P(input);
              }
              Uint64 result;
              int rc = Obj::asUint64(&result, input);
              ASSERTV(LINE, input, STATUS, rc, STATUS == rc);
              ASSERTV(LINE, input, EXPECTED, result, EXPECTED == result);
            }
        }

        if (verbose) {
          bsl::cout << "\tTest an extreme number of digits" << bsl::endl;
        }
        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE     = DATA[i].d_line;
            const int     STATUS   = DATA[i].d_status;
            const char   *INPUT    = DATA[i].d_input;
            const Uint64  EXPECTED = DATA[i].d_result;

            if (veryVerbose) {
                P_(LINE); P_(STATUS); P(INPUT);
            }

            bsl::vector<bsl::string> equivalentData;
            generateEquivalenceSet(&equivalentData, -100000, -99999, INPUT);
            generateEquivalenceSet(&equivalentData,   99999, 100000, INPUT);
            bsl::vector<bsl::string>::const_iterator it =
                                                        equivalentData.begin();
            for (; it != equivalentData.end(); ++it) {
                const bsl::string& input = *it;
                if (veryVeryVerbose) {
                    bsl::cout << "   ";
                    P_(LINE);
                    P_(INPUT);
                    P(input.size());
                }
                Uint64 result;
                int    rc = Obj::asUint64(&result, input);
                ASSERTV(LINE, STATUS, rc, STATUS == rc);
                ASSERTV(LINE, EXPECTED, result, EXPECTED == result);
            }
        }

        if (verbose) {
            bsl::cout << "\tTest JSONSuite Test Data"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_JSON_SUITE_DATA; ++i) {
            const int   LINE  = JSON_SUITE_DATA[i].d_line;
            const bool  EXP   = JSON_SUITE_DATA[i].d_isValid;
            const char *INPUT = JSON_SUITE_DATA[i].d_input;
            const char *NAME  = JSON_SUITE_DATA[i].d_testName;

            if (!EXP) {
                // Ignore invalid JSON numbers.

                continue;
            }

            // The following compares the result of a conversion to double
            // using 'NumberParseUtil', with 'asUint64' for the JSONTestSuite
            // data related to numbers.  This is a fuzzy test just to verify
            // sane results with a separate set of test data.  We allow success
            // if the comparison is successful, either when converting both
            // numbers to an integer, or converting both numbers to a double.

            Uint64 result;
            double expected;

            int rc = bdlb::NumericParseUtil::parseDouble(&expected, INPUT);

            ASSERTV(LINE, INPUT, 0 == rc);

            rc = Obj::asUint64(&result, INPUT);

            if (1 > expected) {
                expected = 0;
            }
            if (static_cast<double>(k_MAX) < expected) {
                expected = static_cast<double>(k_MAX);
            }
            ASSERTV(LINE, NAME, INPUT, expected, result,
                    static_cast<Uint64>(expected) == result ||
                    expected == static_cast<double>(result));
        }

        if (verbose) bsl::cout << "\tNegative Testing." << bsl::endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Uint64 result;

            ASSERT_PASS(Obj::asUint64(&result, "0"));
            ASSERT_FAIL(Obj::asUint64(&result, "NaN"));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING: decompose
        //
        // Concerns:
        //: 1 'isNegative' correctly returns whether the text begins with '-'.
        //:
        //: 2 'integerBegin' and 'integerEnd' correctly returns the integer
        //:    part of the representation
        //:
        //: 3 'fractionBegin' and 'fractionEnd' correctly return the fraction
        //:    part of the representation.
        //:
        //: 4 If there is no fraction part, 'fractionBegin' and 'fractionEnd'
        //:   are both equal to 'exponentBegin'.
        //:
        //: 4 That generateEquivalenceSet loads an array for the range of
        //:   adjustments.
        //:
        //: 5 'isExponentNegative' correctly returns whether the exponent
        //:    begins with '-'.
        //:
        //: 6 'exponentBegin' correctly returns the exponent part of the
        //:    representation.
        //:
        //: 7 If there is no exponent part of the representation,
        //:   'exponentBegin' returns the end iterator.
        //:
        //: 8 If there is no exponent part of the representation,
        //:   'fractionBegin' and 'fractionEnd' equal 'exponentBegin'.
        //
        //: 9 'significantDigitsBegin' and 'significantDigitsEnd' returns the
        //:   range of significant digits (which may or may not include a '.'
        //:   separator)
        //:
        //: 10 If this is a 0 representation, the significantDigits range is a
        //:   single '0' character.
        //:
        //: 11 Any leading and trailing 0s are removed from the
        //:    significantDigits range.
        //:
        //: 12 'significantDigitsBias' provides a value, that when treated as
        //:    an exponent of the digits in the signigicantDigits range
        //:    (ignoring the '.' character), the original numeric value is
        //:    restored.
        //:
        //: 13 QoI: Asserted precondition violations are detected when enabled.
        //
        //Plan:
        //: 1 For a test table that tests a range of varied input,
        //:   verify that decompose returns values that match the expected
        //:   results.
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when.
        //
        // Testing:
        //   void decompose(...);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING: decompose"
                      << "\n==================" << bsl::endl;
        }
        const bool F = false;
        const bool T = true;

        struct Data {
            int         d_line;
            const char *d_text;     // input text
            bool        d_isNeg;    // is negative
            const char *d_intPart;  // integer digits
            const char *d_fracPart; // fraction digits
            bool        d_isExpNeg; // is exponent negative
            const char *d_expPart;  // exponent digits
            const char *d_sigPart;  // significant digits
            Int64       d_sigBias;  // significant digits bias to exponent
        } DATA[] = {
          // line,       text, neg,   int, frac,eN, exp, sigDig, bias
          // ----        ----  ---    ---  ---- --  ---  ------  ----
            { L_,         "0",   F,   "0",   "", F,  "",      "0",  0 },
            { L_,        "-0",   T,   "0",   "", F,  "",      "0",  0 },
            { L_,         "1",   F,   "1",   "", F,  "",      "1",  0 },
            { L_,        "-1",   T,   "1",   "", F,  "",      "1",  0 },
            { L_,        "10",   F,  "10",   "", F,  "",      "1",  1 },
            { L_,       "1e1",   F,   "1",   "", F, "1",      "1",  0 },
            { L_,       "0.1",   F,   "0",  "1", F,  "",      "1", -1 },
            { L_,     "0.1e1",   F,   "0",  "1", F, "1",      "1", -1 },
            { L_,      "1e-1",   F,   "1",   "", T, "1",      "1",  0 },
            { L_,     "1.1e1",   F,   "1",  "1", F, "1",    "1.1", -1 },
            { L_,    "1.1e-1",   F,   "1",  "1", T, "1",    "1.1", -1 },
            { L_,   "-1.1e-1",   T,   "1",  "1", T, "1",    "1.1", -1 },
            { L_,  "-0.01e-1",   T,   "0", "01", T, "1",      "1", -2 },
            { L_,  "-0.01e+1",   T,   "0", "01", F, "1",      "1", -2 },
            { L_,   "0.01e+0",   F,   "0", "01", F, "0",      "1", -2 },
            { L_, "31.415e-1",   F,  "31","415", T, "1", "31.415", -3 },
            { L_,       "100",   F, "100",   "", F,  "",      "1",  2 },
            { L_,      "0.00",   F,   "0", "00", F,  "",      "0",  0 },
            { L_,     "-0.00",   T,   "0", "00", F,  "",      "0",  0 },

            // From documentation
            { L_,      "0.00",   F,   "0", "00", F,  "",      "0",  0 },
            { L_,    "100e+1",   F, "100",   "", F, "1",      "1",  2 },
            { L_,     "0.020",   F,   "0","020", F,  "",      "2", -2 },
            { L_,    "1.12e5",   F,   "1", "12", F, "5",   "1.12", -2 },
            { L_,     "34.50",   F,  "34", "50", F,  "",   "34.5", -1 },
            { L_,     "0.060",   F,   "0","060", F,  "",      "6", -2 },
            { L_,     "10e-2",   F,  "10",   "", T, "2",      "1",  1 },

        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        if (verbose) bsl::cout << "\tTest a wide array of input." << bsl::endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE       = DATA[i].d_line;
            const char  *TEXT       = DATA[i].d_text;
            const bool   IS_NEG     = DATA[i].d_isNeg;
            const char  *INT_PART   = DATA[i].d_intPart;
            const char  *FRAC_PART  = DATA[i].d_fracPart;
            const bool   IS_EXP_NEG = DATA[i].d_isExpNeg;
            const char  *EXP_PART   = DATA[i].d_expPart;
            const char  *SIG_PART   = DATA[i].d_sigPart;
            const Int64  SIG_BIAS   = DATA[i].d_sigBias;

            if (veryVerbose) {
                P_(LINE); P(TEXT);
            }

            bool                        isNeg, isExpNeg;
            bsl::string_view            intPart, fracPart, expPart, sigPart;
            bsl::string_view::size_type significantDotOffset;
            Int64                       sigBias;

            ImpUtil::decompose(&isNeg,
                               &isExpNeg,
                               &intPart,
                               &fracPart,
                               &expPart,
                               &sigPart,
                               &sigBias,
                               &significantDotOffset,
                               TEXT);

            ASSERTV(LINE, TEXT, isNeg, IS_NEG == isNeg);
            ASSERTV(LINE, TEXT, intPart, INT_PART == intPart);
            ASSERTV(LINE, TEXT, fracPart, FRAC_PART == fracPart);
            ASSERTV(LINE, TEXT, isExpNeg, IS_EXP_NEG == isExpNeg);
            ASSERTV(LINE, TEXT, expPart, EXP_PART == expPart);
            ASSERTV(LINE, TEXT, sigPart, SIG_PART == sigPart);
            ASSERTV(LINE, TEXT, sigBias, SIG_BIAS == sigBias);

            ASSERTV(LINE, TEXT, significantDotOffset, sigPart,
                    significantDotOffset == sigPart.find('.'));

        }

        if (verbose)
            bsl::cout << "\tTest problematic empty fraction." << bsl::endl;
        {
            // Verify that 'fractionBegin' and 'fractionEnd' are placed before
            // 'exponentBegin' if there is no fraction.

            bool                        isNeg, isExpNeg;
            bsl::string_view            intPart, fracPart, expPart, sigPart;
            bsl::string_view::size_type significantDotOffset;
            Int64                       sigBias;

            ImpUtil::decompose(&isNeg,
                               &isExpNeg,
                               &intPart,
                               &fracPart,
                               &expPart,
                               &sigPart,
                               &sigBias,
                               &significantDotOffset,
                               "1e2");

            ASSERT("2" == expPart);
            ASSERT(fracPart.empty());
        }
        if (verbose) bsl::cout << "\tNegative Testing." << bsl::endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bool                        isNeg, isExpNeg;
            bsl::string_view            intPart, fracPart, expPart, sigPart;
            bsl::string_view::size_type significantDotOffset;
            Int64                       sigBias;

            ASSERT_SAFE_PASS(ImpUtil::decompose(&isNeg,
                                                &isExpNeg,
                                                &intPart,
                                                &fracPart,
                                                &expPart,
                                                &sigPart,
                                                &sigBias,
                                                &significantDotOffset,
                                                "0"));

            ASSERT_SAFE_FAIL(ImpUtil::decompose(&isNeg,
                                                &isExpNeg,
                                                &intPart,
                                                &fracPart,
                                                &expPart,
                                                &sigPart,
                                                &sigBias,
                                                &significantDotOffset,
                                                "0-"));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: isValidNumber
        //
        // Concerns:
        //: 1 'isValidNumber' rejects text with preceding or trailing white
        //:   space.
        //:
        //: 2 'isValidNumber' rejects text starting with '+'
        //:
        //: 3 'isValidNumber' rejects text with non-numeric characters except
        //:   "+-eE."
        //:
        //: 4 'isValidNumber' rejects text starting with '.'
        //:
        //: 5 'isValidNumber' rejects text with multiple consecutive "+-eE."
        //:    characters
        //:
        //: 6 'isValidNumber' rejects numbers with multiple (non-consecutive)
        //:   '.' characters
        //:
        //: 7 'isValidNumber' rejects numbers with embedded 0 characters
        //:
        //: 8 'isValidNumber' accepts numbers that match the regular
        //:   expression:
        //:   "/^-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][-+]?[0-9]+)?\z/"
        //:   Note that "\z" matches end-of-string but not a preceding '\n'.
        //:
        //: 9 'isValidNumber' accepts characters with a huge number of integer
        //:    digits.
        //:
        //: 10 'isValidNumber' accepts characters with a huge number of
        //:    fraction digits.
        //:
        //: 12 'isValidNumber' accepts characters with a huge number of
        //:    exponent digits.
        //:
        //: 13 'isValidNumber' does not reference data past the end of the
        //:    given input.
        //:
        //: 14 The 'IsValidNumber' functor produces the same result as the
        //:    'isValidNumber' function.
        //
        // Plan:
        //: 1 For a test table that tests a range of varied input,
        //:   call 'isValidNumber', and verify it against an
        //:   expected result.
        //:
        //: 2 For a test table derived from JSONTestSuite, call 'isValidNumber'
        //:   ad verify it against an expected result.
        //:
        //: 3 Test strings with embedded 0s return 'false'.
        //:
        //: 4 For each test of 'isValidNumber' provide input in memory acquired
        //:   from 'bslma::GuardingAllocator' configured so that any reference
        //:   past the end of the valid input triggers a segmentation fault.
        //:
        //: 5 Call the 'IsValidNumber' functor with each data point used in
        //:   P-1..3 and confirm that the result matches that from
        //:   'isValidNumber'.
        //
        // Testing:
        //   bool isValidNumber(const bsl::string_view& );
        //   CONCERN: 'IsValidNumber' functor can be used as an oracle.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING: isValidNumber"
                      << "\n======================" << bsl::endl;
        }

        const char *HUGE_INT =
            "11111111111111111111111111111111111111111111111111111111111111111"
            "1111111111111111111111111111111111111.0";

        const char *HUGE_FRAC =
            "0."
            "11111111111111111111111111111111111111111111111111111111111111111"
            "1111111111111111111111111111111111111";

        const char *HUGE_EXP = "1e11111111111111111111111111111111111111111111"
                               "111111111111111111111"
                               "1111111111111111111111111111111111111";

        const IsValidNumber oracle;

        typedef bdlma::GuardingAllocator GA;
        GA ga(GA::e_AFTER_USER_BLOCK);

        if (verbose)
            bsl::cout << "\tTest with hand created test data"
                      << bsl::endl;
        {
            struct {
                const int   d_line;
                const char *d_string;
                const bool  d_isValid;
            } DATA[] = {
                // line                                       string isValid
                {  L_,                                          "0",    true }
              , {  L_,                                         "-0",    true }
              , {  L_,                                         "10",    true }
              , {  L_,                                        "-10",    true }
              , {  L_,                                 "1234567890",    true }
              , {  L_,                                "-1234567890",    true }
              , {  L_,                                        "0e0",    true }
              , {  L_,                                        "1e0",    true }
              , {  L_,                                        "1e1",    true }
              , {  L_,                                      "1.2e1",    true }
              , {  L_,                                       "1e-1",    true }
              , {  L_,                                     "1.2e-1",    true }
              , {  L_,                                     "1.2e+1",    true }
              , {  L_,                                 "1.2e+00001",    true }
              , {  L_,                                 "1.2e-00001",    true }
              , {  L_,                                  "1.2e00001",    true }
              , {  L_,                                        "0.0",    true }
              , {  L_,                                     HUGE_INT,    true }
              , {  L_,                                    HUGE_FRAC,    true }
              , {  L_,                                     HUGE_EXP,    true }
              , {  L_,                                         "1e",   false }
              , {  L_,                                       "1.2e",   false }
              , {  L_,                                         "0-",   false }
              , {  L_,                                         "0-",   false }
              , {  L_,                                         "00",   false }
              , {  L_,                                         "01",   false }
              , {  L_,                                         "0.",   false }
              , {  L_,                                         ".0",   false }
              , {  L_,                                    "0.0e++1",   false }
              , {  L_,                                    "0.0e+-1",   false }
              , {  L_,                                    "0.0e--1",   false }
              , {  L_,                                     "0.0ee1",   false }
              , {  L_,                                   "++0.0ee1",   false }
              , {  L_,                                       "0..0",   false }
              , {  L_,                                         "+1",   false }
              , {  L_,                                         " 1",   false }
              , {  L_,                                         "1 ",   false }
              , {  L_,                                         "0-",   false }
              , {  L_,                                    "1a0.0e1",   false }
              , {  L_,                                    "100.ae1",   false }
              , {  L_,                                   "100.0e1a",   false }

              , {  L_,                                        "1\n",   false }
              , {  L_,                                        "1\t",   false }
              , {  L_,                                        "1\r",   false }
              , {  L_,                                        "1 " ,   false }
              , {  L_,                                       "\n1" ,   false }
              , {  L_,                                       "\t1" ,   false }
              , {  L_,                                       "\r1" ,   false }
              , {  L_,                                        " 1" ,   false }
              , {  L_,                                      "1\n1" ,   false }
              , {  L_,                                      "1\t1" ,   false }
              , {  L_,                                      "1\r1" ,   false }
              , {  L_,                                       "1 1" ,   false }
              , {  L_,                                     "10022" ,   true }
              , {  L_,                                     "08869" ,   false }
            };

            for (int i = 0; i < int(sizeof(DATA) / sizeof(DATA[0])); ++i) {
                const int           LINE       = DATA[i].d_line;
                const char         *STRING     = DATA[i].d_string;
                const bool          IS_VALID   = DATA[i].d_isValid;

                if (veryVerbose) {
                    P_(LINE) P_(STRING) P(IS_VALID);
                }

                const bsl::size_t  STRLEN     = bsl::strlen(STRING);
                const bsl::size_t  paddedSize =
                        bsls::AlignmentUtil::roundUpToMaximalAlignment(STRLEN);
                char              *block      = static_cast<char *>(
                                                      ga.allocate(paddedSize));
                char *firstProtectedAddress   = block + paddedSize;
                char *data                    = firstProtectedAddress - STRLEN;

                bsl::memcpy(data, STRING, STRLEN);

                bsl::string_view input(data, STRLEN);

                bool rc = Obj::isValidNumber(input);
                ASSERTV(LINE, STRING, rc, IS_VALID, IS_VALID == rc);

                bool rcOr = oracle(input);
                ASSERTV(LINE, STRING, rcOr, IS_VALID, IS_VALID == rcOr);

                ga.deallocate(block);
            }
        }

        if (verbose)
            bsl::cout << "\tTest JSONSuite Test Data"
                      << bsl::endl;
        for (int i = 0; i < NUM_JSON_SUITE_DATA; ++i) {
            const int   LINE  = JSON_SUITE_DATA[i].d_line;
            const bool  EXP   = JSON_SUITE_DATA[i].d_isValid;
            const char *INPUT = JSON_SUITE_DATA[i].d_input;
            const char *NAME  = JSON_SUITE_DATA[i].d_testName;

            if (veryVerbose) {
                P_(LINE) P_(EXP) P_(INPUT) P(NAME);
            }

            const bsl::size_t  LENGTH     = bsl::strlen(INPUT);
            const bsl::size_t  paddedSize =
                        bsls::AlignmentUtil::roundUpToMaximalAlignment(LENGTH);
            char              *block      = static_cast<char *>(
                                                      ga.allocate(paddedSize));
            char *firstProtectedAddress   = block + paddedSize;
            char *data                    = firstProtectedAddress - LENGTH;

            bsl::memcpy(data, INPUT, LENGTH);

            bsl::string_view input(data, LENGTH);

            bool rc = Obj::isValidNumber(input);
            ASSERTV(LINE, NAME, INPUT, EXP, rc,  EXP == rc);

            bool rcOr = oracle(input);
            ASSERTV(LINE, NAME, INPUT, EXP, rcOr,  EXP == rcOr);

            ga.deallocate(block);
        }

        if (verbose)
            bsl::cout << "\tTest embedded null"
                      << bsl::endl;
        {
            // Recreates n_multidigit_number_then_00.json  from JSONTestSuite
            const bsl::string_view input("123\0", 4);
            bool rc = Obj::isValidNumber(input);
            ASSERTV("embedded null char", false == rc);

            bool rcOr = oracle(input);
            ASSERTV("embedded null char", false == rcOr);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONCERN: Test Machinery
        //
        // Concerns:
        //: 1 That generateEquivalenceSet can correctly adjust the exponent
        //:   in a "positive" direction (moving the decimal point right, and
        //:   adding to the exponent).
        //:
        //: 2 That generateEquivalenceSet can correctly adjust the exponent
        //:   in a "negative" direction (moving the decimal point left, and
        //:   subtracting from the exponent).
        //:
        //: 3 That generateEquivalenceSet supplied a 0 adjustment does not
        //:   change the exponent
        //:
        //: 4 That generateEquivalenceSet loads an array for the range of
        //:   adjustments.
        //
        // Plan:
        //: 1 For a test table that tests a range of varied adjustments,
        //:   create a single adjusted value, and verify it against an
        //:   expected result.
        //:
        //: 2 Test a range of values, verify a result is returned for each
        //:   value in the range.
        //
        // Testing:
        //   CONCERN: Test Machinery
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nCONCERN: Test Machinery"
                      << "\n=======================" << bsl::endl;
        }

        if (verbose) {
            bsl::cout << "\tgenerateEquivalenceSet single values\n";
        }

        struct Data {
            int         d_line;
            const char *d_input;
            int         d_adjustment;
            const char *d_expected;
        } DATA[]           = {
           { L_,        "0",  0,       "0e0" },
           { L_,      "0.0",  0,       "0e0" },
           { L_,        "0",  2,    "0.00e2" },
           { L_,        "0", -2,      "0e-2" },
           { L_,       "-0",  2,   "-0.00e2" },
           { L_,       "-0", -2,     "-0e-2" },
           { L_,        "1",  0,       "1e0" },
           { L_,      "1.0",  0,     "1.0e0" },
           { L_,        "1",  2,    "0.01e2" },
           { L_,        "1", -2,    "100e-2" },
           { L_,      "1e0", -2,    "100e-2" },
           { L_,     "-1e0", -2,   "-100e-2" },
           { L_,    "-1e+0", -2,   "-100e-2" },
           { L_,    "-1e-0", -2,   "-100e-2" },
           { L_,   "-1e-01", -2,   "-100e-3" },
           { L_,   "-1e-01",  2,   "-0.01e1" },
           { L_, "-1.0e-01", -2,   "-100e-3" },
           { L_, "-1.0e-01",  2,  "-0.010e1" },
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE  = DATA[i].d_line;
            const char *INPUT = DATA[i].d_input;
            const int   ADJ   = DATA[i].d_adjustment;
            const char *EXP   = DATA[i].d_expected;

            if (veryVerbose) {
                P_(LINE); P_(INPUT); P_(ADJ); P(EXP);
            }

            bsl::vector<bsl::string> rVec;
            generateEquivalenceSet(&rVec, ADJ, ADJ, INPUT);
            ASSERTV(LINE, INPUT, ADJ, rVec.size(), 1 == rVec.size());

            const bsl::string& RESULT = rVec[0];
            ASSERTV(LINE, INPUT, ADJ, EXP, RESULT, EXP == RESULT);
        }

        if (verbose) {
            bsl::cout << "\tgenerateEquivalenceSet range\n";
        }
        {
            const char *INPUT = "10.0e1";
            const char *EXP[] = {
                "1000e-1",
                "100e0",
                "10.0e1",
                "1.00e2",
                "0.100e3"
            };

            bsl::vector<bsl::string> rVec;
            generateEquivalenceSet(&rVec, -2, 2, INPUT);

            ASSERTV(INPUT, rVec.size(), 5 == rVec.size());
            for (unsigned int i = 0; i < rVec.size(); ++i) {
                if (veryVerbose) {
                    P_(EXP[i]); P(rVec[i]);
                }
                ASSERTV(i, EXP[i], rVec[i], rVec[i] == EXP[i]);
            }
        }
      } break;
      case 1: {
        //---------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases
        //
        // Plan:
        //: 1 For the various supported numerical strings, use 'getValue' to
        //:   convert them to the various compatible numerical types, then
        //:   'stringify' the values, checking that the results are as
        //:   expected.  Note that round-trip correctness is NOT necessarily
        //:   checked.
        //
        // Testing:
        //   BREATHING TEST
        //---------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
                      << "BREATHING TEST" << bsl::endl
                      << "==============" << bsl::endl;
        }
        enum SupportedTypes {
            e_CHAR                 = 0x1,
            e_SIGNED_CHAR          = 0x2,
            e_UNSIGNED_CHAR        = 0x4,
            e_SHORT                = 0x8,
            e_SIGNED_SHORT         = 0x10,
            e_UNSIGNED_SHORT       = 0x20,
            e_INT                  = 0x40,
            e_SIGNED_INT           = 0x80,
            e_UNSIGNED_INT         = 0x100,
            e_INT64                = 0x200,
            e_UNSIGNED_INT64       = 0x400,
            e_ALL_INT_TYPES        = 0x7FF,
            e_ALL_SIGNED_INT_TYPES = 0x7FF ^ 0x4 ^ 0x20 ^ 0x100 ^ 0x400,
            e_DOUBLE               = 0x800,
            e_DECIMAL64            = 0x1000,
            e_ALL_FP_TYPES         = 0x2000,
            e_ALL_TYPES            = 0x3800,
            e_ALL_SIGNED_TYPES     = 0x3FFF ^ 0x4 ^ 0x20 ^ 0x100 ^ 0x400
        };

        if (verbose)
            bsl::cout << "\ttest for conversion-stringify round trip"
                      << bsl::endl;
        {
            struct {
                const int     d_line;
                const char   *d_string;
                const char   *d_expectedString;
                unsigned int  d_typesFlags;
            } DATA[] = {
              //  line  string      expect             types
                // simple integers
                { L_,           "0",             "0", e_ALL_TYPES }
              , { L_,           "1",             "1", e_ALL_TYPES }
              , { L_,           "2",             "2", e_ALL_TYPES }
              , { L_,           "3",             "3", e_ALL_TYPES }
              , { L_,           "4",             "4", e_ALL_TYPES }
              , { L_,           "5",             "5", e_ALL_TYPES }
              , { L_,           "6",             "6", e_ALL_TYPES }
              , { L_,           "7",             "7", e_ALL_TYPES }
              , { L_,           "8",             "8", e_ALL_TYPES }
              , { L_,           "9",             "9", e_ALL_TYPES }
              , { L_,          "10",            "10", e_ALL_TYPES }
              , { L_,          "-1",            "-1", e_ALL_SIGNED_TYPES }
              , { L_,          "-2",            "-2", e_ALL_SIGNED_TYPES }
              , { L_,          "-3",            "-3", e_ALL_SIGNED_TYPES }
              , { L_,          "-4",            "-4", e_ALL_SIGNED_TYPES }
              , { L_,          "-5",            "-5", e_ALL_SIGNED_TYPES }
              , { L_,          "-6",            "-6", e_ALL_SIGNED_TYPES }
              , { L_,          "-7",            "-7", e_ALL_SIGNED_TYPES }
              , { L_,          "-8",            "-8", e_ALL_SIGNED_TYPES }
              , { L_,          "-9",            "-9", e_ALL_SIGNED_TYPES }
              , { L_,         "-10",           "-10", e_ALL_SIGNED_TYPES }

                // simple integers (w/ exponents)
              , { L_,         "0e1",             "0", e_ALL_INT_TYPES }
              , { L_,       "0.1e1",             "1", e_ALL_INT_TYPES }
              , { L_,       "10e-1",             "1", e_ALL_INT_TYPES }
              , { L_,         "1e1",            "10", e_ALL_INT_TYPES }
              , { L_,         "2e1",            "20", e_ALL_INT_TYPES }
              , { L_,         "3e1",            "30", e_ALL_INT_TYPES }
              , { L_,         "4e1",            "40", e_ALL_INT_TYPES }
              , { L_,         "5e1",            "50", e_ALL_INT_TYPES }
              , { L_,         "6e1",            "60", e_ALL_INT_TYPES }
              , { L_,         "7e1",            "70", e_ALL_INT_TYPES }
              , { L_,         "8e1",            "80", e_ALL_INT_TYPES }
              , { L_,         "9e1",            "90", e_ALL_INT_TYPES }
              , { L_,        "10e1",           "100", e_ALL_INT_TYPES }
              , { L_,      "-10e-1",            "-1", e_ALL_SIGNED_INT_TYPES }
              , { L_,        "-1e1",           "-10", e_ALL_SIGNED_INT_TYPES }
              , { L_,        "-2e1",           "-20", e_ALL_SIGNED_INT_TYPES }
              , { L_,        "-3e1",           "-30", e_ALL_SIGNED_INT_TYPES }
              , { L_,        "-4e1",           "-40", e_ALL_SIGNED_INT_TYPES }
              , { L_,        "-5e1",           "-50", e_ALL_SIGNED_INT_TYPES }
              , { L_,        "-6e1",           "-60", e_ALL_SIGNED_INT_TYPES }
              , { L_,        "-7e1",           "-70", e_ALL_SIGNED_INT_TYPES }
              , { L_,        "-8e1",           "-80", e_ALL_SIGNED_INT_TYPES }
              , { L_,        "-9e1",           "-90", e_ALL_SIGNED_INT_TYPES }
              , { L_,       "-10e1",          "-100", e_ALL_SIGNED_INT_TYPES }

                // simple FP values (w/ exponents)
              , { L_,      "0.25e1",           "2.5", e_ALL_FP_TYPES }
              , { L_,       "0.5e1",             "5", e_ALL_FP_TYPES }
              , { L_,      "0.75e1",           "7.5", e_ALL_FP_TYPES }
              , { L_,     "-0.25e1",          "-2.5", e_ALL_FP_TYPES }
              , { L_,      "-0.5e1",            "-5", e_ALL_FP_TYPES }
              , { L_,     "-0.75e1",          "-7.5", e_ALL_FP_TYPES }

                // decimal FP
              , { L_,         "0.0",           "0.0", e_DECIMAL64 }
              , { L_,         "0.1",           "0.1", e_DECIMAL64 }
              , { L_,      "0.01e1",           "0.1", e_DECIMAL64 }
              , { L_,      "0.01e2",             "1", e_DECIMAL64 }
              , { L_,      "0.01e3",         "1e+01", e_DECIMAL64 }
              , { L_,     "0.010e3",            "10", e_DECIMAL64 }
              , { L_,     "0.012e3",            "12", e_DECIMAL64 }
              , { L_,    "0.0121e3",          "12.1", e_DECIMAL64 }
              , { L_,      "0.01e4",         "1e+02", e_DECIMAL64 }
              , { L_,    "0.0100e4",           "100", e_DECIMAL64 }
              , { L_,     "0.01e-1",         "0.001", e_DECIMAL64 }
              , { L_,     "0.01e-2",        "0.0001", e_DECIMAL64 }
              , { L_,     "0.01e-3",       "0.00001", e_DECIMAL64 }
              , { L_,         "0.1",           "0.1", e_DECIMAL64 }
              , { L_,         "0.1",           "0.1", e_DECIMAL64 }
              , { L_,         "0.2",           "0.2", e_DECIMAL64 }
              , { L_,         "0.3",           "0.3", e_DECIMAL64 }
              , { L_,         "0.4",           "0.4", e_DECIMAL64 }
              , { L_,         "0.5",           "0.5", e_DECIMAL64 }
              , { L_,         "0.6",           "0.6", e_DECIMAL64 }
              , { L_,         "0.7",           "0.7", e_DECIMAL64 }
              , { L_,         "0.8",           "0.8", e_DECIMAL64 }
              , { L_,         "0.9",           "0.9", e_DECIMAL64 }
              , { L_,        "-0.0",          "-0.0", e_DECIMAL64 }
              , { L_,        "-0.1",          "-0.1", e_DECIMAL64 }
              , { L_,     "-0.01e1",          "-0.1", e_DECIMAL64 }
              , { L_,     "-0.01e2",            "-1", e_DECIMAL64 }
              , { L_,     "-0.01e3",        "-1e+01", e_DECIMAL64 }
              , { L_,    "-0.010e3",           "-10", e_DECIMAL64 }
              , { L_,    "-0.012e3",           "-12", e_DECIMAL64 }
              , { L_,   "-0.0121e3",         "-12.1", e_DECIMAL64 }
              , { L_,     "-0.01e4",        "-1e+02", e_DECIMAL64 }
              , { L_,   "-0.0100e4",          "-100", e_DECIMAL64 }
              , { L_,    "-0.01e-1",        "-0.001", e_DECIMAL64 }
              , { L_,    "-0.01e-2",       "-0.0001", e_DECIMAL64 }
              , { L_,    "-0.01e-3",      "-0.00001", e_DECIMAL64 }
              , { L_,        "-0.2",          "-0.2", e_DECIMAL64 }
              , { L_,        "-0.3",          "-0.3", e_DECIMAL64 }
              , { L_,        "-0.4",          "-0.4", e_DECIMAL64 }
              , { L_,        "-0.5",          "-0.5", e_DECIMAL64 }
              , { L_,        "-0.6",          "-0.6", e_DECIMAL64 }
              , { L_,        "-0.7",          "-0.7", e_DECIMAL64 }
              , { L_,        "-0.8",          "-0.8", e_DECIMAL64 }
              , { L_,        "-0.9",          "-0.9", e_DECIMAL64 }
            };

            for (int i = 0; i < int(sizeof(DATA)/sizeof(DATA[0])); ++i) {
                const int             LINE     = DATA[i].d_line;
                const char         *STRING     = DATA[i].d_string;
                const char         *EXPECT     = DATA[i].d_expectedString;
                const unsigned int  TYPE_FLAGS = DATA[i].d_typesFlags;

#define TEST_TYPE(FLAG, TYPE, CAST_TYPE)                                \
                if (TYPE_FLAGS & FLAG) {                                \
                    TYPE num;                                           \
                                                                        \
                    convertValue(&num, STRING);                         \
                                                                        \
                    bsl::string result;                                 \
                    Obj::stringify(&result, CAST_TYPE(num));            \
                                                                        \
                    ASSERTV(LINE, STRING, EXPECT, result,               \
                            EXPECT == result);                          \
                }

#ifdef BSLS_PLATFORM_CMP_IBM
                // IBM char is unsigned.
                TEST_TYPE(e_CHAR, signed char, Int64);
#else
                TEST_TYPE(e_CHAR, char, Int64);
#endif
                TEST_TYPE(e_SIGNED_CHAR, signed char, Int64);
                TEST_TYPE(e_UNSIGNED_CHAR, unsigned char, Uint64);

                TEST_TYPE(e_SHORT, short, Int64);
                TEST_TYPE(e_SIGNED_SHORT, signed short, Int64);
                TEST_TYPE(e_UNSIGNED_SHORT, unsigned short, Uint64);

                TEST_TYPE(e_INT, int, Int64);
                TEST_TYPE(e_SIGNED_INT, signed int, Int64);
                TEST_TYPE(e_UNSIGNED_INT, unsigned int, Uint64);

                TEST_TYPE(e_INT64, Int64, Int64);
                TEST_TYPE(e_UNSIGNED_INT64, Uint64, Uint64);

                TEST_TYPE(e_DOUBLE, double, double);
                TEST_TYPE(e_DECIMAL64, bdldfp::Decimal64, bdldfp::Decimal64);

#undef TEST_TYPE
            }
        }

        if (verbose)
            bsl::cout << "\ttest for isValidNumber"
                      << bsl::endl;
        {
            struct {
                const int   d_line;
                const char *d_string;
                const bool  d_isValid;
            } DATA[] = {
                // line                                       string isValid
                {  L_,                                          "0",    true }
              , {  L_,                                         "-0",    true }
              , {  L_,                                         "10",    true }
              , {  L_,                                        "-10",    true }
              , {  L_,                                 "1234567890",    true }
              , {  L_,                                "-1234567890",    true }
              , {  L_,                                        "0e0",    true }
              , {  L_,                                        "1e0",    true }
              , {  L_,                                        "1e1",    true }
              , {  L_,                                      "1.2e1",    true }
              , {  L_,                                       "1e-1",    true }
              , {  L_,                                     "1.2e-1",    true }
              , {  L_,                                     "1.2e+1",    true }
              , {  L_,                                 "1.2e+00001",    true }
              , {  L_,                                 "1.2e-00001",    true }
              , {  L_,                                  "1.2e00001",    true }
              , {  L_,                                        "0.0",    true }
              , {  L_,                                         "1e",   false }
              , {  L_,                                       "1.2e",   false }
              , {  L_,                                         "0-",   false }
              , {  L_,                                         "00",   false }
              , {  L_,                                         "01",   false }
              , {  L_,                                         "0.",   false }
              , {  L_,                                         ".0",   false }
            };

            for (int i = 0; i < int(sizeof(DATA) / sizeof(DATA[0])); ++i) {
                const int           LINE       = DATA[i].d_line;
                const char         *STRING     = DATA[i].d_string;
                const bool          IS_VALID   = DATA[i].d_isValid;

                bool rc = Obj::isValidNumber(STRING);
                ASSERTV(LINE, STRING, rc, IS_VALID, IS_VALID == rc);
            }
        }

        if (verbose)
            bsl::cout << "\ttest for asUint64"
                      << bsl::endl;
        {
            Uint64 val = 1234567890;
            int rc;

            rc = Obj::asUint64(&val, "1");
            ASSERTV(L_, rc, val, 0 == rc && 1 == val);

            val = 1234567890;
            rc = Obj::asUint64(&val, "1e1");
            ASSERTV(L_, rc, val, 0 == rc && 10 == val);

            val = 1234567890;
            rc = Obj::asUint64(&val, "0.1e1");
            ASSERTV(L_, rc, val, 0 == rc && 1 == val);

            val = 1234567890;
            rc = Obj::asUint64(&val, "1.2e1");
            ASSERTV(L_, rc, val, 0 == rc && 12 == val);

            val = 1234567890;
            rc = Obj::asUint64(&val, "1.0002e4");
            ASSERTV(L_, rc, val, 0 == rc && 10002 == val);

            val = 1234567890;
            rc = Obj::asUint64(&val, "120.0e-1");
            ASSERTV(L_, rc, val, 0 == rc && 12 == val);

            val = 1234567890;
            rc = Obj::asUint64(&val, "120.0000e-1");
            ASSERTV(L_, rc, val, 0 == rc && 12 == val);

            val = 1234567890;
            rc = Obj::asUint64(&val, "120.0000e+1");
            ASSERTV(L_, rc, val, 0 == rc && 1200 == val);

            val = 1234567890;
            // Note that
            // https://www.rfc-editor.org/rfc/rfc8259#section-6
            // allows for leading 0's after the [eE].
            rc = Obj::asUint64(&val, "120.0000e-00001");
            ASSERTV(L_, rc, val, 0 == rc && 12 == val);

            val = 1234567890;
            rc = Obj::asUint64(&val, "0.2e1");
            ASSERTV(L_, rc, val, 0 == rc && 2 == val);

            val = 1234567890;
            rc = Obj::asUint64(&val, "1.21e1");
            ASSERTV(L_, rc, val, ENOTINT == rc && 12 == val);

            val = 1234567890;
            rc = Obj::asUint64(&val, "-1");
            ASSERTV(L_, rc, val, EUNDER == rc && 0 == val);

        }

      } break;
      case -1: {
        //---------------------------------------------------------------------
        // BENCHMARK: asDecimal64Exact vs asDecimal64ExactOracle
        //
        // This benchmark compares the current 'asDecimal64Exact' function
        // which delegates entirely to 'inteldfp' parsing, to a "homebrewn"
        // implementation that parses the string into digits to use with
        // 'makeDecimla64Raw' (falling back to 'inteldfp' parsing for inexact
        // conversions)
        //---------------------------------------------------------------------

        if (verbose) {
            bsl::cout
                << bsl::endl
                << "BENCHMARK: asDecimal64Exact vs asDecimal64ExactOracle\n"
                << "=====================================================\n"
                << bsl::endl;
        }

        const char *DATA[] = {
            "0",
            "0.0",
            "0.0000",
            "1",
            "1e-0",
            "1e+0",
            "10",
            "105",
            "1e1",
            "1.5e1",
            "1.5e1",
            "1.5e2",
            "1.0e2",
            "150.0e-1",
            "3757.99",
            "27153.83",
            "6574.70",
            "3.022e9",
            "4530e9",
            "7.753e9",
            "1238414121.2",
            "1238414121.21e-12",
            "510072000",
            "9.80665",
            //"1238414121.14134242341543",
            //"18446744073709551615",
        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        int numIterations = 100000;

        if (argc > 2) {
            numIterations = atoi(argv[2]);
        }
        bsls::Stopwatch s;

        bdldfp::Decimal64 totalA, totalB;

        for (int i = 0; i < NUM_DATA; ++i) {
            bdldfp::Decimal64 rA, rB;
            Obj::asDecimal64Exact(&rA, DATA[i]);
            asDecimal64ExactOracle(&rB, DATA[i]);

            ASSERTV(DATA[i], rA, rB, rA == rB);
        }

        s.start();
        for (int i = 0; i < numIterations; ++i) {
            for (int j = 0; j < NUM_DATA; ++j) {
                bdldfp::Decimal64 result;
                asDecimal64ExactOracle(&result, DATA[j]);
                totalB += result;
            }
        }
        s.stop();
        bsl::cout << "homebrew: asDecimal64Exact" << s.elapsedTime()
                  << bsl::endl;

        s.reset();

        s.start();
        for (int i = 0; i < numIterations; ++i) {
            for (int j = 0; j < NUM_DATA; ++j) {
                bdldfp::Decimal64 result;
                Obj::asDecimal64Exact(&result, DATA[j]);
                totalA += result;
            }
        }
        s.stop();
        bsl::cout << "bdldfp: asDecimal64Exact" << s.elapsedTime()
                  << bsl::endl;

        ASSERTV(totalA, totalB, totalA == totalB);

      } break;

      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
