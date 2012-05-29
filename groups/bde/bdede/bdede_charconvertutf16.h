// bdede_charconvertutf16.h                  -*-C++-*-
#ifndef INCLUDED_BDEDE_CHARCONVERTUTF16
#define INCLUDED_BDEDE_CHARCONVERTUTF16

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide fast, safe conversion between UTF-8 and UTF-16 encodings.
//
//@CLASSES:
//  bdede_CharConvertUtf16: namespace for conversions between UTF-8 and UTF-16
//
//@AUTHOR: Mark Terribile (mterribile)
//
//@DESCRIPTION: This component provides a suite of pure procedures supporting
// the *fast* conversion of *valid* UTF-8 encoded "C" strings to *valid* UTF-16
// 16-bit character arrays, and vice versa.  Invalid byte sequences and
// characters forbidden by either encoding are removed and (optionally)
// replaced by a character provided by the caller.  All strings written are
// null-terminated unless the output buffer is zero-length.  The byte or
// word count and character count that are optionally returned through pointer
// arguments include the terminating null in their count.
//
///History and Motivation
///----------------------
// UTF-8 is a character encoding that allows 32-bit character sets like Unicode
// to be represented using null-terminated (8-bit) byte strings (NTBS), while
// allowing "standard ASCII" strings to be used "as-is".  Note that UTF-8 is
// described in detail in RFC 3629 (http://www.ietf.org/rfc/rfc3629.txt).
//
// UTF-16 is a 16-bit character encoding that allows Unicode characters up to
// 0x10FFFF to be encoded using one or two 16-bit values.  Note that UTF-16 is
// described in detail in RFC 2781 (http://www.ietf.org/rfc/rfc2781.txt).
//
// The 'glib' conversion routines are *much* slower than the functions
// implemented here because the 'glib' functions first compute the number of
// output characters required, allocate the memory for them, and then perform
// the conversion while validating the input characters.  The methods in
// 'bdede_CharConvertUtf16', on the other hand, assume that the user-provided
// output buffer is wide enough, make a "best effort" to convert into it, and
// return an error code if not enough space was provided.
//
// Non-minimal UTF-8 encodings of characters are reported as errors.  Octets
// and post-conversion characters in the forbidden ranges are treated as errors
// and removed (or replaced, if a replacement character is provided).
//
///Usage
///-----
// The following snippets of code illustrate a typical use of the
// 'bdede_CharConvertUtf16' struct's utility functions, first converting from
// UTF-8 to UTF-16, and then converting back to make sure the round trip
// returns the same value.
//..
//  void testFunction1()
//  {
//      unsigned short buffer[256];  // arbitrary "wide-enough" size
//      bsl::size_t    buffSize = sizeof buffer / sizeof *buffer;
//      bsl::size_t    charsWritten;
//
//      int retVal = bdede_CharConvertUtf16::utf8ToUtf16(buffer,
//                                                       buffSize,
//                                                       "Hello",
//                                                       &charsWritten);
//
//      assert( 0  == retVal);
//      assert('H' == buffer[0]);
//      assert('e' == buffer[1]);
//      assert('l' == buffer[2]);
//      assert('l' == buffer[3]);
//      assert('o' == buffer[4]);
//      assert( 0  == buffer[5]);
//      assert( 6  == charsWritten);
//
//      // "&Eacute;cole", the French word for School
//      retVal = bdede_CharConvertUtf16::utf8ToUtf16(buffer,
//                                                   buffSize,
//                                                   "\xc3\x89" "cole",
//                                                   &charsWritten);
//
//      assert( 0   == retVal);
//      assert(0xc9 == buffer[0]);// Unicode-E WITH ACUTE, LATIN CAPITAL LETTER
//      assert('c'  == buffer[1]);
//      assert('o'  == buffer[2]);
//      assert('l'  == buffer[3]);
//      assert('e'  == buffer[4]);
//      assert( 0   == buffer[5]);
//      assert( 6   == charsWritten);
//
//      unsigned short buffer2[256];  // arbitrary "wide-enough" size
//      bsl::size_t    buffer2Size  = sizeof buffer2 / sizeof *buffer2;
//      bsl::size_t    bytesWritten = 0;
//
//      // Reversing the conversion returns the original string:
//      retVal = bdede_CharConvertUtf16::utf16ToUtf8(buffer2,
//                                                   buffer2Size,
//                                                   buffer,
//                                                   &charsWritten,
//                                                   &bytesWritten);
//
//      assert(0 == retVal);
//      assert(0 == bsl::strcmp(buffer2, "\xc3\x89" "cole"));
//
//      // 6 characters written, but 7 bytes, since the first character takes 2
//      // octets.
//
//      assert(6 == charsWritten);
//      assert(7 == bytesWritten);
//  }
//..
// In this example, a UTF-8 input string is converted then passed to another
// function, which expects a UTF-16 buffer.
//
// First, we define a utility *strlen* replacement for UTF-16:
//..
//  int wideStrlen(const unsigned short *str)
//  {
//      int len = 0;
//
//      while (*str) {
//          // Check for a 2-word character.
//          unsigned short maskedUpperByte = *str & 0xFF00;
//          if (0xD800 <= maskedUpperByte && maskedUpperByte <= 0xDBFF) {
//             str += 2;
//          }
//          else {
//             ++str;
//          }
//
//          ++len;
//      }
//
//      return len;
//  }
//..
// Now, some arbitrary function that calls 'wideStrlen':
//..
//  void functionRequiringUtf16(const unsigned short *str, bsl::size_t strLen)
//  {
//      // Would probably do something more reasonable here.
//
//      assert(wideStrlen(str) + 1 == strLen);
//  }
//..
// Finally, we can take some UTF-8 as an input and call
// 'functionRequiringUtf16':
//..
//  void processUtf8(const char *strU8)
//  {
//      unsigned short buffer[1024];  // some "large enough" size
//      bsl::size_t    buffSize     = sizeof buffer / sizeof *buffer;
//      bsl::size_t    charsWritten = 0;
//
//      int result = bdede_CharConvertUtf16::utf8ToUtf16(buffer,
//                                                       buffSize,
//                                                       strU8,
//                                                       &charsWritten);
//
//      if (0 == result) {
//          functionRequiringUtf16(buffer, charsWritten);
//      }
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>            // 'bsl::size_t'
#endif

namespace BloombergLP {

                        // =============================
                        // struct bdede_CharConvertUtf16
                        // =============================

struct bdede_CharConvertUtf16 {
    // This 'struct' provides a namespace for a suite of pure procedures to
    // convert character buffers between UTF-8 and UTF-16.  Note that all
    // routines in this component will null-terminate the output strings as
    // long as the return buffer size is positive (i.e., 'dstCapacity > 0').
    // Note that since all UTF-16 operations take place as 'unsigned short'
    // values, byte order is not taken into consideration, and Byte Order Mark
    // (BOM) characters are not generated.  If a BOM is present in the input,
    // it will be translated into the output.

    // CLASS METHODS
    static int utf8ToUtf16(unsigned short *dstBuffer,
                           bsl::size_t     dstCapacity,
                           const char     *srcString,
                           bsl::size_t    *numCharsWritten = 0,
                           bsl::size_t    *numWordsWritten = 0,
                           unsigned short  errorCharacter  = '?');
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-8
        // 'srcString' to its UTF-16 equivalent.  Optionally specify
        // 'numCharsWritten' which (if not 0) indicates the modifiable integer
        // into which the number of UTF-16 characters written (including the
        // null terminator) is to be loaded.  Optionally specify
        // 'numWordsWritten' which (if not 0) indicates the modifiable integer
        // into which the number of 'short' *memory words* written (including
        // the null terminator) is to be loaded.  Optionally specify
        // 'errorCharacter' to be substituted (if not 0) for invalid encodings
        // in the input string.  (Invalid encodings are multibyte encoding
        // parts out of sequence, non-minimal UTF-8 encodings of characters, or
        // characters outside the ranges which UTF-16 can validly encode (1 to
        // D7FF and DE00 to 10FFFF).)  If 'errorCharacter' is 0, invalid
        // encodings are omitted entirely from the output string.  Return 0 on
        // success, 1 on invalid input, 2 if 'dstCapacity' is insufficient to
        // hold the complete conversion, and 3 if both types of error occur.  A
        // value of 1 indicates that one or more input characters are invalid
        // (in which case the conversion continues).  A value of 2 indicates
        // that the length of the converted 'srcString' (including the null
        // terminator) is greater than 'dstCapacity', in which case the maximal
        // null-terminated prefix of the properly converted result string is
        // loaded into 'dstBuffer', and (unless null) '*numCharsWritten' is set
        // to the number of UTF-16 characters written, and (unless null)
        // '*numWordsWritten' is set to the number of 'short' memory words
        // written (at most 'dstCapacity').  The behavior is undefined unless
        // '0 <= dstCapacity', 'dstBuffer' refers to an array of at least
        // 'dstCapacity' elements, 'srcString' is null-terminated, and
        // 'errorCharacter' is either zero or a valid single-word encoded
        // UTF-16 character ('0 < errorCharacter < 0x10000').  Note that if
        // 'dstCapacity' is 0, this function returns 2 and 0 is written into
        // '*numCharsWritten' and '*numWordsWritten' (if those pointers are not
        // zero), since there is insufficient space for even a null terminator
        // alone.  Also note that since UTF-16 is a variable-length encoding,
        // 'numWordsWritten' may be greater than 'numCharsWritten', and
        // therefore that an input 'srcString' of 'dstCapacity - 1'
        // *characters* may not fit into 'dstBuffer'.  However, an input
        // 'srcString' of 'dstCapacity - 1' *bytes* will always fit (since the
        // UTF-8 encoding of a character requires at least as many bytes as the
        // UTF-16 encoding requires words).

    static int utf8ToUtf16Partial(unsigned short  *dstBuffer,
                                  bsl::size_t      dstCapacity,
                                  const char     **srcString,
                                  bsl::size_t     *numCharsWritten = 0,
                                  bsl::size_t     *numWordsWritten = 0,
                                  unsigned short   errorCharacter  = '?');
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-8
        // '*srcString' to its UTF-16 equivalent, and update '*srcString' to
        // point after the characters parsed, including invalid chars and the
        // termiinating zero.  Optionally specify 'numCharsWritten' which (if
        // not 0) indicates the modifiable integer into which the number of
        // UTF-16 characters written (including the null terminator) is to be
        // loaded.  Optionally specify 'numWordsWritten' which (if not 0)
        // indicates the modifiable integer into which the number of 'short'
        // *memory words* written (including the null terminator) is to be
        // loaded.  Optionally specify 'errorCharacter' to be substituted (if
        // not 0) for invalid encodings in the input string.  (Invalid
        // encodings are multibyte encoding parts out of sequence, non-minimal
        // UTF-8 encodings of characters, or characters outside the ranges
        // which UTF-16 can validly encode (1 to D7FF and DE00 to 10FFFF).)  If
        // 'errorCharacter' is 0, invalid encodings are omitted entirely from
        // the output string.  Return 0 on success, 1 on invalid input, 2 if
        // 'dstCapacity' is insufficient to hold the complete conversion, and 3
        // if both types of error occur.  A value of 1 indicates that one or
        // more input characters are invalid (in which case the conversion
        // continues).  A value of 2 indicates that the length of the converted
        // 'srcString' (including the null terminator) is greater than
        // 'dstCapacity', in which case the maximal null-terminated prefix of
        // the properly converted result string is loaded into 'dstBuffer', and
        // (unless null) '*numCharsWritten' is set to the number of UTF-16
        // characters written, and (unless null) '*numWordsWritten' is set to
        // the number of 'short' memory words written (at most 'dstCapacity').
        // The behavior is undefined unless '0 <= dstCapacity', 'dstBuffer'
        // refers to an array of at least 'dstCapacity' elements, 'srcString'
        // is null-terminated, and 'errorCharacter' is either zero or a valid
        // single-word encoded UTF-16 character
        // ('0 < errorCharacter < 0x10000').  Note that if 'dstCapacity' is 0,
        // this function returns 2 and 0 is written into '*numCharsWritten' and
        // '*numWordsWritten' (if those pointers are not zero), since there is
        // insufficient space for even a null terminator alone.  Also note that
        // since UTF-16 is a variable-length encoding, 'numWordsWritten' may be
        // greater than 'numCharsWritten', and therefore that an input
        // 'srcString' of 'dstCapacity - 1' *characters* may not fit into
        // 'dstBuffer'.  However, an input 'srcString' of 'dstCapacity - 1'
        // *bytes* will always fit (since the UTF-8 encoding of a character
        // requires at least as many bytes as the UTF-16 encoding requires
        // words).

    static int utf16ToUtf8(char                 *dstBuffer,
                           bsl::size_t           dstCapacity,
                           const unsigned short *srcString,
                           bsl::size_t          *numCharsWritten = 0,
                           bsl::size_t          *numBytesWritten = 0,
                           char                  errorCharacter  = '?');
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-16
        // 'srcString' to its UTF-8 equivalent.  Optionally specify
        // 'numCharsWritten' which (if not 0) indicates the modifiable integer
        // into which the number of UTF-8 characters written (including the
        // null terminator) is to be loaded.  Optionally specify
        // 'numBytesWritten' which (if not 0) indicates the modifiable integer
        // into which the number of *bytes* written (including the null
        // terminator) is to be loaded.  Optionally specify 'errorCharacter' to
        // be substituted (if not 0) for invalid encodings in the input string.
        // (Invalid encodings are parts of a two-word encoding out of their
        // proper sequence.)  If 'errorCharacter' is zero, invalid encodings
        // are omitted entirely from the output string.  Return 0 on success, 1
        // on invalid input, 2 if 'dstCapacity' is insufficient to hold the
        // complete conversion, and 3 if both types of error occur.  A value of
        // 1 indicates that one or more input characters are invalid (in which
        // case the conversion continues).  A value of 2 indicates that the
        // length of the converted 'srcString' (including the null terminator)
        // is greater than 'dstCapacity', in which case the maximal
        // null-terminated prefix of the properly converted result string is
        // loaded into 'dstBuffer', and (unless null) '*numCharsWritten' is set
        // to the number of UTF-8 characters written, and (unless null)
        // '*numBytesWritten' is set to the number of bytes written (at most
        // 'dstCapacity').  The behavior is undefined unless
        // '0 <= dstCapacity', 'dstBuffer' refers to an array of at least
        // 'dstCapacity' elements, 'srcString' is null-terminated, and
        // 'errorCharacter' is either zero or a valid single-byte encoded UTF-8
        // character ('0 < errorCharacter < 0x80').  Note that if 'dstCapacity'
        // is 0, this function returns 2 and 0 is written into
        // '*numCharsWritten' and '*numBytesWritten' (if those pointers are not
        // zero), since there is insufficient space for even a null terminator
        // alone.  Also note that since UTF-8 is a variable-length encoding,
        // 'numBytesWritten' may be up to four times 'numCharsWritten', and
        // therefore that an input 'srcString' of 'dstCapacity - 1'
        // *characters* may not fit into 'dstBuffer'.  A one-word (two-byte)
        // UTF-16 character will require one to three UTF-8 octets (bytes); a
        // two-word (four-byte) UTF-16 character will require four UTF-8
        // octets.

    static int utf16ToUtf8Partial(
                                 char                  *dstBuffer,
                                 bsl::size_t            dstCapacity,
                                 const unsigned short **srcStringArg,
                                 bsl::size_t           *numCharsWritten = 0,
                                 bsl::size_t           *numBytesWritten = 0,
                                 char                   errorCharacter  = '?');
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-16
        // '*srcString' to its UTF-8 equivalent, and update '*srcString' to
        // point after the last word parsed, including invalid chars and the
        // terminating null.  Optionally specify 'numCharsWritten' which (if
        // not 0) indicates the modifiable integer into which the number of
        // UTF-8 characters written (including the null terminator) is to be
        // loaded.  Optionally specify 'numBytesWritten' which (if not 0)
        // indicates the modifiable integer into which the number of *bytes*
        // written (including the null terminator) is to be loaded.  Optionally
        // specify 'errorCharacter' to be substituted (if not 0) for invalid
        // encodings in the input string.  (Invalid encodings are parts of a
        // two-word encoding out of their proper sequence.)  If
        // 'errorCharacter' is zero, invalid encodings are omitted entirely
        // from the output string.  Return 0 on success, 1 on invalid input, 2
        // if 'dstCapacity' is insufficient to hold the complete conversion,
        // and 3 if both types of error occur.  A value of 1 indicates that one
        // or more input characters are invalid (in which case the conversion
        // continues).  A value of 2 indicates that the length of the converted
        // 'srcString' (including the null terminator) is greater than
        // 'dstCapacity', in which case the maximal null-terminated prefix of
        // the properly converted result string is loaded into 'dstBuffer', and
        // (unless null) '*numCharsWritten' is set to the number of UTF-8
        // characters written, and (unless null) '*numBytesWritten' is set to
        // the number of bytes written (at most 'dstCapacity').  The behavior
        // is undefined unless '0 <= dstCapacity', 'dstBuffer' refers to an
        // array of at least 'dstCapacity' elements, 'srcString' is
        // null-terminated, and 'errorCharacter' is either zero or a valid
        // single-byte encoded UTF-8 character ('0 < errorCharacter < 0x80').
        // Note that if 'dstCapacity' is 0, this function returns 2 and 0 is
        // written into '*numCharsWritten' and '*numBytesWritten' (if those
        // pointers are not zero), since there is insufficient space for even a
        // null terminator alone.  Also note that since UTF-8 is a
        // variable-length encoding, 'numBytesWritten' may be up to four times
        // 'numCharsWritten', and therefore that an input 'srcString' of
        // 'dstCapacity - 1' *characters* may not fit into 'dstBuffer'.  A
        // one-word (two-byte) UTF-16 character will require one to three UTF-8
        // octets (bytes); a two-word (four-byte) UTF-16 character will require
        // four UTF-8 octets.
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
