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

#ifndef INCLUDED_BDEMA_BUFFEREDSEQUENTIALALLOCATOR
#include <bdema_bufferedsequentialallocator.h>
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

                            // ===================
                            // class baejsn_Reader
                            // ===================

class baejsn_Reader {

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
        BAEJSN_BUFSIZE         = 1024,
        BAEJSN_MAX_STRING_SIZE = BAEJSN_BUFSIZE - 1
    };

    char                               d_buffer[BAEJSN_BUFSIZE];
    bdema_BufferedSequentialAllocator  d_allocator;
    bsl::string                        d_stringBuffer;
    bsl::size_t                        d_cursor;
    bsl::streambuf                    *d_streamBuf_p;

    char                               d_lastChar;
    TokenType                          d_tokenType;

    // PRIVATE MANIPULATORS
    int reloadData();
    int skipWhitespace();

  public:
    // CREATORS
    baejsn_Reader(bslma::Allocator *basicAllocator = 0);
        // Create this reader.

    ~baejsn_Reader();

    // MANIPULATORS

    void reset(bsl::streambuf *streamBuf);

    int advanceToNextToken();

    // ACCESSORS
    TokenType tokenType();

    int value(bslstl::StringRef *data);
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
baejsn_Reader::baejsn_Reader(bslma::Allocator *basicAllocator)
: d_allocator(d_buffer, BAEJSN_BUFSIZE, basicAllocator)
, d_stringBuffer(&d_allocator)
, d_cursor(0)
, d_streamBuf_p(0)
, d_tokenType(BAEJSN_BEGIN)
{
    d_stringBuffer.reserve(BAEJSN_MAX_STRING_SIZE);
}

inline
baejsn_Reader::~baejsn_Reader()
{
}

// MANIPULATORS
inline
void baejsn_Reader::reset(bsl::streambuf *streamBuf)
{
    d_streamBuf_p = streamBuf;
    d_cursor      = 0;
    d_tokenType   = BAEJSN_BEGIN;
}

// ACCESSORS
inline
baejsn_Reader::TokenType baejsn_Reader::tokenType()
{
    return d_tokenType;
}

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
