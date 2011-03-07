// bdede_utf8util.h                                                   -*-C++-*-
#ifndef INCLUDED_BDEDE_UTF8UTIL
#define INCLUDED_BDEDE_UTF8UTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide basic utilities for UTF-8 encodings.
//
//@CLASSES:
//  bdede_Utf8Util: namespace for utilities for UTF-8 encodings
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides, within the 'bdede_Utf8Util' 'struct',
// a suite of static functions supporting UTF-8 encoded strings.  Two
// interfaces are provided for each function, one where the length of the
// string (in *bytes*) is passed as a separate argument, and one where the
// string is passed as a null-terminated C-style string.
//
// A string is deemed to contain valid UTF-8 if it is compliant with RFC 3629,
// meaning that only 1-, 2-, 3-, and 4-byte sequences are allowed.  Values
// above 'U+10ffff' are also not allowed.
//
// Two types of functions are provided:
//..
//: o 'isValid', which checks for validity, per RFC 3629, of a (candidate)
//:   UTF-8 string.  "Overlong values", that is, values encoded in more bytes
//:   than necessary, are not tolerated; nor are "surrogate values", which are
//:   values in the range '[U+d800 .. U+dfff]'.
//:
//: o 'numCharactersIfValid' and 'numCharactersRaw', which return the length of
//:   a sequence of UTF-8 characters, each of which may be encoded in multiple
//:   bytes.  Note that 'numCharactersIfValid' both validates a (candidate)
//:   UTF-8 string and counts the number of UTF-8 characters that it contains.
//..
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
///Usage
///-----
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
//      *string += (char)value;
//  }
//
//  void utf8AppendTwoBytes(bsl::string *string, int value)
//      // Append the specified 2-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert(0 == (value & ~0x7ff));
//
//      unsigned char buf[3];
//      buf[0] = ((value & 0x7c0) >> 6) | 0xc0;
//      buf[1] =  (value &  0x3f)       | 0x80;
//      buf[2] = 0;
//
//      *string += (char *)buf;
//  }
//
//  void utf8AppendThreeBytes(bsl::string *string, int value)
//      // Append the specified 3-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert(0 == (value & ~0xffff));
//
//      unsigned char buf[4];
//      buf[0] = ((value & 0xf000) >> 12) | 0xe0;
//      buf[1] = ((value &  0xfc0) >>  6) | 0x80;
//      buf[2] =  (value &   0x3f)        | 0x80;
//      buf[3] = 0;
//
//      *string += (char *)buf;
//  }
//
//  void utf8AppendFourBytes(bsl::string *string, int value)
//      // Append the specified 4-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert((unsigned)value <= 0x10ffff);
//
//      unsigned char buf[5];
//      buf[0] = ((value & 0x1c0000) >> 18) | 0xf0;
//      buf[1] = ((value &  0x3f000) >> 12) | 0x80;
//      buf[2] = ((value &    0xfc0) >>  6) | 0x80;
//      buf[3] =  (value &     0x3f)        | 0x80;
//      buf[4] = 0;
//
//      *string += (char *)buf;
//  }
//
//  void utf8Append(bsl::string *string, int value)
//      // Append the specified UTF-8-encoded 'value' in the minimum number of
//      // bytes to the end of the specified 'string'.
//  {
//      assert((unsigned)value <= 0x10ffff);
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
//  assert(true == bdede_Utf8Util::isValid(string.data(), string.length()));
//  assert(true == bdede_Utf8Util::isValid(string.c_str()));
//
//  assert(   9 == bdede_Utf8Util::numCharactersRaw(string.data(),
//                                                  string.length()));
//  assert(   9 == bdede_Utf8Util::numCharactersRaw(string.c_str()));
//..
// Next we encode a lone surrogate value, which is not allowed:
//..
//  bsl::string stringWithSurrogate = string;
//  utf8Append(&stringWithSurrogate, 0xd8ab);
//..
//  assert(false == bdede_Utf8Util::isValid(stringWithSurrogate.data(),
//                                          stringWithSurrogate.length()));
//  assert(false == bdede_Utf8Util::isValid(stringWithSurrogate.c_str()));
//..
// We cannot use 'numCharactersRaw' to count the characters in
// 'stringWithSurrogate', since the behavior of that method is undefined unless
// the string is valid.  Instead, the 'numCharactersIfValid' method can be used
// on strings whose validity we are uncertain of:
//..
//  const char *invalidPosition = 0;
//
//  assert(-1 == bdede_Utf8Util::numCharactersIfValid(
//                                              &invalidPosition,
//                                              stringWithSurrogate.data(),
//                                              stringWithSurrogate.length()));
//  assert(invalidPosition == stringWithSurrogate.data() + string.length());
//
//  invalidPosition = 0;  // reset
//
//  assert(-1 == bdede_Utf8Util::numCharactersIfValid(
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
//  assert(true == bdede_Utf8Util::isValid(stringWithNull.data(),
//                                         stringWithNull.length()));
//
//  assert(  10 == bdede_Utf8Util::numCharactersRaw(stringWithNull.data(),
//                                                  stringWithNull.length()));
//..
// Finally, we encode '0x61' ('a') as an overlong value using 2 bytes, which is
// not valid UTF-8 (since 'a' can be "encoded" in 1 byte):
//..
//  bsl::string stringWithOverlong = string;
//  utf8AppendTwoBytes(&stringWithOverlong, 'a');
//
//  assert(false == bdede_Utf8Util::isValid(stringWithOverlong.data(),
//                                          stringWithOverlong.length()));
//  assert(false == bdede_Utf8Util::isValid(stringWithOverlong.c_str()));
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                        // =====================
                        // struct bdede_Utf8Util
                        // =====================

struct bdede_Utf8Util {
    // This struct provides a namespace for static methods used for validating
    // UTF-8 strings and for counting the number of UTF-8 characters in them.

    // CLASS METHODS
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
        // 'string' that does not consitute the start of a valid UTF-8
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
        // does not consitute the start of a valid UTF-8 character encoding;
        // 'invalidString' is uneffected if 'string' contains valid UTF-8.
        // 'string' need not be null-terminated and can contain embedded null
        // characters.  The behavior is undefined unless '0 <= length'.

    static int numCharactersIfValid(const char **invalidString,
                                    const char  *string);
        // Return the number of UTF-8 characters in the specified 'string' if
        // it contains valid UTF-8, with no effect on the specified
        // 'invalidString'.  Otherwise, return a negative value and load into
        // 'invalidString' the address of the first character in 'string' that
        // does not consitute the start of a valid UTF-8 character encoding.
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
        // the first character in 'string' that does not consitute the start of
        // a valid UTF-8 character encoding.  'string' need not be
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
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // ---------------------
                            // struct bdede_Utf8Util
                            // ---------------------

// CLASS METHODS
inline
bool bdede_Utf8Util::isValid(const char *string)
{
    BSLS_ASSERT_SAFE(string);

    const char *dummy = 0;

    return isValid(&dummy, string);
}

inline
bool bdede_Utf8Util::isValid(const char *string, int length)
{
    BSLS_ASSERT_SAFE(string);
    BSLS_ASSERT_SAFE(0 <= length);

    const char *dummy = 0;

    return isValid(&dummy, string, length);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
