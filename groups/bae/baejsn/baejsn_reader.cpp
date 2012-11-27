// baejsn_reader.cpp                                                  -*-C++-*-
#include <baejsn_reader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_reader_cpp,"$Id$ $CSID$")

#include <bsl_streambuf.h>

namespace BloombergLP {

namespace {
    const char *WHITESPACE = " \n\t\v\f\r";
}

                            // --------------------
                            // struct baejsn_Reader
                            // --------------------

// PRIVATE MANIPULATORS
int baejsn_Reader::reloadData()
{
    const int numRead = d_streamBuf_p->sgetn(&d_stringBuffer[0],
                                             BAEJSN_MAX_STRING_SIZE);
    d_cursor = 0;
    d_stringBuffer.resize(numRead);
    return numRead;
}

int baejsn_Reader::skipWhitespace()
{
    while (true) {
        d_cursor = d_stringBuffer.find_first_not_of(WHITESPACE, d_cursor);
        if (bsl::string::npos != d_cursor) {
            break;
        }
        const int numRead = reloadData();
        if (0 == numRead) {
            return -1;                                                // RETURN
        }
    }
    return 0;
}

// MANIPULATORS
int baejsn_Reader::advanceToNextToken()
{
    if (d_cursor >= d_stringBuffer.size()) {
        const int numRead = reloadData();
        if (0 == numRead) {
            return -1;                                                // RETURN
        }
    }

    const int rc = skipWhitespace();
    if (!rc) {
        return -1;                                                    // RETURN
    }

    switch (d_stringBuffer[d_cursor]) {
      case '[': {
        d_tokenType = BAEJSN_START_ARRAY;
      } break;

      case ']': {
        d_tokenType = BAEJSN_END_ARRAY;
      } break;

      case '{': {
        d_tokenType = BAEJSN_START_OBJECT;
      } break;

      case '}': {
        d_tokenType = BAEJSN_END_OBJECT;
      } break;

      case ',': {
        d_tokenType = BAEJSN_COMMA;
      } break;

      case ':': {
        d_tokenType = BAEJSN_VALUE;
      } break;

      default: {
        d_tokenType = BAEJSN_NAME;
      } break;
    }

    ++d_cursor;
    return 0;
}

// ACCESSORS
int baejsn_Reader::value(bslstl::StringRef *data)
{
    // TBD: Double check
    if (BAEJSN_NAME == d_tokenType || BAEJSN_VALUE == d_tokenType) {
        bsl::size_t pos = d_stringBuffer.find_first_of(WHITESPACE, d_cursor);
        if (bsl::string::npos == pos) {
            d_stringBuffer.erase(d_stringBuffer.begin(),
                                 d_stringBuffer.begin() + d_cursor);
            d_stringBuffer.resize(BAEJSN_MAX_STRING_SIZE);
            int numRead = d_streamBuf_p->sgetn(
                                            &d_stringBuffer[d_cursor],
                                            BAEJSN_MAX_STRING_SIZE - d_cursor);
            if (0 == numRead) {
                d_stringBuffer.resize(d_cursor);
                data->assign(d_stringBuffer.data(),
                             d_stringBuffer.data() + d_cursor);
                return 0;                                             // RETURN
            }

            do {
                d_stringBuffer.resize(d_cursor + numRead);
                pos = d_stringBuffer.find_first_of(WHITESPACE, d_cursor);
                if (bsl::string::npos == pos) {
                    numRead = d_streamBuf_p->sgetn(
                                            &d_stringBuffer[d_cursor],
                                            BAEJSN_MAX_STRING_SIZE - d_cursor);
                    if (0 == numRead) {
                        data->assign(d_stringBuffer.data(),
                                     d_stringBuffer.data() + d_cursor);
                        return 0;                                     // RETURN
                    }
                }
                else {
                    break;
                }
            } while (1);
        }

        d_cursor = pos;
        data->assign(d_stringBuffer.data(), d_stringBuffer.begin() + d_cursor);
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
