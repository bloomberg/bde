// bdljsn_jsonnumber.h                                                -*-C++-*-
#ifndef INCLUDED_BDLJSN_JSONNUMBER
#define INCLUDED_BDLJSN_JSONNUMBER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic type representing a JSON number.
//
//@CLASSES:
//  bdljsn::JsonNumber: value-semantic type representing a JSON number
//
//@DESCRIPTION: This component provides a single value-semantic class,
// 'bdljsn::JsonNumber', that represents a JSON number.  The value of a
// 'bdljsn::JsonNumber' object is set at construction using a string
// representation of the JSON number (see {JSON Textual Specification}) or from
// one of several C++ arithmetic types (see {Supported Conversions}).
//
// Arithmetic operations are *not* defined for 'bdljsn::JsonNumber' objects.
// For such operations, the value of a 'bdljsn::JsonNumber' object can be
// converted to any of those supported types, though the conversion may not be
// exact.
//
// The 'bdlsn::JsonNumber' equality operation returns 'true' if the string
// representation of the number (returned by the 'value' accessor method) is
// the same, even where the two strings represent the same number (e.g., "10"
// and "1e1").  This definition of equality reflects the fact that the JSON
// textual representation for the two 'JsonNumber' objects will be different.
// The function 'isEqual' is provided for (a more expensive) numeric equality
// comparison.
//
///JSON Textual Specification
///--------------------------
// JSON numbers are defined by strings that match the grammar given at
// https://www.rfc-editor.org/rfc/rfc8259#section-6.  The equivalent regular
// expression is:
//..
//  /^-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][-+]?[0-9]+)?\z/
//..
// Note that "\z" matches end-of-string but not a preceding '\n'.
//
// For example:
//..
//   1
//   2.1
//  -3
//   4e1
//   5.1e+2
//   6.12e-3
//   7e+04
//  -8.1e+005
//..
// Notice that:
//
//: o Leading zeros are not allowed for the mantissa but are allowed for the
//:   exponent.
//:
//: o A decimal point must be followed by at least one digit.
//:
//: o The grammar does *not* specify any limit on the number of digits in the
//:   mantissa or the exponent.
//
//:   o One can validly represent JSON numbers that are too large or too small
//:     for conversion to any of the supported arithmetic types.
//:
//: o The special values, 'INF' (infinity) and 'NaN' (Not A Number) are
//:   disallowed.
//
// The value of a 'bdljsn::JsonNumber' object is determined by its given string
// representation, which is *not* normalized.  Unequal strings lead to unequal
// 'bdljsn::JsonNumber' objects even if their numerical values are equal.
// Numerical equality can be tested with the 'isEqual' method.  Note that the
// 'isEqual' method is more computationally expensive than the equality and
// inequality operators:
//..
//  // The following 'JsonNumber' objects do not compare equal because their
//  // string representations are different:
//  assert(bdljsn::JsonNumber("1")      != bdljsn::JsonNumber("1.0"));
//
//  // But, they are numerically equal, so 'isEqual' returns 'true':
//  assert(bdljsn::JsonNumber("1").isEqual(bdljsn::JsonNumber("1.0")));
//..
//
///Supported Conversions
///---------------------
// The value of a 'bdljsn::JsonNumber' object can be converted to an assortment
// of useful types:
//
//: o 'int'
//: o 'unsigned int'
//: o 'bsls::Types::Int64'
//: o 'bsls::Types::Uint64'
//: o 'float'
//: o 'double'
//: o 'bdldfp::Decimal64'
//
// In addition to named conversion functions (like 'asInt' and 'asDouble') This
// component provides explicit conversion operations for floating point types
// ('float', 'double', 'Decimal64') on platforms where explicit conversions are
// supported.
//
///Handling Inexact Conversions: Floating Point Vs Integral Types
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Converting a 'bdlsjn::JsonNumber' to another representation may result in a
// value that is not the same as the original 'bdljsn::JsonNumber'.  Either the
// 'bdljsn::JsonNumber' may represent a numeric value outside of the
// representable range of the requested type (i.e., it is too large, or too
// small), or the value may not be representable exactly.  'bdljsn::JsonNumber'
// conversions will return the closest approximation of the
// 'bdljsn::JsonNumber', even when a non-zero status is returned indicating an
// inexact conversion.
//
// All the provided conversions to integral types have signatures that require
// a return status, whereas conversion functions are provided for floating
// point types that do not return a status.  This is because:
//
//: 1 Floating point representations have specific values to indicate a
//:   'bdljsn::JsonNumber' is outside of the representable range
//:   '(-INF, +INF)'.
//:
//: 2 Truncating the fractional part of a number to coerce a value to an
//:   integer is typically an error (the data being processed did not meet the
//:   programmer's expectation), whereas returning the closest floating point
//:   approximation to a 'bdljsn::JsonNumber' is very often not an error.
//
///Exact 'Decima64' Representations
/// - - - - - - - - - - - - - - - -
// For users requiring precise conversions to 'bdldfp::Decimal64', the function
// 'asDecimal64Exact' returns additional status indicating whether the
// conversion is exact.  An exact conversion for a 'Decimal64' is one that
// preserves all the significant digits resulting in a decimal representation
// having the same numerical value as the original JSON text.  Note that
// 'asDecimal64Exact' has very similar performance to 'asDecimal64' (i.e.,
// there is not a notable performance penalty to determining this property).
//
///Known Issues With 'asDecima64Exact'
///- - - - - - - - - - - - - - - - - -
// Currently 'asDecimal64Exact' will return 'bdljsn::JsonNumber::k_NOT_EXACT'
// if the input is 0 with an exponent outside of the range ('[-398, 369]').
// For example, '0e-400'.  This reflects the behavior of the underlying 3rd
// party implementation.  Please contact BDE if this is a concern.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating JSON Number Object from User Input
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The specification of values for JSON numbers often starts with user input
// textual representations of those values.  As the specifications for valid
// representation are complicated and not always intuitive it is prudent to
// validate that input using the 'bdljsn::JsonNumber::isValidNumber' function;
// otherwise, one might try to create a 'bdljsn::JsonNumber' object from an
// invalid specification and that leads to undefined behavior.
//
// First, as a expedient for this example, we organize in an array input that
// might well be entered by some user:
//..
//  struct {
//      const char *d_text_p;
//      const char *d_description_p;
//      bool        d_expected;
//  } USER_INPUT[] = {
//
//  //  VALUE                   DESCRIPTION                             EXP
//  //  ----------------------  --------------------------------------  ---
//
//    // Invalid Input (that is valid in other contexts).
//
//    { "1.",                   "Not uncommon way to write '1'."      , 0  }
//  , { "1,000",                "No commas allowed"                   , 0  }
//  , { "01",                   "Leading '0',  disallowed by JSON."   , 0  }
//  , { "",                     "0 per 'atoi', disallowed by JSON."   , 0  }
//  , { "Hello, world!",        "0 per 'atoi', disallowed by JSON."   , 0  }
//  , { "NaN",                  "invalid number"                      , 0  }
//  , { "INF",                  "invalid number"                      , 0  }
//  , { "-INF",                 "invalid number"                      , 0  }
//  , { "+INF",                 "invalid number"                      , 0  }
//
//    // Valid input (some surprising)
//
//  , { "1234567890",           "Integral value"                      , 1  }
//  , { "1234567890.123456",    "Non-integral value"                  , 1  }
//  , { "1234567890.1234567",   "Beyond Decimal64 precision"          , 1  }
//  , { "-9223372036854775809", "INT64_MIN, underflow, but valid JSON", 1  }
//  , { "1.5e27",               "INT64_MAX,  overflow, but valid JSON", 1  }
//  , { "999999999999999999999999999999999999999999999999999999999999"
//      "e"
//      "999999999999999999999999999999999999999999999999999999999999",
//                              "astronomic value"                    , 1 }
//  };
//
//  const bsl::size_t NUM_USER_INPUT = sizeof USER_INPUT / sizeof *USER_INPUT;
//..
// Now, if and only if the input is valid, we use the input to construct a
// 'bdljsn::JsonNumber' object and add that object to a vector for later
// processing.
//..
//  bsl::vector<bdljsn::JsonNumber> userInput; // when valid input
//
//  for (bsl::size_t ti = 0; ti < NUM_USER_INPUT; ++ti) {
//      const char *TEXT = USER_INPUT[ti].d_text_p;
//      const char *DESC = USER_INPUT[ti].d_description_p; (void) DESC;
//      const bool  EXP  = USER_INPUT[ti].d_expected;
//
//      const bool isValid  = bdljsn::JsonNumber::isValidNumber(TEXT);
//      assert(EXP == isValid);
//
//      if (isValid) {
//          userInput.push_back(bdljsn::JsonNumber(TEXT));
//      }
//  }
//..
// Finally, we confirm that the vector has the expected number of elements:
//..
//  assert(6 == userInput.size());
//..
//
///Example 2: Using 'bdljsn::JsonNumber' Objects
///- - - - - - - - - - - - - - - - - - - - - - -
// We saw in {Example 1} that 'bdljsn::JsonNumber' objects can validly hold
// values numeric values that cannot be converted to any of the supported types
// (e.g., the "astronomic value") for arithmetic operations.  Applications that
// accept arbitrary 'bdljsn::JsonNumber' objects should be prepared to
// categorize the contained value and adapt their handling accordingly.  In
// practice, applications may have some assurances of the contents of received
// 'bdljsn::JsonNumber' objects.  Here, we intentionally avoid such assumptions
// to explore the wide range of variations that can arise.
//
// Legend, in the output below:
//
//: o "OK":
//:   o Means "OKay to use".  In some cases, the numeric value of the
//:     arithmetic type is an approximation of JSON number and the application
//:     may have to allow for that difference.
//:
//: o "NG":
//:   o Means "No Good" (do not use).  The JSON number is outside of the valid
//:     range of the arithmetic type.
//
// First, we set up a framework (in this case, a 'for' loop) for examining our
// input, the same 'userInput' vector created in {Example 1}:
//..
//  for (bsl::size_t i = 0; i < userInput.size(); ++i) {
//      const bdljsn::JsonNumber obj = userInput[i];
//..
// Then, we categorize the value as integral or not:
//..
//      if (obj.isIntegral()) {
//          bsl::cout << "Integral: ";
//..
// If integral, we check if the value is a usable range.  Let us assume that
// 'bslsl::Type::Int64' is as large a number as we can accept.
//
// Then, we convert the JSON number to that type and check for overflow and
// underflow:
//..
//          bsls::Types::Int64 value;
//          int                rc = obj.asInt64(&value);
//          switch (rc) {
//              case 0: {
//                  bsl::cout << value      << " : OK to USE" << bsl::endl;
//              } break;
//              case bdljsn::JsonNumber::k_OVERFLOW: {
//                  bsl::cout << obj.value() << ": NG too large" << bsl::endl;
//              } break;
//              case bdljsn::JsonNumber::k_UNDERFLOW: {
//                  bsl::cout << obj.value() << ": NG too small" << bsl::endl;
//              } break;
//              case bdljsn::JsonNumber::k_NOT_INTEGRAL: {
//                assert(!"reached");
//              } break;
//          }
//..
// Next, if the value is not integral, we try to handle it as a floating point
// value -- a 'bdldfp::Decimal64' in this example -- and further categorize it
// as exact/inexact, too large/small.
//..
//      } else {
//          bsl::cout << "Not-Integral: ";
//
//          bdldfp::Decimal64 value;
//          int               rc = obj.asDecimal64Exact(&value);
//          switch (rc) {
//              case 0: {
//                  bsl::cout << value << " :  exact: OK to USE";
//              } break;
//              case bdljsn::JsonNumber::k_INEXACT: {
//                  bsl::cout << value << ": inexact: USE approximation";
//              } break;
//              case bdljsn::JsonNumber::k_NOT_INTEGRAL: {
//                assert(!"reached");
//              } break;
//          }
//
//          const bdldfp::Decimal64 INF =
//                          bsl::numeric_limits<bdldfp::Decimal64>::infinity();
//
//          if        ( INF == value) {
//              bsl::cout << ": NG too large" << bsl::endl;
//          } else if (-INF == value) {
//              bsl::cout << ": NG too small" << bsl::endl;
//          } else {
//              bsl::cout << bsl::endl;
//          }
//      }
//  }
//..
// Finally, we observe for particular input:
//..
//  Integral: 1234567890 : OK to USE
//  Not-Integral: 1234567890.123456 :  exact: OK to USE
//  Not-Integral: 1234567890.123457: inexact: USE approximation
//  Integral: -9223372036854775809: NG too small
//  Integral: 1.5e27: NG too large
//  Integral: 999999999999999999999999999999999999999999999999999999999999e9999
//  99999999999999999999999999999999999999999999999999999999: NG too large
//..

#include <bdlscm_version.h>

#include <bdljsn_numberutil.h>

#include <bdlb_float.h>
#include <bdldfp_decimal.h>
#include <bdldfp_decimalutil.h>

#include <bslalg_swaputil.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isintegral.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>  // 'BSLS_KEYWORD_NOEXCEPT'
#include <bsls_types.h>    // 'bsls::Types::Int64', 'bsls::Types::Uint64'

#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace bdljsn {

                              // ================
                              // class JsonNumber
                              // ================

class JsonNumber {
    // This class defines a value-semantic class that represents a JSON number.
    // Objects of this class have a value determined at construction and does
    // not change except by assignment from or swap with another 'JsonNumber'
    // object.  The value can be specified by supplying a string that conforms
    // to the {JSON Textual Specification} or from one of the {Supported
    // Types}.  The value of a JSON object can be converted to any of those
    // types; however, some of those conversions can be inexact.

    // PRIVATE TYPES
    typedef NumberUtil Nu;

    // DATA
    bsl::string d_value;

    // FRIENDS
    friend void swap(JsonNumber& , JsonNumber& );

  public:
    // CONSTANTS
    enum {
        // special integer conversion status  values
        k_OVERFLOW     = Nu::k_OVERFLOW,     // above the representable range
        k_UNDERFLOW    = Nu::k_UNDERFLOW,    // below the representable range
        k_NOT_INTEGRAL = Nu::k_NOT_INTEGRAL, // the number is not an integer

        // special exact Decimal64 conversion status values
        k_INEXACT = Nu::k_INEXACT
    };

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(JsonNumber, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(JsonNumber, bslmf::IsBitwiseMoveable);

    // CLASS METHODS
    static bool isValidNumber(const bsl::string_view& text);
        // Return 'true' if the specified 'text' complies with the grammar of a
        // JSON number, and 'false' otherwise.  See the {JSON Textual
        // Specification}.

    // CREATORS
    JsonNumber();
    explicit JsonNumber(bslma::Allocator *basicAllocator);
        // Create a 'JsonNumber' having the value "0".  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit JsonNumber(const char              *text,
                        bslma::Allocator        *basicAllocator = 0);
    explicit JsonNumber(const bsl::string_view&  text,
                        bslma::Allocator        *basicAllocator = 0);
        // Create a 'JsonNumber' having the value of the specified 'text'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'isValidJsonNumber(text)'
        // is 'true'.  See {JSON Textual Specification}.

    explicit JsonNumber(bslmf::MovableRef<bsl::string> text);
        // Create a 'JsonNumber' object having the same value and the same
        // allocator as the specified 'text'.  The contents of the 'value'
        // string becomes unspecified but valid, and its allocator remains
        // unchanged.  The behavior is undefined unless 'isValidNumber(text)'
        // is 'true'.  See {JSON Textual Specification}.

    explicit JsonNumber(bslmf::MovableRef<bsl::string>  text,
                        bslma::Allocator               *basicAllocator);
        // Create a 'JsonNumber' object having the same value as the specified
        // 'text', using the specified 'basicAllocator' to supply memory.  The
        // allocator of the 'text' string remains unchanged.  If the 'text' and
        // the newly created object have the same allocator then the contents
        // of 'text' string becomes unspecified but valid, and no exceptions
        // will be thrown; otherwise the 'text' string is unchanged and an
        // exception may be thrown.  The behavior is undefined unless
        // 'isValidNumber(text)' is 'true'.  See {JSON Textual Specification}.

    explicit JsonNumber(int                      value,
                        bslma::Allocator        *basicAllocator = 0);
    explicit JsonNumber(unsigned int             value,
                        bslma::Allocator        *basicAllocator = 0);
    explicit JsonNumber(bsls::Types::Int64       value,
                        bslma::Allocator        *basicAllocator = 0);
    explicit JsonNumber(bsls::Types::Uint64      value,
                        bslma::Allocator        *basicAllocator = 0);
        // Create a 'JsonNumber' having the specified 'value'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit JsonNumber(float                    value,
                        bslma::Allocator        *basicAllocator = 0);
    explicit JsonNumber(double                   value,
                        bslma::Allocator        *basicAllocator = 0);
    explicit JsonNumber(bdldfp::Decimal64        value,
                        bslma::Allocator        *basicAllocator = 0);
        // Create a 'JsonNumber' having the specified 'value'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined if the 'value' is infinite ('INF')
        // or not-a-number ('NaN').

    JsonNumber(const JsonNumber&  original,
               bslma::Allocator  *basicAllocator = 0);
        // Create a 'JsonNumber' object having the same value as the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    JsonNumber(bslmf::MovableRef<JsonNumber> original) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'JsonNumber' object having the same value and the same
        // allocator as the specified 'original' object.  The value of
        // 'original' becomes unspecified but valid, and its allocator remains
        // unchanged.

    JsonNumber(bslmf::MovableRef<JsonNumber>  original,
               bslma::Allocator              *basicAllocator);
        // Create a 'JsonNumber' object having the same value as the specified
        // 'original' object, using the specified 'basicAllocator' to supply
        // memory.  The allocator of 'original' remains unchanged.  If
        // 'original' and the newly created object have the same allocator then
        // the value of 'original' becomes unspecified but valid, and no
        // exceptions will be thrown; otherwise 'original' is unchanged (and an
        // exception may be thrown).

//! ~JsonNumber() = default;
        // Destroy this object.

    // MANIPULATORS
    JsonNumber& operator=(const JsonNumber& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.

    JsonNumber& operator=(bslmf::MovableRef<JsonNumber> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.  The allocators of
        // this object and 'rhs' both remain unchanged.  If 'rhs' and this
        // object have the same allocator then the value of 'rhs' becomes
        // unspecified but valid, and no exceptions will be thrown; otherwise
        // 'rhs' is unchanged (and an exception may be thrown).

    JsonNumber& operator=(int                 rhs);
    JsonNumber& operator=(unsigned int        rhs);
    JsonNumber& operator=(bsls::Types::Int64  rhs);
    JsonNumber& operator=(bsls::Types::Uint64 rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // non-'const' reference to this object.

    JsonNumber& operator=(float             rhs);
    JsonNumber& operator=(double            rhs);
    JsonNumber& operator=(bdldfp::Decimal64 rhs);
        // Assign to this object the value of the specified 'rhs', and return a
        // non-'const' reference to this object.  The behavior is undefined if
        // 'rhs' is infinite ('INF') or not-a-number ('NaN').

    void swap(JsonNumber& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    bool isEqual(const JsonNumber& other) const;
        // Return 'true' if this number and the specified 'other' number
        // represent the same numeric value, and 'false' otherwise.  This
        // method will return 'true' for differing representations of the same
        // number (e.g., '1.0', "1", "0.1e+1" are all equivalent) *except* in
        // cases where the exponent cannot be represented by a 64-bit integer.
        // If the exponent is outside the range of a 64-bit integer, 'true'
        // will be returned if '*this == other'.  For example, comparing
        // "1e18446744073709551615" with itself will return 'true', but
        // comparing it to "10e18446744073709551614" will return 'false'. Note
        // that this method is more computationally expensive than the equality
        // and inequality operators.

    bool isIntegral() const;
        // Return 'true' if the value of this 'JsonNumber' is an (exact)
        // integral value, or 'false' otherwise.   Note that this function may
        // return 'true' even this number cannot be represented in a
        // fundamental integral type.

    const bsl::string& value() const;
        // Return the textual representation of this 'JsonNumber'.

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01 // not in alphabetic order

                        //  Integer Accessors

    int asInt   (int                 *result) const;
    int asInt64 (bsls::Types::Int64  *result) const;
    int asUint  (unsigned int        *result) const;
    int asUint64(bsls::Types::Uint64 *result) const;
        // Load into the specified 'result' the integer value of this number.
        // Return 0 on success, 'k_OVERFLOW' if 'value' is larger than can be
        // represented by 'result', 'k_UNDERFLOW' if 'value' is smaller than
        // can be represented by 'result',  and 'k_NOT_INTEGRAL' if 'value' is
        // not an integral number (i.e., there is a fractional part).  For
        // underflow, 'result' will be loaded with the minimum representable
        // value, for overflow, 'result' will be loaded with the maximum
        // representable value, for non-integral values 'result' will be loaded
        // with the integer part of 'value' (truncating the fractional part).
        // If the result is not an integer and also either overflows or
        // underflows, it is treated as an overflow or underflow
        // (respectively).  Note that this operation returns an error status
        // value (unlike similar floating point conversions) because typically
        // it is an error if a conversion to an integer results in an in-exact
        // value.

    float              asFloat()     const;
    double             asDouble()    const;
    bdldfp::Decimal64  asDecimal64() const;
        // Return the closest floating point representation to this number.  If
        // this number is outside the representable range, return '+INF' or
        // '-INF' (as appropriate).  Note that values smaller than the smallest
        // representable non-zero value (a.k.a, 'MIN') are rounded to 'MIN'
        // (positive or negative, as appropriate) or 0, whichever is the better
        // approximation.

                        // 'Exact' Accessors

    int asDecimal64Exact(bdldfp::Decimal64 *result) const;
        // Load to the specified 'result' the closest floating point
        // representation to this number, even if a non-zero status is
        // returned.  Return 0 if this number can be represented exactly, and
        // return 'k_INEXACT' and load 'result' with the closest approximation
        // if 'value' cannot be represented exactly.  If this number is outside
        // the representable range, load 'result' with '+INF' or '-INF' (as
        // appropriate).  A number can be represented exactly as a 'Decimal64'
        // if, for the significand and exponent,
        // 'abs(significand) <= 9,999,999,999,999,999' and
        // '-398 <= exponent <= 369'.

// BDE_VERIFY pragma: pop

                       // 'explicit' (conversion) operators

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
    explicit operator float()             const;
    explicit operator double()            const;
    explicit operator bdldfp::Decimal64() const;
        // Return the closest floating point representation to this number.  If
        // this number is outside the representable range, return '+INF' or
        // '-INF' (as appropriate).  Note that the values returned by these
        // operators match those returned by 'asFloat', 'asDouble', and
        // 'asDecimal64', respectively.
#endif

                        // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a non-'const' reference to
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const JsonNumber& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a non-'const' reference to
    // 'stream'.  If 'stream' is not valid on entry, this operation has no
    // effect.  Note that this human-readable format is not fully specified and
    // can change without notice.  Also note that this method has the same
    // behavior as 'object.print(stream, 0, -1)'.

bool operator==(const JsonNumber& lhs, const JsonNumber& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'JsonNumber' objects have the same
    // value if their 'value' attributes are the same.

bool operator!=(const JsonNumber& lhs, const JsonNumber& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'JsonNumber' objects do not have
    // the same value if their 'value' attributes are not the same.

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlgorithm, const JsonNumber& object);
    // Pass the specified 'object' to the specified 'hashAlgorithm'.  This
    // function integrates with the 'bslh' modular hashing system and
    // effectively provides a 'bsl::hash' specialization for 'JsonNumber'.

void swap(JsonNumber& a, JsonNumber& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the two objects were
    // created with the same allocator and the basic guarantee otherwise.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // -----------------
                              // struct JsonNumber
                              // -----------------

// CLASS METHODS
inline
bool JsonNumber::isValidNumber(const bsl::string_view& text)
{
    return NumberUtil::isValidNumber(text);
}

// CREATORS
inline
JsonNumber::JsonNumber()
: d_value(1, '0')
{
}

inline
JsonNumber::JsonNumber(bslma::Allocator *basicAllocator)
: d_value(1, '0', basicAllocator)
{
}

inline
JsonNumber::JsonNumber(const char       *text,
                       bslma::Allocator *basicAllocator)
: d_value(text, basicAllocator)
{
    BSLS_ASSERT(NumberUtil::isValidNumber(text));
}

inline
JsonNumber::JsonNumber(const bsl::string_view&  text,
                       bslma::Allocator        *basicAllocator)
: d_value(text, basicAllocator)
{
    BSLS_ASSERT(NumberUtil::isValidNumber(text));
}

inline
JsonNumber::JsonNumber(int value, bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
    NumberUtil::stringify(&d_value, static_cast<bsls::Types::Int64>(value));
}

inline
JsonNumber::JsonNumber(unsigned int value, bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
    NumberUtil::stringify(&d_value, static_cast<bsls::Types::Uint64>(value));
}

inline
JsonNumber::JsonNumber(bsls::Types::Int64  value,
                       bslma::Allocator   *basicAllocator)
: d_value(basicAllocator)
{
    NumberUtil::stringify(&d_value, value);
}

inline
JsonNumber::JsonNumber(bsls::Types::Uint64  value,
                       bslma::Allocator    *basicAllocator)
: d_value(basicAllocator)
{
    NumberUtil::stringify(&d_value, value);
}

inline
JsonNumber::JsonNumber(float value, bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
    BSLS_ASSERT(!bdlb::Float::isNan     (value));
    BSLS_ASSERT(!bdlb::Float::isInfinite(value));

    NumberUtil::stringify(&d_value, value);
}

inline
JsonNumber::JsonNumber(double value, bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
    BSLS_ASSERT(!bdlb::Float::isNan     (value));
    BSLS_ASSERT(!bdlb::Float::isInfinite(value));

    NumberUtil::stringify(&d_value, value);
}

inline
JsonNumber::JsonNumber(bdldfp::Decimal64  value,
                       bslma::Allocator  *basicAllocator)
: d_value(basicAllocator)
{
    BSLS_ASSERT(!bdldfp::DecimalUtil::isNan(value));
    BSLS_ASSERT(!bdldfp::DecimalUtil::isInf(value));

    NumberUtil::stringify(&d_value, value);
}

inline
JsonNumber::JsonNumber(bslmf::MovableRef<bsl::string> text)
: d_value(bslmf::MovableRefUtil::move(text))
{
    BSLS_ASSERT(NumberUtil::isValidNumber(d_value));
}

inline
JsonNumber::JsonNumber(bslmf::MovableRef<bsl::string>  text,
                       bslma::Allocator               *basicAllocator)
: d_value(bslmf::MovableRefUtil::move(text), basicAllocator)
{
    BSLS_ASSERT(NumberUtil::isValidNumber(d_value));
}

inline
JsonNumber::JsonNumber(const JsonNumber&  original,
                       bslma::Allocator  *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
JsonNumber::JsonNumber(bslmf::MovableRef<JsonNumber> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_value(bslmf::MovableRefUtil::move(
                              bslmf::MovableRefUtil::access(original).d_value))
{
}

inline
JsonNumber::JsonNumber(bslmf::MovableRef<JsonNumber>  original,
                       bslma::Allocator              *basicAllocator)
: d_value(bslmf::MovableRefUtil::move(
                              bslmf::MovableRefUtil::access(original).d_value),
                              basicAllocator)
{
}

// MANIPULATORS
inline
JsonNumber& JsonNumber::operator=(const JsonNumber& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
JsonNumber& JsonNumber::operator=(bslmf::MovableRef<JsonNumber> rhs)
{
    d_value = bslmf::MovableRefUtil::move(
                                   bslmf::MovableRefUtil::access(rhs).d_value);
    return *this;
}

inline
JsonNumber& JsonNumber::operator=(int rhs)
{
    NumberUtil::stringify(&d_value, static_cast<bsls::Types::Int64>(rhs));
    return *this;
}

inline
JsonNumber& JsonNumber::operator=(unsigned int rhs)
{
    NumberUtil::stringify(&d_value, static_cast<bsls::Types::Uint64>(rhs));
    return *this;
}

inline
JsonNumber& JsonNumber::operator=(bsls::Types::Int64 rhs)
{
    NumberUtil::stringify(&d_value, rhs);
    return *this;
}

inline
JsonNumber& JsonNumber::operator=(bsls::Types::Uint64 rhs)
{
    NumberUtil::stringify(&d_value, rhs);
    return *this;
}

inline
JsonNumber& JsonNumber::operator=(float rhs)
{
    BSLS_ASSERT(!bdlb::Float::isNan     (rhs));
    BSLS_ASSERT(!bdlb::Float::isInfinite(rhs));

    NumberUtil::stringify(&d_value, rhs);
    return *this;
}

inline
JsonNumber& JsonNumber::operator=(double rhs)
{
    BSLS_ASSERT(!bdlb::Float::isNan     (rhs));
    BSLS_ASSERT(!bdlb::Float::isInfinite(rhs));

    NumberUtil::stringify(&d_value, rhs);
    return *this;
}

inline
JsonNumber& JsonNumber::operator=(bdldfp::Decimal64 rhs)
{
    BSLS_ASSERT(!bdldfp::DecimalUtil::isNan(rhs));
    BSLS_ASSERT(!bdldfp::DecimalUtil::isInf(rhs));

    NumberUtil::stringify(&d_value, rhs);
    return *this;
}

inline
void JsonNumber::swap(JsonNumber& other)
{
    BSLS_ASSERT(d_value.allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_value, &other.d_value);
}

// ACCESSORS
inline
bool JsonNumber::isEqual(const JsonNumber& other) const
{
    return NumberUtil::areEqual(d_value, other.d_value);
}

inline
bool JsonNumber::isIntegral() const
{
    return NumberUtil::isIntegralNumber(d_value);
}

inline
const bsl::string& JsonNumber::value() const
{
    return d_value;
}

                        // Integer Accessors

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01 // not in alphabetic order

inline
int JsonNumber::asInt(int *result) const
{
    return NumberUtil::asInt(result, d_value);
}

inline
int JsonNumber::asInt64(bsls::Types::Int64 *result) const
{
    return NumberUtil::asInt64(result, d_value);
}

inline
int JsonNumber::asUint(unsigned int *result) const
{
    return NumberUtil::asUint(result, d_value);
}

inline
int JsonNumber::asUint64(bsls::Types::Uint64 *result) const
{
    return NumberUtil::asUint64(result, d_value);
}

inline
float JsonNumber::asFloat() const
{
    return static_cast<float>(asDouble());
}

inline
double JsonNumber::asDouble() const
{
    return NumberUtil::asDouble(d_value);
}

inline
bdldfp::Decimal64 JsonNumber::asDecimal64() const
{
    return NumberUtil::asDecimal64(d_value);
}

                        // 'Exact' Accessors

inline
int JsonNumber::asDecimal64Exact(bdldfp::Decimal64 *result) const
{
    return NumberUtil::asDecimal64Exact(result, d_value);
}

// BDE_VERIFY pragma: pop

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)

                        // 'explicit' (conversion) operators
inline
JsonNumber::operator float() const
{
    return asFloat();
}

inline
JsonNumber::operator double() const
{
    return asDouble();
}

inline
JsonNumber::operator bdldfp::Decimal64() const
{
    return asDecimal64();
}
#endif

                        // Aspects
inline
bslma::Allocator *JsonNumber::allocator() const
{
    return d_value.get_allocator().mechanism();
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& bdljsn::operator<<(bsl::ostream&             stream,
                                 const bdljsn::JsonNumber& object)
{
    return object.print(stream, 0, -1);
}

inline
bool bdljsn::operator==(const bdljsn::JsonNumber& lhs,
                        const bdljsn::JsonNumber& rhs)
{
    return lhs.value() == rhs.value();
}

inline
bool bdljsn::operator!=(const bdljsn::JsonNumber& lhs,
                        const bdljsn::JsonNumber& rhs)
{
    return lhs.value() != rhs.value();
}

// FREE FUNCTIONS
template <class HASHALG>
inline
void bdljsn::hashAppend(HASHALG&                  hashAlgorithm,
                        const bdljsn::JsonNumber& object)
{
    hashAppend(hashAlgorithm, object.value());
}

inline
void bdljsn::swap(bdljsn::JsonNumber& a, bdljsn::JsonNumber& b)
{
    bslalg::SwapUtil::swap(&a.d_value, &b.d_value);
}

}  // close enterprise namespace

#endif  // INCLUDED_BDLJSN_JSONNUMBER

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
