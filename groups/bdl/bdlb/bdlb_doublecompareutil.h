// bdlb_doublecompareutil.h                                           -*-C++-*-
#ifndef INCLUDED_BDLB_DOUBLECOMPAREUTIL
#define INCLUDED_BDLB_DOUBLECOMPAREUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide "fuzzy" equality/relational operations on `double` values.
//
//@CLASSES:
//   bdlb::DoubleCompareUtil: namespace for "fuzzy" comparison functions
//
//@DESCRIPTION: This component implements a utility class that performs "fuzzy"
// equality and relational operations as a suite of functions, each of which
// operates on a pair of `double` values `a` and `b`.  The term "fuzzy
// equality" expresses the notion that `a` and `b` are "close enough", and
// that any difference that may exist between the values implied by the
// physical representations of `a` and `b` is not significant, and should be
// ignored for comparison purposes.
//
// For example:
//```
// bdlb::DoubleCompareUtil::fuzzyEq(a, b)
//```
// This comparison returns whether values `a` and `b` are "close enough" to be
// considered equal.  This example uses the default tolerances of
// `bdlb::DoubleCompareUtil` for "absolute difference" and "relative
// difference" (which are defined below).
//
// Notice that being able ignore small differences between floating point
// numbers for the purpose of comparison is important, because `double`
// provides binary *approximations* for the mathematical concept of real
// numbers.  Differences related to these approximation may accumulate when
// performing calculations such that a sequence of operations performed on two
// numbers, which might result in equal values for real numbers, may result in
// `double` approximations that are very close (but different).
//
///Formal Definition and Tolerances
///--------------------------------
// More formally, the *absolute* *difference* between `a` and `b` is defined
// as the absolute value of the difference of `a` and `b`:
//```
//  fabs(a - b)                              // absolute difference
//```
// The *relative* *difference* is defined as the quotient of the absolute
// difference and the absolute value of the average of `a` and `b`.
//```
//  fabs(a - b) / fabs((a + b) / 2.0)        // relative difference
//```
// Objects `a` and `b` have fuzzy equality if they have the same value, or if
// either their absolute difference is less than or equal to some
// user-specified (or default) tolerance value `absTol`, or their relative
// difference is less or equal to some separate, user-specified (or default)
// tolerance value `relTol`.  Using the notation "A ::= B" to mean
// "A is defined as B", we define fuzzy equality (denoted "~eq") as follows:
//```
//  a ~eq b ::= a == b
//           || fabs(a - b)                       <= absTol
//           || fabs(a - b) / fabs((a + b) / 2.0) <= relTol
//```
// where `fabs(double)` is the standard C absolute-value function and all
// symbols other than `~` and `::=` have their usual C++ meaning.  Note that
// the fuzzy equality operation is symmetric (i.e., commutative) with respect
// to `a` and `b` for *all* values of `relTol` and `absTol`.
//
// The following table identifies the six fuzzy equality and relational
// operations corresponding to the standard C++ operators, along with the
// associated static member functions defined within the
// `bdlb::DoubleCompareUtil` utility `struct` (note that the parameters
// `relTol` and `absTol` are optional, and default to reasonable,
// implementation-dependent values):
//```
//     C++       fuzzy
//  operator   operation   qualified static member-function name
//  --------   ---------   -------------------------------------------
//     ==         ~eq      DoubleCompareUtil::eq(a, b, relTol, absTol)
//     !=         ~ne      DoubleCompareUtil::ne(a, b, relTol, absTol)
//     <          ~lt      DoubleCompareUtil::lt(a, b, relTol, absTol)
//     <=         ~le      DoubleCompareUtil::le(a, b, relTol, absTol)
//     >          ~gt      DoubleCompareUtil::gt(a, b, relTol, absTol)
//     >=         ~ge      DoubleCompareUtil::ge(a, b, relTol, absTol)
//```
// Each of the other fuzzy (inequality and relational) operations is defined
// with respect to fuzzy equality:
//```
//  a ~ne b  ::=  !(a ~eq b)
//  a ~lt b  ::=  !(a ~eq b) && (a < b)
//  a ~le b  ::=   (a ~eq b) || (a < b)
//  a ~gt b  ::=  !(a ~eq b) && (a > b)
//  a ~ge b  ::=   (a ~eq b) || (a > b)
//```
//
///Special Values Handling
///-----------------------
// Floating point numbers support infinity, NaN and negative zero values.  Such
// values are allowed only for the `a` and `b` parameters, but not for the
// absolute or the relative tolerance parameters.
//
// If a NaN value is specified as either (or both) compared argument (`a` or
// `b`) the `fuzzyCompare` method returns `e_NON_COMPARABLE`, `fuzzyNe` returns
// `true`, while all the other methods return `false` values as dictated by the
// IEEE 754 standard.
//
// If infinity is provided as a value the comparisons will behave in the
// expected manner that positive infinity is larger than any non-infinite
// value, while negative infinity is smaller than any finite value.  Two
// infinity values will compare equal if their sign is the same, otherwise the
// negative infinity value will be reported as less than the positive.
//
// As per IEEE 754 negative zero values are considered equal to "normal" zero
// values.  Note that negative zero values represent a minuscule negative value
// that could not be represented by the finite precision and exponent of a
// floating point representation.  In other words it indicates that we have
// "arrived" at the zero value from the negative "side" during a calculation.
//
///Table Demonstrating the Results for Comparisons
///-----------------------------------------------
// To illustrate precise behavior, the following example tabulates the
// numerical results of calling the six fuzzy-comparison functions on a set of
// carefully selected inputs `x` and `y`, where `relTol` and `absTol` are,
// respectively the relative and absolute tolerances (the actual relative and
// absolute differences of `x` and `y`, rounded to five decimal digits, are
// tabulated, for reference, in the two columns to the extreme right):
//```
//  <---------INPUTS----------->   <--------OUTPUTS----->   <-ACTUAL DIFFS->
//    x      y    relTol  absTol   eq  ne  lt  le  gt  ge   RelDiff  AbsDiff
//  -----  -----  ------  ------   --  --  --  --  --  --   -------  -------
//   99.0  100.0   0.010   0.001   0   1   1   1   0   0    0.01005  1.00000
//  100.0   99.0   0.010   0.001   0   1   0   0   1   1    0.01005  1.00000
//   99.0  100.0   0.011   0.001   1   0   0   1   0   1    0.01005  1.00000
//   99.0  100.0   0.010   0.999   0   1   1   1   0   0    0.01005  1.00000
//   99.0  100.0   0.010   1.000   1   0   0   1   0   1    0.01005  1.00000
//
//  100.0  101.0   0.009   0.001   0   1   1   1   0   0    0.00995  1.00000
//  101.0  100.0   0.009   0.001   0   1   0   0   1   1    0.00995  1.00000
//  100.0  101.0   0.010   0.001   1   0   0   1   0   1    0.00995  1.00000
//  100.0  101.0   0.009   0.999   0   1   1   1   0   0    0.00995  1.00000
//  100.0  101.0   0.009   1.000   1   0   0   1   0   1    0.00995  1.00000
//```
///Valid Use, Limitations, and Caveats
///------------------------------------
// Each of the functions implemented in this component are well behaved for all
// non-negative numeric values of `relTol` and `absTol`.  If either `relTol`
// or `absTol` is NaN, infinioty, or negative the behavior is undefined.  (Note
// that this includes negative zero!)  If either `relTol` or `absTol` is
// (positive) zero, that aspect of "fuzzy comparison" is suppressed; if both
// are zero, each of the six fuzzy-comparison operations behave as
// runtime-intensive versions of their non-fuzzy counterparts.
//
// Note that the definition of fuzzy equality used in this component does have
// one intermediate singularity: When `(fabs(a - b) > absTol && (a == -b))` is
// true, the pseudo-expression `a ~eq b` defined above has a zero denominator.
// In this case, the test for relative fuzzy equality is suppressed.  This
// intermediate singularity does not, however, lead to a special-case behavior
// of fuzzy comparisons: By definition, the relative difference is the quotient
// of the absolute difference and the absolute average, so the case `(a == -b)`
// truly represents an "infinite relative difference", and thus fuzzy equality
// via the relative difference criteria should be false (although absolute
// fuzzy equality may still prevail).
//
// Finally, note that the implementations of the functions in this component
// are vulnerable to the limitations of values that can be represented by
// a `double`.  In particular, if `(a + b)` or `(a - b)` cannot be represented,
// the functions will fail outright.  More subtly, as `a` or `b` approaches the
// limits of precision of representation, the algorithms used in this component
// become increasingly unreliable.  The user is responsible for determining
// the limits of applicability of this component to a given calculation, and
// for coding accordingly.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Meaningful Comparisons
///- - - - - - - - - - - - - - - - -
// The `bdlb::DoubleCompareUtil` utility functions are well suited for
// comparing both final or intermediate values such as prices, volumes,
// interest rates, and the products and quotients thereof.   We'll now
// illustrate the use of some of the various `bdlb::DoubleCompareUtil`
// comparison methods on two `double` values `a` and `b`.  First we'll
// determine whether the two values are "CLOSE ENOUGH" using the
// implementation-defined (default) tolerances:
//```
//  if (bdlb::DoubleCompareUtil::fuzzyEq(a, b)) {
//      bsl::cout << "Values `a` and `b` are CLOSE ENOUGH.\n";
//  }
//```
// Next, we'll determine whether the same two values are "NOT RELATIVELY CLOSE"
// using our own (unusually large) criteria of 1.0 for relative tolerance, but
// continuing to rely on the default value for absolute tolerance:
//```
//  if (bdlb::DoubleCompareUtil::fuzzyNe(a, b, 1.0)) {
//      bsl::cout << "Values `a` and `b` are NOT RELATIVELY CLOSE.\n";
//  }
//```
// Finally, we'll determine if the value `a` is "SIGNIFICANTLY GREATER THAN"
// `b` by supplying our own rather larger values of 1e-1 and 1e-3 for the
// relative and absolute tolerances, respectively:
//```
//  if (bdlb::DoubleCompareUtil::fuzzyGt(a, b, 1e-1, 1e-3)) {
//      bsl::cout << "Value `a` is SIGNIFICANTLY GREATER THAN `b`.\n";
//  }
//```
// Which will print to `bsl::cout` if and only if:
//```
//  bdlb::DoubleCompareUtil::fuzzyNe(a, b, 1e-1, 1e-3) && a > b
//```
// is `true`.

#include <bdlscm_version.h>

namespace BloombergLP {
namespace bdlb {

                     // ==============================
                     // struct bdlb::DoubleCompareUtil
                     // ==============================

/// This struct provides a namespace for a suite of fuzzy (equality
/// and relational) comparison functions on pairs of `double` values,
/// parameterized by (optionally specified) relative and absolute
/// tolerances.  Note that all methods are naturally thread-safe, and
/// well-behaved for all tolerance values.
struct DoubleCompareUtil {
  public:
    // PUBLIC TYPES
    enum CompareResult {
        e_EQUAL          =  0,
        e_GREATER_THAN   =  1,
        e_LESS_THAN      = -1,
        e_NON_COMPARABLE = -127,
    };

  public:
    // PUBLIC CONSTANTS
    static const double k_DEFAULT_RELATIVE_TOLERANCE;
    static const double k_DEFAULT_ABSOLUTE_TOLERANCE;

  public:
    // CLASS METHODS

    /// Return `e_EQUAL` if the specified `a` and `b` have fuzzy equality,
    /// `e_GREATER_THAN` if `a > b`, `e_LESS_THAN` if `a < b`, and
    /// `e_NON_COMPARABLE` if either `a` or `b` are a NaN.  Optionally specify
    /// the relative tolerance `relTol`, or `relTol` and the absolute tolerance
    /// `absTol`, used to determine fuzzy equality.  If an optional tolerance
    /// argument is not specified, a reasonable (implementation-dependent)
    /// default value for that tolerance is used.  Fuzzy equality (denoted
    /// "a ~eq b") between `a` and `b` is defined in terms of the relative
    /// tolerance `relTol` and the absolute tolerance `absTol` such that the
    /// expression:
    ///```
    ///  a == b || fabs(a - b)                       <= absTol
    ///         || fabs(a - b) / fabs((a + b) / 2.0) <= relTol
    ///``
    /// is `true`; however, in the special case where `a != 0 && a == -b`
    /// is `true`, the actual relative difference is effectively infinite,
    /// and no value of `relTol` can imply fuzzy equality (although a
    /// sufficiently large value of `absTol` can).  Note that if either
    /// `absTol` or `relTol` is 0.0, that aspect of fuzzy comparison is
    /// effectively suppressed, but the behavior of this function is defined.
    /// The behavior is undefined unless both 'relTol` and `absTol` are
    /// non-negative, finite numbers (which excludes NaN and infinity).  Note
    /// also that the primary purpose of this public static comparison method
    /// is to implement the six fuzzy equality and relational functions also
    /// defined within this utility `struct` (see `fuzzyEq`, `fuzzyNe`,
    /// `fuzzyLt`, `fuzzyLe`, `fuzzyGt`, and `fuzzyGe`).
    static CompareResult fuzzyCompare(double a, double b);
    static CompareResult fuzzyCompare(double a, double b, double relTol);
    static CompareResult fuzzyCompare(double a,
                                      double b,
                                      double relTol,
                                      double absTol);

    /// Return `true` if the specified `a` and `b` satisfy the fuzzy
    /// equality relation (denoted "a ~eq b") as defined by the expression:
    ///```
    ///  fuzzyCompare(a, b, relTol, absTol) == e_EQUAL
    ///```
    /// and `false` otherwise.  Optionally specify the relative tolerance
    /// `relTol`, or `relTol` and the absolute tolerance `absTol`, used to
    /// determine fuzzy equality.  If an optional tolerance argument is
    /// not specified, a reasonable (implementation-dependent) default
    /// value for that tolerance is used.  Note that if either `absTol` or
    /// `relTol` is 0.0, that aspect of fuzzy comparison is effectively
    /// suppressed, but the behavior of this function is defined.  The behavior
    /// is undefined unless both 'relTol` and `absTol` are non-negative, finite
    /// numbers (which excludes NaN and infinity).
    static bool fuzzyEq(double a, double b);
    static bool fuzzyEq(double a, double b, double relTol);
    static bool fuzzyEq(double a, double b, double relTol, double absTol);

    /// Return `true` if the specified `a` and `b` satisfy the fuzzy
    /// inequality relation (denoted "a ~ne b") as defined by the
    /// expression:
    ///```
    ///  fuzzyCompare(a, b, relTol, absTol) != e_EQUAL
    ///```
    /// and `false` otherwise.  Optionally specify the relative tolerance
    /// `relTol`, or `relTol` and the absolute tolerance `absTol`, used to
    /// determine fuzzy inequality.  If an optional tolerance argument is
    /// not specified, a reasonable (implementation-dependent) default
    /// value for that tolerance is used.  Note that if either `absTol` or
    /// `relTol` is 0.0, that aspect of fuzzy comparison is effectively
    /// suppressed, but the behavior of this function is defined.  The behavior
    /// is undefined unless both 'relTol` and `absTol` are non-negative, finite
    /// numbers (which excludes NaN and infinity).
    static bool fuzzyNe(double a, double b);
    static bool fuzzyNe(double a, double b, double relTol);
    static bool fuzzyNe(double a, double b, double relTol, double absTol);

    /// Return `true` if the specified `a` and `b` satisfy the fuzzy
    /// less-than relation (denoted "a ~lt b") as defined by the expression:
    ///```
    ///  fuzzyCompare(a, b, relTol, absTol) == e_LESS_THAN
    ///```
    /// and `false` otherwise.  Optionally specify the relative tolerance
    /// `relTol`, or `relTol` and the absolute tolerance `absTol`, used to
    /// determine fuzzy less-than.  If an optional tolerance argument is
    /// not specified, a reasonable (implementation-dependent) default
    /// value for that tolerance is used.  Note that if either `absTol` or
    /// `relTol` is 0.0, that aspect of fuzzy comparison is effectively
    /// suppressed, but the behavior of this function is defined.  The behavior
    /// is undefined unless both 'relTol` and `absTol` are non-negative, finite
    /// numbers (which excludes NaN and infinity).
    static bool fuzzyLt(double a, double b);
    static bool fuzzyLt(double a, double b, double relTol);
    static bool fuzzyLt(double a, double b, double relTol, double absTol);

    /// Return `true` if the specified `a` and `b` satisfy the fuzzy
    /// less-equal relation (denoted "a ~le b") as defined by the
    /// expression:
    ///```
    ///  fuzzyCompare(a, b, relTol, absTol) == e_LESS_THAN or == e_EQUAL
    ///```
    /// and `false` otherwise.  Optionally specify the relative tolerance
    /// `relTol`, or `relTol` and the absolute tolerance `absTol`, used to
    /// determine fuzzy less-equal.  If an optional tolerance argument is
    /// not specified, a reasonable (implementation-dependent) default
    /// value for that tolerance is used.  Note that if either `absTol` or
    /// `relTol` is 0.0, that aspect of fuzzy comparison is effectively
    /// suppressed, but the behavior of this function is defined.  The behavior
    /// is undefined unless both 'relTol` and `absTol` are non-negative, finite
    /// numbers (which excludes NaN and infinity).
    static bool fuzzyLe(double a, double b);
    static bool fuzzyLe(double a, double b, double relTol);
    static bool fuzzyLe(double a, double b, double relTol, double absTol);

    /// Return `true` if the specified `a` and `b` satisfy the fuzzy
    /// greater-than relation (denoted "a ~lt b") as defined by the
    /// expression:
    ///```
    ///  fuzzyCompare(a, b, relTol, absTol) == e_GREATER_THAN
    ///```
    /// and `false` otherwise.  Optionally specify the relative tolerance
    /// `relTol`, or `relTol` and the absolute tolerance `absTol`, used to
    /// determine fuzzy greater-than.  If an optional tolerance argument is
    /// not specified, a reasonable (implementation-dependent) default
    /// value for that tolerance is used.  Note that if either `absTol` or
    /// `relTol` is 0.0, that aspect of fuzzy comparison is effectively
    /// suppressed, but the behavior of this function is defined.  The behavior
    /// is undefined unless both 'relTol` and `absTol` are non-negative, finite
    /// numbers (which excludes NaN and infinity).
    static bool fuzzyGt(double a, double b);
    static bool fuzzyGt(double a, double b, double relTol);
    static bool fuzzyGt(double a, double b, double relTol, double absTol);

    /// Return `true` if the specified `a` and `b` satisfy the fuzzy
    /// greater-equal relation (denoted "a ~ge b") as defined by the
    /// expression:
    ///..
    ///  fuzzyCompare(a, b, relTol, absTol) == e_GREATER_THAN or == e_EQUAL
    ///..
    /// and `false` otherwise.  Optionally specify the relative tolerance
    /// `relTol`, or `relTol` and the absolute tolerance `absTol`, used to
    /// determine fuzzy greater-equal.  If an optional tolerance argument
    /// is not specified, a reasonable (implementation-dependent) default
    /// value for that tolerance is used.  Note that if either `absTol` or
    /// `relTol` is 0.0, that aspect of fuzzy comparison is effectively
    /// suppressed, but the behavior of this function is defined.  The behavior
    /// is undefined unless both 'relTol` and `absTol` are non-negative, finite
    /// numbers (which excludes NaN and infinity).
    static bool fuzzyGe(double a, double b);
    static bool fuzzyGe(double a, double b, double relTol);
    static bool fuzzyGe(double a, double b, double relTol, double absTol);
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // ------------------------------
                     // struct bdlb::DoubleCompareUtil
                     // ------------------------------

// CLASS METHODS
inline
DoubleCompareUtil::CompareResult
DoubleCompareUtil::fuzzyCompare(double a, double b)
{
    return fuzzyCompare(a,
                        b,
                        k_DEFAULT_RELATIVE_TOLERANCE,
                        k_DEFAULT_ABSOLUTE_TOLERANCE);
}

inline
DoubleCompareUtil::CompareResult
DoubleCompareUtil::fuzzyCompare(double a, double b, double relTol)
{
    return fuzzyCompare(a, b, relTol, k_DEFAULT_ABSOLUTE_TOLERANCE);
}

inline
bool DoubleCompareUtil::fuzzyEq(double a, double b)
{
    return fuzzyCompare(a,
                        b,
                        k_DEFAULT_RELATIVE_TOLERANCE,
                        k_DEFAULT_ABSOLUTE_TOLERANCE) == e_EQUAL;
}

inline
bool DoubleCompareUtil::fuzzyEq(double a, double b, double relTol)
{
    return fuzzyCompare(a, b, relTol, k_DEFAULT_ABSOLUTE_TOLERANCE) == e_EQUAL;
}

inline
bool
DoubleCompareUtil::fuzzyEq(double a, double b, double relTol, double absTol)
{
    return fuzzyCompare(a, b, relTol, absTol) == e_EQUAL;
}

inline
bool DoubleCompareUtil::fuzzyNe(double a, double b)
{
    return fuzzyCompare(a,
                        b,
                        k_DEFAULT_RELATIVE_TOLERANCE,
                        k_DEFAULT_ABSOLUTE_TOLERANCE) != e_EQUAL;
}

inline
bool DoubleCompareUtil::fuzzyNe(double a, double b, double relTol)
{
    return fuzzyCompare(a, b, relTol, k_DEFAULT_ABSOLUTE_TOLERANCE) != e_EQUAL;
}

inline
bool
DoubleCompareUtil::fuzzyNe(double a, double b, double relTol, double absTol)
{
    return fuzzyCompare(a, b, relTol, absTol) != e_EQUAL;
}

inline
bool DoubleCompareUtil::fuzzyLt(double a, double b)
{
    return fuzzyCompare(a,
                        b,
                        k_DEFAULT_RELATIVE_TOLERANCE,
                        k_DEFAULT_ABSOLUTE_TOLERANCE) == e_LESS_THAN;
}

inline
bool DoubleCompareUtil::fuzzyLt(double a, double b, double relTol)
{
    return fuzzyCompare(a,
                        b,
                        relTol,
                        k_DEFAULT_ABSOLUTE_TOLERANCE) == e_LESS_THAN;
}

inline
bool
DoubleCompareUtil::fuzzyLt(double a, double b, double relTol, double absTol)
{
    return fuzzyCompare(a, b, relTol, absTol) == e_LESS_THAN;
}

inline
bool DoubleCompareUtil::fuzzyLe(double a, double b)
{
    const CompareResult rv = fuzzyCompare(a,
                                          b,
                                          k_DEFAULT_RELATIVE_TOLERANCE,
                                          k_DEFAULT_ABSOLUTE_TOLERANCE);

    return rv == e_LESS_THAN || rv == e_EQUAL;
}

inline
bool DoubleCompareUtil::fuzzyLe(double a, double b, double relTol)
{
    const CompareResult rv =
                      fuzzyCompare(a, b, relTol, k_DEFAULT_ABSOLUTE_TOLERANCE);

    return rv == e_LESS_THAN || rv == e_EQUAL;
}

inline
bool
DoubleCompareUtil::fuzzyLe(double a, double b, double relTol, double absTol)
{
    const CompareResult rv = fuzzyCompare(a, b, relTol, absTol);

    return rv == e_LESS_THAN || rv == e_EQUAL;
}

inline
bool DoubleCompareUtil::fuzzyGt(double a, double b)
{
    return fuzzyCompare(a,
                        b,
                        k_DEFAULT_RELATIVE_TOLERANCE,
                        k_DEFAULT_ABSOLUTE_TOLERANCE) == e_GREATER_THAN ;
}

inline
bool DoubleCompareUtil::fuzzyGt(double a, double b, double relTol)
{
    return fuzzyCompare(a,
                        b,
                        relTol,
                        k_DEFAULT_ABSOLUTE_TOLERANCE) == e_GREATER_THAN;
}

inline
bool
DoubleCompareUtil::fuzzyGt(double a, double b, double relTol, double absTol)
{
    return fuzzyCompare(a, b, relTol, absTol) == e_GREATER_THAN;
}

inline
bool DoubleCompareUtil::fuzzyGe(double a, double b)
{
    const CompareResult rv = fuzzyCompare(a,
                                          b,
                                          k_DEFAULT_RELATIVE_TOLERANCE,
                                          k_DEFAULT_ABSOLUTE_TOLERANCE);

    return rv == e_GREATER_THAN || rv == e_EQUAL;
}

inline
bool DoubleCompareUtil::fuzzyGe(double a, double b, double relTol)
{
    const CompareResult rv = fuzzyCompare(a,
                                          b,
                                          relTol,
                                          k_DEFAULT_ABSOLUTE_TOLERANCE);

    return rv == e_GREATER_THAN || rv == e_EQUAL;
}

inline
bool
DoubleCompareUtil::fuzzyGe(double a, double b, double relTol, double absTol)
{
    const CompareResult rv = fuzzyCompare(a, b, relTol, absTol);

    return rv == e_GREATER_THAN || rv == e_EQUAL;
}

}  // close package namespace
}  // close corporate namespace

#endif

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
