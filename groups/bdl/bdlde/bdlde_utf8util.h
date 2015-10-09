// bdlde_utf8util.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_UTF8UTIL
#define INCLUDED_BDLDE_UTF8UTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide basic utilities for UTF-8 encodings.
//
//@CLASSES:
//  bdlde::Utf8Util: namespace for utilities for UTF-8 encodings
//
//@DESCRIPTION: This component provides, within the 'bdlde::Utf8Util' 'struct',
// a suite of static functions supporting UTF-8 encoded strings.  Two
// interfaces are provided for each function, one where the length of the
// string (in *bytes*) is passed as a separate argument, and one where the
// string is passed as a null-terminated C-style string.
//
// A string is deemed to contain valid UTF-8 if it is compliant with RFC 3629,
// meaning that only 1-, 2-, 3-, and 4-byte sequences are allowed.  Values
// above 'U+10ffff' are also not allowed.
//
// Three types of functions are provided:
//
//: o 'isValid', which checks for validity, per RFC 3629, of a (candidate)
//:   UTF-8 string.  "Overlong values", that is, values encoded in more bytes
//:   than necessary, are not tolerated; nor are "surrogate values", which are
//:   values in the range '[U+d800 .. U+dfff]'.
//:
//: o 'advanceIfValid' and 'advanceRaw', which advance some number of UTF-8
//:   characters, each of which may be encoded in multiple bytes in a UTF-8
//:   string.  'advanceRaw' assumes the string is valid UTF-8, while
//:   'advanceIfValid' checks the input for validity and stops advancing if a
//:   sequence is encountered that is not valid UTF-8.
//:
//: o 'numCharactersIfValid' and 'numCharactersRaw', which return the length of
//:   a sequence of UTF-8 characters, each of which may be encoded in multiple
//:   bytes.  Note that 'numCharactersIfValid' both validates a (candidate)
//:   UTF-8 string and counts the number of UTF-8 characters that it contains.
//
// Embedded null characters are allowed in strings that are accompanied by an
// explicit length argument.  Naturally, null-terminated C-style strings cannot
// contain embedded null characters.
//
// The UTF-8 format is described in the RFC 3629 document at:
//..
//  http://tools.ietf.org/html/rfc3629
//..
// and in Wikipedia at:
//..
//  http://en.wikipedia.org/wiki/Utf-8
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Validating Strings and Counting Unicode Characters
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// For our usage example we will define some functions that can encode UTF-8,
// use them to build some strings, and observe how the functions defined in
// this class perform on them.
//..
//  void utf8AppendOneByte(bsl::string *string, int value)
//      // Append the specified 1-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert(0 == (value & ~0x7f));
//
//      *string += static_cast<char>(value);
//  }
//
//  void utf8AppendTwoBytes(bsl::string *string, int value)
//      // Append the specified 2-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert(0 == (value & ~0x7ff));
//
//      unsigned char buf[3];
//      buf[0] = static_cast<unsigned char>(((value & 0x7c0) >> 6) | 0xc0);
//      buf[1] = static_cast<unsigned char>( (value &  0x3f)       | 0x80);
//      buf[2] = 0;
//
//      *string += reinterpret_cast<char *>(buf);
//  }
//
//  void utf8AppendThreeBytes(bsl::string *string, int value)
//      // Append the specified 3-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert(0 == (value & ~0xffff));
//
//      unsigned char buf[4];
//      buf[0] = static_cast<unsigned char>(((value & 0xf000) >> 12) | 0xe0);
//      buf[1] = static_cast<unsigned char>(((value &  0xfc0) >>  6) | 0x80);
//      buf[2] = static_cast<unsigned char>( (value &   0x3f)        | 0x80);
//      buf[3] = 0;
//
//      *string += reinterpret_cast<char *>(buf);
//  }
//
//  void utf8AppendFourBytes(bsl::string *string, int value)
//      // Append the specified 4-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert(static_cast<unsigned>(value) <= 0x10ffff);
//
//      unsigned char buf[5];
//      buf[0] = static_cast<unsigned char>(((value & 0x1c0000) >> 18) | 0xf0);
//      buf[1] = static_cast<unsigned char>(((value &  0x3f000) >> 12) | 0x80);
//      buf[2] = static_cast<unsigned char>(((value &    0xfc0) >>  6) | 0x80);
//      buf[3] = static_cast<unsigned char>( (value &     0x3f)        | 0x80);
//      buf[4] = 0;
//
//      *string += reinterpret_cast<char *>(buf);
//  }
//
//  void utf8Append(bsl::string *string, int value)
//      // Append the specified UTF-8-encoded 'value' in the minimum number of
//      // bytes to the end of the specified 'string'.
//  {
//      assert(static_cast<unsigned>(value) <= 0x10ffff);
//
//      if (value <= 0x7f) {
//          utf8AppendOneByte(string, value);
//          return;                                                   // RETURN
//      }
//      if (value <= 0x7ff) {
//          utf8AppendTwoBytes(string, value);
//          return;                                                   // RETURN
//      }
//      if (value <= 0xffff) {
//          utf8AppendThreeBytes(string, value);
//          return;                                                   // RETURN
//      }
//
//      utf8AppendFourBytes(string, value);
//  }
//..
// In this usage example, we will encode some UTF-8 strings and demonstrate
// which ones are valid and which ones are not.
//
// First, we build an unquestionably valid UTF-8 string:
//..
//  bsl::string string;
//  utf8Append(&string, 0xff00);
//  utf8Append(&string, 0x856);
//  utf8Append(&string, 'a');
//  utf8Append(&string, 0x1008aa);
//  utf8Append(&string, 0xfff);
//  utf8Append(&string, 'w');
//  utf8Append(&string, 0x1abcd);
//  utf8Append(&string, '.');
//  utf8Append(&string, '\n');
//..
// Next we check its validity and measure its length:
//..
//  assert(true == bdlde::Utf8Util::isValid(string.data(), string.length()));
//  assert(true == bdlde::Utf8Util::isValid(string.c_str()));
//
//  assert(   9 == bdlde::Utf8Util::numCharactersRaw(string.data(),
//                                                  string.length()));
//  assert(   9 == bdlde::Utf8Util::numCharactersRaw(string.c_str()));
//..
// Next we encode a lone surrogate value, which is not allowed:
//..
//  bsl::string stringWithSurrogate = string;
//  utf8Append(&stringWithSurrogate, 0xd8ab);
//
//  assert(false == bdlde::Utf8Util::isValid(stringWithSurrogate.data(),
//                                          stringWithSurrogate.length()));
//  assert(false == bdlde::Utf8Util::isValid(stringWithSurrogate.c_str()));
//..
// We cannot use 'numCharactersRaw' to count the characters in
// 'stringWithSurrogate', since the behavior of that method is undefined unless
// the string is valid.  Instead, the 'numCharactersIfValid' method can be used
// on strings whose validity we are uncertain of:
//..
//  const char *invalidPosition = 0;
//
//  assert(-1 == bdlde::Utf8Util::numCharactersIfValid(
//                                              &invalidPosition,
//                                              stringWithSurrogate.data(),
//                                              stringWithSurrogate.length()));
//  assert(invalidPosition == stringWithSurrogate.data() + string.length());
//
//  invalidPosition = 0;  // reset
//
//  assert(-1 == bdlde::Utf8Util::numCharactersIfValid(
//                                              &invalidPosition,
//                                              stringWithSurrogate.c_str()));
//  assert(invalidPosition == stringWithSurrogate.data() + string.length());
//..
// Next we encode 0, which is allowed.  However, note that we cannot use any
// interfaces that take a null-terminated string for this case:
//..
//  bsl::string stringWithNull = string;
//  utf8AppendOneByte(&stringWithNull, 0);
//..
//  assert(true == bdlde::Utf8Util::isValid(stringWithNull.data(),
//                                         stringWithNull.length()));
//
//  assert(  10 == bdlde::Utf8Util::numCharactersRaw(stringWithNull.data(),
//                                                  stringWithNull.length()));
//..
// Finally, we encode '0x61' ('a') as an overlong value using 2 bytes, which is
// not valid UTF-8 (since 'a' can be "encoded" in 1 byte):
//..
//  bsl::string stringWithOverlong = string;
//  utf8AppendTwoBytes(&stringWithOverlong, 'a');
//
//  assert(false == bdlde::Utf8Util::isValid(stringWithOverlong.data(),
//                                          stringWithOverlong.length()));
//  assert(false == bdlde::Utf8Util::isValid(stringWithOverlong.c_str()));
//..
//
///Example 2: Advancing Characters
///- - - - - - - - - - - - - - - -
// In this example, we will re-use the 'utf8Append' function from Example 1 to
// build a string.
//
// First, build the string, keeping track of how many bytes are in each Unicode
// character:
//..
//  bsl::string string;
//  utf8Append(&string, 0xff00);        // 3 bytes
//  utf8Append(&string, 0x1ff);         // 2 bytes
//  utf8Append(&string, 'a');           // 1 byte
//  utf8Append(&string, 0x1008aa);      // 4 bytes
//  utf8Append(&string, 0x1abcd);       // 4 bytes
//  string += "\xe3\x8f\xfe";           // 3 bytes (invalid 3-byte sequence,
//                                      // the first 2 bytes are valid but the
//                                      // last continuation byte is invalid)
//  utf8Append(&string, 'w');           // 1 byte
//  utf8Append(&string, '\n');          // 1 byte
//..
// Then, declare a few variables we'll need:
//..
//  int rc, status;
//  const char *result;
//  const char *const start = string.c_str();
//..
// Next, try advancing 2 characters, then 3, then 4, observing that the value
// returned is the number of Unicode characters advanced.  Note that since
// we're only advancing over valid UTF-8, we can use either 'advanceRaw' or
// 'advanceIfValid':
//..
//  rc = bdlde::Utf8Util::advanceRaw(              &result, start, 2);
//  assert(2 == rc);
//  assert(3 + 2 == result - start);
//
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, 2);
//  assert(0 == status);
//  assert(2 == rc);
//  assert(3 + 2 == result - start);
//
//  rc = bdlde::Utf8Util::advanceRaw(             &result, start, 3);
//  assert(0 == status);
//  assert(3 == rc);
//  assert(3 + 2 + 1 == result - start);
//
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, 3);
//  assert(0 == status);
//  assert(3 == rc);
//  assert(3 + 2 + 1 == result - start);
//
//  rc = bdlde::Utf8Util::advanceRaw(             &result, start, 4);
//  assert(4 == rc);
//  assert(3 + 2 + 1 + 4 == result - start);
//
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, 4);
//  assert(0 == status);
//  assert(4 == rc);
//  assert(3 + 2 + 1 + 4 == result - start);
//..
// Then, try advancing by more characters than are present using
// 'advanceIfValid', and wind up stopping when we encounter invalid input.  The
// behavior of 'advanceRaw' is undefined if it is used on invalid input, so we
// cannot use it here.  Also note that we will stop at the beginning of the
// invalid Unicode character, and not at the first incorrect byte, which is two
// bytes later:
//..
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, INT_MAX);
//  assert(0 != status);
//  assert(5 == rc);
//  assert(3 + 2 + 1 + 4 + 4                 == result - start);
//  assert(static_cast<int>(string.length()) >  result - start);
//..
// Now, doctor the string to replace the invalid character with a valid one, so
// the string is entirely correct UTF-8:
//.
//  string[3 + 2 + 1 + 4 + 4 + 2] = static_cast<char>(0x8a);
//..
// Finally, advance using both functions by more characters than are in the
// string and in both cases wind up at the end of the string.  Note that
// 'advanceIfValid' does not return an error (non-zero) value to 'status' when
// it encounters the end of the string:
//..
//  rc = bdlde::Utf8Util::advanceRaw(             &result, start, INT_MAX);
//  assert(8 == rc);
//  assert(3 + 2 + 1 + 4 + 4 + 3 + 1 + 1     == result - start);
//  assert(static_cast<int>(string.length()) == result - start);
//
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, INT_MAX);
//  assert(0 == status);
//  assert(8 == rc);
//  assert(3 + 2 + 1 + 4 + 4 + 3 + 1 + 1     == result - start);
//  assert(static_cast<int>(string.length()) == result - start);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

namespace bdlde {
                              // ===============
                              // struct Utf8Util
                              // ===============

struct Utf8Util {
    // This struct provides a namespace for static methods used for validating
    // UTF-8 strings, for counting the number of UTF-8 characters in them, and
    // for advancing pointers through UTF-8 strings.

    // CLASS METHODS
    static int advanceIfValid(int         *status,
                              const char **result,
                              const char  *string,
                              int          numCharacters);
        // Advance past 0 or more consecutive *valid* UTF-8 characters at the
        // beginning of the specified 'string', until either the specified
        // 'numCharacters' have been traversed, or the terminating null
        // character or invalid UTF-8 is encountered (whichever occurs first),
        // and return the number of UTF-8 characters traversed.  Set the
        // specified '*status' to 0 if no invalid UTF-8 is encountered, and to
        // a non-zero value otherwise.  Set the specified '*result' to the
        // address of the byte immediately following the last character
        // traversed, or to 'string' if 'string' is empty or 'numCharacters' is
        // 0.  'string' is necessarily null-terminated, so it cannot contain
        // embedded null characters.  The behavior is undefined unless
        // '0 <= numCharacters'.  Note that the value returned will be in the
        // range '[0 .. numCharacters]'.  Also note that 'string' may contain
        // less than 'bsl::strlen(string)' UTF-8 characters.

    static int advanceIfValid(int         *status,
                              const char **result,
                              const char  *string,
                              int          length,
                              int          numCharacters);
        // Advance past 0 or more consecutive *valid* UTF-8 characters at the
        // beginning of the specified 'string' having the specified 'length'
        // (in bytes), until either the specified 'numCharacters' or 'length'
        // bytes have been traversed, or invalid UTF-8 is encountered
        // (whichever occurs first), and return the number of UTF-8 characters
        // traversed.  Set the specified '*status' to 0 if no invalid UTF-8 is
        // encountered, and to a non-zero value otherwise.  Set the specified
        // '*result' to the address of the byte immediately following the last
        // character traversed, or to 'string' if 'length' or 'numCharacters'
        // is 0.  'string' need not be null-terminated and can contain embedded
        // null characters.  The behavior is undefined unless '0 <= length' and
        // '0 <= numCharacters'.  Note that the value returned will be in the
        // range '[0 .. numCharacters]'.  Also note that 'string' may contain
        // less than 'length' UTF-8 characters.

    static int advanceRaw(const char **result,
                          const char  *string,
                          int          numCharacters);
        // Advance past 0 or more consecutive UTF-8 characters at the beginning
        // of the specified 'string', until either the specified
        // 'numCharacters' have been traversed or the terminating null
        // character is encountered (whichever occurs first), and return the
        // number of UTF-8 characters traversed.  Set the specified '*result'
        // to the address of the byte immediately following the last character
        // traversed, or to 'string' if 'string' is empty or 'numCharacters' is
        // 0.  'string' is necessarily null-terminated, so it cannot contain
        // embedded null characters.  The behavior is undefined unless 'string'
        // contains valid UTF-8 and '0 <= numCharacters'.  Note that the value
        // returned will be in the range '[0 .. numCharacters]'.  Also note
        // that 'string' may contain less than 'bsl::strlen(string)' UTF-8
        // characters.

    static int advanceRaw(const char **result,
                          const char  *string,
                          int          length,
                          int          numCharacters);
        // Advance past 0 or more consecutive UTF-8 characters at the beginning
        // of the specified 'string' having the specified 'length' (in bytes),
        // until either the specified 'numCharacters' or 'length' bytes have
        // been traversed (whichever occurs first), and return the number of
        // UTF-8 characters traversed.  Set the specified '*result' to the
        // address of the byte immediately following the last character
        // traversed, or to 'string' if 'length' or 'numCharacters' is 0.
        // 'string' need not be null-terminated and can contain embedded null
        // characters.  The behavior is undefined unless '0 <= length', the
        // initial 'length' bytes of 'string' contain valid UTF-8, and
        // '0 <= numCharacters'.  Note that the value returned will be in the
        // range '[0 .. numCharacters]'.  Also note that 'string' may contain
        // less than 'length' UTF-8 characters.

    static bool isValid(const char *string);
        // Return 'true' if the specified 'string' contains valid UTF-8, and
        // 'false' otherwise.  'string' is necessarily null-terminated, so it
        // cannot contain embedded null characters.

    static bool isValid(const char *string, int length);
        // Return 'true' if the specified 'string' having the specified
        // 'length' (in bytes) contains valid UTF-8, and 'false' otherwise.
        // 'string' need not be null-terminated and can contain embedded null
        // characters.  The behavior is undefined unless '0 <= length'.

    static bool isValid(const char **invalidString, const char *string);
        // Return 'true' if the specified 'string' contains valid UTF-8, and
        // 'false' otherwise.  If 'string' contains invalid UTF-8, load into
        // the specified 'invalidString' the address of the first character in
        // 'string' that does not constitute the start of a valid UTF-8
        // character encoding; 'invalidString' is uneffected if 'string'
        // contains valid UTF-8.  'string' is necessarily null-terminated, so
        // it cannot contain embedded null characters.

    static bool isValid(const char **invalidString,
                        const char  *string,
                        int          length);
        // Return 'true' if the specified 'string' having the specified
        // 'length' (in bytes) contains valid UTF-8, and 'false' otherwise.
        // If 'string' contains invalid UTF-8, load into the specified
        // 'invalidString' the address of the first character in 'string' that
        // does not constitute the start of a valid UTF-8 character encoding;
        // 'invalidString' is uneffected if 'string' contains valid UTF-8.
        // 'string' need not be null-terminated and can contain embedded null
        // characters.  The behavior is undefined unless '0 <= length'.

    static int numCharactersIfValid(const char **invalidString,
                                    const char  *string);
        // Return the number of UTF-8 characters in the specified 'string' if
        // it contains valid UTF-8, with no effect on the specified
        // 'invalidString'.  Otherwise, return a negative value and load into
        // 'invalidString' the address of the first character in 'string' that
        // does not constitute the start of a valid UTF-8 character encoding.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null characters.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' UTF-8 characters.

    static int numCharactersIfValid(const char **invalidString,
                                    const char  *string,
                                    int          length);
        // Return the number of UTF-8 characters in the specified 'string'
        // having the specified 'length' (in bytes) if 'string' contains valid
        // UTF-8, with no effect on the specified 'invalidString'.  Otherwise,
        // return a negative value and load into 'invalidString' the address of
        // the first character in 'string' that does not constitute the start
        // of a valid UTF-8 character encoding.  'string' need not be
        // null-terminated and can contain embedded null characters.  The
        // behavior is undefined unless '0 <= length'.  Note that 'string' may
        // contain less than 'length' UTF-8 characters.

    static int numCharactersRaw(const char *string);
        // Return the number of UTF-8 characters in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null characters.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' UTF-8 characters.

    static int numCharactersRaw(const char *string, int length);
        // Return the number of UTF-8 characters in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null characters.  The
        // behavior is undefined unless '0 <= length', and 'string' contains
        // valid UTF-8.  Note that 'string' may contain less than 'length'
        // UTF-8 characters.

    static int numCharacters(const char *string);
        // Return the number of UTF-8 characters in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null characters.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' UTF-8 characters.
        //
        // DEPRECATED: Use 'numCharactersRaw' instead.

    static int numCharacters(const char *string, int length);
        // Return the number of UTF-8 characters in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null characters.  The
        // behavior is undefined unless '0 <= length', and 'string' contains
        // valid UTF-8.  Note that 'string' may contain less than 'length'
        // UTF-8 characters.
        //
        // DEPRECATED: Use 'numCharactersRaw' instead.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct Utf8Util
                              // ---------------

// CLASS METHODS
inline
bool Utf8Util::isValid(const char *string)
{
    BSLS_ASSERT_SAFE(string);

    const char *dummy = 0;

    return isValid(&dummy, string);
}

inline
bool Utf8Util::isValid(const char *string, int length)
{
    BSLS_ASSERT_SAFE(string);
    BSLS_ASSERT_SAFE(0 <= length);

    const char *dummy = 0;

    return isValid(&dummy, string, length);
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
