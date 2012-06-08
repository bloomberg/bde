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

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

                        // =============================
                        // struct bdede_CharConvertUtf16
                        // =============================

struct bdede_CharConvertUtf16 {
    // This 'struct' provides a namespace for a suite of pure procedures to
    // convert character buffers between UTF-8 and UTF-16.  Note that all
    // routines in this component will always null-terminate the output strings
    // unless there is a 'dstCapacity' parameter which is passed 0, or there is
    // an 'alwaysTerminate' parameter which is passed 'false'.  If
    // 'alwaysTerminate' is passed 'false', the output will still be zero
    // terminated if there is room for a complete translation of the input and
    // the terminating 0.  Note that since all UTF-16 operations take place as
    // 'unsigned short' values, byte order is not taken into consideration, and
    // Byte Order Mark (BOM) characters are neither generated nor recognized as
    // special.  If a BOM is present in the input, it will be translated,
    // correct ('0xfeff') or incorrect ('0xfffe'), into the output without any
    // special handling.

    enum Utf16Flags {
        BDEDE_UTF16_INVALID_CHARS_FLAG = 0x1,    // one or more invalid chars
                                                 // encountered

        BDEDE_UTF16_OUT_OF_SPACE_FLAG  = 0x2,    // the available output space
                                                 // was exhausted before
                                                 // conversion was complete
    };

    // CLASS METHODS
    static int utf8ToUtf16(
                         bsl::vector<unsigned short>  *dstVector,
                         const char                   *srcString,
                         bsl::size_t                  *numCharsWritten = 0,
                         unsigned short                errorCharacter  = '?');
    static int utf8ToUtf16(
                         bsl::wstring                 *dstWstring,
                         const char                   *srcString,
                         bsl::size_t                  *numCharsWritten = 0,
                         unsigned short                errorCharacter  = '?');
    static int utf8ToUtf16(
                         unsigned short               *dstBuffer,
                         bsl::size_t                   dstCapacity,
                         const char                  **srcString,
                         bsl::size_t                  *numCharsWritten = 0,
                         bsl::size_t                  *numWordsWritten = 0,
                         unsigned short                errorCharacter  = '?',
                         bool                          alwaysTerminate = true);
    static int utf8ToUtf16(
                         unsigned short               *dstBuffer,
                         bsl::size_t                   dstCapacity,
                         const char                   *srcString,
                         bsl::size_t                  *numCharsWritten = 0,
                         bsl::size_t                  *numWordsWritten = 0,
                         unsigned short                errorCharacter  = '?');
        // Load into either the specified 'dstVector', 'dstWstring, or
        // 'dstBuffer' of capacity 'dstCapacity', the result of converting the
        // specified null-terminated UTF-8 'srcString' to its UTF-16
        // equivalent.
        //: o If the specified 'srcString' is a pointer-pointer, update
        //:   '*srcString' to point after the end of successfully parsed input
        //:   (including the terminating 0, if parsed).
        //: o Optionally specify 'numCharsWritten' which (if not 0) indicates
        //:   the location of the modifiable variable into which the number of
        //:   UTF-16 characters (including the terminating 0, if any) written,
        //:   including the null terminator, is to be loaded.  Note that one
        //:   *character* can occupy multiple 16-bit *words*.
        //: o If the output is not a 'vector' or 'wstring', optionally specify
        //:   'numWordsWritten' which (if not 0) indicates the location of the
        //:   modifiable variable into which the number of 'short' *memory
        //:   words* written (including the null terminator, if any) is to be
        //:   loaded.  If a 'vector' or 'wstring' is passed as destination, it
        //:   is grown as necessary and fitted exactly to the length of the
        //:   output (including the terminating 0) in which case,
        //:   'numWordsWritten' would be redundant with 'dstVector->size()' or
        //:   'dstWstring->length() + 1' and is not part of the interface.
        //: o Optionally specify 'errorCharacter' to be substituted (if not 0)
        //:   for invalid encodings in the input string.  (Invalid encodings
        //:   are multibyte encoding parts out of sequence, non-minimal UTF-8
        //:   encodings of characters, or characters outside the ranges which
        //:   UTF-16 can validly encode (1 to D7FF and DE00 to 10FFFF).)  If
        //:   'errorCharacter' is 0, invalid encodings are omitted entirely
        //:   from the output string.
        //: o Return 0 on success, and a bitwise-or of the flags defined by
        //:   enum 'Utf16Flags' if problem(s) occurred.
        //:   'BDEDE_UTF16_INVALID_CHARS_FLAG' will be set if one or more
        //:   invalid character sequences (see 'errorCharacter' above) were
        //:   encountered in the input, and 'BDEDE_UTF16_OUT_OF_SPACE_FLAG'
        //:   will be set if the output space was exhausted before conversion
        //:   was complete.  'BDEDE_UTF16_OUT_OF_SPACE_FLAG' is never set if
        //:   the output is a 'vector' or 'wstring'.
        //: o If 'dstCapacity > 0' yet 'dstCapacity' specifies a buffer too
        //:   small to hold the output, if the the specified 'alwaysTeriminate'
        //:   is 'true', the maximal null-terminated prefix of the properly
        //:   converted result string is loaded into 'dstBuffer', and if
        //:   'alwaysTerminate' is 'false', the maximal output sequence without
        //:   the terminating 0 is written.
        //: o The behavior is undefined unless 'dstBuffer', if specified,
        //:   refers to an array of at least 'dstCapacity' elements,
        //:   'srcString' is null-terminated, and 'errorCharacter' is either 0
        //:   or a valid single-word encoded UTF-16 character (in the range
        //:   '[ 1, 0xd7ff ]' or '[ 0xe000, 0xffff ]')
        //: o Note that if 'dstCapacity' is 0, '*dstBuffer' is not modified and
        //:   this function returns 'BDEDE_UTF16_OUT_OF_SPACE_FLAG' set and 0
        //:   is written into '*numCharsWritten' and '*numWordsWritten' (if
        //:   those pointers are not zero), since there is insufficient space
        //:   for even a null terminator alone.
        //: o Also note that since UTF-16 is a variable-length encoding,
        //:   '*numWordsWritten' or the final 'dstVector->size()' or
        //:   'dstWstring->length()' may be greater than '*numCharsWritten',
        //:   and therefore that an input 'srcString' of 'dstCapacity'
        //:   *characters* may not fit into 'dstBuffer'.  However, an input
        //:   'srcString' of 'dstCapacity' *bytes* (including terminating 0)
        //:   will always fit (since the UTF-8 encoding of a character requires
        //:   at least as many bytes as the UTF-16 encoding requires words).
        //: o Also note that if the output is a vector or wstring, any previous
        //:   contents of the destination are discarded.
        //: o Also note that if the output is a vector or wstring, the
        //:   efficiency is improved if enough room for the output is reserved
        //:   in the vector.  The amount of room needed will vary with the
        //:   contents of the data and the language being translated, but never
        //:   will the number of words output exceed the number of bytes input.

    static int utf16ToUtf8(bsl::string           *dstString,
                           const unsigned short  *srcString,
                           bsl::size_t           *numCharsWritten = 0,
                           char                   errorCharacter  = '?');
    static int utf16ToUtf8(bsl::vector<char>     *dstVector,
                           const unsigned short  *srcString,
                           bsl::size_t           *numCharsWritten = 0,
                           char                   errorCharacter  = '?');
    static int utf16ToUtf8(char                  *dstBuffer,
                           bsl::size_t            dstCapacity,
                           const unsigned short **srcString,
                           bsl::size_t           *numCharsWritten = 0,
                           bsl::size_t           *numBytesWritten = 0,
                           char                   errorCharacter  = '?',
                           bool                   alwaysTerminate = true);
    static int utf16ToUtf8(char                 *dstBuffer,
                           bsl::size_t           dstCapacity,
                           const unsigned short *srcString,
                           bsl::size_t          *numCharsWritten = 0,
                           bsl::size_t          *numBytesWritten = 0,
                           char                  errorCharacter  = '?');
        // Load, into the specified destination, of the form 'dstString',
        // 'dstVector', or 'dstBuffer' of the specified 'dstCapacity', the
        // result of converting the specified null-terminated UTF-16
        // '*srcString' to its UTF-8 equivalent.
        //: o If the specfied 'srcString' is a pointer-pointer, update
        //:   '*srcString' to point after the last word parsed, including
        //:   invalid chars and the terminating null.
        //: o Optionally specify 'numCharsWritten' which (if not 0) indicates
        //:   the location of the modifiable variable into which the number of
        //:   UTF-16 *characters* (including the terminating 0, if any)
        //:   written, including the null terminator, is to be loaded.  Note
        //:   that one *character* can occupy multiple *bytes*.
        //: o If the output is not a string or vector, optionally specify
        //:   'numBytesWritten' which (if not 0) indicates the location of the
        //:   modifiable variable into which the number of bytes written
        //:   (including the null terminator, if any) is to be loaded.  If a
        //:   vector is passed as destination, it is grown as necessary and
        //:   fitted exactly to the length of the output (including the
        //:   terminating 0).  In the case where the destination is a string,
        //:   likewise, the string is fitted to the output
        //:   ('dstString->length()' will *NOT* include the terminating 0).  In
        //:   the string and vector cases, 'numBytesWritten' would be redundant
        //:   with 'dstVector->size()' or 'dstString->length() + 1' and is not
        //:   part of the interface.
        //: o Optionally specify 'errorCharacter' to be substituted (if not 0)
        //:   for invalid encodings in the input string.  (Invalid encodings
        //:   are incomplete multi-word encodings or parts of a two-word
        //:   encoding out of their proper sequence.)  If 'errorCharacter' is
        //:   0, invalid encodings are omitted entirely from the output string.
        //: o Return 0 on success, and a bitwise-or of the flags defined by
        //:   enum 'Utf16Flags' if problem(s) occurred.
        //:   'BDEDE_UTF16_INVALID_CHARS_FLAG' will be set if one or more
        //:   invalid character sequences (see 'errorCharacter' above) were
        //:   encountered in the input, and 'BDEDE_UTF16_OUT_OF_SPACE_FLAG'
        //:   will be set if the output space was exhausted before conversion
        //:   was complete.  'BDEDE_UTF16_OUT_OF_SPACE_FLAG' is never set if
        //:   the output is a 'string' or 'vector'.
        //: o The behavior is undefined unless 'dstBuffer' refers to an array
        //:   of at least 'dstCapacity' elements, 'srcString' is
        //:   null-terminated, and 'errorCharacter' is either zero or a valid
        //:   single-byte encoded UTF-8 character
        //:   ('0 < errorCharacter < 0x80').
        //: o Note that if 'dstCapacity' is 0, this function returns
        //:   'BDEDE_UTF16_OUT_OF_SPACE_FLAG' set and 0 is written into
        //:   '*numCharsWritten' and '*numBytesWritten' (if those pointers are
        //:   not zero), since there is insufficient space for even a null
        //:   terminator alone.
        //: o Also note that since UTF-8 is a variable-length encoding,
        //:   'numBytesWritten' may be up to four times 'numCharsWritten', and
        //:   therefore that an input 'srcString' of 'dstCapacity' *characters*
        //:   (including terminating 0) may not fit into 'dstBuffer'.  A
        //:   one-word (two-byte) UTF-16 character will require one to three
        //:   UTF-8 octets (bytes); a two-word (four-byte) UTF-16 character
        //:   will always require four UTF-8 octets.
        //: o Also note that if the destination is a 'string' or 'vector', any
        //:   previous contents of the destination are discarded.
        //: o Also note that if the output is a string or vector, the
        //:   efficiency is improved if enough room for the output is reserved
        //:   in the vector.  The amount of room needed will vary with the
        //:   contents of the data and the language being translated, but never
        //:   will the number of *bytes* output exceed three times the number
        //:   of *short words* input.
};

//=============================================================================
//                         INLINE FUNCTION DEFINITIONS
//=============================================================================

// CLASS METHODS
inline
int bdede_CharConvertUtf16::utf8ToUtf16(unsigned short *dstBuffer,
                                        bsl::size_t     dstCapacity,
                                        const char     *srcString,
                                        bsl::size_t    *numCharsWritten,
                                        bsl::size_t    *numWordsWritten,
                                        unsigned short  errorCharacter)
{
    return utf8ToUtf16(dstBuffer,
                       dstCapacity,
                       &srcString,
                       numCharsWritten,
                       numWordsWritten,
                       errorCharacter,
                       true);
}

inline
int bdede_CharConvertUtf16::utf16ToUtf8(char                 *dstBuffer,
                                        bsl::size_t           dstCapacity,
                                        const unsigned short *srcString,
                                        bsl::size_t          *numCharsWritten,
                                        bsl::size_t          *numBytesWritten,
                                        char                  errorCharacter)
{
    return utf16ToUtf8(dstBuffer,
                       dstCapacity,
                       &srcString,
                       numCharsWritten,
                       numBytesWritten,
                       errorCharacter,
                       true);
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
