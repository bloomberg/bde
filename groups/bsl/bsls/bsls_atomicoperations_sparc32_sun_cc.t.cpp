// bsls_atomicoperations_sparc32_sun_cc.t.cpp                         -*-C++-*-

#include <bsls_atomicoperations_sparc32_sun_cc.h>

#include <cstdlib>

using namespace BloombergLP;
using namespace std;

// Make sure routines are at least defined somewhere.

#if defined(BSLS_PLATFORM_CPU_SPARC_32) \
    && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN))

typedef bsls::Types::Int64 (*GetInt64Ptr)(
                                 const volatile bsls::Types::Int64 *atomicInt);
GetInt64Ptr getInt64        = &bsls_AtomicOperations_Sparc32_GetInt64;
GetInt64Ptr getInt64Relaxed = &bsls_AtomicOperations_Sparc32_GetInt64Relaxed;

typedef void (*SetInt64Ptr)(
             volatile bsls::Types::Int64 *atomicInt, bsls::Types::Int64 value);
SetInt64Ptr setInt64        = &bsls_AtomicOperations_Sparc32_SetInt64;
SetInt64Ptr setInt64Relaxed = bsls_AtomicOperations_Sparc32_SetInt64Relaxed;

typedef bsls::Types::Int64 (*SwapInt64Ptr)(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 swapValue);

SwapInt64Ptr swapInt64       = &bsls_AtomicOperations_Sparc32_SwapInt64;
SwapInt64Ptr swapInt64AcqRel = &bsls_AtomicOperations_Sparc32_SwapInt64AcqRel;

typedef bsls::Types::Int64 (*TestAndSwapInt64)(
            volatile bsls::Types::Int64 *atomicInt,
            bsls::Types::Int64 compareValue,
            bsls::Types::Int64 swapValue);

TestAndSwapInt64 testAndSwapInt64 =
                         &bsls_AtomicOperations_Sparc32_TestAndSwapInt64;
TestAndSwapInt64 testAndSwapInt64AcqRel =
                         &bsls_AtomicOperations_Sparc32_TestAndSwapInt64AcqRel;

typedef bsls::Types::Int64 (*AddInt64)(
            volatile bsls::Types::Int64 *atomicInt, bsls::Types::Int64 value);

AddInt64 addInt64        = &bsls_AtomicOperations_Sparc32_AddInt64;
AddInt64 addInt64Relaxed = &bsls_AtomicOperations_Sparc32_AddInt64Relaxed;

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
