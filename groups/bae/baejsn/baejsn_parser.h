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
//@SEE_ALSO: baejsn_decoder, baejsn_parserutil
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a class, 'baejsn_parser', that
// traverses data stored in a 'bsl::streambuf' one node at a time and provides
// clients access to the data associated with that node, including its type
// and data value.  Client code can use the 'reset' function to associate a
// 'bsl::streambuf' containing JSON data with a parser object and then call
// the 'advanceToNextToken' function to extract individual data values.
//
// This 'class' was created to be used by other components in the 'baejsn'
// package and in most cases clients should use the 'baejsn_decoder' component
// instead of using this 'class'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Extracting JSON data into an object
///----------------------------------------------
// For this example, we will use 'baejsn_Parser' to read each node in a
// JSON document and populate a simple 'Employee' object.
//
// First, we will define the JSON data that the parser will traverse over:
//..
//  const char *INPUT = "    {\n"
//                      "        \"street\" : \"Lexington Ave\",\n"
//                      "        \"state\" : \"New York\",\n"
//                      "        \"zipcode\" : 10022\n"
//                      "    }";
//..
// Next, we will construct populate a 'streambuf' with this data:
//..
//  bdesb_FixedMemInStreamBuf isb(INPUT, bsl::strlen(INPUT));
//..
// Then, we will create a 'baejsn_Parser' object and associate the above
// streambuf with it:
//..
//  baejsn_Parser parser;
//  parser.reset(&isb);
//..
// Next, we will create a 'bcem_Aggregate' representing an employee:
//..
//  bcema_SharedPtr<bdem_Schema> schema(new bdem_Schema);
//
//  bdem_RecordDef *addressRecord = schema->createRecord("Address");
//  addressRecord->appendField(bdem_ElemType::BDEM_STRING, "street");
//  addressRecord->appendField(bdem_ElemType::BDEM_STRING, "state");
//  addressRecord->appendField(bdem_ElemType::BDEM_INT, "zipcode");
//
//  bcem_Aggregate address(schema, "Address");
//..
// Then, we will traverse the JSON data one node at a time:
//..
//  // Read '{'
//
//  int rc = parser.advanceToNextToken();
//  assert(!rc);
//
//  baejsn_Parser::TokenType token = parser.tokenType();
//  assert(baejsn_Parser::BAEJSN_START_OBJECT == token);
//
//  rc = parser.advanceToNextToken();
//  assert(!rc);
//  token = parser.tokenType();
//
//  // Continue reading elements till '}' is encountered
//
//  while (baejsn_Parser::BAEJSN_END_OBJECT != token) {
//      assert(baejsn_Parser::BAEJSN_ELEMENT_NAME == token);
//
//      // Read element name
//
//      bslstl::StringRef nodeValue;
//      rc = parser.value(&nodeValue);
//      assert(!rc);
//
//      bsl::string elementName = nodeValue;
//
//      // Read element value
//
//      int rc = parser.advanceToNextToken();
//      assert(!rc);
//
//      token = parser.tokenType();
//      assert(baejsn_Parser::BAEJSN_ELEMENT_VALUE == token);
//
//      rc = parser.value(&nodeValue);
//      assert(!rc);
//
//      // Extract the simple type with the data
//
//      if (bdem_ElemType::BDEM_STRING == address.fieldType(elementName)) {
//
//          bsl::string data;
//          rc = baejsn_ParserUtil::getValue(&data, nodeValue);
//          ASSERT(!rc);
//
//          // Populate the element with the read value
//
//          address.setField(elementName, data);
//      }
//      else if (bdem_ElemType::BDEM_INT == address.fieldType(elementName)) {
//          int data;
//          rc = baejsn_ParserUtil::getValue(&data, nodeValue);
//          ASSERT(!rc);
//
//          // Populate the element with the read value
//
//          address.setField(elementName, data);
//      }
//
//      rc = parser.advanceToNextToken();
//      assert(!rc);
//      token = parser.tokenType();
//  }
//..
// Finally, we will verify that the 'address' aggregate has the correct values:
//..
//  assert("Lexington Ave" == address["street"].asString());
//  assert("New York"      == address["state"].asString());
//  assert(10022           == address["zipcode"].asInt());
//..

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
    // This 'class' provides a mechanism for traversing JSON data stored in a
    // 'bsl::streambuf' one node at a time and allows clients to access the
    // data associated with that node, including its type and data value.

  public:
    // TYPES
    enum TokenType {
        // This 'enum' lists all the possible token types.

        BAEJSN_BEGIN = 1,                  // starting token
        BAEJSN_ELEMENT_NAME,               // element name
        BAEJSN_START_OBJECT,               // start of an object ('{')
        BAEJSN_END_OBJECT,                 // end of an object   ('}')
        BAEJSN_START_ARRAY,                // start of an array  ('[')
        BAEJSN_END_ARRAY,                  // end of an array    (']')
        BAEJSN_ELEMENT_VALUE,              // element value of a simple type
        BAEJSN_ERROR                       // error token
    };
  private:
    // TYPES
    enum ContextType {
        // This 'enum' lists the possible contexts that the parser can be in.

        BAEJSN_OBJECT_CONTEXT = 1,
        BAEJSN_ARRAY_CONTEXT
    };

    // CONSTANTS
    enum {
        BAEJSN_BUFSIZE         = 1024,
        BAEJSN_MAX_STRING_SIZE = BAEJSN_BUFSIZE
                                  - 1 - bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
    };

    char                               d_buffer[BAEJSN_BUFSIZE];  // data
                                                                  // buffer
                                                                  // (owned)

    bdema_BufferedSequentialAllocator  d_allocator;               // allocater
                                                                  // (owned)

    bsl::string                        d_stringBuffer;            // string 
                                                                  // buffer

    bsl::streambuf                    *d_streamBuf_p;             // streambuf
                                                                  // (held,
                                                                  // not owned)

    bsl::size_t                        d_cursor;                  // current
                                                                  // cursor

    bsl::size_t                        d_valueBegin;              // cursor for
                                                                  // beginning
                                                                  // of value

    bsl::size_t                        d_valueEnd;                // cursor for
                                                                  // end of
                                                                  // value

    TokenType                          d_tokenType;               // token type

    ContextType                        d_context;                 // context
                                                                  // type

    // PRIVATE MANIPULATORS
    int extractStringValue();
        // Extract the string value starting at the current data cursor and
        // update the value begin and end pointers to refer to the begin and
        // end of the extracted string.  Return 0 on success and a non-zero
        // value otherwise.

    int reloadStringBuffer();
        // Reload the string buffer with new data read from the underlying
        // 'streambuf' and overwriting the current buffer.  After reading
        // update the cursor to the new read location.  Return the number of
        // bytes read from the 'streamBuf'.

    int skipWhitespace();
        // Skip all whitespace characters and position the cursor onto the
        // first non-whitespace character.  Return 0 on success and a non-zero
        // value otherwise.

    int skipNonWhitespaceOrTillToken();
        // Skip all characters until a whitespace or a token character is
        // encountered and position the cursor onto the first such character.
        // Return 0 on success and a non-zero value otherwise.

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
        // 'advanceToNextToken' is called.

    int advanceToNextToken();
        // Move to the next token in the data steam.  Return 0 on success and a
        // non-zero value otherwise.  Note that each call to
        // 'advanceToNextToken' invalidates the string references returned by
        // the 'value' accessor for prior nodes.

    // ACCESSORS
    TokenType tokenType() const;
        // Return the token type of the current token.

    int value(bslstl::StringRef *data) const;
        // Load into the specified 'data' the value of the specified token if
        // the current token's type is 'BAEJSN_ELEMENT_NAME' or
        // 'BAEJSN_ELEMENT_VALUE' or leave 'data' unmodified otherwise.  Return
        // 0 on success and a non-zero value otherwise.
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
