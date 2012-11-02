// baejsn_parserutil.cpp                                              -*-C++-*-
#include <baejsn_parserutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_parserutil_cpp,"$Id$ $CSID$")

#include <bdema_bufferedsequentialallocator.h>

#include <bsl_cmath.h>
#include <bsl_cctype.h>
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

inline
bool isValidNextChar(int nextChar)
{
    return bsl::streambuf::traits_type::eof() == nextChar
        || bsl::isspace(nextChar)
        || ',' == static_cast<char>(nextChar)
        || ']' == static_cast<char>(nextChar)
        || '}' == static_cast<char>(nextChar);
}

}  // close anonymous namespace

                            // ------------------------
                            // struct baejsn_ParserUtil
                            // ------------------------

// CLASS METHODS
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
        double  tmp = bsl::strtod(str.c_str(), &end);

        if (end       == str.data()
         || *end      != '\0'
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
    // TBD: Revisit

    skipSpaces(streamBuf);

    int ch = streamBuf->sgetc();
    while (bsl::isdigit(ch)) {
        if (*value * 10 > bsl::numeric_limits<bsls::Types::Uint64>::max()) {
            return -1;                                                // RETURN
        }
        *value *= 10;
        *value += ch - '0';
        ch = streamBuf->snextc();
    }

    const int                         SIZE = 64;
    char                              buffer[SIZE];
    bdema_BufferedSequentialAllocator allocator(buffer, SIZE);
    bsl::string                       str(&allocator);

    if ('.' == static_cast<char>(ch)) {

        // Store the fractional portion in 'str'

        ch = streamBuf->snextc();
        while (bsl::isdigit(ch)) {
            str += static_cast<char>(ch);
            ch   = streamBuf->snextc();
        }
    }

    if ('E' == static_cast<char>(bsl::toupper(ch))) {

        // extract the exponent part

        ch = streamBuf->snextc();

        bool isNegative;
        if ('-' == ch) {
            isNegative = true;
            streamBuf->snextc();
        }
        else {
            isNegative = false;
            if ('+' == ch) {
                streamBuf->snextc();
            }
        }

        int exponent = 0;
        if (0 != getValue(streamBuf, &exponent)) {
            return -1;                                                // RETURN
        }

        int fraction = 0;
        if (isNegative) {
            // ignore fractional portion

            *value /=
                    static_cast<bsls::Types::Uint64>(bsl::pow(10.0, exponent));
            return 0;                                                 // RETURN
        }
        else if (str.length()) {
            if (static_cast<unsigned int>(exponent) < str.length()) {
                str.erase(str.begin() + exponent, str.end());
            }
            else {
                exponent = str.length();
            }

            char *end = 0;
            fraction = static_cast<int>(bsl::strtol(str.c_str(), &end, 10));
        }

        double tmp = *value * bsl::pow(10.0, exponent) + fraction;
        if (tmp > static_cast<double>(
                            bsl::numeric_limits<bsls::Types::Uint64>::max())) {
            return -1;                                                // RETURN
        }

        *value *= static_cast<bsls::Types::Uint64>(bsl::pow(10.0, exponent));
        *value += fraction;
    }

    return isValidNextChar(streamBuf->sgetc()) ? 0 : -1;
}

int baejsn_ParserUtil::getString(bsl::streambuf *streamBuf, bsl::string *value)
{
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

                const int SIZE = 4;
                char buffer[SIZE];

                const int numRead = streamBuf->sgetn(buffer, SIZE);
                if (SIZE != numRead) {
                    return -1;                                        // RETURN
                }

                if (!('0' <= buffer[0] && buffer[0] <= '7')
                 || !bsl::isxdigit(buffer[1])
                 || !('0' <= buffer[2] && buffer[2] <= '7')
                 || !bsl::isxdigit(buffer[3])) {
                    return -1;                                        // RETURN
                }

                // hex encoded, pull the next 4 bytes

                char firstByte  = (char)(((int)hexValueTable[buffer[0]] << 4)
                                       | ((int)hexValueTable[buffer[1]]));
                char secondByte = (char)(((int)hexValueTable[buffer[2]] << 4)
                                       | ((int)hexValueTable[buffer[3]]));

                if (firstByte) {
                    *value += firstByte;
                }
                *value += secondByte;
                streamBuf->sungetc();

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
