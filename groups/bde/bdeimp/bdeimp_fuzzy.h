// bdeimp_fuzzy.h               -*-C++-*-
#ifndef INCLUDED_BDEIMP_FUZZY
#define INCLUDED_BDEIMP_FUZZY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide "fuzzy" equality/relational operations on 'double' values.
//
//@CLASSES:
//   bdeimp_Fuzzy: namespace for "fuzzy" comparison functions
//
//@AUTHOR: Tom Marshall (tmarshall)
//
//@DESCRIPTION: This component implements a utility class that performs "fuzzy"
// equality and relational operations as a suite of functions, each of which
// operates on a pair of 'double' values 'a' and 'b'.  The term "fuzzy
// equality" expresses the notion that 'a' and 'b' are "close enough", and
// that any difference that may exist between the values implied by the
// physical representations of 'a' and 'b' is not significant, and should be
// ignored for comparison purposes.
//
// More formally, the *absolute* *difference* between 'a' and 'b' is defined
// as the absolute value of the difference of 'a' and 'b':
//..
//  fabs(a - b)                              // absolute difference
//..
// The *relative* *difference* is defined as the quotient of the absolute
// difference and the absolute value of the average of 'a' and 'b'.
//..
//  fabs(a - b) / fabs((a + b) / 2.0)        // relative difference
//..
// Objects 'a' and 'b' have fuzzy equality if they have the same value, or if
// either their absolute difference is less than or equal to some
// user-specified (or default) tolerance value 'absTol', or their relative
// difference is less or equal to some separate, user-specified (or default)
// tolerance value 'relTol'.  Using the notation "A ::= B" to mean
// "A is defined as B", we define fuzzy equality (denoted "~eq") as follows:
//..
//  a ~eq b ::= a == b
//           || fabs(a - b)                       <= absTol
//           || fabs(a - b) / fabs((a + b) / 2.0) <= relTol
//..
// where 'fabs(double)' is the standard C absolute-value function and all
// symbols other than '~' and '::=' have their usual C++ meaning.  Note that
// the fuzzy equality operation is symmetric (i.e., commutative) with respect
// to 'a' and 'b' for *all* values of 'relTol' and 'absTol'.
//
// The following table identifies the six fuzzy equality and relational
// operations corresponding to the standard C++ operators, along with the
// associated static member functions defined within the 'bdeimp_Fuzzy'
// utility 'struct' (note that the parameters 'relTol' and 'absTol' are
// optional, and default to reasonable, implementation-dependent values):
//..
//  C++ operator   fuzzy operation   qualified static member-function name
//  ------------   ---------------   --------------------------------------
//       ==             ~eq          bdeimp_Fuzzy::eq(a, b, relTol, absTol)
//       !=             ~ne          bdeimp_Fuzzy::ne(a, b, relTol, absTol)
//       <              ~lt          bdeimp_Fuzzy::lt(a, b, relTol, absTol)
//       <=             ~le          bdeimp_Fuzzy::le(a, b, relTol, absTol)
//       >              ~gt          bdeimp_Fuzzy::gt(a, b, relTol, absTol)
//       >=             ~ge          bdeimp_Fuzzy::ge(a, b, relTol, absTol)
//..
// Each of the other fuzzy (inequality and relational) operations is defined
// with respect to fuzzy equality:
//..
//  a ~ne b  ::=  !(a ~eq b)
//  a ~lt b  ::=  !(a ~eq b) && (a < b)
//  a ~le b  ::=   (a ~eq b) || (a < b)
//  a ~gt b  ::=  !(a ~eq b) && (a > b)
//  a ~ge b  ::=   (a ~eq b) || (a > b)
//..
///Example
///-------
// To illustrate precise behavior, the following example tabulates the
// numerical results of calling the six fuzzy-comparison functions on a set of
// carefully selected inputs 'x' and 'y', where 'relTol' and 'absTol' are,
// respectively the relative and absolute tolerances (the actual relative and
// absolute differences of 'x' and 'y', rounded to five decimal digits, are
// tabulated, for reference, in the two columns to the extreme right):
//..
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
//..
///Valid Use, Limitations, and Caveats
///------------------------------------
// Each of the functions implemented in this component are well behaved for all
// values of 'relTol' and 'absTol': If either 'relTol' or 'absTol' is
// non-positive, that aspect of "fuzzy comparison" is suppressed; if both are
// non-positive, each of the the six fuzzy-comparison operations behave as
// runtime-intensive versions of their non-fuzzy counterparts.
//
// Note that the definition of fuzzy equality used in this component does have
// one intermediate singularity: When '(fabs(a - b) > absTol && (a == -b))' is
// true, the pseudo-expression 'a ~eq b' defined above has a zero denominator.
// In this case, the test for relative fuzzy equality is suppressed.  This
// intermediate singularity does not, however, lead to a special-case behavior
// of fuzzy comparisons: By definition, the relative difference is the quotient
// of the absolute difference and the absolute average, so the case '(a == -b)'
// truly represents an "infinite relative difference", and thus fuzzy equality
// via the relative difference criteria should be false (although absolute
// fuzzy equality may still prevail).
//
// Finally, note that the implementations of the functions in this component
// are vulnerable to the limitations of values that can be represented by
// a 'double'.  In particular, if '(a + b)' or '(a - b)' cannot be represented,
// the functions will fail outright.  More subtly, as 'a' or 'b' approaches the
// limits of precision of representation, the algorithms used in this component
// become increasingly unreliable.  The user is responsible for determining
// the limits of applicability of this component to a given calculation, and
// for coding accordingly.
//
///Usage
///-----
// The 'bdeimp_Fuzzy' utility functions are well suited for comparing both
// final or intermediate values such as prices, volumes, interest rates, and
// the products and quotients thereof.   We'll now illustrate the use of some
// of the various 'bdeimp_Fuzzy' comparison methods on two 'double' values 'a'
// 'a' and 'b'.  First we'll determine whether the two values are
// "CLOSE ENOUGH" using the implementation-defined (default) tolerances:
//..
//  if (bdeimp_Fuzzy::eq(a, b)) {
//      bsl::cout << "Values 'a' and 'b' are CLOSE ENOUGH." << bsl::endl;
//  }
//..
// Next, we'll determine whether the same two values are "NOT RELATIVELY CLOSE"
// using our own (unusually large) criteria of 1.0 for relative tolerance, but
// continuing to rely on the default value for absolute tolerance:
//..
//  if (bdeimp_Fuzzy::ne(a, b, 1.0)) {
//      bsl::cout << "Values 'a' and 'b' are NOT RELATIVELY CLOSE."
//                                                                << bsl::endl;
//  }
//..
// Finally, we'll determine if the value 'a' is "SIGNIFICANTLY GREATER THAN"
// 'b' by supplying our own rather larger values of 1e-1 and 1e-3 for the
// relative and absolute tolerances, respectively:
//..
//  if (bdeimp_Fuzzy::gt(a, b, 1e-1, 1e-3)) {
//      bsl::cout << "Value 'a' is SIGNIFICANTLY GREATER THAN 'b'."
//                                                                << bsl::endl;
//  }
//..
// Which will print to 'bsl::cout' if and only if:
//..
//  bdeimp_fuzzy::ne(a, b, 1e-1, 1e-3) && a > b
//..
// is true.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

                        // ==================
                        // class bdeimp_Fuzzy
                        // ==================

class bdeimp_Fuzzy {
    // This class provides a namespace for a suite of fuzzy (equality
    // and relational) comparison functions on pairs of 'double' values,
    // parameterized by (optionally specified) relative and absolute
    // tolerances.  Note that all methods are naturally thread-safe, and
    // well-behaved for all tolerance values.

    // CLASS DATA
    static const double S_DEFAULT_REL_TOL;  // default relative tolerance
    static const double S_DEFAULT_ABS_TOL;  // default absolute tolerance

  public:
    // CLASS METHODS
    static int compare(double a, double b);
    static int compare(double a, double b, double relTol);
    static int compare(double a, double b, double relTol, double absTol);
        // Return 0 if the specified 'a' and 'b' have fuzzy equality and a
        // non-zero value otherwise, which will be positive if 'a > b' and
        // negative if 'a < b'.  Optionally specify the relative tolerance
        // 'relTol', or 'relTol' and the absolute tolerance 'absTol', used to
        // determine fuzzy equality.  If an optional tolerance argument is not
        // specified, a reasonable (implementation-dependent) default value
        // for that tolerance is used.  Fuzzy equality (denoted "a ~eq b")
        // between 'a' and 'b' is defined in terms of the relative tolerance
        // 'relTol' and the absolute tolerance 'absTol' such that the
        // expression:
        //..
        //  a == b || fabs(a - b)                       <= absTol
        //         || fabs(a - b) / fabs((a + b) / 2.0) <= relTol
        //..
        // is 'true'; however, in the special case where 'a != 0 && 'a == -b'
        // is 'true', the actual relative difference is effectively infinite,
        // and no value of 'relTol can imply fuzzy equality (although a
        // sufficiently large value of 'absTol' can).  Note that if either
        // 'absTol' or 'relTol' is non-positive, that aspect of fuzzy
        // comparison is effectively suppressed, but the behavior of this
        // function is defined (i.e., the behavior is the same as if the
        // tolerance were 0.0).  Note also that the primary purpose of this
        // public static comparison method is to implement the six fuzzy
        // equality and relational functions also defined within this utility
        // 'struct' (see 'eq', 'ne', 'lt', 'le', 'gt', and 'ge').

    static bool eq(double a, double b);
    static bool eq(double a, double b, double relTol);
    static bool eq(double a, double b, double relTol, double absTol);
        // Return 'true' if the specified 'a' and 'b' satisfy the fuzzy
        // equality relation (denoted "a ~eq b") as defined by the expression:
        //..
        //  compare(a, b, relTol, absTol) == 0
        //..
        // and 'false' otherwise.  Optionally specify the relative tolerance
        // 'relTol', or 'relTol' and the absolute tolerance 'absTol', used to
        // determine fuzzy equality.  If an optional tolerance argument is
        // not specified, a reasonable (implementation-dependent) default
        // value for that tolerance is used.  Note that a non-positive
        // tolerance value is valid and effectively suppresses that aspect of
        // fuzzy comparison.

    static bool ne(double a, double b);
    static bool ne(double a, double b, double relTol);
    static bool ne(double a, double b, double relTol, double absTol);
        // Return 'true' if the specified 'a' and 'b' satisfy the fuzzy
        // inequality relation (denoted "a ~ne b") as defined by the
        // expression:
        //..
        //  compare(a, b, relTol, absTol) != 0
        //..
        // and 'false' otherwise.  Optionally specify the relative tolerance
        // 'relTol', or 'relTol' and the absolute tolerance 'absTol', used to
        // determine fuzzy inequality.  If an optional tolerance argument is
        // not specified, a reasonable (implementation-dependent) default
        // value for that tolerance is used.  Note that a non-positive
        // tolerance value is valid and effectively suppresses that aspect of
        // fuzzy comparison.

    static bool lt(double a, double b);
    static bool lt(double a, double b, double relTol);
    static bool lt(double a, double b, double relTol, double absTol);
        // Return 'true' if the specified 'a' and 'b' satisfy the fuzzy
        // less-than relation (denoted "a ~lt b") as defined by the expression:
        //..
        //  compare(a, b, relTol, absTol) < 0
        //..
        // and 'false' otherwise.  Optionally specify the relative tolerance
        // 'relTol', or 'relTol' and the absolute tolerance 'absTol', used to
        // determine fuzzy less-than.  If an optional tolerance argument is
        // not specified, a reasonable (implementation-dependent) default
        // value for that tolerance is used.  Note that a non-positive
        // tolerance value is valid and effectively suppresses that aspect of
        // fuzzy comparison.

    static bool le(double a, double b);
    static bool le(double a, double b, double relTol);
    static bool le(double a, double b, double relTol, double absTol);
        // Return 'true' if the specified 'a' and 'b' satisfy the fuzzy
        // less-equal relation (denoted "a ~le b") as defined by the
        // expression:
        //..
        //  compare(a, b, relTol, absTol) <= 0
        //..
        // and 'false' otherwise.  Optionally specify the relative tolerance
        // 'relTol', or 'relTol' and the absolute tolerance 'absTol', used to
        // determine fuzzy less-equal.  If an optional tolerance argument is
        // not specified, a reasonable (implementation-dependent) default
        // value for that tolerance is used.  Note that a non-positive
        // tolerance value is valid and effectively suppresses that aspect of
        // fuzzy comparison.

    static bool gt(double a, double b);
    static bool gt(double a, double b, double relTol);
    static bool gt(double a, double b, double relTol, double absTol);
        // Return 'true' if the specified 'a' and 'b' satisfy the fuzzy
        // greater-than relation (denoted "a ~lt b") as defined by the
        // expression:
        //..
        //  compare(a, b, relTol, absTol) > 0
        //..
        // and 'false' otherwise.  Optionally specify the relative tolerance
        // 'relTol', or 'relTol' and the absolute tolerance 'absTol', used to
        // determine fuzzy greater-than.  If an optional tolerance argument is
        // not specified, a reasonable (implementation-dependent) default
        // value for that tolerance is used.  Note that a non-positive
        // tolerance value is valid and effectively suppresses that aspect of
        // fuzzy comparison.

    static bool ge(double a, double b);
    static bool ge(double a, double b, double relTol);
    static bool ge(double a, double b, double relTol, double absTol);
        // Return 'true' if the specified 'a' and 'b' satisfy the fuzzy
        // greater-equal relation (denoted "a ~ge b") as defined by the
        // expression:
        //..
        //  compare(a, b, relTol, absTol) >= 0
        //..
        // and 'false' otherwise.  Optionally specify the relative tolerance
        // 'relTol', or 'relTol' and the absolute tolerance 'absTol', used to
        // determine fuzzy greater-equal.  If an optional tolerance argument
        // is not specified, a reasonable (implementation-dependent) default
        // value for that tolerance is used.  Note that a non-positive
        // tolerance value is valid and effectively suppresses that aspect of
        // fuzzy comparison.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

     static int fuzzyCompare(double a, double b, double relTol, double absTol);
        // Return 0 if the specified 'a' and 'b' have fuzzy equality (denoted
        // "a ~eq b").  Otherwise, return a positive value if 'a > b', or a
        // negative value if 'a < b'.  Fuzzy equality between 'a' and 'b' is
        // defined in terms of the specified 'relTol' and 'absTol' such that
        // the expression:
        //..
        //  a == b ||  fabs(a - b)                        <= absTol
        //         || (fabs(a - b) / fabs((a + b) / 2.0)) <= relTol
        //..
        // evaluates to 'true'.  As a consequence of this definition of
        // relative difference, the special case of 'a == -b && a != 0' is
        // treated as the maximum relative difference: no value of 'relTol' can
        // force fuzzy equality (although an appropriate value of 'absTol'
        // can).  If 'relTol <= 0' or 'absTol <= 0', the respective aspect of
        // fuzzy comparison is suppressed, but the function is still valid.
        // Note that although this function may be called directly, its primary
        // purpose is to implement the fuzzy quasi-boolean equality and
        // relational functions named 'eq', 'ne', 'lt', 'le', 'ge', and 'gt',
        // corresponding to the operators '==', '!=', '<', '<=', '>=', and '>',
        // respectively.
        //
        // DEPRECATED: use 'compare' instead.

#endif

};

// ============================================================================
//                       STATIC INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------
                        // class bdeimp_Fuzzy
                        // ------------------

// CLASS METHODS
inline
int bdeimp_Fuzzy::compare(double a, double b)
{
    return compare(a, b, S_DEFAULT_REL_TOL, S_DEFAULT_ABS_TOL);
}

inline
int bdeimp_Fuzzy::compare(double a, double b, double relTol)
{
    return compare(a, b, relTol, S_DEFAULT_ABS_TOL);
}

inline
bool bdeimp_Fuzzy::eq(double a, double b)
{
    return compare(a, b, S_DEFAULT_REL_TOL, S_DEFAULT_ABS_TOL) == 0;
}

inline
bool bdeimp_Fuzzy::eq(double a, double b, double relTol)
{
    return compare(a, b, relTol, S_DEFAULT_ABS_TOL) == 0;
}

inline
bool bdeimp_Fuzzy::eq(double a, double b, double relTol, double absTol)
{
    return compare(a, b, relTol, absTol) == 0;
}

inline
bool bdeimp_Fuzzy::ne(double a, double b)
{
    return compare(a, b, S_DEFAULT_REL_TOL, S_DEFAULT_ABS_TOL) != 0;
}

inline
bool bdeimp_Fuzzy::ne(double a, double b, double relTol)
{
    return compare(a, b, relTol, S_DEFAULT_ABS_TOL) != 0;
}

inline
bool bdeimp_Fuzzy::ne(double a, double b, double relTol, double absTol)
{
    return compare(a, b, relTol, absTol) != 0;
}

inline
bool bdeimp_Fuzzy::lt(double a, double b)
{
    return compare(a, b, S_DEFAULT_REL_TOL, S_DEFAULT_ABS_TOL) < 0;
}

inline
bool bdeimp_Fuzzy::lt(double a, double b, double relTol)
{
    return compare(a, b, relTol, S_DEFAULT_ABS_TOL) < 0;
}

inline
bool bdeimp_Fuzzy::lt(double a, double b, double relTol, double absTol)
{
    return compare(a, b, relTol, absTol) < 0;
}

inline
bool bdeimp_Fuzzy::le(double a, double b)
{
    return compare(a, b, S_DEFAULT_REL_TOL, S_DEFAULT_ABS_TOL) <= 0;
}

inline
bool bdeimp_Fuzzy::le(double a, double b, double relTol)
{
    return compare(a, b, relTol, S_DEFAULT_ABS_TOL) <= 0;
}

inline
bool bdeimp_Fuzzy::le(double a, double b, double relTol, double absTol)
{
    return compare(a, b, relTol, absTol) <= 0;
}

inline
bool bdeimp_Fuzzy::gt(double a, double b)
{
    return compare(a, b, S_DEFAULT_REL_TOL, S_DEFAULT_ABS_TOL) > 0;
}

inline
bool bdeimp_Fuzzy::gt(double a, double b, double relTol)
{
    return compare(a, b, relTol, S_DEFAULT_ABS_TOL) > 0;
}

inline
bool bdeimp_Fuzzy::gt(double a, double b, double relTol, double absTol)
{
    return compare(a, b, relTol, absTol) > 0;
}

inline
bool bdeimp_Fuzzy::ge(double a, double b)
{
    return compare(a, b, S_DEFAULT_REL_TOL, S_DEFAULT_ABS_TOL) >= 0;
}

inline
bool bdeimp_Fuzzy::ge(double a, double b, double relTol)
{
    return compare(a, b, relTol, S_DEFAULT_ABS_TOL) >= 0;
}

inline
bool bdeimp_Fuzzy::ge(double a, double b, double relTol, double absTol)
{
    return compare(a, b, relTol, absTol) >= 0;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

inline
int bdeimp_Fuzzy::fuzzyCompare(double a,
                               double b,
                               double relTol,
                               double absTol)
{
    return compare(a, b, relTol, absTol);
}

#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
