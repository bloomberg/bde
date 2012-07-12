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
// replaced by a character provided by the caller.  The byte or
// word count and character count that are optionally returned through pointer
// arguments include the terminating null, if any, in their count.
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
// The 'glib' conversion routines are slower than the functions implemented
// here because the 'glib' functions first compute the exact number of output
// characters required, allocate the memory for them, and then perform the
// conversion while validating the input characters.  The methods in
// 'bdede_CharConvertUtf16', on the other hand, are potentially faster if given
// a user-provided output buffer that is wide enough, making only a single pass
// through the data.
//
// The methods that output to a 'vector', 'string', or 'wstring' will all grow
// the output object as necessary to fit the data, and in the end will exactly
// resize the object to the finished output, and still make only a single
// translation pass over the data.  The methods will run faster, however, if
// passed an output object that is already big enough.
//
// Non-minimal UTF-8 encodings of characters are reported as errors.  Octets
// and post-conversion characters in the forbidden ranges are treated as errors
// and removed (or replaced, if a replacement character is provided).
//
// All output is null terminated unless a value of 'false' is passed to the
// optional 'alwaysTerminate' argument.  Most methods do not have an
// 'alwaysTerminate' argument, in which case the result is always null
// terminated.  Note that the null is implicit in the 'string' and 'wstring'
// cases.
//
///Usage
///-----
// The following snippets of code illustrate a typical use of the
// 'bdede_CharConvertUtf16' struct's utility functions, first converting from
// UTF-8 to UTF-16, and then converting back to make sure the round trip
// returns the same value.
//
// First, we declare a string of utf8 containing single-, double-, triple-, and
// quadruple-octet characters.
//..
//  const char utf8MultiLang[] = {
//      "Hello"                                         // -- Ascii
//      "\xce\x97"         "\xce\x95"       "\xce\xbb"  // -- Greek
//      "\xe4\xb8\xad"     "\xe5\x8d\x8e"               // -- Chinese
//      "\xe0\xa4\xad"     "\xe0\xa4\xbe"               // -- Hindi
//      "\xf2\x94\xb4\xa5" "\xf3\xb8\xac\x83" };        // -- Quad octets
//..
// Then, we declare an enum summarizing the counts of characters in the string
// and verify that the counts add up to the length of the string.
//..
//  enum { NUM_ASCII_CHARS   = 5,
//         NUM_GREEK_CHARS   = 3,
//         NUM_CHINESE_CHARS = 2,
//         NUM_HINDI_CHARS   = 2,
//         NUM_QUAD_CHARS    = 2 };
//
//  ASSERT(1 * NUM_ASCII_CHARS +
//         2 * NUM_GREEK_CHARS +
//         3 * NUM_CHINESE_CHARS +
//         3 * NUM_HINDI_CHARS +
//         4 * NUM_QUAD_CHARS == bsl::strlen(utf8MultiLang));
//..
// Next, we declare the vector where our utf16 output will go, and a variable
// into which the number of characters (characters, not bytes or words) written
// will be stored.  It is not necessary to initialize 'utf16CharsWritten'.
//..
//  bsl::vector<unsigned short> v16;
//  bsl::size_t utf16CharsWritten;
//..
// Note that for performance, we should 'v16.reserve(sizeof(utf8MultiLang))',
// but it's not strictly necessary -- it will automatically be grown to the
// correct size.  Note also that if 'v16' were not empty, that wouldn't be a
// problem -- any contents will be discarded.
//
// Then, we do the translation to 'utf16'.
//..
//  int retVal = bdede_CharConvertUtf16::utf8ToUtf16(&v16,
//                                                   utf8MultiLang,
//                                                   &utf16CharsWritten);
//
//  ASSERT(0 == retVal);        // verify success
//  ASSERT(0 == v16.back());    // verify null terminated
//..
// Next, we verify that the number of characters (characters, not bytes or
// words) that was returned is correct.
//..
//  enum { EXPECTED_CHARS_WRITTEN = 
//                  NUM_ASCII_CHARS + NUM_GREEK_CHARS + NUM_CHINESE_CHARS +
//                  NUM_HINDI_CHARS + NUM_QUAD_CHARS  + 1 };
//  ASSERT(EXPECTED_CHARS_WRITTEN == utf16CharsWritten);
//..
// Then, we verify that the number of 16-bit words written was correct.  The
// quad octet chars each require 2 short words of output
//..
//  enum { EXPECTED_UTF16_WORDS_WRITTEN =
//                  NUM_ASCII_CHARS + NUM_GREEK_CHARS + NUM_CHINESE_CHARS +
//                  NUM_HINDI_CHARS + NUM_QUAD_CHARS * 2 + 1 };
//
//  ASSERT(EXPECTED_UTF16_WORDS_WRITTEN == v16.size());
//..
// Next, we calculate and confirm the difference betwen the number of utf16
// words output and the number of bytes input.  The ascii chars will take 1
// 16-bit word apiece, the Greek chars are double octets that will become
// single shorts, the Chinese chars are encoded as utf8 triple octets that will
// turn into single 16-bit words, the same for the Hindi chars, and the quad
// chars are quadruple octets that will turn into double shorts.
//..
//  enum { SHRINKAGE =
//                    NUM_ASCII_CHARS   * (1-1) + NUM_GREEK_CHARS * (2-1) +
//                    NUM_CHINESE_CHARS * (3-1) + NUM_HINDI_CHARS * (3-1) +
//                    NUM_QUAD_CHARS    * (4-2) };
//
//  ASSERT(v16.size() == sizeof(utf8MultiLang) - SHRINKAGE);
//..
// Then, we go on to do the reverse 'utf16ToUtf8' transform to turn it back
// into utf8, and we should get a result identical to our original input.
// Declare a 'bsl::string' for our output, and a variable to count the number
// of characters (characters, not bytes or words) translated.
//..
//  bsl::string    s;
//  bsl::size_t    utf8CharsWritten;
//..
// Again, note that for performance, we should ideally
// 's.reserve(3 * v16.size())' but it's not really necessary.
//
// Now, we do the reverse transform:
//..
//  retVal = bdede_CharConvertUtf16::utf16ToUtf8(&s,
//                                               v16.begin(),
//                                               &utf8CharsWritten);
//..
// Finally, we verify a successful status was returned, that the output of the
// reverse transform was identical to the original input, and that the number
// of chars translated was as expected.
//..
//  ASSERT(0 == retVal);
//  ASSERT(utf8MultiLang == s);
//  ASSERT(s.length() + 1         == sizeof(utf8MultiLang));
//
//  ASSERT(EXPECTED_CHARS_WRITTEN == utf8CharsWritten);
//  ASSERT(utf16CharsWritten      == utf8CharsWritten);
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
        // Return values of all methods are a bitwise or of these two flags.
        // Note that when the output is a vector or string,
        // 'BDEDE_UTF16_OUT_OF_SPACE_FLAG' is never returned set.

        BDEDE_UTF16_INVALID_CHARS_FLAG = 0x1,    // one or more invalid chars
                                                 // encountered

        BDEDE_UTF16_OUT_OF_SPACE_FLAG  = 0x2     // the available output space
                                                 // was exhausted before
                                                 // conversion was complete
    };

                        // -- Utf8 to Utf16 Methods

    // CLASS METHODS
    static int utf8ToUtf16(bsl::wstring *dstWstring,
                           const char   *srcBuffer,
                           bsl::size_t  *numCharsWritten = 0,
                           wchar_t       errorCharacter  = '?');
        // Load into the specified 'dstWstring' the result of converting the
        // specified null-terminated UTF-8 'srcBuffer' to its UTF-16
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of UTF-16 characters (including the terminating 0) written is
        // to be loaded.  Note that one *character* can occupy multiple 16-bit
        // *words*.  Optionally specify 'errorCharacter' to be substituted (if
        // not 0) for invalid encodings in the input string.  (Invalid
        // encodings are multibyte encoding parts out of sequence, non-minimal
        // UTF-8 encodings of characters, or characters outside the ranges
        // which UTF-16 can validly encode (1 to D7FF and DE00 to 10FFFF).)  If
        // 'errorCharacter' is 0, invalid encodings are omitted entirely from

    static int utf8ToUtf16(bsl::vector<unsigned short> *dstVector,
                           const char                  *srcBuffer,
                           bsl::size_t                 *numCharsWritten = 0,
                           unsigned short               errorCharacter  = '?');
        // Load into the specified 'dstVector' the result of converting the
        // specified null-terminated UTF-8 'srcBuffer' to its UTF-16
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of UTF-16 characters (including the terminating 0) written is
        // to be loaded.  Note that one *character* can occupy multiple 16-bit
        // *words*.  Optionally specify 'errorCharacter' to be substituted (if
        // not 0) for invalid encodings in the input string.  (Invalid
        // encodings are multibyte encoding parts out of sequence, non-minimal
        // UTF-8 encodings of characters, or characters outside the ranges
        // which UTF-16 can validly encode (1 to D7FF and DE00 to 10FFFF).)  If
        // 'errorCharacter' is 0, invalid encodings are omitted entirely from
        // the output string.  Return 0 on success, and
        // 'BDEDE_UTF16_INVALID_CHARS_FLAG' if one or more invalid character
        // sequences (see 'errorCharacter' above) were encountered in the
        // input.  The behavior is undefined unless 'srcBuffer' is
        // null-terminated and 'errorCharacter' is either 0 or a valid
        // single-word encoded UTF-16 character (in the range '[ 1, 0xd7ff ]'
        // or '[ 0xe000, 0xffff ]').  Note that any previous contents of the
        // destination are discarded.  Also note that the result vector is
        // always fitted to exactly fit the 0 terminated result.

    static int utf8ToUtf16(unsigned short *dstBuffer,
                           bsl::size_t     dstCapacity,
                           const char     *srcBuffer,
                           bsl::size_t    *numCharsWritten = 0,
                           bsl::size_t    *numWordsWritten = 0,
                           unsigned short  errorCharacter  = '?');
        // Load into 'dstBuffer' of capacity 'dstCapacity', the result of
        // converting the specified null-terminated UTF-8 'srcBuffer' to its
        // UTF-16 equivalent.  Optionally specify 'numCharsWritten' which (if
        // not 0) indicates the location of the variable into which the number
        // of UTF-16 characters (including the terminating 0) written is to be
        // loaded.  Note that one *character* can occupy multiple 16-bit
        // *words*.  Optionally specify 'numWordsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of 'short' *memory words* written (including the null
        // terminator) is to be loaded.  Optionally specify 'errorCharacter' to
        // be substituted (if not 0) for invalid encodings in the input string.
        // (Invalid encodings are multibyte encoding parts out of sequence,
        // non-minimal UTF-8 encodings of characters, or characters outside the
        // ranges which UTF-16 can validly encode (1 to D7FF and DE00 to
        // 10FFFF).)  If 'errorCharacter' is 0, invalid encodings are omitted
        // entirely from the output string.  Return 0 on success, and a
        // bitwise-or of the flags defined by enum 'Utf16Flags' if problem(s)
        // occurred.  'BDEDE_UTF16_INVALID_CHARS_FLAG' will be set if one or
        // more invalid character sequences (see 'errorCharacter' above) were
        // encountered in the input, and 'BDEDE_UTF16_OUT_OF_SPACE_FLAG' will
        // be set if the output space was exhausted before conversion was
        // complete.  If 'dstCapacity > 0' yet 'dstCapacity' specifies a buffer
        // too small to hold the output, the maximal null-terminated prefix of
        // the properly converted result string is loaded into 'dstBuffer'.
        // The behavior is undefined unless 'dstBuffer', if specified, refers
        // to an array of at least 'dstCapacity' elements, 'srcBuffer' is
        // null-terminated, and 'errorCharacter' is either 0 or a valid
        // single-word encoded UTF-16 character (in the range '[ 1, 0xd7ff ]'
        // or '[ 0xe000, 0xffff ]') Note that if 'dstCapacity' is 0,
        // '*dstBuffer' is not modified and this function returns
        // 'BDEDE_UTF16_OUT_OF_SPACE_FLAG' set and 0 is written into
        // '*numCharsWritten' and '*numWordsWritten' (if those pointers are not
        // zero), since there is insufficient space for even a null terminator
        // alone.  Also note that since UTF-16 is a variable-length encoding,
        // '*numWordsWritten' may be greater than '*numCharsWritten', and
        // therefore that an input 'srcBuffer' of 'dstCapacity' *characters*
        // may not fit into 'dstBuffer'.  However, an input 'srcBuffer' of
        // 'dstCapacity' *bytes* (including terminating 0) will always fit
        // (since the UTF-8 encoding of a character requires at least as many
        // bytes as the UTF-16 encoding requires words).

    static int utf8ToUtf16(wchar_t     *dstBuffer,
                           bsl::size_t  dstCapacity,
                           const char  *srcBuffer,
                           bsl::size_t *numCharsWritten = 0,
                           bsl::size_t *numWordsWritten = 0,
                           wchar_t      errorCharacter  = '?');
        // Load into 'dstBuffer' of capacity 'dstCapacity', the result of
        // converting the specified null-terminated UTF-8 'srcBuffer' to its
        // UTF-16 equivalent.  Optionally specify 'numCharsWritten' which (if
        // not 0) indicates the location of the variable into which the number
        // of UTF-16 characters (including the terminating 0) written is to be
        // loaded.  Note that one *character* can occupy multiple 16-bit
        // *words*.  Optionally specify 'numWordsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of 'short' *memory words* written (including the null
        // terminator) is to be loaded.  Optionally specify 'errorCharacter' to
        // be substituted (if not 0) for invalid encodings in the input string.
        // (Invalid encodings are multibyte encoding parts out of sequence,
        // non-minimal UTF-8 encodings of characters, or characters outside the
        // ranges which UTF-16 can validly encode (1 to D7FF and DE00 to
        // 10FFFF).)  If 'errorCharacter' is 0, invalid encodings are omitted
        // entirely from the output string.  Return 0 on success, and a
        // bitwise-or of the flags defined by enum 'Utf16Flags' if problem(s)
        // occurred.  'BDEDE_UTF16_INVALID_CHARS_FLAG' will be set if one or
        // more invalid character sequences (see 'errorCharacter' above) were
        // encountered in the input, and 'BDEDE_UTF16_OUT_OF_SPACE_FLAG' will
        // be set if the output space was exhausted before conversion was
        // complete.  If 'dstCapacity > 0' yet 'dstCapacity' specifies a buffer
        // too small to hold the output, the maximal null-terminated prefix of
        // the properly converted result string is loaded into 'dstBuffer'.
        // The behavior is undefined unless 'dstBuffer', if specified, refers
        // to an array of at least 'dstCapacity' elements, 'srcBuffer' is
        // null-terminated, and 'errorCharacter' is either 0 or a valid
        // single-word encoded UTF-16 character (in the range '[ 1, 0xd7ff ]'
        // or '[ 0xe000, 0xffff ]') Note that if 'dstCapacity' is 0,
        // '*dstBuffer' is not modified and this function returns
        // 'BDEDE_UTF16_OUT_OF_SPACE_FLAG' set and 0 is written into
        // '*numCharsWritten' and '*numWordsWritten' (if those pointers are not
        // zero), since there is insufficient space for even a null terminator
        // alone.  Also note that since UTF-16 is a variable-length encoding,
        // '*numWordsWritten' may be greater than '*numCharsWritten', and
        // therefore that an input 'srcBuffer' of 'dstCapacity' *characters*
        // may not fit into 'dstBuffer'.  However, an input 'srcBuffer' of
        // 'dstCapacity' *bytes* (including terminating 0) will always fit
        // (since the UTF-8 encoding of a character requires at least as many
        // bytes as the UTF-16 encoding requires words).

                        // -- Utf16 to Utf8 Methods

    static int utf16ToUtf8(bsl::string          *dstString,
                           const unsigned short *srcBuffer,
                           bsl::size_t          *numCharsWritten = 0,
                           char                  errorCharacter  = '?');
        // Load, into the specified 'dstString' the result of converting the
        // specified null-terminated UTF-16 '*srcBuffer' to its UTF-8
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of UTF-16 *characters* written, including the null
        // terminator, is to be loaded.  Note that one *character* can occupy
        // multiple *bytes*.  Optionally specify 'errorCharacter' to be
        // substituted (if not 0) for invalid encodings in the input string.
        // (Invalid encodings are incomplete multi-word encodings or parts of a
        // two-word encoding out of their proper sequence.)  If
        // 'errorCharacter' is 0, invalid encodings are omitted entirely from
        // the output string.  Return 0 on success or
        // 'BDEDE_UTF16_INVALID_CHARS_FLAG' if one or more invalid character
        // sequences (see 'errorCharacter' above) were encountered in the
        // input.  The behavior is undefined unless 'srcBuffer' is
        // null-terminated and 'errorCharacter' is either zero or a valid
        // single-byte encoded UTF-8 character ('0 < errorCharacter < 0x80').
        // Note that any previous contents of the destination are discarded.
        // Also note that the string length will be exactly fitted to the
        // length of the output, such that
        // 'strlen(dstString->c_str()) == dstString->length()'.

    static int utf16ToUtf8(bsl::string   *dstString,
                           const wchar_t *srcBuffer,
                           bsl::size_t   *numCharsWritten = 0,
                           char           errorCharacter  = '?');
        // Load, into the specified 'dstString' the result of converting the
        // specified null-terminated UTF-16 '*srcBuffer' to its UTF-8
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of UTF-16 *characters* written, including the null
        // terminator, is to be loaded.  Note that one *character* can occupy
        // multiple *bytes*.  Optionally specify 'errorCharacter' to be
        // substituted (if not 0) for invalid encodings in the input string.
        // (Invalid encodings are incomplete multi-word encodings or parts of a
        // two-word encoding out of their proper sequence.)  If
        // 'errorCharacter' is 0, invalid encodings are omitted entirely from
        // the output string.  Return 0 on success or
        // 'BDEDE_UTF16_INVALID_CHARS_FLAG' if one or more invalid character
        // sequences (see 'errorCharacter' above) were encountered in the
        // input.  The behavior is undefined unless 'srcBuffer' is
        // null-terminated and 'errorCharacter' is either zero or a valid
        // single-byte encoded UTF-8 character ('0 < errorCharacter < 0x80').
        // Note that any previous contents of the destination are discarded.
        // Also note that the string length will be exactly fitted to the
        // length of the output, such that
        // 'strlen(dstString->c_str()) == dstString->length()'.

    static int utf16ToUtf8(bsl::vector<char>    *dstVector,
                           const unsigned short *srcBuffer,
                           bsl::size_t          *numCharsWritten = 0,
                           char                  errorCharacter  = '?');
        // Load, into the specified 'dstVector' the result of converting the
        // specified null-terminated UTF-16 '*srcBuffer' to its UTF-8
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of UTF-16 *characters* written, including the null
        // terminator, is to be loaded.  Note that one *character* can occupy
        // multiple *bytes*.  Optionally specify 'errorCharacter' to be
        // substituted (if not 0) for invalid encodings in the input string.
        // (Invalid encodings are incomplete multi-word encodings or parts of a
        // two-word encoding out of their proper sequence.)  If
        // 'errorCharacter' is 0, invalid encodings are omitted entirely from
        // the output vector.  Return 0 on success or
        // 'BDEDE_UTF16_INVALID_CHARS_FLAG' if one or more invalid character
        // sequences (see 'errorCharacter' above) were encountered in the
        // input.  The behavior is undefined unless 'srcBuffer' is
        // null-terminated and 'errorCharacter' is either zero or a valid
        // single-byte encoded UTF-8 character ('0 < errorCharacter < 0x80').
        // Note that any previous contents of the destination are discarded.
        // Also note that the vector length will be exactly fitted to the
        // length of the output, such that
        // 'strlen(dstVector->begin()) + 1 == dstVector->size()'.

    static int utf16ToUtf8(bsl::vector<char> *dstVector,
                           const wchar_t     *srcBuffer,
                           bsl::size_t       *numCharsWritten = 0,
                           char               errorCharacter  = '?');
        // Load, into the specified 'dstVector' the result of converting the
        // specified null-terminated UTF-16 '*srcBuffer' to its UTF-8
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of UTF-16 *characters* written, including the null
        // terminator, is to be loaded.  Note that one *character* can occupy
        // multiple *bytes*.  Optionally specify 'errorCharacter' to be
        // substituted (if not 0) for invalid encodings in the input string.
        // (Invalid encodings are incomplete multi-word encodings or parts of a
        // two-word encoding out of their proper sequence.)  If
        // 'errorCharacter' is 0, invalid encodings are omitted entirely from
        // the output vector.  Return 0 on success or
        // 'BDEDE_UTF16_INVALID_CHARS_FLAG' if one or more invalid character
        // sequences (see 'errorCharacter' above) were encountered in the
        // input.  The behavior is undefined unless 'srcBuffer' is
        // null-terminated and 'errorCharacter' is either zero or a valid
        // single-byte encoded UTF-8 character ('0 < errorCharacter < 0x80').
        // Note that any previous contents of the destination are discarded.
        // Also note that the vector length will be exactly fitted to the
        // length of the output, such that
        // 'strlen(dstVector->begin()) + 1 == dstVector->size()'.

    static int utf16ToUtf8(char                 *dstBuffer,
                           bsl::size_t           dstCapacity,
                           const unsigned short *srcBuffer,
                           bsl::size_t          *numCharsWritten = 0,
                           bsl::size_t          *numBytesWritten = 0,
                           char                  errorCharacter  = '?');
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-16
        // '*srcBuffer' to its UTF-8 equivalent.  Optionally specify
        // 'numCharsWritten' which (if not 0) indicates the location of the
        // modifiable variable into which the number of UTF-16 *characters*
        // (including the terminating 0, if any) written is to be loaded.  Note
        // that one *character* can occupy multiple *bytes*.  Optionally
        // specify 'numBytesWritten' which (if not 0) indicates the location of
        // the modifiable variable into which the number of bytes written
        // (including the null terminator, if any) is to be loaded.  Optionally
        // specify 'errorCharacter' to be substituted (if not 0) for invalid
        // encodings in the input string.  (Invalid encodings are incomplete
        // multi-word encodings or parts of a two-word encoding out of their
        // proper sequence.)  If 'errorCharacter' is 0, invalid encodings are
        // omitted entirely from the output string.  Return 0 on success, and a
        // bitwise-or of the flags defined by enum 'Utf16Flags' if problem(s)
        // occurred.  'BDEDE_UTF16_INVALID_CHARS_FLAG' will be set if one or
        // more invalid character sequences (see 'errorCharacter' above) were
        // encountered in the input, and 'BDEDE_UTF16_OUT_OF_SPACE_FLAG' will
        // be set if the output space was exhausted before conversion was
        // complete.  The behavior is undefined unless 'dstBuffer' refers to an
        // array of at least 'dstCapacity' elements, 'srcBuffer' is
        // null-terminated, and 'errorCharacter' is either zero or a valid
        // single-byte encoded UTF-8 character ('0 < errorCharacter < 0x80').
        // Note that if 'dstCapacity' is 0, this function returns
        // 'BDEDE_UTF16_OUT_OF_SPACE_FLAG' set and 0 is written into
        // '*numCharsWritten' and '*numBytesWritten' (if those pointers are not
        // zero), since there is insufficient space for even a null terminator
        // alone.  Also note that since UTF-8 is a variable-length encoding,
        // 'numBytesWritten' may be up to four times 'numCharsWritten', and
        // therefore that an input 'srcBuffer' of 'dstCapacity' *characters*
        // (including terminating 0) may not fit into 'dstBuffer'.  A one-word
        // (two-byte) UTF-16 character will require one to three UTF-8 octets
        // (bytes); a two-word (four-byte) UTF-16 character will always require
        // four UTF-8 octets.  Also note that the amount of room needed will
        // vary with the contents of the data and the language being
        // translated, but never will the number of *bytes* output exceed three
        // times the number of *short words* input.  Also note that, if
        // 'dstCapacity > 0', then, after completion,
        // 'strlen(dstBuffer) + 1 == *numBytesWritten'.

    static int utf16ToUtf8(char          *dstBuffer,
                           bsl::size_t    dstCapacity,
                           const wchar_t *srcBuffer,
                           bsl::size_t   *numCharsWritten = 0,
                           bsl::size_t   *numBytesWritten = 0,
                           char           errorCharacter  = '?');
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-16
        // '*srcBuffer' to its UTF-8 equivalent.  Optionally specify
        // 'numCharsWritten' which (if not 0) indicates the location of the
        // modifiable variable into which the number of UTF-16 *characters*
        // (including the terminating 0, if any) written is to be loaded.  Note
        // that one *character* can occupy multiple *bytes*.  Optionally
        // specify 'numBytesWritten' which (if not 0) indicates the location of
        // the modifiable variable into which the number of bytes written
        // (including the null terminator, if any) is to be loaded.  Optionally
        // specify 'errorCharacter' to be substituted (if not 0) for invalid
        // encodings in the input string.  (Invalid encodings are incomplete
        // multi-word encodings or parts of a two-word encoding out of their
        // proper sequence.)  If 'errorCharacter' is 0, invalid encodings are
        // omitted entirely from the output string.  Return 0 on success, and a
        // bitwise-or of the flags defined by enum 'Utf16Flags' if problem(s)
        // occurred.  'BDEDE_UTF16_INVALID_CHARS_FLAG' will be set if one or
        // more invalid character sequences (see 'errorCharacter' above) were
        // encountered in the input, and 'BDEDE_UTF16_OUT_OF_SPACE_FLAG' will
        // be set if the output space was exhausted before conversion was
        // complete.  The behavior is undefined unless 'dstBuffer' refers to an
        // array of at least 'dstCapacity' elements, 'srcBuffer' is
        // null-terminated, and 'errorCharacter' is either zero or a valid
        // single-byte encoded UTF-8 character ('0 < errorCharacter < 0x80').
        // Note that if 'dstCapacity' is 0, this function returns
        // 'BDEDE_UTF16_OUT_OF_SPACE_FLAG' set and 0 is written into
        // '*numCharsWritten' and '*numBytesWritten' (if those pointers are not
        // zero), since there is insufficient space for even a null terminator
        // alone.  Also note that since UTF-8 is a variable-length encoding,
        // 'numBytesWritten' may be up to four times 'numCharsWritten', and
        // therefore that an input 'srcBuffer' of 'dstCapacity' *characters*
        // (including terminating 0) may not fit into 'dstBuffer'.  A one-word
        // (two-byte) UTF-16 character will require one to three UTF-8 octets
        // (bytes); a two-word (four-byte) UTF-16 character will always require
        // four UTF-8 octets.  Also note that the amount of room needed will
        // vary with the contents of the data and the language being
        // translated, but never will the number of *bytes* output exceed three
        // times the number of *short words* input.  Also note that, if
        // 'dstCapacity > 0', then, after completion,
        // 'strlen(dstBuffer) + 1 == *numBytesWritten'.
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
