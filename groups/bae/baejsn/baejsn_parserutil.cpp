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

int getUnicodeChar(bsl::string *value, const char *iter)
{
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

    *value += charValue;

    return 0;
}

int getChar(char *value, const char *iter)
{
    if ('\\' == *iter) {
        ++iter;
        switch (*iter) {
          case 'b': {
            *value = '\b';
          } break;

          case 'f': {
            *value = '\f';
          } break;

          case 'n': {
            *value = '\n';
          } break;

          case 'r': {
            *value = '\r';
          } break;

          case 't': {
            *value = '\t';
          } break;

          case '"'  :                                           // FALL THROUGH
          case '\\' :                                           // FALL THROUGH
          case '/'  : {

            // printable characters

            *value = *iter;
          } break;

          case 'u':
          case 'U': {

            ++iter;

            if ((data.end() - iter) < 4) {
                return -1;                                            // RETURN
            }

            if (0 != getUnicodeChar(value, iter)) {
                return -1;                                            // RETURN
            }

            iter += 4;
          } break;

          default: {
            return -1;                                                // RETURN
          } break;
        }
    }
    else if ('"' == *iter) {
        ++iter;
        return 0;                                                     // RETURN
    }
    else {
        *value = *iter;
    }
    return 0;
}

}  // close unnamed namespace

                            // ------------------------
                            // struct baejsn_ParserUtil
                            // ------------------------

// CLASS METHODS
int baejsn_ParserUtil::getString(bsl::string *value, bslstl::StringRef data)
{
    value->clear();

    const char *iter = data.data();

    if ('"' != *iter) {
        return -1;                                                    // RETURN
    }

    ++iter;
    return -1;
}

int baejsn_ParserUtil::getUint64(bsls::Types::Uint64 *value,
                                 bslstl::StringRef    data)
{
    const char *iter = data.begin();
    const char *end  = data.end();

    while (iter < end && bsl::isdigit(*iter)) {
        ++iter;
    }

    const char *valueEnd        = iter;
    const char *fractionalBegin = 0;
    const char *fractionalEnd   = 0;
    const char *expBegin        = 0;

    if (iter < end && '.' == *iter) {

        // Store the fractional portion in 'fractionalStr'

        fractionalBegin = ++iter;
        while (iter < end && bsl::isdigit(*iter)) {
            ++iter;
        }
        fractionalEnd = iter;
    }

    const int fractionalLen = fractionalEnd - fractionalBegin;

    if ('E' == static_cast<char>(bsl::toupper(*iter))) {

        // extract the exponent part

        ++iter;
        char *endPtr = 0;
        errno = 0;
        int exponent = static_cast<int>(bsl::strtol(iter, &endPtr, 10));

        if (0 != errno) {
            return -1;                                                // RETURN
        }

        if (exponent < 0) {
            exponent = -exponent;
            valueEnd = data.begin() + bsl::min(valueEnd - data.begin(),
                                               exponent);
        }
        else {
            if (exponent < fractionalLen) {
                return -1;                                            // RETURN
            }

            exponent -= fractionalLen;
            bsl::memcpy(const_cast<char *>(valueEnd) + 1,
                        fractionalBegin,
                        fractionalLen);
            valueEnd = fractionalBegin + fractionalLen;
        }
    }

    *value = 0;

    while (iter != valueEnd) {
        if (static_cast<bsls::Types::Uint64>(*value * 10 + *iter - '0') >
                            bsl::numeric_limits<bsls::Types::Uint64>::max()) {
            return -1;                                                // RETURN
        }
        *value = *value * 10 + *iter - '0';
        ++iter;
    }

    return 0;
}

int baejsn_ParserUtil::getValue(char *value, bslstl::StringRef data)
{
    enum { BAEJSN_MIN_CHAR_LENGTH = 3 };

    if (data.length() < BAEJSN_EXPECTED_CHAR_LENGTH
     || '"' != data[0]) {
        return -1;                                                    // RETURN
    }

    return -1;
}

int baejsn_ParserUtil::getDouble(double *value, bslstl::StringRef data)
{
    enum {
        BAEJSN_SUCCESS                  = 0,
        BAEJSN_NAN_LENGTH               = 3,
        BAEJSN_INF_LENGTH               = 3,
        BAEJSN_POSITIVE_INFINITY_LENGTH = 4,
        BAEJSN_NEGATIVE_INFINITY_LENGTH = 4,
    };

    if (0 == data.length()) {
        return -1;                                                    // RETURN
    }

    switch (data[0]) {
      case 'N': {
        if (data.length() >= BAEJSN_NAN_LENGTH
         && 0 == bsl::strcmp(data.data() + 1, "aN")) {
            *value = bsl::numeric_limits<double>::quiet_NaN();
            return BAEJSN_SUCCESS;                                    // RETURN
        }
      } break;
      case 'I': {
        if (data.length() >= BAEJSN_INF_LENGTH
         && 0 == bsl::strcmp(data.data() + 1, "NF")) {
            *value = bsl::numeric_limits<double>::infinity();
            return BAEJSN_SUCCESS;                                    // RETURN
        }
      } break;
      case '+': {
        if (data.length() >= BAEJSN_POSITIVE_INFINITY_LENGTH
         && 0 == bsl::strcmp(data.data() + 1, "INF")) {
            *value = bsl::numeric_limits<double>::infinity();
            return BAEJSN_SUCCESS;                                    // RETURN
        }
      } break;
      case '-': {
        if (data.length() >= BAEJSN_NEGATIVE_INFINITY_LENGTH
         && 0 == bsl::strcmp(data.data() + 1, "INF")) {
            *value = -bsl::numeric_limits<double>::infinity();
            return BAEJSN_SUCCESS;                                    // RETURN
        }
      } break;

      default: {
      } break;
    }

    char   *end = 0;
    errno       = 0;
    double  tmp = bsl::strtod(data.data(), &end);

    if ((0        == tmp && 0 != errno)
     || HUGE_VAL  == tmp
     || -HUGE_VAL == tmp) {
        return -1;                                                    // RETURN
    }

    *value = tmp;
    return 0;                                                     // RETURN
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
