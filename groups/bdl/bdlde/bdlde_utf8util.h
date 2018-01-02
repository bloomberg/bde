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
//: o 'advanceIfValid' and 'advanceRaw', which advance some number of Unicode
//:   code points, each of which may be encoded in multiple bytes in a UTF-8
//:   string.  'advanceRaw' assumes the string is valid UTF-8, while
//:   'advanceIfValid' checks the input for validity and stops advancing if a
//:   sequence is encountered that is not valid UTF-8.
//:
//: o 'numCodePointsIfValid' and 'numCodePointsRaw', which return the number of
//:   Unicode code points in a UTF-8 string.  Note that 'numCodePointsIfValid'
//:   both validates a (candidate) UTF-8 string and counts the number of
//:   Unicode code points that it contains.
//
// Embedded null bytes are allowed in strings that are accompanied by an
// explicit length argument.  Naturally, null-terminated C-style strings cannot
// contain embedded null code points.
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
///Example 1: Validating Strings and Counting Unicode Code Points
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this usage example, we will encode some Unicode code points in UTF-8
// strings and demonstrate which ones are valid and which ones are not.
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
// Then, we check its validity and measure its length:
//..
//  assert(true == bdlde::Utf8Util::isValid(string.data(), string.length()));
//  assert(true == bdlde::Utf8Util::isValid(string.c_str()));
//
//  assert(   9 == bdlde::Utf8Util::numCodePointsRaw(string.data(),
//                                                   string.length()));
//  assert(   9 == bdlde::Utf8Util::numCodePointsRaw(string.c_str()));
//..
// Next, we encode a lone surrogate value, which is not allowed:
//..
//  bsl::string stringWithSurrogate = string;
//  utf8Append(&stringWithSurrogate, 0xd8ab);
//..
//  assert(false == bdlde::Utf8Util::isValid(stringWithSurrogate.data(),
//                                           stringWithSurrogate.length()));
//  assert(false == bdlde::Utf8Util::isValid(stringWithSurrogate.c_str()));
//..
// Then, we cannot use 'numCodePointsRaw' to count the code points in
// 'stringWithSurrogate', since the behavior of that method is undefined unless
// the string is valid.  Instead, the 'numCodePointsIfValid' method can be used
// on strings whose validity we are uncertain of:
//..
//  const char *invalidPosition = 0;
//
//  assert(-1 == bdlde::Utf8Util::numCodePointsIfValid(
//                                              &invalidPosition,
//                                              stringWithSurrogate.data(),
//                                              stringWithSurrogate.length()));
//  assert(invalidPosition == stringWithSurrogate.data() + string.length());
//
//  invalidPosition = 0;  // reset
//
//  assert(-1 == bdlde::Utf8Util::numCodePointsIfValid(
//                                              &invalidPosition,
//                                              stringWithSurrogate.c_str()));
//  assert(invalidPosition == stringWithSurrogate.data() + string.length());
//..
// Now, we encode 0, which is allowed.  However, note that we cannot use any
// interfaces that take a null-terminated string for this case:
//..
//  bsl::string stringWithNull = string;
//  utf8AppendOneByte(&stringWithNull, 0);
//..
//  assert(true == bdlde::Utf8Util::isValid(stringWithNull.data(),
//                                          stringWithNull.length()));
//
//  assert(  10 == bdlde::Utf8Util::numCodePointsRaw(stringWithNull.data(),
//                                                   stringWithNull.length()));
//..
// Finally, we encode '0x61' ('a') as an overlong value using 2 bytes, which is
// not valid UTF-8 (since 'a' can be "encoded" in 1 byte):
//..
//  bsl::string stringWithOverlong = string;
//  utf8AppendTwoBytes(&stringWithOverlong, 'a');
//
//  assert(false == bdlde::Utf8Util::isValid(stringWithOverlong.data(),
//                                           stringWithOverlong.length()));
//  assert(false == bdlde::Utf8Util::isValid(stringWithOverlong.c_str()));
//..
//
///Example 2: Advancing Code Points
/// - - - - - - - - - - - - - - - -
// In this example, we will use the various 'advance' functions to advance
// through a UTF-8 string.
//
// First, build the string using 'utf8Append', keeping track of how many bytes
// are in each Unicode code point:
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
// Next, try advancing 2 code points, then 3, then 4, observing that the value
// returned is the number of Unicode code points advanced.  Note that since
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
// Then, try advancing by more code points than are present using
// 'advanceIfValid', and wind up stopping when we encounter invalid input.  The
// behavior of 'advanceRaw' is undefined if it is used on invalid input, so we
// cannot use it here.  Also note that we will stop at the beginning of the
// invalid Unicode code point, and not at the first incorrect byte, which is
// two bytes later:
//..
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, INT_MAX);
//  assert(0 != status);
//  assert(5 == rc);
//  assert(3 + 2 + 1 + 4 + 4                 == result - start);
//  assert(static_cast<int>(string.length()) >  result - start);
//..
// Now, doctor the string to replace the invalid code point with a valid one,
// so the string is entirely correct UTF-8:
//..
//  string[3 + 2 + 1 + 4 + 4 + 2] = static_cast<char>(0x8a);
//..
// Finally, advance using both functions by more code points than are in the
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

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {

namespace bdlde {
                              // ===============
                              // struct Utf8Util
                              // ===============

struct Utf8Util {
    // This struct provides a namespace for static methods used for validating
    // UTF-8 strings, for counting the number of Unicode code points in them,
    // and for advancing pointers through UTF-8 strings by a specified number
    // of Unicode code points.

    // CLASS METHODS
    static int advanceIfValid(int         *status,
                              const char **result,
                              const char  *string,
                              int          numCodePoints);
        // Advance past 0 or more consecutive *valid* Unicode code points at
        // the beginning of the specified 'string', until either the specified
        // 'numCodePoints' have been traversed, or the terminating null byte or
        // invalid UTF-8 is encountered (whichever occurs first), and return
        // the number of Unicode code points traversed.  Set the specified
        // '*status' to 0 if no invalid UTF-8 is encountered, and to a non-zero
        // value otherwise.  Set the specified '*result' to the address of the
        // byte immediately following the last valid code point traversed, or
        // to 'string' if 'string' is empty or 'numCodePoints' is 0.  'string'
        // is necessarily null-terminated, so it cannot contain embedded null
        // bytes.  The behavior is undefined unless '0 <= numCodePoints'.  Note
        // that the value returned will be in the range '[0 .. numCodePoints]'.
        // Also note that 'string' may contain less than 'bsl::strlen(string)'
        // Unicode code points.

    static int advanceIfValid(int         *status,
                              const char **result,
                              const char  *string,
                              bsl::size_t  length,
                              int          numCodePoints);
        // Advance past 0 or more consecutive *valid* Unicode code points at
        // the beginning of the specified 'string' having the specified
        // 'length' (in bytes), until either the specified 'numCodePoints' or
        // 'length' bytes have been traversed, or invalid UTF-8 is encountered
        // (whichever occurs first), and return the number of Unicode code
        // points traversed.  Set the specified '*status' to 0 if no invalid
        // UTF-8 is encountered, and to a non-zero value otherwise.  Set the
        // specified '*result' to the address of the byte immediately following
        // the last valid code point traversed, or to 'string' if 'length' or
        // 'numCodePoints' is 0.  'string' need not be null-terminated and can
        // contain embedded null bytes.  The behavior is undefined unless
        // '0 <= numCodePoints'.  Note that the value returned will be in the
        // range '[0 .. numCodePoints]'.  Also note that 'string' may contain
        // less than 'length' Unicode code points.

    static int
    advanceRaw(const char **result, const char *string, int numCodePoints);
        // Advance past 0 or more consecutive Unicode code points at the
        // beginning of the specified 'string', until either the specified
        // 'numCodePoints' have been traversed or the terminating null byte is
        // encountered (whichever occurs first), and return the number of
        // Unicode code points traversed.  Set the specified '*result' to the
        // address of the byte immediately following the last code point
        // traversed, or to 'string' if 'string' is empty or 'numCodePoints' is
        // 0.  'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8 and '0 <= numCodePoints'.  Note that the value
        // returned will be in the range '[0 .. numCodePoints]'.  Also note
        // that 'string' may contain less than 'bsl::strlen(string)' Unicode
        // code points.

    static int advanceRaw(const char  **result,
                          const char   *string,
                          bsl::size_t   length,
                          int           numCodePoints);
        // Advance past 0 or more consecutive Unicode code points at the
        // beginning of the specified 'string' having the specified 'length'
        // (in bytes), until either the specified 'numCodePoints' or 'length'
        // bytes have been traversed (whichever occurs first), and return the
        // number of Unicode code points traversed.  Set the specified
        // '*result' to the address of the byte immediately following the last
        // code point traversed, or to 'string' if 'length' or 'numCodePoints'
        // is 0.  'string' need not be null-terminated and can contain embedded
        // null bytes.  The behavior is undefined unless the initial 'length'
        // bytes of 'string' contain valid UTF-8 and '0 <= numCodePoints'.
        // Note that the value returned will be in the range
        // '[0 .. numCodePoints]'.  Also note that 'string' may contain less
        // than 'length' Unicode code points.

    static bool isValid(const char *string);
        // Return 'true' if the specified 'string' contains valid UTF-8, and
        // 'false' otherwise.  'string' is necessarily null-terminated, so it
        // cannot contain embedded null bytes.

    static bool isValid(const char *string, bsl::size_t length);
        // Return 'true' if the specified 'string' having the specified
        // 'length' (in bytes) contains valid UTF-8, and 'false' otherwise.
        // 'string' need not be null-terminated and can contain embedded null
        // bytes.

    static bool isValid(const char **invalidString, const char *string);
        // Return 'true' if the specified 'string' contains valid UTF-8, and
        // 'false' otherwise.  If 'string' contains invalid UTF-8, load into
        // the specified 'invalidString' the address of the beginning of the
        // first invalid UTF-8 sequence encountered; 'invalidString' is
        // unaffected if 'string' contains only valid UTF-8.  'string' is
        // necessarily null-terminated, so it cannot contain embedded null
        // bytes.

    static bool
    isValid(const char  **invalidString,
            const char   *string,
            bsl::size_t   length);
        // Return 'true' if the specified 'string' having the specified
        // 'length' (in bytes) contains valid UTF-8, and 'false' otherwise.  If
        // 'string' contains invalid UTF-8, load into the specified
        // 'invalidString' the address of the byte after the last valid code
        // point traversed; 'invalidString' is unaffected if 'string' contains
        // only valid UTF-8.  'string' need not be null-terminated and can
        // contain embedded null bytes.

    static int numCharactersIfValid(const char **invalidString,
                                    const char  *string);
        // Return the number of Unicode code points in the specified 'string'
        // if it contains valid UTF-8, with no effect on the specified
        // 'invalidString'.  Otherwise, return a negative value and load into
        // 'invalidString' the address of the byte after the last valid Unicode
        // code point traversed.  'string' is necessarily null-terminated, so
        // it cannot contain embedded null bytes.  Note that 'string' may
        // contain less than 'bsl::strlen(string)' Unicode code points.
        //
        // DEPRECATED: Use 'numCodePointsIfValid' instead.

    static int numCharactersIfValid(const char  **invalidString,
                                    const char   *string,
                                    bsl::size_t   length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes) if 'string' contains valid
        // UTF-8, with no effect on the specified 'invalidString'.  Otherwise,
        // return a negative value and load into 'invalidString' the address
        // of the byte after the last valid Unicode code point traversed.
        // 'string' need not be null-terminated and may contain embedded null
        // bytes.  Note that 'string' may contain less than 'length' Unicode
        // code points.
        //
        // DEPRECATED: Use 'numCodePointsIfValid' instead.

    static int numCharactersRaw(const char *string);
        // Return the number of Unicode code points in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' Unicode code points.
        //
        // DEPRECATED: Use 'numCodePointsRaw' instead.

    static int numCharactersRaw(const char *string, bsl::size_t length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null bytes.  The behavior
        // is undefined 'string' contains valid UTF-8.  Note that 'string' may
        // contain less than 'length' Unicode code points.
        //
        // DEPRECATED: Use 'numCodePointsRaw' instead.

    static int numCharacters(const char *string);
        // Return the number of Unicode code points in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' Unicode code points.
        //
        // DEPRECATED: Use 'numCodePointsRaw' instead.

    static int numCharacters(const char *string, bsl::size_t length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null bytes.  The behavior
        // is undefined unless 'string' contains valid UTF-8.  Note that
        // 'string' may contain less than 'length' Unicode code points.
        //
        // DEPRECATED: Use 'numCodePointsRaw' instead.

    static int numCodePointsIfValid(const char **invalidString,
                                    const char  *string);
        // Return the number of Unicode code points in the specified 'string'
        // if it contains valid UTF-8, with no effect on the specified
        // 'invalidString'.  Otherwise, return a negative value and load into
        // 'invalidString' the address of the byte after the last valid Unicode
        // code point traversed.  'string' is necessarily null-terminated, so
        // it cannot contain embedded null bytes.  Note that 'string' may
        // contain less than 'bsl::strlen(string)' Unicode code points.

    static int numCodePointsIfValid(const char  **invalidString,
                                    const char   *string,
                                    bsl::size_t   length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes) if 'string' contains valid
        // UTF-8, with no effect on the specified 'invalidString'.  Otherwise,
        // return a negative value and load into 'invalidString' the address of
        // the byte after the last valid Unicode code point traversed.
        // 'string' need not be null-terminated and may contain embedded null
        // bytes.  Note that 'string' may contain less than 'length' Unicode
        // code points.

    static int numCodePointsRaw(const char *string);
        // Return the number of Unicode code points in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' Unicode code points.

    static int numCodePointsRaw(const char *string, bsl::size_t length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null bytes.  The behavior
        // is undefined unless 'string' contains valid UTF-8.  Note that
        // 'string' may contain less than 'length' Unicode code points.
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
bool Utf8Util::isValid(const char *string, bsl::size_t length)
{
    BSLS_ASSERT_SAFE(string);

    const char *dummy = 0;

    return isValid(&dummy, string, length);
}

inline
int Utf8Util::numCharactersIfValid(const char **invalidString,
                                   const char  *string)
{
    return numCodePointsIfValid(invalidString, string);
}

inline
int Utf8Util::numCharactersIfValid(const char  **invalidString,
                                   const char   *string,
                                   bsl::size_t   length)
{
    return numCodePointsIfValid(invalidString, string, length);
}

inline
int Utf8Util::numCharacters(const char *string)
{
    return numCodePointsRaw(string);
}

inline
int Utf8Util::numCharacters(const char *string, bsl::size_t length)
{
    return numCodePointsRaw(string, length);
}

inline
int Utf8Util::numCharactersRaw(const char *string)
{
    return numCodePointsRaw(string);
}

inline
int Utf8Util::numCharactersRaw(const char *string, bsl::size_t length)
{
    return numCodePointsRaw(string, length);
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
