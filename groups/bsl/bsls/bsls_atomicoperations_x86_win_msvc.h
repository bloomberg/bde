// bsls_atomicoperations_x86_win_msvc.h                               -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_X86_WIN_MSVC
#define INCLUDED_BSLS_ATOMICOPERATIONS_X86_WIN_MSVC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for X86/MSVC/Windows.
//
//@CLASSES:
//  bsls::AtomicOperations_X86_WIN_MSVC: atomics for X86/MSVC/Windows
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the Windows X86 platform with MSVC compiler.  The classes are for private
// use only.  See 'bsls_atomicoperations' and 'bsls_atomic' for the public
// interface to atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_X86) && defined(BSLS_PLATFORM_CMP_MSVC)

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

namespace bsls {

struct AtomicOperations_X86_WIN_MSVC;
typedef AtomicOperations_X86_WIN_MSVC  AtomicOperations_Imp;

           // =======================================================
           // struct Atomic_TypeTraits<AtomicOperations_X86_WIN_MSVC>
           // =======================================================

template<>
struct Atomic_TypeTraits<AtomicOperations_X86_WIN_MSVC>
{
    struct Int
    {
        __declspec(align(4))
        volatile int d_value;
    };

    struct Int64
    {
        __declspec(align(8))
        volatile Types::Int64 d_value;
    };

    struct Uint
    {
        __declspec(align(4))
        volatile unsigned int d_value;
    };

    struct Uint64
    {
        __declspec(align(8))
        volatile Types::Uint64 d_value;
    };

    struct Pointer
    {
        __declspec(align(4))
        void * volatile d_value;
    };
};

                    // ====================================
                    // struct AtomicOperations_X86_WIN_MSVC
                    // ====================================

struct AtomicOperations_X86_WIN_MSVC
    : AtomicOperations_Default32<AtomicOperations_X86_WIN_MSVC>
{
    typedef Atomic_TypeTraits<AtomicOperations_X86_WIN_MSVC> AtomicTypes;

        // *** atomic functions for int ***

    static int getInt(const AtomicTypes::Int *atomicInt);

    static int getIntAcquire(const AtomicTypes::Int *atomicInt);

    static void setInt(AtomicTypes::Int *atomicInt, int value);

    static void setIntRelease(AtomicTypes::Int *atomicInt, int value);

    static int swapInt(AtomicTypes::Int *atomicInt, int swapValue);

    static int testAndSwapInt(AtomicTypes::Int *atomicInt,
                              int compareValue,
                              int swapValue);

    static int addIntNv(AtomicTypes::Int *atomicInt, int value);

        // *** atomic functions for Int64 ***

    static Types::Int64 getInt64(const AtomicTypes::Int64 *atomicInt);

    static void setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static Types::Int64 swapInt64(AtomicTypes::Int64 *atomicInt,
                                  Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 compareValue,
                                         Types::Int64 swapValue);

    static Types::Int64 addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                   Types::Int64 value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                    // ------------------------------------
                    // struct AtomicOperations_X86_WIN_MSVC
                    // ------------------------------------

// Memory barrier for atomic operations with sequential consistency.
#define BSLS_ATOMIC_FENCE()  \
    __asm lock add dword ptr [esp], 0

inline
int AtomicOperations_X86_WIN_MSVC::
    getInt(const AtomicTypes::Int *atomicInt)
{
    BSLS_ATOMIC_FENCE();
    return atomicInt->d_value;
}

inline
int AtomicOperations_X86_WIN_MSVC::
    getIntAcquire(const AtomicTypes::Int *atomicInt)
{
    return atomicInt->d_value;
}

inline
void AtomicOperations_X86_WIN_MSVC::
    setInt(AtomicTypes::Int *atomicInt, int value)
{
    atomicInt->d_value = value;
    BSLS_ATOMIC_FENCE();
}

inline
void AtomicOperations_X86_WIN_MSVC::
    setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
    atomicInt->d_value = value;
}

inline
int AtomicOperations_X86_WIN_MSVC::
    swapInt(AtomicTypes::Int *atomicInt, int swapValue)
{
    return _InterlockedExchange(
            reinterpret_cast<long volatile *>(&atomicInt->d_value),
            swapValue);
}

inline
int AtomicOperations_X86_WIN_MSVC::
    testAndSwapInt(AtomicTypes::Int *atomicInt,
                   int compareValue,
                   int swapValue)
{
    return _InterlockedCompareExchange(
            reinterpret_cast<long volatile *>(&atomicInt->d_value),
            swapValue,
            compareValue);
}

inline
int AtomicOperations_X86_WIN_MSVC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
    return _InterlockedExchangeAdd(
            reinterpret_cast<long volatile *>(&atomicInt->d_value),
            value)
        + value;
}

    // *** atomic functions for Int64 ***

inline
Types::Int64 AtomicOperations_X86_WIN_MSVC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    return _InterlockedCompareExchange64(
            const_cast<Types::Int64 volatile *>(&atomicInt->d_value),
            0,
            0);
}

inline
void AtomicOperations_X86_WIN_MSVC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    swapInt64(atomicInt, value);
}

inline
Types::Int64 AtomicOperations_X86_WIN_MSVC::
    swapInt64(AtomicTypes::Int64 *atomicInt,
              Types::Int64 swapValue)
{
    Types::Int64 actual = atomicInt->d_value;
    Types::Int64 expected;

    do
    {
        expected = actual;
        actual = testAndSwapInt64(atomicInt, expected, swapValue);
    } while (actual != expected);

    return expected;
}

inline
Types::Int64 AtomicOperations_X86_WIN_MSVC::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
{
    return _InterlockedCompareExchange64(
            &atomicInt->d_value,
            swapValue,
            compareValue);
}

inline
Types::Int64 AtomicOperations_X86_WIN_MSVC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
    Types::Int64 actual = atomicInt->d_value;
    Types::Int64 expected;

    do
    {
        expected = actual;
        actual = testAndSwapInt64(atomicInt, expected, expected + value);
    } while (actual != expected);

    return expected + value;
}

#undef BSLS_ATOMIC_FENCE

}  // close package namespace

}  // close enterprise namespace

#endif  // X86 && MSVC

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
