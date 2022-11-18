// baljsn_parserutil.cpp                                              -*-C++-*-
#include <baljsn_parserutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_parserutil_cpp,"$Id$ $CSID$")

#include <bdlma_bufferedsequentialallocator.h>

#include <bdlde_base64decoder.h>
#include <bdlde_charconvertutf32.h>

#include <bdlb_chartype.h>
#include <bdlb_numericparseutil.h>
#include <bdlb_string.h>

#include <bdldfp_decimalutil.h>

#include <bsls_alignedbuffer.h>
#include <bsls_annotation.h>
#include <bsls_assert.h>
#include <bsls_libraryfeatures.h>

#include <bsl_algorithm.h>
#include <bsl_iterator.h>
#include <bsl_cmath.h>
#include <bsl_cctype.h>
#include <bsl_cerrno.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_limits.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR)
# include <memory_resource>
#endif

namespace {
namespace u {

using namespace BloombergLP;

template <class TYPE>
int loadInfOrNan(TYPE *value, const bsl::string_view& data)
{
    const int NO_SIGN_NAN_OR_INF_STRING_LEN   =  5;
    const int INF_OR_NAN_WITH_SIGN_STRING_LEN =  6;
    const int INFINITY_NO_SIGN_STRING_LEN     = 10;
    const int INFINITY_WITH_SIGN_STRING_LEN   = 11;

    int rc = -1;

    switch (data.length()) {
      case NO_SIGN_NAN_OR_INF_STRING_LEN: {
        if (bdlb::String::areEqualCaseless("\"nan\"",
                                          NO_SIGN_NAN_OR_INF_STRING_LEN,
                                          data.data(),
                                          NO_SIGN_NAN_OR_INF_STRING_LEN)) {
            *value = bsl::numeric_limits<TYPE>::quiet_NaN();
            rc = 0;
        }
        else if (bdlb::String::areEqualCaseless(
                                              "\"inf\"",
                                              NO_SIGN_NAN_OR_INF_STRING_LEN,
                                              data.data(),
                                              NO_SIGN_NAN_OR_INF_STRING_LEN)) {
            *value = bsl::numeric_limits<TYPE>::infinity();
            rc = 0;
        }
      } break;
      case INF_OR_NAN_WITH_SIGN_STRING_LEN: {
        if (bdlb::String::areEqualCaseless("\"+inf\"",
                                          INF_OR_NAN_WITH_SIGN_STRING_LEN,
                                          data.data(),
                                          INF_OR_NAN_WITH_SIGN_STRING_LEN)) {
            *value = bsl::numeric_limits<TYPE>::infinity();
            rc = 0;
        }
        else if (bdlb::String::areEqualCaseless(
                                            "\"-inf\"",
                                            INF_OR_NAN_WITH_SIGN_STRING_LEN,
                                            data.data(),
                                            INF_OR_NAN_WITH_SIGN_STRING_LEN)) {
            *value = -bsl::numeric_limits<TYPE>::infinity();
            rc = 0;
        }
        else if (bdlb::String::areEqualCaseless(
                                            "\"+nan\"",
                                            INF_OR_NAN_WITH_SIGN_STRING_LEN,
                                            data.data(),
                                            INF_OR_NAN_WITH_SIGN_STRING_LEN)) {

            *value = bsl::numeric_limits<TYPE>::quiet_NaN();
            rc = 0;
        }
        else if (bdlb::String::areEqualCaseless(
                                            "\"-nan\"",
                                            INF_OR_NAN_WITH_SIGN_STRING_LEN,
                                            data.data(),
                                            INF_OR_NAN_WITH_SIGN_STRING_LEN)) {

            *value = -bsl::numeric_limits<TYPE>::quiet_NaN();
            rc = 0;
        }
      } break;
      case INFINITY_NO_SIGN_STRING_LEN: {
        if (bdlb::String::areEqualCaseless("\"infinity\"",
                                          INFINITY_NO_SIGN_STRING_LEN,
                                          data.data(),
                                          INFINITY_NO_SIGN_STRING_LEN)) {
            *value = bsl::numeric_limits<TYPE>::infinity();
            rc = 0;
        }
      } break;
      case INFINITY_WITH_SIGN_STRING_LEN: {
        if (bdlb::String::areEqualCaseless("\"+infinity\"",
                                          INFINITY_WITH_SIGN_STRING_LEN,
                                          data.data(),
                                          INFINITY_WITH_SIGN_STRING_LEN)) {
            *value = bsl::numeric_limits<TYPE>::infinity();
            rc = 0;
        }
        else if (bdlb::String::areEqualCaseless(
                                              "\"-infinity\"",
                                              INFINITY_WITH_SIGN_STRING_LEN,
                                              data.data(),
                                              INFINITY_WITH_SIGN_STRING_LEN)) {
            *value = -bsl::numeric_limits<TYPE>::infinity();
            rc = 0;
        }
      } break;
    }

    return rc;
}

static const bsls::Types::Uint64 UINT64_MAX_VALUE =
                               bsl::numeric_limits<bsls::Types::Uint64>::max();
static const bsls::Types::Uint64 UINT64_MAX_DIVIDED_BY_10 =
                                                         UINT64_MAX_VALUE / 10;
static const bsls::Types::Uint64 UINT64_MAX_DIVIDED_BY_10_TO_THE_10 =
                                             UINT64_MAX_VALUE / 10000000000ULL;
static const bsls::Types::Uint64 UINT64_MAX_VALUE_LAST_DIGIT = 5;

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace baljsn {

                             // -----------------
                             // struct ParserUtil
                             // -----------------

// CLASS METHODS
int ParserUtil::getUnquotedString(bsl::string             *value,
                                  const bsl::string_view&  data)
{
    return bdljsn::StringUtil::readUnquotedString(
                          value,
                          data,
                          bdljsn::StringUtil::e_ACCEPT_CAPITAL_UNICODE_ESCAPE);
}

int ParserUtil::getValue(bool *value, const bsl::string_view& data)
{
    enum { k_TRUE_LENGTH = 4, k_FALSE_LENGTH = 5 };

    if (k_TRUE_LENGTH == data.length()
     && 0                  == bsl::strncmp("true",
                                           data.data(),
                                           k_TRUE_LENGTH)) {
        *value = true;
    }
    else if (k_FALSE_LENGTH == data.length()
          && 0              == bsl::strncmp("false",
                                            data.data(),
                                            k_FALSE_LENGTH)) {
        *value = false;
    }
    else {
        return -1;                                                    // RETURN
    }
    return 0;
}

int ParserUtil::getValue(bdldfp::Decimal64       *value,
                         const bsl::string_view&  data)
{
    if (0 == data.length()) {
        return -1;                                                    // RETURN
    }

    const int k_MAX_STRING_LENGTH = 32;
        // The size of the string sufficient to store 'bdldfp::Decimal64'
        // values.

    char                               buffer[k_MAX_STRING_LENGTH + 1];
    bdlma::BufferedSequentialAllocator allocator(buffer,
                                                 k_MAX_STRING_LENGTH + 1);
    bsl::string dataString(&allocator);

    if ('"' == data[0]) {
        if (3 > data.length() || '"' != data[data.length() - 1]) {
            return -1;                                                // RETURN
        }
        dataString.append(data.data() + 1, data.length() - 2);
    } else {
        dataString.append(data.data(), data.length());
    }

    bdldfp::Decimal64 d;
    int rc = bdldfp::DecimalUtil::parseDecimal64(&d, dataString);
        // Note that 'bdldfp::DecimalUtil::parseDecimal' does not parse signed
        // 'nan' values.

    if (0 != rc) {
        if (6    == data.length()                    &&
            '\"' == data[0]                          &&
            'n'  == bdlb::CharType::toLower(data[2]) &&
            'a'  == bdlb::CharType::toLower(data[3]) &&
            'n'  == bdlb::CharType::toLower(data[4]) &&
            '\"' == data[5]) {

            if ('-'  == data[1]) {
                *value = -bsl::numeric_limits<bdldfp::Decimal64>::quiet_NaN();
                return 0;                                             // RETURN
            }
            else if ('+'  == data[1]) {
                *value = bsl::numeric_limits<bdldfp::Decimal64>::quiet_NaN();
                return 0;                                             // RETURN
            }
        }
        return -1;                                                    // RETURN
    }

    *value = d;
    return 0;
}

int ParserUtil::getValue(double *value, const bsl::string_view& data)
{
    if (0 == data.length()
     || '.' == data[0]
     || '+' == data[0]
     || (data.length() > 1 && '-' == data[0] && '.' == data[1])) {
        return -1;                                                    // RETURN
    }

    if ('"' == data[0]) {
       // Parse "NaN", "+INF" and "-INF" as floating point values.  Note that
       // these values are encoded as strings in a JSON standard unconformant
       // way.

        return u::loadInfOrNan(value, data);                          // RETURN
    }

    const int k_MAX_STRING_LENGTH = 63;
    char      buffer[k_MAX_STRING_LENGTH + 1];

    bdlma::BufferedSequentialAllocator allocator(buffer,
                                                 k_MAX_STRING_LENGTH + 1);
    bsl::string                        dataString(data.data(),
                                                  data.length(),
                                                  &allocator);

    char   *endPtr = 0;
    errno          = 0;
    double  tmp    = bsl::strtod(dataString.c_str(), &endPtr);

    if (endPtr    != dataString.end()
     || (0        == tmp && 0 != errno)
     ||  HUGE_VAL == tmp
     || -HUGE_VAL == tmp
     || !bdlb::CharType::isDigit(*(dataString.end() - 1))) {
        return -1;                                                    // RETURN
    }

    *value = tmp;
    return 0;
}

int ParserUtil::getUint64(bsls::Types::Uint64     *value,
                          const bsl::string_view&  data)
{
    const char *iter = data.data();
    const char *end  = data.data() + data.length();

    // Extract leading digits and store their range in [valueBegin..valueEnd)

    const char *valueBegin = iter;
    while (iter < end && bdlb::CharType::isDigit(*iter)) {
        ++iter;
    }
    const char *valueEnd = iter;

    if (valueBegin == valueEnd) {
        return -1;                                                    // RETURN
    }

    // Extract fractional digits if available and store their range in
    // [fractionalBegin..fractionalEnd).

    const char *fractionalBegin = 0;
    const char *fractionalEnd   = 0;
    if (iter < end && '.' == *iter) {

        fractionalBegin = ++iter;
        while (iter < end && bdlb::CharType::isDigit(*iter)) {
            ++iter;
        }
        fractionalEnd = iter;
    }

    // Extract exponent digits if available and store the unsigned integer part
    // in 'exponent' and the sign in 'isExpNegative'.

    int  exponent = 0;
    bool isExpNegative = false;
    if (iter < end && 'E' == static_cast<char>(bsl::toupper(*iter))) {
        if (++iter == end) {
            return -1;                                                // RETURN
        }
        if ('-' == *iter) {
            isExpNegative = true;
            if (++iter == end) {
                return -1;                                            // RETURN
            }
        }
        else if ('+' == *iter) {
            if (++iter == end) {
                return -1;                                            // RETURN
            }
        }

        while (iter < end && bdlb::CharType::isDigit(*iter)) {
            if (exponent > bsl::numeric_limits<int>::max() / 10) {
                return -1;                                            // RETURN
            }
            exponent = exponent * 10 + *iter - '0';
            ++iter;
        }
    }

    if (iter < end && !bdlb::CharType::isDigit(*iter)) {
        return -1;                                                    // RETURN
    }

    // Based on the value of 'exponent' update the range value digits range,
    // [valueBegin..valueEnd).

    int numFractionalDigits = static_cast<int>(fractionalEnd
                                                            - fractionalBegin);
    int numAdditionalDigits = 0;
    if (isExpNegative) {
        // Shrink the value digits range by 'exponent'.  The fractional digits
        // range is appropriately updated.

        if (valueEnd - valueBegin >= exponent) {
            valueEnd        -= exponent;
            fractionalBegin  = valueEnd;

            if (0 == numFractionalDigits) {
                fractionalEnd = fractionalBegin + exponent;
            }

            exponent = 0;
        }
        else {
            return -1;                                                // RETURN
        }
    }
    else {
        // Expand the value digits range.  If there are fractional digits,
        // coalesce them into the value range by updating
        // 'numAdditionalDigits'.  Update 'exponent' after the operation.

        if (numFractionalDigits > exponent) {
            numAdditionalDigits  = exponent;
            exponent             = 0;
        }
        else {
            numAdditionalDigits  = numFractionalDigits;
            exponent            -= numFractionalDigits;
        }
    }

    bsls::Types::Uint64 tmp = 0;

    // Update a copy of the result by parsing the value digits.

    iter = valueBegin;
    while (iter != valueEnd) {
        const unsigned digitValue = *iter - '0';
        if (tmp < u::UINT64_MAX_DIVIDED_BY_10
         || (u::UINT64_MAX_DIVIDED_BY_10 == tmp
          && digitValue <= u::UINT64_MAX_VALUE_LAST_DIGIT)) {
            tmp = tmp * 10 + digitValue;
            ++iter;
        }
        else {
            return -1;                                                // RETURN
        }
    }

    // Update a copy of the result by parsing the fractional digits if any need
    // to be appended.

    if (numAdditionalDigits) {

        iter = fractionalBegin;
        for (int i = 0; i < numAdditionalDigits; ++i, ++iter) {
            const unsigned digitValue = *iter - '0';
            if (tmp < u::UINT64_MAX_DIVIDED_BY_10
             || (u::UINT64_MAX_DIVIDED_BY_10 == tmp
              && digitValue <= u::UINT64_MAX_VALUE_LAST_DIGIT)) {
                tmp = tmp * 10 + digitValue;
            }
            else {
                return -1;                                            // RETURN
            }
        }
        fractionalBegin = iter;
    }

    // Disallow non-zero fractional digits.

    bool decimalFound = false;
    for (; fractionalBegin < fractionalEnd; ++fractionalBegin) {
        if ('0' != *fractionalBegin) {
            if ('.' == *fractionalBegin && !decimalFound) {
                decimalFound = true;
                continue;
            }
            return -1;                                                // RETURN
        }
    }

    if (exponent) {
        if (exponent > 19) {
            return -1;                                                // RETURN
        }
        if (exponent >= 10) {
            if (tmp > u::UINT64_MAX_DIVIDED_BY_10_TO_THE_10) {
                return -1;                                            // RETURN
            }
            tmp *= 10000000000ULL;
            exponent -= 10;
        }
        BSLS_ASSERT(exponent <= 9);

        // If 'exponent >= 0, the mantissa in 'double' can now represent
        // 'exponentMultiple' exactly because 'exponent <= 9'.  If
        // 'exponent < 0', we don't have to be exact.

        const double exponentMultiple = bsl::pow(10.0, exponent);
        if (exponent < 0) {
            if (exponent < -19) {
                tmp = 0;
            }
            else {
                const bsls::Types::Uint64 uExponentDivisor =
                    static_cast<bsls::Types::Uint64>(0.1 + 1/exponentMultiple);
                tmp /= uExponentDivisor;
            }
        }
        else {
            const bsls::Types::Uint64 uExponentMultiple =
                      static_cast<bsls::Types::Uint64>(exponentMultiple + 0.1);
            if (u::UINT64_MAX_VALUE / uExponentMultiple >= tmp) {
                tmp *= uExponentMultiple;
            }
            else {
                return -1;                                            // RETURN
            }
        }
    }

    *value = tmp;
    return 0;
}

int ParserUtil::getValue(bsl::vector<char>       *value,
                         const bsl::string_view&  data)
{
    BSLS_ASSERT(value);

    const int                       MAX_LENGTH = 1024;
    bsls::AlignedBuffer<MAX_LENGTH> buffer;

    bdlma::BufferedSequentialAllocator allocator(buffer.buffer(), MAX_LENGTH);
    bsl::string                        base64String(&allocator);

    int rc = bdljsn::StringUtil::readString(
                          &base64String,
                          data,
                          bdljsn::StringUtil::e_ACCEPT_CAPITAL_UNICODE_ESCAPE);
    if (rc) {
        return -1;                                                    // RETURN
    }

    value->clear();

    bdlde::Base64Decoder base64Decoder(true);
    int                  length = static_cast<int>(base64String.length());

    value->resize(static_cast<bsl::size_t>(
                              bdlde::Base64Decoder::maxDecodedLength(length)));

    rc = base64Decoder.convert(value->begin(),
                               base64String.begin(),
                               base64String.end());

    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    rc = base64Decoder.endConvert(value->begin() +
                                  base64Decoder.outputLength());

    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    value->resize(static_cast<bsl::size_t>(base64Decoder.outputLength()));

    return 0;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
