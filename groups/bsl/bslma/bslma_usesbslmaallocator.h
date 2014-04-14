// bslma_usesbslmaallocator.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#define INCLUDED_BSLMA_USESBSLMAALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a metafunction that indicates the use of bslma allocators
//
//@CLASSES: 
//  bslma::UsesBslmaAllocator<TYPE>: trait detection metafunction 
//
//@SEE_ALSO: bslalg_typetraitusesblsmaallocator
//
//@DESCRIPTION: This component defines a meta-function, 
// 'bslma::UsesBslmaAllocator', that may be used both to associate 
// a type with the uses-bslma allocator trait (i.e., declare a type uses a
// bslma allocator), and to detect whether a type has been associated with that
// trait (i.e., to test whether a type uses a bslma allocator).
//
///Properties of Types Declaring the 'UsesBslmaAllocator' Trait
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Types that declare the 'UsesBslmaAllocator' trait must meet some minimal 
// requirements in order for that type to be usable with other types that test fr the
// 'UsesBslmaAllocator' trait (e.g., 'bsl' containers).  In addition to syntactic requirements enforced by the compilers, types using allocators and declaring the 'UsesBslmaAllocator' trait are generally expected to have certain properties with respect to  memory allocation.   
//
///Compiler Enforced Requirements of Types Declaring 'UsesBslmaAllocator' 
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Types declaring the 'UsesBslmaAllocator' trait must provide a constructor 
// overload that accepts a 'bslma::Allocator *' as the last parameter.  If a type provides a copy-constructor, it must similary provide a variant that takes a 'bslma::Allocator *' as the last parameter.
//
// Template types (like 'bsl' containers) will frequently, at compile-time, use the 'UsesBslmaAlloctor' trait to test if the contained type uses bslma allocators, and if the contained type does use an allocator, call the constructor variant taking an allocator and supply the container's allocator.  Compilation will fail if
// contained type declares the 'UsesBslmaAllocator' trait, but does not provide
// the expected constructor overload.
//
///Expected Properties of Types Deeclaring the 'UsesBslmaAllocator' Trait
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Types declaring the 'UsesBslmaAllocator' trait are expected to have certain properties with respect to memory allocation.  These properties are not enforced by the compiler, but are important to ensure consistent and comprehensible allocation behavior in a task.
// 
//: o The allocator supplied at construction will be used for all non-transient memory allocation, including allocations provided by sub-objects (i.e., the type will provide the supplied allocator to any data-members which themselves accept an allocator).

//: o If an allocator is not supplied, then the currently installed default allocator will be used (see bslma::Default).

//: o The allocator used by a object is not modified after construction (e.g., the assignment operator does not change the allocator used by a type).
//
//: o Transient memory allocations -- i.e., methods that allocate memory that is de-allocated before the method returns -- are performed using the currently installed default allocator.  For example:  a temporary 'bsl::string' that use destroyed within the scope of a method need not be explicitly supplied an allocator, since it is a transient allocation, and 'bsl::string' will use the default allocator by default.
//
// There may be circumstances where a type that uses an allocator deliberately does not have one (or more) of these properties/  Because of the confusion deviation from these properties can cause, such exceptions should be made with care and be well documented.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
// Example 1: Associating the 'bslma' Allocator Trait with a Type
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
//  class UsesAllocatorType1 {
//      // ...
//  
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(UsesAllocatorType1,
//                                     bslma::UsesBslmaAllocator);
//      // CREATORS
//      explicit UsesAllocatorType1(bslma::Allocator *basicAllocator = 0);
//         // ...
//  
//      UsesAllocatorType1(const UsesAllocatorType1&  original, 
//                     bslma::Allocator              *basicAllocator = 0);
//         // ...
//  };
//..
// Notice that the macro declaration is performed within the scope of the class
// declaration, and must be done with public scope.
//
// Then, we declare a type 'UsesAllocatorType2', and define a specialization of
// the 'UsesBslmaAllocator' trait for 'UsesAllocatorType2' that effectively
// associates the 'UsesBslmaAllocator' trait with the type (note that this is 
// sometimes referred to as a "C++11 style" trait declaration, since it is more
// in keeping with the style of trait declarations found in the C++11 standard):
//..
//  class UsesAllocatorType2 {
//      // ...
//  
//    public:
//      // CREATORS
//      explicit UsesAllocatorType2(bslma::Allocator *basicAllocator = 0);
//         // ...
//  
//      UsesAllocatorType2(const UsesAllocatorType2&  original, 
//                         bslma::Allocator          *basicAllocator = 0);
//         // ...
//  };
//  
//  namespace BloombergLP {
//  namespace bslma {
//  
//  template <> struct UsesBslmaAllocator<UsesAllocatorType2> : bsl::true_type 
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
// Suppose we want to test whether each of a set different types use a 'bslma' 
// allocator. 
//
// Here, we uses the 'UsesBslmaAllocator' template to test whether the types
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
// Finally, we deomnstrate that the trait can be tested at compilation time, 
// by testing the trait within the context of a compile-time 'BSLMF_ASSERT':
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

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEPOINTER
#include <bslmf_removepointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ========================
                        // class UsesBslmaAllocator
                        // ========================

template <typename TYPE, bool IS_NESTED>
struct UsesBslmaAllocator_Imp
{
    typedef bsl::integral_constant<bool, IS_NESTED> Type;
};

template <typename TYPE>
struct UsesBslmaAllocator_Imp<TYPE, false>
{
private:
    struct UniqueType {
        // A class convertible from this type must have a templated
        // constructor or a 'void*' which makes it convertible from EVERY
        // pointer type.
    };

    enum {
        // Detect if 'TYPE' is 'Allocator*' type.
        IS_BSLMA_POINTER
            = bsl::is_same<
                Allocator,
                typename bsl::remove_cv<
                    typename bsl::remove_pointer<TYPE>::type>::type>::value,

        // If a pointer to 'Allocator' is convertible to 'T', then 'T' has a
        // non-explcit constructor taking an allocator.
        BSLMA_POINTER_CTOR = bsl::is_convertible<Allocator *, TYPE>::value,

        // If a pointer to 'UniqueType' is convertible to 'T', it can only mean
        // that ANY POINTER is convertible to 'T'.
        ANY_POINTER_CTOR = bsl::is_convertible<UniqueType *, TYPE>::value
    };

public:
    typedef bsl::integral_constant<bool,
                                   !IS_BSLMA_POINTER
                                   && BSLMA_POINTER_CTOR
                                   && !ANY_POINTER_CTOR>
        Type;
};

template <typename TYPE>
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

template <typename TYPE>
struct UsesBslmaAllocator<const TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Specialization that associates the same trait with 'const TYPE' as with
    // unqualified 'TYPE'.
};

template <typename TYPE>
struct UsesBslmaAllocator<volatile TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Specialization that associates the same trait with 'volatile TYPE' as
    // with unqualified 'TYPE'.
};

template <typename TYPE>
struct UsesBslmaAllocator<const volatile TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Specialization that associates the same trait with 'const volatile
    // TYPE' as with unqualified 'TYPE'.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
