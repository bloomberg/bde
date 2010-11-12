// bdes_float.h                  -*-C++-*-
#ifndef INCLUDED_BDES_FLOAT
#define INCLUDED_BDES_FLOAT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide floating-point classification types and functions.
//
//@CLASSES:
// bdes_Float: namespace for floating-point classification types and functions
//
//@SEE_ALSO: bdeimp_fuzzy
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: In C99, the '<math.h>' library includes macros such as 'isinf'
// and 'isnan' that return whether a 'float' or 'double' value is infinite or
// not-a-number, respectively.  These macros are not available in C++98 and so
// are provided as functions in this component.
//
///Classification of Floating-Point Numbers
///----------------------------------------
// Floating-point numbers are used to represent a subset of the set
// of real numbers.  The C++ 'float' and 'double' types are used to hold
// floating-point numbers, with 'float' having (much) less precision than
// 'double'.  Floating point numbers can be classified into the following
// disjoint sets:
//..
//  Zero       positive and negative zero
//  Normal     full-precision, non-zero, normal numbers
//  Subnormal  reduced-precision numbers with small absolute values
//  Infinity   positive and negative infinities
//  NaN        not a number
//..
// A NaN value can be further classified into two disjoint subsets:
//..
//  Signaling NaN  invalid values that raises a signal in computations
//  Quiet NaN      indeterminate values that propagate through computations
//..
// Note that not all platforms support signaling NaNs and that even those that
// do often require a specific action to enable signals on floating-point
// traps.  Signaling NaNs are never the result of a normal floating-point
// operation.  They are most often used as sentinels to detect the use of a
// value that has not yet been computed.
//
// Quiet NaNs are the result of certain operations where the result is not
// defined mathematically.  If an expression that results in a (quiet) NaN is
// used in a subsequent computation, the result is usually also a (quiet)
// NaN.
//
// On platforms that implement the IEEE 754 standard for floating-point
// arithmetic, the following conditions result in non-normal floating-point
// values.  In the following table, "NaN" aways refers to a quiet NaN:
//..
//  Condition                   Result
//  -------------------         -----------------
//  Overflow                    Infinity
//  Underflow                   Subnormal or Zero
//  Normal / Infinity           Zero
//  Infinity * Infinity         Infinity
//  nonzero / Zero              Infinity
//  Infinity + Infinity         Infinity
//  Zero / Zero                 NaN
//  Infinity - Infinity         NaN
//  Infinity / Infinity         NaN
//  Infinity * Zero             NaN
//..
// Note that the operations that result in Infinity follow the normal rules
// for sign propagation, e.g., -5.0 / 0.0 results in negative Infinity.
//
///Future Enhancements
///-------------------
// At present, this component works with 'float' and 'double' numbers.  In the
// future, it will also work with 'long double'.  Note that casting a 'long
// double' to 'double' before applying these classification functions will not
// always yield correct results.  For example, a large 'long double' may get
// demoted to an infinite 'double'.  Similarly, a small 'long double' may get
// demoted to a subnormal or zero 'double'.
//
///Thread Safety
///-------------
// Any of the functions in this component may safely be called simultaneously
// from multiple threads, even with the same arguments.
//
///Usage
///-----
// On platforms that the implement IEEE 754 standard for floating-point
// arithmetic, dividing a positive number by zero yields positive infinity and
// dividing a negative number by zero yields negative infinity.  The result of
// division by zero will therefore be detected as infinite by the 'isInfinite'
// method and classified as infinity by the 'classify' and 'classifyFine'
// methods in this component:
//..
//  double zero = 0.0;
//  double a = 2.3 / zero;
//  double b = -0.55 / zero;
//  assert(true == bdes_Float::isZero(zero));
//  assert(true == bdes_Float::isInfinite(a));
//  assert(true == bdes_Float::isInfinite(b));
//  assert(bdes_Float::BDES_ZERO     == bdes_Float::classify(zero));
//  assert(bdes_Float::BDES_INFINITE == bdes_Float::classify(a));
//  assert(bdes_Float::BDES_INFINITE == bdes_Float::classify(b));
//  assert(bdes_Float::BDES_POSITIVE_INFINITY == bdes_Float::classifyFine(a));
//  assert(bdes_Float::BDES_NEGATIVE_INFINITY == bdes_Float::classifyFine(b));
//..
// The result of multiplying infinity by infinity is also infinity, but the
// result of multiplying infinity by zero is an indeterminate value (quiet
// NaN):
//..
//  double c = a * b;
//  double d = a * zero;
//  assert(true  == bdes_Float::isInfinite(c));
//  assert(false == bdes_Float::isInfinite(d));
//  assert(true  == bdes_Float::isNan(d));
//  assert(true  == bdes_Float::isQuietNan(d));
//  assert(false == bdes_Float::isSignalingNan(d));
//..
// Quiet NaNs propagate such that further calculations also yield quiet NaNs:
//..
//  double g = d - 3.4e12;
//  assert(false == bdes_Float::isInfinite(g));
//  assert(true  == bdes_Float::isNan(g));
//  assert(true  == bdes_Float::isQuietNan(g));
//..
// We can also detect whether a value has full precision (normal) or is so
// small (close to zero) that precision has been lost (subnormal):
//..
//  double e = -10.0 / 11.0; // Full precision
//  double f = e / DBL_MAX; // Lost precision
//  assert(true  == bdes_Float::isNormal(e));
//  assert(false == bdes_Float::isSubnormal(e));
//  assert(false == bdes_Float::isNormal(f));
//  assert(true  == bdes_Float::isSubnormal(f));
//  assert(bdes_Float::BDES_NORMAL    == bdes_Float::classify(e));
//  assert(bdes_Float::BDES_SUBNORMAL == bdes_Float::classify(f));
//..
// The 'Classification' enumeration type is designed so that each
// classification occupies a separate bit.  This makes it easy to test for
// multiple classifications in one test.  For example, if we are interested in
// very that zero or denormalized (i.e., very small), we can detect both
// conditions with a single mask:
//..
//  const int SMALL_MASK = bdes_Float::BDES_ZERO | bdes_Float::BDES_SUBNORMAL;
//  assert(0 != (SMALL_MASK & bdes_Float::classify(0.0)));
//  assert(0 != (SMALL_MASK & bdes_Float::classify(f)));
//  assert(0 == (SMALL_MASK & bdes_Float::classify(e)));
//..
// Note, however, that although we can create a mask with several
// classification bits, a single number belongs to only one classification and
// the return value of 'classify' will have only one bit set at a time.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

namespace BloombergLP {

                        // =================
                        // struct bdes_Float
                        // =================

struct bdes_Float {
    // Namespace for floating-point classification types and functions.

    // TYPES
    enum Classification {
        // Basic classifications for floating-point numbers.  Every
        // floating-point number belongs to exactly one of these
        // classifications.  However, the enumerated values have disjoint
        // bit-patterns to make it easy to create a "set" of classifications
        // using bit-wise OR.

        BDES_ZERO      = 0x01, // positive or negative zero
        BDES_NORMAL    = 0x02, // full-precision, non-zero, normal number
        BDES_SUBNORMAL = 0x04, // reduced-precision numb with a small abs value
        BDES_INFINITE  = 0x08, // positive or negative infinity
        BDES_NAN       = 0x10  // not a number
    };

    enum FineClassification {
        // Fine-grained classifications for floating-point numbers that
        // distinguish positive numbers from negative numbers and quiet NaNs
        // from signaling NaNs.  Every floating-point number belongs to
        // exactly one of these classifications.

        BDES_NEGATIVE           = 0x8000,  // Bit for negative floats
        BDES_SIGNALING          = 0x4000,  // Bit for signaling NaNs

        BDES_POSITIVE_INFINITY  = BDES_INFINITE,
        BDES_NEGATIVE_INFINITY  = BDES_INFINITE | BDES_NEGATIVE,
        BDES_QNAN               = BDES_NAN,
        BDES_SNAN               = BDES_NAN | BDES_SIGNALING,
        BDES_POSITIVE_NORMAL    = BDES_NORMAL,
        BDES_NEGATIVE_NORMAL    = BDES_NORMAL | BDES_NEGATIVE,
        BDES_POSITIVE_SUBNORMAL = BDES_SUBNORMAL,
        BDES_NEGATIVE_SUBNORMAL = BDES_SUBNORMAL | BDES_NEGATIVE,
        BDES_POSITIVE_ZERO      = BDES_ZERO,
        BDES_NEGATIVE_ZERO      = BDES_ZERO | BDES_NEGATIVE
    };

    // CLASS METHODS
    static Classification classify(float number);
    static Classification classify(double number);
        // Return the course classification ('BDES_ZERO', 'BDES_NORMAL',
        // 'BDES_SUBNORMAL', 'BDES_INFINITE', or 'BDES_NAN) for the specified
        // floating point 'number'.  This function has the same functionality
        // as the 'fpclassify' macro in C99.

    static FineClassification classifyFine(float number);
    static FineClassification classifyFine(double number);
        // Return the fine-grained classification for the specified floating
        // point 'number'.  For positive numbers and quiet NaNs, the
        // 'classifyFine' returns the same integer value as 'classify'.  For
        // negative numbers, 'classifyFine' returns and integer value equal to
        // value returned by 'classify' bit-wise ORed with 'BDES_NEGATIVE'.
        // For signaling NaNs, 'classifyFine' returns 'BDES_SNAN', which has
        // the integer value of 'BDES_NAN | BDES_SIGNALING'.

    static bool isZero(float number);
    static bool isZero(double number);
        // Return true if the specified floating point 'number' has a value of
        // positive or negative zero, and false otherwise.

    static bool isNormal(float number);
    static bool isNormal(double number);
        // Return true if the specified floating point 'number' holds a normal
        // value (neither zero, subnormal, infinite, nor NaN).  This function
        // is equivalent to the 'isnormal' macro in C99.

    static bool isSubnormal(float number);
    static bool isSubnormal(double number);
        // Return true if the specified floating point 'number' holds a
        // subnormal value, and false otherwise.

    static bool isInfinite(float number);
    static bool isInfinite(double number);
        // Return true if the specified floating point 'number' has a value of
        // positive or negative infinity, and false otherwise.  This function
        // is equivalent to the 'isinf' macro in C99.  Note that infinity is a
        // valid floating-point value and is not a "NaN".

    static bool isNan(float number);
    static bool isNan(double number);
        // Return true if the specified floating point 'number' has a value
        // that does not represent a real number ("not-a-number" or "NaN"),
        // and false otherwise.  This function is equivalent to the 'isnan'
        // macro in C99.  Note that if this method returns true, then either
        // 'isQuietNan' or 'isSignalingNan' will also return true.

    static bool signBit(float number);
    static bool signBit(double number);
        // Return true if the specified floating point 'number' has its sign
        // bit set (i.e., it is negative), and false otherwise.  This function
        // is equivalent to the 'signbit' macro in C99.  Note that this
        // function will return true for some NaNs, even though the concepts
        // of negative and positive do not apply to NaNs.

    static bool isFinite(float number);
    static bool isFinite(double number);
        // Return true if the specified floating point 'number' is normal,
        // subnormal or zero, and false if 'number' is infinite or NaN.  This
        // function is equivalent to the 'isfinite' macro in C99.

    static bool isQuietNan(float number);
    static bool isQuietNan(double number);
        // Return true if the specified floating point 'number' has an
        // indeterminate value, and false otherwise.  An indeterminate
        // floating-point value ("quiet NaN" or "QNaN") results from an
        // operation for which the result is not mathematically defined, such
        // as multiplying infinity by zero.  If a QNaN is used in a subsequent
        // operations the result will also be a QNaN.  Note that, because a
        // QNaN is a NaN, if this method returns true, then 'isNan(x)' will
        // also return true.

    static bool isSignalingNan(float number);
    static bool isSignalingNan(double number);
        // Return true if the specified floating point 'number' has an
        // invalid value and false otherwise.  An invalid floating-point value
        // ("signaling NaN" or "SNaN")is never the result of a valid operation
        // -- it must be produced deliberately (i.e., by calling
        // 'bsl::numeric_limits<float>::signaling_NaN()').  If an SNaN is used
        // in a subsequent operation, the result is undefined and may result
        // in a hardware trap leading to a signal.  Some platforms do not
        // support signaling NaNs, especially for single-precision floats, and
        // will convert an SNaN to a QNaN on assignment or copy-initialization
        // (including argument passing).  'isSignalingNan' will always return
        // false on such platforms.  Note that, because an SNaN is a NaN, if
        // this method returns true, then 'isNan(x)' will also return true.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
