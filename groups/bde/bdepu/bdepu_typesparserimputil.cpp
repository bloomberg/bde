// bdepu_typesparserimputil.cpp                                       -*-C++-*-
#include <bdepu_typesparserimputil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdepu_typesparserimputil_cpp,"$Id$ $CSID$")

#include <bdepu_realparserimputil.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_iterator.h>

#include <bsl_cctype.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>

namespace BloombergLP {

typedef bdepu_ParserImpUtil       ImpUtil;
typedef bsls_PlatformUtil::Int64  Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

// WARNING: 'bdepu_TypesParserImpUtil' methods depend on the numeric values
// of the enum elements.

// TYPES
enum { BDEPU_SUCCESS = 0, BDEPU_FAILURE = 1 };

enum {
    // Enum used in generating characters.  These are special characters and
    // escape sequences.

    SES_0  = 0,   // character '\0'
    SES_a  = 1,   // character '\a'
    SES_b  = 2,   // character '\b'
    SES_t  = 3,   // character '\t'
    SES_f  = 4,   // character '\f'
    SES_n  = 5,   // character '\n'
    SES_r  = 6,   // character '\r'
    SES_v  = 7,   // character '\v'
    SES_SQ = 8,   // character '\''
    SES_DQ = 9,   // character '\"'
    SES_QM = 10,  // character '\?'
    SES_BS = 11,  // character '\\'
    HEX    = 12,  // characters that should print as hex digits
    PRINT  = 13   // all printable characters
};

// STATIC DATA
static const char PRINTTYPE[256] = {
    // This table indicates, for each character, whether it will print out
    // as itself, as a hex value, or as an escape sequence.

/*  0*/  SES_0,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,   SES_a,
/*  8*/  SES_b,   SES_t,   SES_n,   SES_v,   SES_f,   SES_r,     HEX,     HEX,
/* 16*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/* 24*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/* 32*/  PRINT,   PRINT,  SES_DQ,   PRINT,   PRINT,   PRINT,   PRINT,  SES_SQ,
/* 40*/  PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,
/* 48*/  PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,
/* 56*/  PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,  SES_QM,
/* 64*/  PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,
/* 72*/  PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,
/* 80*/  PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,
/* 88*/  PRINT,   PRINT,   PRINT,   PRINT,  SES_BS,   PRINT,   PRINT,   PRINT,
/* 96*/  PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,
/*104*/  PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,
/*112*/  PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,
/*120*/  PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,   PRINT,     HEX,
/*128*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*136*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*144*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*152*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*160*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*168*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*176*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*184*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*192*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*200*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*208*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*216*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*224*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*232*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*240*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
/*248*/    HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,     HEX,
};

static const char ESC_SEQUENCE_STRING[12] = {
    '0', 'a', 'b', 't', 'f', 'n', 'r', 'v', '\'', '\"', '?', '\\'
};

static char DIGITS[37] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    // Used in 'generateInt' and 'generateInt64'.  Must be identical in the
    // first 16 members to 'bdepu_TypesParserImpUtil::s_hex' defined below.

// STATIC HELPER FUNCTIONS
static
int parseRealAsDecimal(const char                **endPos,
                       int                        *decSign,
                       bsls_PlatformUtil::Uint64  *decFrac,
                       int                        *decExp,
                       const char                 *inputString)
    // Parse the specified 'inputString' for a sequence of characters matching
    // the production rule <DOUBLE> and place into the specified 'decSign',
    // 'decFrac', and 'decExp' the corresponding value.  Store in the specified
    // '*endPos' the address of the non-modifiable character in 'inputString'
    // immediately following the successfully parsed text, or the position at
    // which the parse failure was detected.  Return zero on success, and a
    // non-zero value otherwise.  The value of 'decSign', 'decFrac', and
    // 'decExp' are unchanged if a parse failure occurs.  The behavior is
    // undefined if any argument is 0.
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(decSign);
    BSLS_ASSERT(decFrac);
    BSLS_ASSERT(decExp);
    BSLS_ASSERT(inputString);

    int sign = ('-' == *inputString) ? 1 : 0;
    inputString += ('+' == *inputString || '-' == *inputString);

    int    hasDigit = 0;
    Uint64 frac     = 0;
    int    exp      = 18;

    // On Sun, int64 divisions are PAINFULLY slow.  Instead of dividing
    // 'frac / 100000000000000000uLL', we'll compare the base 10 logs of the
    // values.  'firstDigitPos' is a constant, and 'fracDigits' will be
    // incremented whenever frac is multiplied by 10 or 'exp' is increased.
    const int firstDigitPos = 18;
    int       fracDigits    =  0;

    while ('0' <= *inputString && *inputString <= '9') {
        if (fracDigits >= firstDigitPos) {
            // It is possible that exp will overflow but that would imply
            // 2^31 - 19 characters so it is not tested for here.
            ++exp;
        }
        else {
            const int digit = *inputString - '0';
            frac = frac * 10 + digit;
            hasDigit = 1;
        }
        ++fracDigits;
        ++inputString;
    }

    if ('.' == *inputString) {
        ++inputString;
        while (0 == frac && '0' == *inputString) {
            // It is possible that exp will underflow but that would imply
            // 2^31 + 17 characters so it is not tested for here.
            --exp;
            hasDigit = 1;
            ++inputString;
        }
        while ('0' <= *inputString && *inputString <= '9') {
            if (fracDigits < firstDigitPos) {
              const int digit = *inputString - '0';
              frac = frac * 10 + digit;
              // It is possible that exp will underflow but that would imply
              // 2^31 + 17 characters so it is not tested for here.
              --exp;
              hasDigit = 1;
              ++fracDigits;
            }
            ++inputString;
        }
    }

    if (0 == hasDigit) {
        *endPos = inputString;
        return BDEPU_FAILURE;
    }

    while (frac && fracDigits < firstDigitPos) {
        frac *= 10;
        ++fracDigits;
        // It is possible that exp will underflow but that would imply
        // 2^31 + 17 characters so it is not tested for here.
        --exp;
    }

    if ('e' == *inputString || 'E' == *inputString) {
        ++inputString;
        Int64 tmp = 0;
        if (bdepu_TypesParserImpUtil::parseInt64(endPos, &tmp, inputString)) {
            return BDEPU_FAILURE;
        }
        if (tmp < -0x7FFFFFFF || tmp > 0x7FFFFFFF) {
            return BDEPU_FAILURE;
        }
        inputString = *endPos;
        Int64 tmpExp = tmp + exp;
        if (tmpExp < -0x7FFFFFFF || tmpExp > 0x7FFFFFFF) {
            return BDEPU_FAILURE;
        }
        exp = tmpExp;
    }

    *decSign = sign;
    *decFrac = frac;
    *decExp = exp;
    *endPos = inputString;
    return BDEPU_SUCCESS;
}

namespace {

// IMPLEMENTATION NOTES: Int64 and Uint64 divisions are expensive, so we have
// separate implementations for 'generateInt' and 'generateInt64'.  In order to
// minimize the overlap between the two implementations, however, we create the
// following helper template.

template <typename UNSIGNED, typename INTEGER>
char *generateIntDecimalHelper(char *buffer, INTEGER value, int length)
    // Write into the specified 'buffer' of the specified 'length', the
    // representation of the specified 'value' in decimal representation,
    // left-padded to end exactly at 'buffer + length', and return the
    // beginning of the string representation within 'buffer'.  The buffer is
    // NOT null-terminated, and must be sufficiently big to accommodate any
    // possible value of the parameterized 'INTEGER' type.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);

    buffer += length;
    if (0 < value) {
        for (; 0 != value; value /= 10) {
            *--buffer = '0' + (int)(value % 10);
        }
        return buffer;
    }
    if (0 > value) {
        UNSIGNED unsignedValue = -value;
        for (; 0 != unsignedValue; unsignedValue /= 10) {
            *--buffer = '0' + (int)(unsignedValue % 10);
        }
        *--buffer = '-';
        return buffer;
    }
    *--buffer = '0';
    return buffer;
}

template <typename UNSIGNED, typename INTEGER>
char *generateIntHelper(char *buffer, INTEGER value, int length, int base)
    // Write into the specified 'buffer' of the specified 'length', the
    // representation of the specified 'value' in the specified 'base',
    // left-padded to end exactly at 'buffer + length', and return the
    // beginning of the string representation within 'buffer'.  The buffer is
    // NOT null-terminated, and must be sufficiently big to accommodate any
    // possible value of the parameterized 'INTEGER' type.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    buffer += length;
    if (0 < value) {
        for (; 0 != value; value /= base) {
            *--buffer = DIGITS[(int)(value % base)];
        }
        return buffer;
    }
    if (0 > value) {
        UNSIGNED unsignedValue = -value;
        for (; 0 != unsignedValue; unsignedValue /= base) {
            *--buffer = DIGITS[(int)(unsignedValue % base)];
        }
        *--buffer = '-';
        return buffer;
    }
    *--buffer = '0';
    return buffer;
}

inline
int copyBuf(char *dest, int destLen, const char *src, int srcLen)
    // Copy the contents of the specified 'src' string having the specified
    // 'srcLen' into the specified 'dest' buffer having the specified
    // 'destLen' and return 'srcLen'.  If 'destLen > srcLen', add a null
    // terminator.  If 'srcLen >= destLen', then copy only 'destLen' bytes
    // and add do not add a null terminator.
{
    BSLS_ASSERT(dest);
    BSLS_ASSERT(0 <= destLen);
    BSLS_ASSERT(src);
    BSLS_ASSERT(0 <= srcLen);

    if (destLen > srcLen) {
        bsl::memcpy(dest, src, srcLen);
        dest[srcLen] = '\0';
    }
    else {
        bsl::memcpy(dest, src, destLen);
    }
    return srcLen;
}

inline
int copyBufRaw(char *dest, const char *src, int srcLen)
    // Copy the contents of the specified 'src' string having the specified
    // 'srcLen' into the specified 'dest' buffer having more than 'srcLen'.
    // Do not add a null terminator.
{
    BSLS_ASSERT(dest);
    BSLS_ASSERT(src);
    BSLS_ASSERT(0 <= srcLen);

    bsl::memcpy(dest, src, srcLen);
    return srcLen;
}

template <typename UNSIGNED, typename INTEGER>
void generateIntHelper(bsl::vector<char> *buffer,
                       INTEGER            value,
                       int                base)
    // Append to the specified 'buffer' vector the individual characters
    // forming the representation of the specified 'value' in the specified
    // 'base'.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    enum { BUF_LEN = bdepu_TypesParserImpUtil::BDEPU_MAX_INT64_STRLEN };
    char tempBuf[BUF_LEN];

    char *tempBegin = tempBuf + BUF_LEN;
    if (0 < value) {
        for (; 0 != value; value /= base) {
            *--tempBegin = DIGITS[(int)(value % base)];
        }
    }
    else if (0 > value) {
        UNSIGNED unsignedValue = -value;
        for (; 0 != unsignedValue; unsignedValue /= base) {
            *--tempBegin = DIGITS[(int)(unsignedValue % base)];
        }
        *--tempBegin = '-';
    }
    else {
        *--tempBegin = '0';
    }

    buffer->insert(buffer->end(), tempBegin, tempBuf + BUF_LEN);
}

}  // close unnamed namespace

// ===========================================================================
//                             COMPONENT DEFINITIONS
// ===========================================================================

                        // -------------------------------
                        // struct bdepu_TypesParserImpUtil
                        // -------------------------------

// CLASS DATA
const char *bdepu_TypesParserImpUtil::s_charTypes = PRINTTYPE;
    // Note that defining the table statically and assigning its address
    // insulates clients from its size.

const char *bdepu_TypesParserImpUtil::s_sesStrings = ESC_SEQUENCE_STRING;
    // Note that defining the table statically and assigning its address
    // insulates clients from its size.

unsigned char bdepu_TypesParserImpUtil::s_hex[16] = { '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

// CLASS METHODS

                        // *** PARSING FUNCTIONS ***

int bdepu_TypesParserImpUtil::parseChar(const char **endPos,
                                        char        *result,
                                        const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    if (bdepu_ParserImpUtil::skipRequiredToken(endPos, inputString, '\'')) {
        return BDEPU_FAILURE;
    }

    char tmpRes;
    int flags;
    if (parseCharRaw(endPos, &tmpRes, *endPos, &flags)) {
        return BDEPU_FAILURE;
    }
    if (!(BDEPU_HAS_ESCAPE & flags) && '\'' == tmpRes) {
        --*endPos;
        return BDEPU_FAILURE;
    }

    if (**endPos != '\'') {
        return BDEPU_FAILURE;
    }

    ++*endPos;
    *result = tmpRes;
    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseCharRaw(const char **endPos,
                                           char        *result,
                                           const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    int rv;

    switch (bdepu_EscapeChar::escapeCodes()[(unsigned char)*inputString]) {
      // The characters 'a', 'b','f', 'n', 'r', 't', 'v', '\'', '\"', '?',
      // and '\\' have a value other than BS_ER in the table because the
      // table's primary use is to identify which escape character a
      // character represents when it follows a backslash.  But here we need
      // differentiate only backslash and <EOS>, so the other values all
      // collapse to the same case as 'ERROR'.
      case bdepu_EscapeChar::BDEPU_ERROR:
      case bdepu_EscapeChar::BDEPU_ALERT:
      case bdepu_EscapeChar::BDEPU_BACKSPACE:
      case bdepu_EscapeChar::BDEPU_FORMFEED:
      case bdepu_EscapeChar::BDEPU_NEWLINE:
      case bdepu_EscapeChar::BDEPU_CARRIAGE_RETURN:
      case bdepu_EscapeChar::BDEPU_TAB:
      case bdepu_EscapeChar::BDEPU_VERTICAL_TAB:
      case bdepu_EscapeChar::BDEPU_QUESTION_MARK:
      case bdepu_EscapeChar::BDEPU_SINGLE_QUOTE:
      case bdepu_EscapeChar::BDEPU_DOUBLE_QUOTE:
      case bdepu_EscapeChar::BDEPU_HEXADECIMAL:
      case bdepu_EscapeChar::BDEPU_OCTAL: {
        // This is not an escape sequence, it is a regular character.
        *result = *inputString;
        *endPos = inputString + 1;
        rv = BDEPU_SUCCESS;
      } break;
      case bdepu_EscapeChar::BDEPU_BACKSLASH: {
        // This is an escape sequence
        ++inputString;
        rv = bdepu_ParserImpUtil::parseEscapeSequence(endPos,
                                                      result,
                                                      inputString);
      } break;
      case bdepu_EscapeChar::BDEPU_ENDLINE:  // '\0' is illegal
      default: {
        *endPos = inputString;
        rv = BDEPU_FAILURE;
      } break;
    }
    return rv;
}

int bdepu_TypesParserImpUtil::parseCharRaw(const char **endPos,
                                           char        *result,
                                           const char  *inputString,
                                           int         *flags)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(flags);

    int rv;

    switch (bdepu_EscapeChar::escapeCodes()[(unsigned char)*inputString]) {
      // The characters 'a', 'b','f', 'n', 'r', 't', 'v', '\'', '\"', '?',
      // and '\\' have a value other than BS_ER in the table because the
      // table's primary use is to identify which escape character a
      // character represents when it follows a backslash.  But here we need
      // differentiate only backslash and <EOS>, so the other values all
      // collapse to the same case as 'ERROR'.
      case bdepu_EscapeChar::BDEPU_ERROR:
      case bdepu_EscapeChar::BDEPU_ALERT:
      case bdepu_EscapeChar::BDEPU_BACKSPACE:
      case bdepu_EscapeChar::BDEPU_FORMFEED:
      case bdepu_EscapeChar::BDEPU_NEWLINE:
      case bdepu_EscapeChar::BDEPU_CARRIAGE_RETURN:
      case bdepu_EscapeChar::BDEPU_TAB:
      case bdepu_EscapeChar::BDEPU_VERTICAL_TAB:
      case bdepu_EscapeChar::BDEPU_QUESTION_MARK:
      case bdepu_EscapeChar::BDEPU_SINGLE_QUOTE:
      case bdepu_EscapeChar::BDEPU_DOUBLE_QUOTE:
      case bdepu_EscapeChar::BDEPU_HEXADECIMAL:
      case bdepu_EscapeChar::BDEPU_OCTAL: {
        // This is not an escape sequence, it is a regular character.
        *result = *inputString;
        *endPos = inputString + 1;
        *flags = 0;
        rv = BDEPU_SUCCESS;
      } break;
      case bdepu_EscapeChar::BDEPU_BACKSLASH: {
        // This is an escape sequence
        ++inputString;
        rv = bdepu_ParserImpUtil::parseEscapeSequence(endPos,
                                                      result,
                                                      inputString);
        *flags = rv ? *flags : BDEPU_HAS_ESCAPE;
      } break;
      case bdepu_EscapeChar::BDEPU_ENDLINE:  // '\0' is illegal
      default: {
        *endPos = inputString;
        rv = BDEPU_FAILURE;
      } break;
    }
    return rv;
}

int bdepu_TypesParserImpUtil::parseDate(const char **endPos,
                                        bdet_Date   *result,
                                        const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    Uint64 tmp;

    int year = -1;
    if (0 != bdepu_ParserImpUtil::parseUnsignedInteger(endPos,
                                                       &tmp,
                                                       inputString,
                                                       10,
                                                       9999,
                                                       4)) {
        return BDEPU_FAILURE;
    }
    year = tmp;
    if (0 == year) {    // Year "0000" is invalid.
        --*endPos;
        return BDEPU_FAILURE;
    }

    if (**endPos != '/') {
        return BDEPU_FAILURE;
    }
    ++*endPos;

    int month;
    if (0 != bdepu_ParserImpUtil::parseUnsignedInteger(endPos,
                                                       &tmp,
                                                       *endPos,
                                                       10,
                                                       99,
                                                       2)) {
        return BDEPU_FAILURE;
    }
    month = tmp;
    if (0 == month) {    // Month "00" is invalid.
        --*endPos;
        return BDEPU_FAILURE;
    }
    if (month > 12) {
        --*endPos;
        return BDEPU_FAILURE;
    }

    if (**endPos != '/') {
        return BDEPU_FAILURE;
    }
    ++*endPos;

    const char *startPos = *endPos;
    int day;
    if (0 != bdepu_ParserImpUtil::parseUnsignedInteger(endPos,
                                                       &tmp,
                                                       *endPos,
                                                       10,
                                                       99,
                                                       2)) {
        return BDEPU_FAILURE;
    }
    day = tmp;
    if (0 == day) {    // Day "00" is invalid.
        --*endPos;
        return BDEPU_FAILURE;
    }
    if (day > 31) {
        --*endPos;
        return BDEPU_FAILURE;
    }
    if (!bdet_Date::isValid(year, month, day)) {
        // Only the 'day' part of the date can be invalid, and this can
        // only be a result of the second digit's value, so set 'endPos' to
        // point to that second digit.
        *endPos = startPos + 1;
        return BDEPU_FAILURE;
    }
    result->setYearMonthDay(year, month, day);
    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseDatetime(const char    **endPos,
                                            bdet_Datetime  *result,
                                            const char     *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdet_Date resultDate;
    bdet_Time resultTime;

    if (parseDate(endPos, &resultDate, inputString)) {
        return BDEPU_FAILURE;
    }

    if (**endPos != ' ' && **endPos != '_') {
        return BDEPU_FAILURE;
    }
    ++*endPos;

    if (parseTime(endPos, &resultTime, *endPos)) {
        return BDEPU_FAILURE;
    }

    *result = bdet_Datetime(resultDate, resultTime);
    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseDatetimeTz(const char      **endPos,
                                              bdet_DatetimeTz  *result,
                                              const char       *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdet_Datetime resultDatetime;

    if (parseDatetime(endPos, &resultDatetime, inputString)) {
        return BDEPU_FAILURE;
    }

    int resultOffset;

    if (parseTz(endPos, &resultOffset, *endPos)) {
        return BDEPU_FAILURE;
    }

    *result = bdet_DatetimeTz(resultDatetime, resultOffset);

    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseDateTz(const char  **endPos,
                                          bdet_DateTz  *result,
                                          const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdet_Date resultDate;

    if (parseDate(endPos, &resultDate, inputString)) {
        return BDEPU_FAILURE;
    }

    int resultOffset;

    if (parseTz(endPos, &resultOffset, *endPos)) {
        return BDEPU_FAILURE;
    }

    *result = bdet_DateTz(resultDate, resultOffset);

    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseDouble(const char **endPos,
                                          double      *result,
                                          const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    int    decSign = 0;
    Uint64 decFrac = 0;
    int    decExp  = 0;

    if (parseRealAsDecimal(endPos, &decSign, &decFrac, &decExp, inputString)) {
        return BDEPU_FAILURE;
    }

    Uint64 binFrac = 0;
    int    binExp  = 0;

    if (bdepu_RealParserImpUtil::convertDecimalToBinary(&binFrac,
                                                        &binExp,
                                                        decFrac,
                                                        decExp)) {
        return BDEPU_FAILURE;
    }
    return bdepu_RealParserImpUtil::convertBinaryToDouble(result,
                                                          decSign,
                                                          binFrac,
                                                          binExp);
}

int bdepu_TypesParserImpUtil::parseFloat(const char **endPos,
                                         float       *result,
                                         const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    int    decSign = 0;
    Uint64 decFrac = 0;
    int    decExp  = 0;

    if (parseRealAsDecimal(endPos, &decSign, &decFrac, &decExp, inputString)) {
        return BDEPU_FAILURE;
    }

    Uint64 binFrac = 0;
    int    binExp  = 0;
    if (bdepu_RealParserImpUtil::convertDecimalToBinary(&binFrac,
                                                        &binExp,
                                                        decFrac,
                                                        decExp)) {
        return BDEPU_FAILURE;
    }
    if (binExp > 127) {        // overflow
        return BDEPU_FAILURE;
    }
    else if (binExp < -126) {  // underflow
        *result = 0.0;
        return BDEPU_SUCCESS;
    }

    double res;
    bdepu_RealParserImpUtil::convertBinaryToDouble(&res,
                                                   decSign,
                                                   binFrac,
                                                   binExp);
                                   // Cannot fail due to previous range checks.

    *result = res;
    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseInt(const char **endPos,
                                       int         *result,
                                       const char  *inputString,
                                       int          base)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    Int64 res = *result;
    int rv = bdepu_ParserImpUtil::parseSignedInteger(endPos,
                                                     &res,
                                                     inputString,
                                                     base,
                                                     -(Int64)0x80000000,
                                                     (Int64)0x7FFFFFFF);
    *result = res;
    return rv;
}

int bdepu_TypesParserImpUtil::parseInt64(
                                        const char               **endPos,
                                        bsls_PlatformUtil::Int64  *result,
                                        const char                *inputString,
                                        int                        base)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    Int64 res = *result;
    int rv = bdepu_ParserImpUtil::parseSignedInteger(
                                              endPos,
                                              &res,
                                              inputString,
                                              base,
                                             -(Int64)0x7FFFFFFFFFFFFFFFuLL - 1,
                                              (Int64)0x7FFFFFFFFFFFFFFFuLL);
    *result = res;
    return rv;
}

int bdepu_TypesParserImpUtil::parseShort(const char **endPos,
                                         short       *result,
                                         const char  *inputString,
                                         int          base)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    Int64 res = *result;
    int rv = bdepu_ParserImpUtil::parseSignedInteger(endPos,
                                                     &res,
                                                     inputString,
                                                     base,
                                                     -(Int64)32768,
                                                     (Int64)32767);
    *result = res;
    return rv;
}

int bdepu_TypesParserImpUtil::parseDelimitedString(
                                                  const char  **endPos,
                                                  bsl::string  *result,
                                                  const char   *inputString,
                                                  char          leftDelimiter,
                                                  char          rightDelimiter)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    *endPos = inputString;

    if (leftDelimiter) {  // 0 delimiter means don't look for one
        if (leftDelimiter != **endPos) {
            return BDEPU_FAILURE;
        }
        ++*endPos;
        ++inputString;
    }

    char charResult;
    int flags = 0;
    while ('\0' != **endPos && rightDelimiter != **endPos) {
        int tmpFlags = 0;
        if (parseCharRaw(endPos, &charResult, *endPos, &tmpFlags)) {
            return BDEPU_FAILURE;
        }
        flags |= tmpFlags;
    }
    if (rightDelimiter != **endPos) {
        return BDEPU_FAILURE;
    }
    ++*endPos;

    result->assign(inputString, *endPos - inputString - 1);
    if (flags & BDEPU_HAS_ESCAPE) {
        bdepu_ParserImpUtil::compactMultibyteChars(result);
    }
    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseSpaceDelimitedString(
                                                     const char  **endPos,
                                                     bsl::string  *result,
                                                     const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    char charResult;
    int flags = 0;

    *endPos = inputString;
    while ('\0' != **endPos
                  && 0 == bsl::isspace(static_cast<unsigned char>(**endPos))) {
        int tmpFlags = 0;
        if (parseCharRaw(endPos, &charResult, *endPos, &tmpFlags)) {
            return BDEPU_FAILURE;
        }
        flags |= tmpFlags;
    }

    if (*endPos == inputString) {
        return BDEPU_FAILURE;
    }

    // Copy into return result
    result->assign(inputString, *endPos - inputString);
    if (flags & BDEPU_HAS_ESCAPE) {
        bdepu_ParserImpUtil::compactMultibyteChars(result);
    }
    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseUnquotedString(const char  **endPos,
                                                  bsl::string  *result,
                                                  const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    char charResult;
    int flags = 0;

    *endPos = inputString;
    while ('\0' != **endPos && '"' != **endPos
                  && 0 == bsl::isspace(static_cast<unsigned char>(**endPos))) {
        int tmpFlags = 0;
        if (parseCharRaw(endPos, &charResult, *endPos, &tmpFlags)) {
            return BDEPU_FAILURE;
        }
        flags |= tmpFlags;
    }

    if (*endPos == inputString) {
        return BDEPU_FAILURE;
    }

    // Copy into return result
    result->assign(inputString, *endPos - inputString);
    if (flags & BDEPU_HAS_ESCAPE) {
        bdepu_ParserImpUtil::compactMultibyteChars(result);
    }
    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseTime(const char **endPos,
                                        bdet_Time   *result,
                                        const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    Uint64 tmp;

    int hour;
    if (0 != bdepu_ParserImpUtil::parseUnsignedInteger(endPos,
                                                       &tmp,
                                                       inputString,
                                                       10,
                                                       99,
                                                       2)) {
        return BDEPU_FAILURE;
    }
    hour = tmp;
    if (hour > 24) {
        *endPos -= 2;
        return BDEPU_FAILURE;
    }
    if (**endPos != ':') {
        return BDEPU_FAILURE;
    }
    ++*endPos;

    int minute;
    const char *start = *endPos;
    if (0 != bdepu_ParserImpUtil::parseUnsignedInteger(endPos,
                                                       &tmp,
                                                       *endPos,
                                                       10,
                                                       99,
                                                       2)) {
        return BDEPU_FAILURE;
    }
    minute = tmp;
    if (minute > 59) {
        *endPos -= 2;
        return BDEPU_FAILURE;
    }
    if ( 2 != (*endPos - start)) { // minutes MUST have 2 digits
        return BDEPU_FAILURE;
    }
    if (24 == hour && minute) {
        *endPos = start + (minute < 10 ? 1 : 0);
        return BDEPU_FAILURE;
    }

    int second      = 0;
    int millisecond = 0;
    if (':' == **endPos) {  // there is a seconds field present
        ++*endPos;
        start = *endPos;
        if (0 != bdepu_ParserImpUtil::parseUnsignedInteger(endPos,
                                                           &tmp,
                                                           *endPos,
                                                           10,
                                                           99,
                                                           2)) {
            return BDEPU_FAILURE;
        }
        second = tmp;
        if (second > 59) {
            *endPos -= 2;
            return BDEPU_FAILURE;
        }
        if ( 2 != (*endPos - start)) { // seconds MUST have 2 digits
            return BDEPU_FAILURE;
        }
        if (24 == hour && second) {
            *endPos = start + (second < 10 ? 1 : 0);
            return BDEPU_FAILURE;
        }

        if ('.' == **endPos) {
            ++*endPos;
            start = *endPos;
            if (0 != bdepu_ParserImpUtil::parseUnsignedInteger(endPos,
                                                               &tmp,
                                                               *endPos,
                                                               10,
                                                               999,
                                                               3)) {
                return BDEPU_FAILURE;
            }
            millisecond = tmp;
            if (2 == *endPos - start) {
                millisecond *= 10;
            }
            if (1 == *endPos - start) {
                millisecond *= 100;
            }
            if (24 == hour && millisecond) {
                *endPos = start;
                if ('0' == **endPos) {
                    ++*endPos;
                    if ('0' == **endPos) {
                        ++*endPos;
                    }
                }
                return BDEPU_FAILURE;
            }
        }
    }

    result->setTime(hour, minute, second, millisecond);
    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseTimeTz(const char  **endPos,
                                          bdet_TimeTz  *result,
                                          const char   *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    bdet_Time resultTime;

    if (parseTime(endPos, &resultTime, inputString)) {
        return BDEPU_FAILURE;
    }

    int resultOffset;

    if (parseTz(endPos, &resultOffset, *endPos)) {
        return BDEPU_FAILURE;
    }

    *result = bdet_TimeTz(resultTime, resultOffset);

    return BDEPU_SUCCESS;
}

int bdepu_TypesParserImpUtil::parseTz(const char **endPos,
                                      int         *result,
                                      const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(result);
    BSLS_ASSERT(inputString);

    *endPos = inputString;

    int sign;
    if ('+' == *inputString) {
        sign = 1;
    }
    else if ('-' == *inputString) {
        sign = -1;
    }
    else {
        return BDEPU_FAILURE;
    }

    ++*endPos;

    Uint64 hours = 0;
    if (bdepu_ParserImpUtil::parseUnsignedInteger(endPos,
                                                  &hours,
                                                  *endPos,
                                                  10,
                                                  99,
                                                  2)) {
        return BDEPU_FAILURE;
    }

    // Ensure that exactly 2 digits have been read.
    if (3 != (*endPos - inputString)) {
        return BDEPU_FAILURE;
    }

    if (hours >= 30) {
        *endPos = inputString + 1;
        return BDEPU_FAILURE;
    }
    else if (hours >= 24) {
        *endPos = inputString + 2;
        return BDEPU_FAILURE;
    }

    Uint64 minutes = 0;
    if (bdepu_ParserImpUtil::parseUnsignedInteger(endPos,
                                                  &minutes,
                                                  *endPos,
                                                  10,
                                                  99,
                                                  2)) {
        return BDEPU_FAILURE;
    }

    // Ensure that exactly 2 digits have been read.
    if (5 != (*endPos - inputString)) {
        return BDEPU_FAILURE;
    }
    if (minutes >= 60) {
        *endPos = inputString + 4;
        return BDEPU_FAILURE;
    }

    *result = hours * 60 + minutes;

    if (sign < 0) {
        *result = -*result;
    }

    return BDEPU_SUCCESS;
}

                        // *** GENERATING FUNCTIONS ***

int bdepu_TypesParserImpUtil::generateLongDouble(char        *buffer,
                                                 long double  value,
                                                 int          length)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);

    enum { BUF_LEN = BDEPU_MAX_LONGDOUBLE_STRLEN10 };
    char tempBuf[BUF_LEN];

    // There is no reason to use the non-standard snprintf, since we are
    // guaranteed that BUF_LEN is large enough for all our platforms.
    // Also, snprintf returns -1 on truncation on some platforms, making it
    // impossible to compute the return value.

    const int strLen = bsl::sprintf(tempBuf, "%.6Lg", value);
    return copyBuf(buffer, length, tempBuf, strLen);
}

int bdepu_TypesParserImpUtil::generateDouble(char   *buffer,
                                             double  value,
                                             int     length)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);

    enum { BUF_LEN = BDEPU_MAX_DOUBLE_STRLEN10 };
    char tempBuf[BUF_LEN];
    const int strLen = bsl::sprintf(tempBuf, "%.6g", value);
    return copyBuf(buffer, length, tempBuf, strLen);
}

int bdepu_TypesParserImpUtil::generateFloat(char  *buffer,
                                            float  value,
                                            int    length)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);

    enum { BUF_LEN = BDEPU_MAX_FLOAT_STRLEN10 };
    char tempBuf[BUF_LEN];
    const int strLen = bsl::sprintf(tempBuf, "%.6g", (double)value);
    return copyBuf(buffer, length, tempBuf, strLen);
}

int bdepu_TypesParserImpUtil::generateInt64(char                     *buffer,
                                            bsls_PlatformUtil::Int64  value,
                                            int                       length)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);

    enum { BUF_LEN = BDEPU_MAX_INT64_STRLEN10 };
    char tempBuf[BUF_LEN];
    char *strBegin = generateIntDecimalHelper<Uint64>(tempBuf, value, BUF_LEN);
    const int strLen = (tempBuf + BUF_LEN) - strBegin;
    return copyBuf(buffer, length, strBegin, strLen);
}

int bdepu_TypesParserImpUtil::generateInt64(char                     *buffer,
                                            bsls_PlatformUtil::Int64  value,
                                            int                       length,
                                            int                       base)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    enum { BUF_LEN = BDEPU_MAX_INT64_STRLEN };
    char tempBuf[BUF_LEN];
    char *strBegin = generateIntHelper<Uint64>(tempBuf,
                                               value,
                                               BUF_LEN,
                                               base);
    const int strLen = (tempBuf + BUF_LEN) - strBegin;
    return copyBuf(buffer, length, strBegin, strLen);
}

int bdepu_TypesParserImpUtil::generateInt(char *buffer, int value, int length)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);

    enum { BUF_LEN = BDEPU_MAX_INT_STRLEN10 };
    char tempBuf[BUF_LEN];
    char *strBegin = generateIntDecimalHelper<unsigned>(tempBuf,
                                                        value,
                                                        BUF_LEN);
    const int strLen = (tempBuf + BUF_LEN) - strBegin;
    return copyBuf(buffer, length, strBegin, strLen);
}

int bdepu_TypesParserImpUtil::generateInt(char *buffer,
                                          int   value,
                                          int   length,
                                          int   base)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    enum { BUF_LEN = BDEPU_MAX_INT_STRLEN };
    char tempBuf[BUF_LEN];
    char *strBegin;
    if (10 == base) {
        strBegin = generateIntDecimalHelper<unsigned>(tempBuf, value, BUF_LEN);
    } else {
        strBegin = generateIntHelper<unsigned>(tempBuf,
                                               value,
                                               BUF_LEN,
                                               base);
    }
    const int strLen = (tempBuf + BUF_LEN) - strBegin;
    return copyBuf(buffer, length, strBegin, strLen);
}

int bdepu_TypesParserImpUtil::generateLongDoubleRaw(char        *buffer,
                                                    long double  value)
{
    BSLS_ASSERT(buffer);

    // Outputs the null terminator anyway.
    return bsl::sprintf(buffer, "%.6Lg", value);
}

int bdepu_TypesParserImpUtil::generateDoubleRaw(char *buffer, double value)
{
    BSLS_ASSERT(buffer);

    // Outputs the null terminator anyway.
    return bsl::sprintf(buffer, "%.6g", value);
}

int bdepu_TypesParserImpUtil::generateFloatRaw(char *buffer, float value)
{
    BSLS_ASSERT(buffer);

    // Outputs the null terminator anyway.
    return bsl::sprintf(buffer, "%.6g", (double)value);
}

int bdepu_TypesParserImpUtil::generateInt64Raw(
                                              char                     *buffer,
                                              bsls_PlatformUtil::Int64  value)
{
    BSLS_ASSERT(buffer);

    enum { BUF_LEN = BDEPU_MAX_INT64_STRLEN10 };
    char tempBuf[BUF_LEN];
    char *strBegin = generateIntDecimalHelper<Uint64>(tempBuf, value, BUF_LEN);
    const int strLen = (tempBuf + BUF_LEN) - strBegin;
    return copyBufRaw(buffer, strBegin, strLen);
}

int bdepu_TypesParserImpUtil::generateInt64Raw(
                                              char                     *buffer,
                                              bsls_PlatformUtil::Int64  value,
                                              int                       base)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    enum { BUF_LEN = BDEPU_MAX_INT64_STRLEN };
    char tempBuf[BUF_LEN];
    char *strBegin;
    if (10 == base) {
        strBegin = generateIntDecimalHelper<Uint64>((char*)tempBuf,
                                                    value,
                                                    BUF_LEN);
    } else {
        strBegin = generateIntHelper<Uint64>((char*)tempBuf,
                                             value,
                                             BUF_LEN,
                                             base);
    }
    const int strLen = (tempBuf + BUF_LEN) - strBegin;
    return copyBufRaw(buffer, strBegin, strLen);
}

int bdepu_TypesParserImpUtil::generateIntRaw(char *buffer, int value)
{
    BSLS_ASSERT(buffer);

    enum { BUF_LEN = BDEPU_MAX_INT_STRLEN10 };
    char tempBuf[BUF_LEN];
    char *strBegin;
    strBegin = generateIntDecimalHelper<unsigned>((char*)tempBuf,
                                                  value,
                                                  BUF_LEN);
    const int strLen = (tempBuf + BUF_LEN) - strBegin;
    return copyBufRaw(buffer, strBegin, strLen);
}

int bdepu_TypesParserImpUtil::generateIntRaw(char *buffer, int value, int base)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    enum { BUF_LEN = BDEPU_MAX_INT_STRLEN };
    char tempBuf[BUF_LEN];
    char *strBegin;
    if (10 == base) {
        strBegin = generateIntDecimalHelper<unsigned>((char*)tempBuf,
                                                      value,
                                                      BUF_LEN);
    } else {
        strBegin = generateIntHelper<unsigned>(tempBuf,
                                               value,
                                               BUF_LEN,
                                               base);
    }
    const int strLen = (tempBuf + BUF_LEN) - strBegin;
    return copyBufRaw(buffer, strBegin, strLen);
}

void bdepu_TypesParserImpUtil::generateCharRaw(bsl::vector<char> *buffer,
                                               char               value)
{
    BSLS_ASSERT(buffer);

    // Relevant values of 'enum' used to populate 's_charTypes'
    const int HEX   = 12;
    const int PRINT = 13;

    switch (s_charTypes[(unsigned char)value]) {
      case HEX: {
        // generate bsl::hex
        buffer->push_back('\\');
        buffer->push_back('x');
        buffer->push_back(s_hex[(value >> 4) & 0xF]);
        buffer->push_back(s_hex[value & 0xF]);
      } break;
      case PRINT: {
        buffer->push_back(value);
      } break;
      default: {
        int index = s_charTypes[(unsigned char)value];
        buffer->push_back('\\');
        buffer->push_back(s_sesStrings[index]);
      } break;
    }
}

void bdepu_TypesParserImpUtil::generateDate(bsl::vector<char> *buffer,
                                            const bdet_Date&   value)
{
    BSLS_ASSERT(buffer);

    int year, month, day;
    value.getYearMonthDay(&year, &month, &day);
    int len = buffer->size();
    buffer->resize(len + 10);
    (*buffer)[  len] = year / 1000 + '0';
    (*buffer)[++len] = (year / 100) % 10 + '0';
    (*buffer)[++len] = (year / 10) % 10 + '0';
    (*buffer)[++len] = year % 10 + '0';
    (*buffer)[++len] = '/';
    (*buffer)[++len] = month / 10 + '0';
    (*buffer)[++len] = month % 10 + '0';
    (*buffer)[++len] = '/';
    (*buffer)[++len] = day / 10 + '0';
    (*buffer)[++len] = day % 10 + '0';
}

void bdepu_TypesParserImpUtil::generateDouble(bsl::vector<char> *buffer,
                                              double             value)
{
    BSLS_ASSERT(buffer);

    // Problem: even with "%.6g" formatter, some outputs are completely wrong,
    // e.g. (on Solaris + SunPRO compiler, from test driver, with float):
    //..
    //  C++ source value        oss.str()      output
    //  -----------------       ------------   ---------------------
    //  1234567890.12345F       1.23457e+09    1.23456793599999999e9
    // .123456789012345E30F     1.23457e+29    1.23456789182729269e29
    // .123456789012345E-30F    1.23457e-31    1.23456787902738074e-31
    //..
    // (See below for definition of oss.str().)
    // Same problems with double and long double.
    //
    // Prefer the generateDouble(char *, ...) versions that use sprintf.

    int    binSign;
    Uint64 binFrac;
    int    binExp;

    bdepu_RealParserImpUtil::convertDoubleToBinary(&binSign,
                                                   &binFrac,
                                                   &binExp,
                                                   value);

    Uint64 decFrac;
    int    decExp;

    bdepu_RealParserImpUtil::convertBinaryToDecimal(&decFrac,
                                                    &decExp,
                                                    binFrac,
                                                    binExp);

    // Attempt to shorten the representation.

    int trailingDigits = decFrac % 1000;
    if (trailingDigits < 100) {
        decFrac -= trailingDigits;
    }
    else if (trailingDigits > 900) {
        decFrac = decFrac - trailingDigits + 1000;
        if (decFrac >= 1000000000000000000uLL) {
            decFrac /= 10;
            ++decExp;
        }
    }

    if (binSign) {
        buffer->push_back('-');
    }

    if (decExp > 3 || decExp < -1) {
        buffer->push_back('0' + decFrac / 100000000000000000uLL);
        decFrac = (decFrac % 100000000000000000uLL) * 10;
        if (decFrac) {
            buffer->push_back('.');
            while (decFrac) {
                buffer->push_back('0' + decFrac / 100000000000000000uLL);
                decFrac = (decFrac % 100000000000000000uLL) * 10;
            }
        }
        buffer->push_back('e');
        generateInt(buffer, decExp - 1);
    }
    else if (decExp > 0) {
        while (decExp) {
            buffer->push_back('0' + decFrac / 100000000000000000uLL);
            decFrac = (decFrac % 100000000000000000uLL) * 10;
            --decExp;
        }
        if (decFrac) {
            buffer->push_back('.');
            while (decFrac) {
                buffer->push_back('0' + decFrac / 100000000000000000uLL);
                decFrac = (decFrac % 100000000000000000uLL) * 10;
            }
        }
    }
    else if (0 == decExp && 0 == decFrac) {
        buffer->push_back('0');
        buffer->push_back('.');
        buffer->push_back('0');
    }
    else {
        buffer->push_back('0');
        buffer->push_back('.');
        while (decExp) {
            buffer->push_back('0');
            ++decExp;
        }
        while (decFrac) {
            buffer->push_back('0' + decFrac / 100000000000000000uLL);
            decFrac = (decFrac % 100000000000000000uLL) * 10;
        }
    }
}

void bdepu_TypesParserImpUtil::generateFloat(bsl::vector<char> *buffer,
                                             float              value)
{
    BSLS_ASSERT(buffer);

    // Problem: even with "%.6g" formatter, some outputs are completely wrong,
    // e.g. (on Solaris + SunPRO compiler, from test driver, with float):
    //..
    //  C++ source value        oss.str()      output
    //  -----------------       ------------   ---------------------
    //  1234567890.12345F       1.23457e+09    1.23456793599999999e9
    // .123456789012345E30F     1.23457e+29    1.23456789182729269e29
    // .123456789012345E-30F    1.23457e-31    1.23456787902738074e-31
    //..
    // (See below for definition of oss.str().)
    // Same problems with double and long double.
    //
    // Prefer the generateDouble(char *, ...) versions that use sprintf.

    int    binSign;
    Uint64 binFrac;
    int    binExp;
    bdepu_RealParserImpUtil::convertDoubleToBinary(&binSign,
                                                   &binFrac,
                                                   &binExp,
                                                   value);

    Uint64 decFrac;
    int    decExp;
    bdepu_RealParserImpUtil::convertBinaryToDecimal(&decFrac,
                                                    &decExp,
                                                    binFrac,
                                                    binExp);

    // Attempt to shorten the representation.

    Uint64 trailingDigits = decFrac % 1000000000000uLL;
    if (trailingDigits < 100000000000uLL) {
        decFrac -= trailingDigits;
    }
    else if (trailingDigits > 900000000000uLL) {
        decFrac = decFrac - trailingDigits + 1000000000000uLL;
        if (decFrac >= 1000000000000000000uLL) {
            decFrac /= 10;
            ++decExp;
        }
    }

    if (binSign) {
        buffer->push_back('-');
    }

    if (decExp > 3 || decExp < -1) {
        buffer->push_back('0' + decFrac / 100000000000000000uLL);
        decFrac = (decFrac % 100000000000000000uLL) * 10;
        if (decFrac) {
            buffer->push_back('.');
            while (decFrac) {
                buffer->push_back('0' + decFrac / 100000000000000000uLL);
                decFrac = (decFrac % 100000000000000000uLL) * 10;
            }
        }
        buffer->push_back('e');
        generateInt(buffer, decExp - 1);
    }
    else if (decExp > 0) {
        while (decExp) {
            buffer->push_back('0' + decFrac / 100000000000000000uLL);
            decFrac = (decFrac % 100000000000000000uLL) * 10;
            --decExp;
        }
        if (decFrac) {
            buffer->push_back('.');
            while (decFrac) {
                buffer->push_back('0' + decFrac / 100000000000000000uLL);
                decFrac = (decFrac % 100000000000000000uLL) * 10;
            }
        }
    }
    else if (0 == decExp && 0 == decFrac) {
        buffer->push_back('0');
        buffer->push_back('.');
        buffer->push_back('0');
    }
    else {
        buffer->push_back('0');
        buffer->push_back('.');
        while (decExp) {
            buffer->push_back('0');
            ++decExp;
        }
        while (decFrac) {
            buffer->push_back('0' + decFrac / 100000000000000000uLL);
            decFrac = (decFrac % 100000000000000000uLL) * 10;
        }
    }
}

void bdepu_TypesParserImpUtil::generateInt64(bsl::vector<char>        *buffer,
                                             bsls_PlatformUtil::Int64  value,
                                             int                       base)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    generateIntHelper<Uint64>(buffer, value, base);
}

void bdepu_TypesParserImpUtil::generateInt(bsl::vector<char> *buffer,
                                           int                value,
                                           int                base)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(     base <= 36);

    generateIntHelper<unsigned>(buffer, value, base);
}

void bdepu_TypesParserImpUtil::generateString(bsl::vector<char> *buffer,
                                              const char        *value)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(value);

    // Does string need quotes?
    int quote = ('\0' == *value || ']' == *value ||
                      ('/' == *value && ('/' == value[1] || '*' == value[1])));
    if (0 == quote) {
        const char *tmp = value;
        while (*tmp != '\0') {
            if (bsl::isspace(static_cast<unsigned char>(*tmp))) {
                quote = 1;
                break;
            }
            ++tmp;
        }
    }

    if (quote) {
        buffer->push_back('"');
    }

    // Now write out the real text
    const char *tmp = value;
    char v;
    while ((v = *tmp) != '\0') {  // = O.K.
        generateCharRaw(buffer, v);
        ++tmp;
    }

    if (quote) {
        buffer->push_back('"');
    }
}

void bdepu_TypesParserImpUtil::generateTime(bsl::vector<char> *buffer,
                                            const bdet_Time&   value)
{
    BSLS_ASSERT(buffer);

    int hour, minute, second, millisecond;
    value.getTime(&hour, &minute, &second, &millisecond);
    int len = buffer->size();
    buffer->resize(len + 12);
    (*buffer)[  len] = hour / 10 + '0';
    (*buffer)[++len] = hour % 10 + '0';
    (*buffer)[++len] = ':';
    (*buffer)[++len] = minute / 10 + '0';
    (*buffer)[++len] = minute % 10 + '0';
    (*buffer)[++len] = ':';
    (*buffer)[++len] = second / 10 + '0';
    (*buffer)[++len] = second % 10 + '0';
    (*buffer)[++len] = '.';
    (*buffer)[++len] = millisecond / 100 + '0';
    (*buffer)[++len] = (millisecond / 10) % 10 + '0';
    (*buffer)[++len] = millisecond % 10 + '0';
}

void bdepu_TypesParserImpUtil::generateTz(bsl::vector<char> *buffer,
                                          int                offset)
{
    BSLS_ASSERT(buffer);

    int len = buffer->size();

    buffer->resize(len + 5);

    if (offset < 0) {
        (*buffer)[len] = '-';
        offset = -offset;
    }
    else {
        (*buffer)[len] = '+';
    }

    const int minute = offset % 60;
    const int hour   = (offset / 60) % 24;
    (*buffer)[++len] = hour / 10 + '0';
    (*buffer)[++len] = hour % 10 + '0';
    (*buffer)[++len] = minute / 10 + '0';
    (*buffer)[++len] = minute % 10 + '0';
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
