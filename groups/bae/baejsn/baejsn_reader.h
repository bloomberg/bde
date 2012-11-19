// baejsn_reader.h                                                    -*-C++-*-
#ifndef INCLUDED_BAEJSN_READER
#define INCLUDED_BAEJSN_READER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE:
//
//@CLASSES:
//
//@SEE_ALSO:
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

namespace BloombergLP {

namespace baejsn {
                            // ============
                            // class Reader
                            // ============

class Reader {

  public:
    enum TokenType {
        BAEJSN_BEGIN,
        BAEJSN_COMMA,
        BAEJSN_NAME,
        BAEJSN_START_OBJECT,
        BAEJSN_END_OBJECT,
        BAEJSN_START_ARRAY,
        BAEJSN_END_ARRAY,
        BAEJSN_VALUE
    };

  private:
    enum {
        BAEJSN_BUFSIZE = 1024
    };

    char            d_buffer[BAEJSN_BUFSIZE];
    const char     *d_begin_p;
    const char     *d_cursor_p;
    const char     *d_end_p;
    bsl::streambuf *d_streambuf_p;

    TokenType       d_tokenType;

    // PRIVATE MANIPULATORS
    void reloadData();
    void skipWhitespace();

  public:
    // CREATORS
    Reader();
        // Create this reader.  TBD: is basicAllocator needed ?

    ~Reader();

    // MANIPULATORS

    void reset(bsl::streambuf *streamBuf);

    int advanceToNextToken();

    // ACCESSORS
    Type tokenType();

    bslstl::StringRef value();
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
Reader::Reader(bsl::streambuf *streamBuf)
: d_begin_p(d_buffer)
, d_cursor_p(d_buffer)
, d_end_p(d_buffer)
, d_streamBuf_p(0)
, d_tokenType(BAEJSN_BEGIN)
{
    *d_begin_p = '\0';
}

inline
Reader::~Reader()
{
}

// MANIPULATORS
inline
void baejsn_Reader::reset(bsl::streambuf *streamBuf)
{
    d_streamBuf = streamBuf;
    d_begin_p   = d_buffer;
    d_cursor_p  = d_buffer;
    d_end_p     = d_buffer;
    d_tokenType = BAEJSN_BEGIN;
}

// ACCESSORS
inline
Type baejsn_Reader::tokenType()
{
    return d_tokenType;
}

}  // close package namespace
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
