// baejsn_reader.cpp                                                  -*-C++-*-
#include <baejsn_reader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_reader_cpp,"$Id$ $CSID$")

#include <bsl_streambuf.h>

// TBD: Remove
#include <bsl_iostream.h>

namespace BloombergLP {

namespace {
    const char *WHITESPACE = " \n\t\v\f\r";
    const char *TOKENS     = "{}[]:,";
}

                            // --------------------
                            // struct baejsn_Reader
                            // --------------------

// PRIVATE MANIPULATORS
int baejsn_Reader::reloadStringBuffer()
{
    d_stringBuffer.resize(BAEJSN_MAX_STRING_SIZE);
    const int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[0],
                                             BAEJSN_MAX_STRING_SIZE);
    d_cursor = 0;
    d_stringBuffer.resize(numRead);
    return numRead;
}

int baejsn_Reader::skipWhitespace()
{
    while (true) {
        bsl::size_t pos = d_stringBuffer.find_first_not_of(WHITESPACE,
                                                           d_cursor);
        if (bsl::string::npos != pos) {
            d_cursor = pos;
            break;
        }

        const int numRead = reloadStringBuffer();
        if (0 == numRead) {
            return -1;                                                // RETURN
        }
    }
    return 0;
}

int baejsn_Reader::extractStringValue()
{
    d_valueBegin          = d_cursor;
    bsl::size_t iter      = d_cursor + 1;
    bool        firstTime = true;

    while (true) {
        while (iter < d_stringBuffer.length()
            && '"' != d_stringBuffer[iter]) {
            ++iter;
        }

        if (iter == d_stringBuffer.length()) {
            if (!firstTime) {
                d_stringBuffer.resize(d_stringBuffer.length()
                                                     + BAEJSN_MAX_STRING_SIZE);
                int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[iter],
                                                   BAEJSN_MAX_STRING_SIZE);
                if (0 == numRead) {
                    return -1;                                        // RETURN
                }
                continue;
            }

            d_stringBuffer.erase(d_stringBuffer.begin(),
                                 d_stringBuffer.begin() + d_cursor);
            d_stringBuffer.resize(BAEJSN_MAX_STRING_SIZE);
            iter = BAEJSN_MAX_STRING_SIZE - d_cursor;
            int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[iter],
                                               d_cursor);
            if (0 == numRead) {
                return -1;                                            // RETURN
            }

            d_stringBuffer.resize(iter + numRead);
            d_valueBegin = 0;
            firstTime = false;
        }
        else {
            d_valueEnd = iter - 1;
            return 0;                                                 // RETURN
        }
    }
    return 0;
}

int baejsn_Reader::skipNonWhitespaceOrTillToken()
{
    d_valueBegin          = d_cursor;
    bsl::size_t iter      = d_cursor + 1;
    bool        firstTime = true;

    while (true) {
        while (iter < d_stringBuffer.length()
            && !bsl::isspace(d_stringBuffer[iter])
            && !bsl::strchr(TOKENS, d_stringBuffer[iter])) {
            ++iter;
        }

        if (iter == d_stringBuffer.length()) {
            if (!firstTime) {
                d_stringBuffer.resize(d_stringBuffer.length()
                                                     + BAEJSN_MAX_STRING_SIZE);
                int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[iter],
                                                   BAEJSN_MAX_STRING_SIZE);
                if (0 == numRead) {
                    return -1;                                        // RETURN
                }
                continue;
            }

            d_stringBuffer.erase(d_stringBuffer.begin(),
                                 d_stringBuffer.begin() + d_cursor);
            d_stringBuffer.resize(BAEJSN_MAX_STRING_SIZE);
            iter = BAEJSN_MAX_STRING_SIZE - d_cursor;
            int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[iter],
                                               d_cursor);
            if (0 == numRead) {
                return -1;                                            // RETURN
            }

            d_stringBuffer.resize(iter + numRead);
            firstTime = false;
        }
        else {
            d_valueEnd = iter;
            return 0;                                                 // RETURN
        }
    }
    return 0;
}

// MANIPULATORS
int baejsn_Reader::advanceToNextToken()
{
    if (d_cursor >= d_stringBuffer.size()) {
        const int numRead = reloadStringBuffer();
        if (0 == numRead) {
            return -1;                                                // RETURN
        }
    }

    const int rc = skipWhitespace();
    if (rc) {
        return -1;                                                    // RETURN
    }

//     bsl::cout << d_stringBuffer[d_cursor] << bsl::endl;

    switch (d_stringBuffer[d_cursor]) {
      case '[': {
        if (BAEJSN_COMMA != d_tokenType
         && BAEJSN_NAME  != d_tokenType
         && BAEJSN_VALUE != d_tokenType) {
            return -1;                                                // RETURN
        }

        d_tokenType = BAEJSN_START_ARRAY;
        ++d_cursor;
      } break;

      case ']': {
        if (BAEJSN_VALUE       != d_tokenType
         && BAEJSN_START_ARRAY != d_tokenType
         && BAEJSN_END_OBJECT  != d_tokenType) {
            return -1;                                                // RETURN
        }

        d_tokenType = BAEJSN_END_ARRAY;
        d_context   = BAEJSN_START_OBJECT;
        ++d_cursor;
      } break;

      case '{': {
        // TBD: Reverse if so not possible are checked
        if (BAEJSN_VALUE       != d_tokenType
         && BAEJSN_NAME        != d_tokenType
         && BAEJSN_COMMA       != d_tokenType
         && BAEJSN_START_ARRAY != d_tokenType
         && BAEJSN_BEGIN       != d_tokenType) {
            return -1;                                                // RETURN
        }

        d_tokenType = BAEJSN_START_OBJECT;
        ++d_cursor;
      } break;

      case '}': {
        if (BAEJSN_VALUE        != d_tokenType
         && BAEJSN_START_OBJECT != d_tokenType
         && BAEJSN_END_OBJECT   != d_tokenType
         && BAEJSN_END_ARRAY    != d_tokenType) {
            return -1;                                                // RETURN
        }

        d_tokenType = BAEJSN_END_OBJECT;
        d_context   = BAEJSN_START_OBJECT;
        ++d_cursor;
      } break;

      case ',': {
        if (BAEJSN_VALUE      != d_tokenType
         && BAEJSN_END_OBJECT != d_tokenType
         && BAEJSN_END_ARRAY  != d_tokenType) {
            return -1;                                                // RETURN
        }

        d_tokenType = BAEJSN_COMMA;
        ++d_cursor;
      } break;

      case ':': {
        if (BAEJSN_NAME != d_tokenType) {
            return -1;                                                // RETURN
        }

        d_tokenType = BAEJSN_VALUE;
        ++d_cursor;

        int rc = skipWhitespace();
        if (rc) {
            return -1;                                                // RETURN
        }

        if (bsl::strchr(TOKENS, d_stringBuffer[d_cursor])) {
            return advanceToNextToken();                              // RETURN
        }

        d_valueBegin = 0;
        d_valueEnd   = 0;
        if ('"' == d_stringBuffer[d_cursor]) {
            // String value

            rc = extractStringValue();
            if (rc) {
                return -1;                                            // RETURN
            }

            // Advance past the end '"'.

            d_valueEnd += 2;
        }
        else {
            rc = skipNonWhitespaceOrTillToken();
        }

        d_cursor = d_valueEnd;
      } break;

      case '"': {
        if (BAEJSN_START_OBJECT == d_tokenType) {
            d_context   = BAEJSN_START_OBJECT;
            d_tokenType = BAEJSN_NAME;
            ++d_cursor;
        }
        else if (BAEJSN_START_ARRAY == d_tokenType) {
            d_context   = BAEJSN_START_ARRAY;
            d_tokenType = BAEJSN_VALUE;
        }
        else if (BAEJSN_COMMA == d_tokenType && BAEJSN_BEGIN != d_context) {
            if (BAEJSN_START_OBJECT == d_context) {
                d_tokenType = BAEJSN_NAME;
                ++d_cursor;
            }
            else {
                d_tokenType = BAEJSN_VALUE;
            }
        }
        else {
            return -1;                                                // RETURN
        }

        d_valueBegin = 0;
        d_valueEnd   = 0;
        int rc = extractStringValue();
        if (rc
         || (d_valueBegin == d_valueEnd && BAEJSN_NAME == d_tokenType)) {
            return -1;                                                // RETURN
        }

        if (BAEJSN_NAME == d_tokenType) {
            ++d_valueEnd;
            d_cursor = d_valueEnd + 1;
        }
        else {
            // Advance past the end '"'.

            d_valueEnd += 2;
            d_cursor = d_valueEnd;
        }
      } break;

      default: {
        if (BAEJSN_START_ARRAY != d_tokenType
         && BAEJSN_COMMA       != d_tokenType) {
            return -1;                                                // RETURN
        }

        d_tokenType = BAEJSN_VALUE;

        d_valueBegin = 0;
        d_valueEnd   = 0;
        const int rc = skipNonWhitespaceOrTillToken();
        if (rc) {
            return -1;
        }
        d_cursor = d_valueEnd;
      } break;
    }

    return 0;
}

// ACCESSORS
int baejsn_Reader::value(bslstl::StringRef *data)
{
    if ((BAEJSN_NAME == d_tokenType || BAEJSN_VALUE == d_tokenType)
     && d_valueBegin != d_valueEnd) {
        data->assign(&d_stringBuffer[d_valueBegin],
                     &d_stringBuffer[d_valueEnd]);
        bsl::cout << *data  << bsl::endl;
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
