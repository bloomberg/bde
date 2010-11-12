// bdeimp_voidptrhash.h                -*-C++-*-
#ifndef INCLUDED_BDEIMP_VOIDPTRHASH
#define INCLUDED_BDEIMP_VOIDPTRHASH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide support implementation for 'void *' hashing.
//
//@DEPRECATED: Use 'bdeu_hashutil' instead.
//
//@CLASSES:
//   bdeimp_VoidPtrHash: namespace for 'void *' hashing function
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@DESCRIPTION: This component provides a 'void *' hashing function that can be
// used to implement a hash table.  In particular, 'bdeimp_VoidPtrHash'
// provides a pure procedure (a static method of a 'struct') for mapping a
// 'void *' to an 'int' suitable for use in a hash table of 'void *' values.
//
///Usage
///-----
// This component is intended to be an implementation utility for a hash table
// class.  For example:
//..
//  bdeci_Hashtable<void *, bdeimp_VoidPtrHash> hashTable;
//..
// See the component-level documentation of 'bdeci_hashtable' for further
// information.  However, note that 'bsl::hash_map' generally should be used
// instead of 'bdeci_Hashtable'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEIMP_INTHASH
#include <bdeimp_inthash.h>
#endif

#ifndef INCLUDED_BDEIMP_INT64HASH
#include <bdeimp_int64hash.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

namespace BloombergLP {

                         // =========================
                         // struct bdeimp_VoidPtrHash
                         // =========================

struct bdeimp_VoidPtrHash {
    // This 'struct' provides a namespace for a function used to implement a
    // hash table of 'void *' values.

    // CLASS METHODS
    static int hash(const void *value, int modulus);
        // Return an integer in the range from zero to one less than the
        // specified 'modulus' corresponding to the specified 'value'.
        // The behavior is undefined unless '0 < modulus < 2^31'.  Note that
        // 'modulus' is expected to be a prime not close to an integral power
        // of 2.  Also note that specifying a 'modulus' of 1 will cause 0 to be
        // returned for every 'value'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // -------------------------
                         // struct bdeimp_VoidPtrHash
                         // -------------------------

// CLASS METHODS
inline
int bdeimp_VoidPtrHash::hash(const void *value, int modulus)
{
    if (4 == sizeof(void *)) {
        const int *v = (const int *)&value;
        return bdeimp_IntHash::hash(*v, modulus);
    }
    else {
        const bsls_PlatformUtil::Int64 *v =
                              (const bsls_PlatformUtil::Int64 *)(void *)&value;
        return bdeimp_Int64Hash::hash(*v, modulus);
    }
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
