// bdlde_charconvertutf16.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_CHARCONVERTUTF16
#define INCLUDED_BDLDE_CHARCONVERTUTF16

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide fast, safe conversion between UTF-8 and UTF-16 encodings.
//
//@CLASSES:
//  bdlde::CharConvertUtf16: namespace for conversions between UTF-8 and UTF-16
//
//@DESCRIPTION: This component provides a suite of static functions supporting
// the *fast* conversion of *valid* UTF-8 encoded strings to *valid* UTF-16
// 16-bit character arrays, wstrings, and vectors, and conversion of *valid*
// UTF-16 encoded strings to *valid* UTF-8 character arrays, strings, and
// character vectors.  Invalid byte sequences and characters forbidden by
// either encoding are removed and (optionally) replaced by a character
// provided by the caller.  The byte or word count and character count that are
// optionally returned through pointer arguments include the terminating null
// character, if any, in their count.  The byte order of the UTF-16 input or
// output can be specified via the optional 'byteOrder' argument, which is
// assumed to be host byte order if not specified.  In functions taking UTF-8,
// input is in the form of a 'bslstl::StringRef' or a null-terminated
// 'const char *'.  In functions taking UTF-16, input is either in the form of
// a 'bslstl::StringRefWide' or a pointer to a null-terminated array of
// 'unsigned short' or 'wchar_t'.
//
///History and Motivation
///----------------------
// UTF-8 is a character encoding that allows 32-bit character sets like Unicode
// to be represented using (8-bit) byte strings, while allowing "standard
// ASCII" strings to be used "as-is".  Note that UTF-8 is described in detail
// in RFC 3629 (http://www.ietf.org/rfc/rfc3629.txt).
//
// UTF-16 is a 16-bit character encoding that allows Unicode characters up to
// 0x10ffff to be encoded using one or two 16-bit values.  Note that UTF-16 is
// described in detail in RFC 2781 (http://www.ietf.org/rfc/rfc2781.txt).
//
// The functions here that translate to fixed buffers make a single pass
// through the data.  The functions that translate to 'bsl::string's and STL
// containers, however, like the 'glib' conversion routines, make two passes: a
// size estimation pass, after which the output container is sized
// appropriately if necessary, and then the translation pass.
//
// The methods that output to a 'vector', 'string', or 'wstring' will all grow
// the output object as necessary to fit the data, and in the end will exactly
// resize the object to the output (including the terminating 0 for 'vector',
// which is not included for 'string' or 'wstring').
//
// Non-minimal UTF-8 encodings of characters are reported as errors.  Octets
// and post-conversion characters in the forbidden ranges are treated as errors
// and removed (or replaced, if a replacement character is provided).
//
///WSTRINGS and UTF-16
///-------------------
// UTF-16 (or UTF-8, for that matter) can be stored in 'wstring's, but note
// that the size of a 'wstring::value_type', also known as a 'wchar_t'
// character, varies across different platforms -- it is 4 bytes on Solaris,
// Linux, HP-UX, and Darwin, and 2 bytes on AIX and Windows.  So a file of
// 'wchar_t' characters written by one platform may not be readable by another.
// Byte order is also a consideration, and a non-host byte order can be handled
// by using the optional 'byteOrder' argument of these functions.  Another
// factor is that, since UTF-16 words all fit in 2 bytes, using 'wchar_t' to
// store UTF-16 is very wasteful of space on many platforms.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Translation to Fixed-Length Buffers
/// - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will translate a string containing a non-ASCII character
// from UTF-16 to UTF-8 and back using fixed-length buffers.
//
// First, we create a UTF-16 string spelling 'ecole' in French, which begins
// with '0xc9', a non-ASCII 'e' with an accent over it:
//..
//  unsigned short utf16String[] = { 0xc9, 'c', 'o', 'l', 'e', 0 };
//..
// Then, we create a byte buffer to store the UTF-8 result of the translation
// in, and variables to monitor counts of characters and bytes translated:
//..
//  char utf8String[7];
//  bsl::size_t numChars, numBytes;
//  numChars = numBytes = -1;    // garbage
//..
// Next, we call 'utf16ToUtf8' to do the translation:
//..
//  int rc = bdlde::CharConvertUtf16::utf16ToUtf8(utf8String,
//                                               sizeof(utf8String),
//                                               utf16String,
//                                               &numChars,
//                                               &numBytes);
//..
// Then, we observe that no errors or warnings occurred, and that the numbers
// of characters and bytes were as expected.  Note that both 'numChars' and
// 'numBytes' include the terminating 0:
//..
//  assert(0 == rc);
//  assert(6 == numChars);
//  assert(7 == numBytes);
//..
// Next, we examine the length of the translated string:
//..
//  assert(numBytes - 1 == bsl::strlen(utf8String));
//..
// Then, we examine the individual bytes of the translated UTF-8:
//..
//  assert((char)0xc3 == utf8String[0]);
//  assert((char)0x89 == utf8String[1]);
//  assert('c' ==        utf8String[2]);
//  assert('o' ==        utf8String[3]);
//  assert('l' ==        utf8String[4]);
//  assert('e' ==        utf8String[5]);
//  assert(0   ==        utf8String[6]);
//..
// Next, in preparation for translation back to UTF-16, we create a buffer of
// 'short' values and the variable 'numWords' to track the number of UTF-16
// words occupied by the result:
//..
//  unsigned short secondUtf16String[6];
//  bsl::size_t numWords;
//  numChars = numWords = -1;    // garbage
//..
// Then, we do the reverse translation:
//..
//  rc = bdlde::CharConvertUtf16::utf8ToUtf16(secondUtf16String,
//                                           6,
//                                           utf8String,
//                                           &numChars,
//                                           &numWords);
//..
// Next, we observe that no errors or warnings were reported, and that the
// number of characters and words were as expected.  Note that 'numChars' and
// 'numWords' both include the terminating 0:
//..
//  assert(0 == rc);
//  assert(6 == numChars);
//  assert(6 == numWords);
//..
// Now, we observe that our output is identical to the original UTF-16 string:
//..
//  assert(0 == bsl::memcmp(utf16String,
//                          secondUtf16String,
//                          sizeof(utf16String)));
//..
// Finally, we examine the individual words of the reverse translation:
//..
//  assert(0xc9 == secondUtf16String[0]);
//  assert('c'  == secondUtf16String[1]);
//  assert('o'  == secondUtf16String[2]);
//  assert('l'  == secondUtf16String[3]);
//  assert('e'  == secondUtf16String[4]);
//  assert(0    == secondUtf16String[5]);
//..
//
///Example 2: Translation to STL Containers
/// - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate a typical use of the
// 'bdlde::CharConvertUtf16' struct's utility functions, first converting from
// UTF-8 to UTF-16, and then converting back to make sure the round trip
// returns the same value, translating to STL containers in both directions.
//
// First, we declare a string of UTF-8 containing single-, double-, triple-,
// and quadruple-octet characters:
//..
//  const char utf8MultiLang[] = {
//      "Hello"                                         // -- ASCII
//      "\xce\x97"         "\xce\x95"       "\xce\xbb"  // -- Greek
//      "\xe4\xb8\xad"     "\xe5\x8d\x8e"               // -- Chinese
//      "\xe0\xa4\xad"     "\xe0\xa4\xbe"               // -- Hindi
//      "\xf2\x94\xb4\xa5" "\xf3\xb8\xac\x83" };        // -- Quad octets
//..
// Then, we declare an 'enum' summarizing the counts of characters in the
// string and verify that the counts add up to the length of the string:
//..
//  enum { NUM_ASCII_CHARS   = 5,
//         NUM_GREEK_CHARS   = 3,
//         NUM_CHINESE_CHARS = 2,
//         NUM_HINDI_CHARS   = 2,
//         NUM_QUAD_CHARS    = 2 };
//
//  assert(1 * NUM_ASCII_CHARS +
//         2 * NUM_GREEK_CHARS +
//         3 * NUM_CHINESE_CHARS +
//         3 * NUM_HINDI_CHARS +
//         4 * NUM_QUAD_CHARS == bsl::strlen(utf8MultiLang));
//..
// Next, we declare the vector where our UTF-16 output will go, and a variable
// into which the number of characters (characters, not bytes or words) written
// will be stored.  It is not necessary to initialize 'utf16CharsWritten':
//..
//  bsl::vector<unsigned short> v16;
//  bsl::size_t utf16CharsWritten;
//..
// Note that for performance, we should 'v16.reserve(sizeof(utf8MultiLang))',
// but it's not strictly necessary -- the vector will automatically be grown to
// the correct size.  Also note that if 'v16' were not empty, that wouldn't be
// a problem -- any contents will be discarded.
//
// Then, we do the translation to UTF-16:
//..
//  int retVal = bdlde::CharConvertUtf16::utf8ToUtf16(&v16,
//                                                   utf8MultiLang,
//                                                   &utf16CharsWritten);
//
//  assert(0 == retVal);        // verify success
//  assert(0 == v16.back());    // verify null terminated
//..
// Next, we verify that the number of characters (characters, not bytes or
// words) that was returned is correct:
//..
//  enum { EXPECTED_CHARS_WRITTEN =
//                      NUM_ASCII_CHARS + NUM_GREEK_CHARS + NUM_CHINESE_CHARS +
//                      NUM_HINDI_CHARS + NUM_QUAD_CHARS  + 1 };
//  assert(EXPECTED_CHARS_WRITTEN == utf16CharsWritten);
//..
// Then, we verify that the number of 16-bit words written was correct.  The
// quad octet characters each require 2 'short' words of output:
//..
//  enum { EXPECTED_UTF16_WORDS_WRITTEN =
//                      NUM_ASCII_CHARS + NUM_GREEK_CHARS + NUM_CHINESE_CHARS +
//                      NUM_HINDI_CHARS + NUM_QUAD_CHARS * 2 + 1 };
//
//  assert(EXPECTED_UTF16_WORDS_WRITTEN == v16.size());
//..
// Next, we calculate and confirm the difference between the number of UTF-16
// words output and the number of bytes input.  The ASCII characters will take
// 1 16-bit word apiece, the Greek characters are double octets that will
// become single 'short' values, the Chinese characters are encoded as UTF-8
// triple octets that will turn into single 16-bit words, the same for the
// Hindi characters, and the quad characters are quadruple octets that will
// turn into double 'short' values:
//..
//  enum { SHRINKAGE = NUM_ASCII_CHARS   * (1-1) + NUM_GREEK_CHARS * (2-1) +
//                     NUM_CHINESE_CHARS * (3-1) + NUM_HINDI_CHARS * (3-1) +
//                     NUM_QUAD_CHARS    * (4-2) };
//
//  assert(v16.size() == sizeof(utf8MultiLang) - SHRINKAGE);
//..
// Then, we go on to do the reverse 'utf16ToUtf8' transform to turn it back
// into UTF-8, and we should get a result identical to our original input.
// We declare a 'bsl::string' for our output, and a variable to count the
// number of characters (characters, not bytes or words) translated:
//..
//  bsl::string s;
//  bsl::size_t utf8CharsWritten;
//..
// Again, note that for performance, we should ideally
// 's.reserve(3 * v16.size())' but it's not really necessary.
//
// Now, we do the reverse transform:
//..
//  retVal = bdlde::CharConvertUtf16::utf16ToUtf8(&s,
//                                               v16.begin(),
//                                               &utf8CharsWritten);
//..
// Finally, we verify that a successful status was returned, that the output of
// the reverse transform was identical to the original input, and that the
// number of characters translated was as expected:
//..
//  assert(0 == retVal);
//  assert(utf8MultiLang == s);
//  assert(s.length() + 1         == sizeof(utf8MultiLang));
//
//  assert(EXPECTED_CHARS_WRITTEN == utf8CharsWritten);
//  assert(utf16CharsWritten      == utf8CharsWritten);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDE_BYTEORDER
#include <bdlde_byteorder.h>
#endif

#ifndef INCLUDED_BDLDE_CHARCONVERTSTATUS
#include <bdlde_charconvertstatus.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>            // 'bsl::size_t'
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace bdlde {
                          // =======================
                          // struct CharConvertUtf16
                          // =======================

struct CharConvertUtf16 {
    // This 'struct' provides a namespace for a suite of static functions to
    // convert character buffers or containers between UTF-8 and UTF-16.  Note
    // that Byte Order Mark (BOM) characters are neither generated nor
    // recognized as special.  If a BOM is present in the input, it will be
    // translated, whether correct ('0xfeff') or incorrect ('0xfffe'), into the
    // output without any special handling.

                        // -- UTF-8 to UTF-16 Methods

    // CLASS METHODS
    static int utf8ToUtf16(bsl::wstring             *dstWstring,
                           const bslstl::StringRef&  srcString,
                           bsl::size_t              *numCharsWritten = 0,
                           wchar_t                   errorCharacter  = '?',
                           ByteOrder::Enum     byteOrder       =
                                                      ByteOrder::e_HOST);
    static int utf8ToUtf16(bsl::wstring             *dstWstring,
                           const char               *srcString,
                           bsl::size_t              *numCharsWritten = 0,
                           wchar_t                   errorCharacter  = '?',
                           ByteOrder::Enum     byteOrder       =
                                                      ByteOrder::e_HOST);
        // Load into the specified 'dstWstring' the result of converting the
        // specified UTF-8 'srcString' to its UTF-16 equivalent.  Optionally
        // specify 'numCharsWritten' which, if not 0, indicates the location of
        // the modifiable variable into which the number of UTF-16 characters
        // written, including the terminating null character, is to be loaded.
        // Optionally specify an 'errorCharacter' to be substituted, if not 0,
        // for invalid encodings in the input string.  Optionally specify
        // 'byteOrder' to indicate the byte order of the UTF-16 output; if
        // 'byteOrder' is not specified, the output is assumed to be in host
        // byte order.  Return 0 on success and
        // 'CharConvertStatus::k_INVALID_CHARS_BIT' otherwise.  Invalid
        // encodings are multi-byte encoding parts out of sequence, non-minimal
        // UTF-8 encodings of characters, or characters outside the ranges
        // which UTF-16 can validly encode (in the range '[ 1 .. 0xd7ff ]' or
        // '[ 0xe000 .. 0x10ffff ]').  If 'errorCharacter' is 0, invalid input
        // characters are ignored (i.e., produce no corresponding output
        // characters).  The behavior is undefined unless 'srcString' is
        // null-terminated when specified as a 'const char *'.  Note that one
        // *character* can occupy multiple 16-bit *words*, and that if
        // 'srcString' is a 'bslstl::StringRef', it may contain embedded null
        // bytes which will be translated to null characters embedded in the
        // output.

    static int utf8ToUtf16(bsl::vector<unsigned short> *dstVector,
                           const bslstl::StringRef&     srcString,
                           bsl::size_t                 *numCharsWritten = 0,
                           unsigned short               errorCharacter  = '?',
                           ByteOrder::Enum        byteOrder       =
                                                      ByteOrder::e_HOST);
    static int utf8ToUtf16(bsl::vector<unsigned short> *dstVector,
                           const char                  *srcString,
                           bsl::size_t                 *numCharsWritten = 0,
                           unsigned short               errorCharacter  = '?',
                           ByteOrder::Enum        byteOrder       =
                                                      ByteOrder::e_HOST);
        // Load into the specified 'dstVector' the result of converting the
        // specified UTF-8 'srcString' to its UTF-16 equivalent.  Optionally
        // specify 'numCharsWritten' which (if not 0) indicates the location of
        // the modifiable variable into which the number of UTF-16 characters
        // (including the null terminator) written is to be loaded.  Optionally
        // specify an 'errorCharacter' to be substituted (if not 0) for invalid
        // encodings in the input string.  Invalid encodings are multi-byte
        // encoding parts out of sequence, non-minimal UTF-8 encodings of
        // characters, or characters outside the ranges which UTF-16 can
        // validly encode (in the range '[ 1 .. 0xd7ff ]' or
        // '[ 0xe000 .. 0x10ffff ]').  If 'errorCharacter' is 0, invalid input
        // characters are ignored (i.e., produce no corresponding output
        // characters).  Optionally specify 'byteOrder' to indicate the byte
        // order of the UTF-16 output; if 'byteOrder' is not specified, the
        // output is assumed to be in host byte order.  Any previous contents
        // of the destination are discarded.  Return 0 on success and
        // 'CharConvertStatus::k_INVALID_CHARS_BIT' otherwise.  The behavior is
        // undefined unless 'errorCharacter' is either 0 or a valid single-word
        // encoded UTF-16 character (in the range '[ 1 .. 0xd7ff ]' or
        // '[ 0xe000 .. 0xffff ]') and 'srcString' is null-terminated when
        // specified as a 'const char *'.  Note that one *character* can occupy
        // multiple 16-bit *words*.  Also note that the size of the result
        // vector is always fitted to the null-terminated result, including the
        // terminating 0.  Also note that if 'srcString' is a
        // 'bslstl::StringRef', it may contain embedded null bytes which will
        // be translated to null characters embedded in the output.

    static int utf8ToUtf16(unsigned short           *dstBuffer,
                           bsl::size_t               dstCapacity,
                           const bslstl::StringRef&  srcString,
                           bsl::size_t              *numCharsWritten = 0,
                           bsl::size_t              *numWordsWritten = 0,
                           unsigned short            errorCharacter  = '?',
                           ByteOrder::Enum     byteOrder       =
                                                      ByteOrder::e_HOST);
    static int utf8ToUtf16(unsigned short           *dstBuffer,
                           bsl::size_t               dstCapacity,
                           const char               *srcString,
                           bsl::size_t              *numCharsWritten = 0,
                           bsl::size_t              *numWordsWritten = 0,
                           unsigned short            errorCharacter  = '?',
                           ByteOrder::Enum     byteOrder       =
                                                      ByteOrder::e_HOST);
        // Load into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified UTF-8 'srcString' to its
        // UTF-16 equivalent.  Optionally specify 'numCharsWritten' which (if
        // not 0) indicates the location of the variable into which the number
        // of UTF-16 characters (including the null terminator) written is to
        // be loaded.  Optionally specify 'numWordsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of 'short' *memory words* written (including the null
        // terminator) is to be loaded.  Optionally specify an 'errorCharacter'
        // to be substituted (if not 0) for invalid encodings in the input
        // string.  Invalid encodings are multi-byte encoding parts out of
        // sequence, non-minimal UTF-8 encodings of characters, or characters
        // outside the ranges which UTF-16 can validly encode (in the range
        // '[ 1 .. 0xd7ff ]' or '[ 0xe000 .. 0x10ffff ]').  If 'errorCharacter'
        // is 0, invalid input characters are ignored (i.e., produce no
        // corresponding output characters).  Optionally specify 'byteOrder' to
        // indicate the byte order of the UTF-16 output; if 'byteOrder' is not
        // specified, the output is assumed to be in host byte order.  Return 0
        // on success and a bit-wise or of the bits specified by
        // 'CharConvertStatus::Enum' otherwise to indicate that there
        // were invalid character sequences or if 'dstCapacity' was inadequate
        // to store the output.  If 'dstCapacity > 0' yet 'dstCapacity'
        // specifies a buffer too small to hold the output, the maximal
        // null-terminated prefix of the properly converted result string is
        // loaded into 'dstBuffer'.  The behavior is undefined unless
        // 'dstBuffer' refers to an array of at least 'dstCapacity' elements,
        // 'errorCharacter' is either 0 or a valid single-word encoded UTF-16
        // character (in the range '[ 1 .. 0xd7ff ]' or
        // '[ 0xe000 .. 0xffff ]'), and 'srcString' is null-terminated when
        // supplied as a 'const char *'.  Note that if 'dstCapacity' is 0,
        // '*dstBuffer' is not modified and this function returns a value with
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' set and 0 is written into
        // '*numCharsWritten' and '*numWordsWritten' (if those pointers are
        // non-null), since there is insufficient space for even a null
        // terminator alone.  Also note that one *character* can occupy
        // multiple 16-bit *words*, so that '*numWordsWritten' may be greater
        // than '*numCharsWritten', and therefore that an input 'srcString' of
        // 'dstCapacity' *characters* may not fit into 'dstBuffer', however, an
        // input 'srcString' of 'dstCapacity' *bytes* (including null
        // terminator, if present) will always fit (since the UTF-8 encoding of
        // a character requires at least as many bytes as the UTF-16 encoding
        // requires words).  Also note that if 'srcString' is a
        // 'bslstl::StringRef', it may contain embedded null bytes which will
        // be translated to null characters embedded in the output.

    static int utf8ToUtf16(wchar_t                  *dstBuffer,
                           bsl::size_t               dstCapacity,
                           const bslstl::StringRef&  srcString,
                           bsl::size_t              *numCharsWritten = 0,
                           bsl::size_t              *numWordsWritten = 0,
                           wchar_t                   errorCharacter  = '?',
                           ByteOrder::Enum     byteOrder       =
                                                      ByteOrder::e_HOST);
    static int utf8ToUtf16(wchar_t                  *dstBuffer,
                           bsl::size_t               dstCapacity,
                           const char               *srcString,
                           bsl::size_t              *numCharsWritten = 0,
                           bsl::size_t              *numWordsWritten = 0,
                           wchar_t                   errorCharacter  = '?',
                           ByteOrder::Enum     byteOrder       =
                                                      ByteOrder::e_HOST);
        // Load into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified UTF-8 'srcString' to its
        // UTF-16 equivalent.  Optionally specify 'numCharsWritten' which (if
        // not 0) indicates the location of the variable into which the number
        // of UTF-16 characters (including the terminating 0) written is to be
        // loaded.  Optionally specify 'numWordsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of 'short' *memory words* written (including the null
        // terminator) is to be loaded.  Optionally specify an 'errorCharacter'
        // to be substituted (if not 0) for invalid encodings in the input
        // string.  Invalid encodings are multi-byte encoding parts out of
        // sequence, non-minimal UTF-8 encodings of characters, or characters
        // outside the ranges which UTF-16 can validly encode (in the range
        // '[ 1 .. 0xd7ff ]' or '[ 0xde00 .. 0x10ffff ]').  Optionally specify
        // 'byteOrder' to indicate the byte order of the UTF-16 output; if
        // 'byteOrder' is not specified, the output is assumed to be in host
        // byte order.  If 'errorCharacter' is 0, invalid input characters are
        // ignored (i.e., produce no corresponding output characters).  Return
        // 0 on success and a bit-wise or of the bits specified by
        // 'CharConvertStatus::Enum' otherwise to indicate that there
        // were invalid character sequences or if 'dstCapacity' was inadequate
        // to store the output.  If 'dstCapacity > 0' yet 'dstCapacity'
        // specifies a buffer too small to hold the output, the maximal
        // null-terminated prefix of the properly converted result string is
        // loaded into 'dstBuffer'.  The behavior is undefined unless
        // 'dstBuffer', if specified, refers to an array of at least
        // 'dstCapacity' elements, 'errorCharacter' is either 0 or a valid
        // single-word encoded UTF-16 character (in the range '[ 1 .. 0xd7ff ]'
        // or '[ 0xe000 .. 0xffff ]'), and 'srcString' is null-terminated if
        // supplied as a 'const char *'.  Note that if 'dstCapacity' is 0,
        // '*dstBuffer' is not modified and this function returns a value with
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' set and 0 is written into
        // '*numCharsWritten' and '*numWordsWritten' (if those pointers are
        // non-null), since there is insufficient space for even a null
        // terminator alone.  Also note that one *character* can occupy
        // multiple 16-bit *words*, so that '*numWordsWritten' may be greater
        // than '*numCharsWritten', and therefore that an input 'srcString' of
        // 'dstCapacity' *characters* may not fit into 'dstBuffer'.  However,
        // an input 'srcString' of 'dstCapacity' *bytes* (including terminating
        // 0, if present) will always fit (since the UTF-8 encoding of a
        // character requires at least as many bytes as the UTF-16 encoding
        // requires words).  Also note that if 'srcString' is a
        // 'bslstl::StringRef', it may contain embedded null bytes which will
        // be translated to null characters embedded in the output.

                        // -- UTF-16 to UTF-8 Methods

    static int utf16ToUtf8(bsl::string          *dstString,
                           const unsigned short *srcString,
                           bsl::size_t          *numCharsWritten = 0,
                           char                  errorCharacter  = '?',
                           ByteOrder::Enum byteOrder       =
                                                      ByteOrder::e_HOST);
        // Load into the specified 'dstString' the result of converting the
        // specified null-terminated UTF-16 '*srcString' to its UTF-8
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of Unicode *characters* written, including the null
        // terminator, is to be loaded, where one *character* may occupy
        // multiple *bytes*.  Optionally specify an 'errorCharacter' to be
        // substituted (if not 0) for invalid encodings in the input string.
        // Invalid encodings are incomplete multi-word encodings or parts of a
        // two-word encoding out of their proper sequence.  If 'errorCharacter'
        // is 0, invalid input characters are ignored (i.e., produce no
        // corresponding output characters).  Optionally specify 'byteOrder' to
        // indicate the byte order of the UTF-16 input; if 'byteOrder' is not
        // specified, the input is assumed to be in host byte order.  Any
        // previous contents of the destination are discarded.  Return 0 on
        // success and 'CharConvertStatus::k_INVALID_CHARS_BIT' if one or more
        // invalid character sequences were encountered in the input.  The
        // behavior is undefined unless 'srcString' is null-terminated and
        // 'errorCharacter' is either 0 or a valid single-byte encoded UTF-8
        // character ('0 < errorCharacter < 0x80').  Note that the string
        // length will be sized to the length of the output, such that
        // 'strlen(dstString->c_str()) == dstString->length()'.

    static int utf16ToUtf8(bsl::string                  *dstString,
                           const bslstl::StringRefWide&  srcString,
                           bsl::size_t                  *numCharsWritten = 0,
                           char                          errorCharacter = '?',
                           ByteOrder::Enum         byteOrder      =
                                                      ByteOrder::e_HOST);
    static int utf16ToUtf8(bsl::string                  *dstString,
                           const wchar_t                *srcString,
                           bsl::size_t                  *numCharsWritten = 0,
                           char                          errorCharacter = '?',
                           ByteOrder::Enum         byteOrder      =
                                                      ByteOrder::e_HOST);
        // Load into the specified 'dstString' the result of converting the
        // specified UTF-16 'srcString' to its UTF-8 equivalent.  Optionally
        // specify 'numCharsWritten' which (if not 0) indicates the location of
        // the modifiable variable into which the number of Unicode
        // *characters* written, including the null terminator, is to be
        // loaded, where one *character* may occupy multiple *bytes*.
        // Optionally specify an 'errorCharacter' to be substituted (if not 0)
        // for invalid encodings in the input string.  Invalid encodings are
        // incomplete multi-word encodings or parts of a two-word encoding out
        // of their proper sequence.  If 'errorCharacter' is 0, invalid input
        // characters are ignored (i.e., produce no corresponding output
        // characters).  Any previous contents of the destination are
        // discarded.  Optionally specify 'byteOrder' to indicate the byte
        // order of the UTF-16 input; if 'byteOrder' is not specified, the
        // input is assumed to be in host byte order.  Return 0 on success and
        // 'CharConvertStatus::k_INVALID_CHARS_BIT' if one or more invalid
        // character sequences were encountered in the input.  The behavior is
        // undefined unless 'errorCharacter' is either 0 or a valid single-byte
        // encoded UTF-8 character ('0 < errorCharacter < 0x80') and
        // 'srcString' is null-terminated if supplied as a 'const wchar_t *'.
        // Note that if 'srcString' is a 'bslstl::StringRefWide', it may
        // contain embedded 0 words which will be translated to null characters
        // embedded in the output.

    static int utf16ToUtf8(bsl::vector<char>    *dstVector,
                           const unsigned short *srcString,
                           bsl::size_t          *numCharsWritten = 0,
                           char                  errorCharacter  = '?',
                           ByteOrder::Enum byteOrder       =
                                                      ByteOrder::e_HOST);
        // Load into the specified 'dstVector' the result of converting the
        // specified null-terminated UTF-16 '*srcString' to its UTF-8
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of Unicode *characters* written, including the null
        // terminator, is to be loaded, where one *character* may occupy
        // multiple *bytes*.  Optionally specify an 'errorCharacter' to be
        // substituted (if not 0) for invalid encodings in the input string.
        // Invalid encodings are incomplete multi-word encodings or parts of a
        // two-word encoding out of their proper sequence.  If 'errorCharacter'
        // is 0, invalid input characters are ignored (i.e., produce no
        // corresponding output characters).  Optionally specify 'byteOrder' to
        // indicate the byte order of the UTF-16 input; if 'byteOrder' is not
        // specified, the input is assumed to be in host byte order.  Any
        // previous contents of the destination are discarded.  Return 0 on
        // success and 'CharConvertStatus::k_INVALID_CHARS_BIT' if one or more
        // invalid character sequences were encountered in the input.  The
        // behavior is undefined unless 'srcString' is null-terminated and
        // 'errorCharacter' is either 0 or a valid single-byte encoded UTF-8
        // character ('0 < errorCharacter < 0x80').

    static int utf16ToUtf8(bsl::vector<char>            *dstVector,
                           const bslstl::StringRefWide&  srcString,
                           bsl::size_t                  *numCharsWritten = 0,
                           char                          errorCharacter  = '?',
                           ByteOrder::Enum         byteOrder       =
                                                      ByteOrder::e_HOST);
    static int utf16ToUtf8(bsl::vector<char>            *dstVector,
                           const wchar_t                *srcString,
                           bsl::size_t                  *numCharsWritten = 0,
                           char                          errorCharacter = '?',
                           ByteOrder::Enum         byteOrder       =
                                                      ByteOrder::e_HOST);
        // Load into the specified 'dstVector' the result of converting the
        // specified UTF-16 'srcString' to its UTF-8 equivalent.  Optionally
        // specify 'numCharsWritten' which (if not 0) indicates the location of
        // the modifiable variable into which the number of Unicode
        // *characters* written, including the null terminator, is to be
        // loaded, where one *character* may occupy multiple *bytes*.
        // Optionally specify an 'errorCharacter' to be substituted (if not 0)
        // for invalid encodings in the input string.  Invalid encodings are
        // incomplete multi-word encodings or parts of a two-word encoding out
        // of their proper sequence.  If 'errorCharacter' is 0, invalid input
        // characters are ignored (i.e., produce no corresponding output
        // characters).  Optionally specify 'byteOrder' to indicate the byte
        // order of the UTF-16 input; if 'byteOrder' is not specified, the
        // input is assumed to be in host byte order.  Any previous contents of
        // the destination are discarded.  Return 0 on success and
        // 'CharConvertStatus::k_INVALID_CHARS_BIT' if one or more invalid
        // character sequences were encountered in the input.  The behavior is
        // undefined unless 'errorCharacter' is either 0 or a valid single-byte
        // encoded UTF-8 character ('0 < errorCharacter < 0x80') and
        // 'srcString' is null-terminated if supplied as a 'const wchar_t *'.
        // Note that if 'srcString' is a 'bslstl::StringRef', it may contain
        // embedded 0 words which will be translated to null characters
        // embedded in the output.

    static int utf16ToUtf8(char                 *dstBuffer,
                           bsl::size_t           dstCapacity,
                           const unsigned short *srcString,
                           bsl::size_t          *numCharsWritten = 0,
                           bsl::size_t          *numBytesWritten = 0,
                           char                  errorCharacter  = '?',
                           ByteOrder::Enum byteOrder       =
                                                      ByteOrder::e_HOST);
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-16
        // '*srcString' to its UTF-8 equivalent.  Optionally specify
        // 'numCharsWritten' which (if not 0) indicates the location of the
        // modifiable variable into which the number of Unicode *characters*
        // (including the terminating 0, if any) written is to be loaded, where
        // one *character* can occupy multiple *bytes*.  Optionally specify
        // 'numBytesWritten' which (if not 0) indicates the location of the
        // modifiable variable into which the number of bytes written
        // (including the null terminator, if any) is to be loaded.  Optionally
        // specify an 'errorCharacter' to be substituted (if not 0) for invalid
        // encodings in the input string.  Invalid encodings are incomplete
        // multi-word encodings or parts of a two-word encoding out of their
        // proper sequence.  If 'errorCharacter' is 0, invalid input characters
        // are ignored (i.e., produce no corresponding output characters).
        // Optionally specify 'byteOrder' to indicate the byte order of the
        // UTF-16 input; if 'byteOrder' is not specified, the input is assumed
        // to be in host byte order.  Return 0 on success and a bitwise-or of
        // the masks defined by 'CharConvertStatus::Enum' otherwise,
        // where 'CharConvertStatus::k_INVALID_CHARS_BIT' will be set if one or
        // more invalid character sequences were encountered in the input, and
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' will be set if the output
        // space was exhausted before conversion was complete.  The behavior is
        // undefined unless 'dstBuffer' refers to an array of at least
        // 'dstCapacity' elements, 'srcString' is null-terminated, and
        // 'errorCharacter' is either 0 or a valid single-byte encoded UTF-8
        // character ('0 < errorCharacter < 0x80').  Note that if 'dstCapacity'
        // is 0, this function returns 'CharConvertStatus::k_OUT_OF_SPACE_BIT'
        // set and 0 is written into '*numCharsWritten' and '*numBytesWritten'
        // (if those pointers are non-null), since there is insufficient space
        // for even a null terminator alone.  Also note that since UTF-8 is a
        // variable-length encoding, 'numBytesWritten' may be up to four times
        // 'numCharsWritten', and therefore that an input 'srcString' of
        // 'dstCapacity' *characters* (including the terminating 0) may not fit
        // into 'dstBuffer'.  A one-word (two-byte) UTF-16 character will
        // require one to three UTF-8 octets (bytes); a two-word (four-byte)
        // UTF-16 character will always require four UTF-8 octets.  Also note
        // that the amount of room needed will vary with the contents of the
        // data and the language being translated, but never will the number of
        // *bytes* output exceed three times the number of *short words* input.
        // Also note that, if 'dstCapacity > 0', then, after completion,
        // 'strlen(dstBuffer) + 1 == *numBytesWritten'.

    static int utf16ToUtf8(char                         *dstBuffer,
                           bsl::size_t                   dstCapacity,
                           const bslstl::StringRefWide&  srcString,
                           bsl::size_t                  *numCharsWritten = 0,
                           bsl::size_t                  *numBytesWritten = 0,
                           char                          errorCharacter  = '?',
                           ByteOrder::Enum         byteOrder       =
                                                      ByteOrder::e_HOST);
    static int utf16ToUtf8(char                         *dstBuffer,
                           bsl::size_t                   dstCapacity,
                           const wchar_t                *srcString,
                           bsl::size_t                  *numCharsWritten = 0,
                           bsl::size_t                  *numBytesWritten = 0,
                           char                          errorCharacter  = '?',
                           ByteOrder::Enum         byteOrder       =
                                                      ByteOrder::e_HOST);
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified UTF-16 'srcString' to its
        // UTF-8 equivalent.  Optionally specify 'numCharsWritten' which (if
        // not 0) indicates the location of the modifiable variable into which
        // the number of Unicode *characters* (including the terminating 0, if
        // any) written is to be loaded, where one *character* can occupy
        // multiple *bytes*.  Optionally specify 'numBytesWritten' which (if
        // not 0) indicates the location of the modifiable variable into which
        // the number of bytes written (including the null terminator, if any)
        // is to be loaded.  Optionally specify an 'errorCharacter' to be
        // substituted (if not 0) for invalid encodings in the input string.
        // Invalid encodings are incomplete multi-word encodings or parts of a
        // two-word encoding out of their proper sequence.  If 'errorCharacter'
        // is 0, invalid input characters are ignored (i.e., produce no
        // corresponding output characters).  Optionally specify 'byteOrder' to
        // indicate the byte order of the UTF-16 input; if 'byteOrder' is not
        // specified, the input is assumed to be in host byte order.  Return 0
        // on success and a bitwise-or of the flags defined by
        // 'CharConvertStatus::Enum' otherwise.
        // 'CharConvertStatus::k_INVALID_CHARS_BIT' will be set if one or more
        // invalid character sequences were encountered in the input, and
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' will be set if the output
        // space was exhausted before conversion was complete.  The behavior is
        // undefined unless 'dstBuffer' refers to an array of at least
        // 'dstCapacity' elements, 'errorCharacter' is either 0 or a valid
        // single-byte encoded UTF-8 character ('0 < errorCharacter < 0x80'),
        // and 'srcString' is null-terminated if supplied as a
        // 'const wchar_t *'.  Note that if 'dstCapacity' is 0, this function
        // returns 'CharConvertStatus::k_OUT_OF_SPACE_BIT' set and 0 is written
        // into '*numCharsWritten' and '*numBytesWritten' (if those pointers
        // are non-null), since there is insufficient space for even a null
        // terminator alone.  Also note that since UTF-8 is a variable-length
        // encoding, 'numBytesWritten' may be up to four times
        // 'numCharsWritten', and therefore that an input 'srcString' of
        // 'dstCapacity' *characters* (including the terminating 0, if present)
        // may not fit into 'dstBuffer'.  A one-word (two-byte) UTF-16
        // character will require one to three UTF-8 octets (bytes); a two-word
        // (four-byte) UTF-16 character will always require four UTF-8 octets.
        // Also note that the amount of room needed will vary with the contents
        // of the data and the language being translated, but never will the
        // number of *bytes* output exceed three times the number of *short
        // words* input.  Also note that, if 'dstCapacity > 0', then, after
        // completion, 'strlen(dstBuffer) + 1 == *numBytesWritten'.  Also note
        // that if 'srcString' is a 'bslstl::StringRef', it may contain
        // embedded 0 words which will be translated to null characters
        // embedded in the output.
};
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
