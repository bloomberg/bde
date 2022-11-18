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
    bool                             d_isNegative;
    bool                             d_isExpNegative;
    bsl::string_view::const_iterator d_integerBegin;
    bsl::string_view::const_iterator d_integerEnd;
    bsl::string_view::const_iterator d_fractionBegin;
    bsl::string_view::const_iterator d_fractionEnd;
    bsl::string_view::const_iterator d_significantDigitsBegin;
    bsl::string_view::const_iterator d_significantDigitsEnd;
    bsl::string_view::const_iterator d_exponentBegin;
    bsls::Types::Int64               d_significantDigitsBias;
    bsl::string_view                 d_value;

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
: d_value(value)
{
    typedef NumberUtil_ImpUtil ImpUtil;
    ImpUtil::decompose(&d_isNegative,
                       &d_integerBegin,
                       &d_integerEnd,
                       &d_fractionBegin,
                       &d_fractionEnd,
                       &d_isExpNegative,
                       &d_exponentBegin,
                       &d_significantDigitsBegin,
                       &d_significantDigitsEnd,
                       &d_significantDigitsBias,
                       value);
    if (*d_integerBegin == '0') {
        d_isExpNegative = false;
    }
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

    bsl::string_view lSignificand(&*lhs.d_integerBegin,
                                  lhs.d_fractionEnd - lhs.d_integerBegin);
    bsl::string_view rSignificand(&*rhs.d_integerBegin,
                                  rhs.d_fractionEnd - rhs.d_integerBegin);

    if (lSignificand != rSignificand) {
        return false;                                                 // RETURN
    }



    bsl::string_view lExp(&*lhs.d_exponentBegin,
                          lhs.d_value.end() - lhs.d_exponentBegin);
    bsl::string_view rExp(&*rhs.d_exponentBegin,
                          rhs.d_value.end() - rhs.d_exponentBegin);

    return lExp == rExp;
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

    bsl::string_view::const_iterator intBegin, intEnd, fracBegin, fracEnd,
        expBegin, sigBegin, sigEnd;

    ImpUtil::decompose(&isNeg,
                       &intBegin,
                       &intEnd,
                       &fracBegin,
                       &fracEnd,
                       &isExpNegative,
                       &expBegin,
                       &sigBegin,
                       &sigEnd,
                       &exponentBias,
                       value);

    // Handle 0 as a special case (no need to consider the exponent of
    // 'isNeg').

    if ('0' == *sigBegin) {
        BSLS_ASSERT(sigBegin + 1 == sigEnd);
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

    bsl::string_view exponentStr(&*expBegin, value.end() - expBegin);

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

    Int64 exponent = (isExpNegative ? -1 : 1) * static_cast<Int64>(uExponent) +
                     exponentBias;

    // A flag indicating if the significant digits range includes a '.'.

    bool sigDigitsSeparated = sigBegin < intEnd && sigEnd > intEnd;

    Int64 numSigDigits = sigEnd - sigBegin - (sigDigitsSeparated  ? 1 : 0);

    // Return an error the number has too many digits (numSigDigits + zeroes
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


    bsl::string_view::const_iterator adjustedSigEnd = sigEnd;
    if (exponent < 0) {
        // This value will not be an integer, but we still must compute the
        // closest integer.  We truncate the fractional digits.

        adjustedSigEnd += exponent;

        //  If the significant digits previously had a separator, and we've
        //  adjusted the end of the significant digits past the separator, we
        //  need to substract one more to account for the separator.

        if (sigDigitsSeparated && adjustedSigEnd <= fracBegin) {
            adjustedSigEnd -= 1;
        }
    }

    // Compute the value of the remaining significant digits.  If there is a
    // decimal separator in the middle of the significant digits, we must do
    // this in two chunks.

    Uint64           tmp;
    bsl::string_view digits, moreDigits;
    if (sigBegin >= fracBegin || adjustedSigEnd <= intEnd) {
        // There isn't a decimal separator in the middle of the significant
        // digits, we have a single block of digits.

        BSLS_ASSERT(sigBegin <= intBegin || fracBegin != value.end());

        digits = bsl::string_view(&*sigBegin, adjustedSigEnd - sigBegin);
    }
    else {
        // There is a decimal separator, so there are two blocks of digits.

        digits     = bsl::string_view(&*sigBegin, intEnd - sigBegin);
        moreDigits = bsl::string_view(&*fracBegin, adjustedSigEnd - fracBegin);
        BSLS_ASSERT(0 != digits.size());
        BSLS_ASSERT(0 != moreDigits.size());
    }

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
        // value is UINT_MAX_VALUE plus some fraction, we prefer to report an
        // overflow.

        return (tmp == u::UINT64_MAX_VALUE) ? k_OVERFLOW : k_NOT_INTEGRAL;
                                                                      // RETURN
    }

    return 0;
}

bdldfp::Decimal64 NumberUtil::asDecimal64(const bsl::string_view&  value)
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

    bsl::string_view::const_iterator lend = l.d_significantDigitsEnd;
    bsl::string_view::const_iterator rend = r.d_significantDigitsEnd;
    bsl::string_view::const_iterator lIt  = l.d_significantDigitsBegin;
    bsl::string_view::const_iterator rIt  = r.d_significantDigitsBegin;

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

    if ('0' == *l.d_significantDigitsBegin) {
        // Both numbers are 0.

        return true;                                                  // RETURN
    }

    if (l.d_isNegative != r.d_isNegative) {
        return false;                                                 // RETURN
    }

    // The significant digits and sign of 'lhs' and 'rhs' are the same.  Next
    // we must test the canonical exponent value.  We start by computing the
    // exponents in the text of 'lhs' and 'rhs'.

    bsl::string_view lExp(&*l.d_exponentBegin, lhs.end() - l.d_exponentBegin);
    bsl::string_view rExp(&*r.d_exponentBegin, rhs.end() - r.d_exponentBegin);

    // If the exponents are too large to fit in int64 values, we fall back to
    // string equality.

    Uint64 lUExp, rUExp;

    int rc = ImpUtil::appendDigits(&lUExp, 0, lExp);
    if (0 != rc) {
        return compareNumberTextFallback(l, r);                       // RETURN
    }

    rc = ImpUtil::appendDigits(&rUExp, 0, rExp);
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
    bsl::string_view::const_iterator end = value.end();

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

        if (!bdlb::CharType::isDigit(*iter)) {
            // Require 1 digit after decimal.
            return false;                                             // RETURN
        }

        while (iter != end && bdlb::CharType::isDigit(*iter)) {
            ++iter;
        }
    }

    if ('e' == *iter || 'E' == *iter) {
        ++iter;

        if (end == iter) {
            return false;                                             // RETURN
        }

        if ('-' == *iter || '+' == *iter) {
            ++iter;
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

    bsl::string_view::const_iterator intBegin, intEnd, fracBegin, fracEnd,
        expBegin, sigBegin, sigEnd;

    ImpUtil::decompose(&isNeg,
                       &intBegin,
                       &intEnd,
                       &fracBegin,
                       &fracEnd,
                       &isExpNegative,
                       &expBegin,
                       &sigBegin,
                       &sigEnd,
                       &exponentBias,
                       value);

    if ('0' == *sigBegin) {
        // 0 is an integer regardless of how its represented.

        return true;                                                  // RETURN
    }

    // Compute the exponent.

    bsl::string_view exp(&*expBegin, value.end() - expBegin);

    Uint64 exponent;

    int rc = ImpUtil::appendDigits(&exponent, 0, exp);
    if (0 != rc) {
        // The exponent is more than UINT_MAX or less than -UINT_MAX 'value'
        // cannot have enough digits to matter.

        return !isExpNegative;                                        // RETURN
    }

    if (0 == exponent){
        isExpNegative = false;
    }

    if (isExpNegative) {
        Uint64 zeroesToSpare = (sigEnd < intEnd) ? intEnd - sigEnd : 0;
        return zeroesToSpare >= exponent;                             // RETURN
    }

    Uint64 zeroesNeeded = (sigEnd > fracBegin) ? sigEnd - fracBegin : 0;
    return exponent >= zeroesNeeded;
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
                      bool                             *isNegative,
                      bsl::string_view::const_iterator *integerBegin,
                      bsl::string_view::const_iterator *integerEnd,
                      bsl::string_view::const_iterator *fractionBegin,
                      bsl::string_view::const_iterator *fractionEnd,
                      bool                             *isExponentNegative,
                      bsl::string_view::const_iterator *exponentBegin,
                      bsl::string_view::const_iterator *significantDigitsBegin,
                      bsl::string_view::const_iterator *significantDigitsEnd,
                      bsls::Types::Int64               *significantDigitsBias,
                      const bsl::string_view&           value)
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

    bsl::string_view::const_iterator iter = value.begin();
    bsl::string_view::const_iterator end = value.end();

    *significantDigitsBegin = value.end();
    *fractionBegin          = value.end();
    *fractionEnd            = value.end();

    if ('-' == *iter) {
        *isNegative = true;
        ++iter;
    }
    else {
        *isNegative = false;
    }

    *integerBegin = iter;

    BSLS_ASSERT(iter != end);

    // We use this variable to keep track of the last non-zero digit (to
    // determine the significant digits).
    bsl::string_view::const_iterator lastNonZeroDigit = end;

    // Iterator over the integer digits.

    for (; iter != end && bdlb::CharType::isDigit(*iter); ++iter) {
        if ('0' != *iter) {
            if (*significantDigitsBegin == end) {
                *significantDigitsBegin = iter;
            }

            lastNonZeroDigit = iter;
        }
    }
    *integerEnd = iter;

    // Iterator over the fractional digits.

    if (iter != end && '.' == *iter) {
        ++iter;
        *fractionBegin = iter;
        for (; iter != end && bdlb::CharType::isDigit(*iter); ++iter) {
            if ('0' != *iter) {
                if (*significantDigitsBegin == end) {
                    *significantDigitsBegin = iter;
                }
                lastNonZeroDigit = iter;
            }
        }
        *fractionEnd = iter;
    }
    else {
        // Even if there is no fractional part, we locate the iterators before
        // the exponentBegin.  This is contractually required, and an important
        // property to maintain (otherwise, for example,
        // 'significantDigitsEnd <= fractionBegin' would incorrectly report
        // 'false' for "1e1").

        *fractionBegin = iter;
        *fractionEnd   = iter;
    }

    if (iter != end && 'E' == static_cast<char>(bsl::toupper(*iter))) {
        ++iter;
        if ('-' == *iter) {
            *isExponentNegative = true;
            ++iter;
        }
        else {
            *isExponentNegative = false;
            if ('+' == *iter) {
                ++iter;
            }
        }
        *exponentBegin = iter;
    } else {
        *isExponentNegative = false;
        *exponentBegin = end;
    }

    if (*significantDigitsBegin == end) {
        // The value 0.
        BSLS_ASSERT(**integerBegin == '0');
        *significantDigitsBegin = *integerBegin;
        *significantDigitsEnd   = *integerEnd;
        *significantDigitsBias  = 0;
    }
    else {
        // Otherwise we need to set 'significantDigitsEnd' to one-past the
        // last non-zero digit, and compute the bias.

        *significantDigitsEnd = lastNonZeroDigit + 1;
        if (*significantDigitsEnd <= *integerEnd) {
            // The significant digits are before the decimal separator, add a
            // positive bias equal to the number of trailing 0s.

            *significantDigitsBias = *integerEnd - *significantDigitsEnd;
        }
        else {
            // There are significant digits after the decimal separator, add a
            // bias for the number of fraction digits.

            BSLS_ASSERT(lastNonZeroDigit >= *fractionBegin);
            *significantDigitsBias = -1 *
                                     (*significantDigitsEnd - *fractionBegin);
        }
    }
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
