// bslmf_isvolatile.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ISVOLATILE
#define INCLUDED_BSLMF_ISVOLATILE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for 'volatile'-qualified types.
//
//@CLASSES:
//  bsl::is_volatile: meta-function for determining 'volatile'-qualified types
//  bsl::is_volatile_v: the result value of 'bsl::is_volatile'
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_volatile' and
// a template variable 'bsl::is_volatile_v', that represents the result value
// of the meta-function, that may be used to query whether a type is
// 'volatile'-qualified as defined in the C++11 standard
// [basic.type.qualifier].
//
// 'bsl::is_volatile' meets the requirements of the 'is_volatile' template
// defined in the C++11 standard [meta.unary.prop].
//
// Note that the template variable 'is_volatile_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::is_volatile_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature, 'bsl::is_volatile_v' is defined
// as a non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify 'volatile' Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is
// 'volatile'-qualified.
//
// First, we create two 'typedef's -- a 'volatile'-qualified type and an
// unqualified type:
//..
//  typedef int           MyType;
//  typedef volatile int  MyVolatileType;
//..
// Now, we instantiate the 'bsl::is_volatile' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_volatile<MyType>::value);
//  assert(true  == bsl::is_volatile<MyVolatileType>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature, then we can re-write the snippet of code above as follows:
//..
//  #ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_volatile_v<MyType>);
//  assert(true  == bsl::is_volatile_v<MyVolatileType>);
//  #endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_issame.h>

#include <bsls_platform.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#include <stddef.h>


#if (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1910)     \
 ||  defined(BSLS_PLATFORM_CMP_IBM)
// The Microsoft compiler does not recognize array-types as cv-qualified (when
// the element type is cv-qualified) when performing matching for partial
// template specialization, but does get the correct result when performing
// overload resolution for functions (taking arrays by reference).  Given the
// function dispatch behavior being correct, we choose to work around this
// compiler bug, rather than try to report compiler behavior, as the compiler
// itself is inconsistent depending on how the trait might be used.  This also
// corresponds to how Microsft itself implements the trait in VC2010 and later.
# define BSLMF_ISVOLATILE_COMPILER_DOES_NOT_DETECT_CV_QUALIFIED_ARRAY_ELEMENT 1
#endif

namespace bsl {

                         // ==================
                         // struct is_volatile
                         // ==================

template <class t_TYPE>
struct is_volatile : false_type {
    // This 'struct' template implements the 'is_volatile' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 't_TYPE' is 'volatile'-qualified.  This 'struct'
    // derives from 'bsl::true_type' if the 't_TYPE' is 'volatile'-qualified,
    // and 'bsl::false_type' otherwise.  Note that this generic default
    // template derives from 'bsl::false_type'.  A template specialization is
    // provided (below) that derives from 'bsl::true_type'.
};

                    // ===================================
                    // struct is_volatile<t_TYPE volatile>
                    // ===================================

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
template <class t_TYPE>
struct is_volatile<volatile t_TYPE>
: integral_constant<bool, !is_same<t_TYPE, volatile t_TYPE>::value> {
    // This partial specialization of 'is_volatile', for when the (template
    // parameter) 't_TYPE' is 'volatile'-qualified, derives from
    // 'bsl::true_type'.  Note that the Solaris CC compiler misdiagnoses
    // cv-qualified "abominable" function types as being cv-qualified
    // themselves.  The correct result is obtained by delegating the result to
    // a call through 'is_same'.
};
#else
template <class t_TYPE>
struct is_volatile<volatile t_TYPE> : true_type {
    // This partial specialization of 'is_volatile', for when the (template
    // parameter) 't_TYPE' is 'volatile'-qualified, derives from
    // 'bsl::true_type'.
};
#endif


#ifdef BSLMF_ISVOLATILE_COMPILER_DOES_NOT_DETECT_CV_QUALIFIED_ARRAY_ELEMENT
// The Microsoft compiler does not recognize array-types as cv-qualified when
// the element type is cv-qualified when performing matching for partial
// template specialization, but does get the correct result when performing
// overload resolution for functions (taking arrays by reference).  Given the
// function dispatch behavior being correct, we choose to work around this
// compiler bug, rather than try to report compiler behavior, as the compiler
// itself is inconsistent depeoning on how the trait might be used.  This also
// corresponds to how Microsft itself implements the trait in VC2010 and later.

template <class t_TYPE>
struct is_volatile<volatile t_TYPE[]> : true_type {
    // This partial specialization of 'is_volatile', for when the (template
    // parameter) 't_TYPE' is 'volatile'-qualified, derives from
    // 'bsl::true_type'.  Note that this single specialization is sufficient to
    // work around the MSVC issue, even for multidimensional arrays.
};

template <class t_TYPE, size_t t_LENGTH>
struct is_volatile<volatile t_TYPE[t_LENGTH]> : true_type {
    // This partial specialization of 'is_volatile', for when the (template
    // parameter) 't_TYPE' is 'volatile'-qualified, derives from
    // 'bsl::true_type'.  Note that this single specialization is sufficient to
    // work around the MSVC issue, even for multidimensional arrays.
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_volatile_v =
                                                    is_volatile<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_volatile' meta-function.
#endif

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
