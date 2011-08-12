// bsls_atomicoperations_powerpc_aix_xlc_default.h                    -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC_AIX_XLC_DEFAULT
#define INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC_AIX_XLC_DEFAULT

//@PURPOSE: Provide default base implentations of atomics for PowerPC/AIX.
//
//@CLASSES:
//  bsls_AtomicOperations_POWERPC_AIX_XLC_Default32: default base for 32bit.
//  bsls_AtomicOperations_POWERPC_AIX_XLC_Default64: default base for 64bit.
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides default base classes necessary to
// implement atomics on the AIX PowerPC platform with the xlC compiler.  The
// classes are for private use only.  See 'bsls_atomicoperations' and
// 'bsls_atomic' for the public inteface to atomics.

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

#if defined(BSLS_PLATFORM__CPU_POWERPC) && defined(BSLS_PLATFORM__CMP_IBM)

#include <builtins.h>

namespace BloombergLP {

    // *** functions implemented in assembly for both 32bit and 64bit ***

int bsls_AtomicOperations_Powerpc_GetInt(const volatile int *atomicInt);
int bsls_AtomicOperations_Powerpc_GetIntAcquire(const volatile int *atomicInt);
void bsls_AtomicOperations_Powerpc_SetInt(volatile int *atomicInt, int value);
void bsls_AtomicOperations_Powerpc_SetIntRelease(volatile int *atomicInt,
                                                 int value);
int bsls_AtomicOperations_Powerpc_SwapInt(volatile int *atomicInt,
                                          int swapValue);
int bsls_AtomicOperations_Powerpc_SwapIntAcqRel(volatile int *atomicInt,
                                                int swapValue);
int bsls_AtomicOperations_Powerpc_TestAndSwapInt(volatile int *atomicInt,
                                                 int compareValue,
                                                 int swapValue);
int bsls_AtomicOperations_Powerpc_TestAndSwapIntAcqRel(volatile int *atomicInt,
                                                       int compareValue,
                                                       int swapValue);
int bsls_AtomicOperations_Powerpc_AddInt(volatile int *atomicInt, int value);
int bsls_AtomicOperations_Powerpc_AddIntAcqRel(volatile int *atomicInt,
                                               int value);

           // =======================================================
           // struct bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt
           // =======================================================

template <typename IMP>
struct bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt
    : bsls_AtomicOperations_DefaultInt<IMP>
{
    typedef bsls_Atomic_TypeTraits<IMP> Types;

        // *** atomic functions for int ***

    static int getInt(typename Types::Int const *atomicInt);

    static int getIntAcquire(typename Types::Int const *atomicInt);

    static void setInt(typename Types::Int *atomicInt, int value);

    static void setIntRelease(typename Types::Int *atomicInt, int value);

    static int swapInt(typename Types::Int *atomicInt, int swapValue);

    static int swapIntAcqRel(typename Types::Int *atomicInt, int swapValue);

    static int testAndSwapInt(typename Types::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int testAndSwapIntAcqRel(typename Types::Int *atomicInt,
                                    int                  compareValue,
                                    int                  swapValue);

    static int addIntNv(typename Types::Int *atomicInt, int value);

    static int addIntNvRelaxed(typename Types::Int *atomicInt, int value);

    static int addIntNvAcqRel(typename Types::Int *atomicInt, int value);
};

           // ======================================================
           // struct bsls_AtomicOperations_POWERPC_AIX_XLC_Default32
           // ======================================================

template <typename IMP>
struct bsls_AtomicOperations_POWERPC_AIX_XLC_Default32
    : bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>
    , bsls_AtomicOperations_DefaultInt64<IMP>
    , bsls_AtomicOperations_DefaultPointer32<IMP>
{};

           // ======================================================
           // struct bsls_AtomicOperations_POWERPC_AIX_XLC_Default64
           // ======================================================

template <typename IMP>
struct bsls_AtomicOperations_POWERPC_AIX_XLC_Default64
    : bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>
    , bsls_AtomicOperations_DefaultInt64<IMP>
    , bsls_AtomicOperations_DefaultPointer64<IMP>
{};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

           // -------------------------------------------------------
           // struct bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt
           // -------------------------------------------------------

template <typename IMP>
inline
int bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    getInt(typename Types::Int const *atomicInt)
{
    return bsls_AtomicOperations_Powerpc_GetInt(&atomicInt->d_value);
}

template <typename IMP>
inline
int bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    getIntAcquire(typename Types::Int const *atomicInt)
{
    return bsls_AtomicOperations_Powerpc_GetIntAcquire(&atomicInt->d_value);
}

template <typename IMP>
inline
void bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    setInt(typename Types::Int *atomicInt, int value)
{
    bsls_AtomicOperations_Powerpc_SetInt(&atomicInt->d_value, value);
}

template <typename IMP>
inline
void bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    setIntRelease(typename Types::Int *atomicInt, int value)
{
    bsls_AtomicOperations_Powerpc_SetIntRelease(&atomicInt->d_value, value);
}

template <typename IMP>
inline
int bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    swapInt(typename Types::Int *atomicInt, int swapValue)
{
    return bsls_AtomicOperations_Powerpc_SwapInt(&atomicInt->d_value,
                                                 swapValue);
}

template <typename IMP>
inline
int bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    swapIntAcqRel(typename Types::Int *atomicInt, int swapValue)
{
    return bsls_AtomicOperations_Powerpc_SwapIntAcqRel(&atomicInt->d_value,
                                                       swapValue);
}

template <typename IMP>
inline
int bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    testAndSwapInt(typename Types::Int *atomicInt,
                   int compareValue,
                   int swapValue)
{
    return bsls_AtomicOperations_Powerpc_TestAndSwapInt(&atomicInt->d_value,
                                                        compareValue,
                                                        swapValue);
}

template <typename IMP>
inline
int bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    testAndSwapIntAcqRel(typename Types::Int *atomicInt,
                         int                  compareValue,
                         int                  swapValue)
{
    return bsls_AtomicOperations_Powerpc_TestAndSwapIntAcqRel(
                                                    &atomicInt->d_value,
                                                    compareValue,
                                                    swapValue);
}

template <typename IMP>
inline
int bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    addIntNv(typename Types::Int *atomicInt, int value)
{
    return bsls_AtomicOperations_Powerpc_AddInt(&atomicInt->d_value, value);
}

template <typename IMP>
inline
int bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    addIntNvRelaxed(typename Types::Int *atomicInt, int value)
{
    return __fetch_and_add(&atomicInt->d_value, value) + value;
}

template <typename IMP>
inline
int bsls_AtomicOperations_POWERPC_AIX_XLC_DefaultInt<IMP>::
    addIntNvAcqRel(typename Types::Int *atomicInt, int value)
{
    return bsls_AtomicOperations_Powerpc_AddIntAcqRel(&atomicInt->d_value,
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
