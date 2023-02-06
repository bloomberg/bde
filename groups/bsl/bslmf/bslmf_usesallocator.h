// bslmf_usesallocator.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_USESALLOCATOR
#define INCLUDED_BSLMF_USESALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to determine if a type uses an allocator.
//
//@CLASSES:
//  bsl::uses_allocator: meta-fn to check if a type uses a certain allocator
//  bsl::uses_allocator_v: the result value of 'bsl::uses_allocator'
//
//@SEE_ALSO: bslmf_isconvertible
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::uses_allocator',
// that may be used to query whether a given type uses a given allocator type.
//
// 'bsl::uses_allocator' meets the requirements of the 'uses_allocator'
// template defined in the C++11 standard [allocator.uses.trait], in addition
// to providing a welcome availability in both C++03 and C++11 compilation
// environments.
//
// A type 'T' uses an allocator type 'A' if 'A' has a nested alias named
// 'allocator_type' and 'A' is convertible to 'allocator_type' (as defined in
// the 'bslmf_isconvertible' component).  If a type 'T' uses an allocator type
// 'A', then 'T' has a constructor that takes either 1) 'allocator_arg_t' as a
// first argument and 'A' as a second argument, or 2) 'A' as the last argument.
// Alternatively, the 'uses_allocator' template may be specialized for a type
// 'T' that does not have a nested alias named 'allocator_type', where 'T' can
// be constructed with 'A' as detailed above.
//
// Note that the template variable 'uses_allocator_v' is defined in the C++17
// standard as an inline variable.  If the current compiler supports the inline
// variable C++17 compiler feature, 'bsl::uses_allocator_v' is defined as an
// 'inline constexpr bool' variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature, 'bsl::uses_allocator_v' is
// defined as a non-inline 'constexpr bool' variable.  See
// 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
// TBD: finish up usage example, add to test driver.
//
///Example 1: Determine If a Type Uses an Allocator
///- - - - - - - - - - - - - - - - - - - - - - - -
//
//..
// template <class CONTAINER>
// class ContainerAdaptor {
//     // ...
//   public:
//     ContainerAdaptor();
//         // Create an empty container adaptor.  No allocator will be provided
//         // to the underlying container, and the container's memory
//         // allocation will be provided by whatever is the default for the
//         // container type.
//
//     template <class t_ALLOC>
//     explicit
//     ContainerAdaptor(const t_ALLOC& basicAllocator,
//                      typename bsl::enable_if<
//                              bsl::uses_allocator<CONTAINER, t_ALLOC>::value,
//                              t_ALLOC>::type * = 0);
//         // Create an empty container adaptor, and use the specified
//         // 'basicAllocator' to supply memory.  Note that this constructor is
//         // available only when the type of the argument is compatible with
//         // the allocator type associated with the container.
//
//     // ...
// };
//..
//

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isconvertible.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslmf {

template <class t_TYPE, class t_ALLOC, class = void>
struct UsesAllocator_Imp : bsl::false_type {
    // This 'struct' template derives from 'bsl::false_type' when the (template
    // parameter) type 't_TYPE' does not have a nested alias 'allocator_type'.
};

template <class t_TYPE, class t_ALLOC>
struct UsesAllocator_Imp<t_TYPE,
                         t_ALLOC,
                         BSLMF_VOIDTYPE(typename t_TYPE::allocator_type)>
: bsl::is_convertible<t_ALLOC, typename t_TYPE::allocator_type>::type {
    // This 'struct' template derives from 'bsl::true_type' when the (template
    // parameter) 't_TYPE' has a nested alias 'allocator_type' and the
    // (template parameter) type 't_ALLOC' is convertible to
    // 't_TYPE::allocator_type', and 'bsl::false_type' otherwise.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                        // =====================
                        // struct uses_allocator
                        // =====================

template <class t_TYPE, class t_ALLOCATOR_TYPE>
struct uses_allocator
: BloombergLP::bslmf::UsesAllocator_Imp<t_TYPE, t_ALLOCATOR_TYPE>::type {
    // This 'struct' template implements a meta-function to determine whether a
    // (template parameter) 't_TYPE' uses a given (template parameter)
    // 't_ALLOCATOR_TYPE'.  This 'struct' derives from 'bsl::true_type' if
    // 't_TYPE' uses 't_ALLOCATOR_TYPE' and from 'bsl::false_type' otherwise.
    // This meta-function has the same syntax as the 'uses_allocator'
    // meta-function defined in the C++11 standard [allocator.uses.trait].
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE, class t_ALLOCATOR_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool uses_allocator_v =
                               uses_allocator<t_TYPE, t_ALLOCATOR_TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::uses_allocator' meta-function.
#endif


}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
