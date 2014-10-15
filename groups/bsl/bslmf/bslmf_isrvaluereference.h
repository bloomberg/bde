// bslmf_isrvaluereference.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISRVALUEREFERENCE
#define INCLUDED_BSLMF_ISRVALUEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for rvalue reference types.
//
//@CLASSES:
//  bsl::is_rvalue_reference: standard meta-function for rvalue reference types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_rvalue_reference', that may be used to query whether a type is an
// rvalue reference type.
//
// 'bsl::is_rvalue_reference' meets the requirements of the
// 'is_rvalue_reference' template defined in the C++11 standard
// [meta.unary.cat].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Rvalue Reference Types
/// - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are rvalue reference
// types.
//
// Now, we instantiate the 'bsl::is_rvalue_reference' template for both a
// non-reference type and an rvalue reference type, and assert the 'value'
// static data member of each instantiation:
//..
//  assert(false == bsl::is_rvalue_reference<int>::value);
//#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//  assert(true  == bsl::is_rvalue_reference<int&&>::value);
//#endif
//..
// Note that rvalue reference is a feature introduced in the C++11 standard,
// and may not be supported by all compilers.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

namespace bsl {

                          // ==========================
                          // struct is_rvalue_reference
                          // ==========================

template <class TYPE>
struct is_rvalue_reference : false_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is a (possibly cv-qualified) rvalue
    // reference type.  This generic default template derives from
    // 'bsl::false_type'.  A template specialization is provided (below) that
    // derives from 'bsl::true_type'.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <class TYPE>
struct is_rvalue_reference<TYPE&&> : true_type {
    // This partial specialization of 'is_rvalue_reference' derives from
    // 'bsl::true_type' for when the (template parameter) 'TYPE' is an rvalue
    // reference type.
};

#endif

}  // close namespace bsl

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
