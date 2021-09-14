// bslma_isstdallocator.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMA_ISSTDALLOCATOR
#define INCLUDED_BSLMA_ISSTDALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining allocator types.
//
//@CLASSES:
//  bsl::IsStdAllocator: standard meta-function for determining allocator types
//  bsl::IsStdAllocator_v: the result of 'bsl::IsStdAllocator' meta-function
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::IsStdAllocator'
// and a template variable 'bsl::IsStdAllocator_v', that represents the result
// value of the 'bsl::IsStdAllocator' meta-function.
//
// 'bsl::IsStdAllocator' is used to determine if a type meets the requirements
// for an allocator, as specified in [container.requirements.general].  Note
// that there is no 'is_allocator' trait specified in the C++ standard, even
// though every implementation has one.
//
// Two implementations are supplied; one for C++11 (and later) conforming
// compilers, and a pre-C++11 compatibility trait that gives different answers
// for custom allocator types due to the lack of 'decltype' in the older
// language.
//
// Also note that the template variable 'IsStdAllocator_v' is defined in the
// style of the C++17 standard as an inline variable.  If the current compiler
// supports the inline variable C++17 compiler feature, 'bsl::IsStdAllocator_v'
// is defined as an 'inline constexpr bool' variable.  Otherwise, if the
// compiler supports the variable templates C++14 compiler feature,
// 'bsl::IsStdAllocator_v' is defined as a non-inline 'constexpr bool'
// variable.  See 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify if a class meets the requirements for an allocator.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types meet the requirements
// for allocators.
//
// First, we create a struct type 'MyAllocator':
//..
//  struct MyAllocator
//  {
//      typedef int value_type;
//      int *allocate(size_t count);
//          // Allocate some memory for use by the caller.
//  };
//..
// Now, we instantiate the 'bsl::IsStdAllocator' template for both a type that
// does not meet the allocator requirements and the defined type 'MyClass',
// that does, asserting the 'value' static data member of each instantiation.
//..
//  assert(false == bsl::IsStdAllocator<int>::value);
//  #ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
//  assert(true  == bsl::IsStdAllocator<MyAllocator>::value);
//  #else
//  assert(false  == bsl::IsStdAllocator<MyAllocator>::value);
//  #endif
//..
// Note that if the current compiler supports the variable the templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::IsStdAllocator_v' variable as follows:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//  assert(false == bsl::IsStdAllocator_v<int>);
//  assert(true  == bsl::IsStdAllocator_v<MyAllocator>);
//#endif
//..

#include <bslscm_version.h>

#include <bslma_stdallocator.h>

#include <bslmf_integralconstant.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_nativestd.h>

#include <memory>      // allocator
#include <stddef.h>    // size_t

namespace bsl {
                           // ===================
                           // struct is_allocator
                           // ===================

template<class ALLOC, class = void, class = void>
struct IsStdAllocator : false_type {};
    // In general, things are not allocators

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
template<class ALLOC>
struct IsStdAllocator<ALLOC,
     void_t<typename ALLOC::value_type>,
     void_t<decltype(native_std::declval<ALLOC&>().allocate(size_t(0)))> >
     : public true_type {};
         // if it has a 'value_type` and method named 'allocate' that takes
         // something convertible from a size_t, then it's an allocator.
#else
template <class TYPE>
struct IsStdAllocator<native_std::allocator<TYPE> > : public true_type {};
    // std::allocator<T> is an allocator

template <class TYPE>
struct IsStdAllocator< ::bsl::allocator<TYPE> > : public true_type {};
    // bsl::Allocator is an allocator
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool IsStdAllocator_v = IsStdAllocator<TYPE>::value;
    // This template variable represents the result value of the
    // 'bsl::IsStdAllocator' meta-function.
#endif

}  // close namespace bsl

#endif // INCLUDED_BSLMA_ISSTDALLOCATOR

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
