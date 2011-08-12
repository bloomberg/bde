// bsls_atomicoperations_powerpc32_aix_xlc.h                          -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC32_AIX_XLC
#define INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC32_AIX_XLC

//@PURPOSE: Provide implentations of atomic operations for PowerPC/AIX (32bit).
//
//@CLASSES:
//  bsls_AtomicOperations_POWERPC32_AIX_XLC: atomics for PPC32/AIX.
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the AIX PowerPC platform in 32bit mode with the xlC compiler.  The
// classes are for private use only.  See 'bsls_atomicoperations' and
// 'bsls_atomic' for the public inteface to atomics.

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC_AIX_XLC_DEFAULT
#include <bsls_atomicoperations_powerpc_aix_xlc_default.h>
#endif

#if defined(BSLS_PLATFORM__CPU_POWERPC) && defined(BSLS_PLATFORM__CMP_IBM) \
    && defined(BSLS_PLATFORM__CPU_32_BIT)

namespace BloombergLP {

    // *** functions implemented in assembly ***

bsls_Types::Int64 bsls_AtomicOperations_Powerpc32_GetInt64(
                                  const volatile bsls_Types::Int64 *atomicInt);
bsls_Types::Int64 bsls_AtomicOperations_Powerpc32_GetInt64Relaxed(
                                  const volatile bsls_Types::Int64 *atomicInt);
bsls_Types::Int64 bsls_AtomicOperations_Powerpc32_GetInt64Acquire(
                                  const volatile bsls_Types::Int64 *atomicInt);

    // *** extern "C" functions due to prototype hacks ***

extern "C"
void bsls_AtomicOperations_Powerpc32_SetInt64(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value);

extern "C"
void bsls_AtomicOperations_Powerpc32_SetInt64Relaxed(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value);

extern "C"
void bsls_AtomicOperations_Powerpc32_SetInt64Release(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value);

extern "C"
bsls_Types::Int64 bsls_AtomicOperations_Powerpc32_SwapInt64(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value);

extern "C"
bsls_Types::Int64 bsls_AtomicOperations_Powerpc32_SwapInt64AcqRel(
                                        volatile bsls_Types::Int64 *atomicInt,
                                        bsls_Types::Int64           value);

extern "C"
bsls_Types::Int64 bsls_AtomicOperations_Powerpc32_testAndSwapInt64(
                                      volatile bsls_Types::Int64 *atomicInt,
                                      bsls_Types::Int64           compareValue,
                                      bsls_Types::Int64           swapValue);

extern "C"
bsls_Types::Int64 bsls_AtomicOperations_Powerpc32_testAndSwapInt64AcqRel(
                                      volatile bsls_Types::Int64 *atomicInt,
                                      bsls_Types::Int64           compareValue,
                                      bsls_Types::Int64           swapValue);

extern "C"
bsls_Types::Int64 bsls_AtomicOperations_Powerpc32_AddInt64(
                                         volatile bsls_Types::Int64 *atomicInt,
                                         bsls_Types::Int64           value);

extern "C"
bsls_Types::Int64 bsls_AtomicOperations_Powerpc32_AddInt64Relaxed(
                                         volatile bsls_Types::Int64 *atomicInt,
                                         bsls_Types::Int64           value);

extern "C"
bsls_Types::Int64 bsls_AtomicOperations_Powerpc32_AddInt64AcqRel(
                                         volatile bsls_Types::Int64 *atomicInt,
                                         bsls_Types::Int64           value);

struct bsls_AtomicOperations_POWERPC32_AIX_XLC;
typedef bsls_AtomicOperations_POWERPC32_AIX_XLC  bsls_AtomicOperations_Imp;

   // ======================================================================
   // struct bsls_Atomic_TypeTraits<bsls_AtomicOperations_POWERPC32_AIX_XLC>
   // ======================================================================

template <>
struct bsls_Atomic_TypeTraits<bsls_AtomicOperations_POWERPC32_AIX_XLC>
{
    struct Int
    {
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
    };

    struct Int64
    {
        volatile bsls_Types::Int64 d_value
              __attribute__((__aligned__(sizeof(bsls_Types::Int64))));
    };

    struct Pointer
    {
        void const * volatile d_value
              __attribute__((__aligned__(sizeof(void *))));
    };
};

               // ==============================================
               // struct bsls_AtomicOperations_POWERPC32_AIX_XLC
               // ==============================================

struct bsls_AtomicOperations_POWERPC32_AIX_XLC
    : bsls_AtomicOperations_POWERPC_AIX_XLC_Default32<
                                       bsls_AtomicOperations_POWERPC32_AIX_XLC>
{
    typedef bsls_Atomic_TypeTraits<bsls_AtomicOperations_POWERPC32_AIX_XLC>
        Types;

        //  *** atomic functions for Int64 ***

    static bsls_Types::Int64 getInt64(const Types::Int64 *atomicInt);

    static bsls_Types::Int64 getInt64Relaxed(const Types::Int64 *atomicInt);

    static bsls_Types::Int64 getInt64Acquire(const Types::Int64 *atomicInt);

    static void setInt64(Types::Int64 *atomicInt, bsls_Types::Int64 value);

    static void setInt64Relaxed(Types::Int64 *atomicInt,
                                bsls_Types::Int64 value);

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

    static bsls_Types::Int64 addInt64NvRelaxed(Types::Int64       *atomicInt,
                                               bsls_Types::Int64   value);

    static bsls_Types::Int64 addInt64NvAcqRel(Types::Int64      *atomicInt,
                                              bsls_Types::Int64  value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

               // ----------------------------------------------
               // struct bsls_AtomicOperations_POWERPC32_AIX_XLC
               // ----------------------------------------------

inline
bsls_Types::Int64 bsls_AtomicOperations_POWERPC32_AIX_XLC::
    getInt64(const Types::Int64 *atomicInt)
{
    return bsls_AtomicOperations_Powerpc32_GetInt64(&atomicInt->d_value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_POWERPC32_AIX_XLC::
    getInt64Relaxed(const Types::Int64 *atomicInt)
{
    return bsls_AtomicOperations_Powerpc32_GetInt64Relaxed(&atomicInt->d_value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_POWERPC32_AIX_XLC::
    getInt64Acquire(const Types::Int64 *atomicInt)
{
    return bsls_AtomicOperations_Powerpc32_GetInt64Acquire(&atomicInt->d_value);
}

inline
void bsls_AtomicOperations_POWERPC32_AIX_XLC::
    setInt64(Types::Int64 *atomicInt, bsls_Types::Int64 value)
{
    bsls_AtomicOperations_Powerpc32_SetInt64(&atomicInt->d_value, value);
}

inline
void bsls_AtomicOperations_POWERPC32_AIX_XLC::
    setInt64Relaxed(Types::Int64 *atomicInt, bsls_Types::Int64 value)
{
    bsls_AtomicOperations_Powerpc32_SetInt64Relaxed(&atomicInt->d_value, value);
}

inline
void bsls_AtomicOperations_POWERPC32_AIX_XLC::
    setInt64Release(Types::Int64 *atomicInt, bsls_Types::Int64 value)
{
    bsls_AtomicOperations_Powerpc32_SetInt64Release(&atomicInt->d_value, value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_POWERPC32_AIX_XLC::
    swapInt64(Types::Int64       *atomicInt,
              bsls_Types::Int64   swapValue)
{
    return bsls_AtomicOperations_Powerpc32_SwapInt64(&atomicInt->d_value,
                                                     swapValue);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_POWERPC32_AIX_XLC::
    swapInt64AcqRel(Types::Int64       *atomicInt,
                    bsls_Types::Int64   swapValue)
{
    return bsls_AtomicOperations_Powerpc32_SwapInt64AcqRel(&atomicInt->d_value,
                                                           swapValue);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_POWERPC32_AIX_XLC::
    testAndSwapInt64(Types::Int64        *atomicInt,
                     bsls_Types::Int64    compareValue,
                     bsls_Types::Int64    swapValue)
{
    return bsls_AtomicOperations_Powerpc32_testAndSwapInt64(
                                                        &atomicInt->d_value,
                                                        compareValue,
                                                        swapValue);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_POWERPC32_AIX_XLC::
    testAndSwapInt64AcqRel(Types::Int64        *atomicInt,
                           bsls_Types::Int64    compareValue,
                           bsls_Types::Int64    swapValue)
{
    return bsls_AtomicOperations_Powerpc32_testAndSwapInt64AcqRel(
                                                        &atomicInt->d_value,
                                                        compareValue,
                                                        swapValue);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_POWERPC32_AIX_XLC::
    addInt64Nv(Types::Int64      *atomicInt,
               bsls_Types::Int64  value)
{
    return bsls_AtomicOperations_Powerpc32_AddInt64(&atomicInt->d_value,
                                                    value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_POWERPC32_AIX_XLC::
    addInt64NvRelaxed(Types::Int64       *atomicInt,
                      bsls_Types::Int64   value)
{
    return bsls_AtomicOperations_Powerpc32_AddInt64Relaxed(&atomicInt->d_value,
                                                           value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_POWERPC32_AIX_XLC::
    addInt64NvAcqRel(Types::Int64      *atomicInt,
                     bsls_Types::Int64  value)
{
    return bsls_AtomicOperations_Powerpc32_AddInt64AcqRel(&atomicInt->d_value,
                                                          value);
}

}  // close enterprise namespace

#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
