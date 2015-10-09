// baljsn_tokenizer.h                                                 -*-C++-*-
#ifndef INCLUDED_BALJSN_TOKENIZER
#define INCLUDED_BALJSN_TOKENIZER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a tokenizer for extracting JSON data from a 'streambuf'.
//
//@CLASSES:
//  baljsn::Tokenizer: tokenizer for parsing JSON data from a 'streambuf'
//
//@SEE_ALSO: baljsn_decoder, baljsn_parserutil
//
//@DESCRIPTION: This component provides a class, 'baljsn::Tokenizer', that
// traverses data stored in a 'bsl::streambuf' one node at a time and provides
// clients access to the data associated with that node, including its type and
// data value.  Client code can use the 'reset' function to associate a
// 'bsl::streambuf' containing JSON data with a tokenizer object and then call
// the 'advanceToNextToken' function to extract individual data values.
//
// This 'class' was created to be used by other components in the 'baljsn'
// package and in most cases clients should use the 'baljsn_decoder' component
// instead of using this 'class'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Extracting JSON Data into an Object
///----------------------------------------------
// For this example, we will use 'baljsn::Tokenizer' to read each node in a
// JSON document and populate a simple 'Employee' object.
//
// First, we will define the JSON data that the tokenizer will traverse over:
//..
//  const char *INPUT = "    {\n"
//                      "        \"street\" : \"Lexington Ave\",\n"
//                      "        \"state\" : \"New York\",\n"
//                      "        \"zipcode\" : 10022\n"
//                      "    }";
//..
// Next, we will construct populate a 'streambuf' with this data:
//..
//  bdlsb::FixedMemInStreamBuf isb(INPUT, bsl::strlen(INPUT));
//..
// Then, we will create a 'baljsn::Tokenizer' object and associate the above
// streambuf with it:
//..
//  baljsn::Tokenizer tokenizer;
//  tokenizer.reset(&isb);
//..
// Next, we will create an address record type and object.
//..
//  struct Address {
//      bsl::string d_street;
//      bsl::string d_state;
//      int         d_zipcode;
//  } address = { "", "", 0 };
//..
// Then, we will traverse the JSON data one node at a time:
//..
//  // Read '{'
//
//  int rc = tokenizer.advanceToNextToken();
//  assert(!rc);
//
//  baljsn::Tokenizer::TokenType token = tokenizer.tokenType();
//  assert(baljsn::Tokenizer::e_START_OBJECT == token);
//
//  rc = tokenizer.advanceToNextToken();
//  assert(!rc);
//  token = tokenizer.tokenType();
//
//  // Continue reading elements till '}' is encountered
//
//  while (baljsn::Tokenizer::e_END_OBJECT != token) {
//      assert(baljsn::Tokenizer::e_ELEMENT_NAME == token);
//
//      // Read element name
//
//      bslstl::StringRef nodeValue;
//      rc = tokenizer.value(&nodeValue);
//      assert(!rc);
//
//      bsl::string elementName = nodeValue;
//
//      // Read element value
//
//      int rc = tokenizer.advanceToNextToken();
//      assert(!rc);
//
//      token = tokenizer.tokenType();
//      assert(baljsn::Tokenizer::e_ELEMENT_VALUE == token);
//
//      rc = tokenizer.value(&nodeValue);
//      assert(!rc);
//
//      // Extract the simple type with the data
//
//      if (elementName == "street") {
//          rc = baljsn::ParserUtil::getValue(&address.d_street, nodeValue);
//          assert(!rc);
//      }
//      else if (elementName == "state") {
//          rc = baljsn::ParserUtil::getValue(&address.d_state, nodeValue);
//          assert(!rc);
//      }
//      else if (elementName == "zipcode") {
//          rc = baljsn::ParserUtil::getValue(&address.d_zipcode, nodeValue);
//          assert(!rc);
//      }
//
//      rc = tokenizer.advanceToNextToken();
//      assert(!rc);
//      token = tokenizer.tokenType();
//  }
//..
// Finally, we will verify that the 'address' aggregate has the correct values:
//..
//  assert("Lexington Ave" == address.d_street);
//  assert("New York"      == address.d_state);
//  assert(10022           == address.d_zipcode);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_BUFFEREDSEQUENTIALALLOCATOR
#include <bdlma_bufferedsequentialallocator.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNEDBUFFER
#include <bsls_alignedbuffer.h>
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
namespace baljsn {

                              // ===============
                              // class Tokenizer
                              // ===============

class Tokenizer {
    // This 'class' provides a mechanism for traversing JSON data stored in a
    // 'bsl::streambuf' one node at a time and allows clients to access the
    // data associated with that node, including its type and data value.

  public:
    // TYPES
    enum TokenType {
        // This 'enum' lists all the possible token types.

        e_BEGIN = 1,                  // starting token
        e_ELEMENT_NAME,               // element name
        e_START_OBJECT,               // start of an object ('{')
        e_END_OBJECT,                 // end of an object   ('}')
        e_START_ARRAY,                // start of an array  ('[')
        e_END_ARRAY,                  // end of an array    (']')
        e_ELEMENT_VALUE,              // element value of a simple type
        e_ERROR                       // error token
    };

  private:
    // TYPES
    enum ContextType {
        // This 'enum' lists the possible contexts that the tokenizer can be
        // in.

        e_OBJECT_CONTEXT = 1,         // object context
        e_ARRAY_CONTEXT               // array context
    };

    // Intermediate data buffer used for reading data from the stream.

    enum {
        k_BUFSIZE = 1024 * 8,
        k_MAX_STRING_SIZE = k_BUFSIZE - 1
    };

    // DATA
    bsls::AlignedBuffer<k_BUFSIZE>  d_buffer;               // buffer

    bdlma::BufferedSequentialAllocator    d_allocator;           // allocator
                                                                 // (owned)

    bsl::string                          d_stringBuffer;         // string
                                                                 // buffer

    bsl::streambuf                      *d_streambuf_p;          // streambuf
                                                                 // (held, not
                                                                 // owned)

    bsl::size_t                          d_cursor;               // current
                                                                 // cursor

    bsl::size_t                          d_valueBegin;           // cursor for
                                                                 // beginning
                                                                 // of value

    bsl::size_t                          d_valueEnd;             // cursor for
                                                                 // end of
                                                                 // value

    bsl::size_t                          d_valueIter;            // cursor for
                                                                 // iterating
                                                                 // value

    TokenType                            d_tokenType;            // token type

    ContextType                          d_context;              // context
                                                                 // type

    bool                                 d_allowStandAloneValues;// option for
                                                                 // allowing
                                                                 // stand alone
                                                                 // values

    // PRIVATE MANIPULATORS
    int extractStringValue();
        // Extract the string value starting at the current data cursor and
        // update the value begin and end pointers to refer to the begin and
        // end of the extracted string.  Return 0 on success and a non-zero
        // value otherwise.

    int moveValueCharsToStartAndReloadBuffer();
        // Move the current sequence of characters being tokenized to the front
        // of the internal string buffer, 'd_stringBuffer', and then append
        // additional characters, from the internally-held 'streambuf'
        // ('d_streambuf_p') to the end of that sequence up to a maximum
        // sequence length of 'd_buffer.size()' characters.  Return the number
        // of bytes read from the 'streambuf'.

    int reloadStringBuffer();
        // Reload the string buffer with new data read from the underlying
        // 'streambuf' and overwriting the current buffer.  After reading
        // update the cursor to the new read location.  Return the number of
        // bytes read from the 'streambuf'.

    int expandBufferForLargeValue();
        // Increase the size of the string buffer, 'd_stringBuffer', and then
        // append additional characters, from the internally-held 'streambuf' (
        // 'd_streambuf_p') to the end of the current sequence of characters.
        // Return 0 on success and a non-zero value otherwise.

    int skipWhitespace();
        // Skip all whitespace characters and position the cursor onto the
        // first non-whitespace character.  Return 0 on success and a non-zero
        // value otherwise.

    int skipNonWhitespaceOrTillToken();
        // Skip all characters until a whitespace or a token character is
        // encountered and position the cursor onto the first such character.
        // Return 0 on success and a non-zero value otherwise.

    // Not implemented:
    Tokenizer(const Tokenizer&);

  public:
    // CREATORS
    explicit Tokenizer(bslma::Allocator *basicAllocator = 0);
        // Create a 'Reader' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~Tokenizer();
        // Destroy this object.

    // MANIPULATORS
    void reset(bsl::streambuf *streambuf);
        // Reset this tokenizer to read data from the specified 'streambuf'.
        // Note that the reader will not be on a valid node until
        // 'advanceToNextToken' is called.  Note that this function does not
        // change the value of the 'allowStandAloneValues' option.

    int advanceToNextToken();
        // Move to the next token in the data steam.  Return 0 on success and a
        // non-zero value otherwise.  Note that each call to
        // 'advanceToNextToken' invalidates the string references returned by
        // the 'value' accessor for prior nodes.

    int resetStreamBufGetPointer();
        // Reset the get pointer of the 'streambuf' held by this object to
        // refer to the byte following the last processed byte, if the held
        // 'streambuf' supports seeking, and return an error otherwise leaving
        // this object unchanged.  After a successful function return users can
        // read data from the 'streambuf' where this object stopped.  Return 0
        // on success, and a non-zero value otherwise.

    void setAllowStandAloneValues(bool value);
        // Set the 'allowStandAloneValues' option to the specified 'value'.  If
        // the 'allowStandAloneValues' value is 'true' this tokenizer will
        // successfully tokenize JSON values (strings and numbers).  If the
        // option's value is 'false' then the tokenizer will only tokenize
        // complete JSON documents (JSON objects and arrays) and return an
        // error for stand alone JSON values.  By default, the value of the
        // 'allowStandAloneValues' is 'true'.

    // ACCESSORS
    TokenType tokenType() const;
        // Return the token type of the current token.

    bool allowStandAloneValues() const;
        // Return the value of the 'allowStandAloneValues' option of this
        // tokenizer.

    int value(bslstl::StringRef *data) const;
        // Load into the specified 'data' the value of the specified token if
        // the current token's type is 'BAEJSN_ELEMENT_NAME' or
        // 'BAEJSN_ELEMENT_VALUE' or leave 'data' unmodified otherwise.  Return
        // 0 on success and a non-zero value otherwise.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CREATORS
inline
Tokenizer::Tokenizer(bslma::Allocator *basicAllocator)
: d_allocator(d_buffer.buffer(), k_BUFSIZE, basicAllocator)
, d_stringBuffer(&d_allocator)
, d_streambuf_p(0)
, d_cursor(0)
, d_valueBegin(0)
, d_valueEnd(0)
, d_valueIter(0)
, d_tokenType(e_BEGIN)
, d_context(e_OBJECT_CONTEXT)
, d_allowStandAloneValues(true)
{
    d_stringBuffer.reserve(k_MAX_STRING_SIZE);
}

inline
Tokenizer::~Tokenizer()
{
}

// MANIPULATORS
inline
void Tokenizer::reset(bsl::streambuf *streambuf)
{
    d_streambuf_p = streambuf;
    d_stringBuffer.clear();
    d_cursor      = 0;
    d_valueBegin  = 0;
    d_valueEnd    = 0;
    d_valueIter   = 0;
    d_tokenType   = e_BEGIN;
}

inline
void Tokenizer::setAllowStandAloneValues(bool value)
{
    d_allowStandAloneValues = value;
}

// ACCESSORS
inline
Tokenizer::TokenType Tokenizer::tokenType() const
{
    return d_tokenType;
}

inline
bool Tokenizer::allowStandAloneValues() const
{
    return d_allowStandAloneValues;
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
