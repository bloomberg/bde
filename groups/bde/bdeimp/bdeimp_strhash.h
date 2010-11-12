// bdeimp_strhash.h                -*-C++-*-
#ifndef INCLUDED_BDEIMP_STRHASH
#define INCLUDED_BDEIMP_STRHASH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide implementation support for 'char *' hashing.
//
//@DEPRECATED: Use 'bdeu_hashutil' instead.
//
//@CLASSES:
//   bdeimp_StrHash: namespace for 'char *' hashing functions
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@DESCRIPTION: This component provides string hashing functions that can be
// used to implement a hash table.  In particular, 'bdeimp_StrHash' provides
// two pure procedures (i.e., static methods of a 'struct') each named 'hash',
// for mapping a 'char *' to an 'int' suitable for use in a hash table of
// 'char * ' values.  One 'hash' method assumes that the string being hashed is
// null-terminated; the other accommodates strings that may not be
// null-terminated or contain embedded null characters.
//
///Usage
///-----
// This component is intended to be an implementation utility for a hash table
// class.  For example:
//..
//  bdeci_Hashtable<char *, bdeimp_StrHash> hashTable;
//..
// See the component-level documentation of 'bdeci_hashtable' for further
// information.  However, note that 'bsl::hash_map' generally should be used
// instead of 'bdeci_Hashtable'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

                           // =====================
                           // struct bdeimp_StrHash
                           // =====================

struct bdeimp_StrHash {
    // This 'struct' provides a namespace for functions used to implement a
    // hash table of 'char *' values.

    // CLASS METHODS
    static int hash(const char *string, int modulus);
        // Return a pseudo-random integer in the range from zero to one less
        // than the specified 'modulus' corresponding to the specified
        // null-terminated 'string'.  The behavior is undefined unless
        // '0 < modulus < 2^31'.  Note that 'modulus' is expected to be a prime
        // not close to an integral power of 2.  Also note that specifying a
        // 'modulus' of 1 will cause 0 to be returned for every 'string'.

    static int hash(const char *string, int stringLength, int modulus);
        // Return a pseudo-random integer in the range from zero to one less
        // than the specified 'modulus' corresponding to the specified 'string'
        // having the specified 'stringLength'.   'string' need not be
        // null-terminated and may contain embedded null characters.  The
        // behavior is undefined unless '0 <= stringLength' and
        // '0 < modulus < 2^31'.  Note that 'modulus' is expected to be a prime
        // not close to an integral power of 2.  Also note that specifying a
        // 'modulus' of 1 will cause 0 to be returned for every 'string'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

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
