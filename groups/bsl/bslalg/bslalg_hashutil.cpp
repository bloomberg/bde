// bslalg_hashutil.cpp                                                -*-C++-*-
#include <bslalg_hashutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslalg_hashutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_platform.h>

// IMPLEMENTATION NOTES: See http://burtleburtle.net/bob/hash/evahash.html
// In particular this hash function has the NoFunnel property, defined in that
// reference as follows:
//..
//  Define h to be a funneling hash if there is some subset t of the input bits
//  which can only affect bits u in the internal state, and |t| > |u| and v >
//  |u|.  h has a funnel of those t input bits into those u bits of the
//  internal state.  If a hash has a funnel of t bits into u, then u of those t
//  bits can cancel out the effects of the other |t|-|u|.  The set of keys
//  differing only in the input bits of the funnel can produce no more than
//  half that number of hash values.  (Those 2^|t| keys can produce no more
//  than 2^|u| out of 2^v hash values.)
//..
// Differing in only a few bits is a common pattern in human and computer keys,
// so a funneling hash is seriously flawed.  In that reference, it is claimed
// that these hashes have no funnels, more specifically:
//..
// There is a funnel of 32 bits to 31 bits, with those 32 bits distributed
// across two blocks.  I backed up my computer, wrote a program that found
// this, then changed computers.  So I don't have the code and don't remember
// where the funnel was.  A funnel of 32 bits to 31 is awfully non-serious,
// though, so I let things be.
//..

namespace BloombergLP
{

namespace bslalg
{
// STATIC HELPER FUNCTIONS
#ifdef BSLS_PLATFORM__IS_BIG_ENDIAN
static
unsigned int hash(const char *data, int length)
{
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(data || 0 == length);

    typedef unsigned char Ub1;
    typedef unsigned int  Ub4;

    const Ub1 *k    = reinterpret_cast<const Ub1 *>(data);
    Ub4        hash = 0;

    for (int i = 0; i < length; ++i) {
        hash += k[i];
        hash += (hash << 10);
        hash ^= (hash >>  6);
    }

    hash += (hash <<  3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
#else
static
unsigned int reverse_hash(const char *data, int length)
{
    BSLS_ASSERT(0 <= length);
    BSLS_ASSERT(data || 0 == length);

    typedef unsigned char Ub1;
    typedef unsigned int  Ub4;

    const Ub1 *k    = reinterpret_cast<const Ub1 *>(data);
    Ub4        hash = 0;

    for (int i = length; i > 0; /* noop */) {
        hash += k[--i];
        hash += (hash << 10);
        hash ^= (hash >>  6);
    }

    hash += (hash <<  3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
#endif

                            // ---------------
                            // struct HashUtil
                            // ---------------

// CLASS METHODS
#ifdef BSLS_PLATFORM__IS_BIG_ENDIAN
  #define HASH2(KEY)  hash((const char *)&KEY, sizeof KEY)
#else
  #define HASH2(KEY)  reverse_hash((const char *)&KEY, sizeof KEY)
#endif

unsigned int HashUtil::computeHash(char key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(signed char key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(unsigned char key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(short key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(unsigned short key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(int key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(unsigned int key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(long key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(unsigned long key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(long long key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(unsigned long long key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(float key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(double key)
{
    return HASH2(key);
}
unsigned int HashUtil::computeHash(const void *key)
{
    return HASH2(key);
}

#undef HASH2

}  // close namespace BloombergLP::bslalg
}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
