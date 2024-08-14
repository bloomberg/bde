// bslstl_formatterunicodeutils.cpp                                   -*-C++-*-

#include <bslfmt_formatterunicodeutils.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslstl_format_cpp, "$Id$ $CSID$")

#include <bsla_fallthrough.h>
#include <bsla_unreachable.h>
#include <bsla_unused.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <bslstl_algorithm.h>
#include <bslstl_map.h>

#include <bslfmt_formatterunicodedata.h>

// LOCAL MACROS

#define UNLIKELY(EXPRESSION) BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(EXPRESSION)
#define LIKELY(EXPRESSION) BSLS_PERFORMANCEHINT_PREDICT_LIKELY(EXPRESSION)

// LOCAL CONSTANTS

namespace {

using namespace BloombergLP;

enum {
    k_MIN_2_BYTE_UTF8_VALUE = 0x80,     // min value needing 2 bytes to encode
    k_MIN_3_BYTE_UTF8_VALUE = 0x800,    // min value needing 3 bytes to encode
    k_MIN_4_BYTE_UTF8_VALUE = 0x10000,  // min value needing 4 bytes to encode

    k_MIN_SURROGATE = 0xd800,  // min surrogate value

    k_MAX_UTF_VALID = 0x10ffff,  // max value encodable in UTF-8

    k_UTF8_CONT_VALUE_MASK = 0x3f,  // part of a continuation byte that
                                    // contains the 6 bits of value info

    k_UTF16_HIGH_SURROGATE_START = 0xd800,  // start of high surrogate range
    k_UTF16_HIGH_SURROGATE_END   = 0xdbff,  // start of high surrogate range

    k_UTF16_LOW_SURROGATE_START = 0xdc00,  // start of high surrogate range
    k_UTF16_LOW_SURROGATE_END   = 0xdcff,  // start of high surrogate range

    k_UTF16_SURROGATE_PAIR_OFFSET = 0x10000,  // Start of surrogate pair
                                              // represented values

    k_UTF16_SURROGATE_MASK_TESTBOTH = 0xfffff800U,  // Mask for testing if
                                                    // somethign is a surrogate
                                                    // or not.

    k_UTF16_SURROGATE_MASK_TESTONE = 0xfffffC00U,  // Mask for testing if
    // something is a high or low
    // surrogate.

    k_MIN_UTF16_SURROGATE = 0xd800,  // min surrogate value
};

static inline
bool isNotUtf8Continuation(unsigned char value)
    // Return 'true' if the specified 'value' is NOT a UTF-8 continuation byte,
    // and 'false' otherwise.
{
    return 0x80 != (value & 0xc0);
}

static inline
bool isSurrogateValue(unsigned int value)
    // Return 'true' if the specified 'value' is a surrogate value, and 'false'
    // otherwise.
{
    // Mask is      1111 1111 1111 1111 1111 1000 0000 0000
    // Test against                     1101 1000 0000 0000
    return (k_UTF16_SURROGATE_MASK_TESTBOTH & value) ==
           k_UTF16_HIGH_SURROGATE_START;
}

static inline
bool isHighSurrogateValue(unsigned int value)
    // Return 'true' if the specified 'value' is a high surrogate value, and
    // 'false' otherwise.
{
    // Mask is      1111 1111 1111 1111 1111 1100 0000 0000
    // Test against                     1101 1000 0000 0000
    return (k_UTF16_SURROGATE_MASK_TESTONE & value) ==
           k_UTF16_HIGH_SURROGATE_START;
}

static inline
bool isLowSurrogateValue(unsigned int value)
    // Return 'true' if the specified 'value' is a high surrogate value, and
    // 'false' otherwise.
{
    // Mask is      1111 1111 1111 1111 1111 1100 0000 0000
    // Test against                     1101 1100 0000 0000
    return (k_UTF16_SURROGATE_MASK_TESTONE & value) ==
           k_UTF16_LOW_SURROGATE_START;
}

static inline
int get2ByteUtf8Value(const unsigned char *pc)
    // Return the integral value of the single code point represented by the
    // 2-byte UTF-8 sequence referred to by the specified 'pc'.  The behavior
    // is undefined unless the 2 bytes starting at 'pc' contain a UTF-8
    // sequence describing a single valid code point.
{
    return ((*pc & 0x1f) << 6) | ((pc[1] & k_UTF8_CONT_VALUE_MASK));
}

static inline
int get3ByteUtf8Value(const unsigned char *pc)
    // Return the integral value of the single code point represented by the
    // 3-byte UTF-8 sequence referred to by the specified 'pc'.  The behavior
    // is undefined unless the 3 bytes starting at 'pc' contain a UTF-8
    // sequence describing a single valid code point.
{
    return ((*pc & 0xf) << 12) | ((pc[1] & k_UTF8_CONT_VALUE_MASK) << 6) |
           ((pc[2] & k_UTF8_CONT_VALUE_MASK));
}

static inline
int get4ByteUtf8Value(const unsigned char *pc)
    // Return the integral value of the single code point represented by the
    // 4-byte UTF-8 sequence referred to by the specified 'pc'.  The behavior
    // is undefined unless the 4 bytes starting at 'pc' contain a UTF-8
    // sequence describing a single valid code point.
{
    return ((*pc & 0x7) << 18) | ((pc[1] & k_UTF8_CONT_VALUE_MASK) << 12) |
           ((pc[2] & k_UTF8_CONT_VALUE_MASK) << 6) |
           ((pc[3] & k_UTF8_CONT_VALUE_MASK));
}

static inline
bool isByteOrderMarker(const void *bytes, size_t maxBytes)
    // Determine whether the start of the specified array `bytes`, up to a
    // maximum length of the specified `maxBytes` contains a UTF Byte Order
    // Marker, and return the result.
{
    const unsigned char *data = static_cast<const unsigned char *>(bytes);

    if (maxBytes < 2)
        return false;

    // UTF-16 BE
    if (0xfe == data[0] && 0xff == data[1])
        return true;                                                  // RETURN

    // UTF-16 LE or UTF-32 LE
    if (0xff == data[0] && 0xfe == data[1])
        return true;                                                  // RETURN

    if (maxBytes < 3)
        return false;

    // UTF-8
    if (0xef == data[0] && 0xbb == data[1] && 0xbf == data[2])
        return true;                                                  // RETURN

    if (maxBytes < 4)
        return false;

    // UTF-32 BE (UTF-32 LE is FF FE 00 00 which is detected above)
    if (0x00 == data[0] && 0x00 == data[1] && 0xfe == data[2] &&
        0xff == data[3])
        return true;                                                  // RETURN

    return false;
}


template <class e_RANGE_TYPE>
struct Formatter_UnicodeData_EndCompare
    // Component-private comparator class to facilitate range searches using
    // standard algorithms.
{
    bool operator()(const e_RANGE_TYPE& range, const unsigned long int value)
        // Return true if the `d_end` member of the specified `range` is less
        // than the specified `value`, false otherwise.
    {
        return range.d_end < value;
    }
};

bslfmt::Formatter_UnicodeData::GraphemeBreakCategory getGraphemeBreakCategory(
                                                   unsigned long int codepoint)
    // Find and return the Unicode Grapheme Break category for the specified
    // `codepoint` if one exists, otherwise return `e_UNASSIGNED`.
{
    const bslfmt::Formatter_UnicodeData::GraphemeBreakCategoryRange *first =
                  bslfmt::Formatter_UnicodeData::s_graphemeBreakCategoryRanges;
    const bslfmt::Formatter_UnicodeData::GraphemeBreakCategoryRange *last =
              bslfmt::Formatter_UnicodeData::s_graphemeBreakCategoryRanges +
              bslfmt::Formatter_UnicodeData::s_graphemeBreakCategoryRangeCount;

    Formatter_UnicodeData_EndCompare<
        bslfmt::Formatter_UnicodeData::GraphemeBreakCategoryRange>
        comparator;

    const bslfmt::Formatter_UnicodeData::GraphemeBreakCategoryRange *found =
                          bsl::lower_bound(first, last, codepoint, comparator);

    // Below the first element in the array.
    if (found == last)
        return bslfmt::Formatter_UnicodeData::e_UNASSIGNED;           // RETURN

    if (found->d_start > codepoint)
        return bslfmt::Formatter_UnicodeData::e_UNASSIGNED;           // RETURN

    if (found->d_end < codepoint)
        return bslfmt::Formatter_UnicodeData::e_UNASSIGNED;           // RETURN

    return found->d_category;
}

bool getExtendedPictogramValue(unsigned long int codepoint)
{
    const bslfmt::Formatter_UnicodeData::BooleanRange *first =
                   bslfmt::Formatter_UnicodeData::s_extendedPictographicRanges;
    const bslfmt::Formatter_UnicodeData::BooleanRange *last =
               bslfmt::Formatter_UnicodeData::s_extendedPictographicRanges +
               bslfmt::Formatter_UnicodeData::s_extendedPictographicRangeCount;

    Formatter_UnicodeData_EndCompare<
        bslfmt::Formatter_UnicodeData::BooleanRange>
        comparator;

    const bslfmt::Formatter_UnicodeData::BooleanRange *found =
                          bsl::lower_bound(first, last, codepoint, comparator);

    // Below the first element in the array.
    if (found == last)
        return false;                                                 // RETURN

    if (found->d_start > codepoint)
        return false;                                                 // RETURN

    if (found->d_end < codepoint)
        return false;                                                 // RETURN

    return true;
}

int getCodepointWidth(unsigned long int codepoint)
    // Determine the width of the specified `codepoint` per the rules in the
    // C++ standard in [format.string.std]. Note that this width may differ
    // from that specified by the Unicode standard.
{
    const bslfmt::Formatter_UnicodeData::BooleanRange *first =
                       bslfmt::Formatter_UnicodeData::s_doubleFieldWidthRanges;
    const bslfmt::Formatter_UnicodeData::BooleanRange *last =
                   bslfmt::Formatter_UnicodeData::s_doubleFieldWidthRanges +
                   bslfmt::Formatter_UnicodeData::s_doubleFieldWidthRangeCount;

    Formatter_UnicodeData_EndCompare<
        bslfmt::Formatter_UnicodeData::BooleanRange>
        comparator;

    const bslfmt::Formatter_UnicodeData::BooleanRange *found =
                          bsl::lower_bound(first, last, codepoint, comparator);

    // Below the first element in the array.
    if (found == last)
        return 1;                                                     // RETURN

    if (found->d_start > codepoint)
        return 1;                                                     // RETURN

    if (found->d_end < codepoint)
        return 1;                                                     // RETURN

    return 2;
}


/// Extract a UTF-8 code point from no more than the specified `maxBytes`
/// of the byte stream at the specified `bytes` location. Return a `
/// Extract a UTF-8 code point from no more than `maxBytes` of the byte
/// stream at the specified `bytes` location. Return a
/// `CodePointExtractionResult` providing a decode status and, if the
/// decode is valid, a count of the source bytes used and the decoded
/// Unicode code point value. Byte Order Markers are not supported.
bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult
extractUtf8(const void *bytes, size_t maxBytes)
{
    bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult result;
    memset(&result,
           0,
           sizeof(bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult));

    if (0 == bytes || maxBytes < 1)
        return result;                                                // RETURN

    if (isByteOrderMarker(bytes, maxBytes))
        return result;                                                // RETURN

    const unsigned char *start = static_cast<const unsigned char *>(bytes);

    switch (static_cast<unsigned char>(*start) >> 4) {
      case 0x0:
        BSLA_FALLTHROUGH;
      case 0x1:
        BSLA_FALLTHROUGH;
      case 0x2:
        BSLA_FALLTHROUGH;
      case 0x3:
        BSLA_FALLTHROUGH;
      case 0x4:
        BSLA_FALLTHROUGH;
      case 0x5:
        BSLA_FALLTHROUGH;
      case 0x6:
        BSLA_FALLTHROUGH;
      case 0x7: {
        result.codePointValue = *start;
        result.numSourceBytes = 1;
        result.isValid        = true;
        result.sourceEncoding = bslfmt::Formatter_UnicodeUtils::e_UTF8;
        result.codePointWidth = 1;  // simple ascii value.
      } break;
      case 0x8:
        BSLA_FALLTHROUGH;
      case 0x9:
        BSLA_FALLTHROUGH;
      case 0xa:
        BSLA_FALLTHROUGH;
      case 0xb: {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Unexpected continuation octet
        return result;                                                // RETURN
      } break;
      case 0xc:
        BSLA_FALLTHROUGH;
      case 0xd: {
        if (UNLIKELY(maxBytes < 2)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Too few bytes available given the length indicated by the first
            // byte.
            return result;                                            // RETURN
        }

        const unsigned int value = get2ByteUtf8Value(start);

        if (UNLIKELY(value < k_MIN_2_BYTE_UTF8_VALUE)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Overlong encoding
            return result;                                            // RETURN
        }
        if (UNLIKELY(isNotUtf8Continuation(start[1]))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Non-continuation octet
            return result;                                            // RETURN
        }

        result.codePointValue = value;
        result.numSourceBytes = 2;
        result.isValid        = true;
        result.sourceEncoding = bslfmt::Formatter_UnicodeUtils::e_UTF8;
        result.codePointWidth = getCodepointWidth(value);
      } break;
      case 0xe: {
        if (UNLIKELY(maxBytes < 3)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Too few bytes available given the length indicated by the first
            // byte.
            return result;                                            // RETURN
        }

        const unsigned int value = get3ByteUtf8Value(start);

        if (UNLIKELY(value < k_MIN_3_BYTE_UTF8_VALUE)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Overlong encoding
            return result;                                            // RETURN
        }
        if (UNLIKELY(isNotUtf8Continuation(start[1]) ||
                     isNotUtf8Continuation(start[2]))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Non-continuation octet
            return result;                                            // RETURN
        }
        if (UNLIKELY(isSurrogateValue(value))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Erroneous surrogate value
            return result;                                            // RETURN
        }

        result.codePointValue = value;
        result.numSourceBytes = 3;
        result.isValid        = true;
        result.sourceEncoding = bslfmt::Formatter_UnicodeUtils::e_UTF8;
        result.codePointWidth = getCodepointWidth(value);
      } break;
      case 0xf: {
        if (UNLIKELY(maxBytes < 4)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Too few bytes available given the length indicated by the first
            // byte.
            return result;                                            // RETURN
        }

        const unsigned int value = get4ByteUtf8Value(start);

        if (UNLIKELY(bool(0x8 & *start))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Invalid initial octet for 4-byte value
            return result;                                            // RETURN
        }
        if (UNLIKELY(value < k_MIN_4_BYTE_UTF8_VALUE)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Overlong encoding
            return result;                                            // RETURN
        }
        if (UNLIKELY(value > k_MAX_UTF_VALID)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Value exceeds 0X10FFFF
            return result;                                            // RETURN
        }
        if (UNLIKELY(isNotUtf8Continuation(start[1]) ||
                     isNotUtf8Continuation(start[2]) ||
                     isNotUtf8Continuation(start[3]))) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            // Non-continuation octet
            return result;                                            // RETURN
        }

        result.codePointValue = value;
        result.numSourceBytes = 4;
        result.isValid        = true;
        result.sourceEncoding = bslfmt::Formatter_UnicodeUtils::e_UTF8;
        result.codePointWidth = getCodepointWidth(value);
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("unreachable");
      }
    }

    return result;
}

/// Extract a UTF-16 code point from no more than the specified `maxBytes`
/// of the byte stream at the specified `bytes` location. Return a
/// `CodePointExtractionResult` providing a decode status and, if the
/// decode is valid, a count of the source bytes used and the decoded
/// Unicode code point value. Behavior is undefined if `bytes` is not a
/// valid pointer to an array of `numBytes/2` `wchar_t` types in contiguous
/// memory. Behaviour is undefined if `16 != sizeof(wchar_t)`. Endianness
/// is assumed to be the same as for the `wchar_t` type and Byte Order
/// Markers are not supported.
bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult
extractUtf16(const void *bytes, size_t maxBytes)
{
    bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult result;
    memset(&result,
           0,
           sizeof(bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult));

    // If ever this function is called on a platform with a 32-bit wchar_t
    // (such as on Linux) then something went very badly wrong.
    BSLS_ASSERT_OPT(sizeof(wchar_t) == 2);

    if (0 == bytes || maxBytes < 2 || sizeof(wchar_t) != 2)
        return result;                                                // RETURN

    if (isByteOrderMarker(bytes, maxBytes))
        return result;                                                // RETURN

    const wchar_t *start = static_cast<const wchar_t *>(bytes);

    const unsigned int first = static_cast<unsigned int>(*start);

    if (UNLIKELY(isLowSurrogateValue(first))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            // Illegal to have a low surrogate without a preceeding high
            // surrogate.

        return result;                                                // RETURN
    }

    if (!isHighSurrogateValue(first)) {
        // Illegal to have a low surrogate without a preceeding high surrogate.

        result.codePointValue = first;
        result.numSourceBytes = 2;
        result.isValid        = true;
        result.sourceEncoding = bslfmt::Formatter_UnicodeUtils::e_UTF16;
        result.codePointWidth = getCodepointWidth(first);

        return result;                                                // RETURN
    }

    if (UNLIKELY(maxBytes < 4)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Too few bytes available given the first is a surrogate
        return result;                                                // RETURN
    }

    const unsigned int second = static_cast<unsigned int>(*(start + 1));

    if (UNLIKELY(!isLowSurrogateValue(second))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            // High surrogate must be followed by a low surrogate

        return result;                                                // RETURN
    }

    const unsigned int value = (((first - k_UTF16_HIGH_SURROGATE_START)
                                 << 10) |
                                (second - k_UTF16_LOW_SURROGATE_START)) +
                               k_UTF16_SURROGATE_PAIR_OFFSET;

    if (UNLIKELY(value > k_MAX_UTF_VALID)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Value exceeds 0X10FFFF
        return result;                                                // RETURN
    }

    result.codePointValue = value;
    result.numSourceBytes = 4;
    result.isValid        = true;
    result.sourceEncoding = bslfmt::Formatter_UnicodeUtils::e_UTF16;
    result.codePointWidth = getCodepointWidth(value);

    return result;                                                    // RETURN
}

/// Extract a UTF-32 code point from no more than the specified `maxBytes`
/// of the byte stream at the specified `bytes` location. Return a
/// `CodePointExtractionResult` providing a decode status and, if the
/// decode is valid, a count of the source bytes used and the decoded
/// Unicode code point value. Behavior is undefined if `bytes` is not a
/// valid pointer to an array of `numBytes/2` `wchar_t` types in contiguous
/// memory. Behaviour is undefined if `32 != sizeof(wchar_t)`. Endianness
/// is assumed to be the same as for the `wchar_t` type and Byte Order
/// Markers are not supported.
bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult
extractUtf32(const void *bytes, size_t maxBytes)
{
    bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult result;
    memset(&result,
           0,
           sizeof(bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult));

    // If ever this function is called on a platform with a 32-bit wchar_t
    // (such as on Linux) then something went very badly wrong.
    BSLS_ASSERT_OPT(sizeof(wchar_t) == 4);

    if (sizeof(wchar_t) != 4)
        return result;                                                // RETURN

    if (0 == bytes || maxBytes < 4)
        return result;                                                // RETURN

    if (isByteOrderMarker(bytes, maxBytes))
        return result;                                                // RETURN

    const wchar_t *start = static_cast<const wchar_t *>(bytes);

    const unsigned int value = static_cast<unsigned int>(*start);

    if (UNLIKELY(isSurrogateValue(value))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            // Illegal to have a low surrogate values in any UTF schema.

        return result;                                                // RETURN
    }

    if (UNLIKELY(value > k_MAX_UTF_VALID)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Value exceeds 0X10FFFF
        return result;                                                // RETURN
    }

    result.codePointValue = value;
    result.numSourceBytes = 4;
    result.isValid        = true;
    result.sourceEncoding = bslfmt::Formatter_UnicodeUtils::e_UTF32;
    result.codePointWidth = getCodepointWidth(value);
    return result;                                                    // RETURN
}

class Formatter_UnicodeData_StartCompare_GB11_LH_Regex
    // A component-private state machine required to detect emoji modifier
    // sequences per https://www.unicode.org/reports/tr29/#GB11
{
  private:
    // PRIVATE TYPES
    enum State {
        e_START,   // Startup state
        e_EXT_PIC  // State indicating `match` has been called for a single
                   // Extended_Pictogram and zero or more Extend characters.
    };

    // DATA
    State d_state; // The current state.

    // NOT IMPLEMENTED
    bool operator==(const Formatter_UnicodeData_StartCompare_GB11_LH_Regex&)
        BSLS_KEYWORD_DELETED;

  public:
    // CREATORS

    Formatter_UnicodeData_StartCompare_GB11_LH_Regex();
        // Construct an object in `e_START` state.

    // MANIPULATORS
    bool
    match(const bslfmt::Formatter_UnicodeData::GraphemeBreakCategory left_gbp,
          bool left_epv)
        // Return true for the ZWJ codepoint (determined by the specified
        // `left_gbp` being `e_ZERO_WIDTH_JOINER`) and the function was
        // previously called for an Extended_Pictogram codepoint (determined by
        // the specified `left_epv` being true) followed by zero or more Extend
        // codepoints (determined by the specified `left_gbp` being
        // `e_EXTEND`). Update the internal state to enable correct
        // calculations on subsequent calls.
    {
        switch (d_state) {
          case e_START:
            if (left_epv) {
                d_state = e_EXT_PIC;
            }
            return false;
          case e_EXT_PIC:
            if (left_gbp == bslfmt::Formatter_UnicodeData::
                                e_ZERO_WIDTH_JOINER) {
                d_state = e_START;
                return true;
            }
            else if (left_gbp != bslfmt::Formatter_UnicodeData::
                                     e_EXTEND) {
                d_state = e_START;
                return false;
            }
            return false;
          default:
            BSLS_ASSERT(false);
            return false;
        }
    }
};

Formatter_UnicodeData_StartCompare_GB11_LH_Regex::
    Formatter_UnicodeData_StartCompare_GB11_LH_Regex()
: d_state(e_START)
{
}

}  // close unnamed namespace

namespace BloombergLP {
namespace bslfmt {

Formatter_UnicodeUtils::CodePointExtractionResult
Formatter_UnicodeUtils::extractCodePoint(UtfEncoding  encoding,
                                         const void   *bytes,
                                         size_t        maxBytes)
{
    switch (encoding) {
        case e_UTF8: {
            return extractUtf8(bytes, maxBytes);
        } break;
        case e_UTF16: {
            return extractUtf16(bytes, maxBytes);
        } break;
        case e_UTF32: {
            return extractUtf32(bytes, maxBytes);
        } break;
        default: {
            BSLS_ASSERT_INVOKE_NORETURN("unreachable");
            BSLA_UNREACHABLE;
            // Dummy return statment to alleviate compiler warnings.
            return CodePointExtractionResult();
        }
    }
}

Formatter_UnicodeUtils::GraphemeClusterExtractionResult
Formatter_UnicodeUtils::extractGraphemeCluster(UtfEncoding  encoding,
                                               const void   *bytes,
                                               size_t        maxBytes)
{
    GraphemeClusterExtractionResult result;

    CodePointExtractionResult codepoint = extractCodePoint(encoding,
                                                           bytes,
                                                           maxBytes);

    bool              isValid             = codepoint.isValid;
    int               numBytes            = codepoint.numSourceBytes;
    int               numCodepoints       = 1;

    // Failed to extract first code point.
    if (!isValid) {
        memset(&result, 0, sizeof(GraphemeClusterExtractionResult));
        return result;                                                // RETURN
    }

    result.firstCodePointValue = codepoint.codePointValue;
    result.firstCodePointWidth = codepoint.codePointWidth;
    result.sourceEncoding      = encoding;

    Formatter_UnicodeData::GraphemeBreakCategory left_gbp =
                            getGraphemeBreakCategory(codepoint.codePointValue);
    bool left_epv = getExtendedPictogramValue(codepoint.codePointValue);

    Formatter_UnicodeData::GraphemeBreakCategory right_gbp =
                    Formatter_UnicodeData::e_UNASSIGNED;
    bool right_epv = false;

    size_t num_RIs   = 0;

    Formatter_UnicodeData_StartCompare_GB11_LH_Regex gb11_matcher;

    for (;; left_gbp = right_gbp, left_epv = right_epv) {

        result.isValid        = isValid;
        result.numCodePoints  = numCodepoints;
        result.numSourceBytes = numBytes;

        if (0 == maxBytes - numBytes) {
                // GB2 Any % eot
                return result;                                        // RETURN
        }

        const void *cp = static_cast<const void *>(
                                  static_cast<const char *>(bytes) + numBytes);
        codepoint = extractCodePoint(encoding, cp, maxBytes - numBytes);

        isValid = isValid && codepoint.isValid;
        numBytes += codepoint.numSourceBytes;
        numCodepoints++;

        // Failed to extract valid code point.
        if (!isValid) {
                memset(&result, 0, sizeof(GraphemeClusterExtractionResult));
                return result;                                        // RETURN
        }

        right_gbp  = getGraphemeBreakCategory(codepoint.codePointValue);
        right_epv = getExtendedPictogramValue(codepoint.codePointValue);

        // match GB11 now, so that we're sure to update it for every character,
        // not just ones where the GB11 rule is considered
        const bool is_GB11_Match = gb11_matcher.match(left_gbp, left_epv);

        // Also update the number of sequential RIs immediately
        if (left_gbp == Formatter_UnicodeData::e_REGIONAL_INDICATOR) {
                ++num_RIs;
        }
        else {
                num_RIs = 0;
        }

        if (left_gbp == Formatter_UnicodeData::e_CR &&
            right_gbp == Formatter_UnicodeData::e_LF) {
                continue;  // GB3 CR x LF
        }

        if (left_gbp ==
                Formatter_UnicodeData::e_CONTROL ||
            left_gbp == Formatter_UnicodeData::e_CR ||
            left_gbp == Formatter_UnicodeData::e_LF) {
                // GB4 (Control | CR | LF) % Any
                return result;                                        // RETURN
        }

        if (right_gbp ==
                Formatter_UnicodeData::e_CONTROL ||
            right_gbp == Formatter_UnicodeData::e_CR ||
            right_gbp == Formatter_UnicodeData::e_LF) {
                // GB5 Any % (Control | CR | LF)
                return result;                                        // RETURN
        }

        if ((left_gbp ==
                Formatter_UnicodeData::e_HANGUL_L) &&
            (right_gbp ==
                 Formatter_UnicodeData::e_HANGUL_L ||
             right_gbp ==
                 Formatter_UnicodeData::e_HANGUL_V ||
             right_gbp ==
                 Formatter_UnicodeData::e_HANGUL_LV ||
             right_gbp ==
                 Formatter_UnicodeData::e_HANGUL_LVT)) {
                continue;  // GB6 L x (L | V | LV | LVT)
        }

        if ((left_gbp ==
                 Formatter_UnicodeData::e_HANGUL_LV ||
             left_gbp ==
                 Formatter_UnicodeData::e_HANGUL_V) &&
            (right_gbp ==
                 Formatter_UnicodeData::e_HANGUL_V ||
             right_gbp ==
                 Formatter_UnicodeData::e_HANGUL_T)) {
                continue;  // GB7 (LV | V) x (V | T)
        }

        if ((left_gbp ==
                 Formatter_UnicodeData::e_HANGUL_LVT ||
             left_gbp ==
                 Formatter_UnicodeData::e_HANGUL_T) &&
            (right_gbp ==
                Formatter_UnicodeData::e_HANGUL_T)) {
                continue;  // GB8 (LVT | T) x T
        }

        if (right_gbp ==
                Formatter_UnicodeData::e_EXTEND ||
            right_gbp == Formatter_UnicodeData::e_ZERO_WIDTH_JOINER) {
                continue;  // GB9 x (Extend | ZWJ)
        }

        if (right_gbp ==
            Formatter_UnicodeData::e_SPACING_MARK) {
                continue;  // GB9a x SpacingMark
        }

        if (left_gbp ==
            Formatter_UnicodeData::e_PREPEND) {
                continue;  // GB9b Prepend x
        }

        if (is_GB11_Match && right_epv) {
                // GB11 \p{ExtendedPictographic} Extend* ZWJ x
                // \p{ExtendedPictographic}
                continue;
        }

        if (left_gbp == Formatter_UnicodeData::e_REGIONAL_INDICATOR &&
            right_gbp == Formatter_UnicodeData::e_REGIONAL_INDICATOR &&
            num_RIs % 2 != 0) {
                // GB12 and 13, do not break between RIs if there are an odd
                // number of RIs before the breakpoint
                continue;
        }

        // No rule will cause `right` to extend the cluster, so return what we
        // have.
        return result;                                                // RETURN
    }
}


}  // close namespace bslfmt
}  // close enterprise namespace

// ----------------------------------------------------------------------------
//
// The class Formatter_UnicodeData_StartCompare_GB11_LH_Regex and the function
// Formatter_UnicodeUtils::extractGraphemeCluster were adapted for bde use from
// Microsoft MSVC library code, 2024
//  'https://github.com/microsoft/STL/blob/main/stl/inc/format'
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// 
// NOTE:
// The Microsoft `format` header was itself derived in part from libfmt under
// the following license:
// 
// Copyright (c) 2012 - present, Victor Zverovich
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// --- Optional exception to the license ---
//
// As an exception, if, as a result of your compiling your source code,
// portions of this Software are embedded into a machine-executable object form
// of such source code, you may redistribute such embedded portions in such
// object form without including the above copyright and permission notices.
// 
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
