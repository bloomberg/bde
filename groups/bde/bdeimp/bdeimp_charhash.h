// bdeimp_charhash.h                -*-C++-*-
#ifndef INCLUDED_BDEIMP_CHARHASH
#define INCLUDED_BDEIMP_CHARHASH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide support implementation for 'char' hashing.
//
//@DEPRECATED: Use 'bdeu_hashutil' instead.
//
//@CLASSES:
//  bdeimp_CharHash: namespace for 'char' hashing function
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@DESCRIPTION: This component provides a 'char' hashing function that can be
// used to implement a hash table.  In particular, 'bdeimp_CharHash' provides a
// pure procedure (a static method of a 'struct') for mapping a 'char' value to
// an 'int' suitable for use in a hash table of 'char' values.
//
///Usage
///-----
// This component is intended to be an implementation utility for a hash table
// class.  For example:
//..
//  bdeci_Hashtable<char, bdeimp_CharHash> hashTable;
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

namespace BloombergLP {

                           // ======================
                           // struct bdeimp_CharHash
                           // ======================

struct bdeimp_CharHash {
    // This 'struct' provides a namespace for a function used to implement a
    // hash table of 'char' values.

    // CLASS METHODS
    static int hash(char value, int modulus);
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

                           // ----------------------
                           // struct bdeimp_CharHash
                           // ----------------------

// CLASS METHODS
inline
int bdeimp_CharHash::hash(char value, int modulus)
{
    return bdeimp_IntHash::hash((int)(unsigned char)value, modulus);
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
