// baejsn_parserutil.cpp                                              -*-C++-*-
#include <baejsn_parserutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_parserutil_cpp,"$Id$ $CSID$")

#include <baexml_hexparser.h>

#include <bdema_bufferedsequentialallocator.h>

#include <bsl_sstream.h>

#include <bsl_sstream.h>
#include <bsl_cmath.h>
#include <bsl_cctype.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                            // ------------------------
                            // struct baejsn_ParserUtil
                            // ------------------------

int baejsn_ParserUtil::skipSpaces(bsl::streambuf *streamBuf)
{
    int ch = streamBuf->sgetc();
    while (ch != bsl::streambuf::traits_type::eof()
        && bsl::isspace(ch)) {

        ch = streamBuf->snextc();
    }

    return ch;
}

int baejsn_ParserUtil::getDouble(bsl::streambuf *streamBuf, double *value)
{
    skipSpaces(streamBuf);

    const int SIZE = 64;
    char buffer[SIZE];

    bdema_BufferedSequentialAllocator allocator(buffer, SIZE);
    bsl::string str(&allocator);

    int ch = streamBuf->sgetc();
    while (ch != bsl::streambuf::traits_type::eof()
        && (bsl::isdigit(ch)
         || '.' == static_cast<char>(ch)
         || 'E' == static_cast<char>(bsl::toupper(ch))
         || '+' == static_cast<char>(ch)
         || '-' == static_cast<char>(ch))) {
        str += static_cast<char>(ch);
        ch = streamBuf->snextc();
    }
    char *end = 0;
    double tmp = bsl::strtod(str.c_str(), &end);
    if (end == str.data() || *end != '\0') {
        return -1;                                                    // RETURN
    }
    *value = tmp;
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

        int exponent = 0;
        if (0 != getValue(streamBuf, &exponent)) {
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
                if (*value * 10 + digitValue <
                             bsl::numeric_limits<bsls::Types::Uint64>::max()) {
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
                if (*value * 10 <
                             bsl::numeric_limits<bsls::Types::Uint64>::max()) {
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
