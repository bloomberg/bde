// bslstl_ownerequal.h                                                -*-C++-*-
#ifndef INCLUDED_BSLSTL_OWNEREQUAL
#define INCLUDED_BSLSTL_OWNEREQUAL

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an ownership comparison for shared and weak pointers.
//
//@CLASSES:
//  bsl::owner_equal: owner equality comparator for 'shared_ptr' and 'weak_ptr'
//
//@CANONICAL_HEADER: bsl_memory.h
//
//@SEE_ALSO: bslstl_sharedptr
//
//@DESCRIPTION: This component provides the C++26 standard binary comparison
// functor, 'bsl::owner_equal', that determines the equality of two smart
// pointer objects by the address of their 'bslma::SharedPtrRep' data.  Note
// that this class is an empty POD type.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'owner_equal'
///- - - - - - - - - - - - - - - - - - -
// Suppose we need an unordered map accepting shared pointers as keys.  We also
// expect that this container will be accessible from multiple threads and some
// of them will store weak versions of smart pointers to break reference
// cycles.
//
// First, we create a container and populate it:
//..
//      typedef bsl::unordered_map<
//          bsl::shared_ptr<int>,
//          int,
//          bsl::owner_hash,
//          bsl::owner_equal> Map;
//
//      Map                  container;
//      bsl::shared_ptr<int> sharedPtr1 = bsl::make_shared<int>(1);
//      bsl::shared_ptr<int> sharedPtr2 = bsl::make_shared<int>(2);
//      bsl::weak_ptr<int>   weakPtr1(sharedPtr1);
//
//      container[sharedPtr1] = 1;
//      container[sharedPtr2] = 2;
//..
// Then we make sure that shared pointers can be used to perform lookup, and
// verify that the results are correct.
//..
//      Map::const_iterator iter = container.find(sharedPtr1);
//      assert(container.end() != iter        );
//      assert(1               == iter->second);
//
//      iter = container.find(sharedPtr2);
//      assert(container.end() != iter);
//      assert(2               == iter->second);
//..
// Finally, we simulate the accessing the container from another thread and
// perform lookup using weak pointers:
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

namespace bsl {

                        // C++26 Compatibility

struct owner_equal {

    // TYPES
    typedef void is_transparent;
        // Type alias indicating this is a transparent comparator.

    // CREATORS
    //! owner_equal() = default;
        // Create an 'owner_equal' object.

    //! owner_equal(const owner_equal& original) = default;
        // Create an 'owner_equal' object.  Note that as 'owner_equal' is an
        // empty (stateless) type, this operation has no observable effect.

    //! ~owner_equal() = default;
        // Destroy this object.

    // MANIPULATORS
    //! owner_equal& operator=(const owner_equal& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'owner_equal' is an empty (stateless) type, this operation
        // has no observable effect.

    // ACCESSORS
    template<class ELEMENT_TYPE_X, class ELEMENT_TYPE_Y>
    bool operator()(const shared_ptr<ELEMENT_TYPE_X>& x,
                    const shared_ptr<ELEMENT_TYPE_Y>& y) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    template<class ELEMENT_TYPE_X, class ELEMENT_TYPE_Y>
    bool operator()(const shared_ptr<ELEMENT_TYPE_X>& x,
                    const weak_ptr<  ELEMENT_TYPE_Y>& y) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    template<class ELEMENT_TYPE_X, class ELEMENT_TYPE_Y>
    bool operator()(const weak_ptr<  ELEMENT_TYPE_X>& x,
                    const shared_ptr<ELEMENT_TYPE_Y>& y) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    template<class ELEMENT_TYPE_X, class ELEMENT_TYPE_Y>
    bool operator()(const weak_ptr<ELEMENT_TYPE_X>& x,
                    const weak_ptr<ELEMENT_TYPE_Y>& y) const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by the specified 'x'
        // is equal to the address of the 'BloombergLP::bslma::SharedPtrRep'
        // object used by the specified 'y', and 'false' otherwise.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                             // ------------------
                             // struct owner_equal
                             // ------------------

template<class ELEMENT_TYPE_X, class ELEMENT_TYPE_Y>
inline
bool owner_equal::operator()(
               const shared_ptr<ELEMENT_TYPE_X>& x,
               const shared_ptr<ELEMENT_TYPE_Y>& y) const BSLS_KEYWORD_NOEXCEPT
{
    return x.owner_equal(y);
}

template<class ELEMENT_TYPE_X, class ELEMENT_TYPE_Y>
inline
bool owner_equal::operator()(
               const shared_ptr<ELEMENT_TYPE_X>& x,
               const weak_ptr<  ELEMENT_TYPE_Y>& y) const BSLS_KEYWORD_NOEXCEPT
{
    return x.owner_equal(y);
}

template<class ELEMENT_TYPE_X, class ELEMENT_TYPE_Y>
inline
bool owner_equal::operator()(
               const weak_ptr<  ELEMENT_TYPE_X>& x,
               const shared_ptr<ELEMENT_TYPE_Y>& y) const BSLS_KEYWORD_NOEXCEPT
{
    return x.owner_equal(y);
}

template<class ELEMENT_TYPE_X, class ELEMENT_TYPE_Y>
inline
bool owner_equal::operator()(
                 const weak_ptr<ELEMENT_TYPE_X>& x,
                 const weak_ptr<ELEMENT_TYPE_Y>& y) const BSLS_KEYWORD_NOEXCEPT
{
    return x.owner_equal(y);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for 'owner_equal'
//: o 'owner_equal' is a stateless POD, trivially constructible and copyable.

template <>
struct is_trivially_default_constructible<owner_equal> : bsl::true_type {};

template <>
struct is_trivially_copyable<owner_equal> : bsl::true_type {};

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
