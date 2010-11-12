// bslmf_isclass.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ISCLASS
#define INCLUDED_BSLMF_ISCLASS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for class types.
//
//@CLASSES:
//    bslmf_IsClass: meta-function for determining class types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a 'class', 'struct', or 'union',
// optionally qualified with 'const' or 'volatile'.  'bslmf_IsClass' defines a
// 'VALUE' member that is initialized (at compile-time) to 1 if the parameter
// is of 'class', 'struct', or 'union' type (or a reference to such a type),
// and to 0 otherwise.  Note that 'bslmf_IsClass' will evaluate to true (i.e.,
// 1) when applied to an incomplete 'class', 'struct', or 'union' type.
//
///Usage
///-----
// For example:
//..
//   struct MyStruct {};
//   enum   MyEnum {};
//   class  MyClass {};
//   class  MyDerivedClass : public MyClass {};
//
//   assert(1 == bslmf_IsClass<MyStruct >::VALUE);
//   assert(1 == bslmf_IsClass<MyStruct&>::VALUE);
//   assert(0 == bslmf_IsClass<MyStruct*>::VALUE);
//
//   assert(1 == bslmf_IsClass<const MyClass          >::VALUE);
//   assert(1 == bslmf_IsClass<const MyDerivedClass&  >::VALUE);
//   assert(0 == bslmf_IsClass<const MyDerivedClass*  >::VALUE);
//   assert(0 == bslmf_IsClass<      MyDerivedClass[1]>::VALUE);
//
//   assert(0 == bslmf_IsClass<int   >::VALUE);
//   assert(0 == bslmf_IsClass<int * >::VALUE);
//   assert(0 == bslmf_IsClass<MyEnum>::VALUE);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_CSTDLIB
#include <cstdlib>  // TBD Robo transitively needs this for 'bsl::atoi', etc.
#define INCLUDED_CSTDLIB
#endif

#endif

namespace BloombergLP {

                         // ====================
                         // struct bslmf_IsClass
                         // ====================

typedef char ISCLASS_TYPE;

struct ISNOTCLASS_TYPE {
    char padding[8];
};

template <class TYPE>
ISCLASS_TYPE bslmf_IsClass_Tester(int TYPE::*);

template <class TYPE>
ISNOTCLASS_TYPE bslmf_IsClass_Tester(...);

template <typename TYPE>
struct bslmf_IsClass_Imp
{
    enum { VALUE =
               sizeof(bslmf_IsClass_Tester<TYPE>(0)) == sizeof(ISCLASS_TYPE) };
};

template <typename TYPE>
struct bslmf_IsClass {
    typedef typename bslmf_RemoveReference<TYPE>::Type NONREF_TYPE;
    typedef typename bslmf_RemoveCvq<NONREF_TYPE>::Type NONCV_TYPE;

    enum { VALUE = bslmf_IsClass_Imp<NONCV_TYPE>::VALUE };
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
