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
//  bsl::is_class: standard meta-function for determining class types
//  bslmf::IsClass: meta-function for determining class types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::is_class' and
// 'BloombergLP::bslmf::IsClass', both of which may be used to query whether a
// type is a 'class', 'struct', or 'union', optionally qualified with 'const'
// or volatile'.
//
// 'bsl::is_class' meets the requirements of the 'is_class' template defined in
// the C++11 standard [meta.unary.cat], while 'bslmf::IsClass' was devised
// before 'is_class' was standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_class' is indicated by the
// class member 'value', while the result for 'bslmf::IsClass' is indicated by
// the class member 'VALUE'.
//
// Note that 'bsl::is_class' should be preferred over 'bslmf::IsClass', and in
// general, should be used by new components.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Class Types
///- - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a class type.
//
// First, we create a class type 'MyClass':
//..
//  class MyClass
//  {
//  };
//..
// Now, we instantiate the 'bsl::is_class' template for both a non-class type
// and the defined type 'MyClass', and assert the 'value' static data member of
// each instantiation:
//..
//  assert(false == bsl::is_class<int>::value);
//  assert(true  == bsl::is_class<MyClass>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
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
namespace bslmf {

template <typename TYPE>
struct IsClass_Imp {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is a class type.

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
struct is_class : integral_constant<bool,
                                 BloombergLP::bslmf::IsClass_Imp<
                                     typename remove_cv<
                                         typename remove_reference<TYPE>::type>
                                             ::type>::Value> {
    // This 'struct' template implements the 'is_class' meta-function defined
    // in the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 'TYPE' is a class.
};

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                         // ==============
                         // struct IsClass
                         // ==============

template <typename TYPE>
struct IsClass : bsl::is_class<TYPE>::type {
    // This metafunction derives from 'bsl::true_type' if the specified 'TYPE'
    // is a class type, or is a reference to a class type, and from
    // 'bsl::false_type' otherwise.
    //
    // Note that although this 'struct' is functionally identical to
    // 'bsl::is_class', and the use of 'bsl::is_class' should be preferred.
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsClass
#undef bslmf_IsClass
#endif
#define bslmf_IsClass bslmf::IsClass
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

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
