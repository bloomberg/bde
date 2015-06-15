// bsls_atomicoperations_all_all_gccintrinsics.h                      -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_ALL_ALL_GCCINTRINSICS
#define INCLUDED_BSLS_ATOMICOPERATIONS_ALL_ALL_GCCINTRINSICS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Implement atomic operations using gcc '__atomic_*' intrinsics.
//
//@CLASSES:
//  bsls::AtomicOperations_ALL_ALL_GCCIntrinsics: gcc '__atomic_*' intrinsics
//
//@DESCRIPTION: This component provides classes necessary to implement
// atomics in 32bit/64bit mode using GCC and clang compiler intrinsics.
// The classes are for private use only.  See 'bsls_atomicoperations'
// and 'bsls_atomic' for the public interface to atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 40700

namespace BloombergLP {

namespace bsls {

struct AtomicOperations_ALL_ALL_GCCIntrinsics;
typedef AtomicOperations_ALL_ALL_GCCIntrinsics  AtomicOperations_Imp;

      // ================================================================
      // struct Atomic_TypeTraits<AtomicOperations_ALL_ALL_GCCIntrinsics>
      // ================================================================

template <>
struct Atomic_TypeTraits<AtomicOperations_ALL_ALL_GCCIntrinsics>
{
    struct __attribute__((__aligned__(sizeof(int)))) Int
    {
          int d_value;
    };

    struct __attribute__((__aligned__(sizeof(Types::Int64)))) Int64
    {
          Types::Int64 d_value;
    };

    struct __attribute__((__aligned__(sizeof(void *)))) Pointer
    {
          void * d_value;
    };
};

               // =============================================
               // struct AtomicOperations_ALL_ALL_GCCIntrinsics
               // =============================================

struct AtomicOperations_ALL_ALL_GCCIntrinsics
  #ifdef BSLS_PLATFORM_CPU_64_BIT
    : AtomicOperations_Default64<AtomicOperations_ALL_ALL_GCCIntrinsics>
  #else
    : AtomicOperations_Default32<AtomicOperations_ALL_ALL_GCCIntrinsics>
  #endif
{
    typedef Atomic_TypeTraits<AtomicOperations_ALL_ALL_GCCIntrinsics>
            AtomicTypes;

    typedef char AtomicInt_SizeCheck[sizeof(int) == 4 ? 1 : -1];
        // compile-time assert

        // *** atomic functions for int ***

    static void initInt(AtomicTypes::Int *atomicInt, int value);

    static int getInt(const AtomicTypes::Int *atomicInt);

    static int getIntAcquire(const AtomicTypes::Int *atomicInt);

    static int getIntRelaxed(const AtomicTypes::Int *atomicInt);

    static void setInt(AtomicTypes::Int *atomicInt, int value);

    static void setIntRelease(AtomicTypes::Int *atomicInt, int value);

    static void setIntRelaxed(AtomicTypes::Int *atomicInt, int value);

    static int swapInt(AtomicTypes::Int *atomicInt, int swapValue);

    static int swapIntAcqRel(AtomicTypes::Int *atomicInt, int swapValue);

    static int testAndSwapInt(AtomicTypes::Int *atomicInt,
                              int               compareValue,
                              int               swapValue);

    static int testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                                    int               compareValue,
                                    int               swapValue);

    static int addIntNv(AtomicTypes::Int *atomicInt, int value);

    static int addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value);

    static int addIntNvRelaxed(AtomicTypes::Int *atomicInt, int value);

        // *** atomic functions for Int64 ***

    static void initInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static Types::Int64 getInt64(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Acquire(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Relaxed(const AtomicTypes::Int64 *atomicInt);

    static void setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static void setInt64Release(AtomicTypes::Int64 *atomicInt,
                                Types::Int64        value);

    static void setInt64Relaxed(AtomicTypes::Int64 *atomicInt,
                                Types::Int64        value);

    static Types::Int64 swapInt64(AtomicTypes::Int64  *atomicInt,
                                  Types::Int64         swapValue);

    static Types::Int64 swapInt64AcqRel(AtomicTypes::Int64  *atomicInt,
                                        Types::Int64         swapValue);

    static Types::Int64 testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64        compareValue,
                                         Types::Int64        swapValue);

    static Types::Int64 testAndSwapInt64AcqRel(
        AtomicTypes::Int64 *atomicInt,
        Types::Int64        compareValue,
        Types::Int64        swapValue);

    static Types::Int64 addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                   Types::Int64        value);

    static Types::Int64 addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64        value);

    static Types::Int64 addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt,
                                          Types::Int64        value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

               // ---------------------------------------------
               // struct AtomicOperations_ALL_ALL_GCCIntrinsics
               // ---------------------------------------------

inline
void AtomicOperations_ALL_ALL_GCCIntrinsics::
    initInt(AtomicTypes::Int *atomicInt, int value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
int AtomicOperations_ALL_ALL_GCCIntrinsics::
    getInt(const AtomicTypes::Int *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_SEQ_CST);
}

inline
int AtomicOperations_ALL_ALL_GCCIntrinsics::
    getIntAcquire(const AtomicTypes::Int *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_ACQUIRE);
}

inline
int AtomicOperations_ALL_ALL_GCCIntrinsics::
    getIntRelaxed(const AtomicTypes::Int *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_RELAXED);
}

inline
void AtomicOperations_ALL_ALL_GCCIntrinsics::
    setInt(AtomicTypes::Int *atomicInt, int value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
}

inline
void AtomicOperations_ALL_ALL_GCCIntrinsics::
    setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELEASE);
}

inline
void AtomicOperations_ALL_ALL_GCCIntrinsics::
    setIntRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
int AtomicOperations_ALL_ALL_GCCIntrinsics::
    swapInt(AtomicTypes::Int *atomicInt, int swapValue)
{
    return
      __atomic_exchange_n(&atomicInt->d_value, swapValue, __ATOMIC_SEQ_CST);
}

inline
int AtomicOperations_ALL_ALL_GCCIntrinsics::
    swapIntAcqRel(AtomicTypes::Int *atomicInt, int swapValue)
{
    return
      __atomic_exchange_n(&atomicInt->d_value, swapValue, __ATOMIC_ACQ_REL);
}

inline
int AtomicOperations_ALL_ALL_GCCIntrinsics::
    testAndSwapInt(AtomicTypes::Int *atomicInt,
                   int               compareValue,
                   int               swapValue)
{
    __atomic_compare_exchange_n(&atomicInt->d_value, &compareValue, swapValue,
                                0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return compareValue;
}

inline
int AtomicOperations_ALL_ALL_GCCIntrinsics::
    testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                         int               compareValue,
                         int               swapValue)
{
    __atomic_compare_exchange_n(&atomicInt->d_value, &compareValue, swapValue,
                                0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
    return compareValue;
}

inline
int AtomicOperations_ALL_ALL_GCCIntrinsics::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
}

inline
int AtomicOperations_ALL_ALL_GCCIntrinsics::
    addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_ACQ_REL);
}

inline
int AtomicOperations_ALL_ALL_GCCIntrinsics::
    addIntNvRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
void AtomicOperations_ALL_ALL_GCCIntrinsics::
    initInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_GCCIntrinsics::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_SEQ_CST);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_GCCIntrinsics::
    getInt64Acquire(const AtomicTypes::Int64 *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_ACQUIRE);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_GCCIntrinsics::
    getInt64Relaxed(const AtomicTypes::Int64 *atomicInt)
{
    return __atomic_load_n(&atomicInt->d_value, __ATOMIC_RELAXED);
}

inline
void AtomicOperations_ALL_ALL_GCCIntrinsics::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
}

inline
void AtomicOperations_ALL_ALL_GCCIntrinsics::
    setInt64Release(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELEASE);
}

inline
void AtomicOperations_ALL_ALL_GCCIntrinsics::
    setInt64Relaxed(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __atomic_store_n(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_GCCIntrinsics::
    swapInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 swapValue)
{
    return
      __atomic_exchange_n(&atomicInt->d_value, swapValue, __ATOMIC_SEQ_CST);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_GCCIntrinsics::
    swapInt64AcqRel(AtomicTypes::Int64 *atomicInt, Types::Int64 swapValue)
{
    return
      __atomic_exchange_n(&atomicInt->d_value, swapValue, __ATOMIC_ACQ_REL);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_GCCIntrinsics::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64        compareValue,
                     Types::Int64        swapValue)
{
    __atomic_compare_exchange_n(&atomicInt->d_value, &compareValue, swapValue,
                                0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return compareValue;
}

inline
Types::Int64 AtomicOperations_ALL_ALL_GCCIntrinsics::
    testAndSwapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                           Types::Int64        compareValue,
                           Types::Int64        swapValue)
{
    __atomic_compare_exchange_n(&atomicInt->d_value, &compareValue, swapValue,
                                0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
    return compareValue;
}

inline
Types::Int64 AtomicOperations_ALL_ALL_GCCIntrinsics::
    addInt64Nv(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_GCCIntrinsics::
    addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_ACQ_REL);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_GCCIntrinsics::
    addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    return __atomic_add_fetch(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

}  // close package namespace

}  // close enterprise namespace

#endif  // BSLS_PLATFORM_CMP_GNU || BSLS_PLATFORM_CMP_CLANG

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
