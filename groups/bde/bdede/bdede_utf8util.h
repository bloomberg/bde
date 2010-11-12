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
//@DESCRIPTION: This component provides a suite of static functions supporting
// UTF-8 encoded strings.  Two interfaces are provided for each routine, one
// where the length of the string (in *bytes*) is passed as a separate
// argument, and one where the string is passed as a null-terminated C-style
// string.
//
// The encoding is assumed to be RFC-3629 compliant, meaning that only 1-, 2-,
// 3-, and 4-byte sequences are allowed.  Values above 'U+10ffff' are also not
// allowed.
//
// Two types of routines are provided:
//..
// * 'isValid', which checks for validity as an RFC-3629 UTF-8 string.
//   "Overlong values", that is, values encoded in more bytes than necessary,
//   are not tolerated, and nor are "surrogate values", which are values in
//   the range '[ U+d800 .. U+dfff ]'.
//
// * 'numCharacters' which returns the length of a sequence of UTF-8
//   characters, each of which may contain multiple bytes.
//..
// Null characters are allowed, but the null-terminated C-style string argument
// interface may not be used if any are present.
//
// In compliance with the RFC-3629 standard, none of these routines tolerate
// characters encoded in more than 4 bytes.
//
// The UTF-8 format is described in the RFC-3629 document at:
//..
//  http://tools.ietf.org/html/rfc3629
//..
// and in Wikipedia at:
//..
//  http://en.wikipedia.org/wiki/Utf-8
//..
///Usage
///-----
// For our usage example we will define some routines that can encode UTF-8,
// use them to build some strings, and observe how the functions defined in
// this class perform on them.
//..
//  void utf8Cat8(bsl::string *str, int b)
//      // Concatenate the utf8-encoded value of the specified 'b' in 1 byte
//      // to the end of the specified 'str'.
//  {
//      assert(0 == (b & ~0x7f));
//
//      *str += (char) b;
//  }
//
//  void utf8Cat16(bsl::string *str, int b)
//      // Concatenate the utf8-encoded value of the specified 'b' in 2 bytes
//      // to the end of the specified 'str'.
//  {
//      assert(0 == (b & ~0x7ff));
//
//      unsigned char buf[3];
//      buf[0] = ((b & 0x7c0) >> 6) | 0xc0;
//      buf[1] =  (b &  0x3f)       | 0x80;
//      buf[2] = 0;
//
//      *str += (char *) buf;
//  }
//
//  void utf8Cat24(bsl::string *str, int b)
//      // Concatenate the utf8-encoded value of the specified 'b' in 3 bytes
//      // to the end of the specified 'str'.
//  {
//      assert(0 == (b & ~0xffff));
//
//      unsigned char buf[4];
//      buf[0] = ((b & 0xf000) >> 12) | 0xe0;
//      buf[1] = ((b &  0xfc0) >>  6) | 0x80;
//      buf[2] =  (b &   0x3f)        | 0x80;
//      buf[3] = 0;
//
//      *str += (char *) buf;
//  }
//
//  void utf8Cat32(bsl::string *str, int b)
//      // Concatenate the utf8-encoded value of the specified 'b' in 4 bytes
//      // to the end of the specified 'str'.
//  {
//      assert((unsigned) b <= 0x10ffff);
//
//      unsigned char buf[5];
//      buf[0] = ((b & 0x1c0000) >> 18) | 0xf0;
//      buf[1] = ((b &  0x3f000) >> 12) | 0x80;
//      buf[2] = ((b &    0xfc0) >>  6) | 0x80;
//      buf[3] =  (b &     0x3f)        | 0x80;
//      buf[4] = 0;
//
//      *str += (char *) buf;
//  }
//
//  void utf8Cat(bsl::string *str, int b)
//      // Concatenate the utf8-encoded value of the specified 'b' in the
//      // minimum needed number of bytes to the end of the specified 'str'.
//  {
//      assert((unsigned) b <= 0x10ffff);
//
//      if (b <= 0x7f) {
//          utf8Cat8(str, b);
//          return;                                                   // RETURN
//      }
//      if (b <= 0x7ff) {
//          utf8Cat16(str, b);
//          return;                                                   // RETURN
//      }
//      if (b <= 0xffff) {
//          utf8Cat24(str, b);
//          return;                                                   // RETURN
//      }
//
//      utf8Cat32(str, b);
//  }
//..
// In this usage example, we will encode some UTF-8 strings and demonstrate
// which ones are valid and which ones are not.
//
// First, let's build an unquestionably valid UTF-8 string:
//..
//  bsl::string str;
//  utf8Cat(&str, 0xff00);
//  utf8Cat(&str, 0x856);
//  utf8Cat(&str, 'a');
//  utf8Cat(&str, 0x1008aa);
//  utf8Cat(&str, 0xfff);
//  utf8Cat(&str, 'w');
//  utf8Cat(&str, 0x1abcd);
//  utf8Cat(&str, '.');
//  utf8Cat(&str, '\n');
//..
// Check its validity and measure its length:
//..
//  assert(true == bdede_Utf8Util::isValid(str.data(), str.length()));
//  assert(true == bdede_Utf8Util::isValid(str.c_str()));
//
//  assert(9 == bdede_Utf8Util::numCharacters(str.data(), str.length()));
//  assert(9 == bdede_Utf8Util::numCharacters(str.c_str()));
//..
// Encode a lone surrogate value, which is not allowed:
//..
//  bsl::string m3Str = str;
//  utf8Cat(&m3Str, 0xd8ab);
//..
//  assert(false == bdede_Utf8Util::isValid(m3Str.data(), m3Str.length()));
//  assert(false == bdede_Utf8Util::isValid(m3Str.c_str()));
//..
// We can't measure the length, since the behavior of 'numCharacters' is
// undefined for an invalid string.
//
// Encode 0, which is allowed.  We can't use the null-terminated string
// interfaces for this case:
//..
//  bsl::string zStr = str;
//  utf8Cat8(&zStr, 0);
//..
//  assert(true == bdede_Utf8Util::isValid(zStr.data(), zStr.length()));
//
//  assert(10 == bdede_Utf8Util::numCharacters(zStr.data(), zStr.length()));
//..
// Encode an overlong value--0x61 ('a') encoded using 2 bytes, which is not
// allowed:
//..
//  bsl::string oStr = str;
//  utf8Cat16(&oStr, 'a');
//
//  assert(false == bdede_Utf8Util::isValid(oStr.data(), oStr.length()));
//  assert(false == bdede_Utf8Util::isValid(oStr.c_str()));
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

                        // =====================
                        // struct bdede_Utf8Util
                        // =====================

struct bdede_Utf8Util {
    // This struct provides a namespace for static methods used for validating
    // UTF-8 strings and for counting the number of UTF-8 characters in them.

    // CLASS METHODS
    static bool isValid(const char *string, int length);
        // Return 'true' if the specified 'string' having the specified
        // 'length' (in bytes) contains valid UTF-8, and 'false' otherwise.
        // 'string' need not be null-terminated and can contain embedded null
        // characters.  The behavior is undefined unless '0 <= length'.

    static bool isValid(const char *string);
        // Return 'true' if the specified 'string' contains valid UTF-8, and
        // 'false' otherwise.  Since 'string' is necessarily null-terminated,
        // it cannot contain embedded null characters.

    static int numCharacters(const char *string, int length);
        // Return the number of UTF-8 characters in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null characters.  The
        // behavior is undefined unless '0 <= length'.  Note that 'string' may
        // contain less than 'length' UTF-8 characters.

    static int numCharacters(const char *string);
        // Return the number of UTF-8 characters in the specified 'string'.
        // Since 'string' is necessarily null-terminated, it cannot contain
        // embedded null characters.  Note that 'string' may contain less than
        // 'length' UTF-8 characters.
};

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
