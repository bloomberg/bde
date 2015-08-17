// bsls_atomicoperations_all_all_clangintrinsics.h                    -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_ALL_ALL_CLANGINTRINSICS
#define INCLUDED_BSLS_ATOMICOPERATIONS_ALL_ALL_CLANGINTRINSICS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Implement atomic operations using clang '__c11_atomic_*' intrinsics
//
//@CLASSES:
//  bsls::AtomicOperations_ALL_ALL_ClangIntrinsics:
//  clang '__c11_atomic_*' intrinsics
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// in 32bit/64bit mode using clang compiler intrinsics.  The classes are for
// private use only.  See 'bsls_atomicoperations' and 'bsls_atomic' for the
// public interface to atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_DEFAULT
#include <bsls_atomicoperations_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)
#if __has_extension(c_atomic) || __has_extension(cxx_atomic)

namespace BloombergLP {

namespace bsls {

struct AtomicOperations_ALL_ALL_ClangIntrinsics;
typedef AtomicOperations_ALL_ALL_ClangIntrinsics  AtomicOperations_Imp;

     // ==================================================================
     // struct Atomic_TypeTraits<AtomicOperations_ALL_ALL_ClangIntrinsics>
     // ==================================================================

template <>
struct Atomic_TypeTraits<AtomicOperations_ALL_ALL_ClangIntrinsics>
{
    struct __attribute__((__aligned__(sizeof(int)))) Int
    {
          _Atomic(int) d_value;
    };

    struct __attribute__((__aligned__(sizeof(Types::Int64)))) Int64
    {
          _Atomic(Types::Int64) d_value;
    };

    struct __attribute__((__aligned__(sizeof(void *)))) Pointer
    {
          _Atomic(void *) d_value;
    };
};

              // ===============================================
              // struct AtomicOperations_ALL_ALL_ClangIntrinsics
              // ===============================================

struct AtomicOperations_ALL_ALL_ClangIntrinsics
  #ifdef BSLS_PLATFORM_CPU_64_BIT
    : AtomicOperations_Default64<AtomicOperations_ALL_ALL_ClangIntrinsics>
  #else
    : AtomicOperations_Default32<AtomicOperations_ALL_ALL_ClangIntrinsics>
  #endif
{
    typedef Atomic_TypeTraits<AtomicOperations_ALL_ALL_ClangIntrinsics>
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

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

              // -----------------------------------------------
              // struct AtomicOperations_ALL_ALL_ClangIntrinsics
              // -----------------------------------------------

inline
void AtomicOperations_ALL_ALL_ClangIntrinsics::
    initInt(AtomicTypes::Int *atomicInt, int value)
{
    __c11_atomic_init(&atomicInt->d_value, value);
}

inline
int AtomicOperations_ALL_ALL_ClangIntrinsics::
    getInt(const AtomicTypes::Int *atomicInt)
{
    return __c11_atomic_load(
               const_cast<_Atomic(int) *>(&atomicInt->d_value),
               __ATOMIC_SEQ_CST);
}

inline
int AtomicOperations_ALL_ALL_ClangIntrinsics::
    getIntAcquire(const AtomicTypes::Int *atomicInt)
{
    return __c11_atomic_load(
               const_cast<_Atomic(int) *>(&atomicInt->d_value),
               __ATOMIC_ACQUIRE);
}

inline
int AtomicOperations_ALL_ALL_ClangIntrinsics::
    getIntRelaxed(const AtomicTypes::Int *atomicInt)
{
    return __c11_atomic_load(
               const_cast<_Atomic(int) *>(&atomicInt->d_value),
               __ATOMIC_RELAXED);
}

inline
void AtomicOperations_ALL_ALL_ClangIntrinsics::
    setInt(AtomicTypes::Int *atomicInt, int value)
{
    __c11_atomic_store(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
}

inline
void AtomicOperations_ALL_ALL_ClangIntrinsics::
    setIntRelease(AtomicTypes::Int *atomicInt, int value)
{
    __c11_atomic_store(&atomicInt->d_value, value, __ATOMIC_RELEASE);
}

inline
void AtomicOperations_ALL_ALL_ClangIntrinsics::
    setIntRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    __c11_atomic_store(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
int AtomicOperations_ALL_ALL_ClangIntrinsics::
    swapInt(AtomicTypes::Int *atomicInt, int swapValue)
{
    return
      __c11_atomic_exchange(&atomicInt->d_value, swapValue, __ATOMIC_SEQ_CST);
}

inline
int AtomicOperations_ALL_ALL_ClangIntrinsics::
    swapIntAcqRel(AtomicTypes::Int *atomicInt, int swapValue)
{
    return
      __c11_atomic_exchange(&atomicInt->d_value, swapValue, __ATOMIC_ACQ_REL);
}

inline
int AtomicOperations_ALL_ALL_ClangIntrinsics::
    testAndSwapInt(AtomicTypes::Int *atomicInt,
                   int               compareValue,
                   int               swapValue)
{
    __c11_atomic_compare_exchange_strong(&atomicInt->d_value,
                                         &compareValue,
                                         swapValue,
                                         __ATOMIC_SEQ_CST,
                                         __ATOMIC_SEQ_CST);
    return compareValue;
}

inline
int AtomicOperations_ALL_ALL_ClangIntrinsics::
    testAndSwapIntAcqRel(AtomicTypes::Int *atomicInt,
                         int               compareValue,
                         int               swapValue)
{
    __c11_atomic_compare_exchange_strong(&atomicInt->d_value,
                                         &compareValue,
                                         swapValue,
                                         __ATOMIC_ACQ_REL,
                                         __ATOMIC_ACQUIRE);
    return compareValue;
}

inline
int AtomicOperations_ALL_ALL_ClangIntrinsics::
    addIntNv(AtomicTypes::Int *atomicInt, int value)
{
    return __c11_atomic_fetch_add(&atomicInt->d_value,
                                  value,
                                  __ATOMIC_SEQ_CST)
         + value;
}

inline
int AtomicOperations_ALL_ALL_ClangIntrinsics::
    addIntNvAcqRel(AtomicTypes::Int *atomicInt, int value)
{
    return __c11_atomic_fetch_add(&atomicInt->d_value,
                                  value,
                                  __ATOMIC_ACQ_REL)
         + value;
}

inline
int AtomicOperations_ALL_ALL_ClangIntrinsics::
    addIntNvRelaxed(AtomicTypes::Int *atomicInt, int value)
{
    return __c11_atomic_fetch_add(&atomicInt->d_value,
                                  value,
                                  __ATOMIC_RELAXED)
         + value;
}

inline
void AtomicOperations_ALL_ALL_ClangIntrinsics::
    initInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __c11_atomic_init(&atomicInt->d_value, value);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_ClangIntrinsics::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    return __c11_atomic_load(
               const_cast<_Atomic(Types::Int64) *>(&atomicInt->d_value),
               __ATOMIC_SEQ_CST);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_ClangIntrinsics::
    getInt64Acquire(const AtomicTypes::Int64 *atomicInt)
{
    return __c11_atomic_load(
               const_cast<_Atomic(Types::Int64) *>(&atomicInt->d_value),
               __ATOMIC_ACQUIRE);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_ClangIntrinsics::
    getInt64Relaxed(const AtomicTypes::Int64 *atomicInt)
{
    return __c11_atomic_load(
               const_cast<_Atomic(Types::Int64) *>(&atomicInt->d_value),
               __ATOMIC_RELAXED);
}

inline
void AtomicOperations_ALL_ALL_ClangIntrinsics::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __c11_atomic_store(&atomicInt->d_value, value, __ATOMIC_SEQ_CST);
}

inline
void AtomicOperations_ALL_ALL_ClangIntrinsics::
    setInt64Release(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __c11_atomic_store(&atomicInt->d_value, value, __ATOMIC_RELEASE);
}

inline
void AtomicOperations_ALL_ALL_ClangIntrinsics::
    setInt64Relaxed(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    __c11_atomic_store(&atomicInt->d_value, value, __ATOMIC_RELAXED);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_ClangIntrinsics::
    swapInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 swapValue)
{
    return
      __c11_atomic_exchange(&atomicInt->d_value,
                            swapValue,
                            __ATOMIC_SEQ_CST);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_ClangIntrinsics::
    swapInt64AcqRel(AtomicTypes::Int64 *atomicInt, Types::Int64 swapValue)
{
    return
      __c11_atomic_exchange(&atomicInt->d_value,
                            swapValue,
                            __ATOMIC_ACQ_REL);
}

inline
Types::Int64 AtomicOperations_ALL_ALL_ClangIntrinsics::
    testAndSwapInt64(AtomicTypes::Int64 *atomicInt,
                     Types::Int64        compareValue,
                     Types::Int64        swapValue)
{
    __c11_atomic_compare_exchange_strong(&atomicInt->d_value,
                                         &compareValue,
                                         swapValue,
                                         __ATOMIC_SEQ_CST,
                                         __ATOMIC_SEQ_CST);
    return compareValue;
}

inline
Types::Int64 AtomicOperations_ALL_ALL_ClangIntrinsics::
    testAndSwapInt64AcqRel(AtomicTypes::Int64 *atomicInt,
                           Types::Int64        compareValue,
                           Types::Int64        swapValue)
{
    __c11_atomic_compare_exchange_strong(&atomicInt->d_value,
                                         &compareValue,
                                         swapValue,
                                         __ATOMIC_ACQ_REL,
                                         __ATOMIC_ACQUIRE);
    return compareValue;
}

inline
Types::Int64 AtomicOperations_ALL_ALL_ClangIntrinsics::
    addInt64Nv(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    return __c11_atomic_fetch_add(&atomicInt->d_value,
                                  value,
                                  __ATOMIC_SEQ_CST)
         + value;
}

inline
Types::Int64 AtomicOperations_ALL_ALL_ClangIntrinsics::
    addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    return __c11_atomic_fetch_add(&atomicInt->d_value,
                                  value,
                                  __ATOMIC_ACQ_REL)
         + value;
}

inline
Types::Int64 AtomicOperations_ALL_ALL_ClangIntrinsics::
    addInt64NvRelaxed(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    return __c11_atomic_fetch_add(&atomicInt->d_value,
                                  value,
                                  __ATOMIC_RELAXED)
         + value;
}

}  // close package namespace

}  // close enterprise namespace

#endif  //  __has_extension(c_atomic) || __has_extension(cxx_atomic)

#endif  // BSLS_PLATFORM_CMP_CLANG

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
