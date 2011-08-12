// bsls_atomicoperations_powerpc64_aix_xlc.cpp                        -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_types.h>
#include <bsls_atomicoperations_powerpc64_aix_xlc.h>

#if defined(BSLS_PLATFORM__CPU_POWERPC) && defined(BSLS_PLATFORM__CMP_IBM) \
    && defined(BSLS_PLATFORM__CPU_64_BIT)

namespace BloombergLP {

// For reference on atomic operations on the PowerPC platform see:
// http://www.rdrop.com/users/paulmck/scalability/paper/N2745r.2011.03.04a.html
// http://www.rdrop.com/users/paulmck/scalability/paper/
//                                                    N2745rP5.2010.02.19a.html

bsls_Types::Int64 bsls_AtomicOperations_Powerpc64_GetInt64(
                                  const volatile bsls_Types::Int64 *atomicInt)
{
    bsls_Types::Int64 result;

    asm volatile (
        "       sync                                \n\t"

        "1:     ldarx %[res], %[zero], %[obj]       \n\t"
        "       cmpd %[res], %[res]                 \n\t" // create data
                                                          // dependency for
                                                          // load/load ordering
        "       bne- 1b                             \n\t" // never taken
        "       isync                               \n\t"
                : [res]  "=b" (result)
                : [obj]  "b"  (atomicInt),
                  [zero] "i"  (0)
                : "cr0", "ctr");

    return result;
}

bsls_Types::Int64 bsls_AtomicOperations_Powerpc64_GetInt64Acquire(
                                  const volatile bsls_Types::Int64 *atomicInt)
{
    bsls_Types::Int64 result;

    asm volatile (
        "1:     ld %[res], %[obj]                   \n\t"
        "       cmpd %[res], %[res]                 \n\t" // create data
                                                          // dependency for
                                                          // load/load ordering
        "       bne- 1b                             \n\t" // never taken
        "       isync                               \n\t"
                : [res]  "=b" (result)
                : [obj]  "m"  (*atomicInt),
                  [zero] "i"  (0)
                : "cr0", "ctr");

    return result;
}

void bsls_AtomicOperations_Powerpc64_SetInt64(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value)
{
    asm volatile (
        "       sync                        \n\t"
        "       std %[val], %[obj]          \n\t"
                : [obj] "=m" (*atomicInt)
                : [val] "b"  (value));
}

void bsls_AtomicOperations_Powerpc64_SetInt64Release(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value)
{
    asm volatile (
        "       lwsync                      \n\t"
        "       std %[val], %[obj]          \n\t"
                : [obj] "=m" (*atomicInt)
                : [val] "b"  (value));
}

bsls_Types::Int64 bsls_AtomicOperations_Powerpc64_SwapInt64(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value)
{
    bsls_Types::Int64 result;

    asm volatile (
        "       sync                                \n\t"

        "1:     ldarx %[res], %[zero], %[obj]       \n\t"
        "       stdcx. %[val], %[zero], %[obj]      \n\t"
        "       bne- 1b                             \n\t"

        "       isync                               \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (value),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

bsls_Types::Int64 bsls_AtomicOperations_Powerpc64_SwapInt64AcqRel(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value)
{
    bsls_Types::Int64 result;

    asm volatile (
        "       lwsync                              \n\t"

        "1:     ldarx %[res], %[zero], %[obj]       \n\t"
        "       stdcx. %[val], %[zero], %[obj]      \n\t"
        "       bne- 1b                             \n\t"

        "       isync                               \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (value),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

bsls_Types::Int64 bsls_AtomicOperations_Powerpc64_testAndSwapInt64(
                                    volatile bsls_Types::Int64 *atomicInt,
                                    bsls_Types::Int64           compareValue,
                                    bsls_Types::Int64           swapValue)
{
    bsls_Types::Int64 result;

    asm volatile (
        "       sync                            \n\t"

        "1:     ldarx %[res], %[zero], %[obj]   \n\t"  // load and reserve
        "       cmpd %[cmp], %[res]             \n\t"  // compare values
        "       bne- 2f                         \n\t"

        "       stdcx. %[val], %[zero], %[obj]  \n\t"  // store the new value
        "       bne- 1b                         \n\t"
        "       isync                           \n\t"
        "2:                                     \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [cmp]  "b"   (compareValue),
                  [val]  "b"   (swapValue),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

bsls_Types::Int64 bsls_AtomicOperations_Powerpc64_testAndSwapInt64AcqRel(
                                    volatile bsls_Types::Int64 *atomicInt,
                                    bsls_Types::Int64           compareValue,
                                    bsls_Types::Int64           swapValue)
{
    bsls_Types::Int64 result;

    asm volatile (
        "       lwsync                          \n\t"

        "1:     ldarx %[res], %[zero], %[obj]   \n\t"  // load and reserve
        "       cmpd %[cmp], %[res]             \n\t"  // compare values
        "       bne- 2f                         \n\t"

        "       stdcx. %[val], %[zero], %[obj]  \n\t"  // store the new value
        "       bne- 1b                         \n\t"
        "       isync                           \n\t"
        "2:                                     \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [cmp]  "b"   (compareValue),
                  [val]  "b"   (swapValue),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

bsls_Types::Int64 bsls_AtomicOperations_Powerpc64_AddInt64(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value)
{
    bsls_Types::Int64 result;

    asm volatile (
        "       sync                            \n\t"

        "1:     ldarx %[res], %[zero], %[obj]   \n\t" // load and reserve
        "       add %[res], %[res], %[val]      \n\t" // add the operand
        "       stdcx. %[res], %[zero], %[obj]  \n\t" // store old value if
                                                      // still reserved

        "       bne- 1b                         \n\t"

        "       isync                           \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (value),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

bsls_Types::Int64 bsls_AtomicOperations_Powerpc64_AddInt64AcqRel(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value)
{
    bsls_Types::Int64 result;

    asm volatile (
        "       lwsync                          \n\t"

        "1:     ldarx %[res], %[zero], %[obj]   \n\t" // load and reserve
        "       add %[res], %[res], %[val]      \n\t" // add the operand
        "       stdcx. %[res], %[zero], %[obj]  \n\t" // store old value if
                                                      // still reserved

        "       bne- 1b                         \n\t"

        "       isync                           \n\t"
                : [res]  "=&b" (result)
                : [obj]  "b"   (atomicInt),
                  [val]  "b"   (value),
                  [zero] "i"   (0)
                : "cr0", "ctr");

    return result;
}

}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
