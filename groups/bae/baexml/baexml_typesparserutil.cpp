// baexml_typesparserutil.cpp                  -*-C++-*-
#include <baexml_typesparserutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_typesparserutil_cpp,"$Id$ $CSID$")

#include <baexml_typesprintutil.h>  // for testing only

#include <baexml_base64parser.h>
#include <baexml_hexparser.h>

#include <bdesb_fixedmeminstreambuf.h>

#include <bsl_climits.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>
#include <bsl_cerrno.h>
#include <bsl_cfloat.h>
#include <bsl_cmath.h>

namespace BloombergLP {

namespace {

// HELPER FUNCTIONS

int parseBoolean(bool *result, const char *input, int inputLength)
    // Set '*result' to true if 'input' is "1" or "true" and false if 'input'
    // is "0" or "false".  Strings are case-insensitive.  Return 0 on success
    // and non-zero if 'input' is not "1", "0", "true", or "false".
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    switch (inputLength) {
      case 1: {
        if ('1' == input[0]) {
            *result = true;
            return BAEXML_SUCCESS;
        }
        else if ('0' == input[0]) {
            *result = false;
            return BAEXML_SUCCESS;
        }
      } break;
      case 4: {
        if (('t' == input[0] || 'T' == input[0])
         && ('r' == input[1] || 'R' == input[1])
         && ('u' == input[2] || 'U' == input[2])
         && ('e' == input[3] || 'E' == input[3])) {
            *result = true;
            return BAEXML_SUCCESS;
        }
      } break;
      case 5: {
        if (('f' == input[0] || 'F' == input[0])
         && ('a' == input[1] || 'A' == input[1])
         && ('l' == input[2] || 'L' == input[2])
         && ('s' == input[3] || 'S' == input[3])
         && ('e' == input[4] || 'E' == input[4])) {
            *result = false;
            return BAEXML_SUCCESS;
        }
      } break;
    }

    return BAEXML_FAILURE;
}

int parseDoubleImpl(double     *result,
                    const char *input,
                    bool        formatDecimal)
    // Parse a string representing a double.
    // Parameter 'input' is null-terminated string.
    // Parameter "formatDecimal" will be true if 'input' should
    // contain only decimal digits, period and sign characters (i.e., INF/NaN,
    // and exponential notation are not allowed);
    // otherwise 'input' can contain any float point representation
    // form.  Return 0 on success and non-zero otherwise.
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };
    static const char decimalChars[] = "+-.0123456789";

    if (formatDecimal && input[bsl::strspn(input, decimalChars)] != '\0') {
        // Non-decimal character (i.e., potential INF, NaN, or exponent) found.
        return BAEXML_FAILURE;
    }

    switch (input[0]) {
      case 'N':
        if (bsl::strcmp(input + 1, "aN") == 0) {
            *result = bsl::numeric_limits<double>::quiet_NaN();
            return BAEXML_SUCCESS;
        }
        break;
      case 'I':
        if (bsl::strcmp(input + 1, "NF") == 0) {
            *result = bsl::numeric_limits<double>::infinity();
            return BAEXML_SUCCESS;
        }
        break;
      case '+':
        if (bsl::strcmp(input + 1, "INF") == 0) {
            *result = bsl::numeric_limits<double>::infinity();
            return BAEXML_SUCCESS;
        }
        break;
      case '-':
        if (bsl::strcmp(input + 1, "INF") == 0) {
            *result = -bsl::numeric_limits<double>::infinity();
            return BAEXML_SUCCESS;
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
        return BAEXML_FAILURE;
    }

    // We ignore underflow errors.  If strtod sets errno to ERANGE, then it
    // will also have set the result to a very large number for overflow or a
    // very small number (or zero) for underflow -- exactly what we want.
    if (ERANGE == errno) {
        if (*result < -1.0 || 1.0 < *result) {
            return BAEXML_FAILURE; // Overflow is an error.
        }
        else {
            return BAEXML_SUCCESS; // Underflow is OK (very small number).
        }
    }
    else if (errno != 0) {
        return BAEXML_FAILURE;
    }

    return BAEXML_SUCCESS;
}

int parseDouble(double     *result,
                const char *input,
                int         inputLength,
                bool        formatDecimal)
    // Parse a string representing a double.
    // Parameter "formatDecimal" will be true if 'input' should
    // contain only decimal digits, period and sign characters;
    // otherwise 'input' can contain any float point representation
    // form.  Return 0 on success and non-zero otherwise.
{
    // 'input' must be zero terminated string
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };
    enum { BUFLEN = 80 };

    if (0 == inputLength) {
        return BAEXML_FAILURE;
    }

    if (inputLength < BUFLEN) {
        // Use a fixed-length buffer for efficiency.
        char  buffer[BUFLEN];
        bsl::memcpy(buffer, input, inputLength);
        buffer[inputLength] = '\0';

        return parseDoubleImpl(result, buffer, formatDecimal);

    }
    else {
        // Use a string for dynamic allocation.
        bsl::string tmp(input, inputLength);
        return parseDoubleImpl(result, tmp.c_str(), formatDecimal);
    }
}

int parseInt(int *result, const char *input, int inputLength)
    // Parse an unsigned long decimal string
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };
    enum { BUFLEN = 80 };

    int consumed = 0;

    if (0 == inputLength) {
        return BAEXML_FAILURE;
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
        return BAEXML_FAILURE;
    }

    return BAEXML_SUCCESS;
}

int parseUnsignedInt(unsigned int *result,
                     const char   *input,
                     int           inputLength)
    // Parse an unsigned long decimal string
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };
    enum { BUFLEN = 80 };

    int consumed = 0;

    if (0 == inputLength) {
        return BAEXML_FAILURE;
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
        return BAEXML_FAILURE;
    }

    return BAEXML_SUCCESS;
}

template <typename INT_TYPE>
inline
int parseSignedDecimal(INT_TYPE *result, const char *input, int inputLength)
    // Parse a signed integral value as decimal.
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    int temp;

    if (0 != parseInt(&temp, input, inputLength) ||
        temp < bsl::numeric_limits<INT_TYPE>::min() ||
        temp > bsl::numeric_limits<INT_TYPE>::max()) {
        return BAEXML_FAILURE;
    }

    *result = static_cast<INT_TYPE>(temp);
    return BAEXML_SUCCESS;
}

template <typename INT_TYPE>
inline
int parseUnsignedDecimal(INT_TYPE *result, const char *input, int inputLength)
    // Parse an unsigned integral value as decimal.
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    unsigned int temp;

    if (0 != parseUnsignedInt(&temp, input, inputLength) ||
        temp > bsl::numeric_limits<INT_TYPE>::max()) {
        return BAEXML_FAILURE;
    }

    *result = static_cast<INT_TYPE>(temp);
    return BAEXML_SUCCESS;
}

}  // close anonymous namespace

                         // ---------------------------------
                         // struct baexml_TypesParserUtil_Imp
                         // ---------------------------------

// BASE64 FUNCTIONS

int baexml_TypesParserUtil_Imp::parseBase64(bsl::string *result,
                                            const char  *input,
                                            int          inputLength,
                                            bdeat_TypeCategory::Simple)
{
    enum { BAEXML_FAILURE = -1 };

    baexml_Base64Parser<bsl::string> base64Parser;

    if (0 != base64Parser.beginParse(result)) {
        return BAEXML_FAILURE;
    }

    if (0 != base64Parser.pushCharacters(input, input + inputLength)) {
        return BAEXML_FAILURE;
    }

    return base64Parser.endParse();
}

int baexml_TypesParserUtil_Imp::parseBase64(bsl::vector<char> *result,
                                            const char        *input,
                                            int                inputLength,
                                            bdeat_TypeCategory::Array)
{
    enum { BAEXML_FAILURE = -1 };

    baexml_Base64Parser<bsl::vector<char> > base64Parser;

    if (0 != base64Parser.beginParse(result)) {
        return BAEXML_FAILURE;
    }

    if (0 != base64Parser.pushCharacters(input, input + inputLength)) {
        return BAEXML_FAILURE;
    }

    return base64Parser.endParse();
}

// DECIMAL FUNCTIONS

int baexml_TypesParserUtil_Imp::parseDecimal(bool       *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseBoolean(result, input, inputLength);
}

int baexml_TypesParserUtil_Imp::parseDecimal(char       *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseSignedDecimal((signed char*) result,
                              input, inputLength);
}

int baexml_TypesParserUtil_Imp::parseDecimal(short      *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseSignedDecimal(result, input, inputLength);
}

int baexml_TypesParserUtil_Imp::parseDecimal(int        *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseSignedDecimal(result, input, inputLength);
}

int
baexml_TypesParserUtil_Imp::parseDecimal(bsls_PlatformUtil::Int64 *result,
                                         const char               *input,
                                         int                       inputLength,
                                         bdeat_TypeCategory::Simple sc)
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    int sign = 0;
    if (inputLength > 0 && '-' == input[0]) {
        sign = -1;
        ++input;
        --inputLength;
    }

    // Parse remaining portion as unsigned
    bsls_PlatformUtil::Uint64 temp = 0;
    int rc = parseDecimal(&temp, input, inputLength, sc);
    if (rc != 0) {
        return rc;
    }

    // TBD Microsoft is warning that -temp is still an unsigned value.
    // Rather than silence the warning, note that there is an unvalidated
    // assumption that temp <= INT_MAX.  It is not clear from the contract
    // in the header how to handle such a case, although returning
    // 'BAEXML_FAILURE' might be most appropriate.

    *result = sign ? -temp : temp;

    return BAEXML_SUCCESS;
}

int baexml_TypesParserUtil_Imp::parseDecimal(unsigned char *result,
                                             const char    *input,
                                             int            inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseUnsignedDecimal(result, input, inputLength);
}

int baexml_TypesParserUtil_Imp::parseDecimal(unsigned short *result,
                                             const char     *input,
                                             int             inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseUnsignedDecimal(result, input, inputLength);
}

int baexml_TypesParserUtil_Imp::parseDecimal(unsigned int *result,
                                             const char   *input,
                                             int           inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseUnsignedDecimal(result, input, inputLength);
}

int baexml_TypesParserUtil_Imp::parseDecimal(
    bsls_PlatformUtil::Uint64 *result,
    const char                *input,
    int                        inputLength,
    bdeat_TypeCategory::Simple)
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    if (0 == inputLength) {
        return BAEXML_FAILURE;
    }

    bsls_PlatformUtil::Uint64 val = 0;

    for (; 0 < inputLength; --inputLength) {
        int c = *input++;
        if ('0' <= c && c <= '9') {
            val *= 10;
            val += c - '0';
        }
        else {
            return BAEXML_FAILURE;
        }
    }

    *result = val;

    return BAEXML_SUCCESS;
}

int baexml_TypesParserUtil_Imp::parseDecimal(
    float                     *result,
    const char                *input,
    int                        inputLength,
    bdeat_TypeCategory::Simple)
{
    double tmpResult;
    int rc = parseDouble(&tmpResult, input, inputLength, true);
    if (rc == 0) {
        *result = tmpResult;  // May overflow to +/- INF.
    }
    return rc;
}

int baexml_TypesParserUtil_Imp::parseDecimal(
    double                    *result,
    const char                *input,
    int                        inputLength,
    bdeat_TypeCategory::Simple)
{
    return parseDouble(result, input, inputLength, true);
}

// DEFAULT FUNCTIONS

int baexml_TypesParserUtil_Imp::parseDefault(bool       *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseBoolean(result, input, inputLength);
}

int baexml_TypesParserUtil_Imp::parseDefault(float      *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    double tmpResult;
    int rc = parseDouble(&tmpResult, input, inputLength, false);
    if (rc == 0) {
        *result = tmpResult;  // May overflow to +/- INF.
    }
    return rc;
}

int baexml_TypesParserUtil_Imp::parseDefault(double     *result,
                                             const char *input,
                                             int         inputLength,
                                             bdeat_TypeCategory::Simple)
{
    return parseDouble(result, input, inputLength, false);
}

// HEX FUNCTIONS

int baexml_TypesParserUtil_Imp::parseHex(bsl::string *result,
                                         const char  *input,
                                         int          inputLength,
                                         bdeat_TypeCategory::Simple)
{
    enum { BAEXML_FAILURE = -1 };

    baexml_HexParser<bsl::string> hexParser;

    if (0 != hexParser.beginParse(result)) {
        return BAEXML_FAILURE;
    }

    if (0 != hexParser.pushCharacters(input, input + inputLength)) {
        return BAEXML_FAILURE;
    }

    return hexParser.endParse();
}

int baexml_TypesParserUtil_Imp::parseHex(bsl::vector<char> *result,
                                         const char        *input,
                                         int                inputLength,
                                         bdeat_TypeCategory::Array)
{
    enum { BAEXML_FAILURE = -1 };

    baexml_HexParser<bsl::vector<char> > hexParser;

    if (0 != hexParser.beginParse(result)) {
        return BAEXML_FAILURE;
    }

    if (0 != hexParser.pushCharacters(input, input + inputLength)) {
        return BAEXML_FAILURE;
    }

    return hexParser.endParse();
}

// TEXT FUNCTIONS

int baexml_TypesParserUtil_Imp::parseText(bool       *result,
                                          const char *input,
                                          int         inputLength,
                                          bdeat_TypeCategory::Simple)
{
    return parseBoolean(result, input, inputLength);
}

int baexml_TypesParserUtil_Imp::parseText(char       *result,
                                          const char *input,
                                          int         inputLength,
                                          bdeat_TypeCategory::Simple)
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    if (1 != inputLength) {
        return BAEXML_FAILURE;
    }

    *result = *input;

    return BAEXML_SUCCESS;
}

int baexml_TypesParserUtil_Imp::parseText(bsl::string *result,
                                          const char  *input,
                                          int          inputLength,
                                          bdeat_TypeCategory::Simple)
{
    enum { BAEXML_SUCCESS = 0 };

    result->assign(input, inputLength);

    return BAEXML_SUCCESS;
}

int baexml_TypesParserUtil_Imp::parseText(bsl::vector<char> *result,
                                          const char        *input,
                                          int                inputLength,
                                          bdeat_TypeCategory::Array)
{
    enum { BAEXML_SUCCESS = 0 };

    result->assign(input, input + inputLength);

    return BAEXML_SUCCESS;
}

} // Close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
