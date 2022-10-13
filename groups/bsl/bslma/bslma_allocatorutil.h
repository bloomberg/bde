// bslma_allocatorutil.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMA_ALLOCATORUTIL
#define INCLUDED_BSLMA_ALLOCATORUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for utility functions on allocators
//
//@CLASSES:
//  bslma::AllocatorUtil: Namespace for utility functions on allocators
//
//@SEE_ALSO: bslma_aatypeutil, bslma_allocatortraits
//
//@DESCRIPTION: This component provides a namespace 'struct',
// 'bslma::AllocatorUtil', with functions that operate on both raw pointers to
// 'bslma::Allocator' or derived classes and objects of C++11 compliant
// allocator classes.
//
///Usage
///-----
//
///Example 1: Future-proofing Member Construction
///- - - - - - - - - - - - - - - - - - - - - - -
// This example shows how we construct an AA member variable, using
// 'bslma::AllocatorUtil::adapt' so that it is both self-documenting and robust
// in case the member type is modernized from *legacy-AA* (using
// 'bslma::Allocator *' directly in its interface) to *bsl-AA* (using
// 'bsl::allocator' in its interface).
//
// First, we define a class, 'Data1', that has a legacy-AA interface:
//..
//  class Data1 {
//      // Legacy-AA data class.
//
//      bslma::Allocator *d_allocator_p;
//      // ...
//
//    public:
//      explicit Data1(bslma::Allocator *basicAllocator = 0)
//          : d_allocator_p(basicAllocator) { /* ... */ }
//
//      bslma::Allocator *allocator() const { return d_allocator_p; }
//  };
//..
// Next, we define a class, 'MyClass1', that has a member of type 'Data1'.
// 'MyClass' uses a modern, bsl-AA interface:
//..
//  class MyClass1 {
//      bsl::allocator<char> d_allocator;
//      Data1                d_data;
//
//    public:
//      typedef bsl::allocator<char> allocator_type;
//
//      explicit MyClass1(const allocator_type& allocator = allocator_type());
//
//      const Data1& data() const { return d_data; }
//      allocator_type get_allocator() const { return d_allocator; }
//  };
//..
// Next, we define the constructor for 'MyClass1'.  Since 'MyClass1' uses
// 'bsl::allocator' and the 'Data1' uses 'bslma::Allocator *', we employ
// 'bslma::AllocatorUtil::adapt' to obtain an allocator suitable for passing to
// the constructor for 'd_data':
//..
//  MyClass1::MyClass1(const allocator_type& allocator)
//      : d_allocator(allocator)
//      , d_data(bslma::AllocatorUtil::adapt(allocator))
//  {
//  }
//..
// Next, assume that we update our 'Data' class from legacy-AA to bsl-AA
// (renamed from 'Data1' to 'Data2' for illustrative purposes):
//..
//  class Data2 {
//      // Bsl-AA data class.
//
//      bsl::allocator<int> d_allocator;
//      // ...
//
//    public:
//      typedef bsl::allocator<int> allocator_type;
//
//      explicit Data2(const allocator_type& allocator = allocator_type())
//          : d_allocator(allocator) { /* ... */ }
//
//      allocator_type get_allocator() const { return d_allocator; }
//  };
//..
// Now, we notice that **nothing** about 'MyClass' needs to change, not even
// the way its constructed passes an allocator to 'd_data':
//..
//  class MyClass2 {
//      bsl::allocator<char> d_allocator;
//      Data2                d_data;
//
//    public:
//      typedef bsl::allocator<char> allocator_type;
//
//      explicit MyClass2(const allocator_type& allocator = allocator_type());
//
//      const Data2& data() const { return d_data; }
//      allocator_type get_allocator() const { return d_allocator; }
//  };
//
//  MyClass2::MyClass2(const allocator_type& allocator)
//      : d_allocator(allocator)
//      , d_data(bslma::AllocatorUtil::adapt(allocator))
//  {
//  }
//..
// Finally, we test both versions of 'MyClass' and show that the allocator that
// is passed to the 'MyClass' constructor gets forwarded to its data member:
//..
//  int main()
//  {
//      bslma::TestAllocator ta;
//      bsl::allocator<char> alloc(&ta);
//
//      MyClass1 obj1(alloc);
//      assert(&ta == obj1.data().allocator());
//
//      MyClass2 obj2(alloc);
//      assert(alloc == obj2.data().get_allocator());
//  }
//..

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>

#include <bslmf_integralconstant.h>
#include <bslmf_enableif.h>

namespace BloombergLP {
namespace bslma {

                        // ===================
                        // class AllocatorUtil
                        // ===================

struct AllocatorUtil {
    // Namespace for utility functions on allocators

  private:
    // PRIVATE CLASS METHODS
    template <class TYPE>
    static char matchBslAlloc(bsl::allocator<TYPE> *, int);
    static long matchBslAlloc(void *,                 ...);
        // DECLARED BUT NOT DEFINED

    // PRIVATE TYPES
    template <class ALLOC>
    struct IsDerivedFromBslAllocator
        : bsl::integral_constant<bool,
                                 1 == sizeof(matchBslAlloc((ALLOC *) 0, 0))>
    {
        // Metafunction derives from 'true_type' if (template argument) 'ALLOC'
        // is derived from any specialization of 'bsl::allocator'; else derives
        // from 'false_type'.
    };

  public:
    // CLASS METHODS
    template <class ALLOC>
    static typename bsl::enable_if<
        ! IsDerivedFromBslAllocator<ALLOC>::value,
        ALLOC>::type
    adapt(const ALLOC& from);
    template <class TYPE>
    static bslma::Allocator *adapt(const bsl::allocator<TYPE>& from);
        // Return the specified 'from' allocator adapted to a type most likely
        // to be usable for initializing another AA object.  Specifically,
        // return 'from.mechanism()' if 'from' is a specialization of
        // 'bsl::allocator' (or a class derived from 'bsl::allocator');
        // otherwise return 'from' unchanged.
};

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // -------------------
                        // class AllocatorUtil
                        // -------------------

// CLASS METHODS
template <class ALLOC>
inline
typename bsl::enable_if<
        ! AllocatorUtil::IsDerivedFromBslAllocator<ALLOC>::value,
        ALLOC>::type
AllocatorUtil::adapt(const ALLOC& from)
{
    return from;
}

template <class TYPE>
inline
bslma::Allocator *AllocatorUtil::adapt(const bsl::allocator<TYPE>& from)
{
    return from.mechanism();
}

}  // close package namespace
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMA_ALLOCATORUTIL)

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
