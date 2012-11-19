// baejsn_reader.cpp                                                  -*-C++-*-
#include <baejsn_reader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baejsn_reader_cpp,"$Id$ $CSID$")

#include <bdema_bufferedsequentialallocator.h>
#include <bsl_streambuf.h>

namespace BloombergLP {

namespace baejsn {

                            // --------------------
                            // struct baejsn_Reader
                            // --------------------

// PRIVATE MANIPULATORS
void baejsn_Reader::reloadData()
{
    const int numRead = d_streamBuf_p->sgetn(d_begin_p, BAEJSN_BUFSIZE);
    d_cursor_p = d_begin_p;
    d_end_p    = d_begin_p + numRead;
}

void baejsn_Reader::skipWhitespace()
{
    while (d_cursor_p < d_end_p && bsl::isspace(*d_cursor_p)) {
        ++d_cursor_p;
    }
    if (d_cursor_p == d_end_p) {
        reloadData();
    }
}

// MANIPULATORS
int baejsn_Reader::advanceToNextToken()
{
    skipWhitespace();

    if (d_cursor_p != d_end_p) {
        switch (*d_cursor_p) {
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
        ++d_cursor_p;
    }
}

// ACCESSORS
bslstl::StringRef baejsn_Reader::value()
{
    if (BAEJSN_NAME == d_tokenType || BAEJSN_VALUE == d_tokenType) {
        while (d_valueEnd_p < d_end_p && !bsl::isspace(*d_valueEnd_p)) {
            ++d_valueEnd_p;
        }
        if (d_valueEnd_p == d_end_p) {
            char tmpBuffer[BAEJSN_BUFSIZE];
            const int oldBytes = d_end_p - d_cursor_p;
            bsl::memcpy(tmpBuffer, d_cursor_p, oldBytes);
            bsl::memcpy(d_begin_p, tmpBuffer, oldBytes);
            d_cursor_p = d_begin_p + oldBytes;
            const int numRead = d_streamBuf_p->sgetn(
                                                    d_cursor_p,
                                                    BAEJSN_BUFSIZE - oldBytes);
            d_end_p = d_cursor_p + numRead;
            d_valueEnd_p = d_cursor_p;
            while (d_valueEnd_p < d_end_p && !bsl::isspace(*d_valueEnd_p)) {
                ++d_valueEnd_p;
            }
        }
        return bslstl::StringRef(d_cursor_p, d_valueEnd_p);           // RETURN
    }
    return bslstl::StringRef(0, 0);
}

}  // close package namespace
}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
