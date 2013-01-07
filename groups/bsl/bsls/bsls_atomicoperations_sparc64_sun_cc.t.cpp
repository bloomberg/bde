// bsls_atomicoperations_sparc64_sun_cc.t.cpp                         -*-C++-*-

#include <bsls_atomicoperations_sparc64_sun_cc.h>

#include <cstdlib>

using namespace BloombergLP;
using namespace std;

#if defined(BSLS_PLATFORM_CPU_SPARC_V9) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))

typedef bsls::Types::Int64 (*GetInt64)(
                                 const volatile bsls::Types::Int64 *atomicInt);

GetInt64 getInt64 = &bsls_AtomicOperations_Sparc64_GetInt64;

typedef void (*SetInt64)(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 value);

SetInt64 setInt64 = &bsls_AtomicOperations_Sparc64_SetInt64;

typedef bsls::Types::Int64 (*SwapInt64)(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 swapValue);

SwapInt64 swapInt64       = &bsls_AtomicOperations_Sparc64_SwapInt64;
SwapInt64 swapInt64AcqRel = &bsls_AtomicOperations_Sparc64_SwapInt64AcqRel;

typedef bsls::Types::Int64 (*TestAndSwapInt64)(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 compareValue,
            bsls::Types::Int64 swapValue);

TestAndSwapInt64 testAndSwapInt64 =
                         &bsls_AtomicOperations_Sparc64_TestAndSwapInt64;
TestAndSwapInt64 testAndSwapInt64AcqRel =
                         &bsls_AtomicOperations_Sparc64_TestAndSwapInt64AcqRel;

typedef bsls::Types::Int64 (*AddInt64)(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 value);

AddInt64 addInt64        = &bsls_AtomicOperations_Sparc64_AddInt64;
AddInt64 addInt64Relaxed = &bsls_AtomicOperations_Sparc64_AddInt64Relaxed;

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
