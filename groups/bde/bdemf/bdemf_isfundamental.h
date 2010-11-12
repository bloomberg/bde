// bdemf_isfundamental.h                -*-C++-*-
#ifndef INCLUDED_BDEMF_ISFUNDAMENTAL
#define INCLUDED_BDEMF_ISFUNDAMENTAL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for fundamental types.
//
//@DEPRECATED: Use 'bslmf_isfundamental' instead.
//
//@CLASSES:
//  bdemf_IsFundamental: meta-function for fundamental type checking
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@SEE_ALSO: bdemf_isenum, bdemf_ispointer
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a fundamental type.
// 'bdemf_IsFundamental' defines a member, 'VALUE', whose value is initialized
// (at compile-time) to 1 if the parameter is a fundamental type (ignoring
// any 'const' or 'volatile' qualification), and 0 if it is not.
// Instantiating 'bdemf_IsFundamental' on a reference type is the same as
// instantiating it under underlying (non-reference) type.  Instantiating
// 'bdemf_IsFundamental' on a pointer type yields a 'VALUE' of zero.  (Pointers
// are not fundamental types, but see 'bdemf_ispointer'.)
// 
// The C++ fundamental types are described in the C++ standard, section 3.9.1
// ("Fundamental types"), and consist of the following distinct types:
//..
//   bool
//   char
//   signed char
//   unsigned char
//   wchar_t
//   short int                                    (also referred to as "short")
//   unsigned short int                  (also referred to as "unsigned short")
//   int
//   unsigned int
//   long int                                      (also referred to as "long")
//   unsigned long int                    (also referred to as "unsigned long")
//   float
//   double
//   long double
//   void
//..
// To these types, this component adds the following non-standard types and
// identifies them as fundamental types:
//..
//   long long int                            (also referred to as "long long")
//   unsigned long long int          (also referred to as "unsigned long long")
//..
//
///Usage
///-----
// The following example shows the result of instantiating
// 'bdemf_IsFundamental' on a number of different types.
//..
//   struct MyType {};
//
//   static const int a1 = bdemf_IsFundamental<int>::VALUE;          // a1 == 1
//   static const int a1 = bdemf_IsFundamental<int&>::VALUE;         // a1 == 1
//   static const int a2 = bdemf_IsFundamental<const int>::VALUE;    // a2 == 1
//   static const int a3 = bdemf_IsFundamental<volatile int>::VALUE; // a3 == 1
//   static const int a4 = bdemf_IsFundamental<int *>::VALUE;        // a4 == 0
//   static const int a5 = bdemf_IsFundamental<MyType>::VALUE;       // a5 == 0
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef bdemf_IsFundamental
#define bdemf_IsFundamental   bslmf_IsFundamental
    // This class implements a meta-function for checking if a type is
    // fundamental.  The static constant 'VALUE' member will be 1 if 'TYPE' is
    // fundamental and 0 otherwize.
#endif

namespace BloombergLP {

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
