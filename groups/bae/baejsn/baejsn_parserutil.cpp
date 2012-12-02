// baejsn_parserutil.cpp                                              -*-C++-*-
#include <baejsn_parserutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_parserutil_cpp,"$Id$ $CSID$")

#include <bdema_bufferedsequentialallocator.h>

#include <bsl_algorithm.h>
#include <bsl_cmath.h>
#include <bsl_cctype.h>
#include <bsl_cerrno.h>
#include <bsl_cstdlib.h>
#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace {

const char hexValueTable[128] =
{
    0  , //   0   0
    0  , //   1   1
    0  , //   2   2
    0  , //   3   3
    0  , //   4   4
    0  , //   5   5
    0  , //   6   6
    0  , //   7   7
    0  , //   8   8 - BACKSPACE
    0  , //   9   9 - TAB
    0  , //  10   a - LF
    0  , //  11   b
    0  , //  12   c
    0  , //  13   d - CR
    0  , //  14   e
    0  , //  15   f
    0  , //  16  10
    0  , //  17  11
    0  , //  18  12
    0  , //  19  13
    0  , //  20  14
    0  , //  21  15
    0  , //  22  16
    0  , //  23  17
    0  , //  24  18
    0  , //  25  19
    0  , //  26  1a
    0  , //  27  1b
    0  , //  28  1c
    0  , //  29  1d
    0  , //  30  1e
    0  , //  31  1f
    0  , //  32  20 - SPACE
    0  , //  33  21 - !
    0  , //  34  22 - "
    0  , //  35  23 - #
    0  , //  36  24 - $
    0  , //  37  25 - %
    0  , //  38  26 - &
    0  , //  39  27 - '
    0  , //  40  28 - (
    0  , //  41  29 - )
    0  , //  42  2a - *
    0  , //  43  2b - +
    0  , //  44  2c - ,
    0  , //  45  2d - -
    0  , //  46  2e - .
    0  , //  47  2f - /
    0  , //  48  30 - 0
    1  , //  49  31 - 1
    2  , //  50  32 - 2
    3  , //  51  33 - 3
    4  , //  52  34 - 4
    5  , //  53  35 - 5
    6  , //  54  36 - 6
    7  , //  55  37 - 7
    8  , //  56  38 - 8
    9  , //  57  39 - 9
    0  , //  58  3a - :
    0  , //  59  3b - ;
    0  , //  60  3c - <
    0  , //  61  3d - =
    0  , //  62  3e - >
    0  , //  63  3f - ?
    0  , //  64  40 - @
    10 , //  65  41 - A
    11 , //  66  42 - B
    12 , //  67  43 - C
    13 , //  68  44 - D
    14 , //  69  45 - E
    15 , //  70  46 - F
    0  , //  71  47 - G
    0  , //  72  48 - H
    0  , //  73  49 - I
    0  , //  74  4a - J
    0  , //  75  4b - K
    0  , //  76  4c - L
    0  , //  77  4d - M
    0  , //  78  4e - N
    0  , //  79  4f - O
    0  , //  80  50 - P
    0  , //  81  51 - Q
    0  , //  82  52 - R
    0  , //  83  53 - S
    0  , //  84  54 - T
    0  , //  85  55 - U
    0  , //  86  56 - V
    0  , //  87  57 - W
    0  , //  88  58 - X
    0  , //  89  59 - Y
    0  , //  90  5a - Z
    0  , //  91  5b - [
    0  , //  92  5c - '\'
    0  , //  93  5d - ]
    0  , //  94  5e - ^
    0  , //  95  5f - _
    0  , //  96  60 - `
    10 , //  97  61 - a
    11 , //  98  62 - b
    12 , //  99  63 - c
    13 , // 100  64 - d
    14 , // 101  65 - e
    15 , // 102  66 - f
    0  , // 103  67 - g
    0  , // 104  68 - h
    0  , // 105  69 - i
    0  , // 106  6a - j
    0  , // 107  6b - k
    0  , // 108  6c - l
    0  , // 109  6d - m
    0  , // 110  6e - n
    0  , // 111  6f - o
    0  , // 112  70 - p
    0  , // 113  71 - q
    0  , // 114  72 - r
    0  , // 115  73 - s
    0  , // 116  74 - t
    0  , // 117  75 - u
    0  , // 118  76 - v
    0  , // 119  77 - w
    0  , // 120  78 - x
    0  , // 121  79 - y
    0  , // 122  7a - z
    0  , // 123  7b - {
    0  , // 124  7c - |
    0  , // 125  7d - }
    0  , // 126  7e - ~
    0    // 127  7f - DEL
};

int getUnicodeChar(char *value, const char *iter)
{
    if ('\0' == iter[0]
     || '\0' == iter[1]
     || '\0' == iter[2]
     || '\0' == iter[3]) {
        return -1;                                                    // RETURN
    }

    // TBD: Get the code point and insert corresponding UTF-8 chars.  For now
    // we only accept Basic Latin characters (U+0000 - U+007F).

    if ('0' != iter[0]
     || '0' != iter[1]
     || !('0' <= iter[2] && iter[2] <= '7')
     || !bsl::isxdigit(iter[3])) {
        return -1;                                                    // RETURN
    }

    // hex encoded, pull the next 4 bytes

    const char charValue = (char)(((int)hexValueTable[iter[2]] << 4)
                                | ((int)hexValueTable[iter[3]]));

    *value = charValue;

    return 0;
}

inline
bool isValidNextChar(int nextChar)
{
    return bsl::streambuf::traits_type::eof() == nextChar
        || bsl::isspace(nextChar)
        || ',' == static_cast<char>(nextChar)
        || ']' == static_cast<char>(nextChar)
        || '}' == static_cast<char>(nextChar);
}

int getUnicodeChar(bsl::streambuf *streamBuf, bsl::string *value)
{
    const int SIZE = 4;
    char      buffer[SIZE];

    const int numRead = streamBuf->sgetn(buffer, SIZE);
    if (SIZE != numRead) {
        return -1;                                                    // RETURN
    }

    // TBD: Get the code point and insert corresponding UTF-8 chars.  For now
    // we only accept Basic Latin characters (U+0000 - U+007F).

    if ('0' != buffer[0]
     || '0' != buffer[1]
     || !('0' <= buffer[2] && buffer[2] <= '7')
     || !bsl::isxdigit(buffer[3])) {
        return -1;                                                    // RETURN
    }

    // hex encoded, pull the next 4 bytes

    const char charValue = (char)(((int)hexValueTable[buffer[2]] << 4)
                                | ((int)hexValueTable[buffer[3]]));

    *value += charValue;

    streamBuf->sungetc();

    return 0;
}

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

                ++iter;

                enum { NUM_UNICODE_DIGITS = 4 };

                if (iter + NUM_UNICODE_DIGITS >= end) {
                    return -1;                                        // RETURN
                }

                char charValue;
                if (0 != getUnicodeChar(&charValue, iter)) {
                    return -1;                                        // RETURN
                }

                *value += charValue;

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
    if (0 == data.length()) {
        return -1;                                                    // RETURN
    }

    char   *endPtr = 0;
    errno          = 0;
    double  tmp    = bsl::strtod(data.data(), &endPtr);

    if (endPtr    != data.data() + data.length()
     || (0        == tmp && 0 != errno)
     ||  HUGE_VAL == tmp
     || -HUGE_VAL == tmp) {
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

    // Extract significant digits

    const char *valueBegin = iter;
    while (iter < end && bsl::isdigit(*iter)) {
        ++iter;
    }
    const char *valueEnd = iter;

    // Extract fractional digits if specified

    const char *fractionalBegin = 0;
    const char *fractionalEnd   = 0;
    if (iter < end && '.' == *iter) {

        fractionalBegin = ++iter;
        while (iter < end && bsl::isdigit(*iter)) {
            ++iter;
        }
        fractionalEnd = iter;
    }

    // Extract exponent digits if specified

    int  exponent = 0;
    bool isExpNegative;
    if ('E' == static_cast<char>(bsl::toupper(*iter))) {

        // extract the exponent part

        ++iter;
        if ('-' == *iter) {
            isExpNegative = true;
            ++iter;
        }
        else {
            if ('+' == *iter) {
                ++iter;
            }
            isExpNegative = false;
        }

        while (iter < end && bsl::isdigit(*iter)) {
            exponent = exponent * 10 + *iter - '0';
            ++iter;
        }
    }

    if (iter < end && !bsl::isdigit(*iter)) {
        return -1;                                                    // RETURN
    }

    int numFractionalDigits = fractionalEnd - fractionalBegin;
    int numAdditionalDigits = 0;
    if (isExpNegative) {
        if (valueEnd - valueBegin >= exponent) {
            valueEnd -= exponent;
            exponent = 0;
        }
        else {
            return -1;                                                // RETURN
        }
    }
    else {
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

    iter = valueBegin;
    while (iter != valueEnd) {
        if (static_cast<bsls::Types::Uint64>(tmp * 10 + *iter - '0') >
                            bsl::numeric_limits<bsls::Types::Uint64>::max()) {
            return -1;                                                // RETURN
        }
        tmp = tmp * 10 + *iter - '0';
        ++iter;
    }

    if (numAdditionalDigits) {
        // Disallow non-zero fractional digits

        iter = fractionalBegin;
        for (int i = 0; i < numAdditionalDigits; ++i, ++iter) {
            if (static_cast<bsls::Types::Uint64>(tmp * 10 + *iter - '0') >
                            bsl::numeric_limits<bsls::Types::Uint64>::max()) {
                return -1;                                            // RETURN
            }
            tmp = tmp * 10 + *iter - '0';
        }
        fractionalBegin = iter;
    }

    while (fractionalBegin < fractionalEnd) {
        if ('0' != *fractionalBegin) {
            return -1;                                                // RETURN
        }
        ++fractionalBegin;
    }

    if (exponent) {
        if (static_cast<double>(tmp * bsl::pow(10.0, exponent) <
                                         bsl::numeric_limits<double>::max())) {
            tmp *= static_cast<bsls::Types::Uint64>(bsl::pow(10.0, exponent));
        }
        else {
            return -1;                                                // RETURN
        }
    }

    *value = tmp;
    return 0;
}

// DEPRECATED CLASS METHODS
void baejsn_ParserUtil::skipSpaces(bsl::streambuf *streamBuf)
{
    int ch = streamBuf->sgetc();
    while (bsl::isspace(ch)) {
        ch = streamBuf->snextc();
    }
}

int baejsn_ParserUtil::getDouble(bsl::streambuf *streamBuf, double *value)
{
    skipSpaces(streamBuf);

    const int                         SIZE = 64;
    char                              buffer[SIZE];
    bdema_BufferedSequentialAllocator allocator(buffer, SIZE);
    bsl::string                       str(&allocator);

    int ch = streamBuf->sgetc();
    if ('-' == static_cast<char>(ch)) {
        str += static_cast<char>(ch);
        ch   = streamBuf->snextc();
    }

    while (bsl::isdigit(ch)) {
        str += static_cast<char>(ch);
        ch   = streamBuf->snextc();
    }

    if ('.' == static_cast<char>(ch)) {
        str += static_cast<char>(ch);
        ch   = streamBuf->snextc();

        while (bsl::isdigit(ch)) {
            str += static_cast<char>(ch);
            ch   = streamBuf->snextc();
        }
    }

    if ('E' == static_cast<char>(bsl::toupper(ch))) {
        str += static_cast<char>(ch);
        ch   = streamBuf->snextc();
        if ('+' == static_cast<char>(ch) || '-' == static_cast<char>(ch)) {
            str += static_cast<char>(ch);
            ch   = streamBuf->snextc();
        }

        while (bsl::isdigit(ch)) {
            str += static_cast<char>(ch);
            ch   = streamBuf->snextc();
        }
    }

    if (isValidNextChar(streamBuf->sgetc())) {
        char   *end = 0;
        errno       = 0;
        double  tmp = bsl::strtod(str.c_str(), &end);

        if (end       == str.data()
         || (0        == tmp && 0 != errno)
         || HUGE_VAL  == tmp
         || -HUGE_VAL == tmp) {
            return -1;                                                // RETURN
        }
        *value = tmp;
        return 0;                                                     // RETURN
    }
    return -1;
}

int baejsn_ParserUtil::getUint64(bsl::streambuf      *streamBuf,
                                 bsls::Types::Uint64 *value)
{
    skipSpaces(streamBuf);

    const int                         BUF_SIZE   = 128;
    const int                         STRING_LEN =  56;
    char                              buffer[BUF_SIZE];
    bdema_BufferedSequentialAllocator allocator(buffer, BUF_SIZE);
    bsl::string                       str(&allocator);
    str.reserve(STRING_LEN);

    int ch = streamBuf->sgetc();
    while (bsl::isdigit(ch)) {
        str += static_cast<char>(ch);
        ch   = streamBuf->snextc();
    }

    bsl::string fractionalStr(&allocator);
    fractionalStr.reserve(STRING_LEN);

    if ('.' == static_cast<char>(ch)) {

        // Store the fractional portion in 'fractionalStr'

        ch = streamBuf->snextc();
        while (bsl::isdigit(ch)) {
            fractionalStr += static_cast<char>(ch);
            ch             = streamBuf->snextc();
        }
    }

    int exponent = 0;
    if ('E' == static_cast<char>(bsl::toupper(ch))) {

        // extract the exponent part

        ch = streamBuf->snextc();
        bool isNegative;
        if ('-' == ch) {
            isNegative = true;
            ch = streamBuf->snextc();
        }
        else {
            isNegative = false;
            if ('+' == ch) {
                ch = streamBuf->snextc();
            }
        }

        while (bsl::isdigit(ch)) {
            exponent *= 10;
            exponent += ch - '0';
            ch = streamBuf->snextc();
        }

        if (isNegative) {
            const int newLen =
                        bsl::max(0, static_cast<int>(str.length()) - exponent);
            str.resize(newLen);
        }
        else {
            if (static_cast<bsl::size_t>(exponent) < fractionalStr.length()) {
                fractionalStr.erase(fractionalStr.begin() + exponent,
                                    fractionalStr.end());
            }
            else {
                fractionalStr.append(exponent - fractionalStr.length(), '0');
            }
            str.append(fractionalStr);
        }
    }

    *value = 0;

    bsl::string::const_iterator iter = str.begin();
    while (iter != str.end()) {
        if (static_cast<double>(*value * 10 + *iter - '0') >
            static_cast<double>(
                            bsl::numeric_limits<bsls::Types::Uint64>::max())) {
            return -1;                                                // RETURN
        }
        *value = *value * 10 + *iter - '0';
        ++iter;
    }

    return isValidNextChar(streamBuf->sgetc()) ? 0 : -1;
}

int baejsn_ParserUtil::getString(bsl::streambuf *streamBuf, bsl::string *value)
{
    skipSpaces(streamBuf);

    value->clear();

    int ch = streamBuf->sgetc();
    if (ch != bsl::streambuf::traits_type::eof()) {
        if (ch != '"') {
            return -1;                                                // RETURN
        }
        ch = streamBuf->snextc();
    }

    bool escaped = false;
    while (ch != bsl::streambuf::traits_type::eof()) {
        if (escaped) {
            switch (ch) {
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

                *value += ch;
              } break;

              case 'u':
              case 'U': {
                streamBuf->snextc();

                if (0 != getUnicodeChar(streamBuf, value)) {
                    return -1;                                        // RETURN
                }
              } break;

              default: {
                return -1;                                            // RETURN
              } break;
            }

            escaped = false;
        }
        else {
            if (ch == '\\') {
                escaped = true;
            }
            else if (ch == '"') {
                streamBuf->snextc();
                return 0;                                             // RETURN
            }
            else {
                *value += ch;
            }
        }

        ch = streamBuf->snextc();
    }

    return -1;
}

int baejsn_ParserUtil::eatToken(bsl::streambuf *streamBuf, const char *token)
{
    int             ch  = streamBuf->sgetc();
    bsl::streampos  pos = streamBuf->pubseekoff(0, bsl::ios_base::cur);
    const char     *ptr = token;

    while (*ptr) {
        if (ch == bsl::streambuf::traits_type::eof()) {
            streamBuf->pubseekpos(pos);
            return -1;                                                // RETURN
        }

        if (ch != *ptr) {
            streamBuf->pubseekpos(pos);
            return -1;                                                // RETURN
        }

        ++ptr;
        ch = streamBuf->snextc();
    }

    return 0;
}

int baejsn_ParserUtil::advancePastWhitespaceAndToken(bsl::streambuf *streamBuf,
                                                     char            token)
{
    skipSpaces(streamBuf);

    const char nextChar = static_cast<char>(streamBuf->sgetc());
    if (nextChar == token) {
        streamBuf->snextc();
        return 0;                                                     // RETURN
    }
    return -1;
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
