// bslalg_numericformatterutil.h                                      -*-C++-*-
#ifndef INCLUDED_BSLALG_NUMERICFORMATTERUTIL
#define INCLUDED_BSLALG_NUMERICFORMATTERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for formatting numbers into strings.
//
//@CLASSES:
//  bslalg::NumericFormatterUtil: namespace for 'toChars' and support functions
//
//@DESCRIPTION: This component, 'bslalg_numericformatterutil' provides a
// namespace 'struct', 'bslalg::NumericFormatterUtil', containing the template
// function 'toChars', that translates integral fundamental types into ASCII
// strings.
//
///Shortest (Textual) Decimal Representation for Binary Floating Point Values
///--------------------------------------------------------------------------
// The floating point 'toChars' implementations (for 'float' and 'double') of
// this component provide the shortest (textual) decimal representation that
// can (later) be parsed back to the original binary value (i.e., a
// "round-trip" conversion).  Such round-tripping enables precise, and
// human-friendly (textual) communication protocols, and storage formats that
// use minimal necessary bandwidth or storage.
//
// Scientific notation, when chosen, always uses the minimum number of
// fractional digits necessary to restore the exact binary floating point
// value.  The shortest *decimal* notation of a binary floating point number is
// text that has enough decimal !fractional! digits so that there can be no
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
// In short, IEEE-754 double precision binary floating point numbers ('double')
// are guaranteed to round-trip when represented by 17 significant decimal
// digits, while single precisions ('float') needs 9 digits.  However those
// numbers are the *maximum* decimal digits that *may* be necessary, and in
// fact many values can be precisely represented precisely by less.  'toChars'
// renders the minimum number of digits needed, so that the value can later be
// restored.
//
///Default Floating Point Format
///-----------------------------
// The default floating point format (that is used when no 'format' argument is
// present in the signature) uses the shortest representation from the decimal
// notation and the scientific notation, favoring decimal notation in case of a
// tie.
//
///Special Floating Point Values
///-----------------------------
// Floating point values may also be special-numerical or non-numerical(*)
// values in addition to what we consider normal numbers.
//
// The special numerical value is really just one, and that is negative zero.
//
// For non-numerical special value both IEEE-754 and W3C XML Schema Definition
// Language (XSD) 1.1(**) 'numericalSpecialRep' requires there to be three
// distinct values supported: positive infinity, negative infinity, and NaN.
// We represent those values according to the XSD lexical mapping
// specification.  That also means that these values will round trip in text
// *only* if the reader algorithm recognizes those representations.
//
//..
// +-------------------+----------------+
// | Special Value     | Textual Repr.  |
// +-------------------+----------------+
// | positive zero     |  "0",  "0e+00" |
// +-------------------+----------------+
// | negative zero     | "-0", "-0e+00" |
// +-------------------+----------------+
// | positive infinity | "+INF"         |
// +-------------------+----------------+
// | negative infinity | "-INF"         |
// +-------------------+----------------+
// | Not-a-number      | "NaN"          |
// +-------------------+----------------+
//..
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
///Example 1: Writing an Integer to a 'streambuf'
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a function that writes an 'int' to a 'streambuf'.
// We can use 'bsl::to_chars' to write the 'int' to a buffer, then write the
// buffer to the 'streambuf'.
//
// First, we declare our function:
//..
//  void writeJsonScalar(std::streambuf *result, int value)
//      // Write the specified 'value', in decimal, to the specified 'result'.
//  {
//..
// Then, we declare a buffer long enough to store any 'int' value in decimal.
//..
//      char buffer[11];        // size large enough to write 'INT_MIN', the
//                              // worst-case value, in decimal.
//..
// Next, we call the function:
//..
//      char *ret = bslalg::NumericFormatterUtil::toChars(
//                                                      buffer,
//                                                      buffer + sizeof buffer,
//                                                      value);
//..
// Then, we check that the buffer was long enough, which should always be the
// case:
//..
//      assert(0 != ret);
//..
// Now, we write our buffer to the 'streambuf':
//..
//      result->sputn(buffer, ret - buffer);
//  }
//..
// Finally, we use an output string stream buffer to exercise the
// 'writeJsonScalar' function for 'int':
//..
//  std::ostringstream  oss;
//  std::streambuf* sb = oss.rdbuf();
//
//  writeJsonScalar(sb, 0);
//  assert("0" == oss.str());
//
//  oss.str("");
//  writeJsonScalar(sb, 99);
//  assert("99" == oss.str());
//
//  oss.str("");
//  writeJsonScalar(sb, -1234567890);  // worst case: max string length
//  assert("-1234567890" == oss.str());
//..
//
///Example 2: Writing the Minimal Form of a 'double'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to store a floating point number using decimal text (such as
// JSON) for later retrieval, using the minimum number of digits that ensures
// we can later restore the same binary floating point value.
//
// First, we declare our writer function:
//..
//  void writeJsonScalar(std::streambuf *result,
//                       double          value,
//                       bool            stringNonNumericValues = false)
//      // Write the specified 'value' in the shortest round-trip decimal
//      // format into the specified 'result'.  Write non-numeric values
//      // according to the optionally specified 'stringNonNumericValues'
//      // either as strings "NaN", "+Infinity", or "-Infinity" when
//      // 'stringNonNumericValues' is 'true', or a null when it is 'false' or
//      // not specified.
//  {
//..
// Then, we handle non-numeric values ('toChars' would write them the XSD way):
//..
//      if (isnan(value) || isinf(value)) {
//          if (false == stringNonNumericValues) {  // JSON standard output
//              result->sputn("null", 4);
//          }
//          else {                                  // Frequent JSON extension
//              if (isnan(value)) {
//                  result->sputn("\"NaN\"", 5);
//              }
//              else if (isinf(value)) {
//                  result->sputn(value < 0 ? "\"-" : "\"+", 2);
//                  result->sputn("Infinity\"", 9);
//              }
//          }
//          return;                                                   // RETURN
//      }
//      //..
// Next, we declare a buffer long enough to store any 'double' value written in
// this minimal-length form:
//..
//      char buffer[24];   // large enough to write the longest 'double'
//                         // without a null terminator character.
//..
// Then, we call the function:
//..
//      char *ret = bslalg::NumericFormatterUtil::toChars(
//                                                      buffer,
//                                                      buffer + sizeof buffer,
//                                                      value);
//..
// Finally, we can write our buffer to the 'streambuf':
//..
//      result->sputn(buffer, ret - buffer);
//  }
//..
// Finally, we use the output string stream buffer defined earlier to exercise
// the floating point 'writeJsonScalar' function:
//..
//  oss.str("");
//  writeJsonScalar(sb, 20211017.0);
//  assert("20211017" == oss.str());
//
//  oss.str("");
//  writeJsonScalar(sb, 3.1415926535897932);
//  assert("3.141592653589793" == oss.str());
//
//  oss.str("");
//  writeJsonScalar(sb, 2e5);
//  assert("2e+05" == oss.str());
//
//  oss.str("");                  // Non-numeric are written as null by default
//  writeJsonScalar(sb, std::numeric_limits<double>::quiet_NaN());
//  assert("null" == oss.str());  oss.str("");
//
//  oss.str("");                  // Non-numeric can be printed as strings
//  writeJsonScalar(sb, std::numeric_limits<double>::quiet_NaN(), true);
//  assert("\"NaN\"" == oss.str());  oss.str("");
//..

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_removecv.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace bslalg {
                        // ===========================
                        // struct NumericFormatterUtil
                        // ===========================

struct NumericFormatterUtil {
    // Namespace 'struct' for free functions supporting 'to_chars'.

  private:
    // PRIVATE CLASS METHODS
    static char *toCharsImpl(char     *first,
                             char     *last,
                             unsigned  value,
                             int       base) BSLS_KEYWORD_NOEXCEPT;
        // Write the specified 'value' into the character buffer starting a the
        // specified 'first' and ending at the specified 'last', rendering the
        // value in the specified base 'base'.  On success, return a the
        // address one past the lowest order digit written, on failure, return
        // 0.  The only reason for failure is if the range '[ first, last )' is
        // not large enough to contain the result.  The written result is to
        // begin at 'first' with leftover room following the return value.  The
        // behavior is undefined unless 'first <= last' and 'base' is in the
        // range '[ 2 .. 36 ]'.

    static char *toCharsImpl(char                *first,
                             char                *last,
                             bsls::Types::Uint64  value,
                             int                  base) BSLS_KEYWORD_NOEXCEPT;
        // Write the specified 'value' into the character buffer starting a the
        // specified 'first' and ending at the specified 'last', ing the value
        // in the specified base 'base'.  On success, return a the address one
        // past the lowest order digit written, on failure, return 0.  The only
        // reason for failure is if the range '[ first, last )' is not large
        // enough to contain the result.  The written result is to begin at
        // 'first' with leftover room following the return value.  The behavior
        // is undefined unless 'first <= last' and 'base' is in the range
        // '[ 2 .. 36 ]'.

    template <class TYPE>
    static char *toCharsIntegral(char *first,
                                 char *last,
                                 TYPE  value,
                                 int   base) BSLS_KEYWORD_NOEXCEPT;
        // Write the textual representation of the specified 'value' in the
        // specified 'base' into the character buffer starting a the specified
        // 'first' and ending at the specified 'last'.  Return the address one
        // past the lowest order digit written on success, or 0 on failure.
        // The only possible reason for failure is if the range
        // '[ first, last )' is not large enough to contain the result.  The
        // written result is to begin at 'first' with leftover room following
        // the return value.  The behavior is undefined unless 'first < last'
        // and 'base' is in the range '[ 2 .. 36 ]'.  The behavior is also
        // undefined unless the specified 'TYPE' is a fundamental integral type
        // not larger than 64 bits.

    static char *toCharsDecimal(char   *first,
                                char   *last,
                                double  value) BSLS_KEYWORD_NOEXCEPT;
    static char *toCharsDecimal(char   *first,
                                char   *last,
                                float   value) BSLS_KEYWORD_NOEXCEPT;
        // Write the textual representation of the specified 'value' in decimal
        // notation into the character buffer starting a the specified 'first'
        // and ending at the specified 'last'.  Return the address one past the
        // lowest order digit written on success, or 0 on failure.  The only
        // possible reason for failure is if the range '[ first, last )' is not
        // large enough to contain the result.  The written result is to begin
        // at 'first' with leftover room following the return value.

    static char *toCharsScientific(char   *first,
                                   char   *last,
                                   double  value) BSLS_KEYWORD_NOEXCEPT;
    static char *toCharsScientific(char   *first,
                                   char   *last,
                                   float   value) BSLS_KEYWORD_NOEXCEPT;
        // Write the textual representation of the specified 'value' in
        // scientific notation into the character buffer starting a the
        // specified 'first' and ending at the specified 'last'.  Return the
        // address one past the lowest order digit of the exponent written on
        // success, or 0 on failure.  The only possible reason for failure is
        // if the range '[ first, last )' is not large enough to contain the
        // result.  The written result is to begin at 'first' with leftover
        // room following the return value.

  private:
    // NOT IMPLEMENTED
    static char *toChars(char*, char*, bool, int = 10) BSLS_KEYWORD_NOEXCEPT
                                                       BSLS_KEYWORD_DELETED;

  public:
    // PUBLIC TYPES
    enum Format {
        // Enumeration that specifies the textual formatting of floating point
        // values.
        e_SCIENTIFIC = 0x01,
        e_FIXED      = 0x02
        //e_HEX        = 0x04,                         -- currently unsupported
        //e_GENERAL    = e_SCIENTIFIC | e_FIXED        -- ditto
    };

    // PUBLIC CLASS METHODS
    static char *toChars(char                *first,
                         char                *last,
                         char                 value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         signed char          value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         unsigned char        value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         short                value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         unsigned short       value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         int                  value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         unsigned int         value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         long                 value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         unsigned long        value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         bsls::Types::Int64   value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         bsls::Types::Uint64  value,
                         int                  base = 10) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         double               value) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         float                value) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         double               value,
                         Format               format) BSLS_KEYWORD_NOEXCEPT;
    static char *toChars(char                *first,
                         char                *last,
                         float                value,
                         Format               format) BSLS_KEYWORD_NOEXCEPT;
        // Write the specified 'value' into the character buffer starting a the
        // specified 'first' and ending at the specified 'last', 'last' not
        // included.  In integer conversions, if the optionally specified
        // 'base' argument is not present or specified, base 10 is used. In
        // floating point conversions, if the optionally specified 'format'
        // argument is not present or specified, the {Default Floating Point
        // Format} is used.  If a 'format' argument is specified ('e_DECIMAL'
        // or 'e_SCIENTIFIC'), the {Shortest (Textual) Decimal Representation
        // for Binary Floating Point Values} is used in that format (that will
        // produce the exact binary floating point 'value' when converted back
        // to the original type from text), but see possible exceptions under
        // {Special Floating Point Values}.  Return the address one past the
        // last character (lowest order digit or last digit of the exponent)
        // written on success, or '0' on failure.  The only reason for failure
        // is when the range '[ first, last )' is not large enough to contain
        // the result.  The written result is to begin at 'first' with leftover
        // room following the return value.  The behavior is undefined unless
        // 'first <= last', and 'base' is in the range '[ 2 .. 36 ]'.  Note
        // that the type 'bool' for the 'value' parameter is explicitly
        // disabled in the "NOT IMPLEMENTED" 'private' section, because 'bool'
        // would otherwise be promoted to 'int' and printed as '0' or '1',
        // instead of the (possibly) expected 'false' and 'true'; and 'bool'
        // is not an integral or numeric type either.  Also note that these
        // functions do !not! null-terminate the result.
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------------
                        // struct 'NumericFormatterUtil'
                        // -----------------------------

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
    BSLMF_ASSERT(sizeof(TYPE) <= sizeof(bsls::Types::Uint64));

    typedef typename bsl::conditional<(sizeof(unsigned) < sizeof(TYPE)),
                                      bsls::Types::Uint64,
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
char *NumericFormatterUtil::toChars(char  *first,
                                    char  *last,
                                    short  value,
                                    int    base) BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char           *first,
                                    char           *last,
                                    unsigned short  value,
                                    int             base) BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char *first,
                                    char *last,
                                    int   value,
                                    int   base) BSLS_KEYWORD_NOEXCEPT
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
char *NumericFormatterUtil::toChars(char *first,
                                    char *last,
                                    long  value,
                                    int   base) BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char          *first,
                                    char          *last,
                                    unsigned long  value,
                                    int            base) BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char               *first,
                                    char               *last,
                                    bsls::Types::Int64  value,
                                    int                 base)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return toCharsIntegral(first, last, value, base);
}

inline
char *NumericFormatterUtil::toChars(char                *first,
                                    char                *last,
                                    bsls::Types::Uint64  value,
                                    int                  base)
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
