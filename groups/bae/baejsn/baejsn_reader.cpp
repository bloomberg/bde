// baejsn_reader.cpp                                                  -*-C++-*-
#include <baejsn_reader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_reader_cpp,"$Id$ $CSID$")

#include <bsl_streambuf.h>

namespace BloombergLP {

namespace {
    const char *WHITESPACE = " \n\t\v\f\r";
    const char *TOKENS     = "{}[]:";
}

                            // --------------------
                            // struct baejsn_Reader
                            // --------------------

// PRIVATE MANIPULATORS
int baejsn_Reader::reloadStringBuffer()
{
    const int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[0],
                                             BAEJSN_MAX_STRING_SIZE);
    d_cursor = 0;
    d_stringBuffer.resize(numRead);
    return numRead;
}

int baejsn_Reader::skipWhitespace(bsl::size_t *cursor)
{
    while (true) {
        bsl::size_t pos = d_stringBuffer.find_first_not_of(WHITESPACE,
                                                           d_cursor);
        if (bsl::string::npos != pos) {
            *cursor = pos;
            break;
        }

        const int numRead = reloadStringBuffer();
        if (0 == numRead) {
            return -1;                                                // RETURN
        }
    }
    return 0;
}

int baejsn_Reader::skipNonWhitespace(bsl::size_t *begin,
                                     bsl::size_t *end,
                                     bsl::size_t  cursor)
{
    bsl::size_t pos = d_stringBuffer.find_first_of(WHITESPACE, cursor);
    if (bsl::string::npos != pos) {
        *begin = cursor;
        *end   = pos;
        return 0;                                                     // RETURN
    }

    d_stringBuffer.erase(d_stringBuffer.begin(),
                         d_stringBuffer.begin() + cursor);
    d_stringBuffer.resize(BAEJSN_MAX_STRING_SIZE);
    pos = BAEJSN_MAX_STRING_SIZE - cursor;

    int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[d_cursor], pos);
    if (0 == numRead) {
        return -1;                                                    // RETURN
    }

    d_stringBuffer.resize(d_cursor + numRead);
    pos = d_stringBuffer.find_first_of(WHITESPACE, pos);
    if (bsl::string::npos != pos) {
        *begin   = 0;
        *end     = pos;
        return 0;                                                     // RETURN
    }
    return -1;
}

// int baejsn_Reader::skipTillWhitespace(bsl::size_t *begin,
//                                       bsl::size_t *end,
//                                       bsl::size_t  cursor)
// {
//     bsl::size_t iter      = cursor;
//     bsl::size_t length    = d_stringBuffer.length();
//     bool        firstTime = true;

//     while (true) {
//         while (iter < length && !bsl::isspace(d_stringBuffer[iter])) {
//             ++iter;
//         }

//         if (iter == length) {
//             if (!firstTime) {
//                 return -1;                                            // RETURN
//             }

//             d_stringBuffer.erase(d_stringBuffer.begin(),
//                                  d_stringBuffer.begin() + cursor);
//             d_stringBuffer.resize(BAEJSN_MAX_STRING_SIZE);
//             d_cursor = BAEJSN_MAX_STRING_SIZE - cursor;
//             int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[d_cursor],
//                                                cursor);
//             if (0 == numRead) {
//                 return -1;                                            // RETURN
//             }

//             d_stringBuffer.resize(d_cursor + numRead);

//             begin  = 0;
//             cursor = d_stringBuffer.length();
//             end    = d_stringBuffer.end();
//             firstTime = false;
//         }
//         else {
//             *begin = cursor;
//             *end   = iter;
//             return 0;                                                 // RETURN
//         }
//     }
//     return 0;
// }

// MANIPULATORS
int baejsn_Reader::advanceToNextToken()
{
    if (d_cursor >= d_stringBuffer.size()) {
        const int numRead = reloadData();
        if (0 == numRead) {
            return -1;                                                // RETURN
        }
    }

    const int rc = skipWhitespace(&d_cursor);
    if (!rc) {
        return -1;                                                    // RETURN
    }

    switch (d_stringBuffer[d_cursor]) {
      case '[': {
        d_tokenType = BAEJSN_START_ARRAY;
        ++d_cursor;
      } break;

      case ']': {
        d_tokenType = BAEJSN_END_ARRAY;
        ++d_cursor;
      } break;

      case '{': {
        d_tokenType = BAEJSN_START_OBJECT;
        ++d_cursor;
      } break;

      case '}': {
        d_tokenType = BAEJSN_END_OBJECT;
        ++d_cursor;
      } break;

      case ',': {
        d_tokenType = BAEJSN_COMMA;
        ++d_cursor;
      } break;

      case ':': {
        d_tokenType = BAEJSN_VALUE;
        ++d_cursor;

        const int rc = skipWhitespace(&d_cursor);
        if (!rc) {
            return -1;                                                // RETURN
        }

        d_valueBegin = 0;
        d_valueEnd   = 0;
        const int rc = skipNonWhitespace(&d_valueBegin, &d_valueEnd, d_cursor);
        if (!rc) {
            return -1;                                                // RETURN
        }

        d_cursor = d_valueEnd + 1;
      } break;

      default: {
        d_tokenType  = BAEJSN_NAME;

        d_valueBegin = 0;
        d_valueEnd   = 0;
        const int rc = skipNonWhitespace(&d_valueBegin, &d_valueEnd, d_cursor);
        if (!rc) {
            return -1;                                                // RETURN
        }

        d_cursor = d_valueEnd + 1;
        if (d_cursor == d_stringBuffer.length()
         || ':' != d_stringBuffer[d_cursor]) {
            return -1;                                                // RETURN
        }
      } break;
    }

    return skipWhitespace(&d_cursor);
}

// ACCESSORS
int baejsn_Reader::value(bslstl::StringRef *data)
{
    if (BAEJSN_NAME == d_tokenType) {
    }
    else if (BAEJSN_VALUE == d_tokenType) {
    }
    data->assign(d_valueBegin, d_valueEnd);
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
