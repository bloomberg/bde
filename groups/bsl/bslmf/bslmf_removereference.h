// bslmf_removereference.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#define INCLUDED_BSLMF_REMOVEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for stripping reference-ness from types.
//
//@CLASSES:
//  bsl::remove_reference: standard meta-function for stripping reference-ness
//  bslmf::RemoveReference: meta-function for stripping reference-ness
//
//@AUTHOR: Oleg Grunin (ogrunin)
//
//@DESCRIPTION: This component defines two meta-functions,
// 'bsl::remove_reference' and 'BloombergLP::bslmf::RemoveReference', both of
// which may be used to strip reference-ness (including both lvalue and rvalue
// reference-ness, if the latter is supported by compiler) from a type.
//
// 'bsl::remove_reference' meets the requirements of the 'remove_reference'
// template defined in the C++11 standard [meta.trans.ref], while
// 'bslmf::RemoveReference' was devised before 'remove_reference' was
// standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::remove_reference' is indicated by
// the class member 'type', while the result for 'bslmf::RemoveReference' is
// indicated by the class member 'Type'.
//
// Note that 'bsl::remove_reference' should be preferred over
// 'bslmf::RemoveReference', and in general, should be used by new components.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Remove Types' Reference-ness
///- - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove reference-ness on a set of types.
//
// Now, we instantiate the 'bsl::remove_reference' template for each of these
// types, and use the 'bsl::is_same' meta-function to assert the 'type' static
// data member of each instantiation:
//..
//  assert(true  ==
//            (bsl::is_same<bsl::remove_reference<int& >::type, int >::value));
//  assert(false ==
//            (bsl::is_same<bsl::remove_reference<int& >::type, int&>::value));
//  assert(true  ==
//            (bsl::is_same<bsl::remove_reference<int  >::type, int >::value));
//#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//  assert(true ==
//            (bsl::is_same<bsl::remove_reference<int&&>::type, int >::value));
//#endif
//..
// Note that rvalue reference is a feature introduced in the C++11 standard and
// may not be supported by all compilers.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

namespace bsl {

                         // =======================
                         // struct remove_reference
                         // =======================

template <typename TYPE>
struct remove_reference
{
    // This 'struct' template implements a meta-function to remove the
    // reference-ness of the (template parameter) 'TYPE'.  This generic default
    // template defines a return type when 'TYPE' is not a reference type.

    typedef TYPE type;
        // This 'typedef' defines the return type of this meta-function.
};

template <typename TYPE>
struct remove_reference<TYPE &>
{
    // This partial specialization of 'remove_reference' defines a return
    // type when it is instantiated with a reference type.

    typedef TYPE type;
        // This 'typedef' defines the return type of this meta-function.
};

}  // close namespace bsl

#else  // !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

                         // ==========================
                         // struct RemoveRvalueRef_Imp
                         // ==========================

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct RemoveRvalueRef_Imp
{
    // This 'struct' template implements a meta-function to remove the rvalue
    // reference-ness of the (template parameter) 'TYPE'.  This generic default
    // template defines a return type when 'TYPE' is not an rvalue reference
    // type.

    typedef TYPE Type;
        // This 'typedef' defines the return type of this meta-function.
};

template <typename TYPE>
struct RemoveRvalueRef_Imp<TYPE &&>
{
    // This partial specialization of 'RemoveRvalueRef_Imp' defines a return
    // type when it is instantiated with an rvalue reference type.

    typedef TYPE Type;
        // This 'typedef' defines the return type of this meta-function.
};

                         // ===========================
                         // struct Remove_LvalueRef_Imp
                         // ===========================

template <typename TYPE>
struct RemoveLvalueRef_Imp
{
    // This 'struct' template implements a meta-function to remove the lvalue
    // reference-ness of the (template parameter) 'TYPE'.  This generic default
    // template defines a return type when 'TYPE' is not an lvalue reference
    // type.

    typedef TYPE Type;
        // This 'typedef' defines the return type of this meta-function.
};

template <typename TYPE>
struct RemoveLvalueRef_Imp<TYPE &>
{
    // This partial specialization of 'RemoveLvalueRef_Imp' defines a return
    // type when it is instantiated with an lvalue reference type.

    typedef TYPE Type;
        // This 'typedef' defines the return type of this meta function.
};

}
}

namespace bsl {
                         // =======================
                         // struct remove_reference
                         // =======================

template <typename TYPE>
struct remove_reference
{
    // This 'struct' template implements a meta-function to remove the
    // reference-ness of the (template parameter) 'TYPE'.

    typedef typename BloombergLP::bslmf::RemoveLvalueRef_Imp<
                typename BloombergLP::bslmf::RemoveRvalueRef_Imp<TYPE>::Type
            >::Type type;
        // This 'typedef' defines the return type of this meta function.  If
        // the 'TYPE' is an (lvalue or rvalue) reference to another type
        // 'TYPE1', then this 'type' returns 'TYPE1'; otherwise it returns
        // 'TYPE'.
};

}  // close namespace bsl

#endif

namespace BloombergLP {
namespace bslmf {

                         // ======================
                         // struct RemoveReference
                         // ======================

template <typename TYPE>
struct RemoveReference
{
    // This 'struct' template implements a meta-function to remove the
    // reference-ness from the (template parameter) 'TYPE'.  Note that although
    // this 'struct' is functionally equivalent to 'bsl::remove_reference', the
    // use of 'bsl::remove_reference' should be preferred.

    typedef typename bsl::remove_reference<TYPE>::type Type;
        // This 'typedef' defines the return type of this meta function.  If
        // the 'TYPE' is a reference to another type 'TYPE1', then this 'Type'
        // returns 'TYPE1'; otherwise it returns 'TYPE'.
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_RemoveReference
#undef bslmf_RemoveReference
#endif
#define bslmf_RemoveReference bslmf::RemoveReference
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
