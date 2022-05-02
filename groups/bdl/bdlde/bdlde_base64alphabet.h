// bdlde_base64alphabet.h                                             -*-C++-*-

#ifndef INCLUDED_BDLDE_BASE64ALPHABET
#define INCLUDED_BDLDE_BASE64ALPHABET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumeration of the set of possible base 64 alphabets.
//
//@CLASSES:
//  bdlde::Base64Alphabet: namespace for a alphabet 'enum'
//
//@SEE_ALSO: bdlde_base64encorderoptions, bdlde_base64decorderoptions,
//           bdlde_base64encoder,         bdlde_base64decoder
//
//@DESCRIPTION: This component provides a namespace for the 'enum' type
// 'bdlde::Base64Alphabet::Enum', which enumerates the set of possible
// alphabets.
//
///Enumerators
///-----------
//..
//  Name              Description
//  ---------------   ---------------------------------------------------
//  e_BASIC           standard
//  e_URL             for URL's
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1:
/// - - - - -
// The following snippets of code provide a simple illustration of
// 'bdlde::Base64Alphabet' usage.
//
// First, we create variable of type 'bdlde::Base64Alphabet::Enum' and
// initialize it with the enumerator values:
//..
//  const bdlde::Base64Alphabet::Enum basic = bdlde::Base64Alphabet::e_BASIC;
//  const bdlde::Base64Alphabet::Enum url   = bdlde::Base64Alphabet::e_URL;
//..
// Next, we store a pointer to their ASCII representation in variables of type
// 'const char *':
//..
//  const char *asciiBasic = bdlde::Base64Alphabet::toAscii(basic);
//  const char *asciiUrl   = bdlde::Base64Alphabet::toAscii(url);
//  assert(0 == bsl::strcmp(asciiBasic, "BASIC"));
//  assert(0 == bsl::strcmp(asciiUrl,   "URL"));
//..
//  Now, we stream some 'Enum's to 'ostream's:
//..
//  bsl::ostringstream ossBasic, ossUrl;
//
//  ossBasic << basic;
//  ossUrl   << url;
//..
// Finally, we observe the output of the streaming:
//..
//  assert(ossBasic.str() == "BASIC");
//  assert(ossUrl.str()   == "URL");
//
//  assert(ossBasic.str() == asciiBasic);
//  assert(ossUrl.str()   == asciiUrl);
//..

#include <bdlscm_version.h>

#include <bsls_platform.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlde {

                            // =====================
                            // struct Base64Alphabet
                            // =====================

struct Base64Alphabet {
    // This 'struct' provides a namespace for enumerating the set of alphabets.
    // See 'Enum' in the TYPES sub-section for details.
    //
    // This class:
    //: o supports a complete set of *enumeration* operations
    //:   o except for 'bdex' serialization
    // For terminology see 'bsldoc_glossary'.

  public:
    // TYPES
    enum Enum {
        e_BASIC,
        e_URL
    };

  public:
    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               Enum          value,
                               int           level          = 0,
                               int           spacesPerLevel = 4);
        // Write the string representation of the specified enumeration 'value'
        // to the specified output 'stream', and return a reference to
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  See 'toAscii' for
        // what constitutes the string representation of a
        // 'ByteOrder::Enum' value.

    static const char *toAscii(Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches the
        // name of its corresponding base enumeration with the "e_" prefix
        // elided.  For example:
        //..
        //  bsl::cout << ByteOrder::toAscii(ByteOrder::e_NETWORK);
        //..
        // will print the following on standard output:
        //..
        //  BIG_ENDIAN
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, Base64Alphabet::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'bdlde::Base64Alphabet::Enum' value.  Note that this
    // method has the same behavior as
    //..
    //  bdlde::Base64Alphabet::print(stream, value, 0, -1);
    //..

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // -----------------------
                          // struct bdlde::ByteOrder
                          // -----------------------

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
