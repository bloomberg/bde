// bdlde_utf8util.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlde_utf8util.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlde_utf8util_cpp,"$Id$ $CSID$")

#include <bsla_fallthrough.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_fstream.h>
#include <bsl_ios.h>
#include <bsl_streambuf.h>

// LOCAL MACROS

#define UNLIKELY(EXPRESSION) BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(EXPRESSION)
#define LIKELY(  EXPRESSION) BSLS_PERFORMANCEHINT_PREDICT_LIKELY(EXPRESSION)

// LOCAL CONSTANTS

namespace {

using namespace BloombergLP;

typedef bdlde::Utf8Util Utf8Util;

enum {
    k_MIN_2_BYTE_VALUE = 0x80,     // min value that requires 2 bytes to encode
    k_MIN_3_BYTE_VALUE = 0x800,    // min value that requires 3 bytes to encode
    k_MIN_4_BYTE_VALUE = 0x10000,  // min value that requires 4 bytes to encode

    k_MIN_SURROGATE    = 0xd800,   // min surrogate value

    k_MAX_VALID        = 0x10ffff, // max value that can be encoded in UTF-8

    k_CONT_VALUE_MASK  = 0x3f,     // part of a continuation byte that
                                   // contains the 6 bits of value info

    k_ONEBYTEHEAD_TEST   = 0x80,   // 1 byte Utf8
    k_ONEBYTEHEAD_RES    = 0,

    k_TWOBYTEHEAD_TEST   = 0xE0,   // 2 byte Utf8
    k_TWOBYTEHEAD_RES    = 0XC0,

    k_THREEBYTEHEAD_TEST = 0xF0,   // 3 byte Utf8
    k_THREEBYTEHEAD_RES  = 0XE0,

    k_FOURBYTEHEAD_TEST  = 0xF8,   // 4 byte Utf8
    k_FOURBYTEHEAD_RES   = 0XF0,

    k_MULTIPLEBYTE_TEST  = 0xC0,   // continuation byte
    k_MULTIPLEBYTE_RES   = 0X80,

    k_END_OF_INPUT_TRUNCATION      = Utf8Util::k_END_OF_INPUT_TRUNCATION,
    k_UNEXPECTED_CONTINUATION_OCTET= Utf8Util::k_UNEXPECTED_CONTINUATION_OCTET,
    k_NON_CONTINUATION_OCTET       = Utf8Util::k_NON_CONTINUATION_OCTET,
    k_OVERLONG_ENCODING            = Utf8Util::k_OVERLONG_ENCODING,
    k_INVALID_INITIAL_OCTET        = Utf8Util::k_INVALID_INITIAL_OCTET,
    k_VALUE_LARGER_THAN_0X10FFFF   = Utf8Util::k_VALUE_LARGER_THAN_0X10FFFF,
    k_SURROGATE                    = Utf8Util::k_SURROGATE
};

#if defined(BSLS_ASSERT_SAFE_IS_USED)

bool isValidUtf8CodePoint(const char *sequence)
    // Return 'true' if 'sequence' points to a valid UTF-8 code point and
    // 'false' otherwise.  Note that this checks for neither:
    //: o values too large
    //:
    //: o surrogates
    //:
    //: o non-minimal encodings
{
    return (sequence[0] & k_ONEBYTEHEAD_TEST)   == k_ONEBYTEHEAD_RES ||
          ((sequence[1] & k_MULTIPLEBYTE_TEST)  == k_MULTIPLEBYTE_RES &&
          ((sequence[0] & k_TWOBYTEHEAD_TEST)   == k_TWOBYTEHEAD_RES ||
          ((sequence[2] & k_MULTIPLEBYTE_TEST)  == k_MULTIPLEBYTE_RES &&
          ((sequence[0] & k_THREEBYTEHEAD_TEST) == k_THREEBYTEHEAD_RES ||
          ((sequence[3] & k_MULTIPLEBYTE_TEST)  == k_MULTIPLEBYTE_RES &&
           (sequence[0] & k_FOURBYTEHEAD_TEST)  == k_FOURBYTEHEAD_RES )))));
}

#endif // defined(BSLS_ASSERT_SAFE_IS_USED)

int utf8Size(char character)
    // Return the length of the UTF-8 code point for which the specified
    // 'character' is the first 'char'.  The behavior is undefined unless
    // 'character' is the first 'char' of a UTF-8 code point.
{
    if ((character & k_ONEBYTEHEAD_TEST) == k_ONEBYTEHEAD_RES) {
        return 1;                                                     // RETURN
    }
    else if ((character & k_TWOBYTEHEAD_TEST) == k_TWOBYTEHEAD_RES) {
        return 2;                                                     // RETURN
    }
    else if ((character & k_THREEBYTEHEAD_TEST) == k_THREEBYTEHEAD_RES) {
        return 3;                                                     // RETURN
    }

    return 4;
}

// BDE_VERIFY pragma: -SP01     // 'FFFF' is not a typo.

template <class STRING>
int appendUtf8CodePointImpl(STRING *output, unsigned int codePoint)
    // Append the UTF-8 encoding of the specified Unicode 'codePoint' to the
    // specified 'output' string.  Return 0 on success, and a non-zero value
    // otherwise.
{
    ///IMPLEMENTATION NOTES
    ///--------------------
    // This UTF-8 documentation was copied verbatim from RFC 3629.  The
    // original version was downloaded from:
    //..
    //     http://tools.ietf.org/html/rfc3629
    //
    ///////////////////////// BEGIN VERBATIM RFC TEXT /////////////////////////
    //
    // Char number range   |        UTF-8 octet sequence
    //    (hexadecimal)    |              (binary)
    // --------------------+---------------------------------------------
    // 0000 0000-0000 007F | 0xxxxxxx
    // 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    // 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    // 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    //..
    ////////////////////////// END VERBATIM RFC TEXT //////////////////////////

    if (codePoint < 0x80U) {
        *output += static_cast<char>( codePoint);
        return 0;                                                     // RETURN
    }
    else if (codePoint < 0x800U) {
        *output += static_cast<char>( (codePoint >>  6)          | 0xC0);
        *output += static_cast<char>( (codePoint        & 0x3FU) | 0x80);
        return 0;                                                     // RETURN
    }
    else if (codePoint < 0x10000U) {
        *output += static_cast<char>(( codePoint >> 12)          | 0xE0);
        *output += static_cast<char>(((codePoint >>  6) & 0x3FU) | 0x80);
        *output += static_cast<char>(( codePoint        & 0x3FU) | 0x80);
        return 0;                                                     // RETURN
    }
    else if (codePoint < 0x110000U) {
        *output += static_cast<char>(( codePoint >> 18)          | 0xF0);
        *output += static_cast<char>(((codePoint >> 12) & 0x3FU) | 0x80);
        *output += static_cast<char>(((codePoint >>  6) & 0x3FU) | 0x80);
        *output += static_cast<char>(( codePoint        & 0x3FU) | 0x80);
        return 0;                                                     // RETURN
    }

    // Invalid code point.

    return k_VALUE_LARGER_THAN_0X10FFFF;
}

// BDE_VERIFY pragma: pop

}  // close unnamed namespace

// STATIC HELPER FUNCTIONS

static inline
bool isNotContinuation(char value)
    // Return 'true' if the specified 'value' is NOT a UTF-8 continuation byte,
    // and 'false' otherwise.
{
    return 0x80 != (value & 0xc0);
}

static inline
bool isSurrogateValue(int value)
    // Return 'true' if the specified 'value' is a surrogate value, and 'false'
    // otherwise.
{
    enum { k_SURROGATE_MASK = 0xfffff800U };

    return (k_SURROGATE_MASK & value) == k_MIN_SURROGATE;
}

static inline
int get2ByteValue(const char *pc)
    // Return the integral value of the single code point represented by the
    // 2-byte UTF-8 sequence referred to by the specified 'pc'.  The behavior
    // is undefined unless the 2 bytes starting at 'pc' contain a UTF-8
    // sequence describing a single valid code point.
{
    return ((*pc & 0x1f) << 6) | (pc[1] & k_CONT_VALUE_MASK);
}

static inline
int get3ByteValue(const char *pc)
    // Return the integral value of the single code point represented by the
    // 3-byte UTF-8 sequence referred to by the specified 'pc'.  The behavior
    // is undefined unless the 3 bytes starting at 'pc' contain a UTF-8
    // sequence describing a single valid code point.
{
    return ((*pc & 0xf) << 12) | ((pc[1] & k_CONT_VALUE_MASK) << 6)
                               |  (pc[2] & k_CONT_VALUE_MASK);
}

static inline
int get4ByteValue(const char *pc)
    // Return the integral value of the single code point represented by the
    // 4-byte UTF-8 sequence referred to by the specified 'pc'.  The behavior
    // is undefined unless the 4 bytes starting at 'pc' contain a UTF-8
    // sequence describing a single valid code point.
{
    return ((*pc & 0x7) << 18) | ((pc[1] & k_CONT_VALUE_MASK) << 12)
                               | ((pc[2] & k_CONT_VALUE_MASK) <<  6)
                               |  (pc[3] & k_CONT_VALUE_MASK);
}

static
int validateAndCountCodePoints(const char **invalidString, const char *string)
    // Return the number of Unicode code points in the specified 'string' if it
    // contains valid UTF-8, with no effect on the specified 'invalidString'.
    // Otherwise, return a negative value and load into 'invalidString' the
    // address of the first sequence in 'string' that does not constitute the
    // start of a valid UTF-8 encoding specifying a valid Unicode code point.
    // 'string' is necessarily null-terminated, so it cannot contain embedded
    // null bytes.  Note that 'string' may contain less than
    // 'bsl::strlen(string)' Unicode code points.
{
    // The following assertions are redundant with those in the CLASS METHODS.
    // Hence, 'BSLS_ASSERT_SAFE' is used.

    BSLS_ASSERT_SAFE(invalidString);
    BSLS_ASSERT_SAFE(string);

    int count = 0;

    while (true) {
        switch (static_cast<unsigned char>(*string) >> 4) {
          case 0: {
            if (UNLIKELY(!*string)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                return count;                                         // RETURN
            }

            ++string;
          } break;
          case 0x1: BSLA_FALLTHROUGH;
          case 0x2: BSLA_FALLTHROUGH;
          case 0x3: BSLA_FALLTHROUGH;
          case 0x4: BSLA_FALLTHROUGH;
          case 0x5: BSLA_FALLTHROUGH;
          case 0x6: BSLA_FALLTHROUGH;
          case 0x7: {
            ++string;
          } break;
          case 0x8: BSLA_FALLTHROUGH;
          case 0x9: BSLA_FALLTHROUGH;
          case 0xa: BSLA_FALLTHROUGH;
          case 0xb: {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *invalidString = string;

            return k_UNEXPECTED_CONTINUATION_OCTET;                   // RETURN
          } break;
          case 0xc: BSLA_FALLTHROUGH;
          case 0xd: {
            if (UNLIKELY(isNotContinuation(string[1]))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return string[1] ? k_NON_CONTINUATION_OCTET
                                 : k_END_OF_INPUT_TRUNCATION;         // RETURN
            }
            const int value = get2ByteValue(string);
            if (UNLIKELY(value < k_MIN_2_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return k_OVERLONG_ENCODING;                           // RETURN
            }
            string += 2;
          } break;
          case 0xe: {
            if (UNLIKELY(isNotContinuation(string[1]))
             || UNLIKELY(isNotContinuation(string[2]))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return isNotContinuation(string[1])
                       ? (string[1] ? k_NON_CONTINUATION_OCTET
                                    : k_END_OF_INPUT_TRUNCATION)
                       : (string[2] ? k_NON_CONTINUATION_OCTET
                                    : k_END_OF_INPUT_TRUNCATION);     // RETURN
            }

            const int value = get3ByteValue(string);
            if (UNLIKELY((value < k_MIN_3_BYTE_VALUE)
                      || isSurrogateValue(value))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return value < k_MIN_3_BYTE_VALUE ? k_OVERLONG_ENCODING
                                                  : k_SURROGATE;      // RETURN
            }
            string += 3;
          } break;
          case 0xf: {
            if (UNLIKELY(0x8 & *string)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;

                return k_INVALID_INITIAL_OCTET;                       // RETURN
            }

            if (UNLIKELY(isNotContinuation(string[1]))
             || UNLIKELY(isNotContinuation(string[2]))
             || UNLIKELY(isNotContinuation(string[3]))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;

                return   isNotContinuation(string[1])
                       ? (string[1] ? k_NON_CONTINUATION_OCTET
                                    : k_END_OF_INPUT_TRUNCATION)
                       : isNotContinuation(string[2])
                       ? (string[2] ? k_NON_CONTINUATION_OCTET
                                    : k_END_OF_INPUT_TRUNCATION)
                       : (string[3] ? k_NON_CONTINUATION_OCTET
                                    : k_END_OF_INPUT_TRUNCATION);     // RETURN
            }

            const int value = get4ByteValue(string);
            if (UNLIKELY((value < k_MIN_4_BYTE_VALUE)
                      || (value > k_MAX_VALID))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = string;
                return value > k_MAX_VALID
                     ? k_VALUE_LARGER_THAN_0X10FFFF
                     : k_OVERLONG_ENCODING;                           // RETURN
            }

            string += 4;
          } break;
          default: {
            BSLS_ASSERT_INVOKE_NORETURN("unreachable");
          }
        }

        ++count;
    }
}

static int validateAndCountCodePoints(const char             **invalidString,
                                      const char              *string,
                                      bsls::Types::size_type   length)
    // Return the number of Unicode code points in the specified 'string'
    // having the specified 'length' (in bytes) if 'string' contains valid
    // UTF-8, with no effect on the specified 'invalidString'.  Otherwise,
    // return a negative value and load into 'invalidString' the address of the
    // first byte in 'string' that does not constitute the start of a valid
    // UTF-8 encoding.  'string' need not be null-terminated and can contain
    // embedded null bytes.  The behavior is undefined unless
    // '0 <= IntPtr(length)'.  Note that 'string' may contain less than
    // 'length' Unicode code points.
{
    // The following assertions are redundant with those in the CLASS METHODS.
    // Hence, 'BSLS_ASSERT_SAFE' is used.

    BSLS_ASSERT_SAFE(invalidString);
    BSLS_ASSERT_SAFE(string || 0 == length);
    BSLS_ASSERT_SAFE(0 <= bsls::Types::IntPtr(length));

    if (0 == length) {
        return 0;                                                     // RETURN
    }

    const char       *pc     = string;
    const char *const pcEnd4 = string + length - 4;

    int count = 0;

    while (pc <= pcEnd4) {
        switch (static_cast<unsigned char>(*pc) >> 4) {
          case 0x0: BSLA_FALLTHROUGH;
          case 0x1: BSLA_FALLTHROUGH;
          case 0x2: BSLA_FALLTHROUGH;
          case 0x3: BSLA_FALLTHROUGH;
          case 0x4: BSLA_FALLTHROUGH;
          case 0x5: BSLA_FALLTHROUGH;
          case 0x6: BSLA_FALLTHROUGH;
          case 0x7: {
            ++pc;
          } break;
          case 0x8: BSLA_FALLTHROUGH;
          case 0x9: BSLA_FALLTHROUGH;
          case 0xa: BSLA_FALLTHROUGH;
          case 0xb: {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *invalidString = pc;

            return k_UNEXPECTED_CONTINUATION_OCTET;                   // RETURN
          } break;
          case 0xc: BSLA_FALLTHROUGH;
          case 0xd: {
            const int value = get2ByteValue(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                      || (value < k_MIN_2_BYTE_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return isNotContinuation(pc[1]) ? k_NON_CONTINUATION_OCTET
                                                : k_OVERLONG_ENCODING;
                                                                      // RETURN
            }

            pc += 2;
          } break;
          case 0xe: {
            const int value = get3ByteValue(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                      || isNotContinuation(pc[2])
                      || (value < k_MIN_3_BYTE_VALUE)
                      || isSurrogateValue(value))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;

                if (isNotContinuation(pc[1]) || isNotContinuation(pc[2])) {
                    return k_NON_CONTINUATION_OCTET;                  // RETURN
                }

                return value < k_MIN_3_BYTE_VALUE ? k_OVERLONG_ENCODING
                                                  : k_SURROGATE;      // RETURN
            }

            pc += 3;
          } break;
          case 0xf: {
            const int value = get4ByteValue(pc);
            if (UNLIKELY(bool(0x8 & *pc)
                      || isNotContinuation(pc[1])
                      || isNotContinuation(pc[2])
                      || isNotContinuation(pc[3])
                      || (value < k_MIN_4_BYTE_VALUE)
                      || (value > k_MAX_VALID))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;

                if (0x8 & *pc) {
                    return k_INVALID_INITIAL_OCTET;                   // RETURN
                }

                if (isNotContinuation(pc[1]) || isNotContinuation(pc[2]) ||
                                                isNotContinuation(pc[3])) {
                    return k_NON_CONTINUATION_OCTET;                  // RETURN
                }

                return k_MAX_VALID < value ? k_VALUE_LARGER_THAN_0X10FFFF
                                           : k_OVERLONG_ENCODING;     // RETURN
            }

            pc += 4;
          } break;
          default: {
            BSLS_ASSERT_INVOKE_NORETURN("unreachable");
          }
        }

        ++count;
    }

    length -= static_cast<int>(pc - string);

    // 'length' is now < 4.

    while (length > 0) {
        int delta;

        switch (static_cast<unsigned char>(*pc) >> 4) {
          case 0x0: BSLA_FALLTHROUGH;
          case 0x1: BSLA_FALLTHROUGH;
          case 0x2: BSLA_FALLTHROUGH;
          case 0x3: BSLA_FALLTHROUGH;
          case 0x4: BSLA_FALLTHROUGH;
          case 0x5: BSLA_FALLTHROUGH;
          case 0x6: BSLA_FALLTHROUGH;
          case 0x7: {
            delta = 1;
            ++count;
          } break;
          case 0x8: BSLA_FALLTHROUGH;
          case 0x9: BSLA_FALLTHROUGH;
          case 0xa: BSLA_FALLTHROUGH;
          case 0xb: {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *invalidString = pc;

            return k_UNEXPECTED_CONTINUATION_OCTET;                   // RETURN
          } break;
          case 0xc: BSLA_FALLTHROUGH;
          case 0xd: {
            if (UNLIKELY(length < 2)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return k_END_OF_INPUT_TRUNCATION;                     // RETURN
            }

            const int value = get2ByteValue(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                      || (value < k_MIN_2_BYTE_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return isNotContinuation(pc[1]) ? k_NON_CONTINUATION_OCTET
                                                : k_OVERLONG_ENCODING;
                                                                      // RETURN
            }

            delta = 2;
            ++count;
          } break;
          case 0xe: {
            if (UNLIKELY(length < 3)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return 2 == length && isNotContinuation(pc[1])
                     ? k_NON_CONTINUATION_OCTET
                     : k_END_OF_INPUT_TRUNCATION;                     // RETURN
            }

            const int value = get3ByteValue(pc);
            if (UNLIKELY(isNotContinuation(pc[1])
                      || isNotContinuation(pc[2])
                      || (value < k_MIN_3_BYTE_VALUE)
                      || isSurrogateValue(value))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *invalidString = pc;
                return (isNotContinuation(pc[1]) || isNotContinuation(pc[2]))
                      ? k_NON_CONTINUATION_OCTET
                      : value < k_MIN_3_BYTE_VALUE
                      ? k_OVERLONG_ENCODING
                      : k_SURROGATE;                                  // RETURN
            }

            delta = 3;
            ++count;
          } break;
          case 0xf: {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *invalidString = pc;

            if (UNLIKELY(0x8 & *pc)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // binary: 11111xxx: invalid code point in all UTF-8 contexts.

                return k_INVALID_INITIAL_OCTET;                       // RETURN
            }

            // binary: 11110xxx: legal start to 4 octet sequence

            for (unsigned ii = 1; ii < length; ++ii) {
                if (isNotContinuation(pc[ii])) {
                    return k_NON_CONTINUATION_OCTET;                  // RETURN
                }
            }

            return k_END_OF_INPUT_TRUNCATION;                         // RETURN
          } break;
          default: {
            BSLS_ASSERT_INVOKE_NORETURN("unreachable");
          }
        }

        pc     += delta;
        length -= delta;
    }

    BSLS_ASSERT(0 == length);
    BSLS_ASSERT(pcEnd4 + 4 == pc);

    return count;
}

namespace BloombergLP {

namespace bdlde {

                          // -----------------------
                          // struct Utf8Util_ImpUtil
                          // -----------------------

// CLASS METHODS
int Utf8Util_ImpUtil::getLineAndColumnNumber(
                                   Uint64         *lineNumber,
                                   Uint64         *utf8Column,
                                   Uint64         *startOfLineByteOffset,
                                   bsl::streambuf *input,
                                   Uint64          byteOffset,
                                   char            lineDelimeter,
                                   char           *temporaryReadBuffer,
                                   int             temporaryReadBufferNumBytes)
{
    BSLS_ASSERT(lineNumber);
    BSLS_ASSERT(utf8Column);
    BSLS_ASSERT(startOfLineByteOffset);
    BSLS_ASSERT(input);
    BSLS_ASSERT(temporaryReadBuffer);
    BSLS_ASSERT(temporaryReadBufferNumBytes >= 4);

    enum {
        k_LOCATION_NOT_FOUND = -100
    };
    int   k_BUFFER_SIZE = temporaryReadBufferNumBytes;
    char *buffer        = temporaryReadBuffer;

    // All of these values are in bytes *except* currentColumnCodePoint, also
    // lineNumber and currentColumn are computed using 0 based values (and +1
    // is added to the return)

    Uint64 targetOffset = byteOffset;

    Uint64 currentPosition    = 0;  // current byte offset in 'input'
    Uint64 currentLineNum     = 0;  // current line number in 'input'
    Uint64 startOfLinePostion = 0;  // start position of current line

    // 'remainder' is the number of bytes remaining to be processed in 'buffer'
    // on a new 'read' (if a code-point is split across reads)
    Uint64 remainder = 0;

    // 'currentColumnCodePoint' is the number of code-points from
    // 'startOfLinePosition'.
    Uint64 currentColumnCodePoint = 0;

    while (true) {

        bsl::streamsize attemptToRead =
                       static_cast<bsl::streamsize>(k_BUFFER_SIZE - remainder);
        bsl::streamsize numRead = input->sgetn(buffer + remainder,
                                               attemptToRead);

        Uint64 onePastLastPosInBuffer = currentPosition + numRead + remainder;
        remainder = 0;
        if ((numRead < attemptToRead) &&
            (targetOffset >= onePastLastPosInBuffer)) {
            // The target offset is past the end of the 'input'.
            return k_LOCATION_NOT_FOUND;                              // RETURN
        }

        // 'position' is the location of 'currentPosition' in 'buffer'.  This
        // is *not* buffer[currentPosition] as currentPosition is the offset
        // from the start of 'input'.
        const char *position = buffer;
        while (currentPosition < onePastLastPosInBuffer) {
            int status;
            bool isValid = Utf8Util::isValidCodePoint(
                        &status,
                        position,
                        static_cast<Utf8Util::size_type>(onePastLastPosInBuffer
                                                           - currentPosition));

            if (!isValid && status != k_END_OF_INPUT_TRUNCATION) {
              // Invalid code point.
              return status;                                          // RETURN
            }

            if (!isValid) {
                BSLS_ASSERT(status == k_END_OF_INPUT_TRUNCATION);

                // The current code-point is split across buffers.
                remainder = onePastLastPosInBuffer - currentPosition;

                BSLS_ASSERT(remainder >= 1 && remainder < 4);

                switch (remainder) {
                  case 3: buffer[2] = position[2]; // FALLTHROUGH
                  case 2: buffer[1] = position[1]; // FALLTHROUGH
                  case 1: buffer[0] = position[0];
                    break;                                             // BREAK
                  default:
                    BSLS_ASSERT_INVOKE_NORETURN("unreachable");
                }
                break;                                                 // BREAK
            }
            int numBytesInCodePoint = status;
            if (targetOffset < (currentPosition + numBytesInCodePoint)) {
                *lineNumber      = currentLineNum + 1;
                *utf8Column            = currentColumnCodePoint + 1;
                *startOfLineByteOffset = startOfLinePostion;
                return 0;                                             // RETURN
            }

            // We have a complete codepoint

            if (*position == lineDelimeter) {
                startOfLinePostion = currentPosition + 1;
                ++currentLineNum;
                currentColumnCodePoint = 0;
            }
            else {
                ++currentColumnCodePoint;
            }

            position += numBytesInCodePoint;
            currentPosition += numBytesInCodePoint;
        }
        BSLS_ASSERT(numRead >= attemptToRead);
    }
    BSLS_ASSERT_INVOKE_NORETURN("unreachable");
}

                              // ---------------
                              // struct Utf8Util
                              // ---------------

// CLASS METHODS

Utf8Util::IntPtr Utf8Util::advanceIfValid(int         *status,
                                          const char **result,
                                          const char  *string,
                                          IntPtr       numCodePoints)
{
    BSLS_ASSERT(status);
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(numCodePoints >= 0);

    IntPtr      ret = 0;      // return value -- number of code points advanced
    const char *next;         // 'next' is calculated during the loop as the
                              // starting position to start parsing the next
                              // code point, and assigned to 'string' between
                              // iterations.

    // Note that we keep 'string' pointing to the beginning of the Unicode code
    // point being processed, and only advance it to the next code point
    // between iterations.

    for (; true; ++ret, string = next) {
        next = string + 1;

        if (UNLIKELY(ret >= numCodePoints)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // This is the first of 2 ways we can exit this function
            // successfully.

            BSLS_ASSERT(ret == numCodePoints);           // impossible to fail

            *status = 0;
            break;
        }

        // Note that if we leave this 'switch' without doing a 'continue' we'll
        // exit the loop at the bottom.

        switch (static_cast<unsigned char>(*string) >> 4) {
          case 0: {
            // binary: 0000xxxx: low ASCII and possibly '\0'

            if (!*string) {
                // '\0', end of input with no errors.  This is the second of
                // two ways we can successfully exit this function.

                *status = 0;
                break;
            }
          } continue;

          case 0x1: BSLA_FALLTHROUGH;
          case 0x2: BSLA_FALLTHROUGH;
          case 0x3: BSLA_FALLTHROUGH;
          case 0x4: BSLA_FALLTHROUGH;
          case 0x5: BSLA_FALLTHROUGH;
          case 0x6: BSLA_FALLTHROUGH;
          case 0x7: {
            // binary: 0xxxxxxx: ASCII, but definitely not '\0'

          } continue;

          case 0x8: BSLA_FALLTHROUGH;
          case 0x9: BSLA_FALLTHROUGH;
          case 0xa: BSLA_FALLTHROUGH;
          case 0xb: {
            // binary: 10xxxxxx: error: unexpected continuation octet

            *status = k_UNEXPECTED_CONTINUATION_OCTET;

            // no 'continue'; exit the loop
          } break;

          case 0xc: BSLA_FALLTHROUGH;
          case 0xd: {
            // binary: 110xxxxx: 2-octet sequence

            if (UNLIKELY(isNotContinuation(*next))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *status = !*next ? k_END_OF_INPUT_TRUNCATION
                                 : k_NON_CONTINUATION_OCTET;
                break;
            }

            ++next;

            if (UNLIKELY(get2ByteValue(string) < k_MIN_2_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *status = k_OVERLONG_ENCODING;
                break;
            }

          } continue;

          case 0xe: {
            // binary: 1110xxxx: 3 octet sequence

            if   (UNLIKELY(isNotContinuation(*next))
               || UNLIKELY(isNotContinuation(*++next))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *status = isNotContinuation(string[1])
                        ? (string[1] ? k_NON_CONTINUATION_OCTET
                                     : k_END_OF_INPUT_TRUNCATION)
                        : (string[2] ? k_NON_CONTINUATION_OCTET
                                     : k_END_OF_INPUT_TRUNCATION);

                break;
            }

            ++next;

            int value = get3ByteValue(string);

            if (UNLIKELY(value < k_MIN_3_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *status = k_OVERLONG_ENCODING;
                break;
            }

            if (UNLIKELY(isSurrogateValue(value))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *status = k_SURROGATE;
                break;
            }
          } continue;

          case 0xf: {
            // binary: 1111xxxx: 4 octet sequence (only legal if 11110xxx).

            if (UNLIKELY(0x8 & *string)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // binary: 11111xxx: illegal start of 5 (or more) octet
                // sequence

                *status = k_INVALID_INITIAL_OCTET;

                break;
            }

            // binary: 11110xxx: legal start of 4 octet sequence

            if   (UNLIKELY(isNotContinuation(*next))
               || UNLIKELY(isNotContinuation(*++next))
               || UNLIKELY(isNotContinuation(*++next))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *status = isNotContinuation(string[1])
                        ? (string[1] ? k_NON_CONTINUATION_OCTET
                                     : k_END_OF_INPUT_TRUNCATION)
                        : isNotContinuation(string[2])
                        ? (string[2] ? k_NON_CONTINUATION_OCTET
                                     : k_END_OF_INPUT_TRUNCATION)
                        : (string[3] ? k_NON_CONTINUATION_OCTET
                                     : k_END_OF_INPUT_TRUNCATION);

                break;
            }

            ++next;


            int value = get4ByteValue(string);

            if (UNLIKELY(value < k_MIN_4_BYTE_VALUE)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *status = k_OVERLONG_ENCODING;
                break;
            }

            if (UNLIKELY(k_MAX_VALID < value)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                *status = k_VALUE_LARGER_THAN_0X10FFFF;
                break;
            }

          } continue;

          default: {
            BSLS_ASSERT_INVOKE_NORETURN("unreachable");
          }
        }

        break;
    }

    *result = string;
    return ret;
}

Utf8Util::IntPtr Utf8Util::advanceIfValid(int         *status,
                                          const char **result,
                                          const char  *string,
                                          size_type    length,
                                          IntPtr       numCodePoints)
{
    BSLS_ASSERT(status);
    BSLS_ASSERT(result);
    BSLS_ASSERT(string || 0 == length);
    BSLS_ASSERT(numCodePoints >= 0);

    IntPtr  ret = 0;      // return value -- number of code points advanced
    const char * const endOfInput = string + length;

    // Note that we keep 'string' pointing to the beginning of the Unicode code
    // point being processed, and only advance it to the next code point
    // between iterations.

    while(true) {
        if (UNLIKELY(string >= endOfInput)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Success.  We have successfully parsed all the input.

            BSLS_ASSERT(string == endOfInput);           // impossible to fail

            *status = 0;
            break;
        }

        if (UNLIKELY(ret >= numCodePoints)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Success.  We have successfully advanced 'numCodePoints' code
            // points.

            BSLS_ASSERT(ret == numCodePoints);           // impossible to fail

            *status = 0;
            break;
        }
        int cpStatus;
        if (!isValidCodePoint(&cpStatus, string, endOfInput - string)) {
          *status = cpStatus;
          break;
        }
        string += cpStatus;
        ++ret;
    }

    *result = string;
    return ret;
}

Utf8Util::IntPtr Utf8Util::advanceRaw(const char **result,
                                      const char  *string,
                                      IntPtr       numCodePoints)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string);
    BSLS_ASSERT(numCodePoints >= 0);

    IntPtr ret = 0;        // return value, # of code points advanced

    for (; ret < numCodePoints; ++ret) {
        // There's a 'break' at the end of this loop, so any case in the switch
        // that leaves without doing a 'continue' will exit the loop.

        switch (static_cast<unsigned char>(*string) >> 4) {
          case 0: {
            // binary: 0000xxxx: low ASCII, possibly '\0'

            if (!*string) {
                // '\0': end of input

                break;
            }

            ++string;
          } continue;

          case 1: BSLA_FALLTHROUGH;
          case 2: BSLA_FALLTHROUGH;
          case 3: BSLA_FALLTHROUGH;
          case 4: BSLA_FALLTHROUGH;
          case 5: BSLA_FALLTHROUGH;
          case 6: BSLA_FALLTHROUGH;
          case 7: {
            // binary: 0xxxxxxx: ASCII (definitely not '\0')

            ++string;
          } continue;

          case 8:   BSLA_FALLTHROUGH;
          case 9:   BSLA_FALLTHROUGH;
          case 0xa: BSLA_FALLTHROUGH;
          case 0xb: {
            // binary: 10xxxxxx: We hit an unexpected continuation octet.

            BSLS_ASSERT(0 && "invalid UTF-8");
          } break;

          // The following are multi-octet sequences.  Since we're assuming
          // valid input, don't bother inspecting the continuation octets.

          case 0xc: BSLA_FALLTHROUGH;
          case 0xd: {
            // binary: 110xxxxx: Start of two-octet sequence.

            string += 2;
          } continue;
          case 0xe: {
            // binary: 1110xxxx: Start of three-octet sequence.

            string += 3;
          } continue;
          case 0xf: {
            // binary: 1111xxxx: Start of four-octet sequence.

            BSLS_ASSERT(0 == (*string & 8));    // only legal if 11110xxx

            string += 4;
          } continue;

          default: {
            BSLS_ASSERT_INVOKE_NORETURN("unreachable");
          }
        }

        break;
    }

    BSLS_ASSERT(ret == numCodePoints || '\0' == *string); // impossible to fail

    *result = string;
    return ret;
}

Utf8Util::IntPtr Utf8Util::advanceRaw(const char **result,
                                      const char  *string,
                                      size_type    length,
                                      IntPtr       numCodePoints)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(string || 0 == length);
    BSLS_ASSERT(numCodePoints >= 0);

    IntPtr ret = 0;  // return value, # of code points advanced

    const char * const endOfInput = string + length;

    for (; true; ++ret) {
        // There's a 'break' at the end of this loop, so any case in the switch
        // that leaves without doing a 'continue' will exit the loop.

        if (UNLIKELY(string >= endOfInput)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Success

            BSLS_ASSERT(string == endOfInput);      // impossible to fail
                                                    // unless invalid UTF-8
            break;
        }

        if (UNLIKELY(ret >= numCodePoints)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Success

            BSLS_ASSERT(ret == numCodePoints);      // impossible to fail
            break;
        }

        switch (static_cast<unsigned char>(*string) >> 4) {
          case 0: BSLA_FALLTHROUGH;
          case 1: BSLA_FALLTHROUGH;
          case 2: BSLA_FALLTHROUGH;
          case 3: BSLA_FALLTHROUGH;
          case 4: BSLA_FALLTHROUGH;
          case 5: BSLA_FALLTHROUGH;
          case 6: BSLA_FALLTHROUGH;
          case 7: {
            // binary: 0xxxxxxx: ASCII, possibly '\0'

            ++string;
          } continue;

          case 8:   BSLA_FALLTHROUGH;
          case 9:   BSLA_FALLTHROUGH;
          case 0xa: BSLA_FALLTHROUGH;
          case 0xb: {
            // binary: 10xxxxxx: We hit an unexpected continuation octet.

            BSLS_ASSERT(0 && "invalid UTF-8");
          } break;

          // The following are multi-octet sequences.  Since we're assuming
          // valid input, don't bother inspecting the continuation octets.

          case 0xc: BSLA_FALLTHROUGH;
          case 0xd: {
            // binary: 110xxxxx: Beginning of two-octet sequence.

            string += 2;
          } continue;

          case 0xe: {
            // binary: 1110xxxx: Beginning of three-octet sequence.

            string += 3;
          } continue;

          case 0xf: {
            // binary: 1111xxxx: Beginning of four-octet sequence.

            BSLS_ASSERT(0 == (*string & 8));    // only legal if 11110xxx

            // binary: 11110xxx: Legal beginning of four-octet sequence

            string += 4;
          } continue;
        }

        break;
    }

    *result = string;
    return ret;
}

int Utf8Util::appendUtf8CodePoint(bsl::string *output, unsigned int codePoint)
{
    BSLS_ASSERT(output);

    return appendUtf8CodePointImpl(output, codePoint);
}

int Utf8Util::appendUtf8CodePoint(std::string *output, unsigned int codePoint)
{
    BSLS_ASSERT(output);

    return appendUtf8CodePointImpl(output, codePoint);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int Utf8Util::appendUtf8CodePoint(std::pmr::string *output,
                                  unsigned int      codePoint)
{
    BSLS_ASSERT(output);

    return appendUtf8CodePointImpl(output, codePoint);
}
#endif

int Utf8Util::numBytesInCodePoint(const char *codePoint)
{
    BSLS_ASSERT(codePoint);
    BSLS_ASSERT_SAFE(isValidUtf8CodePoint(codePoint));

    return utf8Size(codePoint[0]);
}

bool Utf8Util::isValid(const char **invalidString, const char *string)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string);

    return validateAndCountCodePoints(invalidString, string) >= 0;
}

bool Utf8Util::isValid(const char **invalidString,
                       const char  *string,
                       size_type    length)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string || 0 == length);
    BSLS_ASSERT(0 <= bsls::Types::IntPtr(length));

    return validateAndCountCodePoints(invalidString, string, length) >= 0;
}

bool Utf8Util::isValid(const char              **invalidString,
                       const bsl::string_view&   string)
{
    BSLS_ASSERT(invalidString);

    return validateAndCountCodePoints(invalidString,
                                      string.data(),
                                      string.length()) >= 0;
}

bool Utf8Util::isValidCodePoint(int        *status,
                                const char *codePoint,
                                size_type   numBytes)
{
    BSLS_ASSERT(status);
    BSLS_ASSERT(codePoint);
    BSLS_ASSERT(numBytes > 0);

    // Note that if we leave this 'switch' without doing a 'continue' (which we
    // only do if we encounter an error), we'll exit the loop at the bottom.

    const char *const  endOfInput = codePoint + numBytes;
    const char        *next       = codePoint + 1;

    switch (static_cast<unsigned char>(*codePoint) >> 4) {
      case 0x0: BSLA_FALLTHROUGH;
      case 0x1: BSLA_FALLTHROUGH;
      case 0x2: BSLA_FALLTHROUGH;
      case 0x3: BSLA_FALLTHROUGH;
      case 0x4: BSLA_FALLTHROUGH;
      case 0x5: BSLA_FALLTHROUGH;
      case 0x6: BSLA_FALLTHROUGH;
      case 0x7: {
        // binary: 0xxxxxxx: ASCII and possible '\0'
        *status = 1;
        return true;                                                  // RETURN
      }
      case 0x8: BSLA_FALLTHROUGH;
      case 0x9: BSLA_FALLTHROUGH;
      case 0xa: BSLA_FALLTHROUGH;
      case 0xb: {
        // binary: 10xxxxxx: unexpected continuation octet

        *status = k_UNEXPECTED_CONTINUATION_OCTET;
        return false;                                                 // RETURN
      }
      case 0xc: BSLA_FALLTHROUGH;
      case 0xd: {
        // binary: 110xxxxx: 2-octet sequence

        if (UNLIKELY(codePoint + 2 > endOfInput)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *status = k_END_OF_INPUT_TRUNCATION;
            return false;                                             // RETURN
        }

        if (UNLIKELY(isNotContinuation(*next))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *status = k_NON_CONTINUATION_OCTET;
            return false;                                             // RETURN
        }

        ++next;

        if (UNLIKELY(get2ByteValue(codePoint) < k_MIN_2_BYTE_VALUE)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *status = k_OVERLONG_ENCODING;
            return false;                                             // RETURN
        }
        *status = 2;
        return true;                                                  // RETURN
      }

      case 0xe: {
        // binary: 1110xxxx: 3 octet sequence

        if (UNLIKELY(codePoint + 3 > endOfInput)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *status = (next + 1 == endOfInput && isNotContinuation(*next))
                          ? k_NON_CONTINUATION_OCTET
                          : k_END_OF_INPUT_TRUNCATION;

            return false;                                             // RETURN
        }

        if (UNLIKELY(isNotContinuation(*next)) ||
            UNLIKELY(isNotContinuation(*++next))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *status = k_NON_CONTINUATION_OCTET;

            return false;                                             // RETURN
        }

        ++next;

        int value = get3ByteValue(codePoint);

        if (UNLIKELY(value < k_MIN_3_BYTE_VALUE)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *status = k_OVERLONG_ENCODING;
            return false;                                             // RETURN
        }

        if (UNLIKELY(isSurrogateValue(value))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *status = k_SURROGATE;
            return false;                                             // RETURN
        }
        *status = 3;
        return true;                                                  // RETURN
      }
      case 0xf: {
        if (UNLIKELY(0x8 & *codePoint)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // binary: 11111xxx: invalid code point in all UTF-8 contexts.

            *status = k_INVALID_INITIAL_OCTET;
            return false;                                             // RETURN
        }

        // binary: 11110xxx: legal start to 4 octet sequence

        if (UNLIKELY(codePoint + 4 > endOfInput)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *status = k_END_OF_INPUT_TRUNCATION;
            for (; next < endOfInput; ++next) {
                if (isNotContinuation(*next)) {
                    *status = k_NON_CONTINUATION_OCTET;

                    break;                                             // BREAK
                }
            }

            return false;                                             // RETURN
        }

        if (UNLIKELY(isNotContinuation(*next)) ||
            UNLIKELY(isNotContinuation(*++next)) ||
            UNLIKELY(isNotContinuation(*++next))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *status = k_NON_CONTINUATION_OCTET;
            return false;                                             // RETURN
        }

        ++next;

        int value = get4ByteValue(codePoint);

        if (UNLIKELY((k_MAX_VALID < value) || (value < k_MIN_4_BYTE_VALUE))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            *status = k_MAX_VALID < value ? k_VALUE_LARGER_THAN_0X10FFFF
                                          : k_OVERLONG_ENCODING;

            return false;                                             // RETURN
        }
        *status = 4;
        return true;                                                  // RETURN
      }
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("unreachable");
      }
    }
    BSLS_ASSERT_INVOKE_NORETURN("unreachable");
    return false;
}

Utf8Util::IntPtr Utf8Util::numBytesRaw(const bsl::string_view& string,
                                       IntPtr                  numCodePoints)
{
    BSLS_ASSERT(string.data() || string.empty());
    BSLS_ASSERT(0 <= numCodePoints);

    size_t numBytes = 0;

    // Note that since we are assuming the string already passed one of the
    // validation functions our work is very simple.

    for (int i = 0; i < numCodePoints && numBytes < string.length(); ++i) {
        BSLS_ASSERT_SAFE(isValidUtf8CodePoint(&string[numBytes]));
        numBytes += utf8Size(string[numBytes]);
    }

    if (numBytes > string.length()) {
        return k_END_OF_INPUT_TRUNCATION;                             // RETURN
    }

    return numBytes;
}

Utf8Util::IntPtr Utf8Util::numCodePointsIfValid(const char **invalidString,
                                                const char  *string)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string);

    return validateAndCountCodePoints(invalidString, string);
}

Utf8Util::IntPtr Utf8Util::numCodePointsIfValid(const char **invalidString,
                                                const char  *string,
                                                size_type    length)
{
    BSLS_ASSERT(invalidString);
    BSLS_ASSERT(string || 0 == length);
    BSLS_ASSERT(0 <= bsls::Types::IntPtr(length));

    return validateAndCountCodePoints(invalidString, string, length);
}

Utf8Util::IntPtr Utf8Util::numCodePointsIfValid(
                                       const char              **invalidString,
                                       const bsl::string_view&   string)
{
    BSLS_ASSERT(invalidString);

    return validateAndCountCodePoints(invalidString,
                                      string.data(),
                                      string.length());
}

Utf8Util::IntPtr Utf8Util::numCodePointsRaw(const char *string)
{
    BSLS_ASSERT(string);

    IntPtr count = 0;

    // Note that since we assume the string contains valid UTF-8, our work is
    // very simple.

    while (true) {
        switch (static_cast<unsigned char>(*string) >> 4) {
          case 0: {
            if (!*string) {
                return count;                                         // RETURN
            }

            ++string;
          } break;
          case 1: BSLA_FALLTHROUGH;
          case 2: BSLA_FALLTHROUGH;
          case 3: BSLA_FALLTHROUGH;
          case 4: BSLA_FALLTHROUGH;
          case 5: BSLA_FALLTHROUGH;
          case 6: BSLA_FALLTHROUGH;
          case 7: {
            ++string;
          } break;
          case 0xc: BSLA_FALLTHROUGH;
          case 0xd: {
            BSLS_ASSERT(string[1]);
            string += 2;
          } break;
          case 0xe: {
            BSLS_ASSERT(string[1]);
            BSLS_ASSERT(string[2]);
            string += 3;
          } break;
          default: {
            BSLS_ASSERT(string[1]);
            BSLS_ASSERT(string[2]);
            BSLS_ASSERT(string[3]);
            string += 4;
          } break;
        }

        ++count;
    }
}

Utf8Util::IntPtr Utf8Util::numCodePointsRaw(const char *string,
                                            size_type   length)
{
    BSLS_ASSERT(string || 0 == length);

    IntPtr count = 0;

    // Note that since we assume the string contains valid UTF-8, our work is
    // very simple.

    const char *const end = string + length;

    while (string < end) {
        switch (static_cast<unsigned char>(*string) >> 4) {
          case 0: BSLA_FALLTHROUGH;
          case 1: BSLA_FALLTHROUGH;
          case 2: BSLA_FALLTHROUGH;
          case 3: BSLA_FALLTHROUGH;
          case 4: BSLA_FALLTHROUGH;
          case 5: BSLA_FALLTHROUGH;
          case 6: BSLA_FALLTHROUGH;
          case 7: {
            ++string;
          } break;
          case 0xc: BSLA_FALLTHROUGH;
          case 0xd: {
            BSLS_ASSERT(2 <= end - string);
            string += 2;
          } break;
          case 0xe: {
            BSLS_ASSERT(3 <= end - string);
            string += 3;
          } break;
          default: {
            BSLS_ASSERT(4 <= end - string);
            string += 4;
          } break;
        }

        ++count;
    }

    return count;
}

Utf8Util::size_type Utf8Util::readIfValid(int            *status,
                                          char           *outputBuffer,
                                          size_type       outputBufferLength,
                                          bsl::streambuf *input)
{
    BSLS_ASSERT(status);
    BSLS_ASSERT(outputBuffer);
    BSLS_ASSERT(4 <= outputBufferLength);
    BSLS_ASSERT(input);

    typedef bsl::char_traits<char>   Traits;
    typedef Traits::int_type         int_type;
    typedef bsl::streambuf::pos_type pos_type;

    // We first write the UTF-8 sequence into 'tmpBuf', of length 'tmpLen'.
    // Only if it turns out to be valid UTF-8 do we copy it into
    // 'outputBuffer'.

    char             tmpBuf[4] = { 0 };    // zero out to silence purify
    char            *tmpPtr;

    char            *out = outputBuffer;
    const char      *end = outputBuffer + outputBufferLength - 3;
    const int_type   eof = Traits::eof();
    int              rc  = 0;
    int              c;

    while (true) {
        BSLS_ASSERT(0 == rc);

        if (UNLIKELY(end <= out)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            rc = +1;
            break;
        }

        if (UNLIKELY(eof == (c = input->sbumpc()))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            break;
        }

        tmpPtr = tmpBuf;
        *tmpPtr++ = static_cast<char>(c);

        switch (static_cast<unsigned char>(c) >> 4) {
          case 0x0: BSLA_FALLTHROUGH;
          case 0x1: BSLA_FALLTHROUGH;
          case 0x2: BSLA_FALLTHROUGH;
          case 0x3: BSLA_FALLTHROUGH;
          case 0x4: BSLA_FALLTHROUGH;
          case 0x5: BSLA_FALLTHROUGH;
          case 0x6: BSLA_FALLTHROUGH;
          case 0x7: {
            *out++ = *tmpBuf;
          } continue;
          case 0x8: BSLA_FALLTHROUGH;
          case 0x9: BSLA_FALLTHROUGH;
          case 0xa: BSLA_FALLTHROUGH;
          case 0xb: {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            rc = k_UNEXPECTED_CONTINUATION_OCTET;
          } break;
          case 0xc: BSLA_FALLTHROUGH;
          case 0xd: {
            if (UNLIKELY(eof == (c = input->sbumpc()))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                rc = k_END_OF_INPUT_TRUNCATION;
                break;
            }

            *tmpPtr++ = static_cast<char>(c);

            const int value = get2ByteValue(tmpBuf);
            if (UNLIKELY(isNotContinuation(tmpBuf[1])
                       || (value < k_MIN_2_BYTE_VALUE))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                rc = isNotContinuation(tmpBuf[1]) ? k_NON_CONTINUATION_OCTET
                                                  : k_OVERLONG_ENCODING;
                break;
            }

            bsl::memcpy(out, tmpBuf, 2);
            out += 2;
          } continue;
          case 0xe: {
            for (int ii = 1; ii <= 2; ++ii) {
                if (UNLIKELY(eof == (c = input->sbumpc()))) {
                    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                    rc = k_END_OF_INPUT_TRUNCATION;
                    break;
                }

                *tmpPtr++ = static_cast<char>(c);

                if (UNLIKELY(isNotContinuation(tmpBuf[ii]))) {
                    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                    rc = k_NON_CONTINUATION_OCTET;
                    break;
                }
            }

            const int value = get3ByteValue(tmpBuf);
            if (UNLIKELY((0 != rc)
                      || (value < k_MIN_3_BYTE_VALUE)
                      || isSurrogateValue(value))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                rc = rc ? rc : value < k_MIN_3_BYTE_VALUE
                             ? k_OVERLONG_ENCODING
                             : k_SURROGATE;
                break;
            }

            bsl::memcpy(out, tmpBuf, 3);
            out += 3;
          } continue;
          case 0xf: {
            if (UNLIKELY(0x8 & *tmpBuf)) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                // binary: 11111xxx: invalid code point in all UTF-8 contexts.

                rc = k_INVALID_INITIAL_OCTET;

                break;
            }

            for (int ii = 1; ii <= 3; ++ii) {
                if (UNLIKELY(eof == (c = input->sbumpc()))) {
                    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                    rc = k_END_OF_INPUT_TRUNCATION;
                    break;
                }

                *tmpPtr++ = static_cast<char>(c);

                if (UNLIKELY(isNotContinuation(tmpBuf[ii]))) {
                    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                    rc = k_NON_CONTINUATION_OCTET;
                    break;
                }
            }

            const int value = get4ByteValue(tmpBuf);
            if (UNLIKELY((0 != rc)
                      || (value < k_MIN_4_BYTE_VALUE)
                      || (value > k_MAX_VALID))) {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                rc = rc ? rc
                        : k_MAX_VALID < value ? k_VALUE_LARGER_THAN_0X10FFFF
                                              : k_OVERLONG_ENCODING;
                break;
            }

            bsl::memcpy(out, tmpBuf, 4);
            out += 4;
          } continue;
          default: {
            BSLS_ASSERT_INVOKE_NORETURN("unreachable");
          }
        }
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Windows and AIX have a problem with 'std::filebuf::sputbackc'
        // malfunctions under some circumstances when called multiple times, so
        // we use 'pubseekoff' to move backward.  However, some 'streambuf's
        // (including the 'streambuf' in a 'std::stringstream' on Unix) are not
        // seekable, so we do not check the return value here.

        const pos_type pos = input->pubseekoff(tmpBuf - tmpPtr,
                                               bsl::ios_base::cur,
                                               bsl::ios_base::in);

        // If the seek succeeds, it will return a non-negative value.  This may
        // be the case if '*input' is non-seekable (which is rare).  Check it
        // with a safe assert, so we will find out about it in development, but
        // just continue on if the problem occurs in production.

        BSLS_ASSERT_SAFE(0 <= pos);    (void) pos;

        break;
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

    BSLS_ASSERT(outputBuffer <= out);
    BSLS_ASSERT(out <= outputBuffer + outputBufferLength);

    *status = rc;

    return out - outputBuffer;
}

const char *Utf8Util::toAscii(IntPtr value)
{
#undef  U_ASCII_CASE
#define U_ASCII_CASE(es)    case k_ ## es: return #es

    switch (value) {
      U_ASCII_CASE(END_OF_INPUT_TRUNCATION);
      U_ASCII_CASE(UNEXPECTED_CONTINUATION_OCTET);
      U_ASCII_CASE(NON_CONTINUATION_OCTET);
      U_ASCII_CASE(OVERLONG_ENCODING);
      U_ASCII_CASE(INVALID_INITIAL_OCTET);
      U_ASCII_CASE(VALUE_LARGER_THAN_0X10FFFF);
      U_ASCII_CASE(SURROGATE);
      default: return "(* unrecognized value *)";
    }

#undef  U_ASCII_CASE
}

}  // close package namespace
}  // close enterprise namespace

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
