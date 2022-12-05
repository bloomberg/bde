// bdlde_utf8util.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLDE_UTF8UTIL
#define INCLUDED_BDLDE_UTF8UTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide basic utilities for UTF-8 encodings.
//
//@CLASSES:
//  bdlde::Utf8Util: namespace for utilities for UTF-8 encodings
//
//@DESCRIPTION: This component provides, within the 'bdlde::Utf8Util' 'struct',
// a suite of static functions supporting UTF-8 encoded strings.  Two
// interfaces are provided for each function, one where the length of the
// string (in *bytes*) is passed as a separate argument, and one where the
// string is passed as a null-terminated C-style string.
//
// A string is deemed to contain valid UTF-8 if it is compliant with RFC 3629,
// meaning that only 1-, 2-, 3-, and 4-byte sequences are allowed.  Values
// above 'U+10ffff' are also not allowed.
//
// Six types of functions are provided:
//
//: o 'isValid', which checks for validity, per RFC 3629, of a (candidate)
//:   UTF-8 string.  "Overlong values", that is, values encoded in more bytes
//:   than necessary, are not tolerated; nor are "surrogate values", which are
//:   values in the range '[U+d800 .. U+dfff]'.
//:
//: o 'advanceIfValid' and 'advanceRaw', which advance some number of Unicode
//:   code points, each of which may be encoded in multiple bytes in a UTF-8
//:   string.  'advanceRaw' assumes the string is valid UTF-8, while
//:   'advanceIfValid' checks the input for validity and stops advancing if a
//:   sequence is encountered that is not valid UTF-8.
//:
//: o 'numCodePointsIfValid' and 'numCodePointsRaw', which return the number of
//:   Unicode code points in a UTF-8 string.  Note that 'numCodePointsIfValid'
//:   both validates a (candidate) UTF-8 string and counts the number of
//:   Unicode code points that it contains.
//:
//: o 'numBytesIfValid', which returns the number of bytes a specified number
//:   of Unicode code points occupy in a UTF-8 string.
//:
//: o 'getByteSize', which returns the length of a single UTF-8 encoded
//:   character.
//:
//: o 'appendUtf8Character', which appends a single Unicode code point to a
//:   UTF-8 string.
//
// Embedded null bytes are allowed in strings that are accompanied by an
// explicit length argument.  Naturally, null-terminated C-style strings cannot
// contain embedded null code points.
//
// The UTF-8 format is described in the RFC 3629 document at:
//..
//  http://tools.ietf.org/html/rfc3629
//..
// and in Wikipedia at:
//..
//  http://en.wikipedia.org/wiki/Utf-8
//..
//
///Empty Input Strings
///-------------------
// The utility functions provided by this component consider the empty string
// to be valid UTF-8.  For those functions that take input as a
// '(pointer, length)' pair, if '0 == pointer' and '0 == length', then the
// input is interpreted as a valid, empty string.  However, if '0 == pointer'
// and '0 != length', the behavior is undefined.  All such functions have a
// counterpart that takes a lone pointer to a null-terminated (C-style) string.
// The behavior is always undefined if 0 is supplied for that lone pointer.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Validating Strings and Counting Unicode Code Points
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this usage example, we will encode some Unicode code points in UTF-8
// strings and demonstrate those that are valid and those that are not.
//
// First, we build an unquestionably valid UTF-8 string:
//..
//  bsl::string string;
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 0xff00);
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 0x856);
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 'a');
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 0x1008aa);
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 0xfff);
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 'w');
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 0x1abcd);
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, '.');
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, '\n');
//..
// Then, we check its validity and measure its length:
//..
//  assert(true == bdlde::Utf8Util::isValid(string.data(), string.length()));
//  assert(true == bdlde::Utf8Util::isValid(string.c_str()));
//
//  assert(   9 == bdlde::Utf8Util::numCodePointsRaw(string.data(),
//                                                   string.length()));
//  assert(   9 == bdlde::Utf8Util::numCodePointsRaw(string.c_str()));
//..
// Next, we encode a lone surrogate value, '0xd8ab', that we encode as the raw
// 3-byte sequence "\xed\xa2\xab" to avoid validation:
//..
//  bsl::string stringWithSurrogate = string + "\xed\xa2\xab";
//
//  assert(false == bdlde::Utf8Util::isValid(stringWithSurrogate.data(),
//                                           stringWithSurrogate.length()));
//  assert(false == bdlde::Utf8Util::isValid(stringWithSurrogate.c_str()));
//..
// Then, we cannot use 'numCodePointsRaw' to count the code points in
// 'stringWithSurrogate', since the behavior of that method is undefined unless
// the string is valid.  Instead, the 'numCodePointsIfValid' method can be used
// on strings whose validity we are uncertain of:
//..
//  const char *invalidPosition = 0;
//
//  bsls::Types::IntPtr rc;
//  rc = bdlde::Utf8Util::numCodePointsIfValid(&invalidPosition,
//                                             stringWithSurrogate.data(),
//                                             stringWithSurrogate.length());
//  assert(rc < 0);
//  assert(bdlde::Utf8Util::k_SURROGATE == rc);
//  assert(invalidPosition == stringWithSurrogate.data() + string.length());
//
//  invalidPosition = 0;  // reset
//
//  rc = bdlde::Utf8Util::numCodePointsIfValid(&invalidPosition,
//                                             stringWithSurrogate.c_str());
//  assert(rc < 0);
//  assert(bdlde::Utf8Util::k_SURROGATE == rc);
//  assert(invalidPosition == stringWithSurrogate.data() + string.length());
//..
// Now, we encode 0, which is allowed.  However, note that we cannot use any
// interfaces that take a null-terminated string for this case:
//..
//  bsl::string stringWithNull = string;
//  stringWithNull += '\0';
//
//  assert(true == bdlde::Utf8Util::isValid(stringWithNull.data(),
//                                          stringWithNull.length()));
//
//  assert(  10 == bdlde::Utf8Util::numCodePointsRaw(stringWithNull.data(),
//                                                   stringWithNull.length()));
//..
// Finally, we encode '0x3a' (':') as an overlong value using 2 bytes, which is
// not valid UTF-8 (since ':' can be "encoded" in 1 byte):
//..
//  bsl::string stringWithOverlong = string;
//  stringWithOverlong += static_cast<char>(0xc0);        // start of 2-byte
//                                                        // sequence
//  stringWithOverlong += static_cast<char>(0x80 | ':');  // continuation byte
//
//  assert(false == bdlde::Utf8Util::isValid(stringWithOverlong.data(),
//                                           stringWithOverlong.length()));
//  assert(false == bdlde::Utf8Util::isValid(stringWithOverlong.c_str()));
//
//  rc = bdlde::Utf8Util::numCodePointsIfValid(&invalidPosition,
//                                             stringWithOverlong.data(),
//                                             stringWithOverlong.length());
//  assert(rc < 0);
//  assert(bdlde::Utf8Util::k_OVERLONG_ENCODING == rc);
//  assert(invalidPosition == stringWithOverlong.data() + string.length());
//
//  rc = bdlde::Utf8Util::numCodePointsIfValid(&invalidPosition,
//                                             stringWithOverlong.c_str());
//  assert(rc < 0);
//  assert(bdlde::Utf8Util::k_OVERLONG_ENCODING == rc);
//  assert(invalidPosition == stringWithOverlong.data() + string.length());
//..
//
///Example 2: Advancing Over a Given Number of Code Points
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will use the various 'advance' functions to advance
// through a UTF-8 string.
//
// First, build the string using 'appendUtf8CodePoint', keeping track of how
// many bytes are in each Unicode code point:
//..
//  bsl::string string;
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 0xff00);        // 3 bytes
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 0x1ff);         // 2 bytes
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 'a');           // 1 byte
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 0x1008aa);      // 4 bytes
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 0x1abcd);       // 4 bytes
//  string += "\xe3\x8f\xfe";           // 3 bytes (invalid 3-byte sequence,
//                                      // the first 2 bytes are valid but the
//                                      // last continuation byte is invalid)
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, 'w');           // 1 byte
//  bdlde::Utf8Util::appendUtf8CodePoint(&string, '\n');          // 1 byte
//..
// Then, declare a few variables we'll need:
//..
//  bsls::Types::IntPtr  rc;
//  int                  status;
//  const char          *result;
//  const char *const start = string.c_str();
//..
// Next, try advancing 2 code points, then 3, then 4, observing that the value
// returned is the number of Unicode code points advanced.  Note that since
// we're only advancing over valid UTF-8, we can use either 'advanceRaw' or
// 'advanceIfValid':
//..
//  rc = bdlde::Utf8Util::advanceRaw(              &result, start, 2);
//  assert(2 == rc);
//  assert(3 + 2 == result - start);
//
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, 2);
//  assert(0 == status);
//  assert(2 == rc);
//  assert(3 + 2 == result - start);
//
//  rc = bdlde::Utf8Util::advanceRaw(             &result, start, 3);
//  assert(3 == rc);
//  assert(3 + 2 + 1 == result - start);
//
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, 3);
//  assert(0 == status);
//  assert(3 == rc);
//  assert(3 + 2 + 1 == result - start);
//
//  rc = bdlde::Utf8Util::advanceRaw(             &result, start, 4);
//  assert(4 == rc);
//  assert(3 + 2 + 1 + 4 == result - start);
//
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, 4);
//  assert(0 == status);
//  assert(4 == rc);
//  assert(3 + 2 + 1 + 4 == result - start);
//..
// Then, try advancing by more code points than are present using
// 'advanceIfValid', and wind up stopping when we encounter invalid input.  The
// behavior of 'advanceRaw' is undefined if it is used on invalid input, so we
// cannot use it here.  Also note that we will stop at the beginning of the
// invalid Unicode code point, and not at the first incorrect byte, which is
// two bytes later:
//..
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, INT_MAX);
//  assert(0 != status);
//  assert(5 == rc);
//  assert(3 + 2 + 1 + 4 + 4                 == result - start);
//  assert(static_cast<int>(string.length()) >  result - start);
//..
// Now, doctor the string to replace the invalid code point with a valid one,
// so the string is entirely correct UTF-8:
//..
//  string[3 + 2 + 1 + 4 + 4 + 2] = static_cast<char>(0x8a);
//..
// Finally, advance using both functions by more code points than are in the
// string and in both cases wind up at the end of the string.  Note that
// 'advanceIfValid' does not return an error (non-zero) value to 'status' when
// it encounters the end of the string:
//..
//  rc = bdlde::Utf8Util::advanceRaw(             &result, start, INT_MAX);
//  assert(8 == rc);
//  assert(3 + 2 + 1 + 4 + 4 + 3 + 1 + 1     == result - start);
//  assert(static_cast<int>(string.length()) == result - start);
//
//  rc = bdlde::Utf8Util::advanceIfValid(&status, &result, start, INT_MAX);
//  assert(0 == status);
//  assert(8 == rc);
//  assert(3 + 2 + 1 + 4 + 4 + 3 + 1 + 1     == result - start);
//  assert(static_cast<int>(string.length()) == result - start);
//..
//
///Example 3: Validating UTF-8 Read from a 'bsl::streambuf'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this usage example, we will demonstrate reading and validating UTF-8
// from a stream.
//
// We write a function to read valid UTF-8 to a 'bsl::string'.  We don't know
// how long the input will be, so we don't know how long to make the string
// before we start.  We will grow the string in small, 32-byte increments.
//..
//  int utf8StreambufToString(bsl::string    *output,
//                            bsl::streambuf *sb)
//      // Read valid UTF-8 from the specified streambuf 'sb' to the specified
//      // 'output'.  Return 0 if the input was exhausted without encountering
//      // any invalid UTF-8, and a non-zero value otherwise.  If invalid UTF-8
//      // is encountered, log a message describing the problem after loading
//      // all the valid UTF-8 preceding it into 'output'.  Note that after the
//      // call, in no case will 'output' contain any invalid UTF-8.
//  {
//      enum { k_READ_LENGTH = 32 };
//
//      output->clear();
//      while (true) {
//          bsl::size_t len = output->length();
//          output->resize(len + k_READ_LENGTH);
//          int status;
//          IntPtr numBytes = bdlde::Utf8Util::readIfValid(&status,
//                                                         &(*output)[len],
//                                                         k_READ_LENGTH,
//                                                         sb);
//          BSLS_ASSERT(0 <= numBytes);
//          BSLS_ASSERT(numBytes <= k_READ_LENGTH);
//
//          output->resize(len + numBytes);
//          if (0 < status) {
//              // Buffer was full before the end of input was encountered.
//              // Note that 'numBytes' may be up to 3 bytes less than
//              // 'k_READ_LENGTH'.
//
//              BSLS_ASSERT(k_READ_LENGTH - 4 < numBytes);
//
//              // Go on to grow the string and get more input.
//
//              continue;
//          }
//          else if (0 == status) {
//              // Success!  We've reached the end of input without
//              // encountering any invalid UTF-8.
//
//              return 0;                                             // RETURN
//          }
//          else {
//              // Invalid UTF-8 encountered; the value of 'status' indicates
//              // the exact nature of the problem.  'numBytes' returned from
//              // the above call indicated the number of valid UTF-8 bytes
//              // read before encountering the invalid UTF-8.
//
//              BSLS_LOG_ERROR("Invalid UTF-8 error %s at position %u.\n",
//                             bdlde::Utf8Util::toAscii(status),
//                             static_cast<unsigned>(output->length()));
//
//              return -1;                                            // RETURN
//          }
//      }
//  }
//..

#include <bdlscm_version.h>

#include <bsls_assert.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
#include <memory_resource>
#endif
#include <string>            // 'std::string', 'std::pmr::string'
#include <bsl_streambuf.h>

namespace BloombergLP {

namespace bdlde {
                              // ===============
                              // struct Utf8Util
                              // ===============

struct Utf8Util {
    // This struct provides a namespace for static methods used for validating
    // UTF-8 strings, for counting the number of Unicode code points in them,
    // for advancing pointers through UTF-8 strings by a specified number of
    // Unicode code points, for counting the number of bytes a UTF-8 leading
    // substring occupies, for counting the number of bytes in a UTF-8
    // character, and for appending a Unicode character to a UTF-8 string.

    // PUBLIC TYPES
    typedef bsls::Types::size_type size_type;
    typedef bsls::Types::IntPtr    IntPtr;
    typedef bsls::Types::Uint64    Uint64;

    enum ErrorStatus {
        // Enumerate the error status values that are returned (possibly
        // through an out parameter) from some methods in this utility.  Note
        // that some of the functions in this 'struct' have a return value
        // that is non-negative on success, and one of these values when an
        // error occurs, so all of these values must be negative to distinguish
        // them from a "success" value.

        k_END_OF_INPUT_TRUNCATION       = -1,
           // The end of input was reached partway through a multibyte UTF-8
           // sequence.

        k_UNEXPECTED_CONTINUATION_OCTET = -2,
           // A continuation byte was encountered when not within a multibyte
           // sequence.

        k_NON_CONTINUATION_OCTET        = -3,
           // A non-continuation byte was encountered where a continuation byte
           // was expected.

        k_OVERLONG_ENCODING             = -4,
           // The encoded Unicode value could have been encoded in a sequence
           // of fewer bytes.

        k_INVALID_INITIAL_OCTET         = -5,
           // A sequence began with an octet with its 5 highest-order bits all
           // set, which is always invalid in UTF-8.

        k_VALUE_LARGER_THAN_0X10FFFF    = -6,
           // A value larger than 0x10FFFF was encoded.

        k_SURROGATE                     = -7
           // Illegal occurrence of Unicode code point reserved for surrogate
           // values in UTF-16.  Note that all surrogate values are illegal as
           // Unicode code points.
    };

    // CLASS METHODS
    static IntPtr advanceIfValid(int         *status,
                                 const char **result,
                                 const char  *string,
                                 IntPtr       numCodePoints);
        // Advance past 0 or more consecutive *valid* Unicode code points at
        // the beginning of the specified 'string', until either the specified
        // 'numCodePoints' have been traversed, or the terminating null byte or
        // invalid UTF-8 is encountered (whichever occurs first), and return
        // the number of Unicode code points traversed.  Set the specified
        // '*status' to 0 if no invalid UTF-8 is encountered, and to a value
        // from the 'ErrorStatus' 'enum' otherwise.  Set the specified
        // '*result' to the address of the byte immediately following the last
        // valid code point traversed, or to 'string' if 'string' is empty or
        // 'numCodePoints' is 0.  'string' is necessarily null-terminated, so
        // it cannot contain embedded null bytes.  The behavior is undefined
        // unless '0 <= numCodePoints'.  Note that the value returned will be
        // in the range '[0 .. numCodePoints]'.  Also note that 'string' may
        // contain less than 'bsl::strlen(string)' Unicode code points.

    static IntPtr advanceIfValid(int         *status,
                                 const char **result,
                                 const char  *string,
                                 size_type    length,
                                 IntPtr       numCodePoints);
        // Advance past 0 or more consecutive *valid* Unicode code points at
        // the beginning of the specified 'string' having the specified
        // 'length' (in bytes), until either the specified 'numCodePoints' or
        // 'length' bytes have been traversed, or invalid UTF-8 is encountered
        // (whichever occurs first), and return the number of Unicode code
        // points traversed.  Set the specified '*status' to 0 if no invalid
        // UTF-8 is encountered, and to a value from the 'ErrorStatus' 'enum'
        // otherwise.  Set the specified '*result' to the address of the byte
        // immediately following the last valid code point traversed, or to
        // 'string' if 'length' or 'numCodePoints' is 0.  'string' need not be
        // null-terminated and can contain embedded null bytes, and 'string'
        // may be null if '0 == length' (see {Empty Input Strings}).  The
        // behavior is undefined unless '0 <= numCodePoints'.  Note that the
        // value returned will be in the range '[0 .. numCodePoints]'.  Also
        // note that 'string' may contain less than 'length' Unicode code
        // points.

    static IntPtr advanceIfValid(int                      *status,
                                 const char              **result,
                                 const bsl::string_view&   string,
                                 IntPtr                    numCodePoints);
        // Advance past 0 or more consecutive *valid* Unicode code points at
        // the beginning of the specified 'string', until either the specified
        // 'numCodePoints' bytes or the whole 'string' have been traversed, or
        // invalid UTF-8 is encountered (whichever occurs first), and return
        // the number of Unicode code points traversed.  Set the specified
        // '*status' to 0 if no invalid UTF-8 is encountered, and to a value
        // from the 'ErrorStatus' 'enum' otherwise.  Set the specified
        // '*result' to the address of the byte immediately following the last
        // valid code point traversed, or to 'string' if its length or
        // 'numCodePoints' is 0.  'string' need not be null-terminated and can
        // contain embedded null bytes.  The behavior is undefined unless
        // '0 <= numCodePoints'.  Note that the value returned will be in the
        // range '[0 .. numCodePoints]'.  Also note that 'string' may contain
        // less than 'string.length()' Unicode code points.

    static IntPtr advanceRaw(const char **result,
                             const char  *string,
                             IntPtr       numCodePoints);
        // Advance past 0 or more consecutive Unicode code points at the
        // beginning of the specified 'string', until either the specified
        // 'numCodePoints' bytes have been traversed or the terminating null
        // byte is encountered (whichever occurs first), and return the number
        // of Unicode code points traversed.  Set the specified '*result' to
        // the address of the byte immediately following the last code point
        // traversed, or to 'string' if 'string' is empty or 'numCodePoints' is
        // 0.  'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8 and '0 <= numCodePoints'.  Note that the value
        // returned will be in the range '[0 .. numCodePoints]'.  Also note
        // that 'string' may contain less than 'bsl::strlen(string)' Unicode
        // code points.

    static IntPtr advanceRaw(const char **result,
                             const char  *string,
                             size_type    length,
                             IntPtr       numCodePoints);
        // Advance past 0 or more consecutive Unicode code points at the
        // beginning of the specified 'string' having the specified 'length'
        // (in bytes), until either the specified 'numCodePoints' or 'length'
        // bytes have been traversed (whichever occurs first), and return the
        // number of Unicode code points traversed.  Set the specified
        // '*result' to the address of the byte immediately following the last
        // code point traversed, or to 'string' if 'length' or 'numCodePoints'
        // is 0.  'string' need not be null-terminated and can contain embedded
        // null bytes, and 'string' may be null if '0 == length' (see {Empty
        // Input Strings}).  The behavior is undefined unless the initial
        // 'length' bytes of 'string' contain valid UTF-8 and
        // '0 <= numCodePoints'.  Note that the value returned will be in the
        // range '[0 .. numCodePoints]'.  Also note that 'string' may contain
        // less than 'length' Unicode code points.

    static IntPtr advanceRaw(const char              **result,
                             const bsl::string_view&   string,
                             IntPtr                    numCodePoints);
        // Advance past 0 or more consecutive Unicode code points at the
        // beginning of the specified 'string', until either the specified
        // 'numCodePoints' bytes or the whole string have been traversed
        // (whichever occurs first), and return the number of Unicode code
        // points traversed.  Set the specified '*result' to the address of the
        // byte immediately following the last code point traversed, or to
        // 'string' if 'length' or 'numCodePoints' is 0.  'string' need not be
        // null-terminated and can contain embedded null bytes.  The behavior
        // is undefined unless 'string' contains only valid UTF-8 characters
        // and '0 <= numCodePoints'.  Note that the value returned will be in
        // the range '[0 .. numCodePoints]'.  Also note that 'string' may
        // contain less than 'length' Unicode code points.

    static int appendUtf8Character(bsl::string  *output,
                                   unsigned int  codePoint);
        // !DEPRECATED!: Use 'appendUtf8CodePoint' instead.
        //
        // Append the UTF-8 encoding of the specified Unicode 'codePoint' to
        // the specified 'output' string.  Return 0 on success, and a non-zero
        // value otherwise.

    static int appendUtf8CodePoint(bsl::string  *output,
                                   unsigned int  codePoint);
    static int appendUtf8CodePoint(std::string  *output,
                                   unsigned int  codePoint);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    static int appendUtf8CodePoint(std::pmr::string  *output,
                                   unsigned int       codePoint);
#endif
        // Append the UTF-8 encoding of the specified Unicode 'codePoint' to
        // the specified 'output' string.  Return 0 on success, and a non-zero
        // value otherwise.

    static int getByteSize(const char *codePoint);
        // !DEPRECATED!: Use 'numBytesInCodePoint' instead.
        //
        // Return the length (in bytes) of the UTF-8-encoded code point
        // beginning at the specified 'codePoint'.  The behavior is undefined
        // unless 'codePoint' addresses a code point of valid UTF-8.  Note
        // that the value returned will be in the range '[1 .. 4]'.  Also note
        // that 1 is returned if '0 == *codePoint' since '\0' is a valid 1-byte
        // encoding.

    static int numBytesInCodePoint(const char *codePoint);
        // Return the length (in bytes) of the UTF-8-encoded code point
        // beginning at the specified 'codePoint'.  The behavior is undefined
        // unless 'codePoint' addresses a code point of valid UTF-8.  Note
        // that the value returned will be in the range '[1 .. 4]'.  Also note
        // that 1 is returned if '0 == *codePoint' since '\0' is a valid 1-byte
        // encoding.

    static int getLineAndColumnNumber(Uint64         *lineNumber,
                                      Uint64         *utf8Column,
                                      Uint64         *startOfLineByteOffset,
                                      bsl::streambuf *input,
                                      Uint64          byteOffset);
    static int getLineAndColumnNumber(Uint64         *lineNumber,
                                      Uint64         *utf8Column,
                                      Uint64         *startOfLineByteOffset,
                                      bsl::streambuf *input,
                                      Uint64          byteOffset,
                                      char            lineDelimeter);
        // For the specified 'byteOffset' in the specified 'input', load the
        // offset's line number into the specified 'lineNumber', the column
        // number into the specified 'utf8Column', and the byte offset for the
        // start of the line into 'startOfLineByteOffset'.  Optionally specify
        // 'lineDelimeter' used to the determine line separator.   If
        // 'lineDelimeter' is not supplied, lines are delimeted using '\n'.
        // Return 0 on success, or a non-zero value if 'location' cannot be
        // found in 'input' or if 'input' contains non-UTF-8 characters.  The
        // 'utf8Column' is the number of UTF-8 code points between
        // 'startOfLineByteOffset' and 'byteOffset'.

    static bool isValid(const char *string);
        // Return 'true' if the specified 'string' contains valid UTF-8, and
        // 'false' otherwise.  'string' is necessarily null-terminated, so it
        // cannot contain embedded null bytes.

    static bool isValid(const char *string, size_type length);
        // Return 'true' if the specified 'string' having the specified
        // 'length' (in bytes) contains valid UTF-8, and 'false' otherwise.
        // 'string' need not be null-terminated and can contain embedded null
        // bytes, and 'string' may be null if '0 == length' (see {Empty Input
        // Strings}).

    static bool isValid(const bsl::string_view& string);
        // Return 'true' if the specified 'string' contains valid UTF-8, and
        // 'false' otherwise.  'string' need not be null-terminated and can
        // contain embedded null bytes.

    static bool isValid(const char **invalidString, const char *string);
        // Return 'true' if the specified 'string' contains valid UTF-8, and
        // 'false' otherwise.  If 'string' contains invalid UTF-8, load into
        // the specified 'invalidString' the address of the beginning of the
        // first invalid UTF-8 sequence encountered; 'invalidString' is
        // unaffected if 'string' contains only valid UTF-8.  'string' is
        // necessarily null-terminated, so it cannot contain embedded null
        // bytes.

    static bool isValid(const char **invalidString,
                        const char  *string,
                        size_type    length);
        // Return 'true' if the specified 'string' having the specified
        // 'length' (in bytes) contains valid UTF-8, and 'false' otherwise.  If
        // 'string' contains invalid UTF-8, load into the specified
        // 'invalidString' the address of the byte after the last valid code
        // point traversed; 'invalidString' is unaffected if 'string' contains
        // only valid UTF-8.  'string' need not be null-terminated and can
        // contain embedded null bytes, and 'string' may be null if
        // '0 == length' (see {Empty Input Strings}).

    static bool isValid(const char              **invalidString,
                        const bsl::string_view&   string);
        // Return 'true' if the specified 'string' contains only valid UTF-8
        // characters, and 'false' otherwise.  If 'string' contains invalid
        // UTF-8, load into the specified 'invalidString' the address of the
        // byte after the last valid code point traversed; 'invalidString' is
        // unaffected if 'string' contains only valid UTF-8.  'string' need not
        // be null-terminated and can contain embedded null bytes.

    static bool isValidCodePoint(int        *status,
                                 const char *codePoint,
                                 size_type   numBytes);
        // If the specified 'codePoint' (having at least the specified
        // 'numBytes') refers to a valid UTF-8 code point then return 'true'
        // and load the specified 'status' with the number of bytes in the
        // code-point; otherwise, if 'codePoint' is not a valid code-point,
        // return 'false' and load 'status' with one of the (negative)
        // 'ErrorStatus' constants.  The behavior is undefined unless
        // 'numBytes > 0'.

    static IntPtr numBytesIfValid(const bsl::string_view& string,
                                  IntPtr                  numCodePoints);
        // !DEPRECATED!: Use 'numBytesRaw' instead.
        //
        // Return the length (in bytes) of the specified 'numCodePoints' UTF-8
        // encodings in the specified 'string', or a value less than 0 if
        // 'string' contains less than 'numCodePoints' encodings.  The behavior
        // is undefined unless 'string' refers to valid UTF-8.  Note that
        // 'string' may contain more than 'numCodePoints' encodings in which
        // case the trailing ones are ignored.

    static IntPtr numBytesRaw(const bsl::string_view& string,
                              IntPtr                  numCodePoints);
        // Return the length (in bytes) of the specified 'numCodePoints' UTF-8
        // encodings in the specified 'string', or a value less than 0 if
        // 'string' contains less than 'numCodePoints' encodings.  The behavior
        // is undefined unless 'string' refers to valid UTF-8.  Note that
        // 'string' may contain more than 'numCodePoints' encodings in which
        // case the trailing ones are ignored.

    static IntPtr numCharacters(const char *string);
        // !DEPRECATED!: Use 'numCodePointsRaw' instead.
        //
        // Return the number of Unicode code points in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' Unicode code points.

    static IntPtr numCharacters(const char *string, size_type length);
        // !DEPRECATED!: Use 'numCodePointsRaw' instead.
        //
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null bytes, and 'string'
        // may be null if '0 == length' (see {Empty Input Strings}).  The
        // behavior is undefined unless 'string' contains valid UTF-8.  Note
        // that 'string' may contain less than 'length' Unicode code points.

    static IntPtr numCharactersIfValid(const char **invalidString,
                                       const char  *string);
        // !DEPRECATED!: Use 'numCodePointsIfValid' instead.
        //
        // Return the number of Unicode code points in the specified 'string'
        // if it contains valid UTF-8, with no effect on the specified
        // 'invalidString'.  Otherwise, return a negative value and load into
        // 'invalidString' the address of the byte after the last valid Unicode
        // code point traversed.  'string' is necessarily null-terminated, so
        // it cannot contain embedded null bytes.  Note that 'string' may
        // contain less than 'bsl::strlen(string)' Unicode code points.

    static IntPtr numCharactersIfValid(const char **invalidString,
                                       const char  *string,
                                       size_type    length);
        // !DEPRECATED!: Use 'numCodePointsIfValid' instead.
        //
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes) if 'string' contains valid
        // UTF-8, with no effect on the specified 'invalidString'.  Otherwise,
        // return a negative value and load into 'invalidString' the address of
        // the byte after the last valid Unicode code point traversed.
        // 'string' need not be null-terminated and may contain embedded null
        // bytes, and 'string' may be null if '0 == length' (see {Empty Input
        // Strings}).  Note that 'string' may contain less than 'length'
        // Unicode code points.

    static IntPtr numCharactersRaw(const char *string);
        // !DEPRECATED!: Use 'numCodePointsRaw' instead.
        //
        // Return the number of Unicode code points in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' Unicode code points.

    static IntPtr numCharactersRaw(const char *string, size_type length);
        // !DEPRECATED!: Use 'numCodePointsRaw' instead.
        //
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null bytes, and 'string'
        // may be null if '0 == length' (see {Empty Input Strings}).  The
        // behavior is undefined 'string' contains valid UTF-8.  Note that
        // 'string' may contain less than 'length' Unicode code points.

    static IntPtr numCodePointsIfValid(const char **invalidString,
                                       const char  *string);
        // Return the number of Unicode code points in the specified 'string'
        // if it contains valid UTF-8, with no effect on the specified
        // 'invalidString'.  Otherwise, return a value from the 'ErrorStatus'
        // 'enum' (which are all negative) and load into 'invalidString' the
        // address of the byte after the last valid Unicode code point
        // traversed.  'string' is necessarily null-terminated, so it cannot
        // contain embedded null bytes.  Note that 'string' may contain less
        // than 'bsl::strlen(string)' Unicode code points.

    static IntPtr numCodePointsIfValid(const char **invalidString,
                                       const char  *string,
                                       size_type    length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes) if 'string' contains valid
        // UTF-8, with no effect on the specified 'invalidString'.  Otherwise,
        // return a value from the 'ErrorStatus' 'enum' (which are all
        // negative) and load into 'invalidString' the address of the byte
        // after the last valid Unicode code point traversed.  'string' need
        // not be null-terminated and may contain embedded null bytes, and
        // 'string' may be null if '0 == length' (see {Empty Input Strings}).
        // Note that 'string' may contain less than 'length' Unicode code
        // points.

    static IntPtr numCodePointsIfValid(const char              **invalidString,
                                       const bsl::string_view&   string);
        // Return the number of Unicode code points in the specified 'string'
        // if 'string' contains valid UTF-8, with no effect on the specified
        // 'invalidString'.  Otherwise, return a value from the 'ErrorStatus'
        // 'enum' (which are all negative) and load into 'invalidString' the
        // address of the byte after the last valid Unicode code point
        // traversed.  'string' need not be null-terminated and may contain
        // embedded null bytes.

    static IntPtr numCodePointsRaw(const char *string);
        // Return the number of Unicode code points in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' Unicode code points.

    static IntPtr numCodePointsRaw(const char *string, size_type length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null bytes, and 'string'
        // may be null if '0 == length' (see {Empty Input Strings}).  The
        // behavior is undefined unless 'string' contains valid UTF-8.  Note
        // that 'string' may contain less than 'length' Unicode code points.

    static IntPtr numCodePointsRaw(const bsl::string_view& string);
        // Return the number of Unicode code points in the specified 'string'.
        // 'string' need not be null-terminated and can contain embedded null
        // bytes.  The behavior is undefined unless 'string' contains valid
        // UTF-8.

    static size_type readIfValid(int            *status,
                                 char           *outputBuffer,
                                 size_type       outputBufferLength,
                                 bsl::streambuf *input);
        // Read from the specified 'input' and copy *valid* UTF-8 (only) to the
        // specified 'outputBuffer' having the specified 'outputBufferLength'
        // (in bytes).  Load the specified 'status' with:
        //: o 0 if 'input' reached 'eof' without encountering any invalid UTF-8
        //:   or prematurely exhausting 'outputBuffer'.
        //:
        //: o A positive value if 'input' was not completely read due to
        //:   'outputBuffer' being filled (or nearly filled) without
        //:   encountering any invalid UTF-8.
        //:
        //: o A negative value from 'ErrorStatus' if invalid UTF-8 was
        //:   encountered (without having written the invalid sequence to
        //:   'outputBuffer').
        // Return the number of bytes of valid UTF-8 written to 'outputBuffer.
        // If no invalid UTF-8 is encountered, or if 'input' supports
        // 'sputbackc' with a putback buffer capacity of at least 4 bytes,
        // 'input' will be left positioned at the end of the valid UTF-8 read,
        // otherwise, 'input' will be left in an unspecified state.  The
        // behavior is undefined unless '4 <= outputBufferLength'.  Note that
        // this function will stop reading 'input' when less than 4 bytes of
        // space remain in 'outputBuffer' to prevent the possibility of a
        // 4-byte UTF-8 sequence being truncated partway through.

    static const char *toAscii(IntPtr value);
        // Return the non-modifiable string representation of the 'ErrorStatus'
        // enumerator matching the specified 'value', if it exists, and "(*
        // unrecognized value *)" otherwise.  The string representation of an
        // enumerator that matches 'value' is the enumerator name with the "k_"
        // prefix elided.  Note that this method may be used to aid in
        // interpreting status values that are returned from some methods in
        // this utility.  See 'ErrorStatus'.
};

                          // =======================
                          // struct Utf8Util_ImpUtil
                          // =======================

struct Utf8Util_ImpUtil {
    // [!PRIVATE!] This struct provides a namespace for static methods used to
    // implement 'Utf8Util'.  Note that the functions are not typically useful
    // for clients, and are primarily exposed to allow for more thorough
    // testing.

    // TYPES
    typedef bsls::Types::Uint64 Uint64;

    // CLASS METHODS
    static int getLineAndColumnNumber(
                                  Uint64         *lineNumber,
                                  Uint64         *utf8Column,
                                  Uint64         *startOfLineByteOffset,
                                  bsl::streambuf *input,
                                  Uint64          byteOffset,
                                  char            lineDelimeter,
                                  char           *temporaryReadBuffer,
                                  int             temporaryReadBufferNumBytes);
        // For the specified 'byteOffset' in the specified 'input', load the
        // byte offset's line number into the specified 'lineNumber', the
        // column number into the specified 'utf8Column', and the byte offset
        // for the start of the line into the specified
        // 'startOfLineByteOffset', using the specified 'lineDelimeter' as the
        // line separator, and using the specified 'temporaryReadBuffer' (of
        // the specified length 'temporaryReadBufferNumBytes') as a temporary
        // buffer for reading.  Return 0 on success, or a non-zero value if
        // 'location' cannot be found in 'input' or if 'input' contains
        // non-UTF-8 characters.  The 'utf8Column' is the number of UTF-8 code
        // points between 'startOfLineByteOffset' and 'byteOffset'.  The
        // behavior is undefined unless 'temporaryReadBuffer' refers to a valid
        // buffer of at least 'temporaryReadBufferNumBytes' bytes, and
        // 'temporaryReadBufferNumBytes' is greater than or equal to 4.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct Utf8Util
                              // ---------------

// CLASS METHODS
inline
Utf8Util::IntPtr Utf8Util::advanceIfValid(
                                       int                      *status,
                                       const char              **result,
                                       const bsl::string_view&   string,
                                       IntPtr                    numCodePoints)
{

    return advanceIfValid(status,
                          result,
                          string.data(),
                          string.length(),
                          numCodePoints);
}

inline
Utf8Util::IntPtr Utf8Util::advanceRaw(const char              **result,
                                      const bsl::string_view&   string,
                                      IntPtr                    numCodePoints)
{
    return advanceRaw(result, string.data(), string.length(), numCodePoints);
}

inline
int Utf8Util::appendUtf8Character(bsl::string  *output,
                                  unsigned int  codePoint)
{
    return appendUtf8CodePoint(output, codePoint);
}

inline
int Utf8Util::getByteSize(const char *codePoint)
{
    return numBytesInCodePoint(codePoint);
}

inline
int Utf8Util::getLineAndColumnNumber(Uint64         *lineNumber,
                                     Uint64         *utf8Column,
                                     Uint64         *startOfLineByteOffset,
                                     bsl::streambuf *input,
                                     Uint64          byteOffset)
{
    return getLineAndColumnNumber(lineNumber,
                                  utf8Column,
                                  startOfLineByteOffset,
                                  input,
                                  byteOffset,
                                  '\n');
}

inline
int Utf8Util::getLineAndColumnNumber(Uint64         *lineNumber,
                                     Uint64         *utf8Column,
                                     Uint64         *startOfLineByteOffset,
                                     bsl::streambuf *input,
                                     Uint64          byteOffset,
                                     char            lineDelimeter)
{
    enum { k_BUFFER_SIZE = 2048 };
    char buffer[k_BUFFER_SIZE];
    return Utf8Util_ImpUtil::getLineAndColumnNumber(lineNumber,
                                                    utf8Column,
                                                    startOfLineByteOffset,
                                                    input,
                                                    byteOffset,
                                                    lineDelimeter,
                                                    buffer,
                                                    k_BUFFER_SIZE);
}

inline
bool Utf8Util::isValid(const char *string)
{
    BSLS_ASSERT(string);

    const char *dummy = 0;
    return isValid(&dummy, string);
}

inline
bool Utf8Util::isValid(const char *string, size_type length)
{
    BSLS_ASSERT(string || 0 == length);

    const char *dummy = 0;
    return isValid(&dummy, string, length);
}

inline
bool Utf8Util::isValid(const bsl::string_view& string)
{
    const char *dummy = 0;
    return isValid(&dummy, string);
}

inline
Utf8Util::IntPtr Utf8Util::numBytesIfValid(
                                         const bsl::string_view& string,
                                         IntPtr                  numCodePoints)
{
    return numBytesRaw(string, numCodePoints);
}

inline
Utf8Util::IntPtr Utf8Util::numCharacters(const char *string)
{
    return numCodePointsRaw(string);
}

inline
Utf8Util::IntPtr Utf8Util::numCharacters(const char *string, size_type length)
{
    return numCodePointsRaw(string, length);
}

inline
Utf8Util::IntPtr Utf8Util::numCharactersIfValid(const char **invalidString,
                                                const char  *string)
{
    return numCodePointsIfValid(invalidString, string);
}

inline
Utf8Util::IntPtr Utf8Util::numCharactersIfValid(const char **invalidString,
                                                const char  *string,
                                                size_type    length)
{
    return numCodePointsIfValid(invalidString, string, length);
}

inline
Utf8Util::IntPtr Utf8Util::numCharactersRaw(const char *string)
{
    return numCodePointsRaw(string);
}

inline
Utf8Util::IntPtr Utf8Util::numCharactersRaw(const char *string,
                                            size_type   length)
{
    return numCodePointsRaw(string, length);
}

inline
Utf8Util::IntPtr Utf8Util::numCodePointsRaw(const bsl::string_view& string)
{
    return numCodePointsRaw(string.data(), string.length());
}

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
