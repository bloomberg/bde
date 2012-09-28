// bslmf_removereference.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#define INCLUDED_BSLMF_REMOVEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing reference-ness from types.
//
//@CLASSES:
//  bsl::remove_reference: standard meta-function for stripping reference-ness
//  bslmf::RemoveReference: meta-function for stripping reference-ness
//
//@AUTHOR: Oleg Grunin (ogrunin)
//
//@DESCRIPTION: This component defines two meta-functions,
// 'bsl::remove_reference' and 'BloombergLP::bslmf::RemoveReference', both of
// which may be used to strip reference-ness from a type.
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
///Example 1: Verify Pointer Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to remove reference-ness on a couple of types.
//
// Now, we instantiate the 'bsl::remove_reference' template for each of these
// types, and use 'bsl::is_same' meta-function to assert the 'type' static data
// member of each instantiation:
//..
//  assert(true  ==
//             (bsl::is_same<bsl::remove_reference<int&>::type, int >::value));
//  assert(false ==
//             (bsl::is_same<bsl::remove_reference<int&>::type, int&>::value));
//  assert(true  ==
//             (bsl::is_same<bsl::remove_reference<int >::type, int >::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

                         // ======================
                         // struct RemoveReference
                         // ======================

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

namespace bsl {

template <typename TYPE>
struct remove_reference
{
    typedef TYPE type;
};

template <typename TYPE>
struct remove_reference<TYPE &>
{
    typedef TYPE type;
};

}  // close namespace bsl

#else  // !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct RemoveRvalueRef_Imp
{
    typedef TYPE Type;
};

template <typename TYPE>
struct RemoveRvalueRef_Imp<TYPE &&>
{
    typedef TYPE Type;
};

template <typename TYPE>
struct RemoveLvalueRef_Imp
{
    typedef TYPE Type;
};

template <typename TYPE>
struct RemoveLvalueRef_Imp<TYPE &>
{
    typedef TYPE Type;
};

}
}

namespace bsl {

template <typename TYPE>
struct remove_reference
{
    typedef typename BloombergLP::bslmf::RemoveLvalueRef_Imp<
                typename BloombergLP::bslmf::RemoveRvalueRef_Imp<TYPE>::Type
            >::Type type;
};

}  // close namespace bsl

#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct RemoveReference
{
    typedef typename bsl::remove_reference<TYPE>::type Type;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
