// bslmf_issame.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_ISSAME
#define INCLUDED_BSLMF_ISSAME

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for testing if two types are the same.
//
//@CLASSES:
//  bsl::is_same: standard meta-function for testing if two types are the same
//  bsl::is_same_v: the result value of the standard meta-function
//  bslmf::IsSame: meta-function for testing if two types are the same
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::is_same' and
// 'BloombergLP::bslmf::IsSame' and a template variable 'bsl::is_same_v',
// that represents the result value of the 'bsl::is_same' meta-function.  All
// these meta-functions may be used to query whether two types are the same.
// Two types are the same if they name the same type having the same
// cv-qualifications.
//
// 'bsl::is_same' meets the requirements of the 'is_same' template defined in
// the C++11 standard [meta.rel], while 'bslmf::IsSame' was devised before
// 'is_same' was standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_same' is indicated by the class
// member 'value', while the result for 'bslmf::IsSame' is indicated by the
// class member 'VALUE'.
//
// Note that 'bsl::is_same' should be preferred over 'bslmf::IsSame', and in
// general, should be used by new components.
//
// Also note that the template variable 'is_same_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::is_same_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature, 'bsl::is_same_v' is defined as a
// non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Determine Same Types
///- - - - - - - - - - - - - - - -
// Suppose that we have several pairs of types and want to assert whether
// the types in each pair are the same.
//
// First, we define several 'typedef's:
//..
//  typedef       int    INT1;
//  typedef       double DOUBLE;
//  typedef       short  SHORT;
//  typedef const short  CONST_SHORT;
//  typedef       int    INT2;
//  typedef       int&   INT_REF;
//..
// Now, we instantiate the 'bsl::is_same' template for certain pairs of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(true  == (bsl::is_same<INT1, INT2>::value));
//  assert(false == (bsl::is_same<INT, DOUBLE>::value));
//..
// Note that a 'const'-qualified type is considered distinct from the
// non-'const' (but otherwise identical) type:
//..
//  assert(false == (bsl::is_same<SHORT, CONST_SHORT>::value));
//..
// Similarly, a 't_TYPE' and a reference to 't_TYPE' ('t_TYPE&') are distinct:
//..
//  assert(false == (bsl::is_same<INT, INT_REF>::value));
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_same_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == (bsl::is_same_v<SHORT, CONST_SHORT>));
//  assert(false == (bsl::is_same_v<INT, INT_REF>));
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

namespace bsl {

                            // ==============
                            // struct is_same
                            // ==============

template <class t_TYPE1, class t_TYPE2>
struct is_same : false_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 't_TYPE1' and the (template parameter) 't_TYPE2'
    // are the same.  This generic default template derives from
    // 'bsl::false_type'.  A template specialization is provided (below) that
    // derives from 'bsl::true_type'.
};

template <class t_TYPE>
struct is_same<t_TYPE, t_TYPE> : true_type {
    // This partial specialization of 'is_same' derives from 'bsl::true_type'
    // for when the (template parameter) types are the same.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE1, class t_TYPE2>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_same_v =
                                              is_same<t_TYPE1, t_TYPE2>::value;
    // This template variable represents the result value of the 'bsl::is_same'
    // meta-function.
#endif

}  // close namespace bsl

namespace BloombergLP {

namespace bslmf {

                            // =============
                            // struct IsSame
                            // =============

template <class t_TYPE1, class t_TYPE2>
struct IsSame : bsl::is_same<t_TYPE1, t_TYPE2>::type {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 't_TYPE1' and the (template parameter) 't_TYPE2'
    // are the same.  This 'struct' derives from 'bsl::true_type' if 't_TYPE1'
    // and 't_TYPE2' are the same, and 'bsl::false_type' otherwise.
    //
    // Note that although this 'struct' is functionally equivalent to
    // 'bsl::is_same', the use of 'bsl::is_same' should be preferred.
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_IsSame
#undef bslmf_IsSame
#endif
#define bslmf_IsSame bslmf::IsSame
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
