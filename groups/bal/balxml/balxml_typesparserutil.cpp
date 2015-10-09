// balxml_typesparserutil.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_typesparserutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_typesparserutil_cpp,"$Id$ $CSID$")

#include <balxml_typesprintutil.h>  // for testing only

#include <balxml_base64parser.h>
#include <balxml_hexparser.h>

#include <bdlsb_fixedmeminstreambuf.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_cerrno.h>
#include <bsl_cfloat.h>
#include <bsl_cmath.h>

namespace BloombergLP {

namespace {

// HELPER FUNCTIONS

int parseBoolean(bool *result, const char *input, int inputLength)
    // Set the specified '*result' to true if the specified 'input' of
    // specified length 'inputLength' is "1" or "true" and false if 'input' is
    // "0" or "false".  Strings are case-insensitive.  Return 0 on success and
    // non-zero if 'input' is not "1", "0", "true", or "false".
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    switch (inputLength) {
      case 1: {
        if ('1' == input[0]) {
            *result = true;
            return BAEXML_SUCCESS;                                    // RETURN
        }
        else if ('0' == input[0]) {
            *result = false;
            return BAEXML_SUCCESS;                                    // RETURN
        }
      } break;
      case 4: {
        if (('t' == input[0] || 'T' == input[0])
         && ('r' == input[1] || 'R' == input[1])
         && ('u' == input[2] || 'U' == input[2])
         && ('e' == input[3] || 'E' == input[3])) {
            *result = true;
            return BAEXML_SUCCESS;                                    // RETURN
        }
      } break;
      case 5: {
        if (('f' == input[0] || 'F' == input[0])
         && ('a' == input[1] || 'A' == input[1])
         && ('l' == input[2] || 'L' == input[2])
         && ('s' == input[3] || 'S' == input[3])
         && ('e' == input[4] || 'E' == input[4])) {
            *result = false;
            return BAEXML_SUCCESS;                                    // RETURN
        }
      } break;
    }

    return BAEXML_FAILURE;
}

int parseDoubleImpl(double *result, const char *input, bool formatDecimal)
    // Parse a string representing a double into the specified 'result'.  The
    // specified 'input' is a null-terminated string.  The specified
    // 'formatDecimal' should be true if 'input' should contain only decimal
    // digits, period and sign characters (i.e., INF/NaN, and exponential
    // notation are not allowed); otherwise 'input' can contain any floating-
    // point representation form.  Return 0 on success and non-zero otherwise.
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };
    static const char decimalChars[] = "+-.0123456789";

    if (formatDecimal && input[bsl::strspn(input, decimalChars)] != '\0') {
        // Non-decimal character (i.e., potential INF, NaN, or exponent) found.
        return BAEXML_FAILURE;                                        // RETURN
    }

    switch (input[0]) {
      case 'N':
        if (bsl::strcmp(input + 1, "aN") == 0) {
            *result = bsl::numeric_limits<double>::quiet_NaN();
            return BAEXML_SUCCESS;                                    // RETURN
        }
        break;
      case 'I':
        if (bsl::strcmp(input + 1, "NF") == 0) {
            *result = bsl::numeric_limits<double>::infinity();
            return BAEXML_SUCCESS;                                    // RETURN
        }
        break;
      case '+':
        if (bsl::strcmp(input + 1, "INF") == 0) {
            *result = bsl::numeric_limits<double>::infinity();
            return BAEXML_SUCCESS;                                    // RETURN
        }
        break;
      case '-':
        if (bsl::strcmp(input + 1, "INF") == 0) {
            *result = -bsl::numeric_limits<double>::infinity();
            return BAEXML_SUCCESS;                                    // RETURN
        }
        break;

      default:
        break;
    } // End switch

    char *end = 0;
    errno = 0;
    *result = bsl::strtod(input, &end);

    if (end == input || *end != '\0') {  // nothing was consumed or not all
                                         // characters were consumed
        return BAEXML_FAILURE;                                        // RETURN
    }

    // We ignore underflow errors.  If strtod sets errno to ERANGE, then it
    // will also have set the result to a very large number for overflow or a
    // very small number (or zero) for underflow -- exactly what we want.
    if (ERANGE == errno) {
        if (*result < -1.0 || 1.0 < *result) {
            // Overflow is an error.
            return BAEXML_FAILURE;                                    // RETURN
        }
        else {
            // Underflow is OK (very small number).
            return BAEXML_SUCCESS;                                    // RETURN
        }
    }
    else if (errno != 0) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    return BAEXML_SUCCESS;
}

int parseDouble(double     *result,
                const char *input,
                int         inputLength,
                bool        formatDecimal)
    // Parse a string representing a double into the specified 'result'.  The
    // specified 'formatDecimal' will be true if the specified 'input' of
    // specified length 'inputLength' should contain only decimal digits,
    // period and sign characters; otherwise 'input' can contain any floating-
    // point representation form.  Return 0 on success and non-zero otherwise.
{
    // 'input' must be zero terminated string
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };
    enum { BUFLEN = 80 };

    if (0 == inputLength) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    if (inputLength < BUFLEN) {
        // Use a fixed-length buffer for efficiency.
        char  buffer[BUFLEN];
        bsl::memcpy(buffer, input, inputLength);
        buffer[inputLength] = '\0';

        return parseDoubleImpl(result, buffer, formatDecimal);        // RETURN

    }
    else {
        // Use a string for dynamic allocation.
        bsl::string tmp(input, inputLength);
        return parseDoubleImpl(result, tmp.c_str(), formatDecimal);   // RETURN
    }
}

int parseInt(int *result, const char *input, int inputLength)
    // Parse an unsigned long decimal string
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };
    enum { BUFLEN = 80 };

    int consumed = 0;

    if (0 == inputLength) {
        return BAEXML_FAILURE;                                        // RETURN
    }
    else if (inputLength < BUFLEN) {
        // Use a fixed-length buffer for efficiency.
        char  buffer[BUFLEN];
        bsl::memcpy(buffer, input, inputLength);
        buffer[inputLength] = '\0';

        errno = 0;
        char *end = 0;
        *result = (int) bsl::strtol(buffer, &end, 10);
        consumed = end - buffer;
    }
    else {
        // Use a string for dynamic allocation.
        bsl::string tmp(input, inputLength);
        const char *begin = tmp.c_str();

        errno = 0;
        char *end = 0;
        *result = (int) bsl::strtol(begin, &end, 10);
        consumed = end - begin;
    }

    if (errno != 0 || consumed != inputLength) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    return BAEXML_SUCCESS;
}

int parseUnsignedInt(unsigned int *result, const char *input, int inputLength)
    // Parse an unsigned long decimal string
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };
    enum { BUFLEN = 80 };

    int consumed = 0;

    if (0 == inputLength) {
        return BAEXML_FAILURE;                                        // RETURN
    }
    else if (inputLength < BUFLEN) {
        // Use a fixed-length buffer for efficiency.
        char  buffer[BUFLEN];
        bsl::memcpy(buffer, input, inputLength);
        buffer[inputLength] = '\0';

        errno = 0;
        char *end = 0;
        *result = (int) bsl::strtoul(buffer, &end, 10);
        consumed = end - buffer;
    }
    else {
        // Use a string for dynamic allocation.
        bsl::string tmp(input, inputLength);
        const char *begin = tmp.c_str();

        errno = 0;
        char *end = 0;
        *result = (unsigned int) bsl::strtoul(begin, &end, 10);
        consumed = end - begin;
    }

    if (errno != 0 || consumed != inputLength) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    return BAEXML_SUCCESS;
}

template <class INT_TYPE>
inline
int parseSignedDecimal(INT_TYPE *result, const char *input, int inputLength)
    // Parse a signed integral value as decimal.
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    int temp;

    if (0 != parseInt(&temp, input, inputLength) ||
        temp < bsl::numeric_limits<INT_TYPE>::min() ||
        temp > bsl::numeric_limits<INT_TYPE>::max()) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    *result = static_cast<INT_TYPE>(temp);
    return BAEXML_SUCCESS;
}

template <class INT_TYPE>
inline
int parseUnsignedDecimal(INT_TYPE *result, const char *input, int inputLength)
    // Parse an unsigned integral value as decimal.
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    unsigned int temp;

    if (0 != parseUnsignedInt(&temp, input, inputLength) ||
        temp > bsl::numeric_limits<INT_TYPE>::max()) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    *result = static_cast<INT_TYPE>(temp);
    return BAEXML_SUCCESS;
}

}  // close unnamed namespace

namespace balxml {
                         // --------------------------
                         // struct TypesParserUtil_Imp
                         // --------------------------

// BASE64 FUNCTIONS

int TypesParserUtil_Imp::parseBase64(bsl::string                *result,
                                     const char                 *input,
                                     int                         inputLength,
                                     bdlat_TypeCategory::Simple)
{
    enum { BAEXML_FAILURE = -1 };

    Base64Parser<bsl::string> base64Parser;

    if (0 != base64Parser.beginParse(result)) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    if (0 != base64Parser.pushCharacters(input, input + inputLength)) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    return base64Parser.endParse();
}

int TypesParserUtil_Imp::parseBase64(bsl::vector<char>         *result,
                                     const char                *input,
                                     int                        inputLength,
                                     bdlat_TypeCategory::Array)
{
    enum { BAEXML_FAILURE = -1 };

    Base64Parser<bsl::vector<char> > base64Parser;

    if (0 != base64Parser.beginParse(result)) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    if (0 != base64Parser.pushCharacters(input, input + inputLength)) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    return base64Parser.endParse();
}

// DECIMAL FUNCTIONS

int TypesParserUtil_Imp::parseDecimal(bool                       *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseBoolean(result, input, inputLength);
}

int TypesParserUtil_Imp::parseDecimal(char                       *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseSignedDecimal((signed char*) result,
                              input, inputLength);
}

int TypesParserUtil_Imp::parseDecimal(short                      *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseSignedDecimal(result, input, inputLength);
}

int TypesParserUtil_Imp::parseDecimal(int                        *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseSignedDecimal(result, input, inputLength);
}

int
TypesParserUtil_Imp::parseDecimal(bsls::Types::Int64         *result,
                                  const char                 *input,
                                  int                         inputLength,
                                  bdlat_TypeCategory::Simple  sc)
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    int sign = 0;
    if (inputLength > 0 && '-' == input[0]) {
        sign = -1;
        ++input;
        --inputLength;
    }

    // Parse remaining portion as unsigned
    bsls::Types::Uint64 temp = 0;
    int rc = parseDecimal(&temp, input, inputLength, sc);
    if (rc != 0) {
        return rc;                                                    // RETURN
    }

    // TBD Microsoft is warning that -temp is still an unsigned value.  Rather
    // than silence the warning, note that there is an unvalidated assumption
    // that temp <= INT_MAX.  It is not clear from the contract in the header
    // how to handle such a case, although returning 'BAEXML_FAILURE' might be
    // most appropriate.

    *result = sign ? -temp : temp;

    return BAEXML_SUCCESS;
}

int TypesParserUtil_Imp::parseDecimal(unsigned char              *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseUnsignedDecimal(result, input, inputLength);
}

int TypesParserUtil_Imp::parseDecimal(unsigned short             *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseUnsignedDecimal(result, input, inputLength);
}

int TypesParserUtil_Imp::parseDecimal(unsigned int               *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseUnsignedDecimal(result, input, inputLength);
}

int TypesParserUtil_Imp::parseDecimal(bsls::Types::Uint64        *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    if (0 == inputLength) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    bsls::Types::Uint64 val = 0;

    for (; 0 < inputLength; --inputLength) {
        int c = *input++;
        if ('0' <= c && c <= '9') {
            val *= 10;
            val += c - '0';
        }
        else {
            return BAEXML_FAILURE;                                    // RETURN
        }
    }

    *result = val;

    return BAEXML_SUCCESS;
}

int TypesParserUtil_Imp::parseDecimal(float                      *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    double tmpResult;
    int rc = parseDouble(&tmpResult, input, inputLength, true);
    if (rc == 0) {
        *result = static_cast<float>(tmpResult);  // May overflow to +/- INF.
    }
    return rc;
}

int TypesParserUtil_Imp::parseDecimal(double                     *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseDouble(result, input, inputLength, true);
}

// DEFAULT FUNCTIONS

int TypesParserUtil_Imp::parseDefault(bool                       *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseBoolean(result, input, inputLength);
}

int TypesParserUtil_Imp::parseDefault(float                      *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    double tmpResult;
    int rc = parseDouble(&tmpResult, input, inputLength, false);
    if (rc == 0) {
        *result = static_cast<float>(tmpResult);  // May overflow to +/- INF.
    }
    return rc;
}

int TypesParserUtil_Imp::parseDefault(double                     *result,
                                      const char                 *input,
                                      int                         inputLength,
                                      bdlat_TypeCategory::Simple)
{
    return parseDouble(result, input, inputLength, false);
}

// HEX FUNCTIONS

int TypesParserUtil_Imp::parseHex(bsl::string                *result,
                                  const char                 *input,
                                  int                         inputLength,
                                  bdlat_TypeCategory::Simple)
{
    enum { BAEXML_FAILURE = -1 };

    HexParser<bsl::string> hexParser;

    if (0 != hexParser.beginParse(result)) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    if (0 != hexParser.pushCharacters(input, input + inputLength)) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    return hexParser.endParse();
}

int TypesParserUtil_Imp::parseHex(bsl::vector<char>         *result,
                                  const char                *input,
                                  int                        inputLength,
                                  bdlat_TypeCategory::Array)
{
    enum { BAEXML_FAILURE = -1 };

    HexParser<bsl::vector<char> > hexParser;

    if (0 != hexParser.beginParse(result)) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    if (0 != hexParser.pushCharacters(input, input + inputLength)) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    return hexParser.endParse();
}

// TEXT FUNCTIONS

int TypesParserUtil_Imp::parseText(bool                       *result,
                                   const char                 *input,
                                   int                         inputLength,
                                   bdlat_TypeCategory::Simple)
{
    return parseBoolean(result, input, inputLength);
}

int TypesParserUtil_Imp::parseText(char                       *result,
                                   const char                 *input,
                                   int                         inputLength,
                                   bdlat_TypeCategory::Simple)
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    if (1 != inputLength) {
        return BAEXML_FAILURE;                                        // RETURN
    }

    *result = *input;

    return BAEXML_SUCCESS;
}

int TypesParserUtil_Imp::parseText(bsl::string                *result,
                                   const char                 *input,
                                   int                         inputLength,
                                   bdlat_TypeCategory::Simple)
{
    enum { BAEXML_SUCCESS = 0 };

    result->assign(input, inputLength);

    return BAEXML_SUCCESS;
}

int TypesParserUtil_Imp::parseText(bsl::vector<char>         *result,
                                   const char                *input,
                                   int                        inputLength,
                                   bdlat_TypeCategory::Array)
{
    enum { BAEXML_SUCCESS = 0 };

    result->assign(input, input + inputLength);

    return BAEXML_SUCCESS;
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
