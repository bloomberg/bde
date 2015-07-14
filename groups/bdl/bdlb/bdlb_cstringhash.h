// bdlb_cstringhash.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLB_CSTRINGHASH
#define INCLUDED_BDLB_CSTRINGHASH

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a functor enabling C strings as unordered container keys.
//
//@CLASSES:
//  bdlb::CStringHash: functor enabling C strings as unordered container keys
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@SEE_ALSO:
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_HASHUTIL
#include <bdlb_hashutil.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#include <bslmf_istriviallydefaultconstructible.h>
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

namespace bdlb {
                        // =======================
                        // struct CStringHash
                        // =======================

struct CStringHash {
    // This 'struct' defines a hash operation for null-terminated character
    // strings enabling them to be used as keys in the standard unordered
    // associative containers such as 'bsl::unordered_map' and
    // 'bsl::unordered_set'.  Note that this class is an empty POD type.

    // STANDARD TYPEDEFS
    typedef const char  *argument_type;
    typedef bsl::size_t  result_type;

    //! CStringHash() = default;
        // Create a 'CStringHash' object.

    //! CStringHash(const CStringHash& original) = default;
        // Create a 'CStringHash' object.  Note that as 'CStringHash'
        // is an empty (stateless) type, this operation will have no observable
        // effect.

    //! ~CStringHash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! CStringHash& operator=(const CStringHash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // a return a reference providing modifiable access to this object.
        // Note that as 'CStringHash' is an empty (stateless) type, this
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
                        // struct CStringHash
                        // -----------------------

// ACCESSORS
inline
bsl::size_t CStringHash::operator()(const char *argument) const
{
    BSLS_ASSERT_SAFE(argument);

    return HashUtil::hash1(argument, bsl::strlen(argument));
}
}  // close package namespace

}  // close namespace BloombergLP

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// POD TRAITS
namespace bsl {

template <>
struct is_trivially_copyable<BloombergLP::bdlb::CStringHash> :
        bsl::true_type { };

template <>
struct is_trivially_default_constructible<BloombergLP::bdlb::CStringHash> :
        bsl::true_type { };

} // Close namespace bsl


#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
