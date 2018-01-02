// bslma_usesbslmaallocator.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#define INCLUDED_BSLMA_USESBSLMAALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a metafunction that indicates the use of bslma allocators.
//
//@CLASSES:
//  bslma::UsesBslmaAllocator: trait detection metafunction
//
//@SEE_ALSO: bslalg_typetraitusesblsmaallocator
//
//@DESCRIPTION: This component defines a meta-function,
// 'bslma::UsesBslmaAllocator', that may be used to associate a type with the
// uses-bslma allocator trait (i.e., declare that a type uses a bslma
// allocator), and also to detect whether a type has been associated with that
// trait (i.e., to test whether a type uses a bslma allocator, and follows the
// bslma allocator model).
//
///Properties of Types Declaring the 'UsesBslmaAllocator' Trait
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Types that declare the 'UsesBslmaAllocator' trait must meet some minimal
// requirements in order for that type to be usable with code that tests for
// the 'UsesBslmaAllocator' trait (e.g., 'bsl' containers).  In addition, types
// that use allocators must have certain properties with respect to memory
// allocation, which are not enforced by the compiler (such a type is described
// as following the bslma allocator model).
//
///Compiler Enforced Requirements of Types Declaring 'UsesBslmaAllocator'
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Types declaring the 'UsesBslmaAllocator' trait must provide a constructor
// variants that accepts a 'bslma::Allocator *' as the last parameter
// (typically this is an optional parameter).  If such a type provides a
// copy-constructor, it must similarly provide a variant that takes a
// (optional) 'bslma::Allocator *' as the last parameter.
//
// Template types (such as 'bsl' containers), where the template parameter
// 'TYPE' represents some element type encapsulated by the class template,
// often use the 'UsesBslmaAllocator' trait to test if 'TYPE' uses 'bslma'
// allocators, and if so, to create 'TYPE' objects using the constructor
// variant taking an allocator.  In this context, compilation will fail if a
// type declares the 'UsesBslmaAllocator' trait, but does not provide the
// expected constructor variant accepting a 'bslma::Allocator *' as the last
// parameter.
//
///Expected Properties of Types Declaring the 'UsesBslmaAllocator' Trait
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Types declaring the 'UsesBslmaAllocator' trait are expected to have certain
// properties with respect to memory allocation.  These properties are not
// enforced by the compiler, but are necessary to ensure consistent and
// comprehensible allocation behavior.
//
//: o The allocator supplied at construction of an object will be used for
//:   non-transient memory allocation during the object's lifetime.  This
//:   particularly includes allocations performed by sub-objects that
//:   themselves support the bslma allocator model (i.e., the type will provide
//:   the supplied allocator to any data-members which themselves accept an
//:   allocator).
//:
//: o The allocator used by an object is not changed after construction (e.g.,
//:   the assignment operator does not change the allocator used by a type).
//
//: o Transient memory allocations -- i.e., allocations performed within the
//:   scope of a function where the resulting memory is de-allocated before
//:   that function returns -- are generally *not* performed using the object's
//:   allocator.  Although clients may choose whichever allocator suits the
//:   specific context, most often transient memory allocations are performed
//:   using the currently installed default allocator.  For example: a
//:   temporary 'bsl::string' that is destroyed within the scope of a method
//:   need not be explicitly supplied an allocator, since it is a transient
//:   allocation, and 'bsl::string' will use the default allocator by default.
//:
//: o The allocator used by an object is not part of an object's value (e.g.,
//:   it is not tested by comparison operations like 'operator==').
//:
//: o If an allocator is not supplied at construction, then the currently
//:   installed default allocator will typically be used (see 'bslma_default').
//:
//: o Singleton objects, when necessary, allocate memory from the global
//:   allocator (see 'bslma_default')
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Associating the 'bslma' Allocator Trait with a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to declare two types that make use of a 'bslma' allocator,
// and need to associate the 'UsesBslmaAllocator' trait with those types (so
// that they behave correctly when inserted into a 'bsl' container, for
// example).  In this example we will demonstrate two different mechanisms by
// which a trait may be associated with a type.
//
// First, we declare a type 'UsesAllocatorType1', using the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the type with the
// 'UsesBslmaAllocator' trait:
//..
//  namespace xyz {
//
//  class UsesAllocatorType1 {
//      // ...
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(UsesAllocatorType1,
//                                     bslma::UsesBslmaAllocator);
//      // CREATORS
//      explicit UsesAllocatorType1(bslma::Allocator *basicAllocator = 0);
//          // ...
//
//      UsesAllocatorType1(const UsesAllocatorType1&  original,
//                         bslma::Allocator          *basicAllocator = 0);
//          // ...
//  };
//..
// Notice that the macro declaration is performed within the scope of the class
// declaration, and must be done with public scope.
//
// Then, we declare a type 'UsesAllocatorType2', and define a specialization of
// the 'UsesBslmaAllocator' trait for 'UsesAllocatorType2' that associates the
// 'UsesBslmaAllocator' trait with the type (note that this is sometimes
// referred to as a "C++11 style" trait declaration, since it is more in
// keeping with the style of trait declarations found in the C++11 standard):
//..
//  class UsesAllocatorType2 {
//      // ...
//
//    public:
//      // CREATORS
//      explicit UsesAllocatorType2(bslma::Allocator *basicAllocator = 0);
//          // ...
//
//      UsesAllocatorType2(const UsesAllocatorType2&  original,
//                         bslma::Allocator          *basicAllocator = 0);
//          // ...
//  };
//
//  }  // close package namespace
//
//  namespace BloombergLP {
//  namespace bslma {
//
//  template <> struct UsesBslmaAllocator<xyz::UsesAllocatorType2> :
//                                                               bsl::true_type
//  {};
//
//  }  // close package namespace
//  }  // close enterprise namespace
//..
// Notice that the specialization must be performed in the 'BloombergLP::bslma'
// namespace.
//
///Example 2: Testing Whether a Types Uses a 'bslma' Allocator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to test whether each of a set of different types use a
// 'bslma' allocator.
//
// Here, we use the 'UsesBslmaAllocator' template to test whether the types
// 'DoesNotUseAnAllocatorType', 'UsesAllocatorType1', and 'UsesAllocatorType2'
// (defined above) use allocators:
//..
//  assert(false ==
//         bslma::UsesBslmaAllocator<DoesNotUseAnAllocatorType>::value);
//
//  assert(true  ==
//         bslma::UsesBslmaAllocator<UsesAllocatorType1>::value);
//
//  assert(true  ==
//         bslma::UsesBslmaAllocator<UsesAllocatorType2>::value);
//..
// Finally, we demonstrate that the trait can be tested at compilation time, by
// testing the trait within the context of a compile-time 'BSLMF_ASSERT':
//..
//  BSLMF_ASSERT(false ==
//               bslma::UsesBslmaAllocator<DoesNotUseAnAllocatorType>::value);
//
//  BSLMF_ASSERT(true  ==
//               bslma::UsesBslmaAllocator<UsesAllocatorType1>::value);
//
//  BSLMF_ASSERT(true ==
//               bslma::UsesBslmaAllocator<UsesAllocatorType2>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ========================
                        // class UsesBslmaAllocator
                        // ========================

template <class TYPE, bool IS_NESTED>
struct UsesBslmaAllocator_Imp
{
    typedef bsl::integral_constant<bool, IS_NESTED> Type;
};

template <class TYPE>
struct UsesBslmaAllocator_Imp<TYPE, false>
{
  private:
    struct UniqueType {
        // A class convertible from this type must have a constructor template
        // callable with a single argument of any (pointer) type, or a
        // constructor with a single 'void *' parameter (or all subsequent
        // parameters have a default argument), which makes it convertible from
        // EVERY pointer type.
    };

    enum {
        // If a pointer to 'Allocator' is convertible to 'T', then 'T' has a
        // non-explicit constructor taking an allocator.
        k_BSLMA_POINTER_CTOR = bsl::is_convertible<Allocator *, TYPE>::value,

        // If a pointer to 'UniqueType' is convertible to 'T', it can only mean
        // that ANY POINTER is convertible to 'T'.
        k_ANY_POINTER_CTOR = bsl::is_convertible<UniqueType *, TYPE>::value
    };

  public:
    typedef bsl::integral_constant<bool,
                                   k_BSLMA_POINTER_CTOR
                                   && !k_ANY_POINTER_CTOR>
        Type;
};

template <class TYPE>
struct UsesBslmaAllocator
    : UsesBslmaAllocator_Imp<
        TYPE,
        bslmf::DetectNestedTrait<TYPE, UsesBslmaAllocator>::value>::Type::type
{
    // This metafunction is derived from 'true_type' if 'TYPE' adheres to the
    // 'bslma' allocator usage idiom and 'false_type' otherwise.  Note that
    // this trait must be explicitly associated with a type in order for this
    // metafunction to return true; simply having a constructor that implicitly
    // converts 'bslma::Allocator*' to 'TYPE' is no longer sufficient for
    // considering a type follow the idiom.
};

template <class TYPE>
struct UsesBslmaAllocator<TYPE *> : bsl::false_type
{
    // Specialization that avoids special-case template metaprogramming to
    // handle 'bslma::Allocator *' being convertible to itself, but not being
    // a type that uses allocators.  This is true for all pointers, so we take
    // advantage of the simpler metaprogram in all such cases.
};

template <class TYPE>
struct UsesBslmaAllocator<TYPE&> : bsl::false_type
{
    // Specialization that avoids special-case template metaprogramming to
    // handle 'bslma::Allocator *' being convertible to a 'const &' via a
    // temporary object.
};

template <class TYPE>
struct UsesBslmaAllocator<const TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Specialization that associates the same trait with 'const TYPE' as with
    // unqualified 'TYPE'.
};

template <class TYPE>
struct UsesBslmaAllocator<volatile TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Specialization that associates the same trait with 'volatile TYPE' as
    // with unqualified 'TYPE'.
};

template <class TYPE>
struct UsesBslmaAllocator<const volatile TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Specialization that associates the same trait with 'const volatile
    // TYPE' as with unqualified 'TYPE'.
};

}  // close package namespace

}  // close enterprise namespace

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
