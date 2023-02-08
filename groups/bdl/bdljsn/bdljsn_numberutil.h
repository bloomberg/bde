// bdljsn_numberutil.h                                                -*-C++-*-
#ifndef INCLUDED_BDLJSN_NUMBERUTIL
#define INCLUDED_BDLJSN_NUMBERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities converting between JSON text and numeric types.
//
//@CLASSES:
//  bdljsn::NumberUtil: conversion between JSON text and numeric types
//
//@DESCRIPTION: This component provides a struct, 'bdljsn::NumberUtil', that is
// a namespace for a suite of functions for working with the JSON number text
// format.  'bdljsn::NumberUtil' provides a function 'isValidNumber' to
// determine whether a string is a valid JSON number.  Many of the other
// operations in this component have, as a precondition, that 'isValidNumber'
// is 'true' for the text.  For information about the JSON number specification
// and additional background on the behavior of numbers in 'bdljsn' see
// {bdljsn_jsonnumber}.
//
// Many of the operations in this component have 'isValidNumber' as a
// precondition in order to provide simpler and more efficient implementations.
// In the context of 'JsonNumber', the text will always be validated prior to
// performing other operations.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Interpretting a JSON Number String
///- - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using 'bdljsn::NumberUtil' to work with a JSON
// number string.  Imagine we are given and array of strings for numbers we
// expect to be integers, for each string we want to render some properties for
// that number.
//
// First, we define an interesting set of example data:
//..
//  const char *EXAMPLE_DATA[] = {
//     // value                               coverted int value & notes
//     // -----                               --------------------------
//     "NaN",                                // invalid number
//     "INF",                                // invalid number
//     "1",                                  // 1,         exact
//     "1.5",                                // 1,         not an integer
//     "-9223372036854775809",               // INT64_MIN, underflow
//     "1.5e27",                             // INT64_MAX, overflow
//  };
//  const int NUM_DATA = sizeof(EXAMPLE_DATA) / sizeof(*EXAMPLE_DATA);
//..
// Then, for each number, we first check whether it is a valid JSON Number
// (note that the behavior for the other methods is undefined unless the text
// is a valid JSON Number):
//..
//  for (int i = 0; i < NUM_DATA; ++i) {
//      const char *EXAMPLE = EXAMPLE_DATA[i];
//      bsl::cout << "\"" << EXAMPLE << "\": " << bsl::endl;
//      if (!bdljsn::NumberUtil::isValidNumber(EXAMPLE)) {
//          bsl::cout << "  * is NOT a JSON Number" << bsl::endl;
//          continue;                                               // CONTINUE
//      }
//..
// Next we verify that the number is an integer.  This will return an accurate
// result even when the integer cannot be represented.
//..
//      if (bdljsn::NumberUtil::isIntegralNumber(EXAMPLE)) {
//          bsl::cout << "  * is an integer" << bsl::endl;
//      }
//      else {
//          bsl::cout << "  * is not an integer" << bsl::endl;
//      }
//..
// Finally, we convert that number to an integer:
//..
//      bsls::Types::Int64 value;
//      int rc = bdljsn::NumberUtil::asInt64(&value, EXAMPLE);
//
//      bsl::cout << "  * value: " << value;
//
//      if (bdljsn::NumberUtil::k_NOT_INTEGRAL == rc) {
//          bsl::cout << "  (truncated)";
//      }
//      if (bdljsn::NumberUtil::k_OVERFLOW == rc) {
//          bsl::cout << "  (overflow)";
//      }
//      if (bdljsn::NumberUtil::k_UNDERFLOW == rc) {
//          bsl::cout << "  (underflow)";
//      }
//      bsl::cout << bsl::endl;
//  }
//..
// This will output the text:
//..
//  "NaN":
//    * is NOT a JSON Number
//  "INF":
//    * is NOT a JSON Number
//  "1":
//    * is an integer
//    * value: 1
//  "1.5":
//    * is not an integer
//    * value: 1  (truncated)
//  "-9223372036854775809":
//    * is an integer
//    * value: -9223372036854775808  (underflow)
//  "1.5e27":
//    * is an integer
//    * value: 9223372036854775807  (overflow)
//..

#include <bdlscm_version.h>

#include <bdlb_float.h>
#include <bdlb_numericparseutil.h>
#include <bdldfp_decimal.h>
#include <bdldfp_decimalimputil.h>

#include <bsla_nodiscard.h>
#include <bslmf_assert.h>
#include <bslmf_selecttrait.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_types.h>

#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_type_traits.h>
#include <bsl_iostream.h>

namespace BloombergLP {
namespace bdljsn {

struct NumberUtil_ImpUtil;

                           // =================
                           // struct NumberUtil
                           // =================

struct NumberUtil {
    // This 'struct' provides a namespace for a suite of functions that convert
    // between a JSON formated numeric value and various numerical types.  The
    // valid syntax for a JSON formatted numeric value is spelled out in
    // 'https://www.rfc-editor.org/rfc/rfc8259#section-6'.

    // PUBLIC TYPES
    typedef bsls::Types::Uint64 Uint64;
    typedef bsls::Types::Int64  Int64;

    // PUBLIC CONSTANTS
    enum {
        // special integer conversion status values
        k_OVERFLOW     = -1,  // the number is above the representable range
        k_UNDERFLOW    = -2,  // the number is below the representable range
        k_NOT_INTEGRAL = -3,  // the number is not an integer

        // special exact Decimal64 conversion status values
        k_INEXACT = -4
    };

    // CLASS METHODS

           // validation

    static bool isIntegralNumber(const bsl::string_view& value);
        // Return 'true' if the specified 'value' is a valid integral JSON
        // number.  Note that this function may return 'true' even if 'value'
        // cannot be represented in a fundamental integral type.  The behavior
        // is undefined unless 'isValidNumber(value)' is 'true'.

    static bool isValidNumber(const bsl::string_view& value);
        // Return 'true' if the specified 'value' is a valid JSON number.  Note
        // that this function may return 'true' even if 'value' cannot be
        // represented in any particular number type.

           // basic floating point conversions

    static bdldfp::Decimal64 asDecimal64(const bsl::string_view& value);
    static double asDouble(const bsl::string_view& value);
    static float asFloat(const bsl::string_view& value);
        // Return the closest floating point representation to the specified
        // 'value'.  If 'value' is outside the representable range, return +INF
        // or -INF (as appropriate).  The behavior is undefined unless
        // 'isValidNumber(value)' is 'true'.

           // exact floint point conversions

    static int asDecimal64Exact(bdldfp::Decimal64       *result,
                                const bsl::string_view&  value);
        // Load the specified 'result' with the specified 'value', even if a
        // non-zero status is returned.  Return 0 if 'value' can be represented
        // exactly, and return 'k_INEXACT' and load 'result' with the closest
        // approximation of 'value' if 'value' cannot be represented exactly.
        // A 'value' can be represented exactly as a 'Decimal64' if, for the
        // significand and exponent of 'value',
        // 'abs(significand) <= 9,999,999,999,999,999' and
        // '-398 <= exponent <= 369'.  The behavior is undefined unless
        // 'isValidNumber(value)' is 'true'.

           // typed integer conversions

    static int asInt(int *result, const bsl::string_view& value);
    static int asInt64(Int64 *result, const bsl::string_view& value);
    static int asUint(unsigned int *result, const bsl::string_view& value);
    static int asUint64(Uint64 *result, const bsl::string_view& value);
        // Load the specified 'result' with the specified 'value', even if a
        // non-zero status is returned (truncating fractional digits if
        // necessary).  Return 0 on success, 'k_OVERFLOW' if 'value' is larger
        // than can be represented by 'result', 'k_UNDERFLOW' if 'value' is
        // smaller than can be represented by 'result',  and 'k_NOT_INTEGRAL'
        // if 'value' is not an integral number (i.e., there is a fractional
        // part).  For underflow, 'result' will be loaded with the minimum
        // representable value, for overflow, 'result' will be loaded with the
        // maximum representable value, for non-integral values 'result' will
        // be loaded with the integer part of 'value' (truncating the
        // fractional part of 'value').  The behavior is undefined unless
        // 'isValidNumber(value)' is 'true'.  Note that this operation will
        // correctly handle exponents (e.g., a 'value' of
        // "0.00000000000000000001e20" will produce a 'result' of
        // 1).

            // generic integer conversion

    template <class t_INTEGER_TYPE>
    static int asInteger(t_INTEGER_TYPE          *result,
                         const bsl::string_view&  value);
        // Load into the specified 'result' (of the template parameter type
        // 't_INTEGER_TYPE') with the specified 'value', even if a non-zero
        // status is returned (truncating fractional digits if necesssary).
        // Return 0 on success, 'k_OVERFLOW' if 'value' is larger than can be
        // represented by 'result', 'k_UNDERFLOW' if 'value' is smaller than
        // can be represented by 'result',  and 'k_NOT_INTEGRAL' if 'value' is
        // not an integral number (i.e., there is a fractional part).  For
        // underflow, 'result' will be loaded with the minimum representable
        // value, for overflow, 'result' will be loaded with the maximum
        // representable value, for non-integral values 'result' will be loaded
        // with the integer part of 'value' (truncating the value to the
        // nearest integer).  If the result is not an integer and also either
        // overflows or underflows, it is treated as an overflow or underflow
        // (respectively).  The (template parameter) 't_INTEGER_TYPE' shall be
        // either a signed or unsigned integer type (that is not 'bool') where
        // 'sizeof(t_INTEGER_TYPE) <= 8'. The behavior is undefined unless
        // 'isValidNumber(value)' is 'true'.  Note that this operation will
        // correctly handle exponents (e.g., a 'value' of
        // "0.00000000000000000001e20" will produce a 'result' of
        // 1).

           // conversions to string

    static void stringify(bsl::string *result, Int64 value);
    static void stringify(bsl::string *result, Uint64 value);
    static void stringify(bsl::string *result, double value);
    static void stringify(bsl::string *result, const bdldfp::Decimal64& value);
        // Load into the specified 'result' a string representation of
        // specified numerical 'value'.

           // comparison

    static bool areEqual(const bsl::string_view& lhs,
                         const bsl::string_view& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' represent the same
        // numeric value, and 'false' otherwise.  This function will return
        // 'true' for differing representations of the same number (e.g.,
        // '1.0', "1", "0.1e+1" are all equivalent) *except* in cases where the
        // exponent cannot be represented by a 64-bit integer.  If the exponent
        // is outside the range of a 64-bit integer, 'true' will be returned if
        // 'lhs == rhs'.  For example, comparing "1e18446744073709551615" with
        // itself will return 'true', but comparing it to
        // "10e18446744073709551614" will return 'false'.  The behavior is
        // undefined unless 'isValidNumber(lhs)' and 'isValidNumber(rhs)'.
};

                         // ==========================
                         // struct NumberUtil_IsSigned
                         // ==========================

template <class t_TYPE>
struct NumberUtil_IsSigned
: bsl::integral_constant<bool, (t_TYPE(-1) < t_TYPE(0))> {
    // This class will be a 'bsl::true_type' if the specified (template
    // parameter type) 't_TYPE' is a signed type, and 'bsl::false_type'
    // otherwise.  't_TYPE' shall be an integral type.  Note that currently
    // bsl::is_signed is not available for C++03 platforms.
};

                         // =========================
                         // struct NumberUtil_ImpUtil
                         // =========================

struct NumberUtil_ImpUtil {
    // [!PRIVATE!] This private implementation 'struct' provides a namespace
    // for a suite of functions used to help implement 'NumberUtil'.  These
    // functions are private to this component and should not be used by
    // clients.

    // PUBLIC CONSTANTS
    enum {
        k_EXPONENT_OUT_OF_RANGE = -1  // exponent is out of a supported range
    };

    // CLASS METHODS
    static int appendDigits(bsls::Types::Uint64     *result,
                            bsls::Types::Uint64      startingValue,
                            const bsl::string_view&  digits);
        // Load the specified 'result' by appending the specified 'digits' to
        // the specified 'startingValue'.  Return 0 on success,
        // 'NumberUtil::k_OVERFLOW' if the result cannot be represented in a
        // 'Uint64'.  For example, appending "345" to 12 will return a 'result'
        // of 12345.

    static int asInteger(bsls::Types::Uint64     *result,
                         const bsl::string_view&  value);
    template <class t_INTEGER_TYPE>
    static int asInteger(t_INTEGER_TYPE          *result,
                         const bsl::string_view&  value);
        // These function overloads implement 'NumberUtil::asInteger', and are
        // documented there.

    template <class t_INTEGER_TYPE>
    static int asIntegerDispatchImp(
                           t_INTEGER_TYPE                    *result,
                           const bsl::string_view&            value,
                           bslmf::SelectTraitCase<NumberUtil_IsSigned>);
    template <class t_INTEGER_TYPE>
    static int asIntegerDispatchImp(t_INTEGER_TYPE           *result,
                                    const bsl::string_view&   value,
                                    bslmf::SelectTraitCase<>);
        // These functions are the template dispatched implementations for the
        // 'NumberUtil_ImpUtil::asInteger' template function, and serve
        // distinguish the signed from the (default-case) unsigned
        // implementation.  These functions are documented by
        // 'NumberUtil::asInteger'.

    static void decompose(
                      bool                             *isNegative,
                      bsl::string_view::const_iterator *integerBegin,
                      bsl::string_view::const_iterator *integerEnd,
                      bsl::string_view::const_iterator *fractionBegin,
                      bsl::string_view::const_iterator *fractionEnd,
                      bool                             *isExponentNegative,
                      bsl::string_view::const_iterator *exponentBegin,
                      bsl::string_view::const_iterator *significantDigitsBegin,
                      bsl::string_view::const_iterator *significantDigitsEnd,
                      bsls::Types::Int64               *significantDigitsBias,
                      const bsl::string_view&           value);
        // Decompose the specified 'value' into constituent elements, loading
        // the specified 'isNegative' with a flag indicating if 'value' is
        // negative, the specified 'integerBegin' and 'integerEnd' with the
        // beginning and end of the integer portion of 'value', the specified
        // 'fractionBegin' and 'fractionEnd' with the beginning and end of the
        // fraction portion of 'value', the specified 'isExponentNegative' with
        // a flag indicating whether the exponent of 'value' is negative, the
        // specified 'exponentBegin' with the beginning of the exponent part of
        // 'value', the specified 'significantDigitsBegin' and
        // 'significantDigitsEnd' with the beginning and end of the significant
        // digits of 'value', and the specified 'significantDigitsBias' with a
        // bias to add to the exponent when considering the value of the
        // significant digits.  The range of significant digits returned by
        // this function may include a '.' character (which should simply be
        // ignored when considering the significant digits).  For example,
        // "-12.30e-4" would return 'isNegative' as 'true', the integer portion
        // would be "12", the fractional portion would be "30",
        // 'isExponentNegative' would be 'true', 'exponentBegin' would point to
        // '4', the significant digits would be "12.3" and the
        // 'significantDigitsBias' would be -1.  If there is no fraction in
        // 'value', 'fractionBegin' and 'fractionEnd' will both equal
        // 'exponentBegin'.  If there is no exponent in value 'exponentBegin'
        // will be 'value.end()'.  The behavior is undefined unless
        // 'NumberUtil::isValidNumber(value)' is 'true'.
        //
        // Note that if '*significantDigitsBegin' is '0' then 'value' must be
        // 0.  Also, note that the end of the exponent is (implicitly)
        // 'value.end()'.  Finally, note that the range of significant digits
        // returned and 'significantDigitBias' are useful when considering a
        // canonical representation for a JSON Number, which consists of a
        // whole number (with leading and trailing zeroes removed) and an
        // exponent.  For example, "1.45e-1" would have a canonical
        // representation 145e-3.  The returned significant digits range
        // returned by this function represents the whole number part of this
        // canonical representation (ignoring a '.' character if it appears),
        // and 'significantDigitBias' can be added to the exponent of 'value'
        // to get the canonical exponent for the significant digits.  For
        // example, the value "1.45e-1", which has a canonical representation
        // 145e-3, would return a significant digits of "1.45" (ignore the '.'
        // for the canonical significant digits) and a 'significantDigitsBias'
        // of -2 (which is added to the exponent of 'value', -3, to get the
        // canonical exponent).  A bias is returned, rather than a canonical
        // exponent, because the bias is trivially derived from tokenizing
        // 'value' (and does not require more costly computation, in cases
        // where it is not needed).

    static void logUnparseableJsonNumber(const bsl::string_view& value);
        // Log the specified 'value' (for which 'isValidNumber' should be
        // 'true') could not be correctly parsed into a binary floating point
        // representation.  Note that this function should be unreachable (and
        // no test input has been found for which it is needed) but exists to
        // record an issue in case some gap were found between the JSON number
        // specification and the underlying floating point parsing functions
        // (whose quality may be outside of our control and vary by platform).
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // -----------------
                           // struct NumberUtil
                           // -----------------

inline
double NumberUtil::asDouble(const bsl::string_view& value)
{
    BSLS_ASSERT(NumberUtil::isValidNumber(value));

    double result;

    int rc = bdlb::NumericParseUtil::parseDouble(&result, value);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 != rc)) {
        // This should not be possible and no test has found a case where this
        // is occurs.  However, to prevent a possible denial of service attack
        // if some gap were to be found in the implementation of low-level
        // floating point parsing functions and the JSON number spec, we
        // provide in-contract defined behavior for optimized (production)
        // builds.

        BSLS_ASSERT(false);
        NumberUtil_ImpUtil::logUnparseableJsonNumber(value);
        return bsl::numeric_limits<double>::quiet_NaN();              // RETURN
    }
    return result;
}

inline
float NumberUtil::asFloat(const bsl::string_view& value)
{
    return static_cast<float>(asDouble(value));
}

inline
int NumberUtil::asInt(int *result, const bsl::string_view& value)
{
    return asInteger(result, value);
}

inline
int NumberUtil::asInt64(Int64 *result, const bsl::string_view& value)
{
    return asInteger(result, value);
}

inline
int NumberUtil::asUint(unsigned int *result, const bsl::string_view& value)
{
    return asInteger(result, value);
}

template <class t_INTEGER_TYPE>
int NumberUtil::asInteger(t_INTEGER_TYPE          *result,
                          const bsl::string_view&  value)
{
    BSLMF_ASSERT((bsl::is_integral<t_INTEGER_TYPE>::value &&
                 (sizeof(t_INTEGER_TYPE) <= 8) &&
                 !bsl::is_same<t_INTEGER_TYPE, bool>::value));
    return NumberUtil_ImpUtil::asInteger(result, value);
}

                         // -------------------------
                         // struct NumberUtil_ImpUtil
                         // -------------------------

inline
int NumberUtil_ImpUtil::asInteger(bsls::Types::Uint64     *result,
                                  const bsl::string_view&  value)
{
    return NumberUtil::asUint64(result, value);
}

template <class t_TYPE>
int NumberUtil_ImpUtil::asInteger(t_TYPE                  *result,
                                  const bsl::string_view&  value)
{
    BSLMF_ASSERT(bsl::is_integral<t_TYPE>::value);

    typedef bslmf::SelectTrait<t_TYPE, NumberUtil_IsSigned> Selection;
    return NumberUtil_ImpUtil::asIntegerDispatchImp(result,
                                                    value,
                                                    Selection());
}

template <class t_INTEGER_TYPE>
int NumberUtil_ImpUtil::asIntegerDispatchImp(
                           t_INTEGER_TYPE                              *result,
                           const bsl::string_view&                      value,
                           bslmf::SelectTraitCase<NumberUtil_IsSigned>)
{
    BSLMF_ASSERT(bsl::is_integral<t_INTEGER_TYPE>::value);
    BSLMF_ASSERT(bsl::numeric_limits<t_INTEGER_TYPE>::is_signed);

    BSLS_ASSERT(NumberUtil::isValidNumber(value));

    bsl::string_view positiveValue = value;

    bool isNegative;
    if ('-' == value[0]) {
        isNegative = true;
        positiveValue.remove_prefix(1);
    }
    else {
        isNegative = false;
    }

    const bsls::Types::Uint64 maxValue = static_cast<bsls::Types::Uint64>(
                                   bsl::numeric_limits<t_INTEGER_TYPE>::max());

    bsls::Types::Uint64 tmp;

    const int rc = NumberUtil::asUint64(&tmp, positiveValue);

    if (isNegative) {
        if (tmp > maxValue + 1) {
            *result = bsl::numeric_limits<t_INTEGER_TYPE>::min();
            return NumberUtil::k_UNDERFLOW;                           // RETURN
        }
        *result = static_cast<t_INTEGER_TYPE>(tmp * -1ll );
    }
    else {
        if (tmp > maxValue) {
            *result = static_cast<t_INTEGER_TYPE>(maxValue);
            return NumberUtil::k_OVERFLOW;                            // RETURN
        }
        *result = static_cast<t_INTEGER_TYPE>(tmp);
    }
    return rc;
}

template <class t_INTEGER_TYPE>
int NumberUtil_ImpUtil::asIntegerDispatchImp(t_INTEGER_TYPE           *result,
                                             const bsl::string_view&   value,
                                             bslmf::SelectTraitCase<>)
{
    BSLMF_ASSERT(bsl::is_integral<t_INTEGER_TYPE>::value);
    BSLMF_ASSERT(!bsl::numeric_limits<t_INTEGER_TYPE>::is_signed);

    BSLS_ASSERT(NumberUtil::isValidNumber(value));

    typedef bsls::Types::Uint64 Uint64;

    Uint64 tmp;

    int rc = NumberUtil::asUint64(&tmp, value);
    if (tmp >
        static_cast<Uint64>(bsl::numeric_limits<t_INTEGER_TYPE>::max())) {
        *result = bsl::numeric_limits<t_INTEGER_TYPE>::max();
        return NumberUtil::k_OVERFLOW;                                // RETURN
    }
    *result = static_cast<t_INTEGER_TYPE>(tmp);
    return rc;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BDLJSN_NUMBERUTIL

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
