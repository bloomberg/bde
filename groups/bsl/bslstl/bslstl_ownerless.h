// bslstl_ownerless.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_OWNERLESS
#define INCLUDED_BSLSTL_OWNERLESS

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an ordering for shared and weak pointers.
//
//@CLASSES:
//  bsl::owner_less: ordering comparator for 'shared_ptr' and 'weak_ptr'
//
//@CANONICAL_HEADER: bsl_memory.h
//
//@SEE_ALSO: bslstl_sharedptr
//
//@DESCRIPTION: This component provides the C+11 standard binary comparison
// functor, 'bsl::owner_less', that determines the order of two smart pointer
// objects by the relative order of the address of their 'bslma::SharedPtrRep'
// data.  Note that this class is an empty POD type.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'owner_less<void>'
/// - - - - - - - - - - - - - - - - - - - - -
// Suppose we need a map accepting shared pointers as keys.  We also expect
// that this container will be accessible from multiple threads and some of
// them will store weak versions of smart pointers to break reference cycles.
// To avoid excessive conversions we can use a transparent comparator to
// enable heterogeneous lookup with 'bsl::weak_ptr' objects as parameters for
// search functions.
//
// First, we create a container and populate it:
//..
//      typedef bsl::map<bsl::shared_ptr<int>, int, bsl::owner_less<void> >
//                                                                         Map;
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

namespace bsl {

template <class POINTER_TYPE = void>
struct owner_less;

template <class ELEMENT_TYPE>
struct owner_less<shared_ptr<ELEMENT_TYPE> >;

template <class ELEMENT_TYPE>
struct owner_less<weak_ptr<ELEMENT_TYPE> >;

template <>
struct owner_less<void>;

                        // C++11 Compatibility

template <class ELEMENT_TYPE>
struct owner_less<shared_ptr<ELEMENT_TYPE> > {

    // TYPES
    typedef bool                     result_type;
    typedef shared_ptr<ELEMENT_TYPE> first_argument_type;
    typedef shared_ptr<ELEMENT_TYPE> second_argument_type;

    // CREATORS
    //! owner_less() = default;
        // Create an 'owner_less' object.

    //! owner_less(const owner_less& original) = default;
        // Create an 'owner_less' object.  Note that as 'owner_less' is an
        // empty (stateless) type, this operation has no observable effect.

    //! ~owner_less() = default;
        // Destroy this object.

    // MANIPULATORS
    //! owner_less& operator=(const owner_less& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'owner_less' is an empty (stateless) type, this operation
        // has no observable effect.

    // ACCESSORS
    bool operator()(const shared_ptr<ELEMENT_TYPE>& a,
                    const shared_ptr<ELEMENT_TYPE>& b) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    bool operator()(const shared_ptr<ELEMENT_TYPE>& a,
                    const weak_ptr<ELEMENT_TYPE>&   b) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    bool operator()(const weak_ptr<ELEMENT_TYPE>&   a,
                    const shared_ptr<ELEMENT_TYPE>& b) const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by the specified 'a'
        // is ordered before the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by the specified 'b'
        // under the total ordering supplied by
        // 'std::less<BloombergLP::bslma::SharedPtrRep *>', and 'false'
        // otherwise.
};

template <class ELEMENT_TYPE>
struct owner_less<weak_ptr<ELEMENT_TYPE> > {

    // TYPES
    typedef bool                   result_type;
    typedef weak_ptr<ELEMENT_TYPE> first_argument_type;
    typedef weak_ptr<ELEMENT_TYPE> second_argument_type;

    // CREATORS
    //! owner_less() = default;
        // Create an 'owner_less' object.

    //! owner_less(const owner_less& original) = default;
        // Create an 'owner_less' object.  Note that as 'owner_less' is an
        // empty (stateless) type, this operation has no observable effect.

    //! ~owner_less() = default;
        // Destroy this object.

    // MANIPULATORS
    //! owner_less& operator=(const owner_less& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'owner_less' is an empty (stateless) type, this operation
        // has no observable effect.

    // ACCESSORS
    bool operator()(const weak_ptr<ELEMENT_TYPE>&   a,
                    const weak_ptr<ELEMENT_TYPE>&   b) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    bool operator()(const shared_ptr<ELEMENT_TYPE>& a,
                    const weak_ptr<ELEMENT_TYPE>&   b) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    bool operator()(const weak_ptr<ELEMENT_TYPE>&   a,
                    const shared_ptr<ELEMENT_TYPE>& b) const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by the specified 'a'
        // is ordered before the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by the specified 'b'
        // under the total ordering supplied by
        // 'std::less<BloombergLP::bslma::SharedPtrRep *>', and 'false'
        // otherwise.
};

template<>
struct owner_less<void> {

    // TYPES
    typedef void is_transparent;
        // Type alias indicating this is a transparent comparator.

    // CREATORS
    //! owner_less() = default;
        // Create an 'owner_less' object.

    //! owner_less(const owner_less& original) = default;
        // Create an 'owner_less' object.  Note that as 'owner_less' is an
        // empty (stateless) type, this operation has no observable effect.

    //! ~owner_less() = default;
        // Destroy this object.

    // MANIPULATORS
    //! owner_less& operator=(const owner_less& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'owner_less' is an empty (stateless) type, this operation
        // has no observable effect.

    // ACCESSORS
    template<class ELEMENT_TYPE_A, class ELEMENT_TYPE_B>
    bool operator()(const shared_ptr<ELEMENT_TYPE_A> &a,
                    const shared_ptr<ELEMENT_TYPE_B> &b) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    template<class ELEMENT_TYPE_A, class ELEMENT_TYPE_B>
    bool operator()(const shared_ptr<ELEMENT_TYPE_A> &a,
                    const weak_ptr<  ELEMENT_TYPE_B> &b) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    template<class ELEMENT_TYPE_A, class ELEMENT_TYPE_B>
    bool operator()(const weak_ptr<  ELEMENT_TYPE_A> &a,
                    const shared_ptr<ELEMENT_TYPE_B> &b) const
                                                         BSLS_KEYWORD_NOEXCEPT;
    template<class ELEMENT_TYPE_A, class ELEMENT_TYPE_B>
    bool operator()(const weak_ptr<ELEMENT_TYPE_A> &a,
                    const weak_ptr<ELEMENT_TYPE_B> &b) const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by the specified 'a'
        // is ordered before the address of the
        // 'BloombergLP::bslma::SharedPtrRep' object used by the specified 'b'
        // under the total ordering supplied by
        // 'std::less<BloombergLP::bslma::SharedPtrRep *>', and 'false'
        // otherwise.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                // --------------------------------------------
                // struct owner_less<shared_ptr<ELEMENT_TYPE> >
                // --------------------------------------------

template <class ELEMENT_TYPE>
inline
bool owner_less<shared_ptr<ELEMENT_TYPE> >::operator()(
                 const shared_ptr<ELEMENT_TYPE>& a,
                 const shared_ptr<ELEMENT_TYPE>& b) const BSLS_KEYWORD_NOEXCEPT
{
    return a.owner_before(b);
}

template <class ELEMENT_TYPE>
inline
bool owner_less<shared_ptr<ELEMENT_TYPE> >::operator()(
                 const shared_ptr<ELEMENT_TYPE>& a,
                 const weak_ptr<ELEMENT_TYPE>&   b) const BSLS_KEYWORD_NOEXCEPT
{
    return a.owner_before(b);
}

template <class ELEMENT_TYPE>
inline
bool owner_less<shared_ptr<ELEMENT_TYPE> >::operator()(
                 const weak_ptr<ELEMENT_TYPE>&   a,
                 const shared_ptr<ELEMENT_TYPE>& b) const BSLS_KEYWORD_NOEXCEPT
{
    return a.owner_before(b);
}

                // ------------------------------------------
                // struct owner_less<weak_ptr<ELEMENT_TYPE> >
                // ------------------------------------------

template <class ELEMENT_TYPE>
inline
bool owner_less<weak_ptr<ELEMENT_TYPE> >::operator()(
                   const weak_ptr<ELEMENT_TYPE>& a,
                   const weak_ptr<ELEMENT_TYPE>& b) const BSLS_KEYWORD_NOEXCEPT
{
    return a.owner_before(b);
}

template <class ELEMENT_TYPE>
inline
bool owner_less<weak_ptr<ELEMENT_TYPE> >::operator()(
                 const shared_ptr<ELEMENT_TYPE>& a,
                 const weak_ptr<ELEMENT_TYPE>&   b) const BSLS_KEYWORD_NOEXCEPT
{
    return a.owner_before(b);
}

template <class ELEMENT_TYPE>
inline
bool owner_less<weak_ptr<ELEMENT_TYPE> >::operator()(
                 const weak_ptr<ELEMENT_TYPE>&   a,
                 const shared_ptr<ELEMENT_TYPE>& b) const BSLS_KEYWORD_NOEXCEPT
{
    return a.owner_before(b);
}

                        // -----------------------
                        // struct owner_less<void>
                        // -----------------------

template<class ELEMENT_TYPE_A, class ELEMENT_TYPE_B>
inline
bool owner_less<void>::operator()(
               const shared_ptr<ELEMENT_TYPE_A> &a,
               const shared_ptr<ELEMENT_TYPE_B> &b) const BSLS_KEYWORD_NOEXCEPT
{
    return a.owner_before(b);
}

template<class ELEMENT_TYPE_A, class ELEMENT_TYPE_B>
inline
bool owner_less<void>::operator()(
               const shared_ptr<ELEMENT_TYPE_A> &a,
               const weak_ptr<  ELEMENT_TYPE_B> &b) const BSLS_KEYWORD_NOEXCEPT
{
    return a.owner_before(b);
}

template<class ELEMENT_TYPE_A, class ELEMENT_TYPE_B>
inline
bool owner_less<void>::operator()(
               const weak_ptr<  ELEMENT_TYPE_A> &a,
               const shared_ptr<ELEMENT_TYPE_B> &b) const BSLS_KEYWORD_NOEXCEPT
{
    return a.owner_before(b);
}

template<class ELEMENT_TYPE_A, class ELEMENT_TYPE_B>
inline
bool owner_less<void>::operator()(
                 const weak_ptr<ELEMENT_TYPE_A> &a,
                 const weak_ptr<ELEMENT_TYPE_B> &b) const BSLS_KEYWORD_NOEXCEPT
{
    return a.owner_before(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for 'owner_less'
//: o 'owner_less' is a stateless POD, trivially constructible and copyable.

template<class POINTER_TYPE>
struct is_trivially_default_constructible<owner_less<POINTER_TYPE> >
: bsl::true_type
{};

template<class POINTER_TYPE>
struct is_trivially_copyable<owner_less<POINTER_TYPE> >
: bsl::true_type
{};

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
