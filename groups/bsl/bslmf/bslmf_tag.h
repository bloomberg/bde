// bslmf_tag.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLMF_TAG
#define INCLUDED_BSLMF_TAG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an integral-constant-to-type conversion.
//
//@CLASSES:
// bslmf_Tag: map integral constants to C++ types
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// map an integral constant to a C++ type.  'bslmf_Tag<int>' defines a
// different type for each distinct compile-time constant integral parameter.
// That is, instantiations with different integer values form distinct types,
// so that 'bslmf_Tag<0>' is a distinct type from 'bslmf_Tag<1>', which
// is also distinct from 'bslmf_Tag<2>', and so on.
//
// The most common use of this structure is to perform static function
// dispatching based on a compile-time calculation.  Often the calculation is
// nothing more than a simple predicate, allowing us to select one of two
// functions.  The following function, 'doSomething', uses a fast
// implementation (e.g., 'memcpy') if the parameterized type allows for such
// operations, otherwise it will use a more generic and slower implementation
// (e.g., copy constructor).
//..
//       template <class T>
//       void doSomethingImp(T *t, bslmf_Tag<0>*)
//       {
//           // slow but generic implementation
//       }
//
//       template <class T>
//       void doSomethingImp(T *t, bslmf_Tag<1>*)
//       {
//           // fast implementation (works only for some T's)
//       }
//
//       template <class T, bool IsFast>
//       void doSomething(T *t)
//       {
//           doSomethingImp(t, (bslmf_Tag<IsFast>*)0);
//       }
//..
// For some parameter types, the fast version of 'doSomethingImp' is not legal.
// The power of this approach is that the compiler will compile only the
// implementation selected by the tag argument.
//..
//      void f()
//      {
//          int i;
//          doSomething<int, true>(&i); // fast version selected for int
//
//          double m;
//          doSomething<double, false>(&m); // slow version selected for double
//      }
//..
// Note that an alternative design would be to use template partial
// specialization instead of standard function overloading to avoid the
// cost of passing a 'bslmf_Tag' pointer.
//
// In addition to forming new types, the value of the integral paramameter is
// "saved" in the enum member 'VALUE', and is accessible for use in
// compile-time or run-time operations.
//..
//       template <int V>
//       int g()
//       {
//           bslmf_Tag<V> i;
//           std::cout << i.VALUE << std::endl;
//           std::cout << bslmf_Tag<V>::VALUE << std::endl;
//           return bslmf_Tag<V>::VALUE;
//       }
//
//       void h()
//       {
//           1 == g<1>(); // prints the number '1' twice
//       }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

                           // ====================
                           // struct bslmf_TypeTag
                           // ====================

template<unsigned N>
struct bslmf_Tag {
    // A supporting type that is never intended to produce a run-time
    // instance.  The only useful attribute of a tag is its size (which is, of
    // course, computable at compile time, even if an instance is never
    // created.)

    // In case of overflow on Linux 64 bit machines, we split the size into 2.
    char d_upperSizeArray[(N >> 16) + 1];
    char d_lowerSizeArray[(N & 0x0000FFFF) + 1];

};

#define BSLMF_TAG_TO_UINT(expr) (((sizeof(expr.d_upperSizeArray) - 1) << 16)  \
                                 | (sizeof(expr.d_lowerSizeArray) - 1))
    // Given an integral value, 'V', and an expression, 'expr', of type
    // 'bslmf_Tag<V>', these macros return a compile-time constant with
    // value, 'V'.  The expression, 'expr', is not evaluated at run-time.

#define BSLMF_TAG_TO_INT(expr) ((int) BSLMF_TAG_TO_UINT(expr))
    // Given an integral value, 'V', and an expression, 'expr', of type
    // 'bslmf_Tag<V>', this macro returns a compile-time constant with the
    // value 'V'.  The expression, 'expr', is not evaluated at run-time.

#define BSLMF_TAG_TO_BOOL(expr) (BSLMF_TAG_TO_INT(expr) != 0)
    // Given an integral value, 'V', and an expression, 'expr', of type
    // 'bslmf_Tag<V>', this macro returns a compile-time constant with the
    // 'true' or 'false', depending on the Boolean value of 'V'.  The
    // expression, 'expr', is not evaluated at run-time.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
