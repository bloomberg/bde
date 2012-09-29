// bslalg_hashutil.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHUTIL
#define INCLUDED_BSLALG_HASHUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility of hash functions.
//
//@CLASSES:
//  bslalg::HashUtil: utility for hash functions
//
//@AUTHOR: Vladimir Kliatchko (vkliatch), Herve Bronnimann (hbronnimann)
//
//@DESCRIPTION: This component provides a namespace class, 'HashUtil', for
// hash functions.  At the current time it has one hash function,
// 'HashUtil::computeHash', which will hash most fundamental types, and
// pointers, rapidly.  Note that when a pointer is passed, only the bits in the
// pointer itself are hashed, the memory the pointer refers to is not examined.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

namespace bslalg {
                        // ===============
                        // struct HashUtil
                        // ===============

struct HashUtil {
    // This 'struct' provides a namespace for hash functions.

    // CLASS METHODS
    static native_std::size_t computeHash(char key);
    static native_std::size_t computeHash(signed char key);
    static native_std::size_t computeHash(unsigned char key);
    static native_std::size_t computeHash(short key);
    static native_std::size_t computeHash(unsigned short key);
    static native_std::size_t computeHash(int key);
    static native_std::size_t computeHash(unsigned int key);
    static native_std::size_t computeHash(long key);
    static native_std::size_t computeHash(unsigned long key);
    static native_std::size_t computeHash(long long key);
    static native_std::size_t computeHash(unsigned long long key);
    static native_std::size_t computeHash(float key);
    static native_std::size_t computeHash(double key);
    static native_std::size_t computeHash(const void *key);
        // Return a 'size_t' hash value corresponding to the specified 'key'.
        // Note that the return value is seemingly random (i.e., the hash is
        // good) but identical on all platforms (irrespective of endianness).
        //
        // NOTE: We reserve the right to change these hash functions to return
        // different values.  The current implementation only returns a 32 bit
        // value -- when 'native_std::size_t' is 64 bits, the high-order 32
        // bits of the return value are all zero.  This is not a feature, it is
        // a bug that we will fix in a later release.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

}  // close namespace BloombergLP::bslalg
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
