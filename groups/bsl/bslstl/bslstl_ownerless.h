// bslstl_ownerless.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_OWNERLESS
#define INCLUDED_BSLSTL_OWNERLESS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an ordering for shared and weak pointers.
//
//@CLASSES:
//  bsl::owner_less: ordering comparator for 'shared_ptr' and 'weak_ptr'
//
//@SEE_ALSO: bslstl_sharedptr
//
//@DESCRIPTION: This component provides the C+11 standard binary comparison
// functor, 'bsl::owner_less', that determines the order of two smart pointer
// objects by the relative order of the address of their 'bslma::SharedPtrRep'
// data.  Note that this class is an empty POD type.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_memory.h> instead of <bslstl_ownerless.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_SHAREDPTR
#include <bslstl_sharedptr.h>
#endif

namespace bsl {

template <class POINTER_TYPE>
struct owner_less;

template <class ELEMENT_TYPE>
struct owner_less<weak_ptr<ELEMENT_TYPE> >;

template <class ELEMENT_TYPE>
struct owner_less<weak_ptr<ELEMENT_TYPE> >;

                        // C++11 Compatibility

template <class ELEMENT_TYPE>
struct owner_less<shared_ptr<ELEMENT_TYPE> > {

    // STANDARD TYPEDEFS
    typedef bool result_type;
    typedef shared_ptr<ELEMENT_TYPE> first_argument_type;
    typedef shared_ptr<ELEMENT_TYPE> second_argument_type;

    //! owner_less() = default;
        // Create an 'owner_less' object.

    //! owner_less(const owner_less& original) = default;
        // Create an 'owner_less' object.  Note that as 'hash' is an empty
        // (stateless) type, this operation has no observable effect.

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
                    const shared_ptr<ELEMENT_TYPE>& b) const;
    bool operator()(const shared_ptr<ELEMENT_TYPE>& a,
                    const weak_ptr<ELEMENT_TYPE>&   b) const;
    bool operator()(const weak_ptr<ELEMENT_TYPE>&   a,
                    const shared_ptr<ELEMENT_TYPE>& b) const;
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

    // STANDARD TYPEDEFS
    typedef bool result_type;
    typedef weak_ptr<ELEMENT_TYPE> first_argument_type;
    typedef weak_ptr<ELEMENT_TYPE> second_argument_type;

    //! owner_less() = default;
        // Create an 'owner_less' object.

    //! owner_less(const owner_less& original) = default;
        // Create an 'owner_less' object.  Note that as 'hash' is an empty
        // (stateless) type, this operation has no observable effect.

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
                    const weak_ptr<ELEMENT_TYPE>&   b) const;
    bool operator()(const shared_ptr<ELEMENT_TYPE>& a,
                    const weak_ptr<ELEMENT_TYPE>&   b) const;
    bool operator()(const weak_ptr<ELEMENT_TYPE>&   a,
                    const shared_ptr<ELEMENT_TYPE>& b) const;
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
                                       const shared_ptr<ELEMENT_TYPE>& b) const
{
    return a.owner_before(b);
}

template <class ELEMENT_TYPE>
inline
bool owner_less<shared_ptr<ELEMENT_TYPE> >::operator()(
                                       const shared_ptr<ELEMENT_TYPE>& a,
                                       const weak_ptr<ELEMENT_TYPE>&   b) const
{
    return a.owner_before(b);
}

template <class ELEMENT_TYPE>
inline
bool owner_less<shared_ptr<ELEMENT_TYPE> >::operator()(
                                       const weak_ptr<ELEMENT_TYPE>&   a,
                                       const shared_ptr<ELEMENT_TYPE>& b) const
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
                                        const weak_ptr<ELEMENT_TYPE>& b) const
{
    return a.owner_before(b);
}

template <class ELEMENT_TYPE>
inline
bool owner_less<weak_ptr<ELEMENT_TYPE> >::operator()(
                                       const shared_ptr<ELEMENT_TYPE>& a,
                                       const weak_ptr<ELEMENT_TYPE>&   b) const
{
    return a.owner_before(b);
}

template <class ELEMENT_TYPE>
inline
bool owner_less<weak_ptr<ELEMENT_TYPE> >::operator()(
                                       const weak_ptr<ELEMENT_TYPE>&   a,
                                       const shared_ptr<ELEMENT_TYPE>& b) const
{
    return a.owner_before(b);
}


// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for 'equal_to'
//: o 'equal_to' is a stateless POD, trivially constructible, copyable, and
//:   moveable.

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
