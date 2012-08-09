// bdeu_cstringequalto.h                                              -*-C++-*-
#ifndef INCLUDED_BDEU_CSTRINGEQUALTO
#define INCLUDED_BDEU_CSTRINGEQUALTO

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a functor enabling C strings as associative container keys.
//
//@CLASSES:
//  bdeu_CStringEqualTo: functor enabling C strings as associative container keys
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@SEE_ALSO: bslstp_hashmap, bslstp_hashset
//
//@DESCRIPTION: This component provides a functor to compare two
// null-terminated strings using a case-sensitive string comparison, rather
// than simply comparing the two addresses, as the 'std::less' functor would
// do.  This lexicographical ordering makes 'bdeu_CStringEqualTo' suitable for
// supporting C strings as keys in associative containers.  Note that the
// container behavior would be undefined if the strings referenced by such
// pointers were to change value.
//
///Usage
///-----

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

namespace BloombergLP {

                       // ==========================
                       // struct bdeu_CStringEqualTo
                       // ==========================

struct bdeu_CStringEqualTo {
    // This 'struct' defines an ordering on null-terminated character strings,
    // enabling them for use as keys in the standard associative containers
    // such as 'bsl::map' and 'bsl::set'.  Note that this class is an empty
    // POD type.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdeu_CStringEqualTo,
                                 bslalg::TypeTraitsGroupPod);

    // STANDARD TYPEDEFS
    typedef const char *first_argument_type;
    typedef const char *second_argument_type;
    typedef bool result_type;

    //! bdeu_CStringEqualTo() = default;
        // Create a 'bdeu_CStringEqualTo' object.

    //! bdeu_CStringEqualTo(const bdeu_CStringEqualTo& original) = default;
        // Create a 'bdeu_CStringEqualTo' object.  Note that as
        // 'bdeu_CStringEqualTo' is an empty (stateless) type, this operation
        // will have no observable effect.

    //! ~bdeu_CStringEqualTo() = default;
        // Destroy this object.

    // MANIPULATORS
    //! bdeu_CStringEqualTo& operator=(const bdeu_CStringEqualTo& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // a return a reference providing modifiable access to this object.
        // Note that as 'bdeu_CStringEqualTo' is an empty (stateless) type, this
        // operation will have no observable effect.

    // ACCESSORS
    bool operator()(const char *lhs, const char *rhs) const;
        // Return 'true' if the specified 'lhs' string has the same (case
        // sensitive) value as the specified 'rhs' string, and 'false'
        // otherwise.  The behavior is undefined unless both 'lhs' and 'rhs'
        // point to null-terminated strings.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // --------------------------
                       // struct bdeu_CStringEqualTo
                       // --------------------------

// ACCESSORS
inline
bool bdeu_CStringEqualTo::operator()(const char *lhs, const char *rhs) const
{
    BSLS_ASSERT_SAFE(lhs);
    BSLS_ASSERT_SAFE(rhs);

    return 0 == bsl::strcmp(lhs, rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
