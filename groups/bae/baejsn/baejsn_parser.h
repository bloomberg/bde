// baejsn_parser.h                                                    -*-C++-*-
#ifndef INCLUDED_BAEJSN_PARSER
#define INCLUDED_BAEJSN_PARSER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a parser for extracting JSON data from a 'streambuf'
//
//@CLASSES:
//  baejsn_Parser: parser for parsing JSON data from a 'streambuf'
//
//@SEE_ALSO: baejsn_decoder
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a class, 'baejsn_parser', that
// traverses data stored in a 'streambuf' and.

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
                            // class baejsn_Parser
                            // ===================

class baejsn_Parser {

  public:
    enum TokenType {
        BAEJSN_BEGIN = 1,
        BAEJSN_ELEMENT_NAME,
        BAEJSN_START_OBJECT,
        BAEJSN_END_OBJECT,
        BAEJSN_START_ARRAY,
        BAEJSN_END_ARRAY,
        BAEJSN_ELEMENT_VALUE,
        BAEJSN_ERROR
    };

    enum ContextType {
        BAEJSN_OBJECT_CONTEXT = 1,
        BAEJSN_ARRAY_CONTEXT
    };

  private:
    enum {
        BAEJSN_BUFSIZE         = 1024,
        BAEJSN_MAX_STRING_SIZE = BAEJSN_BUFSIZE - 1
    };

    char                               d_buffer[BAEJSN_BUFSIZE];
    bdema_BufferedSequentialAllocator  d_allocator;
    bsl::string                        d_stringBuffer;
    bsl::streambuf                    *d_streamBuf_p;

    bsl::size_t                        d_cursor;
    bsl::size_t                        d_valueBegin;
    bsl::size_t                        d_valueEnd;

    TokenType                          d_tokenType;
    ContextType                        d_context;

    // PRIVATE MANIPULATORS
    int reloadStringBuffer();
    int skipWhitespace();
    int skipNonWhitespaceOrTillToken();
    int extractStringValue();

  public:
    // CREATORS
    baejsn_Parser(bslma::Allocator *basicAllocator = 0);
       // Create a 'baejsn_Reader' object.  Optionally specify a
       // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
       // the currently installed default allocator is used.

    ~baejsn_Parser();
        // Destroy this object.

    // MANIPULATORS
    void reset(bsl::streambuf *streamBuf);
        // Reset this parser to read data from the specified 'streamBuf'.  Note
        // that the reader will not be on a valid node until
        // 'advanceToNextNode' is called.

    int advanceToNextToken();
        // Move to the next token in the data steam.  Return 0 on success and a
        // non-zero value otherwise.  Note that each call to
        // 'advanceToNextToken' invalidates the string references returned by
        // the 'value' accessor for prior nodes.

    // ACCESSORS
    TokenType tokenType() const;
        // Return the token type of the current node.

    int value(bslstl::StringRef *data) const;
        // Load into the specified 'data' the value of the specified token.
        // Return 0 on success and a non-zero value otherwise.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
baejsn_Parser::baejsn_Parser(bslma::Allocator *basicAllocator)
: d_allocator(d_buffer, BAEJSN_BUFSIZE, basicAllocator)
, d_stringBuffer(&d_allocator)
, d_streamBuf_p(0)
, d_cursor(0)
, d_tokenType(BAEJSN_BEGIN)
, d_context(BAEJSN_OBJECT_CONTEXT)
{
    d_stringBuffer.reserve(BAEJSN_MAX_STRING_SIZE);
}

inline
baejsn_Parser::~baejsn_Parser()
{
}

// MANIPULATORS
inline
void baejsn_Parser::reset(bsl::streambuf *streamBuf)
{
    d_streamBuf_p = streamBuf;
    d_stringBuffer.clear();
    d_cursor      = 0;
    d_valueBegin  = 0;
    d_valueEnd    = 0;
    d_tokenType   = BAEJSN_BEGIN;
}

// ACCESSORS
inline
baejsn_Parser::TokenType baejsn_Parser::tokenType() const
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
