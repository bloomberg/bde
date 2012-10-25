// baejsn_parserutil.cpp                                              -*-C++-*-
#include <baejsn_parserutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_parserutil_cpp,"$Id$ $CSID$")

#include <baexml_hexparser.h>

#include <bsl_sstream.h>

#include <bsl_sstream.h>
#include <bsl_cmath.h>
#include <bsl_cctype.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

namespace {

template <typename TYPE>
bool matchFloatDecimalPart(bsl::streambuf *streamBuf, TYPE *value)
{
    if ('.' == streamBuf->sgetc()) {
        double multiplier = 1.0;

        int ch = streamBuf->snextc();
        while (ch != bsl::streambuf::traits_type::eof()
            && ch >= '0'
            && ch <= '9') {
            multiplier /= 10.0;

            if ('0' != ch) {
                *value += multiplier * static_cast<TYPE>(ch - '0');
            }

            ch = streamBuf->snextc();
        }

        if (multiplier == 1.0) {
            return false;                                             // RETURN
        }
    }
    return true;
}

template <typename TYPE>
bool matchFloatExponantPart(bsl::streambuf *streamBuf, TYPE *value)
{
    if (streamBuf->sgetc() == 'e' || streamBuf->sgetc() == 'E') {
        int                 exponentSign = 1;
        bsls::Types::Uint64 exponentPart = 0;

        int ch = streamBuf->snextc();
        if (ch == '-') {
            exponentSign = -1;
            streamBuf->snextc();
        }
        else if (ch == '+') {
            exponentSign = 1;
            streamBuf->snextc();
        }

        if (0 != baejsn_ParserUtil::getInteger(streamBuf, &exponentPart)) {
            return false;                                             // RETURN
        }

        *value *= bsl::pow(static_cast<TYPE>(10),
                           static_cast<TYPE>(exponentSign)
                                            * static_cast<TYPE>(exponentPart));
    }
    return true;
}

}  // close anonymous namespace

                            // ------------------------
                            // struct baejsn_ParserUtil
                            // ------------------------

int baejsn_ParserUtil::skipSpaces(bsl::streambuf *streamBuf)
{
    const char *whitespace = " \t\n\r\f";

    int ch = streamBuf->sgetc();
    while (ch != bsl::streambuf::traits_type::eof()
        && bsl::strchr(whitespace, ch)) {

        ch = streamBuf->snextc();
    }

    return ch;
}

int baejsn_ParserUtil::getDouble(bsl::streambuf *streamBuf, double *value)
{
    // This implementation is not very good.  It needs a lot of division and
    // loses precision.

    skipSpaces(streamBuf);

    // extract the integer part

    int ch = streamBuf->sgetc();

    if (ch == bsl::streambuf::traits_type::eof()) {
        return -1;                                                    // RETURN
    }

    bool negative  = false;
    if (ch == '-') {
        negative = true;
        streamBuf->snextc();
    }

    bsls::Types::Uint64 magnitude = 0;
    if (0 != getInteger(streamBuf, &magnitude)) {
        return -1;                                                    // RETURN
    }

    *value = static_cast<double>(magnitude);

    // extract the decimal part

    if (!matchFloatDecimalPart(streamBuf, value)) {
        return -1;                                                    // RETURN
    }

    // extract the exponent part

    if (!matchFloatExponantPart(streamBuf, value)) {
        return -1;                                                    // RETURN
    }

    if (negative) {
        *value = *value * -1;
    }

    return 0;
}

int baejsn_ParserUtil::getUint64(bsl::streambuf      *streamBuf,
                                 bsls::Types::Uint64 *value)
{
    // This implementation is not very good.  It needs a lot of division and
    // loses precision.

    skipSpaces(streamBuf);

    int ch = streamBuf->sgetc();
    while (ch != bsl::streambuf::traits_type::eof()
        && ch >= '0'
        && ch <= '9') {
        *value *= 10;
        *value += ch - '0';

        ch = streamBuf->snextc();
    }

    const int SIZE = 64;
    char  fractionalBuffer[SIZE];
    char *iter = fractionalBuffer;
    char *end  = iter + SIZE;
    int  numDigits = 0;
    if ('.' == static_cast<char>(ch)) {

        // Adjust or drop fractional portion

        ch = streamBuf->snextc();
        while (ch != bsl::streambuf::traits_type::eof()
            && ch >= '0'
            && ch <= '9'
            && iter < end) {
            *iter = static_cast<char>(ch);
            ++iter;
            ch = streamBuf->snextc();
        }

        if (iter == fractionalBuffer || iter == end) {
            return -1;                                                // RETURN
        }

        numDigits = iter - fractionalBuffer;
        iter = fractionalBuffer;
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

        bsls::Types::Uint64 exponent = 0;

        if (0 != baejsn_ParserUtil::getInteger(streamBuf, &exponent)) {
            return -1;                                                // RETURN
        }

        if (isNegative) {
            // ignore fractional portion

            while (exponent && *value) {
                *value /= 10;
                --exponent;
            }
        }
        else {
            while (exponent && numDigits) {
                int digitValue = *iter - '0';
                if (*value * 10 + digitValue
                   < bsl::numeric_limits<bsls::Types::Uint64>::max()) {
                    *value = *value * 10 + digitValue;
                    --exponent;
                    --numDigits;
                    ++iter;
                }
                else {
                    return -1;                                        // RETURN
                }
            }

            while (exponent) {
                if (*value * 10
                   < bsl::numeric_limits<bsls::Types::Uint64>::max()) {
                    *value *= 10;
                    --exponent;
                }
                else {
                    return -1;                                        // RETURN
                }
            }
        }
    }

    return 0;
}

int baejsn_ParserUtil::getInteger(bsl::streambuf      *streamBuf,
                                  bsls::Types::Uint64 *value)
{
    *value = 0;

    int ch = streamBuf->sgetc();
    bool foundNumberFlag = false;
    while (ch != bsl::streambuf::traits_type::eof()
        && ch >= '0'
        && ch <= '9') {
        foundNumberFlag = true;
        *value *= 10;
        *value += ch - '0';

        ch = streamBuf->snextc();
    }

    return foundNumberFlag ? 0 : 1;
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

              case 'u': {
                streamBuf->snextc();

                // TBD: This doesnt work
                BSLS_ASSERT_OPT(0);

                const int SIZE = 4;
                char buffer[SIZE + 1];

                const int numRead = streamBuf->sgetn(buffer, SIZE);
                if (SIZE != numRead) {
                    return -1;                                        // RETURN
                }
                buffer[SIZE] = '\0';

                // hex encoded, pull the next 4 bytes

                baexml_HexParser<bsl::string> parser;

                if (0 != parser.beginParse(value)) {
                    return -1;                                        // RETURN
                }

                if (0 != parser.pushCharacters(buffer, buffer + SIZE)) {
                    return -1;                                        // RETURN
                }

                parser.endParse();
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

    while ('\0' != *ptr) {
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

    const int nextChar = streamBuf->sgetc();
    if (nextChar == static_cast<int>(token)) {
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
