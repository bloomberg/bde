// bslalg_numericformatterutil.cpp                                    -*-C++-*-
#include <bslalg_numericformatterutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

///NOTICE
///------
//
// This file contains modified parts of Microsoft STL.
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md

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
// can safely ignore the special values as they are very short (0, -0, nan,
// -inf, inf).  Numbers that have integer and fractional digits both cannot
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

#include <bsla_nodiscard.h>

#include <bslmf_enableif.h>
#include <bslmf_floatingtypestructuraltraits.h>

#include <bsls_buildtarget.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <algorithm>
#include <functional>
#include <cctype>
#include <climits>
#include <cstddef>
#include <cstring>
#include <limits>

#include <bbryu/blp_ryu.h>

namespace {
namespace u {

using namespace BloombergLP;

            // DEFENSIVE CHECKS FOR 'Format' ENUMERATOR VALUES

/// Macro to ensure no collision between supported `base` values for
/// integral types and any of the `Format` enumerators for floating point
/// types.
#define U_FORMAT_AND_BASE_VALUE_DIFFERS(enumerator)                           \
    BSLMF_ASSERT(bslalg::NumericFormatterUtil::enumerator <  2                \
              || bslalg::NumericFormatterUtil::enumerator > 36)

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


static const char k_CHARCONV_DIGITS[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z'
};
BSLMF_ASSERT(sizeof(k_CHARCONV_DIGITS) == 36);


                  // INTEGRAL CONVERSION HELPER METHODS

/// Translate the specified `digit` which is a number in the range
/// `[ 0 .. 36 )` to an ASCII char (`0` + digit) for `digit < 10` and (`a` -
/// 10 + digit) for values higher than that.  The behavior is undefined
/// unless `digit < 36`.
inline
char digitToAscii(unsigned digit) BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT_SAFE(digit < 36);

    enum { k_CHAR_BIT_SHIFT = 3,
           k_CHAR_BIT       = 1 << k_CHAR_BIT_SHIFT,
           k_PATTERN        = (('a' - 10) << k_CHAR_BIT) | '0' };

    const char offset = static_cast<char>(
                             k_PATTERN >> ((10 <= digit) << k_CHAR_BIT_SHIFT));
    return static_cast<char>(offset + digit);
}

/// On success, render the specified `value`, in decimal ASCII form, to the
/// beginning of the memory specified by `[ first .. last )` and return the
/// address one past the lowest order digit written.  If the range
/// `[ first .. last )` is not large enough to contain the result, fail and
/// return 0 without rendering any digits.
inline
char *toCharsBase10Uint32(char           *first,
                          char           *last,
                          unsigned        value) BSLS_KEYWORD_NOEXCEPT
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

/// On success, render the specified `value`, in decimal ASCII form, to the
/// beginning of the memory specified by `[ first .. last )` and return the
/// address one past the lowest order digit written.  If the range
/// `[ first .. last )` is not large enough to contain the result, fail and
/// return 0 without rendering any digits.
inline
char *toCharsBase10Uint64(char          *first,
                          char          *last,
                          Uint64         value) BSLS_KEYWORD_NOEXCEPT
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

/// On success, render the specified `value` in the specified `base` to the
/// beginning of the buffer specified by `[ first .. last )` and return the
/// address one past the lowest order digit rendered.  If the specified
/// buffer is not large enough to accommodate the result, return 0 without
/// writing to the buffer.
inline
char *toCharsArbitraryBaseUint32(char     *first,
                                 char     *last,
                                 unsigned  value,
                                 unsigned  base) BSLS_KEYWORD_NOEXCEPT
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

/// On success, render the specified `value` in the specified `base` to the
/// beginning of the buffer specified by `[ first .. last )` and return the
/// address one past the lowest order digit rendered.  If the specified
/// buffer is not large enough to accommodate the result, return 0 without
/// writing to the buffer.
inline
char *toCharsArbitraryBaseUint64(char     *first,
                                 char     *last,
                                 Uint64    value,
                                 unsigned  base) BSLS_KEYWORD_NOEXCEPT
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

/// On success, render the specified `value` in the specified `base`, which
/// is known to be a power of 2, to the beginning of the buffer specified by
/// `[ first .. last )` and return one past the address of the lowest order
/// digit rendered.  Specify `shift` which is the log in base 2 of `base`.
/// If the buffer is not large enough to accommodate the result, return 0
/// without writing to the buffer.  The behavior is undefined unless `shift`
/// is in the range `[ 1 .. 5 ]`, unless `base == (1 << shift)`.
///
/// Note that we don't have a separate 32-bit version of this function,
/// because there are no divides or mods, and masks and shifts are very
/// fast, even on a 64 bit value.
inline
char *toCharsPowerOf2Base(char          *first,
                          char          *last,
                          Uint64         value,
                          unsigned       base,
                          unsigned char  shift) BSLS_KEYWORD_NOEXCEPT
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

                   // Special Values Conversion Function

/// Handles NaN and infinity values.  In case the specified `fpValue` is a
/// "normal" numeric value return `false`.  Otherwise, if `fpValue` is an
/// infinity or NaN value attempt to write their textual representations
/// ("inf", "-inf", "nan", or "-nan") to the specified `[first, last)` and
/// return `true`.  If there is not enough space in the buffer to write the
/// result set the `result` output pointer value to 0.  If the writing was
/// completely successful set the `result` pointer value to the character one
/// past the last character written.  Note that it is unspecified if the output
/// buffer is modified in case of a failure due to it being too short.
template <class t_FLOATING>
BSLA_NODISCARD
bool floatingHandleNanInf(char *       first,
                          char * const last,
                          t_FLOATING   fpValue,
                          char **      result)
{
    typedef bslmf::FloatingTypeStructuralTraits<t_FLOATING> Traits;
    typedef typename Traits::UintType                       UintType;

    const UintType uintValue = Traits::toUintType(fpValue);
    const UintType ieeeMantissa = uintValue & Traits::k_DENORM_MANTISSA_MASK;
    const int32_t  ieeeExponent = static_cast<int32_t>(
                                      uintValue >> Traits::k_EXPONENT_SHIFT)
                                                     & Traits::k_EXPONENT_MASK;

    // NaN - exponent all 1 bits, mantissa not 0
    // inf - exponent all 1 bits, mantissa is 0

    if (ieeeExponent != Traits::k_EXPONENT_MASK) {
        // Not a special value
        return false;                                                 // RETURN
    }

    const bool negative = (uintValue & Traits::k_SHIFTED_SIGN_MASK) != 0;
    if (last - first < 3 + negative) {
        // Too small buffer, the result [-]inf/nan won't fit
        *result = 0;
        return true;                                                  // RETURN
    }

    if (negative) {
        *first++ = '-';
    }

    std::memcpy(first, ieeeMantissa ? "nan" : "inf", 3);

    *result = first + 3;

    return true;
}

//=============================================================================
// Adapted Microsoft open source STL `to_chars` code                      BEGIN
//
// Code in this region is covered by its original license and copyrighted by
// its original authors:
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md

                     // Hex-float Conversion Functions

// The function below is modified source code from Microsoft STL
//
// Original source:
// https://github.com/microsoft/STL/blob/vs-2022-17.11/stl/inc/charconv
// Function `_Floating_to_chars_hex_precision`
//
// Original notices:
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md
template <class t_FLOATING>
BSLA_NODISCARD
char *toCharsHexPrecisionImp(char *       first,
                             char * const last,
                             t_FLOATING   fpValue,
                             int          precision) BSLS_KEYWORD_NOEXCEPT
{
    typedef bslmf::FloatingTypeStructuralTraits<t_FLOATING> Traits;
    typedef typename Traits::UintType UintType;

    UintType uintValue = Traits::toUintType(fpValue);
    if (uintValue & Traits::k_SHIFTED_SIGN_MASK) { // Negative value
        *first++ = '-';
        uintValue &= ~Traits::k_SHIFTED_SIGN_MASK; // make it positive
    }

    // * Determine the effective precision.
    // * Later, we'll decrement precision when printing each hex digit after
    //   the decimal point.

    const int fullPrecision        = Traits::k_MANTISSA_HEX_DIGITS;
    const int AdjustedExplicitBits = fullPrecision * 4;

    if (precision < 0) {
        // C11 7.21.6.1 "The fprintf function"/5: "A negative precision
        // argument is taken as if the precision were omitted." /8: "if the
        // precision is missing and FLT_RADIX is a power of 2, then the
        // precision is sufficient for an exact representation of the value"
        precision = fullPrecision;
    }

    // * Extract the ieeeMantissa and ieeeExponent.
    typedef typename Traits::UintType UintType;

    const UintType ieeeMantissa = uintValue & Traits::k_DENORM_MANTISSA_MASK;
    const int32_t  ieeeExponent = static_cast<int32_t>(
                                         uintValue >> Traits::k_EXPONENT_SHIFT);

    // Prepare the adjustedMantissa. This is aligned to hex digit boundaries,
    // with the implicit bit restored (0 for zero values and subnormal values,
    // 1 for normal values).
    UintType adjustedMantissa = Traits::adjustMantissaForHexDigits(
                                                                 ieeeMantissa);


    // Calculate the unbiasedExponent and further adjust the mantissa if
    // necessary. This unifies the processing of zero, subnormal, and normal
    // values.
    int32_t unbiasedExponent;

    if (ieeeExponent == 0) { // zero or subnormal
        // implicit bit is 0

        if (ieeeMantissa == 0) { // zero
            // C11 7.21.6.1 "The fprintf function"/8:
            // "If the value is zero, the exponent is zero."
            unbiasedExponent = 0;
        } else { // subnormal
            unbiasedExponent = 1 - Traits::k_EXPONENT_BIAS;
        }
    } else { // normal   // implicit bit is 1, hence `UintType(1)`
        adjustedMantissa |= UintType(1) << AdjustedExplicitBits;
        unbiasedExponent = ieeeExponent - Traits::k_EXPONENT_BIAS;
    }

    // `unbiasedExponent` is within
    //    * [-126, 127] for `float`
    //    * [-1022, 1023] for `double`

    // Decompose unbiasedExponent into signChar and absExponent.
    char     signChar;
    uint32_t absExponent;

    if (unbiasedExponent < 0) {
        signChar    = '-';
        absExponent = static_cast<uint32_t>(-unbiasedExponent);
    } else {
        signChar    = '+';
        absExponent = static_cast<uint32_t>(unbiasedExponent);
    }

    // `absExponent` is within
    //    * [0, 127] for float
    //    * [0, 1023] for double

    // Perform a single bounds check.
    {
        int32_t exponentLength;

        if (absExponent < 10) {
            exponentLength = 1;
        } else if (absExponent < 100) {
            exponentLength = 2;
        } else if (absExponent < 1000) {
            exponentLength = 3;
        } else {
            exponentLength = 4;
        }

        // precision might be enormous; avoid integer overflow by testing it
        // separately.
        ptrdiff_t bufferSize = last - first;

        if (bufferSize < precision) {
            return 0;                                                 // RETURN
        }

        bufferSize -= precision;

        const int32_t lengthExcludingPresision =
            1 // leading hex digit
            + static_cast<int32_t>(precision > 0) // possible decimal point
                                                  // excluding `+ precision`,
                                                  // hex digits after decimal
                                                  // point
            + 2 // "p+" or "p-"
            + exponentLength; // exponent

        if (bufferSize < lengthExcludingPresision) {
            return 0;                                                 // RETURN
        }
    }

    // * Perform rounding when we've been asked to omit hex digits.
    if (precision < fullPrecision) {
        // `precision` is within
        //    * [0, 5] for float
        //    * [0, 12] for double

        // `droppedBits` is within
        //    * [4, 24] for float
        //    * [4, 52] for double
        const int droppedBits = (fullPrecision - precision) * 4;

        // Perform rounding by adding an appropriately-shifted bit.

        // This can propagate carries all the way into the leading hex digit.
        // Examples:
        //    * "0.ff9" rounded to a precision of 2 is "1.00".
        //    * "1.ff9" rounded to a precision of 2 is "2.00".

        // Note that the leading hex digit participates in the rounding
        // decision. Examples:
        //    * "0.8" rounded to a precision of 0 is "0".
        //    * "1.8" rounded to a precision of 0 is "2".

        // Reference implementation with suboptimal codegen:
        // const bool lsbBit      =
        //              (adjustedMantissa & (UintType(1) << droppedBits)) != 0;
        // const bool roundBit    =
            //    (adjustedMantissa & (UintType(1) << (droppedBits - 1))) != 0;
        // const bool hasTailBits =
        //     (adjustedMantissa & ((UintType(1) << (droppedBits - 1)) - 1))
        //                                                                != 0;
        // const bool shouldRound = shouldRoundUp(lsbBit,
        //                                        roundBit,
        //                                        hasTailBits);
        // adjustedMantissa += UintType(shouldRound) << droppedBits;

        // Example for optimized implementation: Let droppedBits be 8.
        //        Bit index: ...[8]76543210
        // adjustedMantissa: ...[L]RTTTTTTT (not depicting known details,
        //                                   like hex digit alignment)
        // By focusing on the bit at index droppedBits, we can avoid
        // unnecessary branching and shifting.

        // Bit index: ...[8]76543210
        //  lsbBit: ...[L]RTTTTTTT
        const UintType lsbBit = adjustedMantissa;

        //  Bit index: ...9[8]76543210
        // roundBit: ...L[R]TTTTTTT0
        const UintType roundBit = adjustedMantissa << 1;

        // We can detect (without branching) whether any of the trailing bits
        // are set.  Due to shouldRound below, this computation will be used if
        // and only if R is 1, so we can assume that here.
        //   Bit index: ...9[8]76543210
        //    roundBit: ...L[1]TTTTTTT0
        // hasTailBits: ....[H]........

        // * If all of the trailing bits T are 0, then `roundBit - 1` will
        //   produce 0 for H (due to R being 1).
        // * If any of the trailing bits T are 1, then `roundBit - 1` will
        //   produce 1 for H (due to R being 1).
        const UintType hasTailBits = roundBit - 1;

        // Finally, we can use shouldRoundUp() logic with bitwise-AND and
        // bitwise-OR, selecting just the bit at index droppedBits. This is the
        // appropriately-shifted bit that we want.
        const UintType shouldRound = roundBit & (hasTailBits | lsbBit)
                                                & (UintType(1) << droppedBits);

        // This rounding technique is dedicated to the memory of
        // Peppermint. =^..^=
        adjustedMantissa += shouldRound;
    }

    // Print the leading hex digit, then mask it away.
    {
        const uint32_t nibble =
               static_cast<uint32_t>(adjustedMantissa >> AdjustedExplicitBits);
        BSLS_ASSERT_SAFE(nibble < 3);
        const char leadingHexDigit = static_cast<char>('0' + nibble);

        *first++ = leadingHexDigit;

        const UintType k_Mask = (UintType(1) << AdjustedExplicitBits) - 1;
        adjustedMantissa &= k_Mask;
    }

    // Print the decimal point and trailing hex digits.

    // C11 7.21.6.1 "The fprintf function"/8:
    //    "if the precision is zero and the # flag is not specified, no
    //     decimal-point character appears."
    if (precision > 0) {
        *first++ = '.';

        // 24 for float, 52 for double
        int32_t numOfBitsRemaining = AdjustedExplicitBits;

        for (;;) {
            BSLS_ASSERT_SAFE(numOfBitsRemaining >= 4);
            BSLS_ASSERT_SAFE(numOfBitsRemaining % 4 == 0);
            numOfBitsRemaining -= 4;

            const uint32_t nibble =
                 static_cast<uint32_t>(adjustedMantissa >> numOfBitsRemaining);
            BSLS_ASSERT_SAFE(nibble < 16);
            const char hexDigit = k_CHARCONV_DIGITS[nibble];

            *first++ = hexDigit;

            // precision is the number of hex digits that still need to print
            --precision;
            if (precision == 0) {
                break; // We're completely done with this phase.       // BREAK
            }
            // Otherwise, we need to keep printing hex digits

            if (numOfBitsRemaining == 0) {
                // We've finished printing `adjustedMantissa`, so all remaining
                // hex digits are '0'.
                std::memset(first, '0', static_cast<size_t>(precision));
                first += precision;
                break;                                                 // BREAK
            }

            // Mask away the hex digit that we just printed, then keep looping.
            // (We skip this when breaking out of the loop above, because
            //  `adjustedMantissa` isn't used later.)
            const UintType k_Mask = (UintType(1) << numOfBitsRemaining) - 1;
            adjustedMantissa &= k_Mask;
        }
    }

    // * Print the exponent.

    // C11 7.21.6.1 "The fprintf function"/8: "The exponent always contains at
    //               least one digit, and only as many more digits as necessary
    //               to represent the decimal exponent of 2."

    // Performance note: We should take advantage of the known ranges of
    //                   possible exponents.

    *first++ = 'p';
    *first++ = signChar;

    // We've already printed '-' if necessary, so uint32_t `absExponent` avoids
    // testing that again.
    return bslalg::NumericFormatterUtil::toChars(first, last, absExponent);
}

// The functions below are modified source code from Microsoft STL
//
// Original source:
// https://github.com/microsoft/STL/blob/vs-2022-17.11/stl/inc/charconv
// Function `_Floating_to_chars_hex_shortest`
//
// Original notices:
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md
template <class t_FLOATING>
BSLA_NODISCARD
char *floatingToCharsHexShortestImp(char       *first,
                                    char       *last,
                                    t_FLOATING  fpValue) BSLS_KEYWORD_NOEXCEPT
{

    // This prints "1.728p+0" instead of "2.e5p-1".
    // This prints "0.000002p-126" instead of "1p-149" for float.
    // This prints "0.0000000000001p-1022" instead of "1p-1074" for double.
    // This prioritizes being consistent with printf's de facto behavior (and
    // hex-precision's behavior) over minimizing the number of characters
    // printed.

    typedef bslmf::FloatingTypeStructuralTraits<t_FLOATING> Traits;
    typedef typename Traits::UintType UintType;

    UintType uintValue;
    std::memcpy(&uintValue, &fpValue, sizeof uintValue);

    typedef bslalg::NumericFormatterUtil FmtUtil;
    char buff[FmtUtil::ToCharsMaxLength<t_FLOATING, FmtUtil::e_HEX>::k_VALUE];
    char *savedFirst = first;
    last = buff + (last - first);
    first = buff;

    if (uintValue & Traits::k_SHIFTED_SIGN_MASK) { // Negative value
        *first++ = '-';
        uintValue &= ~Traits::k_SHIFTED_SIGN_MASK; // make it positive
    }

    if (uintValue == 0) { // zero detected; write "0p+0" and return
        // C11 7.21.6.1 "The fprintf function"/8:
        //                        "If the value is zero, the exponent is zero."
        // Special-casing zero is necessary because of the exponent.
        const char * const str    = "0p+0";
        const size_t       length = 4;

        if (last - first < static_cast<ptrdiff_t>(length)) {
            return 0;                                                 // RETURN
        }

        // in case of success copy the characters from the local buffer
        if (first != buff) {  // If there is a sign write it
            *savedFirst++ = buff[0];
        }

        std::memcpy(savedFirst, str, length);

        return savedFirst + length;
    }

    const UintType ieeeMantissa = uintValue & Traits::k_DENORM_MANTISSA_MASK;
    const int32_t  ieeeExponent  =
                   static_cast<int32_t>(uintValue >> Traits::k_EXPONENT_SHIFT);

    char    leadingHexDigit; // implicit bit
    int32_t unbiasedExponent;

    if (ieeeExponent == 0) { // subnormal
        leadingHexDigit  = '0';
        unbiasedExponent = 1 - Traits::k_EXPONENT_BIAS;
    } else { // normal
        leadingHexDigit  = '1';
        unbiasedExponent = ieeeExponent - Traits::k_EXPONENT_BIAS;
    }

    // Performance note: Consider avoiding per-character bounds checking when
    //                   there's plenty of space.

    if (first == last) {
        return 0;                                                     // RETURN
    }

    *first++ = leadingHexDigit;

    if (ieeeMantissa == 0) {
        // The fraction bits are all 0. Trim them away, including the decimal
        // point.
    } else {
        if (first == last) {
            return 0;                                                 // RETURN
        }

        *first++ = '.';

        // The hex digits after the decimal point correspond to the explicitly
        // stored fraction bits.
        // * float explicitly stores 23 fraction bits. 23 / 4 == 5.75, so we'll
        //   print at most 6 hex digits.
        // * double explicitly stores 52 fraction bits. 52 / 4 == 13, so we'll
        //   print at most 13 hex digits.
        UintType adjustedMantissa =  Traits::adjustMantissaForHexDigits(
                                                                 ieeeMantissa);
        int32_t numOfBitsRemaining = Traits::k_MANTISSA_HEX_DIGITS * 4;

        // do-while: The condition `adjustedMantissa != 0` is initially true -
        // we have nonzero fraction bits and we've printed the decimal point.
        // Each iteration, we print a hex digit, mask it away, and keep looping
        // if we still have nonzero fraction bits.  If there would be trailing
        // '0' hex digits, this trims them.  If there wouldn't be trailing '0'
        // hex digits, the same condition works (as we print the final hex
        // digit and mask it away); we don't need to test `numOfBitsRemaining`.
        do {
            BSLS_ASSERT_SAFE(numOfBitsRemaining >= 4);
            BSLS_ASSERT_SAFE(numOfBitsRemaining % 4 == 0);
            numOfBitsRemaining -= 4;

            const uint32_t nibble =
                 static_cast<uint32_t>(adjustedMantissa >> numOfBitsRemaining);
            BSLS_ASSERT_SAFE(nibble < 16);
            const char hexDigit = k_CHARCONV_DIGITS[nibble];

            if (first == last) {
                return 0;                                             // RETURN
            }

            *first++ = hexDigit;

            const UintType k_Mask = (UintType(1) << numOfBitsRemaining) - 1;
            adjustedMantissa &= k_Mask;

        } while (adjustedMantissa != 0);
    }

    // C11 7.21.6.1 "The fprintf function"/8:
    // "The exponent always contains at least one digit, and only as many more
    //  digits as necessary to represent the decimal exponent of 2."

    // Performance note: We should take advantage of the known ranges of
    //                   possible exponents.

    // float: unbiasedExponent is within [-126, 127].
    // double: unbiasedExponent is within [-1022, 1023].

    if (last - first < 3) {
        return 0;                                                     // RETURN
    }

    *first++ = 'p';

    if (unbiasedExponent < 0) {
        *first++          = '-';
        unbiasedExponent = -unbiasedExponent;
    } else {
        *first++ = '+';
    }

    // We've already printed '-' if necessary, so static_cast<uint32_t> avoids
    // testing that again.
    char *rv = bslalg::NumericFormatterUtil::toChars(
                                      first,
                                      last,
                                      static_cast<uint32_t>(unbiasedExponent));
    if (!rv) {
        return 0;                                                     // RETURN
    }

    // in case of success copy the characters from the local buffer
    const size_t resultLength = rv - buff;
    std::memcpy(savedFirst, buff, resultLength);

    return savedFirst + resultLength;
}

               // Floating-Point General Notation w/o Precision

template <class t_FLOATING>
BSLA_NODISCARD
char *floatingToCharsGeneralImp(char       *first,
                                char       *last,
                                t_FLOATING  fpValue) BSLS_KEYWORD_NOEXCEPT
{
    typedef bslalg::NumericFormatterUtil Util;
    char b[Util::ToCharsMaxLength<t_FLOATING, Util::e_SCIENTIFIC>::k_VALUE+1];

    // Get the shortest conversion
    char *rv = Util::toChars(b, b + sizeof b, fpValue, Util::e_SCIENTIFIC);
    if (!rv) {
        return 0;                                                     // RETURN
    }

    // Null terminate the buffer for the C hackery below
    *rv = 0;

    // Get the scientific exponent
    char* p = strrchr(b, 'e');
    if (!p) {
        return 0;                                                     // RETURN
    }

    ++p; // Skip the e
    const int scientificExponent = atoi(p);

    // C11 7.21.6.1 "The fprintf function"/8:
    // "Let P equal [...] 6 if the precision is omitted [...].
    // Then, if a conversion with style E would have an exponent of X:
    // - if P > X >= -4, the conversion is with style f [...].
    // - otherwise, the conversion is with style e [...]."
    if (6 <= scientificExponent || -4 > scientificExponent) {
        // We need to return the scientific notation

        ptrdiff_t len = rv - b;
        if (last - first < len) {  // Won't fit
            return 0;                                                 // RETURN
        }
        memcpy(first, b, len);
        return first + len;
    }

    return Util::toChars(first, last, fpValue, Util::e_FIXED);
}

                      // PRECISION RELATED DATA TABLES

// The data tables below are modified source code from Microsoft STL
//
// Original source:
// https://github.com/microsoft/STL/blob/vs-2022-17.11/stl/inc/xcharconv_tables.h
// Function `_Floating_to_chars_hex_precision`
//
// Original notices:
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md

// For general precision, we can use lookup tables to avoid performing trial
// formatting.

// For a simple example, imagine counting the number of digits D in an integer,
// and needing to know whether D is less than 3, equal to 3/4/5/6, or greater
// than 6. We could use a lookup table:
// D | Largest integer with D digits
// 2 |      99
// 3 |     999
// 4 |   9'999
// 5 |  99'999
// 6 | 999'999
// 7 | table end
//
// Looking up an integer in this table with `lower_bound()` will work:
// * Too-small integers, like 7, 70, and 99, will cause `lower_bound()` to
//   return the D == 2 row. (If all we care about is whether D is less than 3,
//   then it's okay to smash the D == 1 and D == 2 cases together.)
// * Integers in [100, 999] will cause `lower_bound()` to return the D == 3
//   row, and so forth.
// * Too-large integers, like 1'000'000 and above, will cause `lower_bound()`
//   to return the end of the table. If we compute D from that index, this will
//   be considered D == 7, which will activate any "greater than 6" logic.

// Floating-point is slightly more complicated.

// The ordinary lookup tables are for X within [-5, 38] for `float`, and
// [-5, 308] for `double`.  (-5 absorbs too-negative exponents, outside the
// P > X >= -4 criterion. 38 and 308 are the maximum exponents.)  Due to the
// P > X condition, we can use a subset of the table for X within [-5, P - 1],
// suitably clamped.

// When P is small, rounding can affect X. For example:
// For P == 1, the largest `double` with X == 0 is:
//                          9.4999999999999982236431605997495353221893310546875
// For P == 2, the largest `double` with X == 0 is:
//                           9.949999999999999289457264239899814128875732421875
// For P == 3, the largest `double` with X == 0 is:
//                          9.9949999999999992184029906638897955417633056640625

// Exponent adjustment is a concern for P within [1, 7] for `float`, and
// [1, 15] for `double` (determined via brute force).  While larger values of P
// still perform rounding, they can't trigger exponent adjustment.  This is
// because only values with repeated '9' digits can undergo exponent adjustment
// during rounding, and floating-point granularity limits the number of
// consecutive '9' digits that can appear.

// So, we need special lookup tables for small values of P.
// These tables have varying lengths due to the P > X >= -4 criterion.
// For example:
// * For P == 1, need table entries for X: -5, -4, -3, -2, -1, 0
// * For P == 2, need table entries for X: -5, -4, -3, -2, -1, 0, 1
// * For P == 3, need table entries for X: -5, -4, -3, -2, -1, 0, 1, 2
// * For P == 4, need table entries for X: -5, -4, -3, -2, -1, 0, 1, 2, 3

// We can concatenate these tables for compact storage, using triangular
// numbers to access them.  The table for P begins at index
// (P - 1) * (P + 10) / 2 with length P + 5.

// For both the ordinary and special lookup tables, after an index I is
// returned by `lower_bound()`, X is I - 5.

// We need to special-case the floating-point value 0.0, which is considered to
// have X == 0.  Otherwise, the lookup tables would consider it to have a
// highly negative X.

// Finally, because we're working with positive floating-point values,
// representation comparisons behave identically to floating-point comparisons.

// The generator is in /tools/scripts/charconv_tables_generate.cpp

template <class t_Floating>
struct GeneralPrecisionTables;

template <>
struct GeneralPrecisionTables<float> {
    static const int k_MaxSpecialP = 7;

    static const uint32_t specialXtable[63];

    static const int k_MaxP = 39;

    static const uint32_t ordinaryXtable[44];
};

const int GeneralPrecisionTables<float>::k_MaxSpecialP;
const int GeneralPrecisionTables<float>::k_MaxP;

template <>
struct GeneralPrecisionTables<double> {
    static const int k_MaxSpecialP = 15;

    static const uint64_t specialXtable[195];

    static const int k_MaxP = 309;

    static const uint64_t ordinaryXtable[314];
};

const int GeneralPrecisionTables<double>::k_MaxSpecialP;
const int GeneralPrecisionTables<double>::k_MaxP;

// The data tables below are modified source code from Microsoft STL
//
// Original source:
// https://github.com/microsoft/STL/blob/vs-2022-17.11/stl/src/xcharconv_tables_double.cpp
//
// Original notices:
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md
const uint64_t GeneralPrecisionTables<double>::specialXtable[195] = {
    0x3F18E757928E0C9Du, 0x3F4F212D77318FC5u, 0x3F8374BC6A7EF9DBu,
    0x3FB851EB851EB851u, 0x3FEE666666666666u, 0x4022FFFFFFFFFFFFu,
    0x3F1A1554FBDAD751u, 0x3F504D551D68C692u, 0x3F8460AA64C2F837u,
    0x3FB978D4FDF3B645u, 0x3FEFD70A3D70A3D7u, 0x4023E66666666666u,
    0x4058DFFFFFFFFFFFu, 0x3F1A3387ECC8EB96u, 0x3F506034F3FD933Eu,
    0x3F84784230FCF80Du, 0x3FB99652BD3C3611u, 0x3FEFFBE76C8B4395u,
    0x4023FD70A3D70A3Du, 0x4058FCCCCCCCCCCCu, 0x408F3BFFFFFFFFFFu,
    0x3F1A368D04E0BA6Au, 0x3F506218230C7482u, 0x3F847A9E2BCF91A3u,
    0x3FB99945B6C3760Bu, 0x3FEFFF972474538Eu, 0x4023FFBE76C8B439u,
    0x4058FFAE147AE147u, 0x408F3F9999999999u, 0x40C387BFFFFFFFFFu,
    0x3F1A36DA54164F19u, 0x3F506248748DF16Fu, 0x3F847ADA91B16DCBu,
    0x3FB99991361DC93Eu, 0x3FEFFFF583A53B8Eu, 0x4023FFF972474538u,
    0x4058FFF7CED91687u, 0x408F3FF5C28F5C28u, 0x40C387F999999999u,
    0x40F869F7FFFFFFFFu, 0x3F1A36E20F35445Du, 0x3F50624D49814ABAu,
    0x3F847AE09BE19D69u, 0x3FB99998C2DA04C3u, 0x3FEFFFFEF39085F4u,
    0x4023FFFF583A53B8u, 0x4058FFFF2E48E8A7u, 0x408F3FFEF9DB22D0u,
    0x40C387FF5C28F5C2u, 0x40F869FF33333333u, 0x412E847EFFFFFFFFu,
    0x3F1A36E2D51EC34Bu, 0x3F50624DC5333A0Eu, 0x3F847AE136800892u,
    0x3FB9999984200AB7u, 0x3FEFFFFFE5280D65u, 0x4023FFFFEF39085Fu,
    0x4058FFFFEB074A77u, 0x408F3FFFE5C91D14u, 0x40C387FFEF9DB22Du,
    0x40F869FFEB851EB8u, 0x412E847FE6666666u, 0x416312CFEFFFFFFFu,
    0x3F1A36E2E8E94FFCu, 0x3F50624DD191D1FDu, 0x3F847AE145F6467Du,
    0x3FB999999773D81Cu, 0x3FEFFFFFFD50CE23u, 0x4023FFFFFE5280D6u,
    0x4058FFFFFDE7210Bu, 0x408F3FFFFD60E94Eu, 0x40C387FFFE5C91D1u,
    0x40F869FFFDF3B645u, 0x412E847FFD70A3D7u, 0x416312CFFE666666u,
    0x4197D783FDFFFFFFu, 0x3F1A36E2EAE3F7A7u, 0x3F50624DD2CE7AC8u,
    0x3F847AE14782197Bu, 0x3FB9999999629FD9u, 0x3FEFFFFFFFBB47D0u,
    0x4023FFFFFFD50CE2u, 0x4058FFFFFFCA501Au, 0x408F3FFFFFBCE421u,
    0x40C387FFFFD60E94u, 0x40F869FFFFCB923Au, 0x412E847FFFBE76C8u,
    0x416312CFFFD70A3Du, 0x4197D783FFCCCCCCu, 0x41CDCD64FFBFFFFFu,
    0x3F1A36E2EB16A205u, 0x3F50624DD2EE2543u, 0x3F847AE147A9AE94u,
    0x3FB9999999941A39u, 0x3FEFFFFFFFF920C8u, 0x4023FFFFFFFBB47Du,
    0x4058FFFFFFFAA19Cu, 0x408F3FFFFFF94A03u, 0x40C387FFFFFBCE42u,
    0x40F869FFFFFAC1D2u, 0x412E847FFFF97247u, 0x416312CFFFFBE76Cu,
    0x4197D783FFFAE147u, 0x41CDCD64FFF99999u, 0x4202A05F1FFBFFFFu,
    0x3F1A36E2EB1BB30Fu, 0x3F50624DD2F14FE9u, 0x3F847AE147ADA3E3u,
    0x3FB9999999990CDCu, 0x3FEFFFFFFFFF5014u, 0x4023FFFFFFFF920Cu,
    0x4058FFFFFFFF768Fu, 0x408F3FFFFFFF5433u, 0x40C387FFFFFF94A0u,
    0x40F869FFFFFF79C8u, 0x412E847FFFFF583Au, 0x416312CFFFFF9724u,
    0x4197D783FFFF7CEDu, 0x41CDCD64FFFF5C28u, 0x4202A05F1FFF9999u,
    0x42374876E7FF7FFFu, 0x3F1A36E2EB1C34C3u, 0x3F50624DD2F1A0FAu,
    0x3F847AE147AE0938u, 0x3FB9999999998B86u, 0x3FEFFFFFFFFFEE68u,
    0x4023FFFFFFFFF501u, 0x4058FFFFFFFFF241u, 0x408F3FFFFFFFEED1u,
    0x40C387FFFFFFF543u, 0x40F869FFFFFFF294u, 0x412E847FFFFFEF39u,
    0x416312CFFFFFF583u, 0x4197D783FFFFF2E4u, 0x41CDCD64FFFFEF9Du,
    0x4202A05F1FFFF5C2u, 0x42374876E7FFF333u, 0x426D1A94A1FFEFFFu,
    0x3F1A36E2EB1C41BBu, 0x3F50624DD2F1A915u, 0x3F847AE147AE135Au,
    0x3FB9999999999831u, 0x3FEFFFFFFFFFFE3Du, 0x4023FFFFFFFFFEE6u,
    0x4058FFFFFFFFFEA0u, 0x408F3FFFFFFFFE48u, 0x40C387FFFFFFFEEDu,
    0x40F869FFFFFFFEA8u, 0x412E847FFFFFFE52u, 0x416312CFFFFFFEF3u,
    0x4197D783FFFFFEB0u, 0x41CDCD64FFFFFE5Cu, 0x4202A05F1FFFFEF9u,
    0x42374876E7FFFEB8u, 0x426D1A94A1FFFE66u, 0x42A2309CE53FFEFFu,
    0x3F1A36E2EB1C4307u, 0x3F50624DD2F1A9E4u, 0x3F847AE147AE145Eu,
    0x3FB9999999999975u, 0x3FEFFFFFFFFFFFD2u, 0x4023FFFFFFFFFFE3u,
    0x4058FFFFFFFFFFDCu, 0x408F3FFFFFFFFFD4u, 0x40C387FFFFFFFFE4u,
    0x40F869FFFFFFFFDDu, 0x412E847FFFFFFFD5u, 0x416312CFFFFFFFE5u,
    0x4197D783FFFFFFDEu, 0x41CDCD64FFFFFFD6u, 0x4202A05F1FFFFFE5u,
    0x42374876E7FFFFDFu, 0x426D1A94A1FFFFD7u, 0x42A2309CE53FFFE6u,
    0x42D6BCC41E8FFFDFu, 0x3F1A36E2EB1C4328u, 0x3F50624DD2F1A9F9u,
    0x3F847AE147AE1477u, 0x3FB9999999999995u, 0x3FEFFFFFFFFFFFFBu,
    0x4023FFFFFFFFFFFDu, 0x4058FFFFFFFFFFFCu, 0x408F3FFFFFFFFFFBu,
    0x40C387FFFFFFFFFDu, 0x40F869FFFFFFFFFCu, 0x412E847FFFFFFFFBu,
    0x416312CFFFFFFFFDu, 0x4197D783FFFFFFFCu, 0x41CDCD64FFFFFFFBu,
    0x4202A05F1FFFFFFDu, 0x42374876E7FFFFFCu, 0x426D1A94A1FFFFFBu,
    0x42A2309CE53FFFFDu, 0x42D6BCC41E8FFFFCu, 0x430C6BF52633FFFBu
};

// double
const uint64_t GeneralPrecisionTables<double>::ordinaryXtable[314] = {
    0x3F1A36E2EB1C432Cu, 0x3F50624DD2F1A9FBu, 0x3F847AE147AE147Au,
    0x3FB9999999999999u, 0x3FEFFFFFFFFFFFFFu, 0x4023FFFFFFFFFFFFu,
    0x4058FFFFFFFFFFFFu, 0x408F3FFFFFFFFFFFu, 0x40C387FFFFFFFFFFu,
    0x40F869FFFFFFFFFFu, 0x412E847FFFFFFFFFu, 0x416312CFFFFFFFFFu,
    0x4197D783FFFFFFFFu, 0x41CDCD64FFFFFFFFu, 0x4202A05F1FFFFFFFu,
    0x42374876E7FFFFFFu, 0x426D1A94A1FFFFFFu, 0x42A2309CE53FFFFFu,
    0x42D6BCC41E8FFFFFu, 0x430C6BF52633FFFFu, 0x4341C37937E07FFFu,
    0x4376345785D89FFFu, 0x43ABC16D674EC7FFu, 0x43E158E460913CFFu,
    0x4415AF1D78B58C3Fu, 0x444B1AE4D6E2EF4Fu, 0x4480F0CF064DD591u,
    0x44B52D02C7E14AF6u, 0x44EA784379D99DB4u, 0x45208B2A2C280290u,
    0x4554ADF4B7320334u, 0x4589D971E4FE8401u, 0x45C027E72F1F1281u,
    0x45F431E0FAE6D721u, 0x46293E5939A08CE9u, 0x465F8DEF8808B024u,
    0x4693B8B5B5056E16u, 0x46C8A6E32246C99Cu, 0x46FED09BEAD87C03u,
    0x4733426172C74D82u, 0x476812F9CF7920E2u, 0x479E17B84357691Bu,
    0x47D2CED32A16A1B1u, 0x48078287F49C4A1Du, 0x483D6329F1C35CA4u,
    0x48725DFA371A19E6u, 0x48A6F578C4E0A060u, 0x48DCB2D6F618C878u,
    0x4911EFC659CF7D4Bu, 0x49466BB7F0435C9Eu, 0x497C06A5EC5433C6u,
    0x49B18427B3B4A05Bu, 0x49E5E531A0A1C872u, 0x4A1B5E7E08CA3A8Fu,
    0x4A511B0EC57E6499u, 0x4A8561D276DDFDC0u, 0x4ABABA4714957D30u,
    0x4AF0B46C6CDD6E3Eu, 0x4B24E1878814C9CDu, 0x4B5A19E96A19FC40u,
    0x4B905031E2503DA8u, 0x4BC4643E5AE44D12u, 0x4BF97D4DF19D6057u,
    0x4C2FDCA16E04B86Du, 0x4C63E9E4E4C2F344u, 0x4C98E45E1DF3B015u,
    0x4CCF1D75A5709C1Au, 0x4D03726987666190u, 0x4D384F03E93FF9F4u,
    0x4D6E62C4E38FF872u, 0x4DA2FDBB0E39FB47u, 0x4DD7BD29D1C87A19u,
    0x4E0DAC74463A989Fu, 0x4E428BC8ABE49F63u, 0x4E772EBAD6DDC73Cu,
    0x4EACFA698C95390Bu, 0x4EE21C81F7DD43A7u, 0x4F16A3A275D49491u,
    0x4F4C4C8B1349B9B5u, 0x4F81AFD6EC0E1411u, 0x4FB61BCCA7119915u,
    0x4FEBA2BFD0D5FF5Bu, 0x502145B7E285BF98u, 0x50559725DB272F7Fu,
    0x508AFCEF51F0FB5Eu, 0x50C0DE1593369D1Bu, 0x50F5159AF8044462u,
    0x512A5B01B605557Au, 0x516078E111C3556Cu, 0x5194971956342AC7u,
    0x51C9BCDFABC13579u, 0x5200160BCB58C16Cu, 0x52341B8EBE2EF1C7u,
    0x526922726DBAAE39u, 0x529F6B0F092959C7u, 0x52D3A2E965B9D81Cu,
    0x53088BA3BF284E23u, 0x533EAE8CAEF261ACu, 0x53732D17ED577D0Bu,
    0x53A7F85DE8AD5C4Eu, 0x53DDF67562D8B362u, 0x5412BA095DC7701Du,
    0x5447688BB5394C25u, 0x547D42AEA2879F2Eu, 0x54B249AD2594C37Cu,
    0x54E6DC186EF9F45Cu, 0x551C931E8AB87173u, 0x5551DBF316B346E7u,
    0x558652EFDC6018A1u, 0x55BBE7ABD3781ECAu, 0x55F170CB642B133Eu,
    0x5625CCFE3D35D80Eu, 0x565B403DCC834E11u, 0x569108269FD210CBu,
    0x56C54A3047C694FDu, 0x56FA9CBC59B83A3Du, 0x5730A1F5B8132466u,
    0x5764CA732617ED7Fu, 0x5799FD0FEF9DE8DFu, 0x57D03E29F5C2B18Bu,
    0x58044DB473335DEEu, 0x583961219000356Au, 0x586FB969F40042C5u,
    0x58A3D3E2388029BBu, 0x58D8C8DAC6A0342Au, 0x590EFB1178484134u,
    0x59435CEAEB2D28C0u, 0x59783425A5F872F1u, 0x59AE412F0F768FADu,
    0x59E2E8BD69AA19CCu, 0x5A17A2ECC414A03Fu, 0x5A4D8BA7F519C84Fu,
    0x5A827748F9301D31u, 0x5AB7151B377C247Eu, 0x5AECDA62055B2D9Du,
    0x5B22087D4358FC82u, 0x5B568A9C942F3BA3u, 0x5B8C2D43B93B0A8Bu,
    0x5BC19C4A53C4E697u, 0x5BF6035CE8B6203Du, 0x5C2B843422E3A84Cu,
    0x5C6132A095CE492Fu, 0x5C957F48BB41DB7Bu, 0x5CCADF1AEA12525Au,
    0x5D00CB70D24B7378u, 0x5D34FE4D06DE5056u, 0x5D6A3DE04895E46Cu,
    0x5DA066AC2D5DAEC3u, 0x5DD4805738B51A74u, 0x5E09A06D06E26112u,
    0x5E400444244D7CABu, 0x5E7405552D60DBD6u, 0x5EA906AA78B912CBu,
    0x5EDF485516E7577Eu, 0x5F138D352E5096AFu, 0x5F48708279E4BC5Au,
    0x5F7E8CA3185DEB71u, 0x5FB317E5EF3AB327u, 0x5FE7DDDF6B095FF0u,
    0x601DD55745CBB7ECu, 0x6052A5568B9F52F4u, 0x60874EAC2E8727B1u,
    0x60BD22573A28F19Du, 0x60F2357684599702u, 0x6126C2D4256FFCC2u,
    0x615C73892ECBFBF3u, 0x6191C835BD3F7D78u, 0x61C63A432C8F5CD6u,
    0x61FBC8D3F7B3340Bu, 0x62315D847AD00087u, 0x6265B4E5998400A9u,
    0x629B221EFFE500D3u, 0x62D0F5535FEF2084u, 0x630532A837EAE8A5u,
    0x633A7F5245E5A2CEu, 0x63708F936BAF85C1u, 0x63A4B378469B6731u,
    0x63D9E056584240FDu, 0x64102C35F729689Eu, 0x6444374374F3C2C6u,
    0x647945145230B377u, 0x64AF965966BCE055u, 0x64E3BDF7E0360C35u,
    0x6518AD75D8438F43u, 0x654ED8D34E547313u, 0x6583478410F4C7ECu,
    0x65B819651531F9E7u, 0x65EE1FBE5A7E7861u, 0x6622D3D6F88F0B3Cu,
    0x665788CCB6B2CE0Cu, 0x668D6AFFE45F818Fu, 0x66C262DFEEBBB0F9u,
    0x66F6FB97EA6A9D37u, 0x672CBA7DE5054485u, 0x6761F48EAF234AD3u,
    0x679671B25AEC1D88u, 0x67CC0E1EF1A724EAu, 0x680188D357087712u,
    0x6835EB082CCA94D7u, 0x686B65CA37FD3A0Du, 0x68A11F9E62FE4448u,
    0x68D56785FBBDD55Au, 0x690AC1677AAD4AB0u, 0x6940B8E0ACAC4EAEu,
    0x6974E718D7D7625Au, 0x69AA20DF0DCD3AF0u, 0x69E0548B68A044D6u,
    0x6A1469AE42C8560Cu, 0x6A498419D37A6B8Fu, 0x6A7FE52048590672u,
    0x6AB3EF342D37A407u, 0x6AE8EB0138858D09u, 0x6B1F25C186A6F04Cu,
    0x6B537798F428562Fu, 0x6B88557F31326BBBu, 0x6BBE6ADEFD7F06AAu,
    0x6BF302CB5E6F642Au, 0x6C27C37E360B3D35u, 0x6C5DB45DC38E0C82u,
    0x6C9290BA9A38C7D1u, 0x6CC734E940C6F9C5u, 0x6CFD022390F8B837u,
    0x6D3221563A9B7322u, 0x6D66A9ABC9424FEBu, 0x6D9C5416BB92E3E6u,
    0x6DD1B48E353BCE6Fu, 0x6E0621B1C28AC20Bu, 0x6E3BAA1E332D728Eu,
    0x6E714A52DFFC6799u, 0x6EA59CE797FB817Fu, 0x6EDB04217DFA61DFu,
    0x6F10E294EEBC7D2Bu, 0x6F451B3A2A6B9C76u, 0x6F7A6208B5068394u,
    0x6FB07D457124123Cu, 0x6FE49C96CD6D16CBu, 0x7019C3BC80C85C7Eu,
    0x70501A55D07D39CFu, 0x708420EB449C8842u, 0x70B9292615C3AA53u,
    0x70EF736F9B3494E8u, 0x7123A825C100DD11u, 0x7158922F31411455u,
    0x718EB6BAFD91596Bu, 0x71C33234DE7AD7E2u, 0x71F7FEC216198DDBu,
    0x722DFE729B9FF152u, 0x7262BF07A143F6D3u, 0x72976EC98994F488u,
    0x72CD4A7BEBFA31AAu, 0x73024E8D737C5F0Au, 0x7336E230D05B76CDu,
    0x736C9ABD04725480u, 0x73A1E0B622C774D0u, 0x73D658E3AB795204u,
    0x740BEF1C9657A685u, 0x74417571DDF6C813u, 0x7475D2CE55747A18u,
    0x74AB4781EAD1989Eu, 0x74E10CB132C2FF63u, 0x75154FDD7F73BF3Bu,
    0x754AA3D4DF50AF0Au, 0x7580A6650B926D66u, 0x75B4CFFE4E7708C0u,
    0x75EA03FDE214CAF0u, 0x7620427EAD4CFED6u, 0x7654531E58A03E8Bu,
    0x768967E5EEC84E2Eu, 0x76BFC1DF6A7A61BAu, 0x76F3D92BA28C7D14u,
    0x7728CF768B2F9C59u, 0x775F03542DFB8370u, 0x779362149CBD3226u,
    0x77C83A99C3EC7EAFu, 0x77FE494034E79E5Bu, 0x7832EDC82110C2F9u,
    0x7867A93A2954F3B7u, 0x789D9388B3AA30A5u, 0x78D27C35704A5E67u,
    0x79071B42CC5CF601u, 0x793CE2137F743381u, 0x79720D4C2FA8A030u,
    0x79A6909F3B92C83Du, 0x79DC34C70A777A4Cu, 0x7A11A0FC668AAC6Fu,
    0x7A46093B802D578Bu, 0x7A7B8B8A6038AD6Eu, 0x7AB137367C236C65u,
    0x7AE585041B2C477Eu, 0x7B1AE64521F7595Eu, 0x7B50CFEB353A97DAu,
    0x7B8503E602893DD1u, 0x7BBA44DF832B8D45u, 0x7BF06B0BB1FB384Bu,
    0x7C2485CE9E7A065Eu, 0x7C59A742461887F6u, 0x7C9008896BCF54F9u,
    0x7CC40AABC6C32A38u, 0x7CF90D56B873F4C6u, 0x7D2F50AC6690F1F8u,
    0x7D63926BC01A973Bu, 0x7D987706B0213D09u, 0x7DCE94C85C298C4Cu,
    0x7E031CFD3999F7AFu, 0x7E37E43C8800759Bu, 0x7E6DDD4BAA009302u,
    0x7EA2AA4F4A405BE1u, 0x7ED754E31CD072D9u, 0x7F0D2A1BE4048F90u,
    0x7F423A516E82D9BAu, 0x7F76C8E5CA239028u, 0x7FAC7B1F3CAC7433u,
    0x7FE1CCF385EBC89Fu, 0x7FEFFFFFFFFFFFFFu
};

// The data tables below are modified source code from Microsoft STL
//
// Original source:
// https://github.com/microsoft/STL/blob/vs-2022-17.11/stl/src/xcharconv_tables_float.cpp
//
// Original notices:
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md
const uint32_t GeneralPrecisionTables<float>::specialXtable[63] = {
    0x38C73ABCu, 0x3A79096Bu, 0x3C1BA5E3u, 0x3DC28F5Cu, 0x3F733333u,
    0x4117FFFFu, 0x38D0AAA7u, 0x3A826AA8u, 0x3C230553u, 0x3DCBC6A7u,
    0x3F7EB851u, 0x411F3333u, 0x42C6FFFFu, 0x38D19C3Fu, 0x3A8301A7u,
    0x3C23C211u, 0x3DCCB295u, 0x3F7FDF3Bu, 0x411FEB85u, 0x42C7E666u,
    0x4479DFFFu, 0x38D1B468u, 0x3A8310C1u, 0x3C23D4F1u, 0x3DCCCA2Du,
    0x3F7FFCB9u, 0x411FFDF3u, 0x42C7FD70u, 0x4479FCCCu, 0x461C3DFFu,
    0x38D1B6D2u, 0x3A831243u, 0x3C23D6D4u, 0x3DCCCC89u, 0x3F7FFFACu,
    0x411FFFCBu, 0x42C7FFBEu, 0x4479FFAEu, 0x461C3FCCu, 0x47C34FBFu,
    0x38D1B710u, 0x3A83126Au, 0x3C23D704u, 0x3DCCCCC6u, 0x3F7FFFF7u,
    0x411FFFFAu, 0x42C7FFF9u, 0x4479FFF7u, 0x461C3FFAu, 0x47C34FF9u,
    0x497423F7u, 0x38D1B716u, 0x3A83126Eu, 0x3C23D709u, 0x3DCCCCCCu,
    0x3F7FFFFFu, 0x411FFFFFu, 0x42C7FFFFu, 0x4479FFFFu,
    0x461C3FFFu, 0x47C34FFFu, 0x497423FFu, 0x4B18967Fu
};

// float
const uint32_t GeneralPrecisionTables<float>::ordinaryXtable[44] = {
    0x38D1B717u, 0x3A83126Eu, 0x3C23D70Au, 0x3DCCCCCCu, 0x3F7FFFFFu,
    0x411FFFFFu, 0x42C7FFFFu, 0x4479FFFFu, 0x461C3FFFu, 0x47C34FFFu,
    0x497423FFu, 0x4B18967Fu, 0x4CBEBC1Fu, 0x4E6E6B27u, 0x501502F8u,
    0x51BA43B7u, 0x5368D4A5u, 0x551184E7u, 0x56B5E620u, 0x58635FA9u,
    0x5A0E1BC9u, 0x5BB1A2BCu, 0x5D5E0B6Bu, 0x5F0AC723u, 0x60AD78EBu,
    0x6258D726u, 0x64078678u, 0x65A96816u, 0x6753C21Bu, 0x69045951u,
    0x6AA56FA5u, 0x6C4ECB8Fu, 0x6E013F39u, 0x6FA18F07u, 0x7149F2C9u,
    0x72FC6F7Cu, 0x749DC5ADu, 0x76453719u, 0x77F684DFu, 0x799A130Bu,
    0x7B4097CEu, 0x7CF0BDC2u, 0x7E967699u, 0x7F7FFFFFu
};

             // Floating-Point General Notation with Precision

// Original source:
// https://github.com/microsoft/STL/blob/vs-2022-17.11/stl/inc/charconv
// Function `_Floating_to_chars_general_precision`
//
// Original notices:
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md
template <class t_FLOATING>
BSLA_NODISCARD
inline
char *
toCharsGeneralPrecisionImp(char *       first,
                           char * const last,
                           t_FLOATING   fpValue,
                           int          precision) BSLS_KEYWORD_NOEXCEPT
{
    typedef bslmf::FloatingTypeStructuralTraits<t_FLOATING> Traits;
    typedef typename Traits::UintType                       UintType;

    UintType uintValue = Traits::toUintType(fpValue);

    if (uintValue & Traits::k_SHIFTED_SIGN_MASK) { // Negative value
        if (first == last) {
            return 0;
        }
        *first++ = '-';

        uintValue &= ~Traits::k_SHIFTED_SIGN_MASK; // make it positive
        fpValue = Traits::toFloatType(uintValue);
    }

    if (uintValue == 0) { // zero detected; write "0" and return;
                          // precision is irrelevant due to zero-trimming
        if (first == last) {
            return 0;
        }

        *first++ = '0';

        return first;                                                 // RETURN
    }

    // C11 7.21.6.1 "The fprintf function"/5:
    //     "A negative precision argument is taken as if the precision were
    //      omitted."
    // /8: "g,G [...] Let P equal the precision if nonzero, 6 if the precision
    //      is omitted, or 1 if the precision is zero."

    // Performance note: It's possible to rewrite this for branchless codegen,
    // but profiling will be necessary to determine whether that's faster.
    if (precision < 0) {
        precision = 6;
    } else if (precision == 0) {
        precision = 1;
    } else if (precision < 1000000) {
        // precision is ok.
    } else {
        // Avoid integer overflow.
        // Due to general notation's zero-trimming behavior, we can simply
        // clamp precision.
        // This is further clamped below.
        precision = 1000000;
    }

    // precision is now the Standard's P.

    // /8: "Then, if a conversion with style E would have an exponent of X:
    // - if P > X >= -4, the conversion is with style f (or F) and
    //   precision P - (X + 1).
    // - otherwise, the conversion is with style e (or E) and precision P - 1."

    // /8: "Finally, [...] any trailing zeros are removed from the fractional
    //      portion of the result and the decimal-point character is removed if
    //      there is no fractional portion remaining."

    typedef GeneralPrecisionTables<t_FLOATING> Tables;

    const UintType *tableBegin;
    const UintType *tableEnd;

    if (precision <= Tables::k_MaxSpecialP) {
        tableBegin = Tables::specialXtable +
                                        (precision - 1) * (precision + 10) / 2;
        tableEnd   = tableBegin + precision + 5;
    } else {
        tableBegin = Tables::ordinaryXtable;
        tableEnd   = tableBegin + std::min(precision, Tables::k_MaxP) + 5;
    }

    const UintType* const tableLowerBound = std::lower_bound(
                                                        tableBegin,
                                                        tableEnd,
                                                        uintValue,
                                                        std::less<UintType>());

    const ptrdiff_t tableIndex     = tableLowerBound - tableBegin;
    const int scientificExponentX = static_cast<int>(tableIndex - 5);
    const bool useFixedNotation   = precision > scientificExponentX
                                 && scientificExponentX >= -4;

    // Performance note: it might (or might not) be faster to modify Ryu Printf
    // to perform zero-trimming.  Such modifications would involve a fairly
    // complicated state machine (notably, both '0' and '9' digits would need
    // to be buffered, due to rounding), and that would have performance costs
    // due to increased branching.  Here, we're using a simpler approach:
    // writing into a local buffer, manually zero-trimming, and then copying
    // into the output range. The necessary buffer size is reasonably small,
    // the zero-trimming logic is simple and fast, and the final copying is
    // also fast.

    // Note that: `Traits::k_GeneralMaxOutputLen` is determined by scientific
    //            notation and it is more than enough for fixed notation.
    // * 0x1.fffffep+127f is 39 digits, plus 1 for '.', plus
    //   `Traits::k_MaxFixedPrecision` for '0' digits, equals 77.
    // * 0x1.fffffffffffffp+1023 is 309 digits, plus 1 for '.', plus
    //   `Traits::k_MaxFixedPrecision` for '0' digits, equals 376.

    char buff[Traits::k_GENERAL_MAX_OUTPUT_LENGTH];
    const char *const significandFirst = buff; // e.g. "1.234"
    const char *      significandLast  = 0;
    const char *      exponentFirst    = 0; // e.g. "e-05"
    const char *      exponentLast     = 0;
    int               effectivePrecision; // number of digits printed after the
                                          // decimal point, before trimming

    // Write into the local buffer.
    // Clamping `effectivePrecision` allows `buff` to be as small as possible,
    // and increases efficiency.
    if (useFixedNotation) {
        effectivePrecision = std::min(precision - (scientificExponentX + 1),
                                      Traits::k_MAX_FIXED_PRECISION);
        const int n = blp_d2fixed_buffered_n_safe(fpValue,
                                                  effectivePrecision,
                                                  buff,
                                                  buff + sizeof buff);
        significandLast = buff + n;
    } else {
        effectivePrecision = std::min(precision - 1,
                                      Traits::k_MAX_SCIENTIFIC_PRECISION);

        const int n = blp_d2exp_buffered_n_safe(fpValue,
                                                effectivePrecision,
                                                buff,
                                                buff + sizeof buff);
        significandLast = std::find(buff, buff + n, 'e');
        exponentFirst   = significandLast;
        exponentLast    = buff + n;
    }

    // If we printed a decimal point followed by digits, perform zero-trimming.
    if (effectivePrecision > 0) {
        while (significandLast[-1] == '0') { // will stop at '.' or a nonzero
            --significandLast;               //                           digit
        }

        if (significandLast[-1] == '.') {
            --significandLast;
        }
    }

    // Copy the significand to the output range.
    const ptrdiff_t significandDistance = significandLast - significandFirst;
    if (last - first < significandDistance) {
        return 0;                                                     // RETURN
    }
    using std::memcpy;
    memcpy(first, significandFirst, static_cast<size_t>(significandDistance));
    first += significandDistance;

    // Copy the exponent to the output range.
    if (!useFixedNotation) {
        const ptrdiff_t exponentDistance = exponentLast - exponentFirst;
        if (last - first < exponentDistance) {
            return 0;                                                 // RETURN
        }
        memcpy(first, exponentFirst, static_cast<size_t>(exponentDistance));
        first += exponentDistance;
    }

    return first;
}

             // Floating-Point Scientific Notation with Precision

// The function below is modified source code from Microsoft STL
//
// Original source:
// https://github.com/microsoft/STL/blob/vs-2022-17.11/stl/inc/xcharconv_ryu.h
// Function `_Floating_to_chars_scientific_precision`
//
// Original notices relating to this code:
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md
template <class t_FLOATING>
BSLA_NODISCARD
inline
char *
toCharsScientificPrecisionImp(char *           first,
                              char * const     last,
                              const t_FLOATING fpValue,
                              int              precision) BSLS_KEYWORD_NOEXCEPT
{
    // C11 7.21.6.1 "The fprintf function"/5:
    //     "A negative precision argument is taken as if the precision were
    //      omitted."
    // /8: "g,G [...] Let P equal the precision if nonzero, 6 if the precision
    //      is omitted"

    // Performance note: It's possible to rewrite this for branchless codegen,
    // but profiling will be necessary to determine whether that's faster.
    if (precision < 0) {
        precision = 6;
    } else if (precision < 1000000) {
        // precision is ok.
    } else {
        // Avoid integer overflow.
        return 0;                                                     // RETURN
    }

    const int nChars = blp_d2exp_buffered_n_safe(fpValue, precision, first, last);
    if (0 == nChars) {
        return 0;                                                     // RETURN
    }

    return first + nChars;
}

           // Floating-Point Fixed/Decimal Notation with Precision

// The function below is modified source code from Microsoft STL
//
// Original source:
// https://github.com/microsoft/STL/blob/vs-2022-17.11/stl/inc/xcharconv_ryu.h
// Function `_Floating_to_chars_fixed_precision`
//
// Original notices relating to this code:
//
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Certain documents that are required to be distributed along with the code
// from the Microsoft STL repository may be found in the "thirdparty" folder of
// this BDE distribution under the names:
//
// - thirdparty/Microsoft-STL-LICENSE.txt
// - thirdparty/Microsoft-STL-NOTICE.txt
// - thirdparty/Microsoft-STL-README.md
template <class t_FLOATING>
BSLA_NODISCARD
inline
char *
toCharsDecimalPrecisionImp(char *           first,
                           char * const     last,
                           const t_FLOATING fpValue,
                           int              precision) BSLS_KEYWORD_NOEXCEPT
{
    // C11 7.21.6.1 "The fprintf function"/5:
    //     "A negative precision argument is taken as if the precision were
    //      omitted."
    // /8: "g,G [...] Let P equal the precision if nonzero, 6 if the precision
    //      is omitted"

    // Performance note: It's possible to rewrite this for branchless codegen,
    // but profiling will be necessary to determine whether that's faster.
    if (precision < 0) {
        precision = 6;
    } else if (precision < 1000000) {
        // precision is ok.
    } else {
        // Avoid integer overflow.
        return 0;                                                     // RETURN
    }

    const int nChars =
                  blp_d2fixed_buffered_n_safe(fpValue, precision, first, last);
    if (0 == nChars) {
        return 0;                                                     // RETURN
    }
    return first + nChars;
}
// Code in the above region is covered by its original license and copyrighted
// by its original authors.  See notices at the beginning of the region.
//
// Adapted Microsoft open source STL `to_chars` code                        END
//=============================================================================

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

    // Notice that if scientific form is shorter that is the one that will
    // be used, so essentially its maximum length determines the maximum.
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

    // Notice that if scientific form is shorter that is the one that will
    // be used, so essentially its maximum length determines the maximum.
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

char *NumericFormatterUtil::toCharsHex(char *first, char *last, double value)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    char *rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::floatingToCharsHexShortestImp(first, last, value);
}

char *NumericFormatterUtil::toCharsHex(char *first, char *last, float value)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    char* rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::floatingToCharsHexShortestImp(first, last, value);
}

char *
NumericFormatterUtil::toCharsGeneral(char *first, char *last, double value)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    char *rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::floatingToCharsGeneralImp(first, last, value);
}

char *
NumericFormatterUtil::toCharsGeneral(char *first, char *last, float value)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    char* rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::floatingToCharsGeneralImp(first, last, value);
}

char *
NumericFormatterUtil::toCharsDecimalPrec(char   *first,
                                         char   *last,
                                         double  value,
                                         int     precision)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    char* rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::toCharsDecimalPrecisionImp(first, last, value, precision);
}

char *
NumericFormatterUtil::toCharsDecimalPrec(char  *first,
                                         char  *last,
                                         float  value,
                                         int    precision)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    char* rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::toCharsDecimalPrecisionImp(first, last, value, precision);
}

char *
NumericFormatterUtil::toCharsScientificPrec(char   *first,
                                            char   *last,
                                            double  value,
                                            int     precision)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    char* rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::toCharsScientificPrecisionImp(first, last, value, precision);
}

char *
NumericFormatterUtil::toCharsScientificPrec(char  *first,
                                            char  *last,
                                            float  value,
                                            int    precision)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    char* rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::toCharsScientificPrecisionImp(first, last, value, precision);
}

char *
NumericFormatterUtil::toCharsGeneralPrec(char   *first,
                                         char   *last,
                                         double  value,
                                         int     precision)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    char* rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::toCharsGeneralPrecisionImp(first, last, value, precision);
}

char *
NumericFormatterUtil::toCharsGeneralPrec(char  *first,
                                         char  *last,
                                         float  value,
                                         int    precision)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    char* rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::toCharsGeneralPrecisionImp(first, last, value, precision);
}

char *
NumericFormatterUtil::toCharsHexPrec(char   *first,
                                     char   *last,
                                     double  value,
                                     int     precision) BSLS_KEYWORD_NOEXCEPT
{
    char* rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::toCharsHexPrecisionImp(first, last, value, precision);
}

char *
NumericFormatterUtil::toCharsHexPrec(char  *first,
                                     char  *last,
                                     float  value,
                                     int    precision) BSLS_KEYWORD_NOEXCEPT
{
    char* rv = 0;
    if (u::floatingHandleNanInf(first, last, value, &rv)) {
        return rv;
    }

    return u::toCharsHexPrecisionImp(first, last, value, precision);
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
