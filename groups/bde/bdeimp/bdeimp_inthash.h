// bdeimp_inthash.h                -*-C++-*-
#ifndef INCLUDED_BDEIMP_INTHASH
#define INCLUDED_BDEIMP_INTHASH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide support implementation for 'int' hashing.
//
//@DEPRECATED: Use 'bdeu_hashutil' instead.
//
//@CLASSES:
//   bdeimp_IntHash: namespace for 'int' hashing function
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@DESCRIPTION: This component provides an integer hashing function that can be
// used to implement a hash table.  In particular, 'bdeimp_IntHash' provides a
// pure procedure (a static method of a 'struct') for mapping an 'int' to an
// 'int' suitable for use in a hash table of 'int' values.
//
///Usage
///-----
// This component is intended to be an implementation utility for a hash table
// class.  For example:
//..
//  bdeci_Hashtable<int, bdeimp_IntHash> hashTable;
//..
// See the component-level documentation of 'bdeci_hashtable' for further
// information.  However, note that 'bsl::hash_map' generally should be used
// instead of 'bdeci_Hashtable'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

                           // =====================
                           // struct bdeimp_IntHash
                           // =====================

struct bdeimp_IntHash {
    // This 'struct' provides a namespace for a function used to implement a
    // hash table of 'int' values.

    // CLASS METHODS
    static int hash(int value, int modulus);
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

                           // ---------------------
                           // struct bdeimp_IntHash
                           // ---------------------

// CLASS METHODS
inline
int bdeimp_IntHash::hash(int value, int modulus)
{
    if (4 == sizeof(int)) {
        return (int)((unsigned int)value % (unsigned int)modulus);
    }
    else {
        return (int)(((unsigned int)value & 0xffffffff)
                    % (unsigned int)modulus);
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
