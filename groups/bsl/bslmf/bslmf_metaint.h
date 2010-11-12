// bslmf_metaint.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_METAINT
#define INCLUDED_BSLMF_METAINT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to map integral constants to unique types.
//
//@CLASSES:
//   bslmf_Tag: meta-function mapping integral constants to C++ types
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// map an integral constant to a C++ type.  'bslmf_MetaInt<int>' defines a
// different type for each distinct compile-time constant integral parameter.
// That is, instantiations with different integer values form distinct types,
// so that 'bslmf_MetaInt<0>' is a distinct type from 'bslmf_MetaInt<1>', which
// is also distinct from 'bslmf_MetaInt<2>', and so on.
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
//       void doSomethingImp(T *t, bslmf_MetaInt<0>)
//       {
//           // slow but generic implementation
//       }
//
//       template <class T>
//       void doSomethingImp(T *t, bslmf_MetaInt<1>)
//       {
//           // fast implementation (works only for some T's)
//       }
//
//       template <class T, bool IsFast>
//       void doSomething(T *t)
//       {
//           doSomethingImp(t, bslmf_MetaInt<IsFast>());
//       }
//..
// For some parameter types, the fast version of 'doSomethingImp' is not legal.
// The power of this approach is that the compiler will compile only the
// implementation selected by the 'MetaInt' argument.
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
// cost of constructing a 'bslmf_MetaInt' object.
//
// In addition to forming new types, the value of the integral paramameter is
// "saved" in the enum member 'VALUE', and is accessible for use in
// compile-time or run-time operations.
//..
//       template <int V>
//       int g()
//       {
//           bslmf_MetaInt<V> i;
//           std::cout << i.VALUE << std::endl;
//           std::cout << bslmf_MetaInt<V>::VALUE << std::endl;
//           return bslmf_MetaInt<V>::VALUE;
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

#ifndef INCLUDED_BSLMF_TAG
#include <bslmf_tag.h>
#endif

namespace BloombergLP {

                           // ====================
                           // struct bslmf_MetaInt
                           // ====================

template <unsigned INT_VALUE>
struct bslmf_MetaInt {
    // Instantiating this template produces a distinct type for each
    // non-negative integer value.

    // TYPES
    typedef bslmf_MetaInt<INT_VALUE> Type;

    typedef bslmf_Tag<INT_VALUE>     Tag;

    enum { VALUE = (int)INT_VALUE };

    // CLASS METHODS
    static Tag& tag();
        // Declared but not defined.  Meta-function use only.  The tag can be
        // used to recover meta-information from an expression.
        // Example: 'sizeof(f(expr).tag())' returns a different compile-time
        // value depending on the type of the result of calling the 'f'
        // function but does not actually call the 'f' function or the 'tag'
        // method at run-time.  Note that 'sizeof(f(expr)::Type)' would not be
        // legal.

    // ACCESSORS
    operator int () const { return VALUE; }
        // Conversion to integer.  Returns 'VALUE'.
};

#define BSLMF_METAINT_TO_INT(expr)  BSLMF_TAG_TO_INT((expr).tag())
    // Given an integral value, 'V', and an expression, 'expr', of type
    // 'bslmf_MetaInt<V>', this macro returns a compile-time constant with
    // value, 'V'.  The expression, 'expr', is not evaluated at run-time.

#define BSLMF_METAINT_TO_BOOL(expr) BSLMF_TAG_TO_BOOL((expr).tag())
    // Given an integral value, 'V', and an expression, 'expr', of type
    // 'bslmf_MetaInt<V>', this macro returns a compile-time constant with
    // value, 'true' or 'false', according to the Boolean value of 'V'.  The
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
