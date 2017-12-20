// bslmf_isfunction.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ISFUNCTION
#define INCLUDED_BSLMF_ISFUNCTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining function types.
//
//@CLASSES:
//  bsl::is_function: standard meta-function for determining function types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_function',
// that may be used to query whether a template parameter type is a function
// type.
//
// 'bsl::is_function' meets the requirements of the 'is_function' template
// defined in the C++11 standard [meta.unary.cat].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Function Types
/// - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are function types.
//
// Now, we instantiate the 'bsl::is_function' template for both a non-function
// type and a function type, and assert the 'value' static data member of each
// instantiation:
//..
//  assert(false == bsl::is_function<int>::value);
//  assert(true  == bsl::is_function<int (int)>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ADDPOINTER
#include <bslmf_addpointer.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace bsl {

                             // ==================
                             // struct is_function
                             // ==================

template <class TYPE>
struct is_function
    : integral_constant<
            bool,
            BloombergLP::bslmf::FunctionPointerTraits<
                      typename add_pointer<TYPE>::type>::IS_FUNCTION_POINTER> {
    // This 'struct' template implements the 'is_function' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 'TYPE' is a function type.  This 'struct' derives
    // from 'bsl::true_type' if the 'TYPE' is a function type, and
    // from 'bsl::false_type' otherwise.
};

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
