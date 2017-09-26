// bdldfp_decimalformatconfig.h                                      -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALFORMATCONFIG
#define INCLUDED_BDLDFP_DECIMALFORMATCONFIG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide an attribute class to configure decimal formatting.
//
//@CLASSES:
//  bdltdfp::DecimalFormatConfig: configuration for format functions
//
//@SEE_ALSO: bdldfp_decimalutil
//
//@DESCRIPTION: This component provides an unconstrained (value-semantic)
// attribute class, 'bdldfp::DecimalFormatConfig', that is used to configure
// various aspects of decimal value formatting.
//
///Attributes
///----------
//..
//  Name        Type         Values            Default
//  ---------   ----------   ------------      -----------
//  style       enum Style   e_SCIENTIFIC      e_SCIENTIFIC
//                           e_FIXED
//                           e_NATURAL
//  precision   int          0..INT_MAX
//  sign        enum Sign    e_NEGATIVE_ONLY  e_NEGATIVE_ONLY
//                           e_ALWAYS
//  infinity    string                        "infinity"
//  nan         string                        "nan"
//  snan        string                        "snan"
//  point       char                          '.'
//  exponent    char                          'E'
//..
//
//: o 'style': control how the decimal number is written.  If 'style' is
//:   'e_SCIENTIFIC', the number is written as its sign, then a single digit,
//:   then the decimal point, then 'precision' digits, then the 'exponent'
//:   character, then a '-' or '+', then an exponent with no leading zeroes
//:   (with a zero exponent written as '0').  If 'style' is 'e_FIXED', the
//:   number is written as its sign, then one or more digits, then the decimal
//:   point, then 'precision' digits.  If the 'precision' value equals '0' then
//:   'precision' digits and the decimal point are not written.  If 'style' is
//:   'e_NATURAL', the number is written according to the description of
//:   'to-scientific-string' found in
//:   {http://speleotrove.com/decimal/decarith.pdf}.
//:
//: o 'precision': control how many digits are written after the decimal point.
//:
//: o 'sign': control how the sign is output.  If a decimal value has its sign
//:   bit set, a '-' is always written.  Otherwise, if 'sign' is
//:   'e_NEGATIVE_ONLY', no sign is written.  If it is 'e_ALWAYS', a '+' is
//:   written.
//:
//: o 'infinity': specify a string to output infinity value.
//:
//: o 'nan': specify a string to output NaN value.
//:
//: o 'snan': specify a string to output signaling NaN value.
//:
//: o 'point': specify the character to use for decimal points.
//:
//: o 'exponent': specify the character to use for exponent when 'style' is
//:   'e_SCIENTIFIC' or 'e_NATURAL'.

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bdldfp {

                        // =========================
                        // class DecimalFormatConfig
                        // =========================

class DecimalFormatConfig {
    // This attribute class characterizes how to configure certain behavior
    // of 'bdldfp::DecimalUtil::format' functions.

  public:
    enum Sign {
        e_NEGATIVE_ONLY,  // no sign output when sign bit is not set
        e_ALWAYS,         // output '+' when sign bit is not set
    };

    enum Style {
        e_SCIENTIFIC,     // output number in scientific notation
        e_FIXED,          // output number in fixed-format
        e_NATURAL         // output number in "to-scientific-string" format
                          // described in
                          // {http://speleotrove.com/decimal/decarith.pdf}
    };

  private:
    // DATA
    int         d_precision;     // precision (number of digits after point)
    Style       d_style;         // formatting style
    Sign        d_sign;          // sign character
    const char *d_infinityText;  // infinity representation
    const char *d_nanText;       // NaN representation
    const char *d_sNanText;      // signaling NaN representation
    char        d_decimalPoint;  // decimal point character
    char        d_exponent;      // exponent character

  public:
    // CREATORS
    explicit
    DecimalFormatConfig(int         precision,
                        Style       style     = e_SCIENTIFIC,
                        Sign        sign      = e_NEGATIVE_ONLY,
                        const char *infinity  = "inf",
                        const char *nan       = "nan",
                        const char *snan      = "snan",
                        char        point     = '.',
                        char        exponent  = 'E');
        // Create an object of this class havig the specified 'precision' to
        // control how many digits are written after a decimal point.  The
        // behavior is undefined if 'precision' is negative.  Optionally
        // specify 'style' to control how the number is written.  If it is not
        // specified, 'e_SCIENTIFIC' is used.  Optionally specify 'sign' to
        // control how the sign is output.  If is not specified,
        // 'e_NEGATIVE_ONLY' is used.  Optionally specify 'inf' as a string to
        // output infinity value.  If it is not specified, "inf" is used.
        // Optionally specify 'nan' as a string to output NaN value.  If it is
        // not specified, "nan" is used.  Optionally specify 'snan' as a string
        // to output signaling NaN value.  If it is not specified, "snan" is
        // used.  Optionally specify 'point' as the character to use for
        // decimal points.  If it is not specified, '.' is used.  Optionally
        // specify 'exponent' as the character to use for exponent.  If it is
        // not specified, 'E' is used.  See the Attributes section under
        // @DESCRIPTION in the component-level documentation for information on
        // the class attributes.

    // ACCESSORS
    int precision() const;
        // Return the number of digits of precision in the outputs.

    Style style() const;
        // Return the style of output format.

    Sign sign() const;
        // Return the sign attribute.

    const char *infinity() const;
        // Return infinity string representation.

    const char *nan() const;
        // Return NaN string representation.

    const char *sNan() const;
        // Return sNaN string representation.

    char decimalPoint() const;
        // Return point character.

    char exponent() const;
        // Return exponent character.
};


// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // -------------------------
                          // class DecimalFormatConfig
                          // -------------------------

// CREATORS
inline
DecimalFormatConfig::DecimalFormatConfig(int         precision,
                                         Style       style,
                                         Sign        sign,
                                         const char *infinity,
                                         const char *nan,
                                         const char *snan,
                                         char        point,
                                         char        exponent)
    : d_precision(precision)
    , d_style(style)
    , d_sign(sign)
    , d_infinityText(infinity)
    , d_nanText(nan)
    , d_sNanText(snan)
    , d_decimalPoint(point)
    , d_exponent(exponent)
{
    BSLS_ASSERT(precision >= 0);
}

// ACCESSORS
inline
int DecimalFormatConfig::precision() const
{
    return d_precision;
}

inline
DecimalFormatConfig::Style DecimalFormatConfig::style() const
{
    return d_style;
}

inline
DecimalFormatConfig::Sign DecimalFormatConfig::sign() const
{
    return d_sign;
}

inline
const char *DecimalFormatConfig::infinity() const
{
    return d_infinityText;
}

inline
const char *DecimalFormatConfig::nan() const
{
    return d_nanText;
}

inline
const char *DecimalFormatConfig::sNan() const
{
    return d_sNanText;
}

inline
char DecimalFormatConfig::decimalPoint() const
{
    return d_decimalPoint;
}

inline
char DecimalFormatConfig::exponent() const
{
    return d_exponent;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
