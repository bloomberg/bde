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
//  bslmf::IsClass: meta-function for determining class types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a 'class', 'struct', or 'union',
// optionally qualified with 'const' or 'volatile'.  'bslmf::IsClass' defines a
// 'VALUE' member that is initialized (at compile-time) to 1 if the parameter
// is of 'class', 'struct', or 'union' type (or a reference to such a type),
// and to 0 otherwise.  Note that 'bslmf::IsClass' will evaluate to true (i.e.,
// 1) when applied to an incomplete 'class', 'struct', or 'union' type.
//
///Usage
///-----
// For example:
//..
//  struct MyStruct {};
//  enum   MyEnum {};
//  class  MyClass {};
//  class  MyDerivedClass : public MyClass {};
//
//  assert(1 == bslmf::IsClass<MyStruct >::VALUE);
//  assert(1 == bslmf::IsClass<MyStruct&>::VALUE);
//  assert(0 == bslmf::IsClass<MyStruct*>::VALUE);
//
//  assert(1 == bslmf::IsClass<const MyClass          >::VALUE);
//  assert(1 == bslmf::IsClass<const MyDerivedClass&  >::VALUE);
//  assert(0 == bslmf::IsClass<const MyDerivedClass*  >::VALUE);
//  assert(0 == bslmf::IsClass<      MyDerivedClass[1]>::VALUE);
//
//  assert(0 == bslmf::IsClass<int   >::VALUE);
//  assert(0 == bslmf::IsClass<int * >::VALUE);
//  assert(0 == bslmf::IsClass<MyEnum>::VALUE);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef INCLUDED_CSTDLIB
#include <cstdlib>  // TBD Robo transitively needs this for 'bsl::atoi', etc.
#define INCLUDED_CSTDLIB
#endif

#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsClass_Imp
{
    typedef struct { char a; }    YesType;
    typedef struct { char a[2]; } NoType;

    template <typename TEST_TYPE>
    static
    YesType test(int TEST_TYPE::*);

    template <typename TEST_TYPE>
    static
    NoType test(...);

    enum { Value = (sizeof(test<TYPE>(0)) == sizeof(YesType)) };
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct is_class : integer_constant<bool,
                                   BloombergLP::bslmf::IsClass_Imp<
                                        typename remove_cv<
                                            typename remove_reference<TYPE>::type>
                                                ::type>::Value>
{};

}

namespace BloombergLP {
namespace bslmf {

                         // ==============
                         // struct IsClass
                         // ==============

template <typename TYPE>
struct IsClass : MetaInt<bsl::is_class<TYPE>::value>
    // This metafunction derives from 'MetaInt<1>' if the specified 'TYPE' is a
    // class type, or is a reference to a class type, and from 'MetaInt<0>'
    // otherwise.
{};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsClass
#undef bslmf_IsClass
#endif
#define bslmf_IsClass bslmf::IsClass
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
