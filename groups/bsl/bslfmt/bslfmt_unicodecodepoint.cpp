// bslfmt_unicodecodepoint.cpp                                        -*-C++-*-

#include <bslfmt_unicodecodepoint.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslfmt_unicodecodepoint_cpp, "$Id$ $CSID$")

#include <bslfmt_formatterunicodedata.h>

#include <bsla_fallthrough.h>

#include <bsls_performancehint.h>

#include <bslstl_algorithm.h>

namespace {
typedef BloombergLP::bslfmt::FormatterUnicodeData UnicodeData;

// LOCAL MACROS
#define UNLIKELY(EXPRESSION) BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(EXPRESSION)

    // LOCAL CONSTANTS

enum {
    k_MIN_2_BYTE_UTF8_VALUE = 0x80,
        // min value needing 2 bytes to encode

    k_MIN_3_BYTE_UTF8_VALUE = 0x800,
        // min value needing 3 bytes to encode

    k_MIN_4_BYTE_UTF8_VALUE = 0x10000,
        // min value needing 4 bytes to encode

//    k_MIN_SURROGATE = 0xd800,
//        // min surrogate value

    k_MAX_UTF_VALID = 0x10ffff,
        // max value encodable in UTF-8

    k_UTF8_CONT_VALUE_MASK = 0x3f,
        // part of a continuation byte that contains the 6 bits of value info

    k_UTF16_HIGH_SURROGATE_START = 0xd800,
        // start of high surrogate range

//    k_UTF16_HIGH_SURROGATE_END   = 0xdbff,
//        // start of high surrogate range

    k_UTF16_LOW_SURROGATE_START = 0xdc00,
        // start of high surrogate range

//    k_UTF16_LOW_SURROGATE_END   = 0xdcff,
//        // start of high surrogate range

    k_UTF16_SURROGATE_PAIR_OFFSET = 0x10000,
        // start of surrogate pair represented values

    k_UTF16_SURROGATE_MASK_TESTBOTH = 0xfffff800U,
        // mask for testing if something is a surrogate or not

    k_UTF16_SURROGATE_MASK_TESTONE = 0xfffffC00U,
        // mask for testing if something is a high or low surrogate

//    k_MIN_UTF16_SURROGATE = 0xd800,
//        // min surrogate value
};

// ============================================================================
//                              HELPER CLASSES
// ============================================================================

/// Component-private comparator class to facilitate searches by ranges of code
/// points using standard algorithms.
template <class t_CODE_POINT_RANGE_TYPE>
struct EndCompare
{
    /// Return true if the `d_end` member of the specified `range` of code
    /// points is less than the specified `value`, false otherwise.
    bool operator()(const t_CODE_POINT_RANGE_TYPE& range,
                    const unsigned long int        value)
    {
        return range.d_end < value;
    }
};

// ============================================================================
//                              HELPER FUNCTIONS
// ============================================================================

/// Determine whether the start of the specified array `bytes`, up to a maximum
/// length of the specified `maxBytes` contains a UTF Byte Order Marker, and
/// return the result.
inline
static bool isByteOrderMarker(const void *bytes, size_t maxBytes)
{
    if (maxBytes < 2) {
        return false;                                                 // RETURN
    }

    const unsigned char *data = static_cast<const unsigned char *>(bytes);

    const unsigned char data0 = data[0];

    // Early exit for the common (ascii) case
    if (data0 != 0x00 && data0 < 0xef) {
        return false;                                                 // RETURN
    }

    const unsigned char data1 = data[1];

    // UTF-16 BE
    if (0xfe == data0 && 0xff == data1) {
        return true;                                                  // RETURN
    }

    // UTF-16 LE or UTF-32 LE
    if (0xff == data0 && 0xfe == data1) {
        return true;                                                  // RETURN
    }

    if (maxBytes < 3) {
        return false;                                                 // RETURN
    }

    const unsigned char data2 = data[2];

    // UTF-8
    if (0xef == data0 && 0xbb == data1 && 0xbf == data2) {
        return true;                                                  // RETURN
    }

    if (maxBytes < 4) {
        return false;                                                 // RETURN
    }

    const unsigned char data3 = data[3];

    // UTF-32 BE (UTF-32 LE is FF FE 00 00 which is detected above)
    if (0x00 == data0 && 0x00 == data1 && 0xfe == data2 && 0xff == data3) {
        return true;                                                  // RETURN
    }

    return false;
}

/// Return the integral value of the single code point represented by the
/// 2-byte UTF-8 sequence referred to by the specified `pc`.  The behavior is
/// undefined unless the 2 bytes starting at `pc` contain a UTF-8 sequence
/// describing a single valid code point.
inline
static int get2ByteUtf8Value(const unsigned char *pc)
{
    return ((*pc & 0x1f) << 6) | ((pc[1] & k_UTF8_CONT_VALUE_MASK));
}

/// Return the integral value of the single code point represented by the
/// 3-byte UTF-8 sequence referred to by the specified `pc`.  The behavior is
/// undefined unless the 3 bytes starting at `pc` contain a UTF-8 sequence
/// describing a single valid code point.
inline
static int get3ByteUtf8Value(const unsigned char *pc)
{
    return ((*pc & 0xf) << 12) | ((pc[1] & k_UTF8_CONT_VALUE_MASK) << 6) |
           ((pc[2] & k_UTF8_CONT_VALUE_MASK));
}

/// Return the integral value of the single code point represented by the
/// 4-byte UTF-8 sequence referred to by the specified `pc`.  The behavior is
/// undefined unless the 4 bytes starting at `pc` contain a UTF-8 sequence
/// describing a single valid code point.
inline
static int get4ByteUtf8Value(const unsigned char *pc)
{
    return ((*pc & 0x7) << 18) | ((pc[1] & k_UTF8_CONT_VALUE_MASK) << 12) |
           ((pc[2] & k_UTF8_CONT_VALUE_MASK) << 6) |
           ((pc[3] & k_UTF8_CONT_VALUE_MASK));
}

/// Determine the width of the specified `codepoint` per the rules in the C++
/// standard in [format.string.std].  Note that this width may differ from that
/// specified by the Unicode standard.
inline
static int getCodepointWidth(unsigned long int codepoint)
{
    const UnicodeData::BooleanRange *first =
                                         UnicodeData::s_doubleFieldWidthRanges;

    // Early exit for the common (ascii) case:
    if (codepoint < first->d_start) {
        return 1;                                                     // RETURN
    }

    const UnicodeData::BooleanRange *last =
                                     UnicodeData::s_doubleFieldWidthRanges +
                                     UnicodeData::s_doubleFieldWidthRangeCount;

    EndCompare<UnicodeData::BooleanRange> comparator;

    const UnicodeData::BooleanRange *found =
                          bsl::lower_bound(first, last, codepoint, comparator);

    // Below the first element in the array.
    if ((found == last) ||
        (found->d_start > codepoint) ||
        (found->d_end   < codepoint)) {
        return 1;                                                     // RETURN
    }

    return 2;
}

/// Return `true` if the specified `value` is NOT a UTF-8 continuation byte,
/// and `false` otherwise.
inline
static bool isNotUtf8Continuation(unsigned char value)
{
    return 0x80 != (value & 0xc0);
}

/// Return `true` if the specified `value` is a surrogate value, and `false`
/// otherwise.
inline
static bool isSurrogateValue(unsigned int value)
{
    // Mask is      1111 1111 1111 1111 1111 1000 0000 0000
    // Test against                     1101 1000 0000 0000
    return (k_UTF16_SURROGATE_MASK_TESTBOTH & value) ==
           k_UTF16_HIGH_SURROGATE_START;
}

/// Return `true` if the specified `value` is a high surrogate value, and
/// `false` otherwise.
inline
static bool isHighSurrogateValue(unsigned int value)
{
    // Mask is      1111 1111 1111 1111 1111 1100 0000 0000
    // Test against                     1101 1000 0000 0000
    return (k_UTF16_SURROGATE_MASK_TESTONE & value) ==
           k_UTF16_HIGH_SURROGATE_START;
}

/// Return `true` if the specified `value` is a high surrogate value, and
/// `false` otherwise.
inline
static bool isLowSurrogateValue(unsigned int value)
{
    // Mask is      1111 1111 1111 1111 1111 1100 0000 0000
    // Test against                     1101 1100 0000 0000
    return (k_UTF16_SURROGATE_MASK_TESTONE & value) ==
           k_UTF16_LOW_SURROGATE_START;
}

}  // close unnamed namespace

namespace BloombergLP {
namespace bslfmt {

void UnicodeCodePoint::extractUtf8(const void *bytes, size_t maxBytes)
{
   if (0 == bytes || maxBytes < 1) {
        return;                                                       // RETURN
    }

    if (isByteOrderMarker(bytes, maxBytes)) {
        return;                                                       // RETURN
    }

    const unsigned char *start = static_cast<const unsigned char *>(bytes);

    switch (static_cast<unsigned char>(*start) >> 4) {
      case 0x0: BSLA_FALLTHROUGH;
      case 0x1: BSLA_FALLTHROUGH;
      case 0x2: BSLA_FALLTHROUGH;
      case 0x3: BSLA_FALLTHROUGH;
      case 0x4: BSLA_FALLTHROUGH;
      case 0x5: BSLA_FALLTHROUGH;
      case 0x6: BSLA_FALLTHROUGH;
      case 0x7: {
        d_codePointValue = *start;
        d_numSourceBytes = 1;
        d_isValid        = true;
        d_codePointWidth = 1;  // simple ascii value.
      } break;
      case 0x8: BSLA_FALLTHROUGH;
      case 0x9: BSLA_FALLTHROUGH;
      case 0xa: BSLA_FALLTHROUGH;
      case 0xb: {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Unexpected continuation octet
        return;                                                       // RETURN
      } break;
      case 0xc: BSLA_FALLTHROUGH;
      case 0xd: {
        if (UNLIKELY(maxBytes < 2)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Too few bytes available given the length indicated by the first
            // byte
            return;                                                   // RETURN
        }

        const unsigned int value = get2ByteUtf8Value(start);

        if (UNLIKELY(value < k_MIN_2_BYTE_UTF8_VALUE)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Overlong encoding
            return;                                                   // RETURN
        }
        if (UNLIKELY(isNotUtf8Continuation(start[1]))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Non-continuation octet
            return;                                                   // RETURN
        }

        d_codePointValue = value;
        d_numSourceBytes = 2;
        d_isValid        = true;
        d_codePointWidth = getCodepointWidth(value);
      } break;
      case 0xe: {
        if (UNLIKELY(maxBytes < 3)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Too few bytes available given the length indicated by the first
            // byte
            return;                                                   // RETURN
        }

        const unsigned int value = get3ByteUtf8Value(start);

        if (UNLIKELY(value < k_MIN_3_BYTE_UTF8_VALUE)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Overlong encoding
            return;                                                   // RETURN
        }
        if (UNLIKELY(isNotUtf8Continuation(start[1]) ||
                     isNotUtf8Continuation(start[2]))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Non-continuation octet
            return;                                                   // RETURN
        }
        if (UNLIKELY(isSurrogateValue(value))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Erroneous surrogate value
            return;                                                   // RETURN
        }

        d_codePointValue = value;
        d_numSourceBytes = 3;
        d_isValid        = true;
        d_codePointWidth = getCodepointWidth(value);
      } break;
      case 0xf: {
        if (UNLIKELY(maxBytes < 4)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Too few bytes available given the length indicated by the first
            // byte.
            return;                                                   // RETURN
        }

        const unsigned int value = get4ByteUtf8Value(start);

        if (UNLIKELY(bool(0x8 & *start))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Invalid initial octet for 4-byte value
            return;                                                   // RETURN
        }
        if (UNLIKELY(value < k_MIN_4_BYTE_UTF8_VALUE)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Overlong encoding
            return;                                                   // RETURN
        }
        if (UNLIKELY(value > k_MAX_UTF_VALID)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Value exceeds 0X10FFFF
            return;                                                   // RETURN
        }
        if (UNLIKELY(isNotUtf8Continuation(start[1]) ||
                     isNotUtf8Continuation(start[2]) ||
                     isNotUtf8Continuation(start[3]))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Non-continuation octet
            return;                                                   // RETURN
        }

        d_codePointValue = value;
        d_numSourceBytes = 4;
        d_isValid        = true;
        d_codePointWidth = getCodepointWidth(value);
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("unreachable");
      }
    }

}

void UnicodeCodePoint::extractUtf16(const void *bytes, size_t maxBytes)
{
    // If ever this function is called on a platform with a 32-bit wchar_t
    // (such as on Linux) then something went wrong.
    BSLS_ASSERT_OPT(sizeof(wchar_t) == 2);

    if (0 == bytes || maxBytes < 2) {
        return;                                                       // RETURN
    }

    if (isByteOrderMarker(bytes, maxBytes)) {
        return;                                                       // RETURN
    }

    const wchar_t *start = static_cast<const wchar_t *>(bytes);

    const unsigned int first = static_cast<unsigned int>(*start);

    if (UNLIKELY(isLowSurrogateValue(first))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            // It is illegal to have a low surrogate without a preceding high
            // surrogate.

        return;                                                       // RETURN
    }

    if (!isHighSurrogateValue(first)) {
        // It is illegal to have a low surrogate without a preceeding high
        // surrogate.

        d_codePointValue = first;
        d_numSourceBytes = 2;
        d_isValid        = true;
        d_codePointWidth = getCodepointWidth(first);

        return;                                                       // RETURN
    }

    if (UNLIKELY(maxBytes < 4)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Too few bytes available given the first is a surrogate
        return;                                                       // RETURN
    }

    const unsigned int second = static_cast<unsigned int>(*(start + 1));

    if (UNLIKELY(!isLowSurrogateValue(second))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            // High surrogate must be followed by a low surrogate

        return;                                                       // RETURN
    }

    const unsigned int value = (((first - k_UTF16_HIGH_SURROGATE_START)
                                 << 10) |
                                (second - k_UTF16_LOW_SURROGATE_START)) +
                               k_UTF16_SURROGATE_PAIR_OFFSET;

    if (UNLIKELY(value > k_MAX_UTF_VALID)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Value exceeds 0X10FFFF
        return;                                                       // RETURN
    }

    d_codePointValue = value;
    d_numSourceBytes = 4;
    d_isValid        = true;
    d_codePointWidth = getCodepointWidth(value);

}

void UnicodeCodePoint::extractUtf32(const void *bytes, size_t maxBytes)
{
    // If ever this function is called on a platform with a 32-bit wchar_t
    // (such as on Linux) then something went wrong.
    BSLS_ASSERT_OPT(sizeof(wchar_t) == 4);

    if (0 == bytes || maxBytes < 4) {
        return;                                                       // RETURN
    }

    if (isByteOrderMarker(bytes, maxBytes)) {
        return;                                                       // RETURN
    }

    const wchar_t *start = static_cast<const wchar_t *>(bytes);

    const unsigned int value = static_cast<unsigned int>(*start);

    if (UNLIKELY(isSurrogateValue(value))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            // It is illegal to have a low surrogate values in any UTF schema.

        return;                                                       // RETURN
    }

    if (UNLIKELY(value > k_MAX_UTF_VALID)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Value exceeds 0X10FFFF
        return;                                                       // RETURN
    }

    d_codePointValue = value;
    d_numSourceBytes = 4;
    d_isValid        = true;
    d_codePointWidth = getCodepointWidth(value);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
