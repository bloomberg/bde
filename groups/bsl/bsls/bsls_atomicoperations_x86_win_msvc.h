// bsls_atomicoperations_x86_win_msvc.h                               -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_X86_WIN_MSVC
#define INCLUDED_BSLS_ATOMICOPERATIONS_X86_WIN_MSVC

//@PURPOSE: Provide implentations of atomic operations for X86/Windows.
//
//@CLASSES:
//  bsls_AtomicOperations_X86_WIN_MSVC: implementation of atomics for
//                                      X86/Windows
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the Windows X86 platform with MSVC compiler.  The classes are for private
// use only.  See 'bsls_atomicoperations' and 'bsls_atomic' for the public
// inteface to atomics.

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

#if defined(BSLS_PLATFORM__CPU_X86) && defined(BSLS_PLATFORM__OS_WINDOWS)

#include <intrin.h>

// Visual C++ implementation exploits the fact that 'volatile' loads and stores
// have acquire and release semantics (load - acquire, store - release).  So
// these memory ordering guarantees come for free (and accidentally they are
// no-op on x86).  However the implementation of operations providing
// the sequential consistency guarantee still requires a memory barrier.
//
// As for interlocked intrinsics, they provide the sequential consistency
// guarantee, so no additional memory barrier is needed.
//
// For some explanations, see
// http://blogs.msdn.com/b/kangsu/archive/2007/07/16/
//                       volatile-acquire-release-memory-fences-and-vc2005.aspx
// and also MSDN documentation for 'volatile' and interlocked intrinsics in
// VC++ 2005 and later.

namespace BloombergLP {

struct bsls_AtomicOperations_X86_WIN_MSVC;
typedef bsls_AtomicOperations_X86_WIN_MSVC  bsls_AtomicOperations_Imp;

      // =================================================================
      // struct bsls_Atomic_TypeTraits<bsls_AtomicOperations_X86_WIN_MSVC>
      // =================================================================

template<>
struct bsls_Atomic_TypeTraits<bsls_AtomicOperations_X86_WIN_MSVC>
{
    struct Int
    {
        __declspec(align(4))
        volatile int d_value;
    };

    struct Int64
    {
        __declspec(align(8))
        volatile bsls_Types::Int64 d_value;
    };

    struct Pointer
    {
        __declspec(align(4))
        void const * volatile d_value;
    };
};

                  // =========================================
                  // struct bsls_AtomicOperations_X86_WIN_MSVC
                  // =========================================

struct bsls_AtomicOperations_X86_WIN_MSVC
    : bsls_AtomicOperations_Default32<bsls_AtomicOperations_X86_WIN_MSVC>
{
    typedef bsls_Atomic_TypeTraits<bsls_AtomicOperations_X86_WIN_MSVC> Types;

        // *** atomic functions for int ***

    static int getInt(const Types::Int *atomicInt);

    static int getIntAcquire(const Types::Int *atomicInt);

    static void setInt(Types::Int *atomicInt, int value);

    static void setIntRelease(Types::Int *atomicInt, int value);

    static int swapInt(Types::Int *atomicInt, int swapValue);

    static int testAndSwapInt(Types::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int addIntNv(Types::Int *atomicInt, int value);

        // *** atomic functions for Int64 ***

    static bsls_Types::Int64 getInt64(const Types::Int64 *atomicInt);

    static void setInt64(Types::Int64 *atomicInt, bsls_Types::Int64 value);

    static bsls_Types::Int64 swapInt64(Types::Int64       *atomicInt,
                                       bsls_Types::Int64   swapValue);

    static bsls_Types::Int64 testAndSwapInt64(
                                            Types::Int64        *atomicInt,
                                            bsls_Types::Int64    compareValue,
                                            bsls_Types::Int64    swapValue);

    static bsls_Types::Int64 addInt64Nv(Types::Int64      *atomicInt,
                                        bsls_Types::Int64  value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // -----------------------------------------
                  // struct bsls_AtomicOperations_X86_WIN_MSVC
                  // -----------------------------------------

// Memory barrier for atomic operations with sequential consistency.
#define BSLS_ATOMIC_FENCE()  \
    __asm lock add dword ptr [esp], 0

inline
int bsls_AtomicOperations_X86_WIN_MSVC::
    getInt(const Types::Int *atomicInt)
{
    BSLS_ATOMIC_FENCE();
    return atomicInt->d_value;
}

inline
int bsls_AtomicOperations_X86_WIN_MSVC::
    getIntAcquire(const Types::Int *atomicInt)
{
    return atomicInt->d_value;
}

inline
void bsls_AtomicOperations_X86_WIN_MSVC::
    setInt(Types::Int *atomicInt, int value)
{
    atomicInt->d_value = value;
    BSLS_ATOMIC_FENCE();
}

inline
void bsls_AtomicOperations_X86_WIN_MSVC::
    setIntRelease(Types::Int *atomicInt, int value)
{
    atomicInt->d_value = value;
}

inline
int bsls_AtomicOperations_X86_WIN_MSVC::
    swapInt(Types::Int *atomicInt, int swapValue)
{
    return _InterlockedExchange(
            reinterpret_cast<long volatile *>(&atomicInt->d_value),
            swapValue);
}

inline
int bsls_AtomicOperations_X86_WIN_MSVC::
    testAndSwapInt(Types::Int *atomicInt, int compareValue, int swapValue)
{
    return _InterlockedCompareExchange(
            reinterpret_cast<long volatile *>(&atomicInt->d_value),
            swapValue,
            compareValue);
}

inline
int bsls_AtomicOperations_X86_WIN_MSVC::
    addIntNv(Types::Int *atomicInt, int value)
{
    return _InterlockedExchangeAdd(
            reinterpret_cast<long volatile *>(&atomicInt->d_value),
            value)
        + value;
}

    // *** atomic functions for Int64 ***

inline
bsls_Types::Int64 bsls_AtomicOperations_X86_WIN_MSVC::
    getInt64(const Types::Int64 *atomicInt)
{
    return _InterlockedCompareExchange64(
            const_cast<bsls_Types::Int64 volatile *>(&atomicInt->d_value),
            0,
            0);
}

inline
void bsls_AtomicOperations_X86_WIN_MSVC::
    setInt64(Types::Int64 *atomicInt, bsls_Types::Int64 value)
{
    swapInt64(atomicInt, value);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_X86_WIN_MSVC::
    swapInt64(Types::Int64       *atomicInt,
              bsls_Types::Int64   swapValue)
{
    bsls_Types::Int64 actual = atomicInt->d_value;
    bsls_Types::Int64 expected;

    do
    {
        expected = actual;
        actual = testAndSwapInt64(atomicInt, expected, swapValue);
    } while (actual != expected);

    return expected;
}

inline
bsls_Types::Int64 bsls_AtomicOperations_X86_WIN_MSVC::
    testAndSwapInt64(Types::Int64        *atomicInt,
                     bsls_Types::Int64    compareValue,
                     bsls_Types::Int64    swapValue)
{
    return _InterlockedCompareExchange64(
            &atomicInt->d_value,
            swapValue,
            compareValue);
}

inline
bsls_Types::Int64 bsls_AtomicOperations_X86_WIN_MSVC::
    addInt64Nv(Types::Int64      *atomicInt,
               bsls_Types::Int64  value)
{
    bsls_Types::Int64 actual = atomicInt->d_value;
    bsls_Types::Int64 expected;

    do
    {
        expected = actual;
        actual = testAndSwapInt64(atomicInt, expected, expected + value);
    } while (actual != expected);

    return expected + value;
}

#undef BSLS_ATOMIC_FENCE

}  // close enterprise namespace

#endif  // X86 && WINDOWS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
