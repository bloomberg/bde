// bslmf_ispointer.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPOINTER
#define INCLUDED_BSLMF_ISPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for pointer types.
//
//@CLASSES:
//  bslmf::IsPointer: meta-function for determining pointer types
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a pointer.  'bslmf::IsPointer' defines a
// member, 'VALUE', whose value is initialized (at compile-time) to 1 if the
// parameter is a pointer (to anything) (ignoring any 'const' or 'volatile'
// qualification), and 0 if it is not.  For example:
//..
//  struct MyType {};
//  typedef MyType* PMT;
//
//  static const int a1 = bslmf::IsPointer<int *        >::VALUE; // a1 == 1
//  static const int a2 = bslmf::IsPointer<int *const   >::VALUE; // a2 == 1
//  static const int a3 = bslmf::IsPointer<int *volatile>::VALUE; // a3 == 1
//  static const int a4 = bslmf::IsPointer<int          >::VALUE; // a4 == 0
//  static const int a5 = bslmf::IsPointer<MyType       >::VALUE; // a5 == 0
//  static const int a6 = bslmf::IsPointer<MyType*      >::VALUE; // a6 == 1
//  static const int a7 = bslmf::IsPointer<PMT          >::VALUE; // a7 == 1
//..
// This type also provides the public type 'ELEMENT_TYPE' to yield the type
// to which a pointer points.  If the type parameter to the class is not
// a pointer type, 'ELEMENT_TYPE' will yield a struct that will not coincide
// with anything defined by the user.
//..
//  typedef MyType **Woof;
//  typedef bslmf::IsPointer<Woof>::ELEMENT_TYPE WoofElem;
//  static const int a11 = bslmf::IsPointer<Woof>::VALUE &&
//                                bslmf::IsPointer<WoofElem>::VALUE; // a11 = 1
//  static const int a12 = bslmf::IsPointer<Woof>::VALUE &&
//                            bslmf::IsFundamental<WoofElem>::VALUE; // a12 = 0
//  typedef int *Meow;
//  typedef bslmf::IsPointer<Meow>::ELEMENT_TYPE MeowElem;
//  static const int a13 = bslmf::IsPointer<Meow>::VALUE &&
//                                bslmf::IsPointer<MeowElem>::VALUE; // a13 = 0
//  static const int a14 = bslmf::IsPointer<Woof>::VALUE &&
//                            bslmf::IsFundamental<MeowElem>::VALUE; // a14 = 1
//
//  typedef bslmf::IsPointer<MyType>::ELEMENT_TYPE MyTypeElem;
//  static const int a15 = bslmf::IsPointer<MyType>::VALUE &&
//                              bslmf::IsPointer<MyTypeElem>::VALUE; // a15 = 0
//  static const int a16 = bslmf::IsPointer<MyType>::VALUE &&
//                          bslmf::IsFundamental<MyTypeElem>::VALUE; // a16 = 0
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

namespace BloombergLP {

namespace bslmf {

                         // ================
                         // struct IsPointer
                         // ================

template <typename TYPE>
struct IsPointer : MetaInt<0>
{
    struct NotAPointer {};
    typedef NotAPointer ELEMENT_TYPE;

    // This class implements a meta-function for checking if a type is a
    // pointer.
};

// Specializations for pointer types
template <typename TYPE>
struct IsPointer<TYPE *> : MetaInt<1> {
    typedef TYPE ELEMENT_TYPE;
};

template <typename TYPE>
struct IsPointer<TYPE * const> : MetaInt<1> {
    typedef TYPE ELEMENT_TYPE;
};

template <typename TYPE>
struct IsPointer<TYPE * volatile> : MetaInt<1> {
    typedef TYPE ELEMENT_TYPE;
};

template <typename TYPE>
struct IsPointer<TYPE * const volatile> : MetaInt<1> {
    typedef TYPE ELEMENT_TYPE;
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsPointer
#undef bslmf_IsPointer
#endif
#define bslmf_IsPointer bslmf::IsPointer
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
