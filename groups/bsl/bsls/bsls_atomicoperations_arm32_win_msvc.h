// bsls_atomicoperations_arm32_win_msvc.h                             -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_ARM32_WIN_MSVC
#define INCLUDED_BSLS_ATOMICOPERATIONS_ARM32_WIN_MSVC

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for arm32/MSVC/Win.
//
//@CLASSES:
//  bsls::AtomicOperations_ARM32_WIN_MSVC: atomics for arm32/MSVC/Windows
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the Windows platform with MSVC compiler for ARM CPUs.  The classes are 
// for private use only.  See 'bsls_atomicoperations' and 'bsls_atomic' for 
// the public interface to atomics.

#include <bsls_atomicoperations_default.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#if defined(BSLS_PLATFORM_CPU_ARM) && defined(BSLS_PLATFORM_CPU_32_BIT) &&    \
    defined(BSLS_PLATFORM_CMP_MSVC)
#include <intrin.h>

namespace BloombergLP {

namespace bsls {

struct AtomicOperations_ARM32_WIN_MSVC;
typedef AtomicOperations_ARM32_WIN_MSVC  AtomicOperations_Imp;

           // =========================================================
           // struct Atomic_TypeTraits<AtomicOperations_ARM32_WIN_MSVC>
           // =========================================================

template<>
struct Atomic_TypeTraits<AtomicOperations_ARM32_WIN_MSVC>
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

                    // ======================================
                    // struct AtomicOperations_ARM32_WIN_MSVC
                    // ======================================

struct AtomicOperations_ARM32_WIN_MSVC
    : AtomicOperations_Default32<AtomicOperations_ARM32_WIN_MSVC>
{
    typedef Atomic_TypeTraits<AtomicOperations_ARM32_WIN_MSVC> AtomicTypes;

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

                    // --------------------------------------
                    // struct AtomicOperations_ARM32_WIN_MSVC
                    // --------------------------------------

// Memory barrier for atomic operations with sequential consistency.
#define BSLS_ATOMIC_FENCE()  \
    __dmb(_ARM_BARRIER_ISH)

inline
int AtomicOperations_ARM32_WIN_MSVC::
    getInt(const AtomicTypes::Int *atomicInt)
{
    int result = __iso_volatile_load32(&atomicInt->d_value);
    BSLS_ATOMIC_FENCE();
    return result;
}

inline
int AtomicOperations_ARM32_WIN_MSVC::
    getIntAcquire(const AtomicTypes::Int *atomicInt)
{
    int result = __iso_volatile_load32(&atomicInt->d_value);
    BSLS_ATOMIC_FENCE();
    return result;
}

inline
void AtomicOperations_ARM32_WIN_MSVC::
    setInt(AtomicTypes::Int *atomicInt, int value)
{
    BSLS_ATOMIC_FENCE();
    __iso_volatile_store32(&atomicInt->d_value, value);
    BSLS_ATOMIC_FENCE();
}

inline
void AtomicOperations_ARM32_WIN_MSVC::
    setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
    BSLS_ATOMIC_FENCE();
    __iso_volatile_store32(&atomicInt->d_value, value);
}

inline
int AtomicOperations_ARM32_WIN_MSVC::
    swapInt(AtomicTypes::Int *atomicInt, int swapValue)
{
    return _InterlockedExchange(
            reinterpret_cast<long volatile *>(&atomicInt->d_value),
            swapValue);
}

inline
int AtomicOperations_ARM32_WIN_MSVC::
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
int AtomicOperations_ARM32_WIN_MSVC::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
    return static_cast<int>(
                    static_cast<unsigned int>(_InterlockedExchangeAdd(
                        reinterpret_cast<long volatile *>(&atomicInt->d_value),
                        value)) +
                    value);
}
    // *** atomic functions for Int64 ***

inline
Types::Int64 AtomicOperations_ARM32_WIN_MSVC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    Types::Int64 result = __iso_volatile_load64(&atomicInt->d_value);
    BSLS_ATOMIC_FENCE();
    return result;
}

inline
void AtomicOperations_ARM32_WIN_MSVC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    BSLS_ATOMIC_FENCE();
    __iso_volatile_store64(&atomicInt->d_value, value);
    BSLS_ATOMIC_FENCE();
}

inline
Types::Int64 AtomicOperations_ARM32_WIN_MSVC::
    swapInt64(AtomicTypes::Int64 *atomicInt,
              Types::Int64 swapValue)
{
    return _InterlockedExchange64(
            &atomicInt->d_value,
            swapValue);
}

inline
Types::Int64 AtomicOperations_ARM32_WIN_MSVC::
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
Types::Int64 AtomicOperations_ARM32_WIN_MSVC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
    return static_cast<Types::Int64>(
                   static_cast<Types::Uint64>(
                       _InterlockedExchangeAdd64(&atomicInt->d_value, value)) +
                   value);

}

#undef BSLS_ATOMIC_FENCE

}  // close package namespace

}  // close enterprise namespace

#endif  // arm32 && MSVC

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
