// bdeimp_strhash.cpp                                                 -*-C++-*-
#include <bdeimp_strhash.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeimp_strhash_cpp,"$Id$ $CSID$")

struct bdeimp_StrHash_Assertions {
    char assertion[4 >= sizeof(int)];
};

///IMPLEMENTATION NOTES
///--------------------
// The 'bdeimp_StrHash::hash' methods are based upon a simple random number
// generator for 32-bit integer machines.  The methods dictate multiplying by
// 1664525 and adding 1013904223.  The random number generator implied in the
// methods assumes a 32-bit integer, so intermediate results on 64-bit integer
// machines are AND-ed with '2^32 - 1'.

// CONSTANTS
const unsigned int ADDEND       = 1013904223;
const unsigned int MULTIPLICAND =    1664525;
const unsigned int MASK         = 4294967295U;

namespace BloombergLP {

                           // ---------------------
                           // struct bdeimp_StrHash
                           // ---------------------

// CLASS METHODS
int bdeimp_StrHash::hash(const char *string, int modulus)
{
    unsigned int r = 0;

    if (4 == sizeof(int)) {
        while (*string) {
            r ^= *string++;
            r = r * MULTIPLICAND + ADDEND;
        }
    }
    else {
        while (*string) {
            r ^= *string++;
            r = (r * MULTIPLICAND + ADDEND) & MASK;
        }
    }

    return (int)(r % (unsigned int)modulus);
}

int bdeimp_StrHash::hash(const char *string, int stringLength, int modulus)
{
    const char *end = string + stringLength;
    unsigned int r  = 0;

    if (4 == sizeof(int)) {
        while (string != end) {
            r ^= *string++;
            r = r * MULTIPLICAND + ADDEND;
        }
    }
    else {
        while (string != end) {
            r ^= *string++;
            r = (r * MULTIPLICAND + ADDEND) & MASK;
        }
    }

    return (int)(r % (unsigned int)modulus);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
