// bdemf_metavalue.h                  -*-C++-*-
#ifndef INCLUDED_BDEMF_METAVALUE
#define INCLUDED_BDEMF_METAVALUE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an integral-constant-to-type conversion.
//
//@DEPRECATED: Use 'bslmf_metaint', 'bslmf_metatag' instead.
//
//@CLASSES:
// bdemf_MetaValue: map integral constants to C++ types
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// map an integral constant to a C++ type.  'bdemf_MetaInt<int>' defines a
// different type for each distinct compile-time constant integral parameter.
// That is, instantiations with different integer values form distinct types,
// so that bdemf_MetaInt<0> is a distinct type from bdemf_MetaInt<1>, which
// is also distinct from bdemf_MetaInt<2>, and so on.
//
// The most common use of this structure is to perform static function
// dispatching based on a compile-time calculation.  Often the calculation is
// nothing more than a simple predicate, allowing us to select one of two
// functions.  The following function, 'doSomething', uses a fast implemenation
// (e.g., memcpy) if the parameterized type allows for such operations,
// otherwise it will use a more generic and slower implementation (e.g., copy
// constructor).
//..
//       template <class T>
//       void doSomethingImp(T *t, bdemf_MetaInt<0>)
//       {
//           // slow but generic implementation
//       }
//
//       template <class T>
//       void doSomethingImp(T *t, bdemf_MetaInt<1>)
//       {
//           // fast implementation (works only for some T's)
//       }
//
//       template <class T, bool IsFast>
//       void doSomething(T *t)
//       {
//           doSomethingImp(t, bdemf_MetaInt<IsFast>());
//       }
//..
// For some parameter types, the fast version of 'doSomethingImp' is not legal.
// The power of this approach is that the compiler will compile only the
// implementation selected by the MetaValue argument.
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
// cost of constructing a 'bdemf_MetaValue' object.
//
// In addition to forming new types, the value of the integral paramameter is
// "saved" in the enum member 'VALUE', and is accessible for use in
// compile-time or run-time operations.
//..
//       template <int V>
//       int g()
//       {
//           bdemf_MetaInt<V> i;
//           std::cout << i.VALUE << std::endl;
//           std::cout << bdemf_MetaInt<V>::VALUE << std::endl;
//           return bdemf_MetaInt<V>::VALUE;
//       }
//
//       void h()
//       {
//           1 == g<1>(); // prints the number '1' twice
//       }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_TAG
#include <bslmf_tag.h>
#endif

#ifndef bdemf_Tag
#define bdemf_Tag         bslmf_Tag
#endif

#ifndef bdemf_MetaInt
#define bdemf_MetaInt     bslmf_MetaInt
#endif

#ifndef BDEMF_TAG_TO_INT
#define BDEMF_TAG_TO_INT(expr)  BSLMF_TAG_TO_INT(expr)
#endif

#ifndef BDEMF_TAG_TO_BOOL
#define BDEMF_TAG_TO_BOOL(expr) BSLMF_TAG_TO_BOOL(expr)
#endif

#ifndef BDEMF_METAVALUE_TO_INT
#define BDEMF_METAVALUE_TO_INT(expr)  BSLMF_METAINT_TO_INT(expr)
#endif

#ifndef BDEMF_METAVALUE_TO_BOOL
#define BDEMF_METAVALUE_TO_BOOL(expr) BSLMF_METAINT_TO_BOOL(expr)
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

#endif // ! defined(INCLUDED_BDEMF_METAVALUE)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
