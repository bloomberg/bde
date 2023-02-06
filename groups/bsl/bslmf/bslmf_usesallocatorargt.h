// bslmf_usesallocatorargt.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_USESALLOCATORARGT
#define INCLUDED_BSLMF_USESALLOCATORARGT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a metafunction for 'allocator_arg_t' construction
//
//@CLASSES:
//  bslmf::UsesAllocatorArgT: metafunction for 'allocator_arg_t' checking
//
//@SEE_ALSO: bslmf_allocatorargt
//
//@DESCRIPTION: C++11 introduced an idiom for passing allocators to the
// constructors of classes in situations where putting the allocator as the
// last argument would have been ambiguous.  For example, in cases where the
// type and number of arguments is unknown at specification time, it can be
// difficult or impossible to determine if the last argument should be used as
// the allocator for the constructed object.
//
// The idiom adopted is to pass an object of the empty tag class
// 'std::allocator_arg_t' (or 'bsl::allocator_arg_t', within the BSL library)
// as the first argument in the constructor, followed immediately by the
// allocator argument.
//
// The 'UsesAllocatorArgT<T>' metafunction inherits from 'true_type' of 'T' is
// one of the classes that uses this idiom; otherwise 'false_type'.  It is used
// when constructing elements of a container or members of a class template, to
// determine whether and how to pass an allocator to that element or member.
// By default, any 'T' is assumed NOT to use the 'allocator_arg_t' idiom.
// Specialization of this trait is required for classes that do use this idiom.
// Although a C++11 trait could be designed to automatically determine whether
// class 'T' uses the idiom for a specific set of constructor arguments, such
// code would not be portable to C++03 compilers, a requirement for BSL
// classes.
//
// Given a type 'T" where 'bslmf::UsesAllocatorArgT<T>::value' is true, if
// 'bslma::UsesBslmaAllocator<T>::value' is also true, then the argument after
// 'allocator_arg' can be of type 'bslma::Allocator*'; otherwise, it must be of
// a type that meets the STL allocator requirements.
//
///Usage
///-----

#include <bslscm_version.h>

#include <bslmf_detectnestedtrait.h>

namespace BloombergLP {

namespace bslmf {

                        // ================================
                        // class template UsesAllocatorArgT
                        // ================================

template <class t_TYPE>
struct UsesAllocatorArgT : DetectNestedTrait<t_TYPE, UsesAllocatorArgT>::type {
    // User-specialized trait type indicating that the constructor of 't_TYPE'
    // can be invoked using 'bsl::allocator_arg' as its first argument and an
    // allocator object as its second argument.
};

template <class t_TYPE>
struct UsesAllocatorArgT<const t_TYPE> : UsesAllocatorArgT<t_TYPE>::type {
    // Trait metafunction that determines whether the constructor of 't_TYPE'
    // can be invoked using 'bsl::allocator_arg' as its first argument and an
    // allocator object as its second argument.  The value is computed by
    // stripping off the cv-qualifier and forwading to
    // 'UsesAllocatorArgT<t_TYPE>', which is user-specialized for the
    // appropriate types.
};

template <class t_TYPE>
struct UsesAllocatorArgT<volatile t_TYPE> : UsesAllocatorArgT<t_TYPE>::type {
    // Trait metafunction that determines whether the constructor of 't_TYPE'
    // can be invoked using 'bsl::allocator_arg' as its first argument and an
    // allocator object as its second argument.  The value is computed by
    // stripping off the cv-qualifier and forwading to
    // 'UsesAllocatorArgT<t_TYPE>', which is user-specialized for the
    // appropriate types.
};

template <class t_TYPE>
struct UsesAllocatorArgT<const volatile t_TYPE>
: UsesAllocatorArgT<t_TYPE>::type {
    // Trait metafunction that determines whether the constructor of 't_TYPE'
    // can be invoked using 'bsl::allocator_arg' as its first argument and an
    // allocator object as its second argument.  The value is computed by
    // stripping off the cv-qualifier and forwading to
    // 'UsesAllocatorArgT<t_TYPE>', which is user-specialized for the
    // appropriate types.
};

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_USESALLOCATORARGT)

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
