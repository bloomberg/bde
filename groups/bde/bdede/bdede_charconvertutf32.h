// bdede_charconvertutf32.h                                           -*-C++-*-
#ifndef INCLUDED_BDEDE_CHARCONVERTUTF32
#define INCLUDED_BDEDE_CHARCONVERTUTF32

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide fast, safe conversion between UTF-8 encoding and utf-32.
//
//@CLASSES:
//  bdede_CharConvertUtf32: namespace for conversion between UTF-8 and utf-32.
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a suite of pure procedures supporting
// the *fast* conversion of *valid* UTF-8 encoded "C" strings to *valid* UTF-32
// 32-bit character arrays, and vice versa.  Invalid byte sequences and
// characters forbidden by either encoding are removed and (optionally)
// replaced by a character provided by the caller.  The byte or word count and
// character count that are optionally returned through pointer arguments
// include the terminating null character, if any, in their count.
//
///History and Motivation
///----------------------
// UTF-8 is a character encoding that allows 32-bit character sets like Unicode
// to be represented using null-terminated (8-bit) byte strings (NTBS), while
// allowing "standard ASCII" strings to be used "as-is".  Note that UTF-8 is
// described in detail in RFC 3629 (http://www.ietf.org/rfc/rfc3629.txt).
//
// UTF-32 is simply a name for storing raw unicode values as sequential
// 'unsigned int's in memory.
//
// Unicode values can be any value in the ranges '[ 1 .. 0xd7ff ]' and
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
// including it for 'string' or 'wstring).  The resizing will not affect the
// capacity.
//
// Non-minimal UTF-8 encodings of characters are reported as errors.  Octets
// and post-conversion characters in the forbidden ranges are treated as errors
// and removed if 0 is specified as 'errorCharacter', or replaced with
// 'errorCharacter' otherwise.o
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example:
///-------
// The following snippets of code illustrate a typical use of the
// 'bdede_CharConvertUtf32' struct's utility functions, first
// converting from UTF-8 to UTF-32, and then converting back to make
// sure the round trip returns the same value.
//
// First, we declare a string of utf8 containing single-, double-,
// triple-, and quadruple-octet characters.
//..
//  const char utf8MultiLang[] = {
//      "Hello"                                         // -- Ascii
//      "\xce\x97"         "\xce\x95"       "\xce\xbb"  // -- Greek
//      "\xe4\xb8\xad"     "\xe5\x8d\x8e"               // -- Chinese
//      "\xe0\xa4\xad"     "\xe0\xa4\xbe"               // -- Hindi
//      "\xf2\x94\xb4\xa5" "\xf3\xb8\xac\x83" };        // -- Quad octets
//..
// Then, we declare an enum summarizing the counts of characters in the
// string and verify that the counts add up to the length of the
// string.
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
// Next, we declare the vector where our utf32 output will go, and a
// variable into which the number of characters (characters, not bytes
// or words) written will be stored.  It is not necessary to initialize
// 'utf32CharsWritten'.
//..
//  bsl::vector<unsigned int> v32;
//..
// Note that for performance, we should
// 'v32.reserve(sizeof(utf8MultiLang))', but it's not strictly
// necessary -- it will automatically be grown to the correct size.
// Note also that if 'v32' were not empty, that wouldn't be a problem
// -- any contents will be discarded.
//
// Then, we do the translation to 'UTF-32'.
//..
//  int retVal = bdede_CharConvertUtf32::utf8ToUtf32(&v32,
//                                                   utf8MultiLang);
//
//  assert(0 == retVal);        // verify success
//  assert(0 == v32.back());    // verify null terminated
//..
// Next, we verify that the number of characters (characters, not bytes
// or words) that was returned is correct.  Note that in UTF-32, the
// number of unicode characters written is the same as the number of
// 32 bit words written.
//..
//  enum { EXPECTED_CHARS_WRITTEN =
//                  NUM_ASCII_CHARS + NUM_GREEK_CHARS + NUM_CHINESE_CHARS +
//                  NUM_HINDI_CHARS + NUM_QUAD_CHARS  + 1 };
//  assert(EXPECTED_CHARS_WRITTEN == v32.size());
//..
// Then, we calculate and confirm the difference betwen the number of
// utf32 words output and the number of bytes input.  The ascii chars
// will take 1 32-bit word apiece, the Greek chars are double octets
// that will become single unsigneds, the Chinese chars are encoded as
// utf8 triple octets that will turn into single 32-bit words, the same
// for the Hindi chars, and the quad chars are quadruple octets that
// will turn into single unsigned ints.
//..
//  enum { SHRINKAGE =
//                    NUM_ASCII_CHARS   * (1-1) + NUM_GREEK_CHARS * (2-1) +
//                    NUM_CHINESE_CHARS * (3-1) + NUM_HINDI_CHARS * (3-1) +
//                    NUM_QUAD_CHARS    * (4-1) };
//
//  assert(v32.size() == sizeof(utf8MultiLang) - SHRINKAGE);
//..
// Next, we go on to do the reverse 'utf32ToUtf8' transform to turn it
// back into utf8, and we should get a result identical to our original
// input.  Declare a 'bsl::string' for our output, and a variable to
// count the number of characters (characters, not bytes or words)
// translated.
//..
//  bsl::string s;
//  bsl::size_t utf8CharsWritten;
//..
// Again, note that for performance, we should ideally
// 's.reserve(3 * v32.size())' but it's not really necessary.
//
// Now, we do the reverse transform:
//..
//  retVal = bdede_CharConvertUtf32::utf32ToUtf8(&s,
//                                               v32.begin(),
//                                               &utf8CharsWritten);
//..
// Finally, we verify a successful status was returned, that the output
// of the reverse transform was identical to the original input, and
// that the number of chars translated was as expected.
//..
//  assert(0 == retVal);
//  assert(utf8MultiLang == s);
//  assert(s.length() + 1         == sizeof(utf8MultiLang));
//
//  assert(EXPECTED_CHARS_WRITTEN == utf8CharsWritten);
//  assert(v32.size()             == utf8CharsWritten);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEDE_CHARCONVERTSTATUS
#include <bdede_charconvertstatus.h>
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
                        // struct bdede_CharConvertUtf32
                        // =============================

struct bdede_CharConvertUtf32 {
    // This 'struct' provides a namespace for a suite of pure procedures to
    // convert character buffers between UTF-8 and UTF-32.  Note that since all
    // UTF-32 operations take place as 'unsigned int' values, byte order is
    // not taken into consideration, and Byte Order Mark (BOM) characters are
    // neither generated nor recognized as special.  If a BOM is present in the
    // input, it will be translated, whether correct ('0xfeff') or incorrect
    // ('0xfffe'), into the output without any special handling.

                        // -- UTF-8 to UTF-32 Methods

    // CLASS METHODS
    static int utf8ToUtf32(bsl::vector<unsigned int> *dstVector,
                           const char                *srcString,
                           unsigned int               errorCharacter  = '?');
        // Load into the specified 'dstWstring' the result of converting the
        // specified null-terminated UTF-8 'srcString' to its UTF-32
        // equivalent.  Optionally specify 'errorCharacter' to be substituted,
        // if not 0, for invalid encodings in the input string.  Return 0 on
        // success and 'bdede_CharConvertStatus::BDEDE_INVALID_CHARS_BIT'
        // otherwise.  Invalid encodings are multi-byte encoding parts out of
        // sequence, non-minimal UTF-8 encodings of characters, or characters
        // outside the ranges which UTF-32 can validly encode (in the range
        // '[ 1 .. 0xd7ff ]' or '[ 0xe000 .. 0x10ffff ]').  If 'errorCharacter'
        // is 0, invalid input characters are ignored (i.e., produce no
        // corresponding output characters).  The behavior is undefined unless
        // 'srcString' is null-terminated.  Note that one *character* always
        // occupies one 32-bit *words* of output; there is no 'numCharsWritten'
        // argument since, after the call, 'dstVector->size()' will equal the
        // number of characters written.

    static int utf8ToUtf32(unsigned int *dstBuffer,
                           bsl::size_t   dstCapacity,
                           const char   *srcString,
                           bsl::size_t  *numCharsWritten = 0,
                           unsigned int  errorCharacter  = '?');
        // Load into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-8
        // 'srcString' to its UTF-32 equivalent.  Optionally specify
        // 'numCharsWritten' which (if not 0) indicates the location of the
        // variable into which the number of unicode characters (including the
        // null terminator) written is to be loaded.  Optionally specify
        // 'errorCharacter' to be substituted (if not 0) for invalid encodings
        // in the input string.  Invalid encodings are multibyte encoding parts
        // out of sequence, non-minimal UTF-8 encodings of characters, UTF-8
        // encodings of more than four bytes length, or illegal unicode chars
        // (in the range '[ 0xd800 .. 0xdfff ]').  If 'errorCharacter' is 0,
        // invalid input characters are ignored (i.e., produce no corresponding
        // output characters).  Return 0 on success and a bit-wise or of the
        // bits specified by 'bdede_CharConvertStatus::Enum' otherwise to
        // indicate that there were invalid char sequences or if 'dstCapacity'
        // was inadequate to store the output.  If 'dstCapacity > 0' yet
        // 'dstCapacity' specifies a buffer too small to hold the output, the
        // maximal null-terminated prefix of the properly converted result
        // string is loaded into 'dstBuffer'.  The behavior is undefined unless
        // 'dstBuffer', if specified, refers to an array of at least
        // 'dstCapacity' elements, 'srcString' is null-terminated, and
        // 'errorCharacter' is either 0 or a valid single-word encoded UTF-32
        // character (in the range '[ 1 .. 0xd7ff ]' or
        // '[ 0xe000 .. 0x10ffff ]').  Note that if 'dstCapacity' is 0,
        // '*dstBuffer' is not modified and this function returns a value with
        // 'bdede_CharConvertStatus::BDEDE_OUT_OF_SPACE_BIT' set and 0 is
        // written into '*numCharsWritten' (if that pointer is not zero), since
        // there is insufficient space for even a null terminator alone.  Also
        // note that one unicode *character* always occupies one 32-bit *word*
        // in UTF-32, but may occupy more than one *byte* of UTF-8, so that
        // '*numCharsWritten' equals the number of *words* written.

                        // -- UTF-32 to UTF-8 Methods

    static int utf32ToUtf8(bsl::string        *dstString,
                           const unsigned int *srcString,
                           bsl::size_t        *numCharsWritten = 0,
                           unsigned char       errorCharacter  = '?');
        // Load into the specified 'dstString' the result of converting the
        // specified null-terminated UTF-32 '*srcString' to its UTF-8
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of unicode *characters* written, including the null
        // terminator, is to be loaded.  Note that one unicode *character* can
        // occupy multiple *bytes* of UTF-8.  Optionally specify
        // 'errorCharacter' to be substituted (if not 0) for invalid encodings
        // in the input string.  Invalid encodings are values that are illegal
        // unicode values (in the range '[ 0xd800 .. 0xdfff ]' or above
        // '0x10ffff').  If 'errorCharacter' is 0, invalid input characters are
        // ignored (i.e., produce no corresponding output characters).  Any
        // previous contents of the destination are discarded.  Return 0 on
        // success and 'bdede_CharConvertStatus::BDEDE_INVALID_CHARS_BIT' if
        // one or more invalid character sequences were encountered in the
        // input.  The behavior is undefined unless 'srcString' is
        // null-terminated and 'errorCharacter' is either zero or a valid
        // single-byte encoded UTF-8 character ('0 < errorCharacter < 0x80').
        // Also note that the string length will be sized to the length of the
        // output, such that
        // 'strlen(dstString->c_str()) == dstString->length()'.

    static int utf32ToUtf8(bsl::vector<char>  *dstVector,
                           const unsigned int *srcString,
                           bsl::size_t        *numCharsWritten = 0,
                           unsigned char       errorCharacter  = '?');
        // Load into the specified 'dstVector' the result of converting the
        // specified null-terminated UTF-32 '*srcString' to its UTF-8
        // equivalent.  Optionally specify 'numCharsWritten' which (if not 0)
        // indicates the location of the modifiable variable into which the
        // number of unicode *characters* written, including the null
        // terminator, is to be loaded.  Note that one *character* can occupy
        // multiple *bytes* of UTF-8.  Optionally specify 'errorCharacter' to
        // be substituted (if not 0) for invalid encodings in the input string.
        // Invalid encodings are incomplete multi-word encodings or parts of a
        // two-word encoding out of their proper sequence.  If 'errorCharacter'
        // is 0, invalid input characters are ignored (i.e., produce no
        // corresponding output characters).  Any previous contents of the
        // destination are discarded.  Return 0 on success and
        // 'bdede_CharConvertStatus::BDEDE_INVALID_CHARS_BIT' if one or more
        // invalid character sequences were encountered in the input.  The
        // behavior is undefined unless 'srcString' is null-terminated and
        // 'errorCharacter' is either zero or a valid single-byte encoded UTF-8
        // character ('0 < errorCharacter < 0x80').  Note that 'dstVector' is
        // sized to exactly fit the output so that
        // 'strlen(dstVector->begin()) == dstVector->size() - 1'.

    static int utf32ToUtf8(char               *dstBuffer,
                           bsl::size_t         dstCapacity,
                           const unsigned int *srcString,
                           bsl::size_t        *numCharsWritten = 0,
                           bsl::size_t        *numBytesWritten = 0,
                           unsigned char       errorCharacter  = '?');
        // Load, into the specified 'dstBuffer' of the specified 'dstCapacity',
        // the result of converting the specified null-terminated UTF-32
        // '*srcString' to its UTF-8 equivalent.  Optionally specify
        // 'numCharsWritten' which (if not 0) indicates the location of the
        // modifiable variable into which the number of unicode *characters*
        // (including the terminating 0, if any) written is to be loaded.  Note
        // that one unicode *character* can occupy multiple UTF-8 *bytes*, but
        // will be a single UTF-32 *word*.  Optionally specify
        // 'numBytesWritten' which (if not 0) indicates the location of the
        // modifiable variable into which the number of bytes written
        // (including the null terminator, if any) is to be loaded.  Optionally
        // specify 'errorCharacter' to be substituted (if not 0) for invalid
        // encodings in the input string.  Invalid encodings are illegal
        // unicode values (in the range '[ 0xd800 .. 0xdfff ]' or above
        // '0x10ffff').  If 'errorCharacter' is 0, invalid input characters are
        // ignored (i.e., produce no corresponding output characters).  Return
        // 0 on success and a bitwise-or of the masks defined by
        // 'bdede_CharConvertStatus::Enum' otherwise, where
        // 'bdede_CharConvertStatus::BDEDE_INVALID_CHARS_BIT' will be set if
        // one or more invalid character sequences were encountered in the
        // input, and 'bdede_CharConvertStatus::BDEDE_OUT_OF_SPACE_BIT' will be
        // set if the output space was exhausted before conversion was
        // complete.  The behavior is undefined unless 'dstBuffer' refers to an
        // array of at least 'dstCapacity' elements, 'srcString' is
        // null-terminated, and 'errorCharacter' is either zero or a valid
        // single-byte encoded UTF-8 character ('0 < errorCharacter < 0x80').
        // Note that if 'dstCapacity' is 0, this function returns
        // 'bdede_CharConvertStatus::BDEDE_OUT_OF_SPACE_FLAG' set and 0 is
        // written into '*numCharsWritten' and '*numBytesWritten' (if those
        // pointers are not zero), since there is insufficient space for even a
        // null terminator alone.  Also note that since UTF-8 is a
        // variable-length encoding, 'numBytesWritten' may be up to four times
        // 'numCharsWritten', and therefore that an input 'srcString' of
        // 'dstCapacity' *characters* (including terminating 0) may not fit
        // into 'dstBuffer'.  Also note that the amount of room needed will
        // vary with the contents of the data and the language being
        // translated, but never will the number of *bytes* output exceed four
        // times the number of *32-bit words* input.  Also note that, if
        // 'dstCapacity > 0', then, after completion,
        // 'strlen(dstBuffer) + 1 == *numBytesWritten'.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
