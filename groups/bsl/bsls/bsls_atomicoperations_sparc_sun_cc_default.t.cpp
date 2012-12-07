// bsls_atomicoperations_sparc_sun_cc_default.t.cpp                   -*-C++-*-

#include <bsls_atomicoperations_sparc_sun_cc_default.h>

#include <bsls_types.h>

#include <cstdlib>

using namespace BloombergLP;
using namespace std;

#if (defined(BSLS_PLATFORM_CPU_SPARC_V9)                                     \
                                     || defined(BSLS_PLATFORM_CPU_SPARC_32)) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))

typedef bsls::Types::UintPtr UintPtr;

UintPtr getInt = (UintPtr) &bsls_AtomicOperations_Sparc_GetInt;
UintPtr setInt = (UintPtr) &bsls_AtomicOperations_Sparc_SetInt;
UintPtr SwapInt = (UintPtr) &bsls_AtomicOperations_Sparc_SwapInt;
UintPtr SwapIntAcqRel = (UintPtr) &bsls_AtomicOperations_Sparc_SwapIntAcqRel;
UintPtr TestAndSwapInt = (UintPtr) &bsls_AtomicOperations_Sparc_TestAndSwapInt;
UintPtr TestAndSwapIntAcqRel =
                   (UintPtr) &bsls_AtomicOperations_Sparc_TestAndSwapIntAcqRel;
UintPtr AddInt = (UintPtr) &bsls_AtomicOperations_Sparc_AddInt;

#endif

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    switch (test) { case 0:
        return 0;                                                     // RETURN
      default:
        return -1;                                                    // RETURN
    }
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
