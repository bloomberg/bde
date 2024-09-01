// bslma_isstdallocator.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMA_ISSTDALLOCATOR
#define INCLUDED_BSLMA_ISSTDALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining allocator types.
//
//@CLASSES:
//  bslma::IsStdAllocator: meta-function to determine if a type is an allocator
//  bslma::IsStdAllocator_v: Boolean result of `bslma::IsStdAllocator`
//
//@DESCRIPTION: This component defines a meta-function, `bslma::IsStdAllocator`
// and a variable template, `bslma::IsStdAllocator_v`, that represents the
// result value of the `bslma::IsStdAllocator` meta-function.
//
// `bslma::IsStdAllocator` is used to determine if a type meets the
// requirements for an allocator, as specified in
// [container.requirements.general].  Note that there is no `is_allocator`
// trait specified in the C++ standard, even though every C++ implementation
// has a private equivalent.
//
// In C++03, it is impossible to automatically detect conformance to the
// allocator requirements, owing to limitations in SFINAE capabilities; even
// elaborate detection metaprogramming will break if a type has a private
// `allocate` method.  Therefore, a portable allocator type, *some-alloc*, must
// have `bslma::IsStdAllocator<some-alloc>` specified directly.  There are two
// ways to specify this trait:
//
// 1. Specialize `IsStdAllocator<some-alloc>` to derive from `bsl::true_type`
//    in namespace `BloombergLP::bslma`.
// 2. Add 'BSLMF_NESTED_TRAIT_DECLARATION(some-alloc,
//    bslma::IsStdAllocator)' within the public portion of the class definition
//    for *some-alloc*.
//
// The first option will bypass any automatic-detection metalogic.  The second
// option is preferred because it will be checked for correctness -- failing to
// compile if the allocator is missing a critical member.
//
// In C++11 and later, the `bslma::IsStdAllocator` trait is detected
// automatically: for any type `A` having a `value_type` and `allocate` method
// that meet the allocator requirements, `bslma::IsStdAllocator<A>::value` will
// be `true`.  However, to prevent inadvertantly declaring an allocator that is
// not detected in a C++03 build, using this trait in C++11 or later build will
// yield a compilation error if it is detected that a type is an allocator does
// not have `IsStdAllocator` explicitly specified.  The simplest way to avoid
// that error is to specify `IsStdAllocator` deliberately for *every*
// allocator, as described above.  Alternatively, defining the
// `BSLMA_ISALLOCATOR_IGNORE_CPP03_COMPATIBILITY` macro will suppress the error
// in C++11 and later builds, defering to automatic allocator detection.
//
// If C++14 variable templates and constexpr variables are supported, the
// variable template `IsStdAllocator_v` is defined to be the value
// `IsStdAllocator<T>::value`.  If C++17 inline variables are supported, it is
// inline.
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: Verify if a class meets the requirements for an allocator.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types meet the requirements
// for allocators.
//
// First, we create a struct type `MyAllocator`:
// ```
// struct MyAllocator
// {
//     BSLMF_NESTED_TRAIT_DECLARATION(MyAllocator, bslma::IsStdAllocator);
//     typedef int value_type;
//     int *allocate(size_t);
//         // Allocate some memory for use by the caller.
// };
// ```
// Now, we instantiate the `bslma::IsStdAllocator` template for both a type
// that does not meet the allocator requirements and the defined type
// `MyClass`, that does, asserting the `value` static data member of each
// instantiation.
// ```
// int main()
// {
//     assert(false == bslma::IsStdAllocator<int>::value);
//     assert(true  == bslma::IsStdAllocator<MyAllocator>::value);
// ```
// Note that if the current compiler supports C++14 variable templates then we
// can re-write the snippet of code above using the `bslma::IsStdAllocator_v`
// variable:
// ```
// #ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//     assert(false == bslma::IsStdAllocator_v<int>);
//     assert(true  == bslma::IsStdAllocator_v<MyAllocator>);
// #endif
// }
// ```

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_detectnestedtrait.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#include <cstddef>  // 'std::size_t'
#include <memory>   // 'std::allocator'

namespace BloombergLP {
namespace bslma {

// FORWARD DECLARATIONS
template <class TYPE, class SIZE_T, class = void> struct IsStdAllocator_Imp;

                  // =======================================
                  // struct template IsStdAllocator_SizeType
                  // =======================================

/// This component-private metafunction determines the appopriate size type
/// for a specified template parameter `TYPE`.  The nested `type` typedef is
/// `TYPE::size_type` if such a type exists and `std::size_t` otherwise.
/// Note that this metafunction produces the same type as
/// `bsl::allocator_traits<TYPE>::size_type`, but avoids a dependency on
/// `bslma_allocatortraits` and is guaranteed to compile even if `TYPE` is
/// not an allocator type.
template <class TYPE, class = void>
struct IsStdAllocator_SizeType
{

    typedef std::size_t type;
};

/// This specialization is selected when `TYPE` has a nested `size_type`.
template <class TYPE>
struct IsStdAllocator_SizeType<
                    TYPE,
                    typename bslmf::VoidType<typename TYPE::size_type>::type> {

    typedef typename TYPE::size_type type;
};

                      // ==============================
                      // struct template IsStdAllocator
                      // ==============================

/// Metafunction to determine whether the specified template parameter
/// `TYPE` meets the minimum requirements for a C++11 allocator.
/// Specifically, this `struct` is derived from `true_type` if `TYPE` has a
/// nested `value_type` and supports the operation `a.allocate(bytes)`,
/// where `a` has type `TYPE` and `bytes` has type
/// `allocator_traits<TYPE>::size_type`; otherwise it is derived from
/// `false_type`.
template <class TYPE>
struct IsStdAllocator
    : IsStdAllocator_Imp<TYPE,
                         typename IsStdAllocator_SizeType<TYPE>::type>::type
{
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
/// This template variable represents the result value of the
/// `bslma::IsStdAllocator` meta-function.
template <class TYPE>
BSLS_KEYWORD_INLINE_VARIABLE
constexpr bool IsStdAllocator_v = IsStdAllocator<TYPE>::value;
#endif

/// Specialization for lvalue reference types.
template <class TYPE>
struct IsStdAllocator<TYPE&> : bsl::false_type {
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
/// Specialization for rvalue reference types.
template <class TYPE>
struct IsStdAllocator<TYPE&&> : bsl::false_type {
};
#endif

/// Specialization for `std::allocator`.
template <class TYPE>
struct IsStdAllocator<std::allocator<TYPE> > : bsl::true_type {
};

/// `std::allocator<void>` is not an allocator type, even though all other
/// specializations are allocator types.
template <>
struct IsStdAllocator<std::allocator<void> > : bsl::false_type {
};

// ============================================================================
//                         TEMPLATE IMPLEMENTATIONS
// ============================================================================

                      // ----------------------------------
                      // struct template IsStdAllocator_Imp
                      // ----------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE

/// This C++11 primary template yields `false_type`.  It is selected when
/// either `TYPE::value_type` does not exists or `a.allocate(bytes)` is
/// ill-formed, where `a` has type `TYPE` and `bytes` has type `SIZE_T`.
template <class TYPE, class SIZE_T, class>
struct IsStdAllocator_Imp : bsl::false_type {

    // If this assert fails, it means that 'bslma::IsStdAllocator' is declared
    // as a nested trait within 'TYPE' but 'TYPE' does not meet the C++11
    // allocator requirements.
    BSLMF_ASSERT(! (bslmf::DetectNestedTrait<TYPE, IsStdAllocator>::value));
};

/// This C++11 specialization yields `true_type` and is selected when
/// `TYPE::value_type` exists and `a.allocate(bytes)` is well-formed, where
/// `a` has type `TYPE` and `bytes` has type `SIZE_T`.
template <class TYPE, class SIZE_T>
struct IsStdAllocator_Imp<
             TYPE,
             SIZE_T,
             bsl::void_t<typename TYPE::value_type,
                         decltype(std::declval<TYPE&>().allocate(SIZE_T()))> >
    : public bsl::true_type {

#ifndef BSLMA_ISALLOCATOR_IGNORE_CPP03_COMPATIBILITY
    // If this assert fails, it means that 'TYPE' meets the C++11 allocator
    // requirements but cannot be detected as being an allocator using a C++03
    // compiler.  Specifically, 'bslma::IsStdAllocator' is neither specialized
    // for 'TYPE' nor declared as a nested trait within 'TYPE'.  Although
    // explicitly declaring this trait would not be necessary for a C++11 or
    // later compiler, this assertion prevents portability errors whereby real
    // allocator types that would not be recognized as such in C++03.  To
    // suppress this compatibility check, '#define'
    // 'BSLMA_ISALLOCATOR_IGNORE_CPP03_COMPATIBILITY' before the '#include' for
    // this header.
    BSLMF_ASSERT((bslmf::DetectNestedTrait<TYPE, IsStdAllocator>::value));
#endif
};

#else // if ! defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)

template <class TYPE, class SIZE_T, class>
struct IsStdAllocator_Imp : bsl::false_type {
    // This C++03 primary template metafunction is derived from 'false_type'.
    // It is selected when the specified template paramter 'TYPE' does not
    // declare itself to be an allocator, i.e., when
    // 'bslmf::DetectNestedTrait<TYPE, IsStdAllocator>::value' is 'false'.
};

template <class TYPE, class SIZE_T>
struct IsStdAllocator_Imp<
               TYPE,
               SIZE_T,
               typename bsl::enable_if<
                   bslmf::DetectNestedTrait<TYPE, IsStdAllocator>::value>::type
    >
    : bsl::true_type {
    // This C++03 specialization is derived from 'true_type'.  It is selected
    // when the specified template parameter 'TYPE' is a class that declares
    // the nested 'IsStdAllocator' trait.  This specialization contains
    // compile-time correctness checks to ensure that 'TYPE' really does have
    // 'value_type' and 'allocator()' members.

  private:
    // TYPES
    template <class T> struct check { };

    // NOT DEFINED
    static TYPE& allocObj;

    // COMPILE-TIME CORRECTNESS CHECK:
    // If 'TYPE' is declared as being an allocator, the following assertions
    // will fail even to compile unless 'TYPE' also meets the minimum
    // requirements for a C++11 allocator.  These tests prevent non-allocators
    // from being declared as allocators.
    BSLMF_ASSERT(sizeof(check<typename TYPE::value_type>) > 0);
    BSLMF_ASSERT(sizeof(allocObj.allocate(SIZE_T())) > 0);
};

#endif // ! BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE

}  // close package namespace
}  // close enterprise namespace

// For backwards compatibility:
namespace bsl {

using BloombergLP::bslma::IsStdAllocator;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
using BloombergLP::bslma::IsStdAllocator_v;
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
