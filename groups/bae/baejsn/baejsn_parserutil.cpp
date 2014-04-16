// baejsn_parserutil.cpp                                              -*-C++-*-
#include <baejsn_parserutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_parserutil_cpp,"$Id$ $CSID$")

#include <bdema_bufferedsequentialallocator.h>

#include <bdede_base64decoder.h>
#include <bdede_charconvertutf32.h>

#include <bdeu_chartype.h>

#include <bsls_alignedbuffer.h>

#include <bsl_algorithm.h>
#include <bsl_cmath.h>
#include <bsl_cctype.h>
#include <bsl_cerrno.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace {

inline
bool isValidNextChar(int nextChar)
    // Return 'true' if the specified 'nextChar' refers to a valid next
    // character and 'false' otherwise.
{
    return bsl::streambuf::traits_type::eof() == nextChar
        || bsl::isspace(nextChar)
        || ',' == static_cast<char>(nextChar)
        || ']' == static_cast<char>(nextChar)
        || '}' == static_cast<char>(nextChar);
}

const bsls::Types::Uint64 UINT64_MAX_DIVIDED_BY_10 =
                                              static_cast<bsls::Types::Uint64>(
                         bsl::numeric_limits<bsls::Types::Uint64>::max() / 10);
const bsls::Types::Uint64 UINT64_MAX_LAST_DIGIT    = 5;

}  // close unnamed namespace

                            // ------------------------
                            // struct baejsn_ParserUtil
                            // ------------------------

// CLASS METHODS
int baejsn_ParserUtil::getString(bsl::string *value, bslstl::StringRef data)
{
    const char *iter = data.begin();
    const char *end  = data.end();

    if (iter == end || '"' != *iter) {
        return -1;                                                    // RETURN
    }

    value->clear();

    ++iter;
    while (iter < end) {
        if ('\\' == *iter) {
            ++iter;
            if (iter >= end) {
                return -1;                                            // RETURN
            }

            switch (*iter) {
              case 'b': {
                *value += '\b';
              } break;
              case 'f': {
                *value += '\f';
              } break;
              case 'n': {
                *value += '\n';
              } break;
              case 'r': {
                *value += '\r';
              } break;
              case 't': {
                *value += '\t';
              } break;
              case '"'  :                                       // FALL THROUGH
              case '\\' :                                       // FALL THROUGH
              case '/'  : {

                  // printable characters

                *value += *iter;
              } break;
              case 'u':
              case 'U': {

                enum { NUM_UNICODE_DIGITS = 4 };

                if (iter + NUM_UNICODE_DIGITS >= end) {
                    return -1;                                        // RETURN
                }

                ++iter;

                char tmp[NUM_UNICODE_DIGITS + 1];
                bsl::strncpy(tmp, iter, NUM_UNICODE_DIGITS);
                tmp[NUM_UNICODE_DIGITS] = '\0';

                char         *end = 0;
                unsigned int  utf32input[2] = { 0 };

                utf32input[0] = static_cast<unsigned int>(
                                                   bsl::strtol(tmp, &end, 16));

                if (end == tmp || *end != '\0') {
                    return -1;                                        // RETURN
                }

                // Confirm that only the lower two bytes are set.

                BSLS_ASSERT(0 == (utf32input[0] & 0xFF000000)
                         && 0 == (utf32input[0] & 0x00FF0000));

                // Due to the short string optimization there won't be a
                // memory allocation here.

                bsl::string utf8String;
                const int rc = bdede_CharConvertUtf32::utf32ToUtf8(&utf8String,
                                                                   utf32input);

                if (rc) {
                    return rc;                                        // RETURN
                }

                value->append(utf8String);

                // 'iter' is not increased by 4 because 'iter' is incremented
                // at the end of the function.

                iter += 3;
              } break;
              default: {
                return -1;                                            // RETURN
              } break;
            }
        }
        else if ('"' == *iter) {
            return 0;                                                 // RETURN
        }
        else {
            *value += *iter;
        }
        ++iter;
    }

    return -1;
}

int baejsn_ParserUtil::getValue(double *value, bslstl::StringRef data)
{
    if (0 == data.length()
     || '.' == data[0]
     || '+' == data[0]
     || data.length() > 1 && '-' == data[0] && '.' == data[1]) {
        return -1;                                                    // RETURN
    }

    const int MAX_STRING_LENGTH = 63;
    char      buffer[MAX_STRING_LENGTH + 1];

    bdema_BufferedSequentialAllocator allocator(buffer, MAX_STRING_LENGTH + 1);
    bsl::string                       dataString(data.data(),
                                                 data.length(),
                                                 &allocator);

    char   *endPtr = 0;
    errno          = 0;
    double  tmp    = bsl::strtod(dataString.c_str(), &endPtr);

    if (endPtr    != dataString.end()
     || (0        == tmp && 0 != errno)
     ||  HUGE_VAL == tmp
     || -HUGE_VAL == tmp
     || !bsl::isdigit(*(dataString.end() - 1))) {
        return -1;                                                    // RETURN
    }

    *value = tmp;
    return 0;
}

int baejsn_ParserUtil::getUint64(bsls::Types::Uint64 *value,
                                 bslstl::StringRef    data)
{
    const char *iter  = data.begin();
    const char *end   = data.end();

    // Extract leading digits and store their range in [valueBegin..valueEnd)

    const char *valueBegin = iter;
    while (iter < end && bsl::isdigit(*iter)) {
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
        while (iter < end && bsl::isdigit(*iter)) {
            ++iter;
        }
        fractionalEnd = iter;
    }

    // Extract exponent digits if available and store the unsigned integer
    // part in 'exponent' and the sign in 'isExpNegative'.

    int  exponent = 0;
    bool isExpNegative = false;
    if ('E' == static_cast<char>(bsl::toupper(*iter))) {

        ++iter;
        if ('-' == *iter) {
            isExpNegative = true;
            ++iter;
        }
        else {
            if ('+' == *iter) {
                ++iter;
            }
        }

        while (iter < end && bsl::isdigit(*iter)) {
            exponent = exponent * 10 + *iter - '0';
            ++iter;
        }
    }

    if (iter < end && !bsl::isdigit(*iter)) {
        return -1;                                                    // RETURN
    }

    // Based on the value of 'exponent' update the range value digits range,
    // [valueBegin..valueEnd).

    int numFractionalDigits = fractionalEnd - fractionalBegin;
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
        // Expand the value digits range.  If there are fractional
        // digits, coalesce them into the value range by updating
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
        const int digitValue = *iter - '0';
        if (tmp < UINT64_MAX_DIVIDED_BY_10
         || (UINT64_MAX_DIVIDED_BY_10 == tmp
          && digitValue <= UINT64_MAX_LAST_DIGIT)) {
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
            const int digitValue = *iter - '0';
            if (tmp < UINT64_MAX_DIVIDED_BY_10
             || (UINT64_MAX_DIVIDED_BY_10 == tmp
              && digitValue <= UINT64_MAX_LAST_DIGIT)) {
                tmp = tmp * 10 + digitValue;
            }
            else {
                return -1;                                            // RETURN
            }
        }
        fractionalBegin = iter;
    }

    // Disallow non-zero fractional digits.

    while (fractionalBegin < fractionalEnd) {
        bool decimalFound = false;
        if ('0' != *fractionalBegin) {
            if ('.' == *fractionalBegin && !decimalFound) {
                decimalFound = true;
                ++fractionalBegin;
                continue;
            }
            return -1;                                                // RETURN
        }
        ++fractionalBegin;
    }

    if (exponent) {
        const double exponentMultiple = bsl::pow(10.0, exponent);
        if (static_cast<double>(tmp * exponentMultiple <
                                         bsl::numeric_limits<double>::max())) {
            tmp *= static_cast<bsls::Types::Uint64>(exponentMultiple);
        }
        else {
            return -1;                                                // RETURN
        }
    }

    *value = tmp;
    return 0;
}

int baejsn_ParserUtil::getValue(bool *value, bslstl::StringRef data)
{
    enum { BAEJSN_TRUE_LENGTH = 4, BAEJSN_FALSE_LENGTH = 5 };

    if (BAEJSN_TRUE_LENGTH == data.length()
     && 0                  == bsl::strncmp("true",
                                           data.data(),
                                           BAEJSN_TRUE_LENGTH)) {
        *value = true;
    }
    else if (BAEJSN_FALSE_LENGTH == data.length()
          && 0                   == bsl::strncmp("false",
                                                 data.data(),
                                                 BAEJSN_FALSE_LENGTH)) {
        *value = false;
    }
    else {
        return -1;                                                    // RETURN
    }
    return 0;
}

int baejsn_ParserUtil::getValue(bsl::vector<char> *value,
                                bslstl::StringRef  data)
{
    const int MAX_LENGTH = 1024;
    bsls::AlignedBuffer<MAX_LENGTH> buffer;

    bdema_BufferedSequentialAllocator allocator(buffer.buffer(), MAX_LENGTH);
    bsl::string base64String(&allocator);

    int rc = getValue(&base64String, data);
    if (rc) {
        return -1;                                                    // RETURN
    }

    value->clear();

    bdede_Base64Decoder base64Decoder(true);
    bsl::back_insert_iterator<bsl::vector<char> > outputIterator(*value);

    rc = base64Decoder.convert(outputIterator,
                               base64String.begin(),
                               base64String.end());

    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    rc = base64Decoder.endConvert(outputIterator);

    if (rc < 0) {
        return rc;                                                    // RETURN
    }

    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
