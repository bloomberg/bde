// bdlde_charconvertutf32.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_CHARCONVERTUTF32
#define INCLUDED_BDLDE_CHARCONVERTUTF32

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide fast, safe conversion between UTF-8 encoding and UTF-32.
//
//@CLASSES:
//  bdlde::CharConvertUtf32: namespace for conversion between UTF-8 and UTF-32
//
//@DESCRIPTION: This component provides a 'struct', 'bdlde::CharConvertUtf32',
// that provides a suite of static functions supporting the *fast* conversion
// of UTF-8 data to UTF-32, and vice versa.  UTF-8 input can take the form of
// null-terminated "C" strings or 'bsl::string_view's, while UTF-32 input can
// only take the form of null-terminated buffers of 'unsigned int'.  Output can
// be to STL vectors, 'bsl::string's (in the case of UTF-8), and fixed-length
// buffers.  Invalid byte sequences and code points forbidden by either
// encoding are removed and (optionally) replaced by an error byte or word
// provided by the caller.  The byte order of the UTF-32 input or output can be
// specified via the optional 'byteOrder' argument, which is assumed to be host
// byte order if not specified.  The byte or word count and code point count
// that are optionally returned through pointer arguments include the
// terminating null byte or word.
//
///History and Motivation
///----------------------
// UTF-8 is a Unicode encoding that allows 32-bit Unicode to be represented
// using null-terminated (8-bit) byte strings, while allowing "standard ASCII"
// strings to be used "as-is".  Note that UTF-8 is described in detail in RFC
// 3629 (http://www.ietf.org/rfc/rfc3629.txt).
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
// Non-minimal UTF-8 encodings of code points are reported as errors.  Octets
// and post-conversion code points in the forbidden ranges are treated as
// errors and removed if 0 is specified as 'errorWord', or replaced with
// 'errorWord' otherwise.
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
// Next, we declare the vector where our UTF-32 output will go, and a variable
// into which the number of code points written will be stored.  It is not
// necessary to create a 'utf32CodePointsWritten' variable, since the number of
// code points will be the size of the vector when we are done.
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
// Next, we verify that the number of code points that was returned is correct.
// Note that in UTF-32, the number of Unicode code points written is the same
// as the number of 32-bit words written:
//..
//  enum { EXPECTED_CODE_POINTS_WRITTEN =
//                  NUM_ASCII_CODE_POINTS +
//                  NUM_GREEK_CODE_POINTS +
//                  NUM_CHINESE_CODE_POINTS +
//                  NUM_HINDI_CODE_POINTS +
//                  NUM_QUAD_CODE_POINTS  + 1 };
//  assert(EXPECTED_CODE_POINTS_WRITTEN == v32.size());
//..
// Next, we calculate and confirm the difference between the number of UTF-32
// words output and the number of bytes input.  The ASCII bytes will take 1
// 32-bit word apiece, the Greek code points are double octets that will become
// single 'unsigned int' values, the Chinese code points are encoded as UTF-8
// triple octets that will turn into single 32-bit words, the same for the
// Hindi code points, and the quad code points are quadruple octets that will
// turn into single 'unsigned int' words:
//..
//  enum { SHRINKAGE =
//                    NUM_ASCII_CODE_POINTS   * (1-1) +
//                    NUM_GREEK_CODE_POINTS   * (2-1) +
//                    NUM_CHINESE_CODE_POINTS * (3-1) +
//                    NUM_HINDI_CODE_POINTS   * (3-1) +
//                    NUM_QUAD_CODE_POINTS    * (4-1) };
//
//  assert(v32.size() == sizeof(utf8MultiLang) - SHRINKAGE);
//..
// Then, we go on to do the reverse 'utf32ToUtf8' transform to turn it back
// into UTF-8, and we should get a result identical to our original input.
// Declare a 'bsl::string' for our output, and a variable to count the number
// of code points translated:
//..
//  bsl::string s;
//  bsl::size_t codePointsWritten;
//..
// Again, note that it would be a waste of time for the caller to 'resize' or
// 'reserve' 'v32'; it will be automatically 'resize'd by the translator to the
// right length.
//
// Now, we do the reverse transform:
//..
//  retVal = bdlde::CharConvertUtf32::utf32ToUtf8(&s,
//                                                v32.begin(),
//                                                &codePointsWritten);
//..
// Finally, we verify that a successful status was returned, that the output of
// the reverse transform was identical to the original input, and that the
// number of code points translated was as expected:
//..
//  assert(0 == retVal);
//  assert(utf8MultiLang  == s);
//  assert(s.length() + 1 == sizeof(utf8MultiLang));
//
//  assert(EXPECTED_CODE_POINTS_WRITTEN == codePointsWritten);
//  assert(v32.size()                   == codePointsWritten);
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
                          // struct CharConvertUtf32
                          // =======================

struct CharConvertUtf32 {
    // This 'struct' provides a namespace for a suite of static functions to
    // convert buffers between UTF-8 and UTF-32.  Byte Order Mark (BOM) code
    // points are neither generated nor recognized as special, and thus may be
    // incorrect for the actual byte order of output.  If a BOM is present in
    // the input, it will be translated, whether correct ('0xfeff') or
    // incorrect ('0xfffe'), into the output without any special handling.

                              // UTF-8 to UTF-32 Methods

    // CLASS METHODS
    static int utf8ToUtf32(bsl::vector<unsigned int> *dstVector,
                           const char                *srcString,
                           unsigned int               errorWord = '?',
                           ByteOrder::Enum            byteOrder =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf32(std::vector<unsigned int> *dstVector,
                           const char                *srcString,
                           unsigned int               errorWord = '?',
                           ByteOrder::Enum            byteOrder =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf8ToUtf32(
                std::pmr::vector<unsigned int> *dstVector,
                const char                     *srcString,
                unsigned int                    errorWord = '?',
                ByteOrder::Enum                 byteOrder = ByteOrder::e_HOST);
#endif
    static int utf8ToUtf32(bsl::vector<unsigned int> *dstVector,
                           const bsl::string_view&    srcString,
                           unsigned int               errorWord = '?',
                           ByteOrder::Enum            byteOrder =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf32(std::vector<unsigned int> *dstVector,
                           const bsl::string_view&    srcString,
                           unsigned int               errorWord = '?',
                           ByteOrder::Enum            byteOrder =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf8ToUtf32(
                std::pmr::vector<unsigned int> *dstVector,
                const bsl::string_view&         srcString,
                unsigned int                    errorWord = '?',
                ByteOrder::Enum                 byteOrder = ByteOrder::e_HOST);
#endif
        // Load into the specified 'dstVector' the result of converting the
        // specified UTF-8 'srcString' to its UTF-32 equivalent.  Optionally
        // specify 'errorWord' to be substituted, if not 0, for invalid
        // encodings in the input string.  Optionally specify 'byteOrder' to
        // indicate the byte order of the UTF-32 output; if 'byteOrder' is not
        // specified, the output is assumed to be in host byte order.  Return 0
        // on success and 'CharConvertStatus::k_INVALID_INPUT_BIT' otherwise.
        // Invalid encodings are multi-byte encoding parts out of sequence,
        // non-minimal UTF-8 encodings, UTF-8 encodings more than four bytes in
        // length, or code points outside the ranges that UTF-32 can validly
        // encode (i.e., '[ 1 .. 0xd7ff ]' and '[ 0xe000 .. 0x10ffff ]').  If
        // 'errorWord' is 0, invalid input sequences are ignored (i.e., produce
        // no corresponding output).  Any previous contents of the destination
        // are discarded.  The behavior is undefined unless 'srcString' is
        // null-terminated when specified as a 'const char *', and unless
        // 'errorWord' is either 0 or a valid Unicode code point.  Note that
        // one code point always occupies one 32-bit *ord of output; there is
        // no 'numCodePointsWritten' argument since, after the call,
        // 'dstVector->size()' will equal the number of code points written.
        // Also note that when the input is a 'bsl::string_view', it may
        // contain embedded nulls, which are translated to zeroes in the
        // output.  Also note that 'errorWord' is assumed to be in host byte
        // order.

    static int utf8ToUtf32(
                          unsigned int             *dstBuffer,
                          bsl::size_t               dstCapacity,
                          const char               *srcString,
                          bsl::size_t              *numCodePointsWritten = 0,
                          unsigned int              errorWord            = '?',
                          ByteOrder::Enum           byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf8ToUtf32(
                       unsigned int            *dstBuffer,
                       bsl::size_t              dstCapacity,
                       const bsl::string_view&  srcString,
                       bsl::size_t             *numCodePointsWritten = 0,
                       unsigned int             errorWord            = '?',
                       ByteOrder::Enum          byteOrder = ByteOrder::e_HOST);
        // Load into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified UTF-8 'srcString' to its
        // UTF-32 equivalent.  Optionally specify 'numCodePointsWritten', which
        // (if not 0) indicates the location of the variable into which the
        // number of Unicode code points (including the null terminator)
        // written is to be loaded.  Optionally specify 'errorWord' to be
        // substituted (if not 0) for invalid encodings in the input string.
        // Invalid encodings are multi-byte encoding parts out of sequence,
        // non-minimal UTF-8 encodings, UTF-8 encodings more than four bytes in
        // length, or code points outside the ranges that UTF-32 can validly
        // encode (i.e., '[ 1 .. 0xd7ff ]' and '[ 0xe000 .. 0x10ffff ]').  If
        // 'errorWord' is 0, invalid input code points are ignored (i.e.,
        // produce no corresponding output).  Optionally specify 'byteOrder' to
        // indicate the byte order of the UTF-32 output; if 'byteOrder' is not
        // specified, the output is assumed to be in host byte order.  Return 0
        // on success and a bit-wise OR of the masks defined by
        // 'CharConvertStatus::Enum' otherwise, where
        // 'CharConvertStatus::k_INVALID_INPUT_BIT' will be set if one or more
        // invalid sequences were encountered in the input, and
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' will be set if the
        // output space was exhausted before conversion was complete.  If
        // 'dstCapacity > 0' yet 'dstCapacity' specifies a buffer too small to
        // hold the output, the maximal null-terminated prefix of the properly
        // converted result string is loaded into 'dstBuffer'.  The behavior is
        // undefined unless 'dstBuffer' refers to an array of at least
        // 'dstCapacity' elements, 'srcString', if specified as a
        // 'const char *', is null-terminated, and 'errorWord' is either 0 or a
        // valid UTF-32 code point (in the range '[ 1 .. 0xd7ff ]' or
        // '[ 0xe000 .. 0x10ffff ]').  Note that if 'dstCapacity' is 0,
        // '*dstBuffer' is not modified and this function returns a value with
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' set and 0 is written
        // into '*numCodePointsWritten' (if that pointer is not 0), since there
        // is insufficient space for even a null terminator alone.  Also note
        // that one Unicode code point always occupies one 32-bit *word* in
        // UTF-32, but may occupy more than one *byte* of UTF-8, so that
        // '*numCodePointsWritten' equals the number of *words* written.  Also
        // note that 'errorWord' is assumed to be in host byte order.

                           // UTF-32 to UTF-8 Methods

    static int utf32ToUtf8(bsl::string           *dstString,
                           const unsigned int    *srcString,
                           bsl::size_t           *numCodePointsWritten = 0,
                           unsigned char          errorByte            = '?',
                           ByteOrder::Enum        byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf32ToUtf8(std::string           *dstString,
                           const unsigned int    *srcString,
                           bsl::size_t           *numCodePointsWritten = 0,
                           unsigned char          errorByte            = '?',
                           ByteOrder::Enum        byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf32ToUtf8(std::pmr::string   *dstString,
                           const unsigned int *srcString,
                           bsl::size_t        *numCodePointsWritten = 0,
                           unsigned char       errorByte            = '?',
                           ByteOrder::Enum     byteOrder = ByteOrder::e_HOST);
#endif
    static int utf32ToUtf8(bsl::string           *dstString,
                           const unsigned int    *srcString,
                           bsl::size_t            srcStringLength,
                           bsl::size_t           *numCodePointsWritten = 0,
                           unsigned char          errorByte            = '?',
                           ByteOrder::Enum        byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf32ToUtf8(std::string           *dstString,
                           const unsigned int    *srcString,
                           bsl::size_t            srcStringLength,
                           bsl::size_t           *numCodePointsWritten = 0,
                           unsigned char          errorByte            = '?',
                           ByteOrder::Enum        byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf32ToUtf8(std::pmr::string   *dstString,
                           const unsigned int *srcString,
                           bsl::size_t         srcStringLength,
                           bsl::size_t        *numCodePointsWritten = 0,
                           unsigned char       errorByte            = '?',
                           ByteOrder::Enum     byteOrder = ByteOrder::e_HOST);
#endif
        // Load into the specified 'dstString' the result of converting the
        // specified 'srcString' of 'UTF-32' values to 'UTF-8' and return 0 on
        // success or 'CharConvertStatus::k_INVALID_INPUT_BIT' if invalid
        // 'UTF-32' values (in the range '[0xD800 .. 0xDFFF]' or above
        // 0x10FFFF) are encountered.  Optionally specify 'srcStringlength' as
        // the number of 'UTF-32' values to be converted.  If 'srcStringLength'
        // is specified, convert that many UTF-32 values from 'srcString'
        // (including zero values), otherwise convert values up to but not
        // including a terminating zero value.  Optionally specify
        // 'numCodePointsWritten' to receive the number of 'UTF-8' code points
        // written to 'dstString', including the null-terminator.  Optionally
        // specify 'errorByte' as the character to be written to 'dstString' as
        // the translation of invalid 'UTF-32' values; if not specified, '?' is
        // used, and if given as 0, no character is written at all.  Optionally
        // specify 'byteOrder' to determine how 'UTF-32' values in 'srcString'
        // are interpreted; if not given, host byte order is used.  The
        // behavior is undefined if 'errorByte' is 0x80 or above.  Note that if
        // you are passing the 'bsl::vector<unsigned int>' obtained from a call
        // to 'utf8ToUtf32' and using 'srcStringLength', you must take care to
        // pass 'vector.size() - 1' to 'srcStringLength' to avoid embedding the
        // terminating 0.

    static int utf32ToUtf8(bsl::vector<char>     *dstVector,
                           const unsigned int    *srcString,
                           bsl::size_t           *numCodePointsWritten = 0,
                           unsigned char          errorByte            = '?',
                           ByteOrder::Enum        byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf32ToUtf8(std::vector<char>     *dstVector,
                           const unsigned int    *srcString,
                           bsl::size_t           *numCodePointsWritten = 0,
                           unsigned char          errorByte            = '?',
                           ByteOrder::Enum        byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf32ToUtf8(
                        std::pmr::vector<char> *dstVector,
                        const unsigned int     *srcString,
                        bsl::size_t            *numCodePointsWritten = 0,
                        unsigned char           errorByte            = '?',
                        ByteOrder::Enum         byteOrder = ByteOrder::e_HOST);
#endif
    static int utf32ToUtf8(bsl::vector<char>     *dstVector,
                           const unsigned int    *srcString,
                           bsl::size_t            srcStringLength,
                           bsl::size_t           *numCodePointsWritten = 0,
                           unsigned char          errorByte            = '?',
                           ByteOrder::Enum        byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf32ToUtf8(std::vector<char>     *dstVector,
                           const unsigned int    *srcString,
                           bsl::size_t            srcStringLength,
                           bsl::size_t           *numCodePointsWritten = 0,
                           unsigned char          errorByte            = '?',
                           ByteOrder::Enum        byteOrder            =
                                                            ByteOrder::e_HOST);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int utf32ToUtf8(
                        std::pmr::vector<char> *dstVector,
                        const unsigned int     *srcString,
                        bsl::size_t             srcStringLength,
                        bsl::size_t            *numCodePointsWritten = 0,
                        unsigned char           errorByte            = '?',
                        ByteOrder::Enum         byteOrder = ByteOrder::e_HOST);
#endif
    // Load into the specified 'dstVector' the result of converting the
    // specified 'srcString' of 'UTF-32' values to 'UTF-8', always followed by
    // a null character, and return 0 on success or
    // 'CharConvertStatus::k_INVALID_INPUT_BIT' if invalid 'UTF-32' values (in
    // the range '[0xD800 .. 0xDFFF]' or above 0x10FFFF) are seen.  Optionally
    // specify 'srcStringlength' as the number of 'UTF-32' values to be
    // converted.  If 'srcStringLength' is specified, convert that many UTF-32
    // values from 'srcString' (including zero values), otherwise convert
    // values up to but not including a terminating zero value.  Optionally
    // specify 'numCodePointsWritten' to receive the number of 'UTF-8' code
    // points written to 'dstVector'.  Optionally specify 'errorByte' as the
    // character to be written to 'dstVector' as the translation of invalid
    // 'UTF-32' values; if not specified, '?' is used, and if given as 0, no
    // character is written at all.  Optionally specify 'byteOrder' to
    // determine how 'UTF-32' values in 'srcString' are interpreted; if not
    // given, host byte order is used.  The behavior is undefined if
    // 'errorByte' is 0x80 or above.  Note that if you are passing the
    // 'bsl::vector<unsigned int>' obtained from a call to 'utf8ToUtf32' and
    // using 'srcStringLength', you must take care to pass 'vector.size() - 1'
    // to 'srcStringLength' to avoid embedding the terminating 0.

    static int utf32ToUtf8(char               *dstBuffer,
                           bsl::size_t         dstCapacity,
                           const unsigned int *srcString,
                           bsl::size_t        *numCodePointsWritten = 0,
                           bsl::size_t        *numBytesWritten      = 0,
                           unsigned char       errorByte            = '?',
                           ByteOrder::Enum     byteOrder            =
                                                            ByteOrder::e_HOST);
    static int utf32ToUtf8(char               *dstBuffer,
                           bsl::size_t         dstCapacity,
                           const unsigned int *srcString,
                           bsl::size_t         srcStringLength,
                           bsl::size_t        *numCodePointsWritten = 0,
                           bsl::size_t        *numBytesWritten      = 0,
                           unsigned char       errorByte            = '?',
                           ByteOrder::Enum     byteOrder            =
                                                            ByteOrder::e_HOST);
        // Unless 'dstCapacity == 0', load into the specified 'dstBuffer' all
        // or as many complete 'UTF-8' sequences converted from the specified
        // 'srcString' of UTF-32 as will fit, along with an always-present
        // terminating null byte, into the specified 'dstCapacity' bytes, and
        // return 0 on success or a bit-wise OR of
        // 'CharConvertStatus::k_INVALID_INPUT_BIT' if invalid 'UTF-32' values
        // (in the range '[0xD800 .. 0xDFFF]' or above 0x10FFFF) are seen and
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' if there is insufficient
        // room for the entire result to be written.  If 'dstCapacity == 0'
        // return 'CharConvertStatus::k_INVALID_OUT_OF_SPACE_BIT' without
        // modifying 'dstBuffer'.  Optionally specify 'srcStringlength' as the
        // number of 'UTF-32' values to be converted.  If 'srcStringLength' is
        // specified, convert that many UTF-32 values from 'srcString'
        // (including zero values), otherwise convert values up to but not
        // including a terminating zero value.  Optionally specify
        // 'numCodePointsWritten' to receive the number of 'UTF-8' code points
        // written to 'dstBuffer'.  Optionally specify 'numBytesWritten' to
        // receive the number of bytes written to 'dstBuffer'.  Optionally
        // specify 'errorByte' as the character to be written to 'dstBuffer' as
        // the translation of invalid 'UTF-32' values; if not specified, '?' is
        // used, and if given as 0, no character is written at all.  Optionally
        // specify 'byteOrder' to determine how 'UTF-32' values in 'srcString'
        // are interpreted; if not given, host byte order is used.  The
        // behavior is undefined if 'errorByte' is 0x80 or above.  Note that if
        // you are passing the 'bsl::vector<unsigned int>' obtained from a call
        // to 'utf8ToUtf32' and using 'srcStringLength', you must take care to
        // pass 'vector.size() - 1' to 'srcStringLength' to avoid embedding the
        // terminating 0.
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
