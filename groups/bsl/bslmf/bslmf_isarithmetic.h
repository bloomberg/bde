// bslmf_isarithmetic.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMF_ISARITHMETIC
#define INCLUDED_BSLMF_ISARITHMETIC

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining arithmetic types.
//
//@CLASSES:
//  bsl::is_arithmetic: standard meta-function for determining arithmetic types
//  bsl::is_arithmetic_v: the result value of 'bsl::is_arithmetic'
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_arithmetic'
// and a template variable 'bsl::is_arithmetic_v' that represents the result
// value of the 'bsl::is_arithmetic' meta-function, that may be used to query
// whether a template parameter type is an arithmetic type.
//
// 'bsl::is_arithmetic' meets the requirements of the 'is_arithmetic' template
// defined in the C++11 standard [meta.unary.comp].
//
// Note that the template variable 'is_arithmetic_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::is_arithmetic_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports
// the variable templates C++14 compiler feature, 'bsl::is_arithmetic_v' is
// defined as a non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Arithmetic Types
/// - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are arithmetic types.
//
// Now, we instantiate the 'bsl::is_arithmetic' template for these types, and
// assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_arithmetic<int& >::value);
//  assert(false == bsl::is_arithmetic<int *>::value);
//  assert(true  == bsl::is_arithmetic<int  >::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_arithmetic_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::is_arithmetic_v<int& >);
//  assert(false == bsl::is_arithmetic_v<int *>);
//  assert(true  == bsl::is_arithmetic_v<int  >);
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isfloatingpoint.h>
#include <bslmf_isintegral.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

namespace bsl {

                         // ====================
                         // struct is_arithmetic
                         // ====================

template <class t_TYPE>
struct is_arithmetic
: integral_constant<bool,
                    is_integral<t_TYPE>::value ||
                        is_floating_point<t_TYPE>::value> {
    // This 'struct' template implements the 'is_arithmetic' meta-function
    // defined in the C++11 standard [meta.unary.comp] to determine if the
    // (template parameter) 't_TYPE' is an arithmetic type.  This 'struct'
    // derives from 'bsl::true_type' if the 't_TYPE' is an arithmetic type, and
    // from 'bsl::false_type' otherwise.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool is_arithmetic_v =
                                                  is_arithmetic<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::is_arithmetic' meta-function.
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
