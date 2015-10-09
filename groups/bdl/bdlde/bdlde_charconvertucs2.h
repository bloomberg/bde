// bdlde_charconvertucs2.h                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_CHARCONVERTUCS2
#define INCLUDED_BDLDE_CHARCONVERTUCS2

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif

BSLS_IDENT("$Id: $")

//@PURPOSE: Provide efficient conversions between UTF-8 and UCS-2 encodings.
//
//@CLASSES:
//    bdlde::CharConvertUcs2: namespace for conversions between UTF-8 and UCS-2
//
//@DESCRIPTION: This component provides a suite of pure procedures supporting
// the *fast* conversion of *valid* UTF-8 encoded "C" strings to *valid* UCS-2
// 16-bit character arrays and vice versa.  In order to provide the fastest
// possible implementation, some error checking is deliberately omitted, and
// the input strings are required to be null-terminated; however, all C-style
// functions will honor 'strlcpy' semantics and null-terminate any output
// buffer having a non-zero length.
//
///History and Motivation
///----------------------
// UTF-8 is a character encoding that allows 32-bit character sets like Unicode
// to be represented using null-terminated (8-bit) byte strings (NTBS), while
// allowing "standard ASCII" strings to be used "as-is".  Note that UTF-8 is
// described in detail in RFC 2279 (http://tools.ietf.org/html/rfc2279).
//
// UCS-2 is a 16-bit character encoding with no support for "higher-order"
// character encodings.  UCS-2 is equivalent to UTF-16 in the Basic
// Multilingual Plane (BMP) of Unicode (the first 65536 character points,
// excluding the "surrogate code points" U+D800-U+DFFF, which do not map to
// Unicode characters).  If the characters being represented are within the
// BMP, then UCS-2 can be thought of as "the Windows encoding" for
// international characters.  Historically, UCS-2 was the only "wide char"
// representation for Windows versions prior to Windows 2000.  UTF-16 was
// adopted instead for Windows 2000, and has been used ever since.
//
// Most conversion routines strive for correctness at the cost of performance.
// The 'glib' conversion routines are *much* slower than the functions
// implemented here because the 'glib' functions first compute the number of
// output characters required, allocate the memory for them, and then perform
// the conversion, validating the input characters.  The C-style methods of
// 'bdlde::CharConvertUcs2', on the other hand, assume that the user-provided
// output buffer is wide enough, make a "best effort" to convert into it, and
// return an error code if not enough space was provided.  The C++-style
// methods are more forgiving, since the output 'bsl::string' or
// 'bsl::vector<unsigned short>' is resized as needed.  No attempt is made to
// validate whether the character codes correspond to valid Unicode code
// points, nor is validation performed to check for overlong UTF-8 encodings
// (where characters that could be expressed in one octet are encoded using two
// octets).
//
///Usage
///- - -
// This section illustrates intended use of this component.
//
///Example 1: C-Style Interface
/// - - - - - - - - - - - - - -
// The following snippet of code illustrates a typical use of the
// 'bdlde::CharConvertUcs2' struct's C-style utility functions, converting a
// simple UTF-8 string to UCS-2.
//..
// void testCFunction1()
// {
//     unsigned short buffer[256];  // arbitrary "wide-enough" size
//     bsl::size_t    buffSize = sizeof buffer / sizeof *buffer;
//     bsl::size_t    charsWritten;
//
//     int retVal =
//               BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(buffer,
//                                                              buffSize,
//                                                              "Hello",
//                                                              &charsWritten);
//
//     assert( 0  == retVal);
//     assert('H' == buffer[0]);
//     assert('e' == buffer[1]);
//     assert('l' == buffer[2]);
//     assert('l' == buffer[3]);
//     assert('o' == buffer[4]);
//     assert( 0  == buffer[5]);
//     assert( 6  == charsWritten);
// }
//..
//
///Example 2: C-Style Round-Trip
///- - - - - - - - - - - - - - -
// The following snippet of code illustrates another typical use of the
// 'bdlde::CharConvertUcs2' struct's C-style utility functions, converting a
// simple UTF-8 string to UCS-2, then converting the UCS-2 back and making sure
// the round-trip conversion results in the input.
//..
// void testCFunction2()
// {
//     unsigned short buffer[256];  // arbitrary "wide-enough" size
//     bsl::size_t    buffSize = sizeof buffer / sizeof *buffer;
//     bsl::size_t    charsWritten;
//
//     // "&Eacute;cole", the French word for School.  '&Eacute;' is the HTML
//     // entity equivalent to "Unicode-E WITH ACUTE, LATIN CAPITAL LETTER".
//     int retVal =
//           BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(buffer,
//                                                          buffSize,
//                                                          "\xc3\x89" "cole",
//                                                          &charsWritten);
//
//     assert( 0   == retVal);
//     assert(0xc9 == buffer[0]); // Unicode-E WITH ACUTE, LATIN CAPITAL LETTER
//     assert('c'  == buffer[1]);
//     assert('o'  == buffer[2]);
//     assert('l'  == buffer[3]);
//     assert('e'  == buffer[4]);
//     assert( 0   == buffer[5]);
//     assert( 6   == charsWritten);
//
//     char           buffer2[256];  // arbitrary "wide-enough" size
//     bsl::size_t    buffer2Size  = sizeof buffer2 / sizeof *buffer2;
//     bsl::size_t    bytesWritten = 0;
//
//     // Reversing the conversion returns the original string:
//     retVal =
//           BloombergLP::bdlde::CharConvertUcs2::ucs2ToUtf8(buffer2,
//                                                          buffer2Size,
//                                                          buffer,
//                                                          &charsWritten,
//                                                          &bytesWritten);
//
//     assert( 0 == retVal);
//     assert( 0 == bsl::strcmp(buffer2, "\xc3\x89" "cole"));
//
//     // 6 characters written, but 7 bytes, since the first character takes 2
//     // octets.
//
//     assert( 6 == charsWritten);
//     assert( 7 == bytesWritten);
// }
//..
// In this example, a UTF-8 input string is converted then passed to another
// function, which expects a UCS-2 buffer.
//
// First, we define a utility *strlen* replacement for UCS-2:
//..
// int wideStrlen(const unsigned short *str)
// {
//     int len = 0;
//
//     while (*str++) {
//         ++len;
//     }
//
//     return len;
// }
//..
// Now, some arbitrary function that calls 'wideStrlen':
//..
// void functionRequiringUcs2(const unsigned short *str, bsl::size_t strLen)
// {
//     // Would probably do something more reasonable here.
//
//     assert(wideStrlen(str) + 1 == strLen);
// }
//..
// Finally, we can take some UTF-8 as an input and call
// 'functionRequiringUcs2':
//..
// void processUtf8(const char *strU8)
// {
//     unsigned short buffer[1024];  // some "large enough" size
//     bsl::size_t    buffSize     = sizeof buffer / sizeof *buffer;
//     bsl::size_t    charsWritten = 0;
//
//     int result =
//               BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(buffer,
//                                                              buffSize,
//                                                              strU8,
//                                                              &charsWritten);
//
//     if (0 == result) {
//         functionRequiringUcs2(buffer, charsWritten);
//     }
// }
//..
//
///Example 3: C++-Style Interface
/// - - - - - - - - - - - - - - -
// The following snippet of code illustrates a typical use of the
// 'bdlde::CharConvertUcs2' struct's C++-style utility functions, converting a
// simple UTF-8 string to UCS-2.
//..
// void loadUCS2Hello(bsl::vector<unsigned short> *result)
// {
//     int retVal =
//               BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(result,
//                                                              "Hello");
//
//     assert( 0  == retVal);
//     assert('H' == (*result)[0]);
//     assert('e' == (*result)[1]);
//     assert('l' == (*result)[2]);
//     assert('l' == (*result)[3]);
//     assert('o' == (*result)[4]);
//     assert( 0  == (*result)[5]);
//     assert( 6  == result->size());
// }
//..
// The following snippet of code illustrates another typical use of the
// 'bdlde::CharConvertUcs2' struct's C++-style utility functions, first
// converting from UTF-8 to UCS-2, and then converting back to make sure the
// round trip returns the same value.
//..
// void checkCppRoundTrip()
// {
//     bsl::vector<unsigned short> result;
//
//     // "&Eacute;cole", the French word for School.  &Eacute; is the HTML
//     // entity corresponding to "Unicode-E WITH ACUTE, LATIN CAPITAL LETTER".
//     int retVal =
//           BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(&result,
//                                                          "\xc3\x89" "cole");
//
//     assert( 0   == retVal);
//     assert(0xc9 == result[0]); // Unicode-E WITH ACUTE, LATIN CAPITAL LETTER
//     assert('c'  == result[1]);
//     assert('o'  == result[2]);
//     assert('l'  == result[3]);
//     assert('e'  == result[4]);
//     assert( 0   == result[5]);
//     assert( 6   == result.size());
//
//     bsl::string    result2;
//     bsl::size_t    charsWritten = 0;
//
//     // Reversing the conversion returns the original string:
//     retVal =
//           BloombergLP::bdlde::CharConvertUcs2::ucs2ToUtf8(&result2,
//                                                          &result.front(),
//                                                          &charsWritten);
//
//     assert( 0 == retVal);
//     assert( result2 == "\xc3\x89" "cole");
//
//     // 6 characters written (including the null-terminator), and 6 bytes,
//     // since the first character takes 2 octets and the null-terminator is
//     // not counted in "length()".
//     assert( 6 == charsWritten);
//     assert( 6 == result2.length());
// }
//..
// In this example, a UTF-8 input string is converted then returned.
//..
// bsl::vector<unsigned short> processUtf8(const bsl::string& strU8)
// {
//     bsl::vector<unsigned short> result;
//
//     int retCode =
//               BloombergLP::bdlde::CharConvertUcs2::utf8ToUcs2(&result,
//                                                              strU8.c_str());
//
//     return result;
// }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
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
                           // ======================
                           // struct CharConvertUcs2
                           // ======================

struct CharConvertUcs2 {
    // This 'struct' provides a namespace for a suite of pure procedures to
    // convert character buffers between UTF-8 and UCS-2.  UCS-2 conversions
    // are performed to/from the full '2 ^ 16' bit space (the "UTF-16" hole
    // U+D800-U+DFFF is not treated as a special case).  Note that all C-style
    // routines in this component honor *strlcpy* semantics, meaning that all
    // returned C-style strings will be null-terminated as long as the return
    // buffer size is positive (i.e., 'dstCapacity > 0').  Note that since all
    // UCS-2 operations take place as 'unsigned short's, byte order is not
    // taken into consideration, and Byte Order Mark (BOM) characters are not
    // generated.  If a BOM is present in the input, it will be translated into
    // the output.

    // CLASS METHODS
    static int utf8ToUcs2(unsigned short *dstBuffer,
                          bsl::size_t     dstCapacity,
                          const char     *srcString,
                          bsl::size_t    *numCharsWritten = 0,
                          unsigned short  errorCharacter  = '?');
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-8
        // 'srcString' to its UCS-2 equivalent.  Optionally specify
        // 'numCharsWritten' which (if non-zero) indicates the modifiable
        // integer into which the number of characters written (including the
        // null terminator) is to be loaded.  Optionally specify
        // 'errorCharacter' to be substituted for invalid (i.e., not
        // convertible to UCS-2) input characters.  If 'errorCharacter' is 0,
        // invalid input characters are ignored (i.e., produce no corresponding
        // output characters).  Return 0 on success and a bitwise-or of the
        // masks specified by 'CharConvertStatus::Enum' otherwise, with
        // 'CharConvertStatus::k_INVALID_CHARS_BIT' set to indicate that at
        // least one invalid input sequence was encountered, and
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' set to indicate that
        // 'dstCapacity' was insufficient to accommodate the output.  If
        // 'dstCapacity' was insufficient, the maximal null-terminated prefix
        // of the properly converted result string is loaded into 'dstBuffer',
        // and (unless null) '*numCharsWritten' is set to 'dstCapacity'.  The
        // behavior is undefined unless '0 <= dstCapacity', 'dstBuffer' refers
        // to an array of at least 'dstCapacity' elements, and 'srcString' is
        // null-terminated.  Note that if 'dstCapacity' is 0, this function
        // returns exactly 2 and '*numCharsWritten' (if specified) is loaded
        // with 0 (since there is insufficient space for the null terminator
        // even for an empty input string).

    static int utf8ToUcs2(bsl::vector<unsigned short> *result,
                          const char                  *srcString,
                          unsigned short               errorCharacter  = '?');
        // Load into the specified 'result' the conversion of the specified
        // null-terminated UTF-8 'srcString' to its null-terminated UCS-2
        // equivalent.  Optionally specify 'errorCharacter' to be substituted
        // for invalid (i.e., not convertible to UCS-2) input characters.  If
        // 'errorCharacter' is 0, invalid input characters are ignored (i.e.,
        // produce no corresponding output characters).  Return 0 on success
        // and 'CharConvertStatus::BDEDE_INVALILD_CHARS_BIT' otherwise,
        // meaning that at least one sequence of characters was encountered
        // that could not be translated to UCS-2.  If 'result & 1' is non-zero,
        // one or more input characters are invalid (in which case the
        // conversion continues).  The behavior is undefined unless 'srcString'
        // is null-terminated.  Note that the null-terminating word counts
        // towards 'result->size()'.

    static int ucs2ToUtf8(char                 *dstBuffer,
                          bsl::size_t           dstCapacity,
                          const unsigned short *srcString,
                          bsl::size_t          *numCharsWritten = 0,
                          bsl::size_t          *numBytesWritten = 0);
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UCS-2
        // 'srcString' to its UTF-8 equivalent.  Optionally specify
        // 'numCharsWritten' which (if not 0) indicates the modifiable integer
        // into which the number of *UTF-8 characters* written (including the
        // null terminator) is to be loaded.  Optionally specify
        // 'numBytesWritten' which (if not 0) indicates the modifiable integer
        // into which the number of *bytes* written (including the null
        // terminator) is to be loaded.  Return 0 on success and a bitwise-or
        // of the masks specified by 'CharConvertStatus::Enum' otherwise,
        // with 'CharConvertStatus::k_INVALID_CHARS_BIT' set to indicate that
        // at least one invalid input sequence was encountered, and
        // 'CharConvertStatus::k_OUT_OF_SPACE_BIT' set to indicate that
        // 'dstCapacity' was insufficient to accommodate the output.  If
        // 'dstCapacity' was insufficient, the maximal null-terminated prefix
        // of the properly converted result string is loaded into 'dstBuffer'.
        // The behavior is undefined unless '0 <= dstCapacity', 'dstBuffer'
        // refers to an array of at least 'dstCapacity' elements, and
        // 'srcString' is null-terminated.  Note that if 'dstCapacity' is 0,
        // this function returns exactly 2 and '*numCharsWritten' and
        // '*numBytesWritten' (if not null) are loaded with 0 (since there is
        // insufficient space for the null terminator even for an empty input
        // string).  Also note that since UTF-8 is a variable-length encoding,
        // it is possible for 'numBytesWritten' to be greater than
        // 'numCharsWritten', and therefore that an input 'srcString' of
        // 'dstCapacity - 1' *characters* may not fit into 'dstBuffer'.

    static int ucs2ToUtf8(bsl::string          *result,
                          const unsigned short *srcString,
                          bsl::size_t          *numCharsWritten = 0);
        // Load, into the specified 'result', the conversion of the specified
        // null-terminated UCS-2 'srcString' to its UTF-8 equivalent.
        // Optionally specify 'numCharsWritten' which (if not 0) indicates the
        // modifiable integer into which the number of *characters* written
        // (including the null terminator) is to be loaded.  Return 0 on
        // success and 'CharConvertStatus::BDEDE_INVALILD_CHARS_BIT'
        // otherwise, meaning that at least one sequence of characters was
        // encountered that could not be translated to UTF-8.  The behavior is
        // undefined unless 'srcString' is null-terminated.  Note that the
        // null-terminating character is not counted in 'result->length()'.
        // Also note that this function does not currently implement failure
        // modes; however, this could change if UTF-8 input validation is
        // added.
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
