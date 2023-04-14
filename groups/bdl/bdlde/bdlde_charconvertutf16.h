// bdlde_charconvertutf16.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_CHARCONVERTUTF16
#define INCLUDED_BDLDE_CHARCONVERTUTF16

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide fast, safe conversion between UTF-8 and UTF-16 encodings.
//
//@CLASSES:
//  bdlde::CharConvertUtf16: namespace for conversions between UTF-8 and UTF-16
//
//@DESCRIPTION: This component provides a suite of static functions supporting
// the *fast* conversion of *valid* UTF-8 encoded strings to *valid* UTF-16
// 16-bit word arrays, wstrings, and vectors, and conversion of *valid* UTF-16
// encoded word sequences to *valid* UTF-8 byte arrays, strings, and byte
// vectors.  Invalid byte sequences and code points forbidden by either
// encoding are removed and (optionally) replaced by a single word or byte
// provided by the caller.  In UTF-16 -> UTF-8 conversion, the replacement word
// must be a non-zero byte, in the other direction, it must be a single,
// non-zero word.  The byte or word count and code point count that are
// optionally returned through pointer arguments include the terminating null
// code point in their count.  The byte order of the UTF-16 input or output can
// be specified via the optional 'byteOrder' argument, which is assumed to be
// host byte order if not specified.  In functions taking UTF-8, input is in
// the form of a 'bslstl::StringRef' or a null-terminated 'const char *'.  In
// functions taking UTF-16, input is either in the form of a
// 'bslstl::StringRefWide' or a pointer to a null-terminated array of
// 'unsigned short' or 'wchar_t'.
//
///History and Motivation
///----------------------
// UTF-8 is an encoding that allows 32-bit character sets like Unicode
// to be represented using (8-bit) byte strings, while allowing "standard
// ASCII" strings to be used "as-is".  Note that UTF-8 is described in detail
// in RFC 3629 (http://www.ietf.org/rfc/rfc3629.txt).
//
// UTF-16 is a 16-bit encoding that allows Unicode code points up to 0x10ffff
// to be encoded using one or two 16-bit values.  Note that UTF-16 is described
// in detail in RFC 2781 (http://www.ietf.org/rfc/rfc2781.txt).
//
// The functions here that translate to fixed buffers make a single pass
// through the data.  The functions that translate to 'bsl::string's and STL
// containers, however, like the 'glib' conversion routines, make two passes: a
// size estimation pass, after which the output container is sized
// appropriately, and then the translation pass.
//
// The methods that output to a 'vector', 'string', or 'wstring' will all grow
// the output object as necessary to fit the data, and in the end will exactly
// resize the object to the output (including the terminating 0 for 'vector',
// which is not included for 'string' or 'wstring').  Note that in the case of
// 'string' or 'wstring', the terminating 0 code point is still included in the
// code point count.
//
// Non-minimal UTF-8 encodings of code points are reported as errors.  Octets
// and post-conversion code points in the forbidden ranges are treated as
// errors and removed (or replaced, if a replacement word is provided).
//
///WSTRINGS and UTF-16
///-------------------
// UTF-16 (or UTF-8, for that matter) can be stored in 'wstring's, but note
// that the size of a 'wstring::value_type', also known as a 'wchar_t' word,
// varies across different platforms -- it is 4 bytes on Solaris, Linux, and
// Darwin, and 2 bytes on AIX and Windows.  So a file of 'wchar_t' words
// written by one platform may not be readable by another.  Byte order is also
// a consideration, and a non-host byte order can be handled by using the
// optional 'byteOrder' argument of these functions.  Another factor is that,
// since UTF-16 words all fit in 2 bytes, using 'wchar_t' to store UTF-16 is
// very wasteful of space on many platforms.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Translation to Fixed-Length Buffers
/// - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will translate a string containing a non-ASCII code
// point from UTF-16 to UTF-8 and back using fixed-length buffers.
//
// First, we create a UTF-16 string spelling 'ecole' in French, which begins
// with '0xc9', a non-ASCII 'e' with an accent over it:
//..
//  unsigned short utf16String[] = { 0xc9, 'c', 'o', 'l', 'e', 0 };
//..
// Then, we create a byte buffer to store the UTF-8 result of the translation
// in, and variables to monitor counts of code points and bytes translated:
//..
//  char utf8String[7];
//  bsl::size_t numCodePoints, numBytes;
//  numCodePoints = numBytes = -1;    // garbage
//..
// Next, we call 'utf16ToUtf8' to do the translation:
//..
//  int rc = bdlde::CharConvertUtf16::utf16ToUtf8(utf8String,
//                                                sizeof(utf8String),
//                                                utf16String,
//                                                &numCodePoints,
//                                                &numBytes);
//..
// Then, we observe that no errors or warnings occurred, and that the numbers
// of code points and bytes were as expected.  Note that both 'numCodePoints'
// and 'numBytes' include the terminating 0:
//..
//  assert(0 == rc);
//  assert(6 == numCodePoints);
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
//  numCodePoints = numWords = -1;    // garbage
//..
// Then, we do the reverse translation:
//..
//  rc = bdlde::CharConvertUtf16::utf8ToUtf16(secondUtf16String,
//                                            6,
//                                            utf8String,
//                                            &numCodePoints,
//                                            &numWords);
//..
// Next, we observe that no errors or warnings were reported, and that the
// number of code points and words were as expected.  Note that 'numCodePoints'
// and 'numWords' both include the terminating 0:
//..
//  assert(0 == rc);
//  assert(6 == numCodePoints);
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
// and quadruple-octet code points:
//..
//  const char utf8MultiLang[] = {
//      "Hello"                                         // -- ASCII
//      "\xce\x97"         "\xce\x95"       "\xce\xbb"  // -- Greek
//      "\xe4\xb8\xad"     "\xe5\x8d\x8e"               // -- Chinese
//      "\xe0\xa4\xad"     "\xe0\xa4\xbe"               // -- Hindi
//      "\xf2\x94\xb4\xa5" "\xf3\xb8\xac\x83" };        // -- Quad octets
//..
// Then, we declare an 'enum' summarizing the counts of code points in the
// string and verify that the counts add up to the length of the string:
//..
//  enum { NUM_ASCII_CODE_POINTS   = 5,
//         NUM_GREEK_CODE_POINTS   = 3,
//         NUM_CHINESE_CODE_POINTS = 2,
//         NUM_HINDI_CODE_POINTS   = 2,
//         NUM_QUAD_CODE_POINTS    = 2 };
//
//  assert(1 * NUM_ASCII_CODE_POINTS +
//         2 * NUM_GREEK_CODE_POINTS +
//         3 * NUM_CHINESE_CODE_POINTS +
//         3 * NUM_HINDI_CODE_POINTS +
//         4 * NUM_QUAD_CODE_POINTS == bsl::strlen(utf8MultiLang));
//..
// Next, we declare the vector where our UTF-16 output will go, and a variable
// into which the number of code points (not bytes or words) written will be
// stored.  It is not necessary to initialize 'utf16CodePointsWritten':
//..
//  bsl::vector<unsigned short> v16;
//  bsl::size_t utf16CodePointsWritten;
//..
// Note that for performance, we should 'v16.reserve(sizeof(utf8MultiLang))',
// but it's not strictly necessary -- the vector will automatically be grown to
// the correct size.  Also note that if 'v16' were not empty, that wouldn't be
// a problem -- any contents will be discarded.
//
// Then, we do the translation to UTF-16:
//..
//  int retVal = bdlde::CharConvertUtf16::utf8ToUtf16(&v16,
//                                                    utf8MultiLang,
//                                                    &utf16CodePointsWritten);
//
//  assert(0 == retVal);        // verify success
//  assert(0 == v16.back());    // verify null terminated
//..
// Next, we verify that the number of code points (not bytes or words) that was
// returned is correct:
//..
//  enum { EXPECTED_CODE_POINTS_WRITTEN =
//                      NUM_ASCII_CODE_POINTS + NUM_GREEK_CODE_POINTS +
//                      NUM_CHINESE_CODE_POINTS + NUM_HINDI_CODE_POINTS +
//                      NUM_QUAD_CODE_POINTS  + 1 };
//
//  assert(EXPECTED_CODE_POINTS_WRITTEN == utf16CodePointsWritten);
//..
// Then, we verify that the number of 16-bit words written was correct.  The
// quad octet code points each require 2 'short' words of output:
//..
//  enum { EXPECTED_UTF16_WORDS_WRITTEN =
//                      NUM_ASCII_CODE_POINTS + NUM_GREEK_CODE_POINTS +
//                      NUM_CHINESE_CODE_POINTS + NUM_HINDI_CODE_POINTS +
//                      NUM_QUAD_CODE_POINTS * 2 + 1 };
//
//  assert(EXPECTED_UTF16_WORDS_WRITTEN == v16.size());
//..
// Next, we calculate and confirm the difference between the number of UTF-16
// words output and the number of bytes input.  The ASCII code points will take
// 1 16-bit word apiece, the Greek code points are double octets that will
// become single 'short' values, the Chinese code points are encoded as UTF-8
// triple octets that will turn into single 16-bit words, the same for the
// Hindi code points, and the quad code points are quadruple octets that will
// turn into double 'short' values:
//..
//  enum { SHRINKAGE = NUM_ASCII_CODE_POINTS   * (1-1) +
//                     NUM_GREEK_CODE_POINTS   * (2-1) +
//                     NUM_CHINESE_CODE_POINTS * (3-1) +
//                     NUM_HINDI_CODE_POINTS   * (3-1) +
//                     NUM_QUAD_CODE_POINTS    * (4-2) };
//
//  assert(v16.size() == sizeof(utf8MultiLang) - SHRINKAGE);
//..
// Then, we go on to do the reverse 'utf16ToUtf8' transform to turn it back
// into UTF-8, and we should get a result identical to our original input.  We
// declare a 'bsl::string' for our output, and a variable to count the number
// of code points (not bytes or words) translated:
//..
//  bsl::string s;
//  bsl::size_t uf8CodePointsWritten;
//..
// Again, note that for performance, we should ideally
// 's.reserve(3 * v16.size())' but it's not really necessary.
//
// Now, we do the reverse transform:
//..
//  retVal = bdlde::CharConvertUtf16::utf16ToUtf8(&s,
//                                                v16.begin(),
//                                                &uf8CodePointsWritten);
//..
// Finally, we verify that a successful status was returned, that the output of
// the reverse transform was identical to the original input, and that the
// number of code points translated was as expected:
//..
//  assert(0 == retVal);
//  assert(utf8MultiLang == s);
//  assert(s.length() + 1               == sizeof(utf8MultiLang));
//
//  assert(EXPECTED_CODE_POINTS_WRITTEN == uf8CodePointsWritten);
//  assert(utf16CodePointsWritten       == uf8CodePointsWritten);
//..

#include <bdlscm_version.h>

#include <bdlde_byteorder.h>
#include <bdlde_charconvertstatus.h>

#include <bsl_cstddef.h>            // 'bsl::size_t'
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#include <memory_resource>
#endif

#include <string>                   // 'std::string', 'std::pmr::string'
#include <vector>                   // 'std::vector', 'std::pmr::vector'

namespace BloombergLP {

namespace bdlde {
                          // =======================
                          // struct CharConvertUtf16
                          // =======================

struct CharConvertUtf16 {
    // This 'struct' provides a namespace for a suite of static functions to
    // convert buffers or containers between UTF-8 and UTF-16.  Note that Byte
    // Order Mark (BOM) sequences are neither generated nor recognized as
    // special.  If a BOM is present in the input, it will be translated,
    // whether correct ('0xfeff') or incorrect ('0xfffe'), into the output
    // without any special handling.

    // CLASS METHODS

                        // -- UTF-8 to UTF-16 Methods

    static bsl::size_t computeRequiredUtf16Words(const char *srcBuffer,
                                                 const char *endPtr = 0);
        // Return the number of words required to store the translation of the
        // specified UTF-8 string 'srcBuffer' into a 0 terminated UTF-16 string
        // (including the 0 terminating word into the returned count).
        // Optionally specify 'endPtr', referring to one past the last input
        // character.  If 'endPtr' is not supplied, or is 0, treat 'srcBuffer'
        // as 0 terminated.  Note that this function will return the size
        // 'utf8ToUtf16' will require, assuming the 'errorWord' argument to
        // 'utf8ToUtf16' is non-zero.

    static int utf8ToUtf16(
                          bsl::wstring            *dstString,
                          const bsl::string_view&  srcString,
                          bsl::size_t             *numCodePointsWritten = 0,
                          wchar_t                  errorWord            = '?',
                          ByteOrder::Enum          byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf16(
                          std::wstring            *dstString,
                          const bsl::string_view&  srcString,
                          bsl::size_t             *numCodePointsWritten = 0,
                          wchar_t                  errorWord            = '?',
                          ByteOrder::Enum          byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf8ToUtf16(
                       std::pmr::wstring       *dstString,
                       const bsl::string_view&  srcString,
                       bsl::size_t             *numCodePointsWritten = 0,
                       wchar_t                  errorWord            = '?',
                       ByteOrder::Enum          byteOrder = ByteOrder::e_HOST);
#endif
    static int utf8ToUtf16(
                          bsl::wstring             *dstString,
                          const char               *srcString,
                          bsl::size_t              *numCodePointsWritten = 0,
                          wchar_t                   errorWord            = '?',
                          ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf16(
                          std::wstring             *dstString,
                          const char               *srcString,
                          bsl::size_t              *numCodePointsWritten = 0,
                          wchar_t                   errorWord            = '?',
                          ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf8ToUtf16(std::pmr::wstring *dstString,
                           const char        *srcString,
                           bsl::size_t       *numCodePointsWritten = 0,
                           wchar_t            errorWord            = '?',
                           ByteOrder::Enum    byteOrder = ByteOrder::e_HOST);
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
    static int utf8ToUtf16(
                          bsl::u16string          *dstString,
                          const bsl::string_view&  srcString,
                          bsl::size_t             *numCodePointsWritten = 0,
                          char16_t                 errorChar            = '?',
                          ByteOrder::Enum          byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf16(
                          std::u16string          *dstString,
                          const bsl::string_view&  srcString,
                          bsl::size_t             *numCodePointsWritten = 0,
                          char16_t                 errorChar            = '?',
                          ByteOrder::Enum          byteOrder            =
                                                            ByteOrder::e_HOST);
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf8ToUtf16(
                          std::pmr::u16string      *dstString,
                          const bsl::string_view&   srcString,
                          bsl::size_t              *numCodePointsWritten = 0,
                          char16_t                  errorChar            = '?',
                          ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
# endif
    static int utf8ToUtf16(
                          bsl::u16string           *dstString,
                          const char               *srcString,
                          bsl::size_t              *numCodePointsWritten = 0,
                          char16_t                  errorChar            = '?',
                          ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf16(
                          std::u16string           *dstString,
                          const char               *srcString,
                          bsl::size_t              *numCodePointsWritten = 0,
                          char16_t                  errorChar            = '?',
                          ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf8ToUtf16(
                          std::pmr::u16string      *dstString,
                          const char               *srcString,
                          bsl::size_t              *numCodePointsWritten = 0,
                          char16_t                  errorChar            = '?',
                          ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
# endif
#endif
        // Load into the specified 'dstString' the result of converting the
        // specified UTF-8 'srcString' to its UTF-16 equivalent.  Optionally
        // specify 'numCodePointsWritten', which, if not 0, indicates the
        // location of the modifiable variable into which the number of Unicode
        // code points written, including the terminating null character, is to
        // be loaded.  Optionally specify an 'errorChar' to be substituted, if
        // not 0, for invalid encodings in the input string.  Optionally
        // specify 'byteOrder' to indicate the byte order of the UTF-16 output;
        // if 'byteOrder' is not specified, the output is assumed to be in host
        // byte order.  Return 0 on success and
        // 'CharConvertStatus::k_INVALID_INPUT_BIT' otherwise.  Invalid
        // encodings are multi-byte encoding parts out of sequence, non-minimal
        // UTF-8 encodings of code points, or code points outside the ranges
        // that UTF-16 can validly encode (in the range '[ 1 .. 0xd7ff ]' or
        // '[ 0xe000 .. 0x10ffff ]').  If 'errorChar' is 0, invalid input code
        // points are ignored (i.e., produce no corresponding output).  The
        // behavior is undefined unless 'srcString' is null-terminated when
        // specified as a 'const char *'.  Note that one code point can occupy
        // multiple UTF-16 words, and that if 'srcString' is a
        // 'bslstl::StringRef', it may contain embedded null bytes that will be
        // translated to null words embedded in the output.

    static int utf8ToUtf16(
                       bsl::vector<unsigned short> *dstVector,
                       const bsl::string_view&      srcString,
                       bsl::size_t                 *numCodePointsWritten = 0,
                       unsigned short               errorWord            = '?',
                       ByteOrder::Enum              byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf16(
                       std::vector<unsigned short> *dstVector,
                       const bsl::string_view&      srcString,
                       bsl::size_t                 *numCodePointsWritten = 0,
                       unsigned short               errorWord            = '?',
                       ByteOrder::Enum              byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf8ToUtf16(
              std::pmr::vector<unsigned short> *dstVector,
              const bsl::string_view&           srcString,
              bsl::size_t                      *numCodePointsWritten = 0,
              unsigned short                    errorWord            = '?',
              ByteOrder::Enum                   byteOrder = ByteOrder::e_HOST);
#endif
    static int utf8ToUtf16(
                       bsl::vector<unsigned short> *dstVector,
                       const char                  *srcString,
                       bsl::size_t                 *numCodePointsWritten = 0,
                       unsigned short               errorWord            = '?',
                       ByteOrder::Enum              byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf16(
                       std::vector<unsigned short> *dstVector,
                       const char                  *srcString,
                       bsl::size_t                 *numCodePointsWritten = 0,
                       unsigned short               errorWord            = '?',
                       ByteOrder::Enum              byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf8ToUtf16(
              std::pmr::vector<unsigned short> *dstVector,
              const char                       *srcString,
              bsl::size_t                      *numCodePointsWritten = 0,
              unsigned short                    errorWord            = '?',
              ByteOrder::Enum                   byteOrder = ByteOrder::e_HOST);
#endif
        // Load into the specified 'dstVector' the result of converting the
        // specified UTF-8 'srcString' to its UTF-16 equivalent.  Optionally
        // specify 'numCodePointsWritten', which (if not 0) indicates the
        // location of the modifiable variable into which the number of UTF-16
        // code points (including the null terminator) written is to be loaded.
        // Optionally specify an 'errorWord' to be substituted (if not 0) for
        // invalid encodings in the input string.  Invalid encodings are
        // multi-byte encoding parts out of sequence, non-minimal UTF-8
        // encodings, or code points outside the ranges that UTF-16 can validly
        // encode (in the range '[ 1 .. 0xd7ff ]' or '[ 0xe000 .. 0x10ffff ]').
        // If 'errorWord' is 0, invalid input is ignored (i.e., produces no
        // corresponding output).  Optionally specify 'byteOrder' to indicate
        // the byte order of the UTF-16 output; if 'byteOrder' is not
        // specified, the output is assumed to be in host byte order.  Any
        // previous contents of the destination are discarded.  Return 0 on
        // success and 'CharConvertStatus::k_INVALID_INPUT_BIT' otherwise.  The
        // behavior is undefined unless 'errorWord' is either 0 or a valid
        // single-word encoded UTF-16 code point (in the range
        // '[ 1 .. 0xd7ff ]' or '[ 0xe000 .. 0xffff ]') and 'srcString' is
        // null-terminated when specified as a 'const char *'.  Note that one
        // code point can occupy multiple 16-bit words.  Also note that the
        // size of the result vector is always fitted to the null-terminated
        // result, including the terminating 0.  Also note that if 'srcString'
        // is a 'bslstl::StringRef', it may contain embedded null bytes that
        // will be translated to null words embedded in the output.

    static int utf8ToUtf16(
                          unsigned short          *dstBuffer,
                          bsl::size_t              dstCapacity,
                          const bsl::string_view&  srcString,
                          bsl::size_t             *numCodePointsWritten = 0,
                          bsl::size_t             *numWordsWritten      = 0,
                          unsigned short           errorWord            = '?',
                          ByteOrder::Enum          byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf16(
                          unsigned short           *dstBuffer,
                          bsl::size_t               dstCapacity,
                          const char               *srcString,
                          bsl::size_t              *numCodePointsWritten = 0,
                          bsl::size_t              *numWordsWritten      = 0,
                          unsigned short            errorWord            = '?',
                          ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
        // Load into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified UTF-8 'srcString' to its
        // UTF-16 equivalent.  Optionally specify 'numCodePointsWritten', which
        // (if not 0) indicates the location of the variable into which the
        // number of UTF-16 code points (including the null terminator) written
        // is to be loaded.  Optionally specify 'numWordsWritten', which (if
        // not 0) indicates the location of the modifiable variable into which
        // the number of 'short' *memory words* written (including the null
        // terminator) is to be loaded.  Optionally specify an 'errorWord' to
        // be substituted (if not 0) for invalid encodings in the input string.
        // Invalid encodings are multi-byte encoding parts out of sequence,
        // non-minimal UTF-8 encodings of code points, or code points outside
        // the ranges that UTF-16 can validly encode (in the range
        // '[ 1 .. 0xd7ff ]' or '[ 0xe000 .. 0x10ffff ]').  If 'errorWord' is
        // 0, invalid input sequences are ignored (i.e., produce no
        // corresponding output).  Optionally specify 'byteOrder' to indicate
        // the byte order of the UTF-16 output; if 'byteOrder' is not
        // specified, the output is assumed to be in host byte order.  Return 0
        // on success and a bit-wise or of the bits specified by
        // 'CharConvertStatus::Enum' otherwise to indicate that there were
        // invalid input sequences or if 'dstCapacity' was inadequate to store
        // the output.  If 'dstCapacity > 0' yet 'dstCapacity' specifies a
        // buffer too small to hold the output, the maximal null-terminated
        // prefix of the properly converted result string is loaded into
        // 'dstBuffer'.  The behavior is undefined unless 'dstBuffer' refers to
        // an array of at least 'dstCapacity' elements, 'errorWord' is either 0
        // or a valid single-word encoded UTF-16 code point (in the range
        // '[ 1 .. 0xd7ff ]' or '[ 0xe000 .. 0xffff ]'), and 'srcString' is
        // null-terminated when supplied as a 'const char *'.  Note that if
        // 'dstCapacity' is 0, '*dstBuffer' is not modified and this function
        // returns a value with 'CharConvertStatus::k_OUT_OF_SPACE_BIT' set and
        // 0 is written into '*numCodePointsWritten' and '*numWordsWritten' (if
        // those pointers are non-null), since there is insufficient space for
        // even a null terminator alone.  Also note that one code point can
        // occupy multiple 16-bit *words*, so that '*numWordsWritten' may be
        // greater than '*numCodePointsWritten', and therefore that an input
        // 'srcString' of 'dstCapacity' code points may not fit into
        // 'dstBuffer', however, an input 'srcString' of 'dstCapacity' bytes
        // (including null terminator, if present) will always fit (since the
        // UTF-8 encoding of a code point requires at least as many bytes as
        // the UTF-16 encoding requires words).  Also note that if 'srcString'
        // is a 'bslstl::StringRef', it may contain embedded null bytes that
        // will be translated to null words embedded in the output.

    static int utf8ToUtf16(
                          wchar_t                 *dstBuffer,
                          bsl::size_t              dstCapacity,
                          const bsl::string_view&  srcString,
                          bsl::size_t             *numCodePointsWritten = 0,
                          bsl::size_t             *numWordsWritten      = 0,
                          wchar_t                  errorWord            = '?',
                          ByteOrder::Enum          byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf16(
                          wchar_t                  *dstBuffer,
                          bsl::size_t               dstCapacity,
                          const char               *srcString,
                          bsl::size_t              *numCodePointsWritten = 0,
                          bsl::size_t              *numWordsWritten      = 0,
                          wchar_t                   errorWord            = '?',
                          ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
    static int utf8ToUtf16(
                          char16_t                *dstBuffer,
                          bsl::size_t              dstCapacity,
                          const bsl::string_view&  srcString,
                          bsl::size_t             *numCodePointsWritten = 0,
                          bsl::size_t             *numWordsWritten      = 0,
                          char16_t                 errorChar            = '?',
                          ByteOrder::Enum          byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf16(
                          char16_t                 *dstBuffer,
                          bsl::size_t               dstCapacity,
                          const char               *srcString,
                          bsl::size_t              *numCodePointsWritten = 0,
                          bsl::size_t              *numWordsWritten      = 0,
                          char16_t                  errorChar            = '?',
                          ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
#endif
        // Load into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified UTF-8 'srcString' to its
        // UTF-16 equivalent.  Optionally specify 'numCodePointsWritten', which
        // (if not 0) indicates the location of the variable into which the
        // number of UTF-16 code points (including the terminating 0) written
        // is to be loaded.  Optionally specify 'numWordsWritten', which (if
        // not 0) indicates the location of the modifiable variable into which
        // the number of 'short' *memory words* written (including the null
        // terminator) is to be loaded.  Optionally specify an 'errorWord' to
        // be substituted (if not 0) for invalid encodings in the input string.
        // Invalid encodings are multi-byte encoding parts out of sequence,
        // non-minimal UTF-8 encodings of code points, or code points outside
        // the ranges that UTF-16 can validly encode (in the range
        // '[ 1 .. 0xd7ff ]' or '[ 0xde00 .. 0x10ffff ]').  Optionally specify
        // 'byteOrder' to indicate the byte order of the UTF-16 output; if
        // 'byteOrder' is not specified, the output is assumed to be in host
        // byte order.  If 'errorWord' is 0, invalid input sequences are
        // ignored (i.e., produce no corresponding output).  Return 0 on
        // success and a bit-wise or of the bits specified by
        // 'CharConvertStatus::Enum' otherwise to indicate that there were
        // invalid sequences or if 'dstCapacity' was inadequate to store the
        // output.  If 'dstCapacity > 0' yet 'dstCapacity' specifies a buffer
        // too small to hold the output, the maximal null-terminated prefix of
        // the properly converted result string is loaded into 'dstBuffer'.
        // The behavior is undefined unless 'dstBuffer', if specified, refers
        // to an array of at least 'dstCapacity' elements, 'errorWord' is
        // either 0 or a valid single-word encoded UTF-16 code point (in the
        // range '[ 1 .. 0xd7ff ]' or '[ 0xe000 .. 0xffff ]'), and 'srcString'
        // is null-terminated if supplied as a 'const char *'.  Note that if
        // 'dstCapacity' is 0, '*dstBuffer' is not modified and this function
        // returns a value with 'CharConvertStatus::k_OUT_OF_SPACE_BIT' set and
        // 0 is written into '*numCodePointsWritten' and '*numWordsWritten' (if
        // those pointers are non-null), since there is insufficient space for
        // even a null terminator alone.  Also note that one code point can
        // occupy multiple 16-bit words, so that '*numWordsWritten' may be
        // greater than '*numCodePointsWritten', and therefore that an input
        // 'srcString' of 'dstCapacity' code points may not fit into
        // 'dstBuffer'.  However, an input 'srcString' of 'dstCapacity' bytes
        // (including terminating 0, if present) will always fit (since the
        // UTF-8 encoding of a code point requires at least as many bytes as
        // the UTF-16 encoding requires words).  Also note that if 'srcString'
        // is a 'bslstl::StringRef', it may contain embedded null bytes that
        // will be translated to null words embedded in the output.

                        // -- UTF-16 to UTF-8 Methods

    static bsl::size_t computeRequiredUtf8Bytes(
                                              const wchar_t        *srcBuffer,
                                              const wchar_t        *endPtr = 0,
                                              ByteOrder::Enum       byteOrder =
                                                            ByteOrder::e_HOST);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
    static bsl::size_t computeRequiredUtf8Bytes(
                                              const char16_t       *srcBuffer,
                                              const char16_t       *endPtr = 0,
                                              ByteOrder::Enum       byteOrder =
                                                            ByteOrder::e_HOST);
#endif
    static bsl::size_t computeRequiredUtf8Bytes(
                                              const unsigned short *srcBuffer,
                                              const unsigned short *endPtr = 0,
                                              ByteOrder::Enum       byteOrder =
                                                            ByteOrder::e_HOST);
        // Return the length needed in bytes, for a buffer to hold the
        // null-terminated UTF-8 string translated from the specified UTF-16
        // string 'srcBuffer' (including the terminating '\0' in the returned
        // count).  Optionally specify 'endPtr', referring to one past the last
        // input character.  If 'endPtr' is not supplied, or is 0, treat
        // 'srcBuffer' as 0 terminated.  Optionally specify 'byteOrder'
        // indicating the byte order of 'srcBuffer'; if 'byteOrder' is not
        // supplied, the host byte order is used.  Note that this function will
        // return the size 'utf16ToUtf8' will require, assuming the 'errorByte'
        // argument to 'utf16ToUtf8' is non-zero.

    static int utf16ToUtf8(bsl::string          *dstString,
                           const unsigned short *srcString,
                           bsl::size_t          *numCodePointsWritten = 0,
                           char                  errorByte            = '?',
                           ByteOrder::Enum       byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(std::string          *dstString,
                           const unsigned short *srcString,
                           bsl::size_t          *numCodePointsWritten = 0,
                           char                  errorByte            = '?',
                           ByteOrder::Enum       byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(
                          std::pmr::string     *dstString,
                          const unsigned short *srcString,
                          bsl::size_t          *numCodePointsWritten = 0,
                          char                  errorByte            = '?',
                          ByteOrder::Enum       byteOrder = ByteOrder::e_HOST);
#endif
    static int utf16ToUtf8(
                          bsl::string          *dstString,
                          const unsigned short *srcString,
                          bsl::size_t           srcLengthInWords,
                          bsl::size_t          *numCodePointsWritten = 0,
                          char                  errorByte            = '?',
                          ByteOrder::Enum       byteOrder = ByteOrder::e_HOST);
    static int utf16ToUtf8(
                          std::string          *dstString,
                          const unsigned short *srcString,
                          bsl::size_t           srcLengthInWords,
                          bsl::size_t          *numCodePointsWritten = 0,
                          char                  errorByte            = '?',
                          ByteOrder::Enum       byteOrder = ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(
                          std::pmr::string     *dstString,
                          const unsigned short *srcString,
                          bsl::size_t           srcLengthInWords,
                          bsl::size_t          *numCodePointsWritten = 0,
                          char                  errorByte            = '?',
                          ByteOrder::Enum       byteOrder = ByteOrder::e_HOST);
#endif
    static int utf16ToUtf8(
                      bsl::string              *dstString,
                      const bsl::wstring_view&  srcString,
                      bsl::size_t              *numCodePointsWritten = 0,
                      char                      errorByte            = '?',
                      ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(
                      std::string              *dstString,
                      const bsl::wstring_view&  srcString,
                      bsl::size_t              *numCodePointsWritten = 0,
                      char                      errorByte            = '?',
                      ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(
                      std::pmr::string         *dstString,
                      const bsl::wstring_view&  srcString,
                      bsl::size_t              *numCodePointsWritten = 0,
                      char                      errorByte            = '?',
                      ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
#endif
    static int utf16ToUtf8(bsl::string     *dstString,
                           const wchar_t   *srcString,
                           bsl::size_t     *numCodePointsWritten = 0,
                           char             errorByte            = '?',
                           ByteOrder::Enum  byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(std::string     *dstString,
                           const wchar_t   *srcString,
                           bsl::size_t     *numCodePointsWritten = 0,
                           char             errorByte            = '?',
                           ByteOrder::Enum  byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(std::pmr::string *dstString,
                           const wchar_t    *srcString,
                           bsl::size_t      *numCodePointsWritten = 0,
                           char              errorByte            = '?',
                           ByteOrder::Enum   byteOrder            =
                                                            ByteOrder::e_HOST);
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
    static int utf16ToUtf8(
                      bsl::string                *dstString,
                      const bsl::u16string_view&  srcString,
                      bsl::size_t                *numCodePointsWritten = 0,
                      char                        errorByte            = '?',
                      ByteOrder::Enum             byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(
                      std::string                *dstString,
                      const bsl::u16string_view&  srcString,
                      bsl::size_t                *numCodePointsWritten = 0,
                      char                        errorByte            = '?',
                      ByteOrder::Enum             byteOrder            =
                                                            ByteOrder::e_HOST);
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(
                      std::pmr::string           *dstString,
                      const bsl::u16string_view&  srcString,
                      bsl::size_t                *numCodePointsWritten = 0,
                      char                        errorByte            = '?',
                      ByteOrder::Enum             byteOrder            =
                                                            ByteOrder::e_HOST);
# endif
    static int utf16ToUtf8(bsl::string     *dstString,
                           const char16_t  *srcString,
                           bsl::size_t     *numCodePointsWritten = 0,
                           char             errorByte            = '?',
                           ByteOrder::Enum  byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(std::string     *dstString,
                           const char16_t  *srcString,
                           bsl::size_t     *numCodePointsWritten = 0,
                           char             errorByte            = '?',
                           ByteOrder::Enum  byteOrder            =
                                                            ByteOrder::e_HOST);
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(std::pmr::string *dstString,
                           const char16_t   *srcString,
                           bsl::size_t      *numCodePointsWritten = 0,
                           char              errorByte            = '?',
                           ByteOrder::Enum   byteOrder            =
                                                            ByteOrder::e_HOST);
# endif
#endif
        // Load into the specified 'dstString' the result of converting the
        // specified UTF-16 'srcString' to its UTF-8 equivalent.  Optionally
        // specify 'numCodePointsWritten', which (if not 0) indicates the
        // location of the modifiable variable into which the number of Unicode
        // code points written, including the null terminator, is to be loaded,
        // where one code point may occupy multiple bytes.  Optionally specify
        // an 'errorByte' to be substituted (if not 0) for invalid encodings in
        // the input string.  Invalid encodings are incomplete multi-word
        // encodings or parts of a two-word encoding out of their proper
        // sequence.  If 'errorByte' is 0, invalid input sequences are ignored
        // (i.e., produce no corresponding output).  Any previous contents of
        // the destination are discarded.  Optionally specify 'byteOrder' to
        // indicate the byte order of the UTF-16 input; if 'byteOrder' is not
        // specified, the input is assumed to be in host byte order.  Return 0
        // on success and 'CharConvertStatus::k_INVALID_INPUT_BIT' if one or
        // more invalid sequences were encountered in the input.  The behavior
        // is undefined unless 'errorByte' is either 0 or a valid single-byte
        // Unicode code point ('0 < errorByte < 0x80') and 'srcString' is
        // null-terminated if supplied as a 'const wchar_t *'.  Note that if
        // 'srcString' is a 'bslstl::StringRefWide', it may contain embedded 0
        // words that will be translated to null bytes embedded in the output.

    static int utf16ToUtf8(bsl::vector<char>    *dstVector,
                           const unsigned short *srcString,
                           bsl::size_t          *numCodePointsWritten = 0,
                           char                  errorByte            = '?',
                           ByteOrder::Enum       byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(std::vector<char>    *dstVector,
                           const unsigned short *srcString,
                           bsl::size_t          *numCodePointsWritten = 0,
                           char                  errorByte            = '?',
                           ByteOrder::Enum       byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(std::pmr::vector<char> *dstVector,
                           const unsigned short   *srcString,
                           bsl::size_t            *numCodePointsWritten = 0,
                           char                    errorByte            = '?',
                           ByteOrder::Enum         byteOrder            =
                                                            ByteOrder::e_HOST);
#endif
    static int utf16ToUtf8(bsl::vector<char>    *dstVector,
                           const unsigned short *srcString,
                           bsl::size_t           srcLengthInWords,
                           bsl::size_t          *numCodePointsWritten = 0,
                           char                  errorByte            = '?',
                           ByteOrder::Enum       byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(std::vector<char>    *dstVector,
                           const unsigned short *srcString,
                           bsl::size_t           srcLengthInWords,
                           bsl::size_t          *numCodePointsWritten = 0,
                           char                  errorByte            = '?',
                           ByteOrder::Enum       byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(std::pmr::vector<char> *dstVector,
                           const unsigned short   *srcString,
                           bsl::size_t             srcLengthInWords,
                           bsl::size_t            *numCodePointsWritten = 0,
                           char                    errorByte            = '?',
                           ByteOrder::Enum         byteOrder            =
                                                            ByteOrder::e_HOST);
#endif
        // Load into the specified 'dstVector' the null-terminated result of
        // converting the specified UTF-16 '*srcString' to its UTF-8
        // equivalent.  Optionally specify 'srcLengthInWords', the number of
        // 'unsigned short's of input.  If 'srcLengthInWords' is not specified,
        // the input must be terminated by a null word.  Optionally specify
        // 'numCodePointsWritten', which (if not 0) indicates the location of
        // the modifiable variable into which the number of Unicode code points
        // written, including the null terminator, is to be loaded, where one
        // code point may occupy multiple bytes.  Optionally specify an
        // 'errorByte' to be substituted (if not 0) for invalid encodings in
        // the input string.  Invalid encodings are incomplete multi-word
        // encodings or parts of a two-word encoding out of their proper
        // sequence.  If 'errorByte' is 0, invalid input sequences are ignored
        // (i.e., produce no corresponding output).  Optionally specify
        // 'byteOrder' to indicate the byte order of the UTF-16 input; if
        // 'byteOrder' is not specified, the input is assumed to be in host
        // byte order.  Any previous contents of the destination are discarded.
        // Return 0 on success and 'CharConvertStatus::k_INVALID_INPUT_BIT' if
        // one or more invalid sequences were encountered in the input.  The
        // behavior is undefined unless either 'srcLengthInWords' is passed or
        // 'srcString' is null-terminated, and 'errorByte' is either 0 or a
        // valid single-byte Unicode code point ('0 < errorByte < 0x80').

    static int utf16ToUtf8(
                      bsl::vector<char>        *dstVector,
                      const bsl::wstring_view&  srcString,
                      bsl::size_t              *numCodePointsWritten = 0,
                      char                      errorByte            = '?',
                      ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(
                      std::vector<char>        *dstVector,
                      const bsl::wstring_view&  srcString,
                      bsl::size_t              *numCodePointsWritten = 0,
                      char                      errorByte            = '?',
                      ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(
                      std::pmr::vector<char>   *dstVector,
                      const bsl::wstring_view&  srcString,
                      bsl::size_t              *numCodePointsWritten = 0,
                      char                      errorByte            = '?',
                      ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
#endif
    static int utf16ToUtf8(
                      bsl::vector<char>            *dstVector,
                      const wchar_t                *srcString,
                      bsl::size_t                  *numCodePointsWritten = 0,
                      char                          errorByte            = '?',
                      ByteOrder::Enum               byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(
                      std::vector<char>            *dstVector,
                      const wchar_t                *srcString,
                      bsl::size_t                  *numCodePointsWritten = 0,
                      char                          errorByte            = '?',
                      ByteOrder::Enum               byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(
                      std::pmr::vector<char>       *dstVector,
                      const wchar_t                *srcString,
                      bsl::size_t                  *numCodePointsWritten = 0,
                      char                          errorByte            = '?',
                      ByteOrder::Enum               byteOrder            =
                                                            ByteOrder::e_HOST);
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
    static int utf16ToUtf8(
                      bsl::vector<char>          *dstVector,
                      const bsl::u16string_view&  srcString,
                      bsl::size_t                *numCodePointsWritten = 0,
                      char                        errorByte            = '?',
                      ByteOrder::Enum             byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(
                      std::vector<char>          *dstVector,
                      const bsl::u16string_view&  srcString,
                      bsl::size_t                *numCodePointsWritten = 0,
                      char                        errorByte            = '?',
                      ByteOrder::Enum             byteOrder            =
                                                            ByteOrder::e_HOST);
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(
                      std::pmr::vector<char>     *dstVector,
                      const bsl::u16string_view&  srcString,
                      bsl::size_t                *numCodePointsWritten = 0,
                      char                        errorByte            = '?',
                      ByteOrder::Enum             byteOrder            =
                                                            ByteOrder::e_HOST);
# endif
    static int utf16ToUtf8(
                      bsl::vector<char>            *dstVector,
                      const char16_t               *srcString,
                      bsl::size_t                  *numCodePointsWritten = 0,
                      char                          errorByte            = '?',
                      ByteOrder::Enum               byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(
                      std::vector<char>            *dstVector,
                      const char16_t               *srcString,
                      bsl::size_t                  *numCodePointsWritten = 0,
                      char                          errorByte            = '?',
                      ByteOrder::Enum               byteOrder            =
                                                            ByteOrder::e_HOST);
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf16ToUtf8(
                      std::pmr::vector<char>       *dstVector,
                      const char16_t               *srcString,
                      bsl::size_t                  *numCodePointsWritten = 0,
                      char                          errorByte            = '?',
                      ByteOrder::Enum               byteOrder            =
                                                            ByteOrder::e_HOST);
# endif
#endif
        // Load into the specified 'dstVector' the null-terminated result of
        // converting the specified UTF-16 'srcString' to its UTF-8 equivalent.
        // Optionally specify 'numCodePointsWritten', which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of Unicode code points written, including the null
        // terminator, is to be loaded, where one code point may occupy
        // multiple bytes.  Optionally specify an 'errorByte' to be substituted
        // (if not 0) for invalid encodings in the input string.  Invalid
        // encodings are incomplete multi-word encodings or parts of a two-word
        // encoding out of their proper sequence.  If 'errorByte' is 0, invalid
        // input sequences are ignored (i.e., produce no corresponding output).
        // Optionally specify 'byteOrder' to indicate the byte order of the
        // UTF-16 input; if 'byteOrder' is not specified, the input is assumed
        // to be in host byte order.  Any previous contents of the destination
        // are discarded.  Return 0 on success and
        // 'CharConvertStatus::k_INVALID_INPUT_BIT' if one or more invalid
        // sequences were encountered in the input.  The behavior is undefined
        // unless 'errorByte' is either 0 or a valid single-byte Unicode code
        // point ('0 < errorByte < 0x80') and 'srcString' is null-terminated if
        // supplied as a 'const wchar_t *'.  Note that if 'srcString' is a
        // 'bslstl::StringRef', it may contain embedded 0 words that will be
        // translated to null bytes embedded in the output.

    static int utf16ToUtf8(char                 *dstBuffer,
                           bsl::size_t           dstCapacity,
                           const unsigned short *srcString,
                           bsl::size_t          *numCodePointsWritten = 0,
                           bsl::size_t          *numBytesWritten      = 0,
                           char                  errorByte            = '?',
                           ByteOrder::Enum       byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(char                 *dstBuffer,
                           bsl::size_t           dstCapacity,
                           const unsigned short *srcString,
                           bsl::size_t           srcLengthInWords,
                           bsl::size_t          *numCodePointsWritten = 0,
                           bsl::size_t          *numBytesWritten      = 0,
                           char                  errorByte            = '?',
                           ByteOrder::Enum       byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(
                      char                     *dstBuffer,
                      bsl::size_t               dstCapacity,
                      const bsl::wstring_view&  srcString,
                      bsl::size_t              *numCodePointsWritten = 0,
                      bsl::size_t              *numBytesWritten      = 0,
                      char                      errorByte            = '?',
                      ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(
                      char                         *dstBuffer,
                      bsl::size_t                   dstCapacity,
                      const wchar_t                *srcString,
                      bsl::size_t                  *numCodePointsWritten = 0,
                      bsl::size_t                  *numBytesWritten      = 0,
                      char                          errorByte            = '?',
                      ByteOrder::Enum               byteOrder            =
                                                            ByteOrder::e_HOST);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
    static int utf16ToUtf8(
                      char                       *dstBuffer,
                      bsl::size_t                 dstCapacity,
                      const bsl::u16string_view&  srcString,
                      bsl::size_t                *numCodePointsWritten = 0,
                      bsl::size_t                *numBytesWritten      = 0,
                      char                        errorByte            = '?',
                      ByteOrder::Enum             byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf16ToUtf8(
                      char                         *dstBuffer,
                      bsl::size_t                   dstCapacity,
                      const char16_t               *srcString,
                      bsl::size_t                  *numCodePointsWritten = 0,
                      bsl::size_t                  *numBytesWritten      = 0,
                      char                          errorByte            = '?',
                      ByteOrder::Enum               byteOrder            =
                                                            ByteOrder::e_HOST);
#endif
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified UTF-16 'srcString' to its
        // UTF-8 equivalent.  Optionally specify 'numCodePointsWritten', which
        // (if not 0) indicates the location of the modifiable variable into
        // which the number of Unicode code points (including the terminating
        // 0, if any) written is to be loaded, where one code point can occupy
        // multiple bytes.  Optionally specify 'numBytesWritten', which (if not
        // 0) indicates the location of the modifiable variable into which the
        // number of bytes written (including the null terminator, if any) is
        // to be loaded.  Optionally specify an 'errorByte' to be substituted
        // (if not 0) for invalid encodings in the input string.  Invalid
        // encodings are incomplete multi-word encodings or parts of a two-word
        // encoding out of their proper sequence.  If 'errorByte' is 0, invalid
        // input sequences are ignored (i.e., produce no corresponding output).
        // Optionally specify 'byteOrder' to indicate the byte order of the
        // UTF-16 input; if 'byteOrder' is not specified, the input is assumed
        // to be in host byte order.  Return 0 on success and a bitwise-or of
        // the flags defined by 'CharConvertStatus::Enum' otherwise.
        // 'CharConvertStatus::k_INVALID_INPUT_BIT' will be set if one or more
        // invalid sequences were encountered in the input, and
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' will be set if the output
        // space was exhausted before conversion was complete.  The behavior is
        // undefined unless 'dstBuffer' refers to an array of at least
        // 'dstCapacity' elements, 'errorByte' is either 0 or a valid
        // single-byte Unicode code point ('0 < errorByte < 0x80'), and
        // 'srcString' is null-terminated if supplied as a pointer.  Note that
        // if 'dstCapacity' is 0, this function returns
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' set and 0 is written into
        // '*numCodePointsWritten' and '*numBytesWritten' (if those pointers
        // are non-null), since there is insufficient space for even a null
        // terminator alone.  Also note that since UTF-8 is a variable-length
        // encoding, 'numBytesWritten' may be up to four times
        // 'numCodePointsWritten', and therefore that an input 'srcString' of
        // 'dstCapacity' code points (including the terminating 0, if present)
        // may not fit into 'dstBuffer'.  A one-word (two-byte) UTF-16 code
        // point will require one to three UTF-8 octets (bytes); a two-word
        // (four-byte) UTF-16 code point will always require four UTF-8 octets.
        // Also note that the amount of room needed will vary with the contents
        // of the data and the language being translated, but never will the
        // number of bytes output exceed three times the number of words input.
        // Also note that, if 'dstCapacity > 0', then, after completion,
        // 'strlen(dstBuffer) + 1 == *numBytesWritten'.  Also note that if
        // 'srcString' is a 'bslstl::StringRef', it may contain embedded 0
        // words that will be translated to null bytes embedded in the output.
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
