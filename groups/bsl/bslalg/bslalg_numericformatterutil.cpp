// bslalg_numericformatterutil.cpp                                    -*-C++-*-
#include <bslalg_numericformatterutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslmf_enableif.h>

#include <bsls_buildtarget.h>
#include <bsls_libraryfeatures.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <algorithm>
#include <cctype>
#include <climits>
#include <cstddef>
#include <cstring>
#include <limits>

#include <ryu/blp_ryu.h>

namespace {
namespace u {

using namespace BloombergLP;

typedef bsls::Types::Uint64                          Uint64;

// 'shiftLookup' is a lookup table for quickly determining the number of digits
// to shift for each character rendered when rendering a number in a base that
// is a power of 2.  If the shift value at a given index is 0, than then base
// is not a power of 2 (so the lookup table cannot be used.  For example, for
// base 8 (a power of two), 'shiftLookup[8]' returns 3, meaning that each
// rendered digit shifts the input bits to the right by 3 bits, e.g.,
// 'value >>= shiftLookup[8]' (this is equivalent to dividing by the base, 8).

static const unsigned char shiftLookup[] = { 0,  0,  1,  0,  2,  0,  0,  0,
                                             3,  0,  0,  0,  0,  0,  0,  0,
                                             4,  0,  0,  0,  0,  0,  0,  0,
                                             0,  0,  0,  0,  0,  0,  0,  0,
                                             5,  0,  0,  0,  0 };
BSLMF_ASSERT(sizeof(shiftLookup) == 37);

// 'twoDigitStrings' is the concatenation of two-digit representations of
// numbers from 00 to 99.  This enables us to divide by 100 rather than 10 in
// the loops and print our result with half as many divides and mods.

static const char twoDigitStrings[] = { "00010203040506070809"
                                        "10111213141516171819"
                                        "20212223242526272829"
                                        "30313233343536373839"
                                        "40414243444546474849"
                                        "50515253545556575859"
                                        "60616263646566676869"
                                        "70717273747576777879"
                                        "80818283848586878889"
                                        "90919293949596979899" };
BSLMF_ASSERT(sizeof(twoDigitStrings) == 201);

inline
char digitToAscii(unsigned digit) BSLS_KEYWORD_NOEXCEPT
    // Translate the specified 'digit' which is a number in the range
    // '[ 0 .. 36 )' to an ASCII char ('0' + digit) for 'digit < 10' and ('a' -
    // 10 + digit) for values higher than that.  The behavior is undefined
    // unless 'digit < 36'.
{
    BSLS_ASSERT_SAFE(digit < 36);

    enum { k_CHAR_BIT_SHIFT = 3,
           k_CHAR_BIT       = 1 << k_CHAR_BIT_SHIFT,
           k_PATTERN        = (('a' - 10) << k_CHAR_BIT) | '0' };

    const char offset = static_cast<char>(
                             k_PATTERN >> ((10 <= digit) << k_CHAR_BIT_SHIFT));
    return static_cast<char>(offset + digit);
}

inline
char *toCharsBase10Uint32(char           *first,
                          char           *last,
                          unsigned        value) BSLS_KEYWORD_NOEXCEPT
    // On success, render the specified 'value', in decimal ASCII form, to the
    // beginning of the memory specified by '[ first .. last )' and return the
    // address one past the lowest order digit written.  If the range
    // '[ first .. last )' is not large enough to contain the result, fail and
    // return 0 without rendering any digits.
{
    // The following computes 'length', one less than the length in bytes
    // required to render the result.

    enum { k_10000 = 10 * 1000 };

    int length;
    if (value < 100) {
        length = 10 <= value;
    }
    else {
        unsigned tmpValue = value;
        length = 0;

        while (k_10000 <= tmpValue) {
            tmpValue /= k_10000;
            length   += 4;
        }

        length += (tmpValue >= 10) + (tmpValue >= 100) + (tmpValue >= 1000);
    }

    // From here on out, the number of digits remaining to be printed is
    // 'length + 1'.

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(last - first <= length)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        return 0;                                                     // RETURN
    }
    last = first + length + 1;

    while (1 < length) {
        // 3 or more digits to be printed.

        const unsigned twoDigitIdx = (value % 100) * 2;
        value /= 100;

        first[length]     = u::twoDigitStrings[twoDigitIdx + 1];
        first[length - 1] = u::twoDigitStrings[twoDigitIdx];
        length -= 2;
    }

    if (length) {
        // Exactly 2 digits left to be printed.

        BSLS_ASSERT_SAFE(10 <= value);

        value *= 2;

        first[1] = u::twoDigitStrings[value + 1];
        first[0] = u::twoDigitStrings[value];
    }
    else {
        // Exactly 1 digit left to print.  Note that this includes the case
        // where 'value' passed to the function was 0.

        *first = static_cast<char>('0' + value);
    }

    return last;
}

inline
char *toCharsBase10Uint64(char          *first,
                          char          *last,
                          Uint64         value) BSLS_KEYWORD_NOEXCEPT
    // On success, render the specified 'value', in decimal ASCII form, to the
    // beginning of the memory specified by '[ first .. last )' and return the
    // address one past the lowest order digit written.  If the range
    // '[ first .. last )' is not large enough to contain the result, fail and
    // return 0 without rendering any digits.
{
    // The following computes 'length', one less than the length in bytes
    // required to render the result.

    enum { k_10000 = 10 * 1000 };

    int length;
    if (value < 100) {
        length = 10 <= value;
    }
    else {
        Uint64 tmpValue = value;
        length = 0;

        while (UINT_MAX < tmpValue) {
            tmpValue /= k_10000;
            length   += 4;
        }

        unsigned tmpValue32 = static_cast<unsigned>(tmpValue);

        while (k_10000 <= tmpValue32) {
            tmpValue32 /= k_10000;
            length     += 4;
        }

        length += (tmpValue32 >= 10) +
                                    (tmpValue32 >= 100) + (tmpValue32 >= 1000);
    }

    // From here on out, the number of digits remaining to be printed is
    // 'length + 1'.

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(last - first <= length)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        return 0;                                                     // RETURN
    }
    last = first + length + 1;

    while (8 < length) {
        // 10 or more digits to be printed.

        const unsigned twoDigitIdx = static_cast<unsigned>(value % 100) * 2;
        value /= 100;

        first[length]     = u::twoDigitStrings[twoDigitIdx + 1];
        first[length - 1] = u::twoDigitStrings[twoDigitIdx];
        length -= 2;
    }

    // 'value' is now 9 digits or less -- it can fit in a 32-bit variable.
    // Switch for faster divides and mods.

    unsigned value32 = static_cast<unsigned>(value);

    while (1 < length) {
        // 3 or more digits left to be printed.

        const unsigned twoDigitIdx = (value32 % 100) * 2;
        value32 /= 100;

        first[length]     = u::twoDigitStrings[twoDigitIdx + 1];
        first[length - 1] = u::twoDigitStrings[twoDigitIdx];
        length -= 2;
    }

    if (length) {
        // Exactly 2 digits left to be printed.

        BSLS_ASSERT_SAFE(10 <= value32);

        value32 *= 2;

        first[1] = u::twoDigitStrings[value32 + 1];
        first[0] = u::twoDigitStrings[value32];
    }
    else {
        // Exactly 1 digit left to print.

        // The only case where the leading digit of the number should be '0' is
        // when 'value' passed to the function was 0.

        BSLS_ASSERT_SAFE(value32 || 0 == value);

        *first = static_cast<char>('0' + value32);
    }

    return last;
}

inline
char *toCharsArbitraryBaseUint32(char     *first,
                                 char     *last,
                                 unsigned  value,
                                 unsigned  base) BSLS_KEYWORD_NOEXCEPT
    // On success, render the specified 'value' in the specified 'base' to the
    // beginning of the buffer specified by '[ first .. last )' and return the
    // address one past the lowest order digit rendered.  If the specified
    // buffer is not large enough to accommodate the result, return 0 without
    // writing to the buffer.
{
    BSLS_ASSERT_SAFE(0 != (base & (base - 1)));    // not a power of 2
    BSLS_ASSERT_SAFE(10 != base);

    // 'buffer' is sized large enough to handle the highest possible value of
    // 'value' in binary.

    char  buffer[sizeof(value) * 8];
    char *end = buffer + sizeof(buffer);

    // Use a 'do ... while' to cover the case where 'value' was 0.

    int minusLength = 0;
    do {
        end[--minusLength] = u::digitToAscii(value % base);
        value /= base;
    } while (0 < value);

    const int length = -minusLength;

    BSLS_ASSERT_SAFE(0 < length);
    BSLS_ASSERT_SAFE(length <= static_cast<int>(sizeof(buffer)));

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(last - first < length)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        return 0;                                                     // RETURN
    }

    std::memcpy(first, end + minusLength, length);

    return first + length;
}

inline
char *toCharsArbitraryBaseUint64(char     *first,
                                 char     *last,
                                 Uint64    value,
                                 unsigned  base) BSLS_KEYWORD_NOEXCEPT
    // On success, render the specified 'value' in the specified 'base' to the
    // beginning of the buffer specified by '[ first .. last )' and return the
    // address one past the lowest order digit rendered.  If the specified
    // buffer is not large enough to accommodate the result, return 0 without
    // writing to the buffer.
{
    BSLS_ASSERT_SAFE(0 != (base & (base - 1)));    // not a power of 2
    BSLS_ASSERT_SAFE(10 != base);

    // 'buffer' is sized large enough to handle the highest possible value of
    // 'value' in binary.

    char  buffer[sizeof(value) * 8];
    char *end = buffer + sizeof(buffer);

    int minusLength = 0;
    while (UINT_MAX < value) {
        end[--minusLength] =
                          u::digitToAscii(static_cast<unsigned>(value % base));
        value /= base;
    }

    // Switch 'value' to smaller variable for faster divides and mods, and use
    // a 'do ... while' to handle the case where the 'value' input was 0.

    unsigned value32 = static_cast<unsigned>(value);

    do {
        end[--minusLength] = u::digitToAscii(value32 % base);
        value32 /= base;
    } while (0 < value32);

    const int length = -minusLength;

    BSLS_ASSERT_SAFE(0 < length);
    BSLS_ASSERT_SAFE(length <= static_cast<int>(sizeof(buffer)));

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(last - first < length)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        return 0;                                                     // RETURN
    }

    std::memcpy(first, end + minusLength, length);

    return first + length;
}

inline
char *toCharsPowerOf2Base(char          *first,
                          char          *last,
                          Uint64         value,
                          unsigned       base,
                          unsigned char  shift) BSLS_KEYWORD_NOEXCEPT
    // On success, render the specified 'value' in the specified 'base', which
    // is known to be a power of 2, to the beginning of the buffer specified by
    // '[ first .. last )' and return one past the address of the lowest order
    // digit rendered.  Specify 'shift' which is the log in base 2 of 'base'.
    // If the buffer is not large enough to accommodate the result, return 0
    // without writing to the buffer.  The behavior is undefined unless 'shift'
    // is in the range '[ 1 .. 5 ]', unless 'base == (1 << shift)'.
    //
    // Note that we don't have a separate 32-bit version of this function,
    // because there are no divides or mods, and masks and shifts are very
    // fast, even on a 64 bit value.
{
    BSLS_ASSERT_SAFE(base == (1U << shift));
    BSLS_ASSERT_SAFE(1 <= shift);
    BSLS_ASSERT_SAFE(shift <= 5);

    // Make 'mask' an 'unsigned char' to hint to the compiler how low we know
    // the value will be.  This will enable the compiler to do an 8-bit, rather
    // than a 64-bit, mask.

    const unsigned char mask = static_cast<unsigned char>(base - 1);

    // 'buffer' is sized large enough to handle the highest possible value of
    // 'value' in binary.

    char  buffer[sizeof(value) * 8];
    char *end = buffer + sizeof(buffer);

    int minusLength = 0;
    do {
        end[--minusLength] = u::digitToAscii(
                                     static_cast<unsigned char>(value & mask));
        value >>= shift;
    } while (0 < value);

    const int length = -minusLength;

    BSLS_ASSERT_SAFE(0 < length);
    BSLS_ASSERT_SAFE(length <= static_cast<int>(sizeof(buffer)));

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(last - first < length)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        return 0;                                                     // RETURN
    }

    std::memcpy(first, end + minusLength, length);

    return first + length;
}
                        // =======================
                        // struct NumDecimalDigits
                        // =======================

template <int NUMBER, class = void>
struct NumDecimalDigits {
    static const size_t value = 1 + NumDecimalDigits<NUMBER / 10>::value;
};

template <int NUMBER>
struct NumDecimalDigits<NUMBER, typename bslmf::EnableIf<NUMBER < 10>::type> {
    static const size_t value = 1;
};

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bslalg {
                        // -----------------------------
                        // struct 'NumericFormatterUtil'
                        // -----------------------------


// PPRIVATE CLASS METHODS
char *NumericFormatterUtil::toCharsDecimal(char   *first,
                                           char   *last,
                                           double  value) BSLS_KEYWORD_NOEXCEPT
{
    typedef std::numeric_limits<double> lim;

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_ASSERT( true == lim::is_specialized); // Ensuring our assumptions
    BSLMF_ASSERT( true == lim::is_signed);      // about 'double' in case
    BSLMF_ASSERT(false == lim::is_exact);       // 'max_digits10' is not
#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD     // available
    // Yet another Sun "anomaly"
    BSLMF_ASSERT( true == lim::is_iec559);
#endif
    BSLMF_ASSERT(    8 == sizeof(double));
#endif

    BSLS_ASSERT(first <= last);

    static const std::ptrdiff_t k_BUFLEN =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        lim::max_digits10 +
#else
        17 +
#endif                                                          //   17
        lim::max_exponent10 +                                   // +308 ==> 325
        1 + // optional sign character                             +  1 ==> 326
        1;  // optional radix mark (decimal point)                 +  1 ==> 327

    if (last - first >= k_BUFLEN) {
        // Surely fits into the output area
        const int pos = blp_d2d_buffered_n(value, first);
        return first + pos;                                           // RETURN
    }
    else {
        // May be longer than the output area provided
        char buf[k_BUFLEN];
        const int pos = blp_d2d_buffered_n(value, buf);
        if (pos > last - first) {
            // Too long, won't fit
            return 0;                                                 // RETURN
        }
        else {
            // Will fit to output area, need to copy
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
            std::memcpy(first, buf, pos);
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic pop
#endif
            return first + pos;                                       // RETURN
        }
    }
}

char *NumericFormatterUtil::toCharsDecimal(char  *first,
                                           char  *last,
                                           float  value) BSLS_KEYWORD_NOEXCEPT
{
    typedef std::numeric_limits<float> lim;

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_ASSERT( true == lim::is_specialized); // Ensuring our assumptions
    BSLMF_ASSERT( true == lim::is_signed);      // about 'float' when
    BSLMF_ASSERT(false == lim::is_exact);       // 'max_digits10' is not
#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD     // available
    // Yet another Sun "anomaly"
    BSLMF_ASSERT( true == lim::is_iec559);
#endif
    BSLMF_ASSERT(    4 == sizeof(float));
#endif

    BSLS_ASSERT(first <= last);

    static const std::ptrdiff_t k_BUFLEN =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        lim::max_digits10 +
#else
        9 +
#endif                                                             //   9
        lim::max_exponent10 +                                      // +38 -> 47
        1 +   // optional sign character                              + 1 -> 48
        1     // optional radix mark (decimal point)                  + 1 -> 49
        - 1;  // See below                                            - 1 -> 48
        // It has been empirically determined that all IEEE-754 'binary32'
        // values with the largest (positive) and the smallest (negative)
        // exponent require only 8 significant decimal digits (not 9) to
        // represent them precisely, so we need just 48 characters space.

    if (last - first >= k_BUFLEN) {
        // Surely fits into the output area
        const int pos = blp_f2d_buffered_n(value, first);
        return first + pos;                                           // RETURN
    }
    else {
        // May be longer than the output area provided
        char buf[k_BUFLEN];
        const int pos = blp_f2d_buffered_n(value, buf);
        if (pos > last - first) {
            // Too long, won't fit
            return 0;                                                 // RETURN
        }
        else {
            // Will fit to output area, need to copy
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
            std::memcpy(first, buf, pos);
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic pop
#endif
            return first + pos;                                       // RETURN
        }
    }
}

char *
NumericFormatterUtil::toCharsScientific(char   *first,
                                        char   *last,
                                        double  value) BSLS_KEYWORD_NOEXCEPT
{
    typedef std::numeric_limits<double> lim;

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_ASSERT( true == lim::is_specialized); // Ensuring our assumptions
    BSLMF_ASSERT( true == lim::is_signed);      // about 'double' in case
    BSLMF_ASSERT(false == lim::is_exact);       // 'max_digits10' is not
#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD     // available
    // Yet another Sun "anomaly"
    BSLMF_ASSERT( true == lim::is_iec559);
#endif
    BSLMF_ASSERT(    8 == sizeof(double));
#endif

    BSLS_ASSERT(first <= last);

    static const std::ptrdiff_t k_BUFLEN =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        lim::max_digits10 +
#else
        17 +
#endif                                                            //  17
        u::NumDecimalDigits<lim::max_exponent10>::value +         // + 3 ==> 20
        1 + // optional sign character                            // + 1 ==> 21
        1 + // optional radix mark (decimal point)                // + 1 ==> 22
        1 + // 'e' of the scientific format                       // + 1 ==> 23
        1;  // sign for the scientific form exponent              // + 1 ==> 24

    if (last - first >= k_BUFLEN) {
        // Surely fits into the output area
        const int pos = blp_d2s_buffered_n(value, first);
        return first + pos;                                           // RETURN
    }
    else {
        // May be longer than the output area provided
        char buf[k_BUFLEN];
        const int pos = blp_d2s_buffered_n(value, buf);
        if (pos > last - first) {
            // Too long, won't fit
            return 0;                                                 // RETURN
        }
        else {
            // Will fit to output area, need to copy
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
            std::memcpy(first, buf, pos);
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic pop
#endif
            return first + pos;                                       // RETURN
        }
    }
}

char *
NumericFormatterUtil::toCharsScientific(char  *first,
                                        char  *last,
                                        float  value) BSLS_KEYWORD_NOEXCEPT
{
    typedef std::numeric_limits<float> lim;

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_ASSERT( true == lim::is_specialized); // Ensuring our assumptions
    BSLMF_ASSERT( true == lim::is_signed);      // about 'float' when
    BSLMF_ASSERT(false == lim::is_exact);       // 'max_digits10' is not
#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD     // available
    // Yet another Sun "anomaly"
    BSLMF_ASSERT( true == lim::is_iec559);
#endif
    BSLMF_ASSERT(    4 == sizeof(float));
#endif

    BSLS_ASSERT(first <= last);

    static const std::ptrdiff_t k_BUFLEN =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        lim::max_digits10 +
#else
        9 +
#endif                                                             //  9
        u::NumDecimalDigits<lim::max_exponent10>::value +          // +2 ==> 11
        1 + // optional sign character                             // +1 ==> 12
        1 + // optional radix mark (decimal point)                 // +1 ==> 13
        1 + // 'e' of the scientific format                        // +1 ==> 14
        1;  // sign for the scientific form exponent               // +1 ==> 15

    if (last - first >= k_BUFLEN) {
        // Surely fits into the output area
        const int pos = blp_f2s_buffered_n(value, first);
        return first + pos;                                           // RETURN
    }
    else {
        // May be longer than the output area provided
        char buf[k_BUFLEN];
        const int pos = blp_f2s_buffered_n(value, buf);
        if (pos > last - first) {
            // Too long, won't fit
            return 0;                                                 // RETURN
        }
        else {
            // Will fit to output area, need to copy
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
            std::memcpy(first, buf, pos);
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic pop
#endif
            return first + pos;                                       // RETURN
        }
    }
}

// CLASS METHODS
char *NumericFormatterUtil::toCharsImpl(char     *first,
                                        char     *last,
                                        unsigned  value,
                                        int       base) BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(base <= 36);
    BSLS_ASSERT_SAFE(first <= last);

    // The above 'BSLS_ASSERT_SAFE's are redundant with 'BSLS_ASSERT's done in
    // the caller.

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(10 == base)) {
        // 'base' is decimal

        return u::toCharsBase10Uint32(first, last, value);            // RETURN
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

    const unsigned char shift = u::shiftLookup[base];
    if (0 < shift) {
        // 'base' is a power of 2

        return u::toCharsPowerOf2Base(first, last, value, base, shift);
                                                                      // RETURN
    }

    // 'base' is neither decimal nor a power of 2

    return u::toCharsArbitraryBaseUint32(first, last, value, base);
}

char *NumericFormatterUtil::toCharsImpl(char                *first,
                                        char                *last,
                                        bsls::Types::Uint64  value,
                                        int                  base)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(base <= 36);
    BSLS_ASSERT_SAFE(first <= last);

    // The above 'BSLS_ASSERT_SAFE's are redundant with 'BSLS_ASSERT's done in
    // the caller.

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(10 == base)) {
        // 'base' is decimal

        return u::toCharsBase10Uint64(first, last, value);            // RETURN
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

    const unsigned char shift = u::shiftLookup[base];
    if (0 < shift) {
        // 'base' is a power of 2

        return u::toCharsPowerOf2Base(first, last, value, base, shift);
                                                                      // RETURN
    }

    // 'base' is neither decimal nor a power of 2

    return u::toCharsArbitraryBaseUint64(first, last, value, base);
}

char *NumericFormatterUtil::toChars(char   *first,
                                    char   *last,
                                    double  value) BSLS_KEYWORD_NOEXCEPT
{
    typedef std::numeric_limits<double> lim;

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_ASSERT( true == lim::is_specialized); // Ensuring our assumptions
    BSLMF_ASSERT( true == lim::is_signed);      // about 'double' in case
    BSLMF_ASSERT(false == lim::is_exact);       // 'max_digits10' is not
#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD     // available
    // Yet another Sun "anomaly"
    BSLMF_ASSERT( true == lim::is_iec559);
#endif
    BSLMF_ASSERT(    8 == sizeof(double));
#endif

    BSLS_ASSERT(first <= last);

    static const std::ptrdiff_t k_BUFLEN =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        lim::max_digits10 +
#else
        17 +
#endif                                                            //  17
        u::NumDecimalDigits<lim::max_exponent10>::value +         // + 3 ==> 20
        1 + // optional sign character                            // + 1 ==> 21
        1 + // optional radix mark (decimal point)                // + 1 ==> 22
        1 + // 'e' of the scientific format                       // + 1 ==> 23
        1;  // sign for the scientific form exponent              // + 1 ==> 24
        // Notice that if scientific form is shorter that is the one that will
        // be used, so essentially its maximum length determines the maximum.

    if (last - first >= k_BUFLEN) {
        // Surely fits into the output area
        const int pos = blp_d2m_buffered_n(value, first);
        return first + pos;                                           // RETURN
    }
    else {
        // May be longer than the output area provided
        char buf[k_BUFLEN];
        const int pos = blp_d2m_buffered_n(value, buf);
        if (pos > last - first) {
            // Too long, won't fit
            return 0;                                                 // RETURN
        }
        else {
            // Will fit to output area, need to copy
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
            std::memcpy(first, buf, pos);
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic pop
#endif
            return first + pos;                                       // RETURN
        }
    }
}

char *NumericFormatterUtil::toChars(char  *first,
                                    char  *last,
                                    float  value) BSLS_KEYWORD_NOEXCEPT
{
    typedef std::numeric_limits<float> lim;

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_ASSERT( true == lim::is_specialized); // Ensuring our assumptions
    BSLMF_ASSERT( true == lim::is_signed);      // about 'float' when
    BSLMF_ASSERT(false == lim::is_exact);       // 'max_digits10' is not
#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD     // available
    // Yet another Sun "anomaly"
    BSLMF_ASSERT( true == lim::is_iec559);
#endif
    BSLMF_ASSERT(    4 == sizeof(float));
#endif

    BSLS_ASSERT(first <= last);

    static const std::ptrdiff_t k_BUFLEN =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        lim::max_digits10 +
#else
        9 +
#endif                                                             //  9
        u::NumDecimalDigits<lim::max_exponent10>::value +          // +2 ==> 11
        1 + // optional sign character                             // +1 ==> 12
        1 + // optional radix mark (decimal point)                 // +1 ==> 13
        1 + // 'e' of the scientific format                        // +1 ==> 14
        1;  // sign for the scientific form exponent               // +1 ==> 15
        // Notice that if scientific form is shorter that is the one that will
        // be used, so essentially its maximum length determines the maximum.

    if (last - first >= k_BUFLEN) {
        // Surely fits into the output area
        const int pos = blp_f2m_buffered_n(value, first);
        return first + pos;                                           // RETURN
    }
    else {
        // May be longer than the output area provided
        char buf[k_BUFLEN];
        const int pos = blp_f2m_buffered_n(value, buf);
        if (pos > last - first) {
            // Too long, won't fit
            return 0;                                                 // RETURN
        }
        else {
            // Will fit to output area, need to copy
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
            std::memcpy(first, buf, pos);
#if defined(BSLS_PLATFORM_CMP_GNU) && (BDE_BUILD_TARGET_OPT)
#pragma GCC diagnostic pop
#endif
            return first + pos;                                       // RETURN
        }
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
