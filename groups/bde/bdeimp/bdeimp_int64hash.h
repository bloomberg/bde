// bdeimp_int64hash.h              -*-C++-*-
#ifndef INCLUDED_BDEIMP_INT64HASH
#define INCLUDED_BDEIMP_INT64HASH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide support implementation for 64-bit integer hashing.
//
//@DEPRECATED: Use 'bdeu_hashutil' instead.
//
//@CLASSES:
//   bdeimp_Int64Hash: namespace for 64-bit integer hashing function
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@DESCRIPTION: This component provides an integer hashing function that can be
// used to implement a hash table.  In particular, 'bdeimp_Int64Hash' provides
// a pure procedure (a static method of a 'struct') for mapping a 64-bit
// integer to an 'int' suitable for use in a hash table of 64-bit integer
// values.
//
///Usage
///-----
// This component is intended to be an implementation utility for a hash table
// class.  For example:
//..
//  bdeci_Hashtable<bsls_PlatformUtil::Int64, bdeimp_Int64Hash> hashTable;
//..
// See the component-level documentation of 'bdeci_hashtable' for further
// information.  However, note that 'bsl::hash_map' generally should be used
// instead of 'bdeci_Hashtable'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif


namespace BloombergLP {

                          // =======================
                          // struct bdeimp_Int64Hash
                          // =======================

struct bdeimp_Int64Hash {
    // This 'struct' provides a namespace for a function used to implement a
    // hash table of 64-bit integer values.

    // CLASS METHODS
    static int hash(bsls_PlatformUtil::Int64 value, int modulus);
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

                          // -----------------------
                          // struct bdeimp_Int64Hash
                          // -----------------------

// CLASS METHODS
inline
int bdeimp_Int64Hash::hash(bsls_PlatformUtil::Int64 value, int modulus)
{
    return (int)((((unsigned int)((value >> 32) & 0xffffffff) %
                   (unsigned int)modulus) ^
                  (unsigned int)(value & 0xffffffff)) % (unsigned int)modulus);
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
