// bdljsn_stringutil.h                                                -*-C++-*-
#ifndef INCLUDED_BDLJSN_STRINGUTIL
#define INCLUDED_BDLJSN_STRINGUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility functions for JSON strings.
//
//@CLASSES:
//  bdljsn::StringUtil: namespace for utility functions on JSON strings
//
//@DESCRIPTION: This component defines a utility 'struct',
// 'bdljsn::StringUtil', that is a namespace for functions that convert
// arbitrary UTF-8 codepoint sequences to JSON strings and vice versa.  The
// rules for these conversions are outlined below in {JSON Strings} and
// detailed in: https://www.rfc-editor.org/rfc/rfc8259#section-7 (RFC8259)
//
// This utility provides two key functions:
//
//: o 'writeString': Given an arbitrary UTF-8 codepoint sequence, generate a
//:   JSON string representing the same codepoints.
//:
//: o 'readString': Given a JSON string (e.g., the output of 'writeString'),
//:   generate the equivalent sequence of UTF-8 code points.
//
// When using these functions, a UTF-8 codepoint sequence is always preserved
// on the round trip to JSON string and back; however, since there are
// equivalent allowed representations of a JSON string, the converse is not
// guaranteed.
//
///JSON Strings
///------------
// JSON strings consist of UTF-8 codepoints surround by double quotes (i.e.,
// '\"') Within those double quotes certain characters *must* be escaped (i.e.,
// replaced with some alternative, multi-byte representation).  Those
// characters are:
//
//: o quotation marks
//: o backslashes (a.k.a., a "reverse solidus")
//: o the "control characters" in the range 'U+0000' to 'U+001F' (inclusive).
//
// Each of the above characters can be escaped by replacing it with the six
// byte sequence consisting of:
//
//:   o a backslash,
//:   o a lower-case 'u', and
//:   o the Unicode value expressed as four hexadecimal digits.
//
// For example, the character that rings the console bell is represented as
// '\u0007'.  Note that the hexadecimal digits can use upper or lower case
// letters but the lead 'u' character must be lower case.  See {Strictness}.
//
// Eight of the characters that must be escaped can be alternatively
// represented by special, 2-byte sequences:
//..
//  +---------+-----------------+---------------+---------------+
//  | Unicode | Description     | 6-byte escape | 2-byte escape |
//  +---------+-----------------+---------------+---------------+
//  | U+0022  | quotation mark  | \u0022        |  \"           |
//  | U+005C  | backslash       | \u005c        |  \\           |
//  | U+002F  | slash           | \u002f        |  \/           |
//  | U+0008  | backspace       | \u0008        |  \b           |
//  | U+000C  | form feed       | \u000C        |  \f           |
//  | U+000A  | line feed       | \u000A        |  \n           |
//  | U+000D  | carriage return | \u000D        |  \r           |
//  | U+0009  | tab             | \u0009        |  \t           |
//  +---------+-----------------+---------------+---------------+
//..
// Note that the above set is similar to but not identical to the set of two
// byte 'char' literals supported by C++.  For example, '\0' (null) and '\a'
// (bell) are not included above.
//
///Guarantees: Arbitrary UTF-8 to JSON String
/// - - - - - - - - - - - - - - - - - - - - -
//: o No UTF-8 characters in the *Basic* *Multilingual* *Plane* are escaped
//:   unless they are in the set that *must* be escaped.
//:
//: o When a character must be escaped, the 6-byte (hexadecimal) representation
//:   is used only if no 2-byte escape exists.
//:
//: o When a 6-byte (hexadecimal) representation is used, hexadecimal letters
//:   are in upper case.
//:
//: o All UTF-8 characters outside of the *Basic* *Multilingual* *Plane*
//:   are represented by two, adjacent 6-byte hexadecimal escape
//:   sequences.  For details, see:
//:   https://en.wikipedia.org/wiki/UTF-16#U+D800_to_U+DFFF
//
///Strictness
///----------
// By default, the 'bdljsn::StringUtil' read and write methods strictly follow
// the RFC8259 standard.  Variances from those rules are expressed using
// 'bdljsn::StringUtil::FLags', an 'enum' of flag values that can be set in the
// optional 'flags' parameter of the decoding methods.  Multiple flags can be
// bitwise set in 'flags; however, currently, just one variance flag is
// defined.
//
///Example Variance
/// - - - - - - - -
// RFC8259 specifies that the 6-byte Unicode escape sequence start with a
// slash, '/', and lower-case 'u'.  However, if the
// 'bdljsn::StringUtil::e_ACCEPT_CAPITAL_UNICODE_ESCAPE' is set, an upper-case
// 'U' is accepted as well.  Thus, both '\u0007' and '\U0007' would be
// interpreted as the BELL character.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding and Decoding a JSON String
/// - - - - - - - - - - - - - - - - - - - - - - -
// First, we initialize a string with a valid sequence of UTF-8 codepoints.
//..
//  bsl::string initial("Does the name \"Ivan Pavlov\" ring a bell\a?\n");
//  assert(bdlde::Utf8Util::isValid(initial));
//..
// Notice that, as required by C++ syntax, several characters are represented
// by their two-character escape sequence: double quote (twice), bell, and
// newline.
//
// Then, we examine the string as output:
//..
//  bsl::cout << initial << bsl::endl;
//..
// and observe:
//..
//  Does the name "Ivan Pavlov" ring a bell?
//
//..
// Notice that the backslash characters (having served their purpose of giving
// special meaning to the subsequent character) are not shown.  The BELL and
// NEWLINE characters are output but are not visible.
//
// Now, we generate JSON string equivalent of the 'initial' string.
//..
//  bsl::ostringstream oss;
//
//  int rcEncode = bdljsn::StringUtil::writeString(oss, initial);
//  assert(0 == rcEncode);
//
//  bsl::string  jsonCompatibleString = oss.str();
//  bsl::cout << jsonCompatibleString << bsl::endl;
//..
// and observed how the 'initial' string is represented for JSON:
//..
//  "Does the name \"Ivan Pavlov\" ring a bell\u0007?\n"
//..
// Notice that:
//: o The entire string is delimited by double quotes.
//: o The interior double quotes and new line are represented by two character
//:   escape sequences (as they were in the C++ string literal.
//: o Since JSON does not have a two character escape sequence for the BELL
//:   character, '\u0007', the 6-byte Unicode representation is used.
//
// Finally, we convert the 'jsonCompatibleString' back to its original content:
//..
//  bsl::string fromJsonString;
//  const int   rcDecode = bdljsn::StringUtil::readString(
//                                                       &fromJsonString,
//                                                       jsonCompatibleString);
//  assert(0       == rcDecode);
//  assert(initial == fromJsonString);
//
//  bsl::cout << fromJsonString << bsl::endl;
//..
// and observe (again):
//..
//  Does the name "Ivan Pavlov" ring a bell?
//
//..

#include <bdlscm_version.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdljsn {

                             // =================
                             // struct StringUtil
                             // =================

struct StringUtil {
    // This class provides utility functions for converting arbitrary UTF-8
    // sequences into JSON strings and visa versa.  See {JSON Strings} in
    // {DESCRIPTION} for details of these transformations.

  public:
    // TYPES
    enum Flags {
        e_NONE                          = 0
      , e_ACCEPT_CAPITAL_UNICODE_ESCAPE = 1 << 0
    };

    // CLASS METHODS
    static int readString(bsl::string             *value,
                          const bsl::string_view&  string,
                          int                      flags = e_NONE);
        // Load to the specified 'value' the UTF-8 codepoint sequence
        // equivalent to the specified (JSON) 'string' (see {JSON Strings}).
        // Return 0 on success and a non-zero value otherwise.  Optionally
        // specify 'flags' to request variances from certain rules of JSON
        // decoding (see {Strictness}).

    static int readUnquotedString(bsl::string             *value,
                                  const bsl::string_view&  string,
                                  int                      flags = e_NONE);
        // Load to the specified 'value' the UTF-8 codepoint sequence
        // equivalent to the specified 'string', that is JSON-compliant absent
        // the leading and trailing double quote characters (see {JSON
        // Strings}).  Return 0 on success and a non-zero value otherwise.
        // Optionally specify 'flags' to request variances from certain rules
        // of JSON decoding (see {Strictness}).

    static int writeString(bsl::ostream&           stream,
                           const bsl::string_view& string);
        // Write to the specified 'stream' a JSON-compliant string that is
        // equivalent to the specified 'string', an arbitrary UTF-8 codepoint
        // sequence.  Return 0 on success and a non-zero value otherwise.  The
        // operation fails if 'string' is not a sequence of UTF-8 codepoints or
        // if there is an error writing to 'stream'.  See {Conventions:
        // Arbitrary UTF-8 to JSON String} for further details.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                             // -----------------
                             // struct StringUtil
                             // -----------------

// CLASS METHODS
inline
int StringUtil::readString(bsl::string             *value,
                           const bsl::string_view&  string,
                           int                      flags)
{
    BSLS_ASSERT(value);

    if (2 > string.size()) {
        return -1;                                                    // RETURN
    }

    if (string[0] != '"' && string[string.size() - 1] != '"') {
        return -1;                                                    // RETURN
    }

    const bsl::string_view contents = string.substr(1, string.size() - 2);
    return readUnquotedString(value, contents, flags);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
