// bslstl_ownerhash.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_OWNERHASH
#define INCLUDED_BSLSTL_OWNERHASH

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an ownership hash functor for shared and weak pointers.
//
//@CLASSES:
//  bsl::owner_hash: owner hash functor for 'shared_ptr' and 'weak_ptr'
//
//@CANONICAL_HEADER: bsl_memory.h
//
//@SEE_ALSO: bslstl_sharedptr
//
//@DESCRIPTION: This component provides the C++26 standard functor,
// 'bsl::owner_hash', that calculates the hash of a smart pointer object by the
// address of their 'bslma::SharedPtrRep' data.  Note that this class is an
// empty POD type.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'owner_hash'
/// - - - - - - - - - - - - - - - - - -
// Suppose we need an unordered map accepting shared pointers as keys.  We also
// expect that this container will be accessible from multiple threads and some
// of them will store weak versions of smart pointers to break reference
// cycles.
//
// First, we define an owner-based equality predicate, that is required by the
// 'bsl::unordered_map' along with this owner-based hash.
//..
//  struct TestOwnerEqual {
//      // TYPES
//      typedef void is_transparent;
//
//      template <class T1, class T2>
//      bool operator()(const bsl::shared_ptr<T1>& lhs,
//                      const bsl::shared_ptr<T2>& rhs) const
//          // For the specified 'lhs' and 'rhs', return the result of
//          // 'lhs.owner_equal(rhs)'
//      {
//          return lhs.owner_equal(rhs);
//      }
//
//      template <class T1, class T2>
//      bool operator()(const bsl::shared_ptr<T1>& lhs,
//                      const bsl::weak_ptr<T2>&   rhs) const
//          // For the specified 'lhs' and 'rhs', return the result of
//          // 'lhs.owner_equal(rhs)'
//      {
//          return lhs.owner_equal(rhs);
//      }
//
//      template <class T1, class T2>
//      bool operator()(const bsl::weak_ptr<T1>&   lhs,
//                      const bsl::shared_ptr<T2>& rhs) const
//          // For the specified 'lhs' and 'rhs', return the result of
//          // 'lhs.owner_equal(rhs)'
//      {
//          return lhs.owner_equal(rhs);
//      }
//
//      template <class T1, class T2>
//      bool operator()(const bsl::weak_ptr<T1>& lhs,
//                      const bsl::weak_ptr<T2>& rhs) const
//          // For the specified 'lhs' and 'rhs', return the result of
//          // 'lhs.owner_equal(rhs)'
//      {
//          return lhs.owner_equal(rhs);
//      }
//  };
//..
// Note that this struct is defined only to avoid cycle dependencies between
// BDE components.  In real code for these purposes it is recommended to use
// 'bsl::owner_equal'.
//
// Then, we create a container and populate it:
//..
//      typedef bsl::unordered_map<
//          bsl::shared_ptr<int>,
//          int,
//          bsl::owner_hash,
//          TestOwnerEqual> Map;
//
//      Map                  container;
//
//      bsl::shared_ptr<int> sharedPtr1 = bsl::make_shared<int>(1);
//      bsl::shared_ptr<int> sharedPtr2 = bsl::make_shared<int>(2);
//      bsl::weak_ptr<int>   weakPtr1(sharedPtr1);
//
//      container[sharedPtr1] = 1;
//      container[sharedPtr2] = 2;
//..
// Now, we make sure, that shared pointers can be used to perform lookup:
//..
//      Map::const_iterator iter = container.find(sharedPtr1);
//      assert(container.end() != iter        );
//      assert(1               == iter->second);
//
//      iter = container.find(sharedPtr2);
//      assert(container.end() != iter);
//      assert(2               == iter->second);
//..
// Finally, we simulate the situation of accessing the container from another
// thread and perform lookup using weak pointers:
//..
//      iter = container.find(weakPtr1);
//      assert(container.end() != iter        );
//      assert(1               == iter->second);
//
//      bsl::weak_ptr<int> weakPtr3(bsl::make_shared<int>(3));
//      iter = container.find(weakPtr3);
//      assert(container.end() == iter);
//..

#include <bslscm_version.h>

#include <bslstl_sharedptr.h>

#include <bsls_keyword.h> // 'BSLS_KEYWORD_NOEXCEPT'

#include <stddef.h>             // 'size_t'

namespace bsl {

                        // C++26 Compatibility

struct owner_hash {

    // TYPES
    typedef void is_transparent;
        // Type alias indicating this is a transparent hash functor.

    // CREATORS
    //! owner_hash() = default;
        // Create an 'owner_hash' object.

    //! owner_hash(const owner_hash& original) = default;
        // Create an 'owner_hash' object.  Note that as 'owner_hash' is an
        // empty (stateless) type, this operation has no observable effect.

    //! ~owner_hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! owner_hash& operator=(const owner_hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'owner_hash' is an empty (stateless) type, this operation
        // has no observable effect.

    // ACCESSORS
    template <class ELEMENT_TYPE>
    size_t operator()(const shared_ptr<ELEMENT_TYPE>& x) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    template <class ELEMENT_TYPE>
    size_t operator()(const weak_ptr<  ELEMENT_TYPE>& x) const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return the result of calling 'owner_hash()' on the specified 'x'.
        //  Note that for any object 'y' where 'x.owner_equal(y)' is true,
        // 'x.owner_hash() == y.owner_hash()' is true.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                             // -----------------
                             // struct owner_hash
                             // -----------------

template <class ELEMENT_TYPE>
inline
size_t owner_hash::operator()(
                 const shared_ptr<ELEMENT_TYPE>& x) const BSLS_KEYWORD_NOEXCEPT
{
    return x.owner_hash();
}

template <class ELEMENT_TYPE>
inline
size_t owner_hash::operator()(
                   const weak_ptr<ELEMENT_TYPE>& x) const BSLS_KEYWORD_NOEXCEPT
{
    return x.owner_hash();
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for 'owner_hash'
//: o 'owner_hash' is a stateless POD, trivially constructible and copyable.

template<>
struct is_trivially_default_constructible<owner_hash>
: bsl::true_type
{};

template<>
struct is_trivially_copyable<owner_hash>
: bsl::true_type
{};

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
