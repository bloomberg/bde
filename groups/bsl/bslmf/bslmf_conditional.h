// bslmf_conditional.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_CONDITIONAL
#define INCLUDED_BSLMF_CONDITIONAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time conditional type selector.
//
//@CLASSES:
//  bsl::conditional: standard meta-function for conditional type selection
//
//@SEE_ALSO: bslmf_enableif
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::conditional',
// that may be used to conditionally select one of its two (template parameter)
// types based on a 'bool' (template parameter) value.
//
// 'bsl::conditional' meets the requirements of the 'conditional' template
// defined in the C++11 standard [meta.trans.other], providing a 'typedef'
// 'type' that is an alias to the first (template parameter) type if the
// (template parameter) value is 'true'; otherwise, 'type' is an alias to the
// second (template parameter) type.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Conditionally Select From Two Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to select between two types based on a 'bool' value.
//
// Now, we use 'bsl::conditional' to select between two types, 'int' and
// 'char', with a 'bool' value.  When the 'bool' is 'true', we select 'int';
// otherwise, we select 'char'.  We verify that our code behaves correctly by
// asserting the result of 'bsl::conditional' with the expected type using
// 'bsl::is_same':
//..
//  assert(true ==
//      (bsl::is_same<bsl::conditional<true,  int, char>::type, int >::value));
//  assert(true ==
//      (bsl::is_same<bsl::conditional<false, int, char>::type, char>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace bsl {
                           // ==================
                           // struct conditional
                           // ==================

template <bool COND, class TRUE_TYPE, class FALSE_TYPE>
struct conditional {
    // This 'struct' template implements the 'conditional' meta-function
    // defined in the C++ standard [meta.trans.other], providing an alias,
    // 'type', that returns the result.  If the (template parameter) value
    // 'COND' is 'true', then 'type' has the same type as the (template
    // parameter) type 'TRUE_TYPE'; otherwise, 'type' has the same type as the
    // (template parameter) type 'FALSE_TYPE'.  Note that this generic default
    // template defines 'type' to be an alias to 'TRUE_TYPE' for when 'COND' is
    // 'true'.  A template specialization is provided (below) handles the case
    // for when 'COND' is 'false'.

    typedef TRUE_TYPE type;
        // This 'typedef' is an alias to the (template parameter) type
        // 'TRUE_TYPE'.
};

template <class TRUE_TYPE, class FALSE_TYPE>
struct conditional<false, TRUE_TYPE, FALSE_TYPE> {
    // This partial specialization of 'bsl::conditional', for when the
    // (template parameter) value 'COND' is 'false', provides a 'typedef'
    // 'type' that is an alias to the (template parameter) type 'FALSE_TYPE'.

    typedef FALSE_TYPE type;
        // This 'typedef' is an alias to the (template parameter) type
        // 'FALSE_TYPE'.
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
