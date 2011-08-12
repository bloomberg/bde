// bsls_atomicoperations_sparc64_sun_cc.h                             -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_SPARC64_SUN_CC
#define INCLUDED_BSLS_ATOMICOPERATIONS_SPARC64_SUN_CC

//@PURPOSE: Provide implentations of atomic operations for Sparc/Sun (64bit).
//
//@CLASSES:
//  bsls_AtomicOperations_SPARC64_SUN_CC: atomics for Sparc64/Sun.
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the Sun Sparc platform in 64bit mode with SunCC compiler.  The classes
// are for private use only.  See 'bsls_atomicoperations' and 'bsls_atomic' for
// the public inteface to atomics.

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_SPARC_SUN_CC_DEFAULT
#include <bsls_atomicoperations_sparc_sun_cc_default.h>
#endif

#if defined(BSLS_PLATFORM__CPU_SPARC_V9) \
    && (defined(BSLS_PLATFORM__CMP_GNU) || defined(BSLS_PLATFORM__CMP_SUN))

namespace BloombergLP {

extern "C"
{
    // *** 64 bit operations ***

    bsls_Types::Int64 bsls_AtomicOperations_Sparc64_GetInt64(
            const volatile bsls_Types::Int64 *atomicInt);

    void bsls_AtomicOperations_Sparc64_SetInt64(
            volatile bsls_Types::Int64 *atomicInt,
            bsls_Types::Int64 value);

    bsls_Types::Int64 bsls_AtomicOperations_Sparc64_SwapInt64(
            volatile bsls_Types::Int64 *atomicInt,
            bsls_Types::Int64 swapValue);

    bsls_Types::Int64 bsls_AtomicOperations_Sparc64_SwapInt64AcqRel(
            volatile bsls_Types::Int64 *atomicInt,
            bsls_Types::Int64 swapValue);

    bsls_Types::Int64 bsls_AtomicOperations_Sparc64_TestAndSwapInt64(
            volatile bsls_Types::Int64 *atomicInt,
            bsls_Types::Int64 compareValue,
            bsls_Types::Int64 swapValue);

    bsls_Types::Int64 bsls_AtomicOperations_Sparc64_TestAndSwapInt64AcqRel(
            volatile bsls_Types::Int64 *atomicInt,
            bsls_Types::Int64 compareValue,
            bsls_Types::Int64 swapValue);

    bsls_Types::Int64 bsls_AtomicOperations_Sparc64_AddInt64(
            volatile bsls_Types::Int64 *atomicInt,
            bsls_Types::Int64 value);

    bsls_Types::Int64 bsls_AtomicOperations_Sparc64_AddInt64Relaxed(
            volatile bsls_Types::Int64 *atomicInt,
            bsls_Types::Int64 value);
}

struct bsls_AtomicOperations_SPARC64_SUN_CC;
typedef bsls_AtomicOperations_SPARC64_SUN_CC  bsls_AtomicOperations_Imp;

     // ===================================================================
     // struct bsls_Atomic_TypeTraits<bsls_AtomicOperations_SPARC64_SUN_CC>
     // ===================================================================

template <>
struct bsls_Atomic_TypeTraits<bsls_AtomicOperations_SPARC64_SUN_CC>
{
    struct Int
    {
#ifdef BSLS_PLATFORM__CMP_GNU
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
#else
#       pragma align 4 (d_value)
        volatile int d_value;
#endif
    };

    struct Int64
    {
#ifdef BSLS_PLATFORM__CMP_GNU
        volatile bsls_Types::Int64 d_value
                       __attribute__((__aligned__(sizeof(bsls_Types::Int64))));
#else
#       pragma align 8 (d_value)
        volatile bsls_Types::Int64 d_value;
#endif
    };

    struct Pointer
    {
#ifdef BSLS_PLATFORM__CMP_GNU
        void const * volatile d_value
                                  __attribute__((__aligned__(sizeof(void *))));
#else
#       pragma align 8 (d_value)
        void const * volatile d_value;
#endif
    };
};

                 // ===========================================
                 // struct bsls_AtomicOperations_SPARC64_SUN_CC
                 // ===========================================

struct bsls_AtomicOperations_SPARC64_SUN_CC
    : bsls_AtomicOperations_SPARC_SUN_CC_Default64<
                                          bsls_AtomicOperations_SPARC64_SUN_CC>
{
    typedef bsls_Atomic_TypeTraits<bsls_AtomicOperations_SPARC64_SUN_CC> Types;

        // *** atomic functions for Int64 ***

    static bsls_Types::Int64 getInt64(const Types::Int64 *atomicInt);

    static bsls_Types::Int64 getInt64Acquire(const Types::Int64 *atomicInt);

    static void setInt64(Types::Int64 *atomicInt, bsls_Types::Int64 value);

    static void setInt64Release(Types::Int64 *atomicInt,
                                bsls_Types::Int64 value);

    static bsls_Types::Int64 swapInt64(Types::Int64       *atomicInt,
                                       bsls_Types::Int64   swapValue);

    static bsls_Types::Int64 swapInt64AcqRel(Types::Int64       *atomicInt,
                                             bsls_Types::Int64   swapValue);

    static bsls_Types::Int64 testAndSwapInt64(
                                            Types::Int64        *atomicInt,
                                            bsls_Types::Int64    compareValue,
                                            bsls_Types::Int64    swapValue);

    static bsls_Types::Int64 testAndSwapInt64AcqRel(
                                            Types::Int64        *atomicInt,
                                            bsls_Types::Int64    compareValue,
                                            bsls_Types::Int64    swapValue);

    static bsls_Types::Int64 addInt64Nv(Types::Int64      *atomicInt,
                                        bsls_Types::Int64  value);

    static bsls_Types::Int64 addInt64NvRelaxed(Types::Int64      *atomicInt,
                                               bsls_Types::Int64  value);

    static bsls_Types::Int64 addInt64NvAcqRel(Types::Int64      *atomicInt,
                                              bsls_Types::Int64  value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                 // -------------------------------------------
                 // struct bsls_AtomicOperations_SPARC64_SUN_CC
                 // -------------------------------------------

inline
bsls_Types::Int64 bsls_AtomicOperations_SPARC64_SUN_CC::
    getInt64(const Types::Int64 *atomicInt)
{
    return bsls_AtomicOperations_Sparc64_GetInt64(&atomicInt->d_value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_SPARC64_SUN_CC::
    getInt64Acquire(const Types::Int64 *atomicInt)
{
    return getInt64Relaxed(atomicInt);
}

inline
void bsls_AtomicOperations_SPARC64_SUN_CC::
    setInt64(Types::Int64 *atomicInt, bsls_Types::Int64 value)
{
    bsls_AtomicOperations_Sparc64_SetInt64(&atomicInt->d_value, value);
}

inline
void bsls_AtomicOperations_SPARC64_SUN_CC::
    setInt64Release(Types::Int64 *atomicInt, bsls_Types::Int64 value)
{
    setInt64Relaxed(atomicInt, value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_SPARC64_SUN_CC::
    swapInt64(Types::Int64       *atomicInt,
              bsls_Types::Int64   swapValue)
{
    return bsls_AtomicOperations_Sparc64_SwapInt64(&atomicInt->d_value,
                                                   swapValue);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_SPARC64_SUN_CC::
    swapInt64AcqRel(Types::Int64       *atomicInt,
                    bsls_Types::Int64   swapValue)
{
    return bsls_AtomicOperations_Sparc64_SwapInt64AcqRel(&atomicInt->d_value,
                                                         swapValue);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_SPARC64_SUN_CC::
    testAndSwapInt64(Types::Int64        *atomicInt,
                     bsls_Types::Int64    compareValue,
                     bsls_Types::Int64    swapValue)
{
    return bsls_AtomicOperations_Sparc64_TestAndSwapInt64(&atomicInt->d_value,
                                                          compareValue,
                                                          swapValue);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_SPARC64_SUN_CC::
    testAndSwapInt64AcqRel(Types::Int64        *atomicInt,
                           bsls_Types::Int64    compareValue,
                           bsls_Types::Int64    swapValue)
{
    return bsls_AtomicOperations_Sparc64_TestAndSwapInt64AcqRel(
                                                    &atomicInt->d_value,
                                                    compareValue,
                                                    swapValue);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_SPARC64_SUN_CC::
    addInt64Nv(Types::Int64      *atomicInt,
               bsls_Types::Int64  value)
{
    return bsls_AtomicOperations_Sparc64_AddInt64(&atomicInt->d_value, value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_SPARC64_SUN_CC::
    addInt64NvRelaxed(Types::Int64      *atomicInt,
                      bsls_Types::Int64  value)
{
    return bsls_AtomicOperations_Sparc64_AddInt64Relaxed(&atomicInt->d_value,
                                                         value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_SPARC64_SUN_CC::
    addInt64NvAcqRel(Types::Int64      *atomicInt,
                     bsls_Types::Int64  value)
{
    return addInt64NvRelaxed(atomicInt, value);
}

}  // close enterprise namespace

#endif  // SPARC_V9 && (GNU || SUN)

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
