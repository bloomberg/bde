// baljsn_tokenizer.h                                                 -*-C++-*-
#ifndef INCLUDED_BALJSN_TOKENIZER
#define INCLUDED_BALJSN_TOKENIZER

#include <bsls_ident.h>
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
// On malformed JSON, tokenization may fail before the end of input is reached,
// but not all such errors are detected.  In particular, callers should check
// that closing brackets and braces match opening ones.
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

#include <balscm_version.h>

#include <bdljsn_tokenizer.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_ios.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace baljsn {

                              // ===============
                              // class Tokenizer
                              // ===============

typedef bdljsn::Tokenizer Tokenizer;

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
