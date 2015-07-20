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
//@SEE_ALSO: bslmf_addreference
//
//@AUTHOR: Oleg Grunin (ogrunin)
//
//@DESCRIPTION: This component defines two meta-functions,
// 'bsl::remove_reference' and 'BloombergLP::bslmf::RemoveReference', both of
// which may be used to strip reference-ness (including both lvalue and rvalue
// reference-ness, if the latter is supported by the compiler) from a type.
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
///Example 1: Remove Reference-ness of Types
///- - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove the reference-ness of a set of types.
//
// Now, remove the reference-ness of a set of types using
// 'bsl::remove_reference' and verify that the returned type has any
// reference-ness removed:
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

                         // =======================
                         // struct remove_reference
                         // =======================

namespace bsl {

template <class TYPE>
struct remove_reference {
    // This 'struct' template implements the 'remove_reference' meta-function
    // defined in the C++11 standard [meta.trans.ref], providing an alias,
    // 'type', that returns the result.  'type' has the same type as the
    // (template parameter) 'TYPE' except with reference-ness removed.  Note
    // that this generic default template provides a 'type' that is an alias to
    // 'TYPE' for when 'TYPE' is not a reference.  A template specialization is
    // provided (below) that removes reference-ness for when 'TYPE' is a
    // reference.

    typedef TYPE type;
        // This 'typedef' is an alias to the (template parameter) 'TYPE'.
};

template <class TYPE>
struct remove_reference<TYPE&> {
    // This partial specialization of 'bsl::remove_reference', for when the
    // (template parameter) 'TYPE' is an rvalue reference, provides a
    // 'typedef', 'type', that has reference-ness of 'TYPE' removed.

    typedef TYPE type;
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE' except with the reference-ness removed.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <class TYPE>
struct remove_reference<TYPE&&> {
    // This partial specialization of 'bsl::remove_reference', for when the
    // (template parameter) 'TYPE' is a reference, provides a 'typedef',
    // 'type', that has reference-ness of 'TYPE' removed.

    typedef TYPE type;
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE' except with the reference-ness removed.
};

#endif

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                         // ======================
                         // struct RemoveReference
                         // ======================

template <class TYPE>
struct RemoveReference {
    // This 'struct' template implements a meta-function to remove the
    // reference-ness from the (template parameter) 'TYPE'.  Note that although
    // this 'struct' is functionally equivalent to 'bsl::remove_reference', the
    // use of 'bsl::remove_reference' should be preferred.

    typedef typename bsl::remove_reference<TYPE>::type Type;
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE' except with any reference-ness removed.
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_RemoveReference
#undef bslmf_RemoveReference
#endif
#define bslmf_RemoveReference bslmf::RemoveReference
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
