// baejsn_parserutil.cpp                                              -*-C++-*-
#include <baejsn_parserutil.h>
#include <bsl_cmath.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_parserutil_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace {

template <typename TYPE>
bool matchFloatDecimalPart(bsl::streambuf *streamBuf, TYPE *value)
{
    if (streamBuf->sgetc() == '.')
    {
        double multiplier = 1.0;

        int ch = streamBuf->snextc();
        while (ch != bsl::streambuf::traits_type::eof()
               && ch >= '0'
               && ch <= '9') {
            multiplier /= 10.0;

            if (ch != '0')
                *value += multiplier * static_cast<TYPE>(ch - '0');

            ch = streamBuf->snextc();
        }

        if (multiplier == 1.0) {
            //BAEJSN_THROW(mobcmn::ExBadArg,
            //             "Could not decode float decimal @ "
            //             << (ch == bsl::streambuf::traits_type::eof()
            //                ? ' '
            //                : ch));
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

template <typename TYPE>
bool matchFloatExponantPart(bsl::streambuf *streamBuf, TYPE *value)
{
    if (streamBuf->sgetc() == 'e' || streamBuf->sgetc() == 'E')
    {
        int  exponentSign = 1;
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
            //BAEJSN_THROW(mobcmn::ExBadArg,
            //             "Could not decode float exponent @ "
            //             << (streamBuf->sgetc() == bsl::streambuf::traits_type::eof() ? ' ' : streamBuf->sgetc()));
            return false;
        }

        *value *= bsl::pow(static_cast<TYPE>(10), static_cast<TYPE>(exponentSign) * static_cast<TYPE>(exponentPart));

        return true;
    }
    else
    {
        return false;
    }
}

}  // close anonymous namespace

                            // ------------------------
                            // struct baejsn_ParserUtil
                            // ------------------------

int baejsn_ParserUtil::skipSpaces(bsl::streambuf *streamBuf)
{
    const char white[] = { ' ', '\t', '\n', '\r', '\f' };

    int ch = streamBuf->sgetc();
    while (ch != bsl::streambuf::traits_type::eof()) {
        bool isWhite = false;

        for (int i = 0; !isWhite && i < static_cast<int>(sizeof(white)); ++i) {
            isWhite = (white[i] == ch);
        }

        if (!isWhite) {
            return ch;
        }

        ch = streamBuf->snextc();
    }

    return ch;
}


int baejsn_ParserUtil::getDouble(bsl::streambuf *streamBuf, double *value)
{
    baejsn_ParserUtil::skipSpaces(streamBuf);

    // extract the integer part
    {
        bsls::Types::Int64 integerPart = 0;
        if (0 != getInteger(streamBuf, &integerPart)) {
            return 1;
        }

        *value = integerPart;
    }

    // extract the decimal part
    matchFloatDecimalPart(streamBuf, value);

    // extract the exponent part
    matchFloatExponantPart(streamBuf, value);
    return 0;
}

int baejsn_ParserUtil::getInteger(bsl::streambuf     *streamBuf,
                                  bsls::Types::Int64 *value)
{
    int ch = streamBuf->sgetc();

    if (ch == bsl::streambuf::traits_type::eof()) {
        return false;
    }

    bool                negative  = false;
    bsls::Types::Uint64 magnitude = 0;

    if (ch == '-')
    {
        negative = true;
        streamBuf->snextc();
    }

    if (0 == getInteger(streamBuf, &magnitude))
    {
        // match int will have moved mPosition
        *value = static_cast<int64_t>(magnitude) * (negative ? -1 : 1);
        return 0;
    }

    return 1;
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

    return !foundNumberFlag;
}

int baejsn_ParserUtil::getString(bsl::streambuf *streamBuf,
                                 bsl::string    *value)
{
    value->clear();

    int ch = streamBuf->sgetc();
    if (ch != bsl::streambuf::traits_type::eof()) {
        if (ch != '"')
            return 1;

        ch = streamBuf->snextc();
    }

    bool escaped = false;

    while (ch != bsl::streambuf::traits_type::eof()) {
        if (escaped)
        {
            if (ch == '"' || ch =='\\' || ch == '/') // printable
                *value += ch;
            else if (ch == 'b') // non printable
                *value += '\b';
            else if (ch == 'f')
                *value += '\f';
            else if (ch == 'n')
                *value += '\n';
            else if (ch == 'r')
                *value += '\r';
            else if (ch == 't')
                *value += '\t';
            else if (ch == 'u')
            {
                // hex encoded, pull the next 4 bytes
                std::string hex;
                for (int i = 0; i < 4; ++i)
                {
                    ch = streamBuf->snextc();

                    if (ch == bsl::streambuf::traits_type::eof()) {
                        return 1; // badly encoded
                    }

                    if (!(ch >= '0' && ch <= '9') && !(ch >= 'a' && ch <= 'f')
                     && !(ch >= 'A' && ch <= 'F')) {
                        return 1; // badly encoded
                    }

                    hex += static_cast<char>(ch);
                }

                //std::string raw = mobcmn::HexConvert::fromHex(hex);
                std::string raw = "h";

                //if (mOptions[Options::useModifiedUtf8] && raw == "\xc0\x80")
                //    *value += '\0';
                //else
                if (raw.length() == 2 && raw[0] == '\0')
                    *value += raw[1];
                else
                    *value += raw;
            }

            escaped = false;
        }
        else
        {
            if (ch == '\\')
                escaped = true;
            else if (ch == '"')
            {
                streamBuf->snextc();
                return 0;
            }
            else
                *value += ch;
        }

        ch = streamBuf->snextc();
    }

    return 1;
}

int baejsn_ParserUtil::eatToken(bsl::streambuf *streamBuf,
                                 const char     *token)
{
    int ch = streamBuf->sgetc();
    bsl::streampos pos = streamBuf->pubseekoff(0, bsl::ios_base::cur);
    const char *ptr = token;

    while ('\0' != *ptr)
    {
        if (ch == bsl::streambuf::traits_type::eof()) {
            streamBuf->pubseekpos(pos);
            return 1;
        }

        if (ch != *ptr) {
            streamBuf->pubseekpos(pos);
            return 1;
        }

        ++ptr;
        ch = streamBuf->snextc();
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
