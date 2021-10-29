// bslalg_numericformatterutil.cpp                                    -*-C++-*-
#include <bslalg_numericformatterutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------
//
///Adding 'Format's
/// - - - - - - - -
// Floating point 'toChars' conversions have an optional 'format' parameter,
// with the type 'bslalg::NumericFormatterUtil::Format'.  This parameter/type
// is intended to be the equivalent of 'std::chars_format' from C++17.  We are
// implementing those formats we need, so 'hex' and 'general' are not yet
// implemented.  This component writes into output buffers (and has some
// interdependencies between values) we have to be very rigorous about
// implementing the missing formats.  This section provides a checklist that is
// meant to ensure that we do not add errors such as buffer overflow.
//
// Note that the 'toChars' overloads with 'precision' parameter are not
// supported either.  Adding those would require substantial design work that
// we will avoid until it is proven absolutely necessary.  (It is substantial
// work because we need a second non-type parameter to 'ToCharsMaxLength', we
// need to extend Ryu to support 'precision' for all formats, and because
// 'precision' is more likely to be a runtime value (than 'format') we might
// need to add a runtime way of calculating the necessary buffer size.)
//
// It is unusual to provide this detailed descriptions for further work,
// however because we suspend the work for an unspecified time it seemed
// prudent to record the details that might otherwise be forgotten.
//
/// The 'Format' Implementation Checklist
/// -  -  -  -  -  -  -  -  -  -  -  -  -
// The checklist has 5 goals:
//: 1 Ensure that the new enumerator values aren't also valid 'base' values for
//:   integer types (the are less than 2 or larger than 36).
//:
//: 2 Ensure no-excuses, precise calculation of the necessary output buffer
//:   size.  (Guessing or "let's add 10 to be sure" are not acceptable for such
//:   a low level component as every user of it pays the price of failure.)
//:
//: 3 Document the calculation so it can later be easily verified, or its logic
//:   applied to other IEEE binary floating point types (extended precision).
//:
//: 4 Verification of the above calculation on the subset of values that are
//:   feasible (see {Implementing 'ToCharsMaxLength'} below).
//:
//: 5 Ensuring good test coverage for the conversion itself, so that all corner
//:   case, special cases and branching in the implementation are covered.
//
// Rigorous math and relentless verification is important as IEEE-754 binary64,
// or 'double', has so many possible values that it is not feasible to verify
// all of them.
//
//:  0 Do not try to implement more than one format at once.
//:
//:  1 Move the to-be-implemented format enumerator and value into the body of
//:    the 'Format' 'enum' (from its documentation): 'e_HEX = 0x100', or
//:    'e_GENERAL = e_SCIENTIFIC | e_FIXED'.  The value for 'e_GENERAL'
//:    must be 'e_SCIENTIFIC | e_FIXED'.  All enumerators must have distinct
//:    values.
//:
//:  2 All the 'Format' enumerator values *must* be different from all valid
//:    'base' parameter values (of the integer 'toChars' overloads) to enable
//:    "type safety by values" in the 'ToCharsMaxLength' template.  To ensure
//:    that no subsequent (uninformed) update breaks the rule find, in this
//:    file, the 'U_FORMAT_AND_BASE_VALUE_DIFFERS' of the enumerator you are
//:    implementing and uncomment it.  Build this file, it should compile.
//:
//:  3 Study the definition of the format being implemented starting from C++17
//:    all the way to the current working draft by examining the specifications
//:    of the corresponding enumerator 'std::chars_format' 'enum class' as well
//:    as the 'std::to_chars' overloads of the '<charconv>' standard header.
//:    Look out for changes that affect the ASCII-only conversion implemented
//:    in this component.  Most probably the latest specification is what needs
//:    to be implemented.  If unclear, discuss with management or stakeholder.
//:
//:  4 Once the format specification is understood create concrete examples
//:    for both 'float' and 'double'.  See the existing test cases and test
//:    tables for ways to pair the binary float values (built from bits or
//:    specified by C++ floating point literals) with expected textual output.
//:    Use existing 3rd party implementations of 'std::to_chars' as "oracle" to
//:    verify your understanding of the format.  Those test tables will later
//:    be extended with more values to be the tests.  3rd party implementations
//:    may cater to backwards compatibility to old, non-standard 'printf'
//:    behavior, hence the quotes around "oracle".  You may need to introduce a
//:    special comparison function that knows about the deviations from the
//:    standard.  Be sure to use values that are "interesting" for the format,
//:    such as minimum, maximum, subnormals, exactly-represented integers etc.
//:    Use a huge buffer size for the output for now.
//:
//:  5 Take a break at this point for a *design review*, make sure that before
//:    you start writing production code you go through your understanding of
//:    the specification with a colleague, as well as your examples.
//:
//:  6 Implement the actual conversion for the format either by extending our
//:    existing Ryu code, or writing new code (Ryu may be of no use for
//:    'e_HEX').  Start with 'float', it is simpler to think about.  Create or
//:    extend the existing negative test case to test the implementation
//:    against the "oracle".  If you implement code in Ryu, implement tests for
//:    that code in the test driver of this component.
//:
//:  7 During implementation remember to fulfill the spirit of 'std::to_chars':
//:    it has to be light weight (for example no memory allocation), and very
//:    fast.  But do not go overboard with optimization, what you make is a
//:    temporary solution until all platforms we need to support will have all
//:    of '<charconv>' implemented with good quality.
//:
//:  8 Once you have implemented the 'toChars' for 'float' values examine the
//:    {Implementing 'ToCharsMaxLength'} section below, and follow similarly
//:    rigorous methodology/logic to determine the maximum necessary buffer
//:    size for 'float'.  Document your thinking.  Change the negative test
//:    case you've made to use that number for output buffer size.  Use the
//:    'prepareOutputBufer' and 'expectedBytesHaveChanged' functions in the
//:    test driver to ensure there is no buffer overrun on *all* platforms.
//:    See existing test code for examples.  On platforms that support address
//:    sanitizer (asan) use that with exact buffer size to verify no overrun
//:    occurs.  You want to verify two things: No 'float' value converts to
//:    text longer than the maximum you have calculated, and that at least
//:    *one* 'float' value converts to that exact length.  (Remember the length
//:    does not include  closing null character.)
//:
//:  9 Add the call to the new implementation in addition to the "oracle", and
//:    add tests for each branching decision your code does.  There should be
//:    no surprises as you have tested all 'float' values already, this is more
//:    of an exercise to create the thinking on how to test 'double', where we
//:    *cannot* test all values.  See existing test tables for different
//:    considerations.
//:
//: 10 Extend 'FltMaxLen' member template to support the newly implemented
//:    format for 'float'.  Add tests for it into MAXIMUM NECESSARY BUFFER
//:    LENGTH TESTS (case 17).
//:
//: 11 Clean-up the documentation on how you have calculated the maximum
//:    necessary buffer size for 'float' and the implemented format and add it
//:    below under the heading {Implementing 'ToCharsMaxLength'}.
//:
//: 12 After implementing the 'float' variant you are better prepared to face
//:    'double'.  The method is very similar.  Create a table of your
//:    understanding of the format and test it against some "oracles".  Select
//:    "interesting" numbers for the format.  Once you are confident you
//:    thought of every case do a design review with a colleague.
//:
//: 13 Implement 'toChars' for 'double' for the format.  Same as for 'float',
//:    if lower level functions are implemented, or existing ones changed they
//:    have to be tested on that level.
//:
//: 14 Add the 'toChars' call to the existing "oracle" tests, extend the table
//:    with additional values that verify branches in your code.  Use a huge
//:    buffer size for now.
//:
//: 15 It is not possible to test all 'double' values as it might require
//:    hundreds of thousands of CPU years.  Therefore your calculation for the
//:    maximum necessary buffer size must be extremely rigorous, and preferably
//:    reviewed by several people.  Document your thinking.  In that
//:    documentation reason about what values will be the longest in text.
//:
//: 16 Change the buffer size of the 'toChars'/"oracle" test case to the
//:    maximum you calculated.  Extend the table with values that you think
//:    will have the longest possible output.  Use address sanitizer (asan) and
//:    the helper functions to verify that the buffer is not overrun.
//:
//: 17 Extend 'FltMaxLen' member template to support the newly implemented
//:    format for 'double'.  Add tests for it into MAXIMUM NECESSARY BUFFER
//:    LENGTH TESTS (case 17).
//:
//: 18 Clean-up the documentation on how you have calculated the maximum
//:    necessary buffer size for 'double' and the implemented format and add it
//:    below under the heading {Implementing 'ToCharsMaxLength'}.
//:
//: 19 Update the 'toChars' contract by adding the new format.
//:
//: 20 Consider extending the usage examples to show the new format and what it
//:    is good for.
//
///Implementing 'ToCharsMaxLength'
///- - - - - - - - - - - - - - - -
// The 'ToCharsMaxLength' member template provides the necessary buffer sizes
// to ensure that a certain call to a 'toChars' overload succeeds for any value
// of its input type.  This section explains how we have calculated those
// values.
//
///Calculating Signed Integer Sufficient Buffer Length
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// The possible maximum length formula for signed integers for a base is:
// '1 + ceil(log(abs(minimum-value))/log(base))'.  Because we assume two's
// complement integers we know that the absolute value of the minimum is
// always the largest/longest.  The plus one is for the '-' character.
//
///Calculating Unsigned Integer Sufficient Buffer Length
/// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// The possible maximum length formula for unsigned integers for a base is:
// 'ceil(log(maximum-value))/log(base))'.  We know that for an unsigned
// integral type the maximum value is always the largest/longest.
//
///Calculating Floating Point Scientific Sufficient Buffer Length
/// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// The possible maximum length formula for floating point numbers in (C++)
// scientific format is:
// '1 + 1 + max_digits10 + 2 + numDecimalDigits(max(-min_exp10, max_exp10, 2))'
// sign + optional decimal radix point + maximum significand digits + the
// letter 'e' + the mandatory sign for the exponent + min. 2 exponent digits.
// Comes out to '2 + 9 + 2 + 2 == 15' for 'float' and '2 + 17 + 2 + 3 == 24'
// for 'double'.
//
///Calculating Floating Point Default Sufficient Buffer Length
/// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Because default format is specified to fall back to scientific format when
// that is shorter the maximum necessary buffer length is the same as above.
//
///Calculating Floating Point Fixed/Decimal Sufficient Buffer Length
/// -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// The decimal notation (or how C++ calls it: 'fixed', but its number of
// significant digits aren't always fixed) is more involved to calculate as
// both the length of the necessary decimal significand (for round-tripping)
// and the actual value of the decimal exponent is involved in the length of
// the necessary buffer for the output.  There are also huge complexity
// differences between 'float' (we are able to brute-force test all values) and
// 'double', hence they will be described separately.
//
// In general the sufficient decimal (fixed) length is the longest of 4
// variations: special values, integers, values less than one but larger than
// zero, and values that have both integer and fractional (decimal) digits.
// The longer are the negative values because they also have the '-' sign.  We
// can safely ignore the special values as they are very short (0, -0, NaN,
// -INF, +INF).  Numbers that have integer and fractional digits both cannot
// have more than 9 digits for 'float' and 17 for 'double', so those are very
// short as well (sign + 9 or 17 + 1 for the decimal dot).  So the
// "competition" for longest is between integers and numbers that are less than
// one but larger than zero.  Theoretical integer length can be calculated
// easiest by imagining the decimal scientific format of such value and how it
// can be turned into the decimal value.  We have a maximum of 9 or 17
// significand digits, the radix point is after the first one, and we have a
// non-negative decimal exponent.  Applying the non-negative decimal exponent
// means moving the decimal point to the right, consuming all the fractional
// digits of the significand, and then adding trailing zeros if we still have
// a positive exponent left.  This leaves us with the following formula:
// (9 or 17) + (maximum-exponent - (9 or 17) + 1), which simplifies into:
// maximum-exponent + 1.  The longest will turn out to be the numbers that are
// less than 1 but more than zero (that have fractional part only).  The
// theoretical calculation for those can be created by starting from the
// decimal scientific format which has 9 or 17 significand digits, and a
// negative exponent that is small than -1.  Creating the decimal involves
// moving the radix point to the left, consuming the only integer digit of the
// scientific significand, then adding leading zeros after the decimal point if
// it needs to be moved more because we still have a non-zero exponent value.
// The formula for the length of the theoretical longest number then comes to:
// 3 for "-0." + (abs(minimum-possible-normal-exponent) - 1) + (9 or 17),
// simplified: 2 + abs(minimum-possible-normal-exponent) + (9 or 17).  There is
// difficulty behind the simplicity of that formula, both are hinted at by the
// long description of the exponent value.  Why "normal"?  Why "possible"?  The
// first one is about subnormal numbers that we will discuss in more detail at
// the description for 'double'.  The possible is the bigger "pitfall" here, as
// the values C ('FLT_MAX_EXP_10' and 'DBL_MAX_EXP_10') and C++
// ('std::numeric_limits<>::min_exponent10') provides us are *not* the possible
// largest (in absolute value) negative decimal exponent values for a normal
// number!  They are the exponent value that, with *any* significand will
// create a normal floating point value (instead of a subnormal or denormal
// that has less precision).  So the formula that we use as base for the
// theoretical longest length of decimal floating point values will be:
// '3 - numeric_limits<T>::min_exponent10 + numeric_limits<T>::max_digits10'.
//
///Calculating 'float' Fixed/Decimal Sufficient Buffer Length
/// -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
// 'float' is a small enough type that we are able to verify our assumptions
// about it in minutes.  See test case -2 that verifies our length assumptions
// about all numeric 'float' values in less than 10 minutes.
//
// Applying our theoretical formula we get 3 + 37 + 9, which is 49 characters
// for the longest possible 'float' in decimal format.  But running test case
// -2 tells us that no printout is longer than 48 characters?  How is that
// possible?  This is about the value of 'numeric_limits<float>::max_digits10',
// and the *minimum* round trip length Ryu (that we use) provides.  The
// standard library 'max_digits10' value means: use this many decimal digits
// for *any* number a 'float' can represent, and it is guaranteed that it will
// round-trip (reading it back will result in the exact same 'float' value).
// But Ryu prints the *minimum* number of digits necessary, not the maximum.
// And our formula needs a 'max_digits10' value for the *subset* of values that
// have -38 as their decimal exponent.  It turns out, none of those values need
// 9 digits to round trip, 8 is perfectly enough.
//
///Calculating 'double' Fixed/Decimal Sufficient Buffer Length
///  -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
// 'double' has 32 more bits than 'float' to store numeric values.  It means
// that if we were try to run the same tests for 'double' that take around 8
// minutes for 'float' would result in a run-time of around 65 thousand years.
// Even if we throw a thousand computer cores at the problem that are twice as
// fast we would need more than a year.  Clearly, that is not an option, so our
// calculations for the longest decimal must be precise.
//
// Our theoretical formula gets 3 + 307 + 17, which is 327 characters.  However
// because we cannot just run a brute-force test to verify all numbers the
// question arises: could subnormal (denormal) numbers need more decimal
// digits?  The answer is no, but an explanation is necessary.
//
// What is a subnormal or denormal number?  Denormal is the old name, we will
// use subnormal from here on.  IEEE-754 values do not store the top bit of
// their binary significand (old name is mantissa) and it is always considered
// to be 1.  All calculations for value however use all 53 theoretical bits, so
// our 'max_digits10' value was calculated from 'ln(2^53)/ln(10) + 1', and so
// it includes *both* the zero and one value of that non-existent top bit.
// Why?  The answer is subnormals.  When a floating point number reaches its
// smallest normal value 'std::numeric_limits<T>::min()', it does not just drop
// to zero as the next value towards zero (see 'std::nextafter' of <cmath> of
// C++11 and C99).  For the smallest value the biased exponent stored in the
// 'double' is not 0, but 1.  Zero biased exponent is used for special values:
// zero itself when the significand is zero, and the so-called subnormals when
// the significand has a non-zero value.  Subnormals fill the "void" between
// 'std::numeric_limits<T>::min()' and 'std::numeric_limits<T>::denorm_min()'
// with values that have gracefully dropping precision by considering the
// "invisible" top (53rd) bit to be zero.  The formula for subnormal values is
// exactly the same as for normal values, except the top bit is assumed zero:
//:
//:      binExponent   --  unbiased IEEE 11 bit exponent value
//:      fractionBits  --  the 52 mantissa bits stored in the 'double'
//:
//:      normal    = 2^binExponent * 1.fractionBits(as binary)
//:      subnormal = 2^-1022       * 0.fractionBits(as binary)
//:
// The -1022 is the unbiased exponent value for IEEE biased exponent 1, not 0!
// That means that subnormals are a smooth continuation of the discrete values
// a 'double' can store and will not increase the length necessary for decimal
// notation representation.  The two reasons for this conclusion are: the
// binary unbiased exponent range is *not* extended by subnormals, and the 53rd
// bit being 0 was already included in the calculations of 'max_digit10'.
//
// However because the price of failure in our current implementation is a
// buffer overflow we have decided to very our assumptions.  Naive brute-force
// verification of all subnormal numbers would have taken 55 days using 3
// workstations.  That not being feasible a simplified code was created that
// does not convert the subnormals to text, just calculates their length.  That
// simplification and the use of loop parallelization has resulted in all
// possible 'double' subnormal values tested in approximately 3 days.  The test
// has proven that no subnormal value is longer than 327 characters when
// converted to decimal notation.

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

            // DEFENSIVE CHECKS FOR 'Format' ENUMERATOR VALUES

#define U_FORMAT_AND_BASE_VALUE_DIFFERS(enumerator)                           \
    BSLMF_ASSERT(bslalg::NumericFormatterUtil::enumerator <  2                \
              || bslalg::NumericFormatterUtil::enumerator > 36)
    // Macro to ensure no collision between supported 'base' values for
    // integral types and any of the 'Format' enumerators for floating point
    // types.

U_FORMAT_AND_BASE_VALUE_DIFFERS(e_FIXED);
U_FORMAT_AND_BASE_VALUE_DIFFERS(e_SCIENTIFIC);
//U_FORMAT_AND_BASE_VALUE_DIFFERS(e_HEX);
//U_FORMAT_AND_BASE_VALUE_DIFFERS(e_GENERAL);

// The above lines should be commented out when those floating point formats
// are implemented.

#undef U_FORMAT_AND_BASE_VALUE_DIFFERS

         // DEFENSIVE CHECKS FOR ASSUMPTIONS ABOUT FLOATING POINT TYPES

// Ensuring our assumptions about the 'double' type being IEEE-754 'binary64'
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_ASSERT( true == std::numeric_limits<double>::is_specialized);
    BSLMF_ASSERT( true == std::numeric_limits<double>::is_signed);
    BSLMF_ASSERT(false == std::numeric_limits<double>::is_exact);
#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD     // available
    // Yet another Sun "anomaly"
    BSLMF_ASSERT( true == std::numeric_limits<double>::is_iec559);
#endif
    BSLMF_ASSERT(    8 == sizeof(double));
#endif

 // Ensuring our assumptions about the 'float' type being IEEE-754 'binary32'
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    BSLMF_ASSERT( true == std::numeric_limits<float>::is_specialized);
    BSLMF_ASSERT( true == std::numeric_limits<float>::is_signed);
    BSLMF_ASSERT(false == std::numeric_limits<float>::is_exact);
#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD     // available
    // Yet another Sun "anomaly"
    BSLMF_ASSERT( true == std::numeric_limits<float>::is_iec559);
#endif
    BSLMF_ASSERT(    4 == sizeof(float));
#endif

                          // LOCALLY USED TYPES

typedef unsigned long long int Uint64;

                         // CONSTANT DATA TABLES

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

                  // INTEGRAL CONVERSION HELPER METHODS

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

                          // LOCAL META-FUNCTIONS

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
    BSLS_ASSERT(first <= last);

    const size_t k_BUFLEN = ToCharsMaxLength<double, e_FIXED>::k_VALUE;

    if (last - first >= static_cast<ptrdiff_t>(k_BUFLEN)) {
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
    BSLS_ASSERT(first <= last);

    const size_t k_BUFLEN = ToCharsMaxLength<float, e_FIXED>::k_VALUE;

    if (last - first >= static_cast<ptrdiff_t>(k_BUFLEN)) {
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
    BSLS_ASSERT(first <= last);

    const size_t k_BUFLEN = ToCharsMaxLength<double, e_SCIENTIFIC>::k_VALUE;

    if (last - first >= static_cast<ptrdiff_t>(k_BUFLEN)) {
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
    BSLS_ASSERT(first <= last);

    const size_t k_BUFLEN = ToCharsMaxLength<float, e_SCIENTIFIC>::k_VALUE;

    if (last - first >= static_cast<ptrdiff_t>(k_BUFLEN)) {
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

char *NumericFormatterUtil::toCharsImpl(char                   *first,
                                        char                   *last,
                                        unsigned long long int  value,
                                        int                     base)
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
