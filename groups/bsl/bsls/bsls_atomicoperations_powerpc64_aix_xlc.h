// bsls_atomicoperations_powerpc64_aix_xlc.h                          -*-C++-*-
#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC64_AIX_XLC
#define INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC64_AIX_XLC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide implementations of atomic operations for PowerPC/AIX-64bit.
//
//@CLASSES:
//  bsls::AtomicOperations_POWERPC64_AIX_XLC: atomics for PPC64/AIX.
//
//@DESCRIPTION: This component provides classes necessary to implement atomics
// on the AIX PowerPC platform in 64bit mode with the xlC compiler.  The
// classes are for private use only.  See 'bsls_atomicoperations' and
// 'bsls_atomic' for the public interface to atomics.

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS_POWERPC_AIX_XLC_DEFAULT
#include <bsls_atomicoperations_powerpc_aix_xlc_default.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#if defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CMP_IBM) \
    && defined(BSLS_PLATFORM_CPU_64_BIT)

#include <builtins.h>

namespace BloombergLP {

namespace bsls {

    // *** functions implemented in assembly ***

Types::Int64 AtomicOperations_Powerpc64_GetInt64(
                                  const volatile Types::Int64 *atomicInt);

Types::Int64 AtomicOperations_Powerpc64_GetInt64Acquire(
                                  const volatile Types::Int64 *atomicInt);

void AtomicOperations_Powerpc64_SetInt64(volatile Types::Int64 *atomicInt,
                                         Types::Int64 value);

void AtomicOperations_Powerpc64_SetInt64Release(
                                        volatile Types::Int64 *atomicInt,
                                        Types::Int64 value);

Types::Int64 AtomicOperations_Powerpc64_SwapInt64(
                                         volatile Types::Int64 *atomicInt,
                                         Types::Int64 value);

Types::Int64 AtomicOperations_Powerpc64_SwapInt64AcqRel(
                                         volatile Types::Int64 *atomicInt,
                                         Types::Int64 value);

Types::Int64 AtomicOperations_Powerpc64_testAndSwapInt64(
                                      volatile Types::Int64 *atomicInt,
                                      Types::Int64 compareValue,
                                      Types::Int64 swapValue);

Types::Int64 AtomicOperations_Powerpc64_testAndSwapInt64AcqRel(
                                      volatile Types::Int64 *atomicInt,
                                      Types::Int64 compareValue,
                                      Types::Int64 swapValue);

Types::Int64 AtomicOperations_Powerpc64_AddInt64(
                                         volatile Types::Int64 *atomicInt,
                                         Types::Int64 value);

Types::Int64 AtomicOperations_Powerpc64_AddInt64AcqRel(
                                         volatile Types::Int64 *atomicInt,
                                         Types::Int64 value);

struct AtomicOperations_POWERPC64_AIX_XLC;
typedef AtomicOperations_POWERPC64_AIX_XLC  AtomicOperations_Imp;

        // ============================================================
        // struct Atomic_TypeTraits<AtomicOperations_POWERPC64_AIX_XLC>
        // ============================================================

template <>
struct Atomic_TypeTraits<AtomicOperations_POWERPC64_AIX_XLC>
{
    struct Int
    {
        volatile int d_value __attribute__((__aligned__(sizeof(int))));
    };

    struct Int64
    {
        volatile Types::Int64 d_value
              __attribute__((__aligned__(sizeof(Types::Int64))));
    };

    struct Uint
    {
        volatile unsigned int d_value
              __attribute__((__aligned__(sizeof(unsigned int))));
    };

    struct Uint64
    {
        volatile Types::Uint64 d_value
              __attribute__((__aligned__(sizeof(Types::Uint64))));
    };

    struct Pointer
    {
        void * volatile d_value __attribute__((__aligned__(sizeof(void *))));
    };
};

                  // =========================================
                  // struct AtomicOperations_POWERPC64_AIX_XLC
                  // =========================================

struct AtomicOperations_POWERPC64_AIX_XLC
    : AtomicOperations_POWERPC_AIX_XLC_Default64<
                                       AtomicOperations_POWERPC64_AIX_XLC>
{
    typedef Atomic_TypeTraits<AtomicOperations_POWERPC64_AIX_XLC>
        AtomicTypes;

        //  *** atomic functions for Int64 ***

    static Types::Int64 getInt64(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Relaxed(const AtomicTypes::Int64 *atomicInt);

    static Types::Int64 getInt64Acquire(const AtomicTypes::Int64 *atomicInt);

    static void setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value);

    static void setInt64Relaxed(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static void setInt64Release(AtomicTypes::Int64 *atomicInt,
                                Types::Int64 value);

    static Types::Int64 swapInt64(AtomicTypes::Int64  *atomicInt,
                                  Types::Int64 swapValue);

    static Types::Int64 swapInt64AcqRel(AtomicTypes::Int64  *atomicInt,
                                        Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64(AtomicTypes::Int64   *atomicInt,
                                         Types::Int64 compareValue,
                                         Types::Int64 swapValue);

    static Types::Int64 testAndSwapInt64AcqRel(AtomicTypes::Int64   *atomicInt,
                                               Types::Int64 compareValue,
                                               Types::Int64 swapValue);

    static Types::Int64 addInt64Nv(AtomicTypes::Int64 *atomicInt,
                                   Types::Int64 value);

    static Types::Int64 addInt64NvRelaxed(AtomicTypes::Int64  *atomicInt,
                                          Types::Int64 value);

    static Types::Int64 addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                                         Types::Int64 value);
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // -----------------------------------------
                  // struct AtomicOperations_POWERPC64_AIX_XLC
                  // -----------------------------------------

inline
Types::Int64 AtomicOperations_POWERPC64_AIX_XLC::
    getInt64(const AtomicTypes::Int64 *atomicInt)
{
    return AtomicOperations_Powerpc64_GetInt64(&atomicInt->d_value);
}

inline
Types::Int64 AtomicOperations_POWERPC64_AIX_XLC::
    getInt64Relaxed(const AtomicTypes::Int64 *atomicInt)
{
    return atomicInt->d_value;
}

inline
Types::Int64 AtomicOperations_POWERPC64_AIX_XLC::
    getInt64Acquire(const AtomicTypes::Int64 *atomicInt)
{
    return AtomicOperations_Powerpc64_GetInt64Acquire(&atomicInt->d_value);
}

inline
void AtomicOperations_POWERPC64_AIX_XLC::
    setInt64(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    AtomicOperations_Powerpc64_SetInt64(&atomicInt->d_value, value);
}

inline
void AtomicOperations_POWERPC64_AIX_XLC::
    setInt64Relaxed(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    atomicInt->d_value = value;
}

inline
void AtomicOperations_POWERPC64_AIX_XLC::
    setInt64Release(AtomicTypes::Int64 *atomicInt, Types::Int64 value)
{
    AtomicOperations_Powerpc64_SetInt64Release(&atomicInt->d_value,
                                               value);
}

inline
Types::Int64 AtomicOperations_POWERPC64_AIX_XLC::
    swapInt64(AtomicTypes::Int64  *atomicInt,
              Types::Int64 swapValue)
{
    return AtomicOperations_Powerpc64_SwapInt64(&atomicInt->d_value,
                                                swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC64_AIX_XLC::
    swapInt64AcqRel(AtomicTypes::Int64  *atomicInt,
                    Types::Int64 swapValue)
{
    return AtomicOperations_Powerpc64_SwapInt64AcqRel(&atomicInt->d_value,
                                                      swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC64_AIX_XLC::
    testAndSwapInt64(AtomicTypes::Int64   *atomicInt,
                     Types::Int64 compareValue,
                     Types::Int64 swapValue)
{
    return AtomicOperations_Powerpc64_testAndSwapInt64(&atomicInt->d_value,
                                                       compareValue,
                                                       swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC64_AIX_XLC::
    testAndSwapInt64AcqRel(AtomicTypes::Int64   *atomicInt,
                           Types::Int64 compareValue,
                           Types::Int64 swapValue)
{
    return AtomicOperations_Powerpc64_testAndSwapInt64AcqRel(
                                                      &atomicInt->d_value,
                                                      compareValue,
                                                      swapValue);
}

inline
Types::Int64 AtomicOperations_POWERPC64_AIX_XLC::
    addInt64Nv(AtomicTypes::Int64 *atomicInt,
               Types::Int64 value)
{
    return AtomicOperations_Powerpc64_AddInt64(&atomicInt->d_value,
                                               value);
}

inline
Types::Int64 AtomicOperations_POWERPC64_AIX_XLC::
    addInt64NvRelaxed(AtomicTypes::Int64  *atomicInt,
                      Types::Int64 value)
{
    return __fetch_and_addlp(
                    reinterpret_cast<volatile long *>(&atomicInt->d_value),
                    static_cast<long>(value))
           + value;
}

inline
Types::Int64 AtomicOperations_POWERPC64_AIX_XLC::
    addInt64NvAcqRel(AtomicTypes::Int64 *atomicInt,
                     Types::Int64 value)
{
    return AtomicOperations_Powerpc64_AddInt64AcqRel(&atomicInt->d_value,
                                                     value);
}

}  // close package namespace

}  // close enterprise namespace

#endif

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
