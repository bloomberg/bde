// bdeu_cstringhash.h                                                 -*-C++-*-
#ifndef INCLUDED_BDEU_CSTRINGHASH
#define INCLUDED_BDEU_CSTRINGHASH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a functor enabling C strings as unordered container keys.
//
//@CLASSES:
//  bdeu_CStringHash: functor enabling C strings as unordered container keys
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@SEE_ALSO: bslstp_hashmap, bslstp_hashset
//
//@DESCRIPTION: This component provides a functor to generate a hash code for a
// null-terminated string, rather than simply generating a hash code for the
// address of the string, as the 'std::hash' functor would do.  This hash
// functor is suitable for supporting C strings as keys in unordered
// associative containers.  Note that the container behavior would be
// undefined if the strings referenced by such pointers were to change value.
//
///Usage
///-----

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEU_HASHUTIL
#include <bdeu_hashutil.h>
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

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {

                        // =======================
                        // struct bdeu_CStringHash
                        // =======================

struct bdeu_CStringHash {
    // This 'struct' defines a hash operation for null-terminated character
    // strings enabling them to be used as keys in the standard unordered
    // associative containers such as 'bsl::hash_map' and 'bsl::hash_set'.
    // Note that this class is an empty POD type.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdeu_CStringHash, bslalg::TypeTraitsGroupPod);

    // STANDARD TYPEDEFS
    typedef const char *argument_type;
    typedef bool result_type;

    //! bdeu_CStringHash() = default;
        // Create a 'bdeu_CStringHash' object.

    //! bdeu_CStringHash(const bdeu_CStringHash& original) = default;
        // Create a 'bdeu_CStringHash' object.  Note that as 'bdeu_CStringHash'
        // is an empty (stateless) type, this operation will have no observable
        // effect.

    //! ~bdeu_CStringHash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! bdeu_CStringHash& operator=(const bdeu_CStringHash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // a return a reference providing modifiable access to this object.
        // Note that as 'bdeu_CStringHash' is an empty (stateless) type, this
        // operation will have no observable effect.

    // ACCESSORS
    bsl::size_t operator()(const char *argument) const;
        // Return a hash code generated from the contents of the specified
        // null-terminated 'argument' string.  The behavior is undefined
        // unless both 'argument' is a null-terminated strings.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================


                        // -----------------------
                        // struct bdeu_CStringHash
                        // -----------------------

// ACCESSORS
inline
bsl::size_t bdeu_CStringHash::operator()(const char *argument) const
{
    BSLS_ASSERT_SAFE(argument);

    return bdeu_HashUtil::hash1(argument, bsl::strlen(argument));
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
