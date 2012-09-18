// bslstl_equalto.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_EQUALTO
#define INCLUDED_BSLSTL_EQUALTO

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a functor enabling C strings as unordered containers keys.
//
//@CLASSES:
//  equal_to: functor enabling C strings as 'unordered_map' keys
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@SEE_ALSO: bslstp_hashmap, bslstp_hashset
//
//@DESCRIPTION: This component provides a functor to compare two
// null-terminated strings using a case-sensitive string comparison, rather
// than simply comparing the two addresses (as the 'std::equal_to' functor
// would do).  This comparison functor is suitable for supporting C strings as
// keys in unordered associative containers.  Note that the container behavior
// would be undefined if the strings referenced by such pointers were to
// change value.
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

namespace bsl
{

                       // ===============
                       // struct equal_to
                       // ===============

template<class VALUE_TYPE>
struct equal_to {
    // This 'struct' defines a comparison functor...
    // ... for use as keys in the standard unordered associative containers
    // such as 'bsl::unordered_map' and 'bsl::unordered_set'.
    // Note that this class is an empty POD type.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(equal_to,
                                 ::BloombergLP::bslalg::TypeTraitsGroupPod);

    // STANDARD TYPEDEFS
    typedef VALUE_TYPE first_argument_type;
    typedef VALUE_TYPE second_argument_type;
    typedef bool       result_type;

    //! equal_to() = default;
        // Create a 'equal_to' object.

    //! equal_to(const equal_to& original) = default;
        // Create a 'equal_to' object.  Note that as
        // 'equal_to' is an empty (stateless) type, this operation
        // will have no observable effect.

    //! ~equal_to() = default;
        // Destroy this object.

    // MANIPULATORS
    //! equal_to& operator=(const equal_to&) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // a return a reference providing modifiable access to this object.
        // Note that as 'equal_to' is an empty (stateless) type,
        // this operation will have no observable effect.

    // ACCESSORS
    bool operator()(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs) const;
        // Return 'true' if the specified 'lhs' compares equal to the specified
        // 'rhs' using the equality-comaprision operator, 'lhs == rhs'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // --------------------------
                       // struct equal_to
                       // --------------------------

// ACCESSORS
template<class VALUE_TYPE>
inline
bool equal_to<VALUE_TYPE>::operator()(const VALUE_TYPE& lhs,
                                      const VALUE_TYPE& rhs) const
{
    return lhs == rhs;
}

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
