// bslalg_numericformatterutil.h                                      -*-C++-*-
#ifndef INCLUDED_BSLALG_NUMERICFORMATTERUTIL
#define INCLUDED_BSLALG_NUMERICFORMATTERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for formatting numbers into strings.
//
//@CLASSES:
//  bslalg::NumericFormatterUtil: namespace for `toChars` and support functions
//
//@DESCRIPTION: This component, `bslalg_numericformatterutil` provides a
// namespace `struct`, `bslalg::NumericFormatterUtil`, containing the
// overloaded function `toChars`, that converts integral and floating point
// types into ASCII strings.
//
///Shortest (Textual) Decimal Representation for Binary Floating Point Values
///--------------------------------------------------------------------------
// The floating point `toChars` implementations (for `float` and `double`) of
// this component provide the shortest (textual) decimal representation that
// can (later) be parsed back to the original binary value (i.e., a
// "round-trip" conversion).  Such round-tripping enables precise, and
// human-friendly (textual) communication protocols, and storage formats that
// use minimal necessary bandwidth or storage.
//
// Scientific notation, when chosen, always uses the minimum number of
// fractional digits necessary to restore the exact binary floating point
// value.  The shortest *decimal* notation of a binary floating point number is
// text that has enough decimal **fractional** digits so that there can be no
// ambiguity in which binary representation value is closest to it.  Notice
// that the previous sentence only addresses the number of *fractional* digits
// in the decimal notation.  Floating point values that are mathematically
// integer are always written as their exact integer value in decimal notation.
// For large integers it would not strictly be necessary to use the exact
// decimal value as many integers (differing in some lower-decimal digits) may
// resolve to the same binary value, but readers may not expect integers to be
// "rounded", so C and C++ chose to standardize on the exact value.
//
// Note that strictly speaking the C++-defined shortest round trip
// representation is not the shortest *possible* one as the C++ scientific
// notation is defined to possibly contain up to two extra characters: the sign
// of the exponent is always written (even for positive exponents), and at
// least 2 decimal digits of the exponent are always written.
//
// More information about the difficulty of rendering binary floating point
// numbers as decimals can be found at
// https://bloomberg.github.io/bde/articles/binary_decimal_conversion.html .
// In short, IEEE-754 double precision binary floating point numbers (`double`)
// are guaranteed to round-trip when represented by 17 significant decimal
// digits, while single precisions (`float`) needs 9 digits.  However those
// numbers are the *maximum* decimal digits that *may* be necessary, and in
// fact many values can be precisely represented precisely by less.  `toChars`
// renders the minimum number of digits needed, so that the value can later be
// restored.
//
///Default Floating Point Format
///-----------------------------
// The default floating point format (that is used when no `format` argument is
// present in the signature) uses the shortest representation from the decimal
// notation and the scientific notation, favoring decimal notation in case of a
// tie.
//
///General Floating Point Format
///-----------------------------
// The general floating point format uses the shortest representation of either
// the decimal notation or the scientific notation.  The decision between the
// two notations is done so that if the scientific notation's exponent would be
// [6, -4) (less than 7 and greater than -4) decimal notation is produced,
// otherwise (if the exponent is 7 or greater, or -4 or smaller) the scientific
// notation will be used.
//
///Special Floating Point Values
///-----------------------------
// Floating point values may also be special-numerical or non-numerical(*)
// values in addition to what we consider normal numbers.
//
// The special numerical value is really just one, and that is negative zero.
//
// For non-numerical special value both IEEE-754 and W3C XML Schema Definition
// Language (XSD) 1.1(**) `numericalSpecialRep` requires there to be three
// distinct values supported: positive infinity, negative infinity, and NaN.
// We represent those values according to the XSD lexical mapping
// specification.  That also means that these values will round trip in text
// *only* if the reader algorithm recognizes those representations.
//
// ```
// +-------------------+----------------+
// | Special Value     | Textual Repr.  |
// +-------------------+----------------+
// | positive zero     |  "0",  "0e+00" |
// +-------------------+----------------+
// | negative zero     | "-0", "-0e+00" |
// +-------------------+----------------+
// | positive infinity | "inf"          |
// +-------------------+----------------+
// | negative infinity | "-inf"         |
// +-------------------+----------------+
// | not-a-number      | "nan"          |
// +-------------------+----------------+
// | neg. not-a-number | "-nan"         |
// +-------------------+----------------+
// ```
//
// (*) Non-numerical values do not represent a specific mathematical value.  Do
//     not confuse non-numerical values with Not-a-Number.  NaN is just one of
//     the possible non-numerical values.  The positive and negative infinity
//     represent *all* values too large (in their absolute value) to store. NaN
//     represents all other values that cannot be represented by a real number.
//     Non-numerical values normally come from computation results such as the
//     square root of -1 resulting in Not-a-Number.
//
// (**) https://www.w3.org/TR/xmlschema11-2/
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example 1: Writing an Integer to a `streambuf`
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a function that writes an `int` to a `streambuf`.
// We can use `bsl::to_chars` to write the `int` to a buffer, then write the
// buffer to the `streambuf`.
//
// First, we declare our function:
// ```
// void writeJsonScalar(std::streambuf *result, int value)
//     // Write the specified 'value', in decimal, to the specified 'result'.
// {
// ```
// Then, we declare a buffer long enough to store any `int` value in decimal.
// ```
//     char buffer[bslalg::NumericFormatterUtil::
//                                            ToCharsMaxLength<int>::k_VALUE];
//                                // size large enough to write 'INT_MIN', the
//                                // worst-case value, in decimal.
// ```
// Next, we call the function:
// ```
//     char *ret = bslalg::NumericFormatterUtil::toChars(
//                                                     buffer,
//                                                     buffer + sizeof buffer,
//                                                     value);
// ```
// Then, we check that the buffer was long enough, which should always be the
// case:
// ```
//     assert(0 != ret);
// ```
// Now, we write our buffer to the `streambuf`:
// ```
//     result->sputn(buffer, ret - buffer);
// }
// ```
// Finally, we use an output string stream buffer to exercise the
// `writeJsonScalar` function for `int`:
// ```
// std::ostringstream  oss;
// std::streambuf* sb = oss.rdbuf();
//
// writeJsonScalar(sb, 0);
// assert("0" == oss.str());
//
// oss.str("");
// writeJsonScalar(sb, 99);
// assert("99" == oss.str());
//
// oss.str("");
// writeJsonScalar(sb, -1234567890);  // worst case: max string length
// assert("-1234567890" == oss.str());
// ```
//
///Example 2: Writing the Minimal Form of a `double`
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to store a floating point number using decimal text (such as
// JSON) for later retrieval, using the minimum number of digits that ensures
// we can later restore the same binary floating point value.
//
// First, we declare our writer function:
// ```
// void writeJsonScalar(std::streambuf *result,
//                      double          value,
//                      bool            stringNonNumericValues = false)
//     // Write the specified 'value' in the shortest round-trip decimal
//     // format into the specified 'result'.  Write non-numeric values
//     // according to the optionally specified 'stringNonNumericValues'
//     // either as strings "NaN", "+Infinity", or "-Infinity" when
//     // 'stringNonNumericValues' is 'true', or a null when it is 'false' or
//     // not specified.
// {
// ```
// Then, we handle non-numeric values (`toChars` would write them the XSD way):
// ```
//     if (isnan(value) || isinf(value)) {
//         if (false == stringNonNumericValues) {  // JSON standard output
//             result->sputn("null", 4);
//         }
//         else {                                  // Frequent JSON extension
//             if (isnan(value)) {
//                 result->sputn("\"NaN\"", 5);
//             }
//             else if (isinf(value)) {
//                 result->sputn(value < 0 ? "\"-" : "\"+", 2);
//                 result->sputn("Infinity\"", 9);
//             }
//         }
//         return;                                                   // RETURN
//     }
// ```
// Next, we declare a buffer long enough to store any `double` value written in
// this minimal-length form:
// ```
//     char buffer[bslalg::NumericFormatterUtil::
//                                         ToCharsMaxLength<double>::k_VALUE];
//                               // large enough to write the longest 'double'
//                               // without a null terminator character.
// ```
// Then, we call the function:
// ```
//     char *ret = bslalg::NumericFormatterUtil::toChars(
//                                                     buffer,
//                                                     buffer + sizeof buffer,
//                                                     value);
// ```
// Finally, we can write our buffer to the `streambuf`:
// ```
//     result->sputn(buffer, ret - buffer);
// }
// ```
// Finally, we use the output string stream buffer defined earlier to exercise
// the floating point `writeJsonScalar` function:
// ```
// oss.str("");
// writeJsonScalar(sb, 20211017.0);
// assert("20211017" == oss.str());
//
// oss.str("");
// writeJsonScalar(sb, 3.1415926535897932);
// assert("3.141592653589793" == oss.str());
//
// oss.str("");
// writeJsonScalar(sb, 2e5);
// assert("2e+05" == oss.str());
//
// oss.str("");                  // Non-numeric are written as null by default
// writeJsonScalar(sb, std::numeric_limits<double>::quiet_NaN());
// assert("null" == oss.str());  oss.str("");
//
// oss.str("");                  // Non-numeric can be printed as strings
// writeJsonScalar(sb, std::numeric_limits<double>::quiet_NaN(), true);
// assert("\"NaN\"" == oss.str());  oss.str("");
// ```
//
///Example 3: Determining The Necessary Minimum Buffer Size
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose you are writing code that uses `bslalg::NumericFormatterUtil` to
// convert values to text.  Determining the necessary buffer sizes to ensure
// successful conversions, especially for floating point types, is non-trivial,
// and frankly usually strikes as a distraction in the flow of the work.  This
// component provides the `ToCharsMaxLength` `struct` "overloaded" template
// that parallels the overloaded `toChars` function variants and provides the
// well-vetted and tested minimum sufficient buffer size values as compile time
// constants.
//
// Determining the sufficient buffer size for any conversion starts with
// determining "What type are we converting?" and "Do we use an argument to
// control the conversion, and is that argument a compile time time constant?
//
// First, because of the descriptive type names we may want to start by locally
// shortening them using a `typedef`:
// ```
// typedef bslalg::NumericFormatterUtil NfUtil;
// ```
// Next, we determine the sufficient buffer size for converting a `long` to
// decimal.  `long` is a type that has different `sizeof` on different 64 bit
// platforms, so it is especially convenient to have that difference hidden:
// ```
// const size_t k_LONG_DEC_SIZE = NfUtil::ToCharsMaxLength<long>::k_VALUE;
//     // Sufficient buffer size to convert any 'long' value to decimal text.
// ```
// Then, we can write the longest possible `long` successfully into a buffer:
// ```
// char longDecimalBuffer[k_LONG_DEC_SIZE];
//     // We can write any 'long' in decimal into this buffer using
//     // 'NfUtil::toChars' safely.
//
// char *p = NfUtil::toChars(longDecimalBuffer,
//                           longDecimalBuffer + sizeof longDecimalBuffer,
//                           LONG_MIN);
// assert(p != 0);
// ```
// Next, we can get the sufficient size for conversion of an `unsigned int` to
// octal:
// ```
// const size_t k_UINT_OCT_SIZE = NfUtil::ToCharsMaxLength<unsigned,
//                                                         8>::k_VALUE;
// ```
// Then, if we do not know what `base` value `toChars` will use we have to,
// assume the longest, which is always base 2:
// ```
// const size_t k_SHRT_MAX_SIZE = NfUtil::ToCharsMaxLength<short, 2>::k_VALUE;
// ```
// Now, floating point types have an optional `format` argument instead of a
// `base`, with "default" format as the default, and "fixed" and "scientific"
// formats are selectable when a `format` argument is specified:
// ```
// const size_t k_DBL_DFL_SIZE = NfUtil::ToCharsMaxLength<double>::k_VALUE;
//
// const size_t k_FLT_DEC_SIZE = NfUtil::ToCharsMaxLength<
//                                                  float,
//                                                  NfUtil::e_FIXED>::k_VALUE;
//
// const size_t k_DBL_SCI_SIZE = NfUtil::ToCharsMaxLength<
//                                             double,
//                                             NfUtil::e_SCIENTIFIC>::k_VALUE;
// ```
// Finally, the longest floating point format is `e_FIXED`, so if the `format`
// argument is not known at compile time, `e_FIXED` should be used:
// ```
// const size_t k_DBL_MAX_SIZE = NfUtil::ToCharsMaxLength<
//                                                  double,
//                                                  NfUtil::e_FIXED>::k_VALUE;
// ```

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_removecv.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>

#include <limits>  // 'bsl' cannot be used in 'bslalg'

namespace BloombergLP {
namespace bslalg {

                        // ===========================
                        // struct NumericFormatterUtil
                        // ===========================

/// Namespace `struct` for free functions supporting `to_chars`.
struct NumericFormatterUtil {

  private:
    // PRIVATE CLASS METHODS

    /// Write the specified `value` into the character buffer starting at the
    /// specified `first` and ending at the specified `last`, rendering the
    /// value in the specified `base`.  On success, return a the address one
    /// past the lowest order digit written, on failure, return 0.  The only
    /// reason for failure is if the range `[ first, last )` is not large
    /// enough to contain the result.  The written result is to begin at
    /// `first` with leftover room following the return value.  The behavior is
    /// undefined unless `first <= last` and `base` is in the range
    /// `[ 2 .. 36 ]`.
    static char *toCharsImpl(char     *first,
                             char     *last,
                             unsigned  value,
                             int       base) BSLS_KEYWORD_NOEXCEPT;

    /// Write the specified `value` into the character buffer starting at the
    /// specified `first` and ending at the specified `last`, rendering the
    /// value in the specified `base`.  On success, return a the address one
    /// past the lowest order digit written, on failure, return 0.  The only
    /// reason for failure is if the range `[ first, last )` is not large
    /// enough to contain the result.  The written result is to begin at
    /// `first` with leftover room following the return value.  The behavior
    /// is undefined unless `first <= last` and `base` is in the range
    /// `[ 2 .. 36 ]`.
    static
    char *toCharsImpl(char                    *first,
                      char                    *last,
                      unsigned long long int   value,
                      int                      base) BSLS_KEYWORD_NOEXCEPT;

    /// Write the textual representation of the specified `value` in the
    /// specified `base` into the character buffer starting at the specified
    /// `first` and ending at the specified `last`.  Return the address one
    /// past the lowest order digit written on success, or 0 on failure.
    /// The only possible reason for failure is if the range
    /// `[ first, last )` is not large enough to contain the result.  The
    /// written result is to begin at `first` with leftover room following
    /// the return value.  The behavior is undefined unless `first < last`
    /// and `base` is in the range `[ 2 .. 36 ]`.  The behavior is also
    /// undefined unless the specified `TYPE` is a fundamental integral type
    /// not larger than 64 bits.
    template <class TYPE>
    static char *toCharsIntegral(char *first,
                                 char *last,
                                 TYPE  value,
                                 int   base) BSLS_KEYWORD_NOEXCEPT;

    /// Write the textual representation of the specified `value` in decimal
    /// notation into the character buffer starting at the specified `first`
    /// and ending at the specified `last`.  Return the address one past the
    /// lowest order digit written on success, or 0 on failure.  The only
    /// possible reason for failure is if the range `[ first, last )` is not
    /// large enough to contain the result.  The written result is to begin at
    /// `first` with leftover room following the return value.
    static char *toCharsDecimal(char   *first,
                                char   *last,
                                double  value) BSLS_KEYWORD_NOEXCEPT;
    static char *toCharsDecimal(char   *first,
                                char   *last,
                                float   value) BSLS_KEYWORD_NOEXCEPT;

    /// Write the textual representation of the specified `value` in scientific
    /// notation into the character buffer starting at the specified `first`
    /// and ending at the specified `last`.  Return the address one past the
    /// lowest order digit of the exponent written on success, or 0 on failure.
    /// The only possible reason for failure is if the range `[ first, last )`
    /// is not large enough to contain the result.  The written result is to
    /// begin at `first` with leftover room following the return value.
    static char *toCharsScientific(char   *first,
                                   char   *last,
                                   double  value) BSLS_KEYWORD_NOEXCEPT;
    static char *toCharsScientific(char   *first,
                                   char   *last,
                                   float   value) BSLS_KEYWORD_NOEXCEPT;

    /// Write the textual representation of the specified `value` in general
    /// notation into the character buffer starting at the specified `first`
    /// and ending at the specified `last`.  Return the address one past the
    /// lowest order digit written on success, or 0 on failure.  The only
    /// possible reason for failure is if the range `[ first, last )` is not
    /// large enough to contain the result.  The written result is to begin at
    /// `first` with leftover room following the return value.
    static char *toCharsGeneral(char   *first,
                                char   *last,
                                double  value) BSLS_KEYWORD_NOEXCEPT;
    static char *toCharsGeneral(char   *first,
                                char   *last,
                                float   value) BSLS_KEYWORD_NOEXCEPT;

    /// Write the textual representation of the specified `value` in hexfloat
    /// notation into the character buffer starting at the specified `first`
    /// and ending at the specified `last`.  Return the address one past the
    /// lowest order digit of the exponent written on success, or 0 on failure.
    /// The only possible reason for failure is if the range `[ first, last )`
    /// is not large enough to contain the result.  The written result is to
    /// begin at `first` with leftover room following the return value.
    static char *toCharsHex(char   *first,
                            char   *last,
                            double  value) BSLS_KEYWORD_NOEXCEPT;
    static char *toCharsHex(char   *first,
                            char   *last,
                            float   value) BSLS_KEYWORD_NOEXCEPT;

    /// Write the textual representation of the specified `value` in decimal
    /// notation using the specified `precision` into the character buffer
    /// starting at the specified `first` and ending at the specified `last`.
    /// Return the address one past the lowest order digit written on success,
    /// or 0 on failure.  The only possible reason for failure is if the range
    /// `[ first, last )` is not large enough to contain the result.  The
    /// written result is to begin at `first` with leftover room following the
    /// return value.
    static char *toCharsDecimalPrec(char   *first,
                                    char   *last,
                                    double  value,
                                    int     precision) BSLS_KEYWORD_NOEXCEPT;
    static char *toCharsDecimalPrec(char   *first,
                                    char   *last,
                                    float   value,
                                    int     precision) BSLS_KEYWORD_NOEXCEPT;

    /// Write the textual representation of the specified `value` in scientific
    /// notation using the specified `precision` into the character buffer
    /// starting at the specified `first` and ending at the specified `last`.
    /// Return the address one past the lowest order digit of the exponent
    /// written on success, or 0 on failure.  The only possible reason for
    /// failure is if the range `[ first, last )` is not large enough to
    /// contain the result.  The written result is to begin at `first` with
    /// leftover room following the return value.
    static char *toCharsScientificPrec(char   *first,
                                       char   *last,
                                       double  value,
                                       int     precision)
                                                         BSLS_KEYWORD_NOEXCEPT;
    static char *toCharsScientificPrec(char   *first,
                                       char   *last,
                                       float   value,
                                       int     precision)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Write the textual representation of the specified `value` in general
    /// notation using the specified `precision` into the character buffer
    /// starting at the specified `first` and ending at the specified `last`.
    /// Return the address one past the lowest order digit written on success,
    /// or 0 on failure.  The only possible reason for failure is if the range
    /// `[ first, last )` is not large enough to contain the result.  The
    /// written result is to begin at `first` with leftover room following the
    /// return value.
    static char *toCharsGeneralPrec(char   *first,
                                    char   *last,
                                    double  value,
                                    int     precision) BSLS_KEYWORD_NOEXCEPT;
    static char *toCharsGeneralPrec(char   *first,
                                    char   *last,
                                    float   value,
                                    int     precision) BSLS_KEYWORD_NOEXCEPT;

    /// Write the textual representation of the specified `value` in hexfloat
    /// notation using the specified `precision` into the character buffer
    /// starting at the specified `first` and ending at the specified `last`.
    /// Return the address one past the lowest order digit of the exponent
    /// written on success, or 0 on failure.  The only possible reason for
    /// failure is if the range `[ first, last )` is not large enough to
    /// contain the result.  The written result is to begin at `first` with
    /// leftover room following the return value.
    static char *toCharsHexPrec(char   *first,
                                char   *last,
                                double  value,
                                int precision) BSLS_KEYWORD_NOEXCEPT;
    static char *toCharsHexPrec(char   *first,
                                char   *last,
                                float   value,
                                int precision) BSLS_KEYWORD_NOEXCEPT;

  private:
    // NOT IMPLEMENTED

    /// This deleted/private method declaration exists to prevent `toChars`
    /// being called with a `bool` input argument.
    static char *toChars(char*, char*, bool, int = 10) BSLS_KEYWORD_NOEXCEPT
                                                       BSLS_KEYWORD_DELETED;

  private:
    // PRIVATE TYPES

    /// This enumerator is used internally to achieve "type safe"
    /// pseudo-overloading of the `ToCharsMaxLength` template to mimic the
    /// different `toChars` class method overloads.
    enum { k_MAXLEN_ARG_DEFAULT = -256 };

    // PRIVATE METAFUNCTIONS

    /// This `struct` template implements a meta-function to determine the
    /// minimum sufficient size (in characters) of an output buffer to
    /// successfully convert any value of the specified `FLT_TYPE` floating
    /// point type into text of the specified `FORMAT` using `toChars`.
    /// Format may be any of the `Format` enumerator values or an
    /// unspecified value for default format.  The floating point types are
    /// assumed to be IEEE-754 binary types.  The result ("return value") is
    /// a member enumerator `k_VALUE`.  This meta-function is private and
    /// contains no defensive checks.
    template <class FLT_TYPE, int FORMAT>
    struct FltMaxLen;

    /// This `struct` template implements a meta-function to determine the
    /// minimum sufficient size (in characters) of an output buffer to
    /// successfully convert any value of a fundamental integer type to text
    /// into the specified `BASE` using `toChars`.  The integer type is
    /// described by the specified `IS_SIGNED` and `SIZEOF` parameters, and
    /// is assumed to be two's complement.  The result ("return value") is
    /// an enumerator `k_VALUE`.  The meta-function is private and contains
    /// no defensive checks.
    template <bool IS_SIGNED, unsigned SIZEOF, int BASE>
    struct IntMaxLen;

    /// This `struct` template implements the meta-function to determine if
    /// the specified `TYPE` is a supported floating point type for the
    /// `toChars` overloaded method-set.  The result ("return value") is an
    /// enumerator `k_SUPPORTED` that has a non-zero value if `TYPE` is a
    /// supported floating point type, or zero otherwise.
    template <class TYPE>
    struct IsSupportedFloatingPoint;

    /// This `struct` template implements the meta-function to determine if
    /// the specified `TYPE` is a supported integral type for the `toChars`
    /// overloaded method-set.  The result ("return value") is an enumerator
    /// `k_SUPPORTED` that has a non-zero value if `TYPE` is a supported
    /// integral type, or zero otherwise.
    template <class TYPE>
    struct IsSupportedIntegral;

  public:
    // PUBLIC TYPES

    /// This enumeration lists the supported, explicitly specified 'toChars'
    /// formatting options for floating point values, according to ISO C++17.
    enum Format {
        e_SCIENTIFIC = 0x040,
        e_FIXED      = 0x080,
        e_HEX        = 0x100,
        e_GENERAL    = e_FIXED | e_SCIENTIFIC
    };

    // PUBLIC METAFUNCTIONS

    /// This `struct` template implements the meta-function to determine the
    /// minimum sufficient size of a buffer to successfully convert any
    /// numeric value of a specified `TYPE` supported by one of the
    /// `toChars` function overloads in `NumericFormatterUtil`.  The
    /// meta-function allows specifying an argument value to the `toChars`
    /// overloads, as a non-type template parameter.  That value stands for
    /// the `base` parameter for integral conversions, and the `format`
    /// parameter for floating point conversions.  A second non-type
    /// template parameter is reserved for further addition in case the
    /// `precision` parameter overloads are implemented for floating point
    /// conversions (in addition to the `format` parameter).  The
    /// compile-time "return value" of `ToCharsMaxLength` is an enumerator
    /// name `k_VALUE`.  For usage examples see {Example 3: Determining The
    /// Required Buffer Size}.
    template <class TYPE, int ARG = k_MAXLEN_ARG_DEFAULT>
    struct ToCharsMaxLength;

    // PUBLIC CLASS METHODS

    /// Write the specified integral `value` into the character buffer starting
    // a the/ specified `first` and ending at the specified `last`, `last` not
    /// included.  If the optionally specified `base`argument is not present
    /// base 10 is used. Return the address one past the last character (lowest
    /// order digit or last digit of the exponent) written on success, or `0`
    /// on failure.  The only reason for failure is when the range
    /// `[ first, last )` is not large enough to contain the result.  The
    /// written result is to begin at `first` with leftover room following the
    /// return value.  The behavior is undefined unless `first <= last`, and
    /// `base` is in the range `[ 2 .. 36 ]`.  Note that the type `bool` for
    /// the `value` parameter is explicitly disabled in the "NOT IMPLEMENTED"
    /// `private` section, because `bool` would otherwise be promoted to `int`
    /// and printed as `0` or `1`, instead of the (possibly) expected `false`
    /// and `true`; and `bool` is not an integral or numeric type either.  Also
    /// note that these functions do **not** null-terminate the result.
    static
    char *toChars(char                   *first,
                  char                   *last,
                  char                    value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  signed char             value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  unsigned char           value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  signed short int        value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  unsigned short int      value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  signed int              value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  unsigned int            value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  signed long int         value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  unsigned long int       value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  signed long long int    value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  unsigned long long int  value,
                  int                     base = 10) BSLS_KEYWORD_NOEXCEPT;

    /// Write the specified floating point `value` into the character buffer
    /// starting a the specified `first` and ending at the specified `last`,
    /// `last` not included.  Use the {Default Floating Point Format}.  Return
    /// the address one past the last character (lowest order digit or last
    /// digit of the exponent) written on success, or `0` on failure.  The only
    /// reason for failure is when the range `[ first, last )` is not large
    /// enough to contain the result.  The written result is to begin at
    /// `first` with leftover room following the return value.  The behavior is
    /// undefined unless `first <= last`.  Note that these functions do **not**
    /// null-terminate the result.
    static
    char *toChars(char                   *first,
                  char                   *last,
                  double                  value) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  float                   value) BSLS_KEYWORD_NOEXCEPT;

    /// Write the specified floating point `value` into the character buffer
    /// starting a the specified `first` and ending at the specified `last`,
    /// `last` not included, using the specified `format` with the {Shortest
    /// (Textual) Decimal Representation for Binary Floating Point Values} used
    /// in that format (that will produce the exact binary floating point
    /// `value` when converted back to the original type from text), but see
    /// possible exceptions under {Special Floating Point Values}.  Return the
    /// address one past the last character (lowest order digit or last digit
    /// of the exponent) written on success, or `0` on failure.  The only
    /// reason for failure is when the range `[ first, last )` is not large
    /// enough to contain the result.  The written result is to begin at
    /// `first` with leftover room following the return value.  The behavior is
    /// undefined unless `first <= last`.  Note that these functions do **not**
    /// null-terminate the result.
    static
    char *toChars(char                   *first,
                  char                   *last,
                  double                  value,
                  Format                  format) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  float                   value,
                  Format                  format) BSLS_KEYWORD_NOEXCEPT;

    /// Write the specified floating point `value` into the character buffer
    /// starting a the specified `first` and ending at the specified `last`,
    /// `last` not included, using the specified `format` and `precision`.
    /// Return the address one past the last character (lowest order digit or
    /// last digit of the exponent) written on success, or `0` on failure.  The
    /// only reason for failure is when the range `[ first, last )` is not
    /// large enough to contain the result.  The written result is to begin at
    /// `first` with leftover room following the return value.  The behavior is
    /// undefined unless `first <= last`.  Note that these functions do **not**
    /// null-terminate the result.
    static
    char *toChars(char                   *first,
                  char                   *last,
                  double                  value,
                  Format                  format,
                  int                     precision) BSLS_KEYWORD_NOEXCEPT;
    static
    char *toChars(char                   *first,
                  char                   *last,
                  float                   value,
                  Format                  format,
                  int                     precision) BSLS_KEYWORD_NOEXCEPT;

    /// Metafunction that provides access to `float` and `double` variation of
    /// the `value` class method that returns the maximum required buffer size
    /// to for the textual representation (`toChars`) for a specified format
    /// and precision value.  The metafuction provides a single class method
    /// `value` with the signature `size_t value(Format format, int precision)`
    /// that returns the maximum required buffer size for the specified
    /// `format` and `precision`.
    template <class t_FLOATING>
    struct PrecisionMaxBufferLength;
};

      // --------------------------------------------------------------
      // template struct NumericFormatterUtil::PrecisionMaxBufferLength

template <class t_FLOATING>
struct NumericFormatterUtil::PrecisionMaxBufferLength {};

template <>
struct NumericFormatterUtil::PrecisionMaxBufferLength<double> {

    ///  Return the maximum required output buffer size for `toChars`
    ///  conversion of a `double` with the specified `format` and `precision`.
    static
    BSLS_KEYWORD_CONSTEXPR_CPP14 size_t value(Format format, int precision)
    {
        if (-1 == precision) {
            precision = (e_HEX == format) ? 13 : 6;
        }

        switch (format) {
          case e_FIXED:      return 311 + precision;                  // RETURN
          case e_SCIENTIFIC: return   8 + precision;                  // RETURN
          case e_HEX:        return  22 + precision;                  // RETURN
          case e_GENERAL:    return   8 + precision;                  // RETURN
        }

        return 0;
    }
};

template <>
struct NumericFormatterUtil::PrecisionMaxBufferLength<float> {

    ///  Return the maximum required output buffer size for `toChars`
    ///  conversion of a `float` with the specified `format` and `precision`.
    static
    BSLS_KEYWORD_CONSTEXPR_CPP14 size_t value(Format format, int precision)
    {
        if (-1 == precision) {
            precision = 6;
        }

        switch (format) {
          case e_FIXED:      return 41 + precision;                   // RETURN
          case e_SCIENTIFIC: return  7 + precision;                   // RETURN
          case e_HEX:        return  8 + precision;                   // RETURN
          case e_GENERAL:    return  7 + precision;                   // RETURN
        }

        return 0;
    }
};

            // -----------------------------------------------
            // template struct NumericFormatterUtil::FltMaxLen
            // -----------------------------------------------

template <class FLT_TYPE, int FORMAT>
struct NumericFormatterUtil::FltMaxLen {
    // PUBLIC TYPES
    enum {
        k_VALUE = -1 // Ensure this is invalid for array length
    };
};

template <>
struct NumericFormatterUtil::FltMaxLen<double, NumericFormatterUtil::e_FIXED> {
    // PUBLIC TYPES
    enum {
        k_VALUE = 327
    };
};

template <>
struct NumericFormatterUtil::FltMaxLen<double, NumericFormatterUtil::e_GENERAL> {
    // PUBLIC TYPES
    enum {
        k_VALUE = 24
    };
};

template <>
struct NumericFormatterUtil::FltMaxLen<double, NumericFormatterUtil::e_HEX> {
    // PUBLIC TYPES
    enum {
        k_VALUE = 22
    };
};

template <>
struct NumericFormatterUtil::FltMaxLen<double, NumericFormatterUtil::e_SCIENTIFIC> {
    // PUBLIC TYPES
    enum {
        k_VALUE = 24
    };
};

template <>
struct NumericFormatterUtil::FltMaxLen<
                                    double,
                                    NumericFormatterUtil::k_MAXLEN_ARG_DEFAULT>
: NumericFormatterUtil::FltMaxLen<double, NumericFormatterUtil::e_SCIENTIFIC> {
};


template <>
struct NumericFormatterUtil::FltMaxLen<float, NumericFormatterUtil::e_FIXED> {
    // PUBLIC TYPES
    enum {
        k_VALUE = 48
    };
};

template <>
struct NumericFormatterUtil::FltMaxLen<float, NumericFormatterUtil::e_GENERAL> {
    // PUBLIC TYPES
    enum {
        k_VALUE = 15
    };
};

template <>
struct NumericFormatterUtil::FltMaxLen<float, NumericFormatterUtil::e_HEX> {
    // PUBLIC TYPES
    enum {
        k_VALUE = 14
    };
};

template <>
struct NumericFormatterUtil::FltMaxLen<float, NumericFormatterUtil::e_SCIENTIFIC> {
    // PUBLIC TYPES
    enum {
        k_VALUE = 15
    };
};

template <>
struct NumericFormatterUtil::FltMaxLen<
                                    float,
                                    NumericFormatterUtil::k_MAXLEN_ARG_DEFAULT>
: NumericFormatterUtil::FltMaxLen<float, NumericFormatterUtil::e_SCIENTIFIC> {
};

            // -----------------------------------------------
            // template struct NumericFormatterUtil::IntMaxLen
            // -----------------------------------------------

#define BSLALG_NUMERICFORMATTERUTIL_INTMAXLEN_ONE(issigned, bytes,            \
        val02, val03, val04, val05, val06, val07, val08, val09, val10,        \
        val11, val12, val13, val14, val15, val16, val17, val18, val19, val20, \
        val21, val22, val23, val24, val25, val26, val27, val28, val29, val30, \
        val31, val32, val33, val34, val35, val36)                             \
    template <int BASE>                                                       \
    struct NumericFormatterUtil::IntMaxLen<issigned, bytes, BASE> {           \
        enum Enum {                                                           \
            k_VALUE = (BASE ==  2) ? val02                                    \
                    : (BASE ==  3) ? val03                                    \
                    : (BASE ==  4) ? val04                                    \
                    : (BASE ==  5) ? val05                                    \
                    : (BASE ==  6) ? val06                                    \
                    : (BASE ==  7) ? val07                                    \
                    : (BASE ==  8) ? val08                                    \
                    : (BASE ==  9) ? val09                                    \
                    : (BASE == 10) ? val10                                    \
                    : (BASE == 11) ? val11                                    \
                    : (BASE == 12) ? val12                                    \
                    : (BASE == 13) ? val13                                    \
                    : (BASE == 14) ? val14                                    \
                    : (BASE == 15) ? val15                                    \
                    : (BASE == 16) ? val16                                    \
                    : (BASE == 17) ? val17                                    \
                    : (BASE == 18) ? val18                                    \
                    : (BASE == 19) ? val19                                    \
                    : (BASE == 20) ? val20                                    \
                    : (BASE == 21) ? val21                                    \
                    : (BASE == 22) ? val22                                    \
                    : (BASE == 23) ? val23                                    \
                    : (BASE == 24) ? val24                                    \
                    : (BASE == 25) ? val25                                    \
                    : (BASE == 26) ? val26                                    \
                    : (BASE == 27) ? val27                                    \
                    : (BASE == 28) ? val28                                    \
                    : (BASE == 29) ? val29                                    \
                    : (BASE == 30) ? val30                                    \
                    : (BASE == 31) ? val31                                    \
                    : (BASE == 32) ? val32                                    \
                    : (BASE == 33) ? val33                                    \
                    : (BASE == 34) ? val34                                    \
                    : (BASE == 35) ? val35                                    \
                    : (BASE == 36) ? val36                                    \
                    : val10 /* default value */                               \
        };                                                                    \
    }

// BDE_VERIFY pragma: push   // Relax mandatory tag rules for macro-created
// BDE_VERIFY pragma: -KS00  // template specializations
BSLALG_NUMERICFORMATTERUTIL_INTMAXLEN_ONE(true, 1,                  //  int8_t
    9,  // base  2: "-10000000"
    6,  // base  3: "-11202"
    5,  // base  4: "-2000"
    5,  // base  5: "-1003"
    4,  // base  6: "-332"
    4,  // base  7: "-242"
    4,  // base  8: "-200"
    4,  // base  9: "-152"
    4,  // base 10: "-128"
    4,  // base 11: "-107"
    3,  // base 12: "-a8"
    3,  // base 13: "-9b"
    3,  // base 14: "-92"
    3,  // base 15: "-88"
    3,  // base 16: "-80"
    3,  // base 17: "-79"
    3,  // base 18: "-72"
    3,  // base 19: "-6e"
    3,  // base 20: "-68"
    3,  // base 21: "-62"
    3,  // base 22: "-5i"
    3,  // base 23: "-5d"
    3,  // base 24: "-58"
    3,  // base 25: "-53"
    3,  // base 26: "-4o"
    3,  // base 27: "-4k"
    3,  // base 28: "-4g"
    3,  // base 29: "-4c"
    3,  // base 30: "-48"
    3,  // base 31: "-44"
    3,  // base 32: "-40"
    3,  // base 33: "-3t"
    3,  // base 34: "-3q"
    3,  // base 35: "-3n"
    3); // base 36: "-3k"

BSLALG_NUMERICFORMATTERUTIL_INTMAXLEN_ONE(false, 1,                 // uint8_t
    8,  // base  2: "11111111"
    6,  // base  3: "100110"
    4,  // base  4: "3333"
    4,  // base  5: "2010"
    4,  // base  6: "1103"
    3,  // base  7: "513"
    3,  // base  8: "377"
    3,  // base  9: "313"
    3,  // base 10: "255"
    3,  // base 11: "212"
    3,  // base 12: "193"
    3,  // base 13: "168"
    3,  // base 14: "143"
    3,  // base 15: "120"
    2,  // base 16: "ff"
    2,  // base 17: "f0"
    2,  // base 18: "e3"
    2,  // base 19: "d8"
    2,  // base 20: "cf"
    2,  // base 21: "c3"
    2,  // base 22: "bd"
    2,  // base 23: "b2"
    2,  // base 24: "af"
    2,  // base 25: "a5"
    2,  // base 26: "9l"
    2,  // base 27: "9c"
    2,  // base 28: "93"
    2,  // base 29: "8n"
    2,  // base 30: "8f"
    2,  // base 31: "87"
    2,  // base 32: "7v"
    2,  // base 33: "7o"
    2,  // base 34: "7h"
    2,  // base 35: "7a"
    2); // base 36: "73"

BSLALG_NUMERICFORMATTERUTIL_INTMAXLEN_ONE(true, 2,                  //  int16_t
    17,  // base  2: "-1000000000000000"
    11,  // base  3: "-1122221122"
     9,  // base  4: "-20000000"
     8,  // base  5: "-2022033"
     7,  // base  6: "-411412"
     7,  // base  7: "-164351"
     7,  // base  8: "-100000"
     6,  // base  9: "-48848"
     6,  // base 10: "-32768"
     6,  // base 11: "-2268a"
     6,  // base 12: "-16b68"
     6,  // base 13: "-11bb8"
     5,  // base 14: "-bd28"
     5,  // base 15: "-9a98"
     5,  // base 16: "-8000"
     5,  // base 17: "-6b69"
     5,  // base 18: "-5b28"
     5,  // base 19: "-4eec"
     5,  // base 20: "-41i8"
     5,  // base 21: "-3b68"
     5,  // base 22: "-31fa"
     5,  // base 23: "-2flg"
     5,  // base 24: "-28l8"
     5,  // base 25: "-22ai"
     5,  // base 26: "-1mc8"
     5,  // base 27: "-1hph"
     5,  // base 28: "-1dm8"
     5,  // base 29: "-19rr"
     5,  // base 30: "-16c8"
     5,  // base 31: "-1331"
     5,  // base 32: "-1000"
     4,  // base 33: "-u2w"
     4,  // base 34: "-sbq"
     4,  // base 35: "-qq8"
     4); // base 36: "-pa8"

BSLALG_NUMERICFORMATTERUTIL_INTMAXLEN_ONE(false, 2,                 // uint16_t
    16,  // base  2: "1111111111111111"
    11,  // base  3: "10022220020"
     8,  // base  4: "33333333"
     7,  // base  5: "4044120"
     7,  // base  6: "1223223"
     6,  // base  7: "362031"
     6,  // base  8: "177777"
     6,  // base  9: "108806"
     5,  // base 10: "65535"
     5,  // base 11: "45268"
     5,  // base 12: "31b13"
     5,  // base 13: "23aa2"
     5,  // base 14: "19c51"
     5,  // base 15: "14640"
     4,  // base 16: "ffff"
     4,  // base 17: "d5d0"
     4,  // base 18: "b44f"
     4,  // base 19: "9aa4"
     4,  // base 20: "83gf"
     4,  // base 21: "71cf"
     4,  // base 22: "638j"
     4,  // base 23: "58k8"
     4,  // base 24: "4hif"
     4,  // base 25: "44la"
     4,  // base 26: "3iof"
     4,  // base 27: "38o6"
     4,  // base 28: "2rgf"
     4,  // base 29: "2jqo"
     4,  // base 30: "2cof"
     4,  // base 31: "2661"
     4,  // base 32: "1vvv"
     4,  // base 33: "1r5u"
     4,  // base 34: "1mnh"
     4,  // base 35: "1ihf"
     4); // base 36: "1ekf"

BSLALG_NUMERICFORMATTERUTIL_INTMAXLEN_ONE(true, 4,                  //  int32_t
    33,  // base  2: "-10000000000000000000000000000000"
    21,  // base  3: "-12112122212110202102"
    17,  // base  4: "-2000000000000000"
    15,  // base  5: "-13344223434043"
    13,  // base  6: "-553032005532"
    13,  // base  7: "-104134211162"
    12,  // base  8: "-20000000000"
    11,  // base  9: "-5478773672"
    11,  // base 10: "-2147483648"
    10,  // base 11: "-a02220282"
    10,  // base 12: "-4bb2308a8"
    10,  // base 13: "-282ba4aab"
    10,  // base 14: "-1652ca932"
     9,  // base 15: "-c87e66b8"
     9,  // base 16: "-80000000"
     9,  // base 17: "-53g7f549"
     9,  // base 18: "-3928g3h2"
     9,  // base 19: "-27c57h33"
     9,  // base 20: "-1db1f928"
     9,  // base 21: "-140h2d92"
     8,  // base 22: "-ikf5bf2"
     8,  // base 23: "-ebelf96"
     8,  // base 24: "-b5gge58"
     8,  // base 25: "-8jmdnkn"
     8,  // base 26: "-6oj8ioo"
     8,  // base 27: "-5ehnckb"
     8,  // base 28: "-4clm98g"
     8,  // base 29: "-3hk7988"
     8,  // base 30: "-2sb6cs8"
     8,  // base 31: "-2d09uc2"
     8,  // base 32: "-2000000"
     8,  // base 33: "-1lsqtl2"
     8,  // base 34: "-1d8xqrq"
     8,  // base 35: "-15v22un"
     7); // base 36: "-zik0zk"

BSLALG_NUMERICFORMATTERUTIL_INTMAXLEN_ONE(false, 4,                 // uint32_t
    32,  // base  2: "11111111111111111111111111111111"
    21,  // base  3: "102002022201221111210"
    16,  // base  4: "3333333333333333"
    14,  // base  5: "32244002423140"
    13,  // base  6: "1550104015503"
    12,  // base  7: "211301422353"
    11,  // base  8: "37777777777"
    11,  // base  9: "12068657453"
    10,  // base 10: "4294967295"
    10,  // base 11: "1904440553"
     9,  // base 12: "9ba461593"
     9,  // base 13: "535a79888"
     9,  // base 14: "2ca5b7463"
     9,  // base 15: "1a20dcd80"
     8,  // base 16: "ffffffff"
     8,  // base 17: "a7ffda90"
     8,  // base 18: "704he7g3"
     8,  // base 19: "4f5aff65"
     8,  // base 20: "3723ai4f"
     8,  // base 21: "281d55i3"
     8,  // base 22: "1fj8b183"
     8,  // base 23: "1606k7ib"
     7,  // base 24: "mb994af"
     7,  // base 25: "hek2mgk"
     7,  // base 26: "dnchbnl"
     7,  // base 27: "b28jpdl"
     7,  // base 28: "8pfgih3"
     7,  // base 29: "76beigf"
     7,  // base 30: "5qmcpqf"
     7,  // base 31: "4q0jto3"
     7,  // base 32: "3vvvvvv"
     7,  // base 33: "3aokq93"
     7,  // base 34: "2qhxjlh"
     7,  // base 35: "2br45qa"
     7); // base 36: "1z141z3"

BSLALG_NUMERICFORMATTERUTIL_INTMAXLEN_ONE(true, 8,                  //  int64_t
    65,  // base  2: "-10000000000000000000....00000000000000000000"
    41,  // base  3: "-2021110011022210012102010021220101220222"
    33,  // base  4: "-20000000000000000000000000000000"
    29,  // base  5: "-1104332401304422434310311213"
    26,  // base  6: "-1540241003031030222122212"
    24,  // base  7: "-22341010611245052052301"
    23,  // base  8: "-1000000000000000000000"
    21,  // base  9: "-67404283172107811828"
    20,  // base 10: "-9223372036854775808"
    20,  // base 11: "-1728002635214590698"
    19,  // base 12: "-41a792678515120368"
    19,  // base 13: "-10b269549075433c38"
    18,  // base 14: "-4340724c6c71dc7a8"
    18,  // base 15: "-160e2ad3246366808"
    17,  // base 16: "-8000000000000000"
    17,  // base 17: "-33d3d8307b214009"
    17,  // base 18: "-16agh595df825fa8"
    16,  // base 19: "-ba643dci0ffeehi"
    16,  // base 20: "-5cbfjia3fh26ja8"
    16,  // base 21: "-2heiciiie82dh98"
    16,  // base 22: "-1adaibb21dckfa8"
    15,  // base 23: "-i6k448cf4192c3"
    15,  // base 24: "-acd772jnc9l0l8"
    15,  // base 25: "-64ie1focnn5g78"
    15,  // base 26: "-3igoecjbmca688"
    15,  // base 27: "-27c48l5b37oaoq"
    15,  // base 28: "-1bk39f3ah3dmq8"
    14,  // base 29: "-q1se8f0m04isc"
    14,  // base 30: "-hajppbc1fc208"
    14,  // base 31: "-bm03i95hia438"
    14,  // base 32: "-8000000000000"
    14,  // base 33: "-5hg4ck9jd4u38"
    14,  // base 34: "-3tdtk1v8j6tpq"
    14,  // base 35: "-2pijmikexrxp8"
    14); // base 36: "-1y2p0ij32e8e8"

BSLALG_NUMERICFORMATTERUTIL_INTMAXLEN_ONE(false, 8,                 // uint64_t
    64,  // base  2: "11111111111111111111....11111111111111111111"
    41,  // base  3: "11112220022122120101211020120210210211220"
    32,  // base  4: "33333333333333333333333333333333"
    28,  // base  5: "2214220303114400424121122430"
    25,  // base  6: "3520522010102100444244423"
    23,  // base  7: "45012021522523134134601"
    22,  // base  8: "1777777777777777777777"
    21,  // base  9: "145808576354216723756"
    20,  // base 10: "18446744073709551615"
    19,  // base 11: "335500516a429071284"
    18,  // base 12: "839365134a2a240713"
    18,  // base 13: "219505a9511a867b72"
    17,  // base 14: "8681049adb03db171"
    17,  // base 15: "2c1d56b648c6cd110"
    16,  // base 16: "ffffffffffffffff"
    16,  // base 17: "67979g60f5428010"
    16,  // base 18: "2d3fgb0b9cg4bd2f"
    16,  // base 19: "141c8786h1ccaagg"
    15,  // base 20: "b53bjh07be4dj0f"
    15,  // base 21: "5e8g4ggg7g56dif"
    15,  // base 22: "2l4lf104353j8kf"
    15,  // base 23: "1ddh88h2782i515"
    14,  // base 24: "l12ee5fn0ji1if"
    14,  // base 25: "c9c336o0mlb7ef"
    14,  // base 26: "7b7n2pcniokcgf"
    14,  // base 27: "4eo8hfam6fllmo"
    14,  // base 28: "2nc6j26l66rhof"
    14,  // base 29: "1n3rsh11f098rn"
    14,  // base 30: "14l9lkmo30o40f"
    13,  // base 31: "nd075ib45k86f"
    13,  // base 32: "fvvvvvvvvvvvv"
    13,  // base 33: "b1w8p7j5q9r6f"
    13,  // base 34: "7orp63sh4dphh"
    13,  // base 35: "5g24a25twkwff"
    13); // base 36: "3w5e11264sgsf"
// BDE_VERIFY pragma: pop  // End of macro-created template specializations

#undef BSLALG_NUMERICFORMATTERUTIL_INTMAXLEN_ONE

     // ==============================================================
     // template struct NumericFormatterUtil::IsSupportedFloatingPoint
     // ==============================================================

template <class TYPE>
struct NumericFormatterUtil::IsSupportedFloatingPoint {
  private:
    // PRIVATE TYPES

    /// For more readable lines below.
    typedef typename bsl::remove_cv<TYPE>::type NoCvT;
  public:
    // PUBLIC TYPES
    enum Enum {
        k_SUPPORTED = bsl::is_same<NoCvT, float>::value ||
                      bsl::is_same<NoCvT, double>::value
    };
};

       // ---------------------------------------------------------
       // template struct NumericFormatterUtil::IsSupportedIntegral
       // ---------------------------------------------------------

template <class TYPE>
struct NumericFormatterUtil::IsSupportedIntegral {
  private:
    // PRIVATE TYPES

    /// For more readable lines below.
    typedef typename bsl::remove_cv<TYPE>::type NoCvT;
  public:
    // PUBLIC TYPES
    enum Enum {
        k_SUPPORTED =
            bsl::is_same<NoCvT,          char          >::value ||
            bsl::is_same<NoCvT, signed   char          >::value ||
            bsl::is_same<NoCvT, unsigned char          >::value ||
            bsl::is_same<NoCvT, signed   short int     >::value ||
            bsl::is_same<NoCvT, unsigned short int     >::value ||
            bsl::is_same<NoCvT, signed   int           >::value ||
            bsl::is_same<NoCvT, unsigned int           >::value ||
            bsl::is_same<NoCvT, signed   long int      >::value ||
            bsl::is_same<NoCvT, unsigned long int      >::value ||
            bsl::is_same<NoCvT, signed   long long int >::value ||
            bsl::is_same<NoCvT, unsigned long long int >::value
    };
};

         // ------------------------------------------------------
         // template struct NumericFormatterUtil::ToCharsMaxLength
         // ------------------------------------------------------

template <class TYPE, int ARG>
struct NumericFormatterUtil::ToCharsMaxLength {
  private:
    // PRIVATE TYPES

    /// This type alias exists to make the lines below shorter, therefore
    /// easier to read at a glance.
    typedef const bool Cbool;

    // PRIVATE CONSTANTS
                              // Type Related

    static Cbool k_INTEGRAL_TYPE = IsSupportedIntegral<TYPE>::k_SUPPORTED;
    static Cbool k_FLOAT_TYPE    = IsSupportedFloatingPoint<TYPE>::k_SUPPORTED;

    static Cbool k_UNSUPPORTED_INPUT_TYPE = !k_INTEGRAL_TYPE && !k_FLOAT_TYPE;

                            // Argument Related

    static Cbool k_ARG_NOT_BASE   = (ARG < 2 || ARG > 36);
    static Cbool k_ARG_NOT_FORMAT = (ARG != e_FIXED && ARG != e_SCIENTIFIC &&
                                     ARG != e_GENERAL && ARG != e_HEX);

    static Cbool k_ARGUMENT_VALUE_IS_WRONG = !( // We negate the valid cases
                                                // for easier human consumption
          (ARG == k_MAXLEN_ARG_DEFAULT)                                      ||
          // The default value is always valid, regardless of the type

          (k_INTEGRAL_TYPE && !k_ARG_NOT_BASE)                               ||
          // Integral types with a valid 'base' value (2-36) are supported

          (k_FLOAT_TYPE && !k_ARG_NOT_FORMAT)                                ||
          // Floating point types with a valid 'format' are supported

          (k_UNSUPPORTED_INPUT_TYPE && !(k_ARG_NOT_BASE && k_ARG_NOT_FORMAT)));
          // In case of an unsupported type we accept any argument value that
          // would be supported for any valid (supported) type.  This way we
          // avoid burdening the user (programmer) with unhelpful extra
          // compiler error messages.

    // CONTRACT VERIFICATION
    BSLMF_ASSERT(false == k_UNSUPPORTED_INPUT_TYPE);
    BSLMF_ASSERT(false == k_ARGUMENT_VALUE_IS_WRONG);

  public:
    // PUBLIC TYPES
    enum ValueType {
        k_VALUE = k_INTEGRAL_TYPE
                  ? IntMaxLen<std::numeric_limits<TYPE>::is_signed,
                              static_cast<unsigned>(sizeof(TYPE)),
                              ARG>::k_VALUE
                  : k_FLOAT_TYPE
                  ? FltMaxLen<TYPE, ARG>::k_VALUE
                  : -1  // In case of bad 'TYPE' or 'ARG'
    };
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // ---------------------------
                       // struct NumericFormatterUtil
                       // ---------------------------

// PPRIVATE CLASS METHODS
template <class TYPE>
inline
char *NumericFormatterUtil::toCharsIntegral(char *first,
                                            char *last,
                                            TYPE  value,
                                            int   base) BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT_SAFE(2 <= base);
    BSLS_ASSERT_SAFE(base <= 36);
    BSLS_ASSERT_SAFE(first <= last);

    BSLMF_ASSERT(bsl::is_integral<TYPE>::value);
    BSLMF_ASSERT(sizeof(TYPE) <= sizeof(unsigned long long int));

    typedef typename bsl::conditional<(sizeof(unsigned) < sizeof(TYPE)),
                                      unsigned long long int,
                                      unsigned>::type VirtualUnsignedType;

    if (first == last) {
        // The ISO equivalent of this function allows empty ranges, so we shall
        // allow them, too.  The early return is necessary due to the sign
        // "trick" below.
        return 0;                                                     // RETURN
    }

    // Note that if 'value' is a negative value and 'TYPE' is smaller than
    // 'VirtualUnsignedType', assigning it here will extend the sign, even
    // though 'VirtualUnsignedType' is an unsigned type.

    VirtualUnsignedType uValue = value;

    if (value < 0) {
        uValue = ~uValue + 1;   // Absolute value -- note this works even for
                                // 'numeric_limits<TYPE>::min()'.
        *first++ = '-';
    }

    return toCharsImpl(first, last, uValue, base);
}

// PUBLIC CLASS METHODS
inline
char *NumericFormatterUtil::toChars(char *first,
                                    char *last,
                                    char  value,
                                    int   base) BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char        *first,
                                    char        *last,
                                    signed char  value,
                                    int          base) BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char          *first,
                                    char          *last,
                                    unsigned char  value,
                                    int            base) BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *
NumericFormatterUtil::toChars(char             *first,
                              char             *last,
                              signed short int  value,
                              int               base) BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char               *first,
                                    char               *last,
                                    unsigned short int  value,
                                    int                 base)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char       *first,
                                    char       *last,
                                    signed int  value,
                                    int         base) BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char         *first,
                                    char         *last,
                                    unsigned int  value,
                                    int           base) BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char            *first,
                                    char            *last,
                                    signed long int  value,
                                    int              base)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char              *first,
                                    char              *last,
                                    unsigned long int  value,
                                    int                base)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char                 *first,
                                    char                 *last,
                                    signed long long int  value,
                                    int                   base)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char                   *first,
                                    char                   *last,
                                    unsigned long long int  value,
                                    int                     base)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char   *first,
                                    char   *last,
                                    double  value,
                                    Format  format) BSLS_KEYWORD_NOEXCEPT
{
    switch (format) {
      case e_FIXED:      return toCharsDecimal(first, last, value);   // RETURN
      case e_SCIENTIFIC: return toCharsScientific(first, last, value);// RETURN
      case e_GENERAL:    return toCharsGeneral(first, last, value);   // RETURN
      case e_HEX:        return toCharsHex(first, last, value);       // RETURN
    }

    BSLS_ASSERT_INVOKE_NORETURN("Invalid 'format' argument value.");
    return 0;  // To avoid warning from AIX xlC
}

inline
char *NumericFormatterUtil::toChars(char   *first,
                                    char   *last,
                                    float   value,
                                    Format  format) BSLS_KEYWORD_NOEXCEPT
{
    switch (format) {
      case e_FIXED:      return toCharsDecimal(first, last, value);   // RETURN
      case e_SCIENTIFIC: return toCharsScientific(first, last, value);// RETURN
      case e_GENERAL:    return toCharsGeneral(first, last, value);   // RETURN
      case e_HEX:        return toCharsHex(first, last, value);       // RETURN
    }

    BSLS_ASSERT_INVOKE_NORETURN("Invalid 'format' argument value.");
    return 0;  // To avoid warning from AIX xlC
}

inline
char *NumericFormatterUtil::toChars(char   *first,
                                    char   *last,
                                    double  value,
                                    Format  format,
                                    int     precision) BSLS_KEYWORD_NOEXCEPT
{
    switch (format) {
      case e_FIXED: {
        return toCharsDecimalPrec(first, last, value, precision);     // RETURN
      }
      case e_SCIENTIFIC: {
        return toCharsScientificPrec(first, last, value, precision);  // RETURN
      }
      case e_GENERAL: {
        return toCharsGeneralPrec(first, last, value, precision);     // RETURN
      }
      case e_HEX: {
        return toCharsHexPrec(first, last, value, precision);         // RETURN
      }
    }

    BSLS_ASSERT_INVOKE_NORETURN("Invalid 'format' argument value.");
    return 0;  // To avoid warning from AIX xlC
}

inline
char *NumericFormatterUtil::toChars(char   *first,
                                    char   *last,
                                    float   value,
                                    Format  format,
                                    int     precision) BSLS_KEYWORD_NOEXCEPT
{
    switch (format) {
      case e_FIXED: {
        return toCharsDecimalPrec(first, last, value, precision);     // RETURN
      }
      case e_SCIENTIFIC: {
        return toCharsScientificPrec(first, last, value, precision);  // RETURN
      }
      case e_GENERAL: {
        return toCharsGeneralPrec(first, last, value, precision);     // RETURN
      }
      case e_HEX: {
        return toCharsHexPrec(first, last, value, precision);         // RETURN
      }
    }

    BSLS_ASSERT_INVOKE_NORETURN("Invalid 'format' argument value.");
    return 0;  // To avoid warning from AIX xlC
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
