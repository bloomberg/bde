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
//  bdlde::Utf8Util::appendUtf8Character(&string, 0xff00);
//  bdlde::Utf8Util::appendUtf8Character(&string, 0x856);
//  bdlde::Utf8Util::appendUtf8Character(&string, 'a');
//  bdlde::Utf8Util::appendUtf8Character(&string, 0x1008aa);
//  bdlde::Utf8Util::appendUtf8Character(&string, 0xfff);
//  bdlde::Utf8Util::appendUtf8Character(&string, 'w');
//  bdlde::Utf8Util::appendUtf8Character(&string, 0x1abcd);
//  bdlde::Utf8Util::appendUtf8Character(&string, '.');
//  bdlde::Utf8Util::appendUtf8Character(&string, '\n');
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
// Next, we encode a lone surrogate value, which is not allowed, using
// 'utf8Append' instead of 'appendUtf8Character' to avoid validation:
//..
//  bsl::string stringWithSurrogate = string;
//  utf8Append(&stringWithSurrogate, 0xd8ab);
//..
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
//  assert(bdlde::Utf8Util::e_SURROGATE == rc);
//  assert(invalidPosition == stringWithSurrogate.data() + string.length());
//
//  invalidPosition = 0;  // reset
//
//  rc = bdlde::Utf8Util::numCodePointsIfValid(&invalidPosition,
//                                             stringWithSurrogate.c_str());
//  assert(rc < 0);
//  assert(bdlde::Utf8Util::e_SURROGATE == rc);
//  assert(invalidPosition == stringWithSurrogate.data() + string.length());
//..
// Now, we encode 0, which is allowed.  However, note that we cannot use any
// interfaces that take a null-terminated string for this case:
//..
//  bsl::string stringWithNull = string;
//  utf8AppendOneByte(&stringWithNull, 0);
//..
//  assert(true == bdlde::Utf8Util::isValid(stringWithNull.data(),
//                                          stringWithNull.length()));
//
//  assert(  10 == bdlde::Utf8Util::numCodePointsRaw(stringWithNull.data(),
//                                                   stringWithNull.length()));
//..
// Finally, we encode '0x61' ('a') as an overlong value using 2 bytes, which is
// not valid UTF-8 (since 'a' can be "encoded" in 1 byte):
//..
//  bsl::string stringWithOverlong = string;
//  utf8AppendTwoBytes(&stringWithOverlong, 'a');
//
//  assert(false == bdlde::Utf8Util::isValid(stringWithOverlong.data(),
//                                           stringWithOverlong.length()));
//  assert(false == bdlde::Utf8Util::isValid(stringWithOverlong.c_str()));
//
//  rc = bdlde::Utf8Util::numCodePointsIfValid(&invalidPosition,
//                                             stringWithOverlong.data(),
//                                             stringWithOverlong.length());
//  assert(rc < 0);
//  assert(bdlde::Utf8Util::e_NON_MINIMAL_ENCODING == rc);
//  assert(invalidPosition == stringWithOverlong.data() + string.length());
//
//  rc = bdlde::Utf8Util::numCodePointsIfValid(&invalidPosition,
//                                             stringWithOverlong.c_str());
//  assert(rc < 0);
//  assert(bdlde::Utf8Util::e_NON_MINIMAL_ENCODING == rc);
//  assert(invalidPosition == stringWithOverlong.data() + string.length());
//..
//
///Example 2: Advancing Code Points
/// - - - - - - - - - - - - - - - -
// For our usage example we will define some functions that can encode UTF-8,
// use them to build some strings, and observe how the functions defined in
// this class perform on them.
//..
//  void utf8AppendOneByte(bsl::string *string, int value)
//      // Append the specified 1-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert(0 == (value & ~0x7f));
//
//      *string += static_cast<char>(value);
//  }
//
//  void utf8AppendTwoBytes(bsl::string *string, int value)
//      // Append the specified 2-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert(0 == (value & ~0x7ff));
//
//      unsigned char buf[3];
//      buf[0] = static_cast<unsigned char>(((value & 0x7c0) >> 6) | 0xc0);
//      buf[1] = static_cast<unsigned char>( (value &  0x3f)       | 0x80);
//      buf[2] = 0;
//
//      *string += reinterpret_cast<char *>(buf);
//  }
//
//  void utf8AppendThreeBytes(bsl::string *string, int value)
//      // Append the specified 3-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert(0 == (value & ~0xffff));
//
//      unsigned char buf[4];
//      buf[0] = static_cast<unsigned char>(((value & 0xf000) >> 12) | 0xe0);
//      buf[1] = static_cast<unsigned char>(((value &  0xfc0) >>  6) | 0x80);
//      buf[2] = static_cast<unsigned char>( (value &   0x3f)        | 0x80);
//      buf[3] = 0;
//
//      *string += reinterpret_cast<char *>(buf);
//  }
//
//  void utf8AppendFourBytes(bsl::string *string, int value)
//      // Append the specified 4-byte UTF-8-encoded 'value' to the end of the
//      // specified 'string'.
//  {
//      assert(static_cast<unsigned>(value) <= 0x10ffff);
//
//      unsigned char buf[5];
//      buf[0] = static_cast<unsigned char>(((value & 0x1c0000) >> 18) | 0xf0);
//      buf[1] = static_cast<unsigned char>(((value &  0x3f000) >> 12) | 0x80);
//      buf[2] = static_cast<unsigned char>(((value &    0xfc0) >>  6) | 0x80);
//      buf[3] = static_cast<unsigned char>( (value &     0x3f)        | 0x80);
//      buf[4] = 0;
//
//      *string += reinterpret_cast<char *>(buf);
//  }
//
//  void utf8Append(bsl::string *string, unsigned int value)
//      // Append the specified UTF-8-encoded 'value' in the minimum number of
//      // bytes to the end of the specified 'string'.
//  {
//      assert(value <= 0x10ffff);
//
//      if (value <= 0x7f) {
//          utf8AppendOneByte(string, value);
//          return;                                                   // RETURN
//      }
//      if (value <= 0x7ff) {
//          utf8AppendTwoBytes(string, value);
//          return;                                                   // RETURN
//      }
//      if (value <= 0xffff) {
//          utf8AppendThreeBytes(string, value);
//          return;                                                   // RETURN
//      }
//
//      utf8AppendFourBytes(string, value);
//  }
//..
// In this example, we will use the various 'advance' functions to advance
// through a UTF-8 string.
//
// First, build the string using 'appendUtf8Character', keeping track of how
// many bytes are in each Unicode code point:
//..
//  bsl::string string;
//  bdlde::Utf8Util::appendUtf8Character(&string, 0xff00);        // 3 bytes
//  bdlde::Utf8Util::appendUtf8Character(&string, 0x1ff);         // 2 bytes
//  bdlde::Utf8Util::appendUtf8Character(&string, 'a');           // 1 byte
//  bdlde::Utf8Util::appendUtf8Character(&string, 0x1008aa);      // 4 bytes
//  bdlde::Utf8Util::appendUtf8Character(&string, 0x1abcd);       // 4 bytes
//  string += "\xe3\x8f\xfe";           // 3 bytes (invalid 3-byte sequence,
//                                      // the first 2 bytes are valid but the
//                                      // last continuation byte is invalid)
//  bdlde::Utf8Util::appendUtf8Character(&string, 'w');           // 1 byte
//  bdlde::Utf8Util::appendUtf8Character(&string, '\n');          // 1 byte
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
//.
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
///Example 3: Validating UTF-8 read from a 'bsl::streambuf':
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this usage example, we will demonstrate reading and validating UTF-8
// from a stream.
//
// First, we write a function to read valid UTF-8 to a 'bsl::string'.  We don't
// know how long the input will be, so we don't know how long to make the
// string, so we will grow the string in small, 32-byte increments.
//..
//  int utf8StreambufToString(bsl::string    *output,
//                            bsl::streambuf *sb)
//      // Read valid UTF-8 from the specified streambuf 'sb' to the specified
//      // 'output'.  Return 0 if the input was exhausted without encountering
//      // any invalid UTF-8, and a non-zero value otherwise.  If invalid UTF-8
//      // is encounted, log a mesage describing the problem after writing all
//      // the valid UTF-8 preceding it to 'output'.  Note that after the call,
//      // in no case will 'output' contain any invalid UTF-8.
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
//              // Buffer was full before the end of input was encounted.  Note
//              // that 'numBytes' may be up to 3 bytes less than
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
//              // Invalid UTF-8 encounted, the value of 'status' indicates the
//              // exact nature of the problem.  'numBytes' returned from the
//              // above call indicated the number of valid UTF-8 bytes read
//              // before encountering the invalid UTF-8.
//
//              BSLS_LOG_ERROR("Invalid UTF-8 error %s at position %u.\n",
//                             bdlde::Utf8Util::toErrorMessage(status),
//                             static_cast<unsigned>(output->length()));
//
//              return -1;                                            // RETURN
//          }
//      }
//  }
//..
// Then, in 'main', we try streaming an ASCII string (since ASCII is valid
// UTF-8),
//..
//  bsl::stringstream ss;
//  ss << "Hello, world!\n";
//  bsl::string out;
//  int rc = utf8StreambufToString(&out, ss.rdbuf());
//  assert(0 == rc);
//  assert("Hello, world!\n" == out);
//..
// and we observe that no error messages were logged.
//
// Next, we take a less trivial valid UTF-8 string of Chinese, and it's length,
// 'validLen':
//..
//  static const char validChineseUtf8[] = {
//      "\xe4\xb8\xad\xe5\x8d\x8e\xe4\xba\xba\xe6\xb0\x91\xe5\x85\xb1"
//      "\xe5\x92\x8c\xe5\x9b\xbd\xef\xbc\x8c\xe9\x80\x9a\xe7\xa7\xb0"
//      "\xe4\xb8\xad\xe5\x9b\xbd\x5b\xe6\xb3\xa8\x20\x33\x5d\xef\xbc"
//      "\x8c\xe6\x98\xaf\xe4\xbd\x8d\xe6\x96\xbc\xe4\xba\x9a\xe6\xb4"
//      "\xb2\xe6\x9d\xb1\xe9\x83\xa8\xe3\x80\x81\xe5\xa4\xaa\xe5\xb9"
//      "\xb3\xe6\xb4\x8b\xe8\xa5\xbf\xe5\xb2\xb8\xe7\x9a\x84\xe4\xb8"
//      "\x80\xe4\xb8\xaa\xe7\xa4\xbe\xe4\xbc\x9a\xe4\xb8\xbb\xe4\xb9"
//      "\x89\xe5\x9b\xbd\xe5\xae\xb6\xe3\x80\x82\xe9\xa6\x96\xe9\x83"
//      "\xbd\xe7\x82\xba\xe5\x8c\x97\xe4\xba\xac\xe3\x80\x82\xe5\x85"
//      "\xb6\xe9\x99\x86\xe5\x9c\xb0\xe7\x96\x86\xe5\x9f\x9f\xe8\x88"
//      "\x87\xe5\x91\xa8\xe9\x82\x8a\x31\x34\xe5\x80\x8b\xe5\x9c\x8b"
//      "\xe5\xae\xb6\xe6\x8e\xa5\xe5\xa3\xa4\xef\xbc\x8c\xe9\x99\x86"
//      "\xe5\x9c\xb0\xe5\x8f\x8a\xe6\xb9\x96\xe6\xb3\x8a\xe7\x9a\x84"
//      "\xe6\x80\xbb\xe9\x9d\xa2\xe7\xa9\x8d\xe7\xba\xa6\x39\x36\x30"
//      "\xe8\x90\xac\xe5\xb9\xb3\xe6\x96\xb9\xe5\x85\xac\xe9\x87\x8c"
//      "\x5b\x31\x31\x5d\x5b\x31\x32\x5d\x5b\x31\x33\x5d\xef\xbc\x8c"
//      "\xe6\x98\xaf\xe5\x85\xa8\xe4\xb8\x96\xe7\x95\x8c\xe9\x99\x86"
//      "\xe5\x9c\xb0" };
//  const bsl::size_t validLen = sizeof(validChineseUtf8) - 1;
//..
// Then, we print the length of the string:
//..
//  cout << "Length of Chinese string: " << validLen << endl;
//..
// and observe the output:
//..
//  Length of Chinese string: 258
//..
// Since our functions streams in only up to 32 bytes at a time, it will take
// our function several iterations to input this string.
//
// Next, we stream the valid Chinese UTF-8 into 'out':
//..
//  ss.str("");
//  ss << validChineseUtf8;
//  rc = utf8StreambufToString(&out, ss.rdbuf());
//  assert(0 == rc);
//  assert(validChineseUtf8 == out);
//..
// and we see that it succeeds and we observe that no error message is logged.
//
// Then, we create another string which is our valid Chinese UTF-8 with more
// appended to it.  The first byte of the appended string is '0xaf', which is a
// UTF-8 continuation octet, which will be unexpected, meaning that the string
// will have invalid UTF-8 at offset 258.
//..
//  bsl::string invalidUtf8Str(validChineseUtf8);
//  invalidUtf8Str += "\xaf Keep cool with Coolidge!";
//..
// Now, we attempt to stream in the invalid UTF-8:
//..
//  ss.str("");
//  ss << invalidUtf8Str;
//  rc = utf8StreambufToString(&out, ss.rdbuf());
//  assert(rc != 0);
//  assert(invalidUtf8Str != out);
//  assert(out.length() == validLen);
//  assert(validChineseUtf8 == out);
//..
// And finally, we see that everything before the invalid UTF-8 was
// successfully input, and the following message was logged by our function (on
// a single line):
//..
//  ERROR .../bdlde_utf8util.t.cpp:2612 Invalid UTF-8 error
//                             k_UNEXPECTED_CONTINUATION_OCTET at position 258.
//..

#include <bdlscm_version.h>

#include <bsls_assert.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

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
    typedef bsls::Types::size_type       size_type;
    typedef bsls::Types::IntPtr          IntPtr;

    enum ErrorStatus {
        // For those functions in this 'struct' for which invalid UTF-8 input
        // is not undefined behavior, if invalid UTF-8 is passed, either the
        // return value or the 'status' code returned from the argument list
        // are set to one of these values (all of which are negative) to
        // indicate which type of UTF-8 error occurred.

        k_END_OF_INPUT_TRUNCATION       = -3,
           // The end of input from a 'streambuf' was reached partway through a
           // multibyte UTF-8 sequence, or if the input was a buffer, the
           // buffer ended partway through a multibyte sequence.

        k_UNEXPECTED_CONTINUATION_OCTET = -4,
           // A continuation byte was encountered when not within a multi-bytes
           // sequence.

        k_NON_CONTINUATION_OCTET        = -5,
           // A non-continuation byte was encountered where a continuation byte
           // was expected.  Note that this may include a '\0' in a non
           // zero-terminated string that occurs where a continuation byte was
           // expected.

        k_NON_MINIMAL_ENCODING          = -6,
           // The unicode value encoded could have been encoded in a sequence
           // of fewer bytes.

        k_SEQUENCE_TOO_LONG             = -7,
           // The start of the sequence indicated that the sequence was over 4
           // bytes long.

        k_VALUE_TOO_LARGE               = -8,
           // A value larger than 0x10FFFF was encoded.

        k_SURROGATE                     = -9
           // Illegal occurrance of unicode code point reserved for surrogate
           // values in UTF-16.
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
        // from the 'Utf8Util::ReturnCode' 'enum' otherwise.  Set the specified
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
        // UTF-8 is encountered, and to a value from the 'Utf8Util::ReturnCode'
        // 'enum' otherwise.  Set the specified '*result' to the address of the
        // byte immediately following the last valid code point traversed, or
        // to 'string' if 'length' or 'numCodePoints' is 0.  'string' need not
        // be null-terminated and can contain embedded null bytes.  The
        // behavior is undefined unless '0 <= numCodePoints'.  Note that the
        // value returned will be in the range '[0 .. numCodePoints]'.  Also
        // note that 'string' may contain less than 'length' Unicode code
        // points.

    static IntPtr advanceRaw(const char **result,
                             const char  *string,
                             IntPtr       numCodePoints);
        // Advance past 0 or more consecutive Unicode code points at the
        // beginning of the specified 'string', until either the specified
        // 'numCodePoints' have been traversed or the terminating null byte is
        // encountered (whichever occurs first), and return the number of
        // Unicode code points traversed.  Set the specified '*result' to the
        // address of the byte immediately following the last code point
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
        // null bytes.  The behavior is undefined unless the initial 'length'
        // bytes of 'string' contain valid UTF-8 and '0 <= numCodePoints'.
        // Note that the value returned will be in the range
        // '[0 .. numCodePoints]'.  Also note that 'string' may contain less
        // than 'length' Unicode code points.

    static int appendUtf8Character(bsl::string  *output,
                                   unsigned int  codepoint);
        // Write the specified 'codepoint' unicode code point encoded in UTF-8
        // to the end of the specified 'output'.  Return 0 on success, and a
        // non-zero value otherwise.

    static int getByteSize(const char* codepoint);
        // Return the size in bytes of the specified UTF-8 'codepoint'.  The
        // behavior is undefined unless 'codepoint' points to a valid UTF-8
        // character in contiguous memory.  Note that a 'codepoint' pointing to
        // a '\0' 'char' will result in a return value of '1', since the '\0'
        // byte is a 1-byte encoding.

    static bool isValid(const char *string);
        // Return 'true' if the specified 'string' contains valid UTF-8, and
        // 'false' otherwise.  'string' is necessarily null-terminated, so it
        // cannot contain embedded null bytes.

    static bool isValid(const char *string, size_type length);
        // Return 'true' if the specified 'string' having the specified
        // 'length' (in bytes) contains valid UTF-8, and 'false' otherwise.
        // 'string' need not be null-terminated and can contain embedded null
        // bytes.

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
        // contain embedded null bytes.

    static IntPtr numBytesIfValid(const bslstl::StringRef& string,
                                  IntPtr                   numCodePoints);
        // Return the number of bytes used by the specified 'numCodePoints'
        // first utf8 characters in the specified 'string', or a value less
        // than zero if 'string' contains less than 'numCharacters' UTF-8
        // characters.  The behavior is undefined unless 'string' is a valid
        // UTF-8 string.

    static IntPtr numCharactersIfValid(const char **invalidString,
                                       const char  *string);
        // Return the number of Unicode code points in the specified 'string'
        // if it contains valid UTF-8, with no effect on the specified
        // 'invalidString'.  Otherwise, return a negative value and load into
        // 'invalidString' the address of the byte after the last valid Unicode
        // code point traversed.  'string' is necessarily null-terminated, so
        // it cannot contain embedded null bytes.  Note that 'string' may
        // contain less than 'bsl::strlen(string)' Unicode code points.
        //
        // DEPRECATED: Use 'numCodePointsIfValid' instead.

    static IntPtr numCharactersIfValid(const char **invalidString,
                                       const char  *string,
                                       size_type    length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes) if 'string' contains valid
        // UTF-8, with no effect on the specified 'invalidString'.  Otherwise,
        // return a negative value and load into 'invalidString' the address of
        // the byte after the last valid Unicode code point traversed.
        // 'string' need not be null-terminated and may contain embedded null
        // bytes.  Note that 'string' may contain less than 'length' Unicode
        // code points.
        //
        // DEPRECATED: Use 'numCodePointsIfValid' instead.

    static IntPtr numCharactersRaw(const char *string);
        // Return the number of Unicode code points in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' Unicode code points.
        //
        // DEPRECATED: Use 'numCodePointsRaw' instead.

    static IntPtr numCharactersRaw(const char *string, size_type length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null bytes.  The behavior
        // is undefined 'string' contains valid UTF-8.  Note that 'string' may
        // contain less than 'length' Unicode code points.
        //
        // DEPRECATED: Use 'numCodePointsRaw' instead.

    static IntPtr numCharacters(const char *string);
        // Return the number of Unicode code points in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' Unicode code points.
        //
        // DEPRECATED: Use 'numCodePointsRaw' instead.

    static IntPtr numCharacters(const char *string, size_type length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null bytes.  The behavior
        // is undefined unless 'string' contains valid UTF-8.  Note that
        // 'string' may contain less than 'length' Unicode code points.
        //
        // DEPRECATED: Use 'numCodePointsRaw' instead.

    static IntPtr numCodePointsIfValid(const char **invalidString,
                                       const char  *string);
        // Return the number of Unicode code points in the specified 'string'
        // if it contains valid UTF-8, with no effect on the specified
        // 'invalidString'.  Otherwise, return a value from the
        // 'Utf8Util::ReturnCode' 'enum' (which are all negative) and load into
        // 'invalidString' the address of the byte after the last valid Unicode
        // code point traversed.  'string' is necessarily null-terminated, so
        // it cannot contain embedded null bytes.  Note that 'string' may
        // contain less than 'bsl::strlen(string)' Unicode code points.

    static IntPtr numCodePointsIfValid(const char **invalidString,
                                       const char  *string,
                                       size_type    length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes) if 'string' contains valid
        // UTF-8, with no effect on the specified 'invalidString'.  Otherwise,
        // return a value from the 'Utf8Util::ReturnCode' 'enum' (which are all
        // negative) and load into 'invalidString' the address of the byte
        // after the last valid Unicode code point traversed.  'string' need
        // not be null-terminated and may contain embedded null bytes.  Note
        // that 'string' may contain less than 'length' Unicode code points.

    static IntPtr numCodePointsRaw(const char *string);
        // Return the number of Unicode code points in the specified 'string'.
        // 'string' is necessarily null-terminated, so it cannot contain
        // embedded null bytes.  The behavior is undefined unless 'string'
        // contains valid UTF-8.  Note that 'string' may contain less than
        // 'bsl::strlen(string)' Unicode code points.

    static IntPtr numCodePointsRaw(const char *string, size_type length);
        // Return the number of Unicode code points in the specified 'string'
        // having the specified 'length' (in bytes).  'string' need not be
        // null-terminated and can contain embedded null bytes.  The behavior
        // is undefined unless 'string' contains valid UTF-8.  Note that
        // 'string' may contain less than 'length' Unicode code points.

    static size_type readIfValid(int            *status,
                                 char           *outputBuffer,
                                 size_type       outputBufferLength,
                                 bsl::streambuf *input);
        // Read from the specified 'input', validating the UTF-8 in the
        // process, copying correct UTF-8 input into the specified
        // 'outputBuffer' of length 'outputBufferLength'.  Always return the
        // number of bytes of valid UTF-8 written into the buffer.  If a UTF-8
        // error is encountered in the input, set the specified 'status' to a
        // value from the 'ErrorStatus' enum (all of which are negative)
        // indicating what type of error occurred.  Otherwise, if end of input
        // was reached, return 0, and return a positive value if the output
        // buffer was full or nearly full before end of input was reached.  The
        // contents of 'outputBuffer' following the correct UTF-8 is
        // unspecified.  The behavior is undefined if 'outputBufferLength < 4'.
        // Note that a positive value may be returned with up to 3 unfilled
        // bytes at the end of 'outputBuffer'.

    static const char *toErrorMessage(int errorStatus);
        // Return a null-terminated string describing which value of the
        // 'ErrorStatus' enum is matched by 'errorStatus', or
        // '(* unrecognized value *)' if no value in 'ErrorStatus' is matched.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct Utf8Util
                              // ---------------

// CLASS METHODS
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
    BSLS_ASSERT(string);

    const char *dummy = 0;

    return isValid(&dummy, string, length);
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
