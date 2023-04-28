// bdljsn_numberutil.cpp                                              -*-C++-*-
#include <bdljsn_numberutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdljsn_numberutil_cpp, "$Id$ $CSID$")

#include <bdlb_chartype.h>

#include <bdlb_string.h>
#include <bdldfp_decimal.h>
#include <bdldfp_decimalutil.h>
#include <bdlma_localsequentialallocator.h>

#include <bslalg_numericformatterutil.h>
#include <bsls_log.h>

#include <bsl_cctype.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace bdljsn {
namespace {
namespace u {

static const bsls::Types::Uint64 INT64_MAX_VALUE =
                               bsl::numeric_limits<bsls::Types::Int64>::max();

static const bsls::Types::Uint64 UINT64_MAX_VALUE =
                               bsl::numeric_limits<bsls::Types::Uint64>::max();
static const bsls::Types::Uint64 UINT64_MAX_DIVIDED_BY_10 =
                                                         UINT64_MAX_VALUE / 10;
static const bsls::Types::Uint64 UINT64_MAX_DIVIDED_BY_10_TO_THE_10 =
                                             UINT64_MAX_VALUE / 10000000000ULL;
static const bsls::Types::Uint64 UINT64_MAX_VALUE_LAST_DIGIT = 5;

static const bsls::Types::Uint64 POWER_OF_TEN_LOOKUP[] = {
    1ULL,              // 1e0
    10ULL,             // 1e1
    100ULL,            // 1e2
    1000ULL,           // 1e3
    10000ULL,          // 1e4
    100000ULL,         // 1e5
    1000000ULL,        // 1e6
    10000000ULL,       // 1e7
    100000000ULL,      // 1e8
    1000000000ULL,     // 1e9
    10000000000ULL,    // 1e10
    100000000000ULL};  // 1e11

static bool skipIfIsValidUint(bsl::string_view::const_iterator *iter,
                              bsl::string_view::const_iterator  end)
    // Return 'true' if the string specified by the range '[*iter, end)' has a
    // valid leading unsigned integer prefix.  Empty strings and leading 0's
    // are not allowed.  Increment '*iter' to point to the first character
    // after the leading unsigned integer sequence (or to 'end').  If this
    // function returns 'true', '*iter' has been advanced at least once past a
    // valid unsigned integer.  If this function returns 'false', '*iter' is
    // left in a valid but unspecified state (i.e., it may or may not have been
    // advanced).
{
    // The empty string is invalid.
    if (*iter == end) {
        return false;                                                 // RETURN
    }

    if ('0' == **iter) {
        ++*iter;

        if (*iter == end) {
            return true;                                              // RETURN
        }

        if (bdlb::CharType::isDigit(**iter)) {
            // Leading 0's are not allowed.
            return false;                                             // RETURN
        }

        return true;                                                  // RETURN
    }

    if (!bdlb::CharType::isDigit(**iter)) {
        return false;                                                 // RETURN
    }

    while (*iter != end && bdlb::CharType::isDigit(**iter)) {
        ++*iter;
    }

    return true;
}

struct DecomposedNumber {
    // This struct holds a  data members for each of the  decomposed elements
    // of a JSON number (see 'NumberUtil_ImpUtil::decompose').  This type
    // serves to avoid a large number of local variables, particularly when
    // working with multiple decomposed number presentations at the same time
    // (e.g., in implementing 'areEqual').

    // PUBLIC DATA
    bool                        d_isNegative;
    bool                        d_isExpNegative;
    bsl::string_view            d_integer;
    bsl::string_view            d_fraction;
    bsl::string_view            d_exponent;
    bsl::string_view            d_significantDigits;
    bsls::Types::Int64          d_significantDigitsBias;
    bsl::string_view::size_type d_significantDigitsDotOffset;

  public:
    // CREATORS
    explicit DecomposedNumber(const bsl::string_view& value);
        // Load the public data members of this object with the results of
        // calling 'NumberUtil_ImpUtil::decompose' on the specified 'value'.
        // The behavior is undefined unless 'NumberUtil::isValidNumber(value)'
        // is 'true'.
};

inline
DecomposedNumber::DecomposedNumber(const bsl::string_view& value)
{
    typedef NumberUtil_ImpUtil ImpUtil;
    ImpUtil::decompose(&d_isNegative,
                       &d_isExpNegative,
                       &d_integer,
                       &d_fraction,
                       &d_exponent,
                       &d_significantDigits,
                       &d_significantDigitsBias,
                       &d_significantDigitsDotOffset,
                       value);
    if (d_integer[0] == '0') {
        d_isExpNegative = false;
    }
}

BSLS_ANNOTATION_UNUSED
bsl::ostream& operator<<(bsl::ostream& stream, const DecomposedNumber& n)
    // Write the specified number 'n' to the specified 'stream' and return a
    // modifiable reference to the specified 'stream'.  Note that this function
    // is frequently useful in debugging, but is not currently used in the
    // implementation.
{
    stream << "[ isNegative = " << n.d_isNegative << ", "
           << "isExpNegative = " << n.d_isExpNegative << ", "
           << "integer = \"" << n.d_integer << "\", "
           << "fraction = \"" << n.d_fraction << "\", "
           << "exponent = \"" << n.d_exponent << "\", "
           << "significant = \"" << n.d_significantDigits << "\", "
           << "significantDigitsBias = " << n.d_significantDigitsBias << ", "
           << "significantDigitsDotOffset = " << n.d_significantDigitsDotOffset
           << " ]";
    return stream;
}

static bool compareNumberTextFallback(const DecomposedNumber& lhs,
                                      const DecomposedNumber& rhs)
    // Return 'true' if the specified 'lhs' has the same text for its digits
    // and exponent as the specified 'rhs'.  This operation serves as a
    // fallback for 'areEqual' if the exponent cannot be represented in an
    // Int64.  This operation ignores the sign (that has already been tested
    // for equality), and primarily serves to ignore a '+' character if it
    // appears in the Exp.  E.g., "1e100000000000000000000" and
    // "1e+100000000000000000000" would compare equal.
{
    if (lhs.d_isExpNegative != rhs.d_isExpNegative) {
        return false;                                                 // RETURN
    }

    if (lhs.d_significantDigits != rhs.d_significantDigits) {
        return false;                                                 // RETURN
    }

    return lhs.d_exponent == rhs.d_exponent;
}

static bsl::string_view::const_iterator findFirstNonDigit(
                                        bsl::string_view::const_iterator begin,
                                        bsl::string_view::const_iterator end)
    // Return the first non digit character starting at the specified 'begin'
    // iterator, and prior to the specified 'end' character.  Return 'end' if
    // all of the characters are digits.  Note that 'find_if_not' is a C++20
    // algorithm.
{
    for (; begin != end && bdlb::CharType::isDigit(*begin); ++begin);
    return begin;
}

}  // close namespace u
}  // close unnamed namespace

                           // -----------------
                           // struct NumberUtil
                           // -----------------

int NumberUtil::asUint64(bsls::Types::Uint64     *result,
                         const bsl::string_view&  value)
{
    BSLS_ASSERT(isValidNumber(value));

    typedef NumberUtil_ImpUtil ImpUtil;

    bool  isNeg, isExpNegative;
    Int64 exponentBias;

    bsl::string_view integer, fraction, exponentStr, significantDigits;
    bsl::string_view::size_type significantDigitsDotOffset;

    ImpUtil::decompose(&isNeg,
                       &isExpNegative,
                       &integer,
                       &fraction,
                       &exponentStr,
                       &significantDigits,
                       &exponentBias,
                       &significantDigitsDotOffset,
                       value);

    // Handle 0 as a special case (no need to consider the exponent of
    // 'isNeg').

    if ('0' == significantDigits[0]) {
        BSLS_ASSERT(1 == significantDigits.size());
        *result = 0;
        return 0;                                                     // RETURN
    }

    // We consider negative values after testing for '0' to correctly handle
    // "-0"

    if (isNeg) {
        *result = 0;
        return k_UNDERFLOW;                                           // RETURN
    }

    // Compute the exponent.

    Uint64 uExponent;

    int rc = ImpUtil::appendDigits(&uExponent, 0, exponentStr);

    if (0 != rc || uExponent > u::UINT64_MAX_DIVIDED_BY_10) {
        if (isExpNegative) {
            *result = 0;
            return k_NOT_INTEGRAL;                                    // RETURN
        }
        *result = u::UINT64_MAX_VALUE;
        return k_OVERFLOW;                                            // RETURN
    }

    Int64 exponent = (isExpNegative ? -1 : 1) *
                              static_cast<Int64>(uExponent) +
                          exponentBias;

    // A flag indicating if the significant digits range includes a '.'.

    bool sigDigitsSeparated =
        significantDigitsDotOffset != bsl::string_view::npos;

    Int64 numSigDigits = significantDigits.size() -
                                                 (sigDigitsSeparated  ? 1 : 0);

    // Return an error the number has too many digits (numSigDigits + zeros
    // from the exponent) to fit into a Uint64.

    if (numSigDigits + exponent > 20) {
        *result = u::UINT64_MAX_VALUE;
        return k_OVERFLOW;                                            // RETURN
    }

    if (numSigDigits + exponent < 0) {
        // There are no integer digits

        *result = 0;
        return k_NOT_INTEGRAL;                                        // RETURN
    }

    bsl::string_view adjustedSignificant = significantDigits;
    if (exponent < 0) {
        // This value will not be an integer, but we still must compute the
        // closest integer.  We truncate the fractional digits.

        adjustedSignificant.remove_suffix(static_cast<bsl::size_t>(-exponent));

        //  If the significant digits previously had a separator, and we've
        //  adjusted the end of the significant digits past the separator or
        //  just up to the separator, we need to subtract one more to account
        //  for the separator.

        if (sigDigitsSeparated && adjustedSignificant.size() <=
                                      significantDigitsDotOffset + 1) {
            adjustedSignificant.remove_suffix(1);
            sigDigitsSeparated = false;
        }
    }

    // Compute the value of the remaining significant digits.  If there is a
    // decimal separator in the middle of the significant digits, we must do
    // this in two chunks.

    bsl::string_view digits, moreDigits;
    if (!sigDigitsSeparated) {
        // There isn't a decimal separator in the middle of the significant
        // digits, we have a single block of digits.
        digits = adjustedSignificant;
    }
    else {
        // There is a decimal separator, so there are two blocks of digits.

        digits     = adjustedSignificant.substr(0, significantDigitsDotOffset);
        moreDigits = adjustedSignificant.substr(significantDigitsDotOffset +
                                                1);
        BSLS_ASSERT(0 != digits.size());
        BSLS_ASSERT(0 != moreDigits.size());
    }

    Uint64 tmp;
    if (0 != ImpUtil::appendDigits(&tmp, 0, digits)) {
        *result = u::UINT64_MAX_VALUE;
        return k_OVERFLOW;                                            // RETURN
    }
    if (0 != ImpUtil::appendDigits(&tmp, tmp, moreDigits)) {
        *result = u::UINT64_MAX_VALUE;
        return k_OVERFLOW;                                            // RETURN
    }

    // Add 0's for any remaining exponent.
    if (exponent > 0) {
        BSLS_ASSERT(exponent <= 19);  // sanity. tested above

        if (exponent >= 10) {
            if (tmp > u::UINT64_MAX_DIVIDED_BY_10_TO_THE_10) {
                *result = u::UINT64_MAX_VALUE;
                return k_OVERFLOW;                                    // RETURN
            }
            tmp *= 10000000000ULL;
            exponent -= 10;
        }

        BSLS_ASSERT(exponent <= 9);  // sanity. tested above

        const bsls::Types::Uint64 uExponentMultiple =
                                              u::POWER_OF_TEN_LOOKUP[exponent];

        if (u::UINT64_MAX_VALUE / uExponentMultiple >= tmp) {
            tmp *= uExponentMultiple;
        }
        else {
            *result = u::UINT64_MAX_VALUE;
            return k_OVERFLOW;                                        // RETURN
        }
    }

    *result = tmp;
    if (exponent < 0) {
        // This number was not an integer.  For the special case where the
        // value is UINT64_MAX_VALUE plus some fraction, we prefer to report an
        // overflow.

        return (tmp == u::UINT64_MAX_VALUE) ? k_OVERFLOW : k_NOT_INTEGRAL;
                                                                      // RETURN
    }

    return 0;
}

bdldfp::Decimal64 NumberUtil::asDecimal64(const bsl::string_view& value)
{
    BSLS_ASSERT(NumberUtil::isValidNumber(value));

    // 'parseDecimal64' expects a 0 terminated string, so we load a string
    // backed by stack buffer sufficient to store any normal Decimal64 value.

    bdlma::LocalSequentialAllocator<32> allocator;

    bsl::string dataString(value, &allocator);

    bdldfp::Decimal64 d;

    int rc = bdldfp::DecimalUtil::parseDecimal64(&d, dataString.c_str());
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 != rc)) {
        // This should not be possible and no test has found a case where this
        // is occurs.  However, to prevent a possible denial of service attack
        // if some gap were to be found in the implementation of low-level
        // floating point parsing functions and the JSON number spec, we
        // provide in-contract defined behavior for optimized (production)
        // builds.

        BSLS_ASSERT(false);
        NumberUtil_ImpUtil::logUnparseableJsonNumber(value);
        return bsl::numeric_limits<bdldfp::Decimal64>::quiet_NaN();   // RETURN
    }
    return d;
}

int NumberUtil::asDecimal64Exact(bdldfp::Decimal64       *result,
                                 const bsl::string_view&  value)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(isValidNumber(value));

    // 'parseDecimal64' expects a 0 terminated string, so we load a string
    // backed by stack buffer sufficient to store any normal Decimal64 value.

    bdlma::LocalSequentialAllocator<32> allocator;

    bsl::string dataString(value, &allocator);

    // Note that there an issue in the inteldfp library with incorrect status
    // values returned for 0's with an exponent outside the valid range for a
    // 'Decimal64'.  E.g., 0e200 will incorrectly return k_INEXACT.  The test
    // driver function 'asDecimal64ExactOracle' provides an alternative
    // implementation that fixes that (but is far more complicated).

    int rc = bdldfp::DecimalUtil::parseDecimal64Exact(result,
                                                      dataString.c_str());
    return 0 != rc ? k_INEXACT : 0;
}

bool NumberUtil::areEqual(const bsl::string_view& lhs,
                          const bsl::string_view& rhs)
{
    BSLS_ASSERT(isValidNumber(lhs));
    BSLS_ASSERT(isValidNumber(rhs));

    typedef NumberUtil_ImpUtil ImpUtil;

    // Implementation Note: The comparison implemented below attempts to
    // perform a comparison as if it were using a canonical representation of
    // the JSON numbers where leading and trailing 0s are removed, and the
    // exponent is adjusted so that the value is a whole number (without
    // trailing 0s).  E.g, "1.43e1" would be 143e-1 in this canonical
    // representation, and "14300" would be 143e2.
    //
    // This form is simulated by using the significant digits from 'value'
    // (ignoring the '.' character, i.e., treating it as a whole number), and
    // applying the 'significantDigitBias' to the exponent of 'value' to get
    // the canonical exponent.
    //
    // If we cannot represent the canonical exponent (ignoring its sign) as a
    // uint64 (a value that is beyond astronomically large or small) we fall
    // back to a simple text comparison of the two numbers (doing better would
    // require a big-int type, which is not currently available in BDE).

    u::DecomposedNumber l(lhs), r(rhs);

    bsl::string_view::const_iterator lend = l.d_significantDigits.end();
    bsl::string_view::const_iterator rend = r.d_significantDigits.end();
    bsl::string_view::const_iterator lIt  = l.d_significantDigits.begin();
    bsl::string_view::const_iterator rIt  = r.d_significantDigits.begin();

    // Compare the significant digits, ignoring any '.' character.

    while (rIt != rend && lIt != lend) {
        if (*rIt != *lIt) {
            if (*rIt == '.') {
                ++rIt;
            }
            else if (*lIt == '.') {
                ++lIt;
            }
            else {
                return false;                                         // RETURN
            }
        }
        else {
            ++rIt;
            ++lIt;
        }
    }

    if (rIt != rend || lIt != lend) {
        return false;                                                 // RETURN
    }

    if ('0' == l.d_significantDigits[0]) {
        // Both numbers are 0.

        return true;                                                  // RETURN
    }

    if (l.d_isNegative != r.d_isNegative) {
        return false;                                                 // RETURN
    }

    // The significant digits and sign of 'lhs' and 'rhs' are the same.  Next
    // we must test the canonical exponent value.  We start by computing the
    // exponents in the text of 'lhs' and 'rhs'.

    // If the exponents are too large to fit in int64 values, we fall back to
    // string equality.

    Uint64 lUExp, rUExp;

    int rc = ImpUtil::appendDigits(&lUExp, 0, l.d_exponent);
    if (0 != rc) {
        return compareNumberTextFallback(l, r);                       // RETURN
    }

    rc = ImpUtil::appendDigits(&rUExp, 0, r.d_exponent);
    if (0 != rc) {
        return compareNumberTextFallback(l, r);                       // RETURN
    }

    // If computing the canonical exponent at any point requires values that
    // cannot be represented as a int64, fall back to a string comparison.

    if (lUExp > u::INT64_MAX_VALUE || rUExp > u::INT64_MAX_VALUE ||
        u::INT64_MAX_VALUE - l.d_significantDigitsBias < lUExp ||
        u::INT64_MAX_VALUE - r.d_significantDigitsBias < rUExp) {
        return compareNumberTextFallback(l, r);                       // RETURN
    }

    Int64 lExponent = (l.d_isExpNegative ? -1 : 1) * static_cast<Int64>(lUExp)
                      + l.d_significantDigitsBias;
    Int64 rExponent = (r.d_isExpNegative ? -1 : 1) * static_cast<Int64>(rUExp)
                      + r.d_significantDigitsBias;

    return lExponent == rExponent;
}

bool NumberUtil::isValidNumber(const bsl::string_view& value)
{
    // Validate the syntax specified in
    //   https://www.rfc-editor.org/rfc/rfc8259#section-6

    // The pattern is /^-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][-+]?[0-9]+)?$/

    if (0 == value.size()) {
        return false;                                                 // RETURN
    }

    bsl::string_view::const_iterator iter = value.begin();
    bsl::string_view::const_iterator end  = value.end();

    if ('-' == *iter) {
        // Skip valid leading '-'.
        ++iter;
    }

    bool rc = u::skipIfIsValidUint(&iter, end);

    if (!rc) {
        return false;                                                 // RETURN
    }

    if (end == iter) {
        return true;                                                  // RETURN
    }

    if ('.' != *iter && 'e' != *iter && 'E' != *iter) {
        return false;                                                 // RETURN
    }

    if ('.' == *iter) {
        ++iter;

        if (end == iter) {  // Nothing after '.'
            return false;                                             // RETURN
        }

        if (!bdlb::CharType::isDigit(*iter)) {
            // Require 1 digit after decimal.
            return false;                                             // RETURN
        }

        while (iter != end && bdlb::CharType::isDigit(*iter)) {
            ++iter;
        }
    }

    if (end == iter) {
        return true;                                                  // RETURN
    }

    if ('e' == *iter || 'E' == *iter) {
        ++iter;

        if (end == iter) {
            return false;                                             // RETURN
        }

        if ('-' == *iter || '+' == *iter) {
            ++iter;
        }

        if (end == iter) {  // Nothing after '+' or '-' sign.
            return false;                                             // RETURN
        }

        if (!bdlb::CharType::isDigit(*iter)) {
            // Require 1 digit after [Ee] and optional sign.
            return false;                                             // RETURN
        }

        while (iter != end && bdlb::CharType::isDigit(*iter)) {
            ++iter;
        }
    }

    if (iter != end) {
        // Don't allow trailing characters.
        return false;                                                 // RETURN
    }

    return true;
}

bool NumberUtil::isIntegralNumber(const bsl::string_view& value)
{
    BSLS_ASSERT(isValidNumber(value));

    typedef NumberUtil_ImpUtil ImpUtil;

    bool  isNeg, isExpNegative;
    Int64 exponentBias;

    bsl::string_view integer, fraction, exponentStr, significantDigits;
    bsl::string_view::size_type significantDigitsDotOffset;

    ImpUtil::decompose(&isNeg,
                       &isExpNegative,
                       &integer,
                       &fraction,
                       &exponentStr,
                       &significantDigits,
                       &exponentBias,
                       &significantDigitsDotOffset,
                       value);

    if ('0' == significantDigits[0]) {
        // 0 is an integer regardless of how its represented.

        return true;                                                  // RETURN
    }

    // Compute the exponent.

    Uint64 exponent;

    int rc = ImpUtil::appendDigits(&exponent, 0, exponentStr);
    if (0 != rc) {
        // The exponent is more than UINT64_MAX or less than -UINT64_MAX
        // 'value' cannot have enough digits to matter.

        return !isExpNegative;                                        // RETURN
    }

    // 'value' is integral if the canonical exponent for 'value' is >= 0 (see
    // NumberUtil_Imp::decompose for an explantion of the canonical exponent).
    // A mathematical expression for this would be:
    //..
    // return 0 <= (isExpNegative ? -exponent : exponent) + exponentBias
    //..
    // However, to avoid edge cases and rounding issues we use the following
    // more complicated logic (it is safe to negate 'exponentBias', because
    // 'exponentBias' cannot be INT64_MIN, as that would require a string with
    // INT64_MAX characters).

    if (isExpNegative) {
        return exponentBias >= 0 &&
            static_cast<Uint64>(exponentBias) >= exponent;            // RETURN
    }
    return exponentBias >= 0 || static_cast<Uint64>(-exponentBias) <= exponent;
}

void NumberUtil::stringify(bsl::string *result, bsls::Types::Int64 value)
{
    typedef bslalg::NumericFormatterUtil NFU;
    char  buffer[NFU::ToCharsMaxLength<bsls::Types::Int64>::k_VALUE];
    char *ret = bslalg::NumericFormatterUtil::toChars(
        buffer, buffer + sizeof buffer, value);
    BSLS_ASSERT(0 != ret);

    result->assign(buffer, ret - buffer);
}

void NumberUtil::stringify(bsl::string *result, bsls::Types::Uint64 value)
{
    typedef bslalg::NumericFormatterUtil NFU;
    char  buffer[NFU::ToCharsMaxLength<bsls::Types::Uint64>::k_VALUE];
    char *ret = bslalg::NumericFormatterUtil::toChars(
        buffer, buffer + sizeof buffer, value);
    BSLS_ASSERT(0 != ret);

    result->assign(buffer, ret - buffer);
}

void NumberUtil::stringify(bsl::string *result, double value)
{
    typedef bslalg::NumericFormatterUtil NFU;
    char  buffer[NFU::ToCharsMaxLength<double>::k_VALUE];
    char *ret = bslalg::NumericFormatterUtil::toChars(
        buffer, buffer + sizeof buffer, value);
    BSLS_ASSERT(0 != ret);

    result->assign(buffer, ret - buffer);
}

void NumberUtil::stringify(bsl::string *result, const bdldfp::Decimal64& value)
{
    enum {
        k_LENGTH = 40
    };
    char buffer[k_LENGTH];

    int numChars = bdldfp::DecimalUtil::format(buffer, k_LENGTH, value);

    BSLS_ASSERT(numChars < k_LENGTH);
    result->assign(buffer, numChars);
}

                         // -------------------------
                         // struct NumberUtil_ImpUtil
                         // -------------------------

int NumberUtil_ImpUtil::appendDigits(bsls::Types::Uint64     *result,
                                     bsls::Types::Uint64      startingValue,
                                     const bsl::string_view&  digits)
{
    bsls::Types::Uint64 value = startingValue;

    bsl::string_view::const_iterator iter = digits.begin();
    while (iter != digits.end()) {
        const unsigned digitValue = *iter - '0';
        if (value < u::UINT64_MAX_DIVIDED_BY_10 ||
            (u::UINT64_MAX_DIVIDED_BY_10 == value &&
             digitValue <= u::UINT64_MAX_VALUE_LAST_DIGIT)) {
            value = value * 10 + digitValue;
            ++iter;
        }
        else {
            return NumberUtil::k_OVERFLOW;                            // RETURN
        }
    }
    *result = value;
    return 0;
}

void NumberUtil_ImpUtil::decompose(
                       bool                        *isNegative,
                       bool                        *isExpNegative,
                       bsl::string_view            *integer,
                       bsl::string_view            *fraction,
                       bsl::string_view            *exponent,
                       bsl::string_view            *significantDigits,
                       bsls::Types::Int64          *significantDigitsBias,
                       bsl::string_view::size_type *significantDigitsDotOffset,
                       const bsl::string_view&      value)
{
    // Design note:  This function returns a bias for the significant digits,
    // rather than actual exponent of significant digits, because the bias can
    // trivially derived from the iterator positions returned by this function,
    // without needing to compute the actual numeric exponent 'value'.
    // Computing the exponent of 'value' requires more expensive arithmetic,
    // and must deal with out-of-range values, and in some use-cases may not be
    // needed.  See the function documentation for a definition of
    // 'significantDigitsBias'.

    // We use 'ASSERT_SAFE' because this is a (relatively expensive) redundant
    // check of our own implementation (all the functions calling this should
    // have already checked this condition).

    BSLS_ASSERT_SAFE(NumberUtil::isValidNumber(value));

    const bsl::string_view::size_type npos = bsl::string_view::npos;

    bsl::string_view input(value);

    if ('-' == input[0]) {
        *isNegative = true;
        input.remove_prefix(1);
    }
    else {
        *isNegative = false;
    }

    BSLS_ASSERT(!input.empty());

    bsl::string_view::const_iterator intEnd =
                              u::findFirstNonDigit(input.begin(), input.end());
    bsl::string_view::size_type intLen = bsl::distance(input.begin(), intEnd);

    *integer = input.substr(0, intLen);

    bsl::string_view digits;
    bsl::string_view::size_type digitsDotOffset;

    // Iterate over the fractional digits.
    if (intEnd != input.end() && '.' == *intEnd) {
        bsl::string_view::size_type fracLen = bsl::distance(
                                intEnd + 1,
                                u::findFirstNonDigit(intEnd + 1, input.end()));

        *fraction       = input.substr(intLen + 1, fracLen);
        digits          = input.substr(0, intLen + 1 + fracLen);
        digitsDotOffset = intLen;
    }
    else {
        *fraction       = input.substr(intLen, 0);
        digits          = *integer;
        digitsDotOffset = npos;
    }
    input.remove_prefix(digits.length());

    *isExpNegative = false;
    if (!input.empty()) {
        input.remove_prefix(1);  // Skip 'e' or 'E'
        switch (input[0]) {
          case '-': {
            *isExpNegative = true;
          } BSLS_ANNOTATION_FALLTHROUGH;
          case '+': {
            input.remove_prefix(1);
          } break;
        }
    }
    *exponent = input;

    // 'digits' and 'digitsDotOffset' contain the non-canonical digits (and
    // decimal point) of 'value', from them we need to remove the leading and
    // trailing 0s to compute 'significantDigits',
    // 'significantDigitsDotOffset', and 'signficantDigitsBias'.  Some
    // example values:
    //..
    // | digits   | significantDigits | sigDigitsDotOffset | sigDigitsBias |
    // |----------|-------------------|--------------------|---------------|
    // | "0.00"   | "0"               | npos               | 0             |
    // | "100"    | "1"               | npos               | 2             |
    // | "34.50"  | "34.5"            | 2                  | -1            |
    // | "0.020"  | "2"               | npos               | -2            |
    //..
    // Notice that the '.' is ignored when considering 'significantDigits' (so
    // "34.5" is treated as "345" and the bias is -1).


    // We compute the index of the first and last significant digit in order to
    // assign 'significantDigits'.

    bsl::string_view::size_type firstSignificantDigit, lastSignificantDigit;
    bsl::string_view::size_type lastNotZero = digits.find_last_not_of('0');

    // We ignore the fraction of 'digits' if there is no decimal point,
    // or if the fraction 'digits' are all 0s.

    bool ignoreFraction =  digitsDotOffset == npos;
    if (!ignoreFraction && lastNotZero == digitsDotOffset) {
        // All the fraction digits are '0'.

        ignoreFraction = true;
        lastNotZero    = integer->find_last_not_of('0');
    }

    if (ignoreFraction) {
        // An integer (no fraction digits, or all 0 fraction digits).  E.g.:
        // "0.00", "100"

        firstSignificantDigit = 0;

        // if 'lastNotZero' is 'npos' the number is all 0's

        lastSignificantDigit = lastNotZero == npos ? 0 : lastNotZero;

        *significantDigitsBias = integer->length() - lastSignificantDigit - 1;

        *significantDigitsDotOffset = npos;
    }
    else {
        // A number with non-zero fraction digits.  E.g.: "34.50", "0.020"

        BSLS_ASSERT(digitsDotOffset != npos && lastNotZero > digitsDotOffset);

        lastSignificantDigit = lastNotZero;

        *significantDigitsBias = -1LL * (lastSignificantDigit - integer->length());

        if (digits[0] != '0') {
            // E.g., "34.50"
            firstSignificantDigit = 0;
            *significantDigitsDotOffset = digitsDotOffset;
        }
        else {
            // E.g., "0.020"
            BSLS_ASSERT(1 == digitsDotOffset);

            const bsl::string_view::size_type firstNotZero =
                                              fraction->find_first_not_of('0');

            // Already handled 0.[0]+ (ignoreFraction would have been 'true').
            BSLS_ASSERT(firstNotZero != bsl::string_view::npos);

            firstSignificantDigit = 2 + firstNotZero;

            *significantDigitsDotOffset = bsl::string_view::npos;
        }
    }
    *significantDigits =
        digits.substr(firstSignificantDigit,
                      lastSignificantDigit - firstSignificantDigit + 1);
}

void NumberUtil_ImpUtil::logUnparseableJsonNumber(
                                                 const bsl::string_view& value)
{
    bsl::ostringstream stream;
    stream << "Unparseable JSON number found: ";
    if (value.size() < 30) {
        stream << value;
    }
    else {
        stream << value.substr(0, 30) << " (truncated from " << value.size()
               << " bytes)";
    }
    bsl::string text(stream.str());
    BSLS_LOG_FATAL(text.c_str());
}

}  // close package namespace
}  // close enterprise namespace

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
