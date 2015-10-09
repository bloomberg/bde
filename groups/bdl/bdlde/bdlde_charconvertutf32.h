// bdlde_charconvertutf32.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_CHARCONVERTUTF32
#define INCLUDED_BDLDE_CHARCONVERTUTF32

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide fast, safe conversion between UTF-8 encoding and UTF-32.
//
//@CLASSES:
//  bdlde::CharConvertUtf32: namespace for conversion between UTF-8 and UTF-32
//
//@DESCRIPTION: This component provides a 'struct', 'bdlde::CharConvertUtf32',
// that provides a suite of static functions supporting the *fast* conversion
// of UTF-8 data to UTF-32, and vice versa.  UTF-8 input can take the form of
// null-terminated "C" strings or 'bslstl::StringRef's, while UTF-32 input can
// only take the form of null-terminated buffers of 'unsigned int'.  Output can
// be to STL vectors, 'bsl::string's (in the case of UTF-8), and fixed-length
// buffers.  Invalid byte sequences and characters forbidden by either encoding
// are removed and (optionally) replaced by a character provided by the caller.
// The byte order of the UTF-32 input or output can be specified via the
// optional 'byteOrder' argument, which is assumed to be host byte order if not
// specified.  The byte or word count and character count that are optionally
// returned through pointer arguments include the terminating null character,
// if any, in their count.
//
///History and Motivation
///----------------------
// UTF-8 is a character encoding that allows 32-bit character sets like Unicode
// to be represented using null-terminated (8-bit) byte strings (NTBS), while
// allowing "standard ASCII" strings to be used "as-is".  Note that UTF-8 is
// described in detail in RFC 3629 (http://www.ietf.org/rfc/rfc3629.txt).
//
// UTF-32 is simply a name for storing raw Unicode values as sequential
// 'unsigned int' values in memory.
//
// Valid Unicode values are in the ranges '[ 1 .. 0xd7ff ]' and
// '[ 0xe000 .. 0x10ffff ]'.  The value '0' is used to terminate sequences.
//
// The functions here that translate to fixed buffers make a single pass
// through the data.  The functions that translate to 'bsl::string's and
// 'bsl::vector's, however, like the 'glib' conversion routines, make two
// passes: a size estimation pass, after which the output container is sized
// appropriately, and then the translation pass.
//
// The methods that output to a 'vector' or 'string' will all grow the output
// object as necessary to fit the data, and in the end will exactly resize the
// object to the output (including the terminating 0 for 'vector', not
// including it for 'string').  The resizing will not affect the capacity.
//
// Non-minimal UTF-8 encodings of characters are reported as errors.  Octets
// and post-conversion characters in the forbidden ranges are treated as errors
// and removed if 0 is specified as 'errorCharacter', or replaced with
// 'errorCharacter' otherwise.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example: Round-Trip Multi-Lingual Conversion
///--------------------------------------------
// The following snippets of code illustrate a typical use of the
// 'bdlde::CharConvertUtf32' struct's utility functions, first converting from
// UTF-8 to UTF-32, and then converting back to make sure the round trip
// returns the same value.
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
// Next, we declare the vector where our UTF-32 output will go, and a variable
// into which the number of characters (characters, not bytes or words) written
// will be stored.  It is not necessary to initialize 'utf32CharsWritten':
//..
//  bsl::vector<unsigned int> v32;
//..
// Note that it is a waste of time to 'v32.reserve(sizeof(utf8MultiLang))'; it
// is entirely redundant -- 'v32' will automatically be grown to the correct
// size.  Also note that if 'v32' were not empty, that would not be a problem
// -- any contents will be discarded.
//
// Then, we do the translation to 'UTF-32':
//..
//  int retVal = bdlde::CharConvertUtf32::utf8ToUtf32(&v32,
//                                                    utf8MultiLang);
//
//  assert(0 == retVal);        // verify success
//  assert(0 == v32.back());    // verify null terminated
//..
// Next, we verify that the number of characters (characters, not bytes or
// words) that was returned is correct.  Note that in UTF-32, the number of
// Unicode characters written is the same as the number of 32-bit words
// written:
//..
//  enum { EXPECTED_CHARS_WRITTEN =
//                  NUM_ASCII_CHARS + NUM_GREEK_CHARS + NUM_CHINESE_CHARS +
//                  NUM_HINDI_CHARS + NUM_QUAD_CHARS  + 1 };
//  assert(EXPECTED_CHARS_WRITTEN == v32.size());
//..
// Next, we calculate and confirm the difference between the number of UTF-32
// words output and the number of bytes input.  The ASCII characters will take
// 1 32-bit word apiece, the Greek characters are double octets that will
// become single 'unsigned int' values, the Chinese characters are encoded as
// UTF-8 triple octets that will turn into single 32-bit words, the same for
// the Hindi characters, and the quad characters are quadruple octets that will
// turn into single 'unsigned int' words:
//..
//  enum { SHRINKAGE =
//                    NUM_ASCII_CHARS   * (1-1) + NUM_GREEK_CHARS * (2-1) +
//                    NUM_CHINESE_CHARS * (3-1) + NUM_HINDI_CHARS * (3-1) +
//                    NUM_QUAD_CHARS    * (4-1) };
//
//  assert(v32.size() == sizeof(utf8MultiLang) - SHRINKAGE);
//..
// Then, we go on to do the reverse 'utf32ToUtf8' transform to turn it back
// into UTF-8, and we should get a result identical to our original input.
// Declare a 'bsl::string' for our output, and a variable to count the number
// of characters (characters, not bytes or words) translated:
//..
//  bsl::string s;
//  bsl::size_t utf8CharsWritten;
//..
// Again, note that it would be a waste of time for the caller to 'resize' or
// 'reserve' 's'; it will be automatically 'resize'd by the translator to the
// right length.
//
// Now, we do the reverse transform:
//..
//  retVal = bdlde::CharConvertUtf32::utf32ToUtf8(&s,
//                                                v32.begin(),
//                                                &utf8CharsWritten);
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
//  assert(v32.size()             == utf8CharsWritten);
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
                          // struct CharConvertUtf32
                          // =======================

struct CharConvertUtf32 {
    // This 'struct' provides a namespace for a suite of static functions to
    // convert character buffers between UTF-8 and UTF-32.  Byte Order Mark
    // (BOM) characters are neither generated nor recognized as special, and
    // thus may be incorrect for the actual byte order of output.  If a BOM is
    // present in the input, it will be translated, whether correct ('0xfeff')
    // or incorrect ('0xfffe'), into the output without any special handling.

                        // -- UTF-8 to UTF-32 Methods

    // CLASS METHODS
    static int utf8ToUtf32(bsl::vector<unsigned int> *dstVector,
                           const char                *srcString,
                           unsigned int               errorCharacter = '?',
                           ByteOrder::Enum            byteOrder      =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf32(bsl::vector<unsigned int> *dstVector,
                           const bslstl::StringRef&   srcString,
                           unsigned int               errorCharacter = '?',
                           ByteOrder::Enum            byteOrder      =
                                                            ByteOrder::e_HOST);
        // Load into the specified 'dstVector' the result of converting the
        // specified UTF-8 'srcString' to its UTF-32 equivalent.  Optionally
        // specify 'errorCharacter' to be substituted, if not 0, for invalid
        // encodings in the input string.  Optionally specify 'byteOrder' to
        // indicate the byte order of the UTF-32 output; if 'byteOrder' is not
        // specified, the output is assumed to be in host byte order.  Return 0
        // on success and 'CharConvertStatus::k_INVALID_CHARS_BIT' otherwise.
        // Invalid encodings are multi-byte encoding parts out of sequence,
        // non-minimal UTF-8 encodings of characters, UTF-8 encodings more than
        // four bytes in length, or characters outside the ranges which UTF-32
        // can validly encode (i.e., '[ 1 .. 0xd7ff ]' and
        // '[ 0xe000 .. 0x10ffff ]').  If 'errorCharacter' is 0, invalid input
        // characters are ignored (i.e., produce no corresponding output
        // characters).  Any previous contents of the destination are
        // discarded.  The behavior is undefined unless 'srcString' is
        // null-terminated when specified as a 'const char *', and unless
        // 'errorCharacter' is either 0 or a valid Unicode character.  Note
        // that one *character* always occupies one 32-bit *word* of output;
        // there is no 'numCharsWritten' argument since, after the call,
        // 'dstVector->size()' will equal the number of characters written.
        // Also note that when the input is a 'bslstl::StringRef', it may
        // contain embedded nulls, which are translated to zeroes in the
        // output.  Also note that 'errorCharacter' is assumed to be in host
        // byte order.

    static int utf8ToUtf32(unsigned int             *dstBuffer,
                           bsl::size_t               dstCapacity,
                           const char               *srcString,
                           bsl::size_t              *numCharsWritten = 0,
                           unsigned int              errorCharacter  = '?',
                           ByteOrder::Enum           byteOrder       =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf32(unsigned int             *dstBuffer,
                           bsl::size_t               dstCapacity,
                           const bslstl::StringRef&  srcString,
                           bsl::size_t              *numCharsWritten = 0,
                           unsigned int              errorCharacter  = '?',
                           ByteOrder::Enum           byteOrder       =
                                                            ByteOrder::e_HOST);
        // Load into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified UTF-8 'srcString' to its
        // UTF-32 equivalent.  Optionally specify 'numCharsWritten' which (if
        // not 0) indicates the location of the variable into which the number
        // of Unicode characters (including the null terminator) written is to
        // be loaded.  Optionally specify 'errorCharacter' to be substituted
        // (if not 0) for invalid encodings in the input string.  Invalid
        // encodings are multi-byte encoding parts out of sequence, non-minimal
        // UTF-8 encodings of characters, UTF-8 encodings more than four bytes
        // in length, or characters outside the ranges which UTF-32 can validly
        // encode (i.e., '[ 1 .. 0xd7ff ]' and '[ 0xe000 .. 0x10ffff ]').  If
        // 'errorCharacter' is 0, invalid input characters are ignored (i.e.,
        // produce no corresponding output characters).  Optionally specify
        // 'byteOrder' to indicate the byte order of the UTF-32 output; if
        // 'byteOrder' is not specified, the output is assumed to be in host
        // byte order.  Return 0 on success and a bit-wise OR of the masks
        // defined by 'CharConvertStatus::Enum' otherwise, where
        // 'CharConvertStatus::k_INVALID_CHARS_BIT' will be set if one or more
        // invalid character sequences were encountered in the input, and
        // 'CharConvertStatus::BDEDE_OUT_OF_SPACE_BIT' will be set if the
        // output space was exhausted before conversion was complete.  If
        // 'dstCapacity > 0' yet 'dstCapacity' specifies a buffer too small to
        // hold the output, the maximal null-terminated prefix of the properly
        // converted result string is loaded into 'dstBuffer'.  The behavior is
        // undefined unless 'dstBuffer' refers to an array of at least
        // 'dstCapacity' elements, 'srcString', if specified as a
        // 'const char *', is null-terminated, and 'errorCharacter' is either 0
        // or a valid single-word encoded UTF-32 character (in the range
        // '[ 1 .. 0xd7ff ]' or '[ 0xe000 .. 0x10ffff ]').  Note that if
        // 'dstCapacity' is 0, '*dstBuffer' is not modified and this function
        // returns a value with 'CharConvertStatus::BDEDE_OUT_OF_SPACE_BIT' set
        // and 0 is written into '*numCharsWritten' (if that pointer is not 0),
        // since there is insufficient space for even a null terminator alone.
        // Also note that one Unicode *character* always occupies one 32-bit
        // *word* in UTF-32, but may occupy more than one *byte* of UTF-8, so
        // that '*numCharsWritten' equals the number of *words* written.  Also
        // note that 'errorCharacter' is assumed to be in host byte order.

                        // -- UTF-32 to UTF-8 Methods

    static int utf32ToUtf8(bsl::string           *dstString,
                           const unsigned int    *srcString,
                           bsl::size_t           *numCharsWritten = 0,
                           unsigned char          errorCharacter  = '?',
                           ByteOrder::Enum        byteOrder       =
                                                            ByteOrder::e_HOST);
        // Load into the specified 'dstString' the result of converting the
        // specified null-terminated UTF-32 'srcString' to its UTF-8
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of Unicode *characters* written, including the null
        // terminator, is to be loaded.  Optionally specify 'errorCharacter' to
        // be substituted (if not 0) for invalid encodings in the input string.
        // Invalid encodings are illegal Unicode values (in the range
        // '[ 0xd800 .. 0xdfff ]' or above '0x10ffff').  If 'errorCharacter' is
        // 0, invalid input characters are ignored (i.e., produce no
        // corresponding output characters).  Optionally specify 'byteOrder' to
        // indicate the byte order of the UTF-32 input; if 'byteOrder' is not
        // specified, the input is assumed to be in host byte order.  Any
        // previous contents of the destination are discarded.  Return 0 on
        // success and 'CharConvertStatus::k_INVALID_CHARS_BIT' if one or more
        // invalid character sequences were encountered in the input.  The
        // behavior is undefined unless 'srcString' is null-terminated and
        // 'errorCharacter' is either 0 or a valid single-byte encoded UTF-8
        // character ('0 < errorCharacter < 0x80').  Note that one Unicode
        // *character* can occupy multiple *bytes* of UTF-8.  Also note that
        // the string length will be sized to the length of the output such
        // that 'strlen(dstString->c_str()) == dstString->length()'.

    static int utf32ToUtf8(bsl::vector<char>     *dstVector,
                           const unsigned int    *srcString,
                           bsl::size_t           *numCharsWritten = 0,
                           unsigned char          errorCharacter  = '?',
                           ByteOrder::Enum        byteOrder       =
                                                            ByteOrder::e_HOST);
        // Load into the specified 'dstVector' the result of converting the
        // specified null-terminated UTF-32 'srcString' to its UTF-8
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of Unicode *characters* written, including the null
        // terminator, is to be loaded.  Optionally specify 'errorCharacter' to
        // be substituted (if not 0) for invalid encodings in the input string.
        // Invalid encodings are illegal Unicode values (in the range
        // '[ 0xd800 .. 0xdfff ]' or above '0x10ffff').  If 'errorCharacter' is
        // 0, invalid input characters are ignored (i.e., produce no
        // corresponding output characters).  Optionally specify 'byteOrder' to
        // indicate the byte order of the UTF-32 input; if 'byteOrder' is not
        // specified, the input is assumed to be in host byte order.  Any
        // previous contents of the destination are discarded.  Return 0 on
        // success and 'CharConvertStatus::k_INVALID_CHARS_BIT' if one or more
        // invalid character sequences were encountered in the input.  The
        // behavior is undefined unless 'srcString' is null-terminated and
        // 'errorCharacter' is either 0 or a valid single-byte encoded UTF-8
        // character ('0 < errorCharacter < 0x80').  Note that one *character*
        // can occupy multiple *bytes* of UTF-8.  Also note that 'dstVector' is
        // sized to exactly fit the output so that
        // 'strlen(dstVector->begin()) == dstVector->size() - 1'.

    static int utf32ToUtf8(char                  *dstBuffer,
                           bsl::size_t            dstCapacity,
                           const unsigned int    *srcString,
                           bsl::size_t           *numCharsWritten = 0,
                           bsl::size_t           *numBytesWritten = 0,
                           unsigned char          errorCharacter  = '?',
                           ByteOrder::Enum        byteOrder       =
                                                            ByteOrder::e_HOST);
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-32
        // 'srcString' to its UTF-8 equivalent.  Optionally specify
        // 'numCharsWritten' which (if not 0) indicates the location of the
        // modifiable variable into which the number of Unicode *characters*
        // (including the terminating 0, if any) written is to be loaded.
        // Optionally specify 'numBytesWritten' which (if not 0) indicates the
        // location of the modifiable variable into which the number of bytes
        // written (including the null terminator, if any) is to be loaded.
        // Optionally specify 'errorCharacter' to be substituted (if not 0) for
        // invalid encodings in the input string.  Invalid encodings are
        // illegal Unicode values (in the range '[ 0xd800 .. 0xdfff ]' or above
        // '0x10ffff').  If 'errorCharacter' is 0, invalid input characters are
        // ignored (i.e., produce no corresponding output characters).
        // Optionally specify 'byteOrder' to indicate the byte order of the
        // UTF-32 input; if 'byteOrder' is not specified, the input is assumed
        // to be in host byte order.  Return 0 on success and a bit-wise OR of
        // the masks defined by 'CharConvertStatus::Enum' otherwise,
        // where 'CharConvertStatus::k_INVALID_CHARS_BIT' will be set if one or
        // more invalid character sequences were encountered in the input, and
        // 'CharConvertStatus::BDEDE_OUT_OF_SPACE_BIT' will be set if the
        // output space was exhausted before conversion was complete.  The
        // behavior is undefined unless 'dstBuffer' refers to an array of at
        // least 'dstCapacity' elements, 'srcString' is null-terminated, and
        // 'errorCharacter' is either 0 or a valid single-byte encoded UTF-8
        // character ('0 < errorCharacter < 0x80').  Note that one Unicode
        // *character* can occupy multiple UTF-8 *bytes*, but will be a single
        // UTF-32 *word*.  Also note that if 'dstCapacity' is 0, this function
        // returns 'CharConvertStatus::BDEDE_OUT_OF_SPACE_BIT' set and 0 is
        // written into '*numCharsWritten' and '*numBytesWritten' (if those
        // pointers are not zero), since there is insufficient space for even a
        // null terminator alone.  Also note that since UTF-8 is a
        // variable-length encoding, 'numBytesWritten' may be up to four times
        // 'numCharsWritten', and, therefore, an input 'srcString' of
        // 'dstCapacity' *characters* (including terminating 0) may not fit
        // into 'dstBuffer'.  Also note that the amount of room needed will
        // vary with the contents of the data and the language being
        // translated, but never will the number of *bytes* output exceed four
        // times the number of 32-bit *words* input.  Also note that, if
        // 'dstCapacity > 0', then, after completion,
        // 'strlen(dstBuffer) + 1 == *numBytesWritten'.
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
