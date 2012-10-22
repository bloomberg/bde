// baejsn_parserutil.cpp                                              -*-C++-*-
#include <baejsn_parserutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_parserutil_cpp,"$Id$ $CSID$")

#include <bsl_sstream.h>
#include <bsl_cmath.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

namespace {

template <typename TYPE>
bool matchFloatDecimalPart(bsl::streambuf *streamBuf, TYPE *value)
{
    if (streamBuf->sgetc() == '.') {
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
            return false;
        }

        return true;
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
            return false;
        }

        *value *= bsl::pow(static_cast<TYPE>(10),
                           static_cast<TYPE>(exponentSign) * static_cast<TYPE>(exponentPart));

        return true;
    }
    return true;
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
    // This implementation is not very good.  It needs a lot of division and
    // loses precision.

    baejsn_ParserUtil::skipSpaces(streamBuf);

    // extract the integer part
    int ch = streamBuf->sgetc();

    if (ch == bsl::streambuf::traits_type::eof()) {
        return 1;
    }

    bool negative  = false;
    if (ch == '-')
    {
        negative = true;
        streamBuf->snextc();
    }

    bsls::Types::Uint64 magnitude = 0;
    if (0 != getInteger(streamBuf, &magnitude))
    {
        return 1;
    }

    *value = static_cast<double>(magnitude);

    // extract the decimal part
    if (!matchFloatDecimalPart(streamBuf, value)) 
    {
        return 1;
    }


    // extract the exponent part
    if (!matchFloatExponantPart(streamBuf, value)) {
        return 1;
    }

    if (negative) {
        *value = *value * -1;
    }

    return 0;
}

int baejsn_ParserUtil::getInteger(bsl::streambuf     *streamBuf,
                                  bsls::Types::Int64 *value)
{
    int ch = streamBuf->sgetc();

    if (ch == bsl::streambuf::traits_type::eof()) {
        return 1;
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

    return foundNumberFlag ? 0 : 1;
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

int baejsn_ParserUtil::putString(bsl::streambuf     *streamBuf,
                                 const bsl::string&  value)
{
    streamBuf->sputc('"');

    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
    {
        if (*it == '"' || *it == '\\' || *it == '/') {// printable (but miss-enterpreted)
            streamBuf->sputc('\\');
            streamBuf->sputc(*it);
        }
        else if (*it == '\b') {
            // non printable
            streamBuf->sputc('\\');
            streamBuf->sputc('b');
            //streamBuf->sputc("\\b");
        }
        else if (*it == '\f') {
            streamBuf->sputc('\\');
            streamBuf->sputc('f');
            //streamBuf->sputc("\\f");
        }
        else if (*it == '\n') {
            streamBuf->sputc('\\');
            streamBuf->sputc('n');
            //streamBuf->sputc("\\n");
        }
        else if (*it == '\r') {
            streamBuf->sputc('\\');
            streamBuf->sputc('r');
            //streamBuf->sputc("\\r");
        }
        else if (*it == '\t') {
            streamBuf->sputc('\\');
            streamBuf->sputc('t');
            //streamBuf->sputc("\\t");
        }
        // TBD: Disable mOptions for now.
        //else if (*it == '\0' && mOptions[Options::useModifiedUtf8])
        //{
        //    streamBuf->sputc("\\uc080";
        //}
        else if ((static_cast<unsigned int>(*it) & 0xff) < 32) {
            // Any other control characters as hex.

            // todo add back multi byte support, but unicode >= 0xD800 && unicode <= 0xDBFF checks are required to ensure that
            //      surrogate encodings are or are not needed when pushing binary as a string
            //
            //std::string::const_iterator next = it;
            //++next;

            bsl::ostringstream str;

            //if (*it == 0 || next == value.end())
            {
                str << "\\u00"
                    << std::hex
                    << std::setfill('0')
                    << std::setw(2)
                    << (static_cast<unsigned int>(*it) & 0xff);
            }
            //else
            //{
            //    str << "\\u" << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(*it) & 0xff)
            //                 << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(*next) & 0xff);
            //    ++it;
            //}

            streamBuf->sputn(str.str().c_str(), str.str().length());
        }
        else
        {
            streamBuf->sputc(*it);
        }
    }

    streamBuf->sputc('"');
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
